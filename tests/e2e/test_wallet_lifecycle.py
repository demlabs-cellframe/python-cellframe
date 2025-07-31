"""
🔄 End-to-End Tests for Wallet Lifecycle
Complete wallet management scenarios from creation to transactions.
"""

import pytest
import tempfile
import os
from pathlib import Path
from decimal import Decimal
from unittest.mock import Mock, patch

# Import the classes under test
try:
    from CellFrame.core import CellframeNode
    from CellFrame.chain import Wallet, WalletType, TX, TxType
    from CellFrame.core.context import LibContext
    from CellFrame.core.exceptions import CellframeException
except ImportError as e:
    pytest.skip(f"CellFrame modules not available: {e}", allow_module_level=True)


class TestWalletLifecycleE2E:
    """End-to-End tests for complete wallet lifecycle."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.temp_dir = tempfile.mkdtemp()
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.app_name = "e2e_test"
        self.mock_context.get_data_dir.return_value = self.temp_dir
        self.mock_context.initialize.return_value = True
        
    def teardown_method(self):
        """Cleanup after each test."""
        # Clean up temp directory
        import shutil
        if os.path.exists(self.temp_dir):
            shutil.rmtree(self.temp_dir)
    
    @pytest.mark.e2e
    @patch('CellFrame.chain.create_wallet')
    @patch('CellFrame.chain.open_wallet')
    def test_complete_wallet_creation_and_loading(self, mock_open, mock_create):
        """Test complete wallet creation and loading scenario."""
        # Setup mocks
        mock_wallet = Mock(spec=Wallet)
        mock_wallet.name = "test_wallet"
        mock_wallet.type = WalletType.SIMPLE
        mock_wallet.get_address.return_value = "test_address_123"
        
        mock_create.return_value = mock_wallet
        mock_open.return_value = mock_wallet
        
        # Create node
        node = CellframeNode(context=self.mock_context)
        
        # Step 1: Create wallet
        wallet = node.chain.create_wallet("test_wallet", WalletType.SIMPLE)
        assert wallet is not None
        assert wallet.name == "test_wallet"
        
        # Step 2: Get wallet address
        address = wallet.get_address()
        assert address == "test_address_123"
        
        # Step 3: Save wallet (simulate)
        wallet_path = Path(self.temp_dir) / "test_wallet.dat"
        
        # Step 4: Load wallet from file
        loaded_wallet = node.chain.load_wallet(wallet_path)
        assert loaded_wallet is not None
        
        # Verify loaded wallet has same properties
        assert loaded_wallet.name == "test_wallet"
        
    @pytest.mark.e2e
    @patch('CellFrame.chain.create_wallet')
    @patch('CellFrame.chain.get_account_balance')
    def test_wallet_balance_check_scenario(self, mock_balance, mock_create):
        """Test wallet balance checking scenario."""
        # Setup
        mock_wallet = Mock(spec=Wallet)
        mock_wallet.name = "balance_test_wallet"
        mock_wallet.get_address.return_value = "balance_test_address"
        
        mock_create.return_value = mock_wallet
        mock_balance.return_value = Decimal("100.5")
        
        node = CellframeNode(context=self.mock_context)
        
        # Create wallet
        wallet = node.chain.create_wallet("balance_test_wallet")
        
        # Check balance
        balance = wallet.get_balance("CELL")
        assert balance == Decimal("100.5")
        
        # Check balance for different token
        mock_balance.return_value = Decimal("0.0")
        balance_empty = wallet.get_balance("OTHER_TOKEN")
        assert balance_empty == Decimal("0.0")
        
    @pytest.mark.e2e
    @patch('CellFrame.chain.create_wallet')
    @patch('CellFrame.chain.broadcast_tx')
    def test_complete_transaction_flow(self, mock_broadcast, mock_create):
        """Test complete transaction creation and broadcast flow."""
        # Setup wallets
        sender_wallet = Mock(spec=Wallet)
        sender_wallet.name = "sender"
        sender_wallet.get_address.return_value = "sender_address"
        sender_wallet.get_balance.return_value = Decimal("1000.0")
        
        recipient_wallet = Mock(spec=Wallet)
        recipient_wallet.name = "recipient"
        recipient_wallet.get_address.return_value = "recipient_address"
        
        mock_create.side_effect = [sender_wallet, recipient_wallet]
        mock_broadcast.return_value = True
        
        node = CellframeNode(context=self.mock_context)
        
        # Step 1: Create sender and recipient wallets
        sender = node.chain.create_wallet("sender")
        recipient = node.chain.create_wallet("recipient")
        
        # Step 2: Check sender has sufficient balance
        sender_balance = sender.get_balance("CELL")
        assert sender_balance >= Decimal("100.0")
        
        # Step 3: Create transaction
        tx = node.chain.create_transaction(TxType.TRANSFER)
        
        # Step 4: Add transaction details (mocked)
        with patch.object(tx, 'add_input') as mock_add_input, \
             patch.object(tx, 'add_output') as mock_add_output, \
             patch.object(tx, 'sign') as mock_sign:
            
            mock_add_input.return_value = True
            mock_add_output.return_value = True  
            mock_sign.return_value = True
            
            # Add input from sender
            tx.add_input(sender.get_address(), Decimal("100.0"), "CELL")
            
            # Add output to recipient
            tx.add_output(recipient.get_address(), Decimal("95.0"), "CELL")
            
            # Add fee output
            tx.add_output("fee_address", Decimal("5.0"), "CELL")
            
            # Sign transaction
            tx.sign(sender)
            
            # Step 5: Broadcast transaction
            result = node.chain.broadcast_transaction(tx)
            assert result is True
            
            mock_broadcast.assert_called_once_with(tx)
    
    @pytest.mark.e2e
    @patch('CellFrame.chain.get_all_wallets')
    def test_wallet_management_scenario(self, mock_get_all):
        """Test wallet management operations."""
        # Setup multiple wallets
        wallets = []
        for i in range(3):
            wallet = Mock(spec=Wallet)
            wallet.name = f"wallet_{i}"
            wallet.get_address.return_value = f"address_{i}"
            wallets.append(wallet)
        
        mock_get_all.return_value = wallets
        
        node = CellframeNode(context=self.mock_context)
        
        # Get all wallets
        all_wallets = node.chain.get_all_wallets()
        assert len(all_wallets) == 3
        
        # Verify wallet names
        wallet_names = [w.name for w in all_wallets]
        assert "wallet_0" in wallet_names
        assert "wallet_1" in wallet_names
        assert "wallet_2" in wallet_names
        
    @pytest.mark.e2e
    @patch('CellFrame.chain.create_wallet')
    def test_wallet_error_recovery_scenario(self, mock_create):
        """Test wallet error recovery scenarios."""
        # First call fails, second succeeds
        mock_wallet = Mock(spec=Wallet)
        mock_wallet.name = "recovery_wallet"
        
        mock_create.side_effect = [CellframeException("Creation failed"), mock_wallet]
        
        node = CellframeNode(context=self.mock_context)
        
        # First attempt should fail
        with pytest.raises(CellframeException):
            node.chain.create_wallet("recovery_wallet")
        
        # Second attempt should succeed
        wallet = node.chain.create_wallet("recovery_wallet")
        assert wallet is not None
        assert wallet.name == "recovery_wallet"


class TestMultiWalletScenarios:
    """E2E tests for multi-wallet scenarios."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.app_name = "multi_wallet_test"
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.e2e
    @patch('CellFrame.chain.create_wallet')
    @patch('CellFrame.chain.broadcast_tx')
    def test_multi_wallet_transfer_scenario(self, mock_broadcast, mock_create):
        """Test transfer between multiple wallets."""
        # Create multiple wallets
        wallets = {}
        for name in ['alice', 'bob', 'charlie']:
            wallet = Mock(spec=Wallet)
            wallet.name = name
            wallet.get_address.return_value = f"{name}_address"
            wallet.get_balance.return_value = Decimal("1000.0")
            wallets[name] = wallet
        
        mock_create.side_effect = lambda name, *args: wallets[name]
        mock_broadcast.return_value = True
        
        node = CellframeNode(context=self.mock_context)
        
        # Create wallets
        alice = node.chain.create_wallet("alice")
        bob = node.chain.create_wallet("bob")
        charlie = node.chain.create_wallet("charlie")
        
        # Scenario: Alice sends to Bob and Charlie
        tx = node.chain.create_transaction(TxType.TRANSFER)
        
        with patch.object(tx, 'add_input') as mock_add_input, \
             patch.object(tx, 'add_output') as mock_add_output, \
             patch.object(tx, 'sign') as mock_sign:
            
            mock_add_input.return_value = True
            mock_add_output.return_value = True
            mock_sign.return_value = True
            
            # Alice sends 500 CELL total
            tx.add_input(alice.get_address(), Decimal("500.0"), "CELL")
            
            # 200 to Bob, 290 to Charlie, 10 fee
            tx.add_output(bob.get_address(), Decimal("200.0"), "CELL")
            tx.add_output(charlie.get_address(), Decimal("290.0"), "CELL")
            tx.add_output("fee_address", Decimal("10.0"), "CELL")
            
            tx.sign(alice)
            
            result = node.chain.broadcast_transaction(tx)
            assert result is True
            
    @pytest.mark.e2e 
    @patch('CellFrame.chain.create_wallet')
    def test_wallet_type_scenarios(self, mock_create):
        """Test different wallet types."""
        wallet_types = [WalletType.SIMPLE, WalletType.HD]
        created_wallets = []
        
        for i, wallet_type in enumerate(wallet_types):
            wallet = Mock(spec=Wallet)
            wallet.name = f"wallet_{wallet_type.name.lower()}"
            wallet.type = wallet_type
            created_wallets.append(wallet)
        
        mock_create.side_effect = created_wallets
        
        node = CellframeNode(context=self.mock_context)
        
        # Create different types of wallets
        simple_wallet = node.chain.create_wallet("simple_wallet", WalletType.SIMPLE)
        hd_wallet = node.chain.create_wallet("hd_wallet", WalletType.HD)
        
        assert simple_wallet.type == WalletType.SIMPLE
        assert hd_wallet.type == WalletType.HD


class TestNetworkScenarios:
    """E2E tests involving network operations."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.app_name = "network_test"
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.e2e
    @pytest.mark.slow
    @patch('CellFrame.chain.create_wallet')
    @patch('CellFrame.chain.get_tx_by_hash')
    def test_transaction_confirmation_scenario(self, mock_get_tx, mock_create):
        """Test transaction confirmation waiting scenario."""
        import time
        
        # Setup
        mock_wallet = Mock(spec=Wallet)
        mock_wallet.name = "confirmation_wallet"
        mock_create.return_value = mock_wallet
        
        # Mock transaction states
        pending_tx = Mock(spec=TX)
        pending_tx.status = "pending"
        pending_tx.hash = "test_tx_hash"
        
        confirmed_tx = Mock(spec=TX)
        confirmed_tx.status = "confirmed"
        confirmed_tx.hash = "test_tx_hash"
        
        # First calls return pending, later calls return confirmed
        mock_get_tx.side_effect = [pending_tx, pending_tx, confirmed_tx]
        
        node = CellframeNode(context=self.mock_context)
        wallet = node.chain.create_wallet("confirmation_wallet")
        
        # Simulate waiting for confirmation
        max_attempts = 3
        for attempt in range(max_attempts):
            tx = node.chain.get_transaction("test_tx_hash")
            
            if tx.status == "confirmed":
                break
                
            if attempt < max_attempts - 1:
                time.sleep(0.1)  # Short sleep for test
        
        # Should eventually be confirmed
        assert tx.status == "confirmed"
        
    @pytest.mark.e2e
    @patch('CellFrame.chain.create_wallet')
    def test_network_error_handling(self, mock_create):
        """Test handling of network-related errors."""
        # Simulate network error
        mock_create.side_effect = ConnectionError("Network unavailable")
        
        node = CellframeNode(context=self.mock_context)
        
        # Should handle network errors gracefully
        with pytest.raises(ConnectionError):
            node.chain.create_wallet("network_test_wallet")


class TestPerformanceScenarios:
    """E2E tests for performance-critical scenarios."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.app_name = "performance_test"
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.e2e
    @pytest.mark.performance
    @patch('CellFrame.chain.get_all_wallets')
    def test_large_wallet_list_performance(self, mock_get_all):
        """Test performance with large number of wallets."""
        # Create many mock wallets
        large_wallet_count = 1000
        wallets = []
        
        for i in range(large_wallet_count):
            wallet = Mock(spec=Wallet)
            wallet.name = f"wallet_{i:04d}"
            wallet.get_address.return_value = f"address_{i:04d}"
            wallets.append(wallet)
        
        mock_get_all.return_value = wallets
        
        node = CellframeNode(context=self.mock_context)
        
        # Measure time to get all wallets
        import time
        start_time = time.time()
        
        all_wallets = node.chain.get_all_wallets()
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should handle large lists efficiently (under 1 second for mocked data)
        assert elapsed < 1.0
        assert len(all_wallets) == large_wallet_count
        
    @pytest.mark.e2e
    @pytest.mark.performance
    @patch('CellFrame.chain.create_wallet')
    def test_rapid_wallet_creation(self, mock_create):
        """Test rapid wallet creation performance."""
        # Setup mock
        def create_wallet_mock(name, *args):
            wallet = Mock(spec=Wallet)
            wallet.name = name
            return wallet
        
        mock_create.side_effect = create_wallet_mock
        
        node = CellframeNode(context=self.mock_context)
        
        # Create many wallets rapidly
        wallet_count = 100
        import time
        start_time = time.time()
        
        wallets = []
        for i in range(wallet_count):
            wallet = node.chain.create_wallet(f"rapid_wallet_{i}")
            wallets.append(wallet)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should create wallets efficiently
        assert len(wallets) == wallet_count
        assert elapsed < 5.0  # Should create 100 wallets in under 5 seconds


if __name__ == "__main__":
    pytest.main([__file__, "-v"])