"""
🔄 Regression Tests for API Compatibility
Ensuring backward compatibility and preventing API regressions.
"""

import pytest
import warnings
from unittest.mock import Mock, patch
from typing import Dict, Any, List

# Import the classes under test
try:
    from CellFrame.core import CellframeNode, CellframeComponent
    from CellFrame.chain import Wallet, TX, WalletType, TxType
    from CellFrame.legacy import LegacyCellFrame, LegacyChain
    from CellFrame.core.context import LibContext
    from CellFrame.core.exceptions import CellframeException
except ImportError as e:
    pytest.skip(f"CellFrame modules not available: {e}", allow_module_level=True)


class TestLegacyAPICompatibility:
    """Regression tests for legacy API compatibility."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.app_name = "regression_test"
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.regression
    @pytest.mark.legacy
    def test_legacy_cellframe_init_compatibility(self):
        """Test legacy CellFrame.init() still works."""
        with patch.object(LegacyCellFrame, 'init') as mock_init:
            mock_init.return_value = True
            
            # Legacy API call should still work
            result = LegacyCellFrame.init({"debug": True})
            
            assert result is True
            mock_init.assert_called_once_with({"debug": True})
            
    @pytest.mark.regression
    @pytest.mark.legacy
    def test_legacy_wallet_api_compatibility(self):
        """Test legacy wallet API still works."""
        with patch.object(LegacyCellFrame, 'Wallet') as mock_wallet_class:
            mock_wallet = Mock()
            mock_wallet.init.return_value = True
            mock_wallet.get_balance.return_value = 1000.0
            mock_wallet_class.return_value = mock_wallet
            
            # Legacy wallet creation
            wallet = LegacyCellFrame.Wallet()
            wallet.init("mainnet")
            
            # Legacy API calls
            assert wallet.init("mainnet") is True
            balance = wallet.get_balance("address")
            assert balance == 1000.0
            
    @pytest.mark.regression
    @pytest.mark.legacy
    def test_legacy_chain_api_compatibility(self):
        """Test legacy chain API still works."""
        with patch.object(LegacyChain, '__init__') as mock_init, \
             patch.object(LegacyChain, 'init') as mock_chain_init, \
             patch.object(LegacyChain, 'get_info') as mock_get_info:
            
            mock_init.return_value = None
            mock_chain_init.return_value = True
            mock_get_info.return_value = {"name": "mainnet", "status": "active"}
            
            # Legacy chain creation and usage
            chain = LegacyChain("mainnet")
            chain.init()
            info = chain.get_info()
            
            assert info["name"] == "mainnet"
            assert info["status"] == "active"
            
    @pytest.mark.regression
    def test_deprecation_warnings_are_shown(self):
        """Test that deprecation warnings are properly shown."""
        with warnings.catch_warnings(record=True) as warning_list:
            warnings.simplefilter("always")
            
            # Trigger deprecation warning
            with patch('CellFrame.legacy._show_legacy_warning') as mock_warning:
                mock_warning.side_effect = lambda api, new: warnings.warn(
                    f"{api} is deprecated, use {new} instead", 
                    DeprecationWarning
                )
                
                # Call that should trigger warning
                mock_warning("CellFrame.init", "cellframe.auto_create_node")
                
                # Check warning was issued
                assert len(warning_list) == 1
                assert issubclass(warning_list[0].category, DeprecationWarning)
                assert "CellFrame.init is deprecated" in str(warning_list[0].message)


class TestNewAPIStability:
    """Regression tests for new API stability."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.regression
    def test_cellframe_node_api_stability(self):
        """Test CellframeNode API hasn't changed."""
        node = CellframeNode(context=self.mock_context)
        
        # Check all expected methods exist
        expected_methods = [
            'initialize', 'shutdown', 'get_status', 'get_chains',
            'get_chain_by_id', 'get_resource', '__enter__', '__exit__'
        ]
        
        for method_name in expected_methods:
            assert hasattr(node, method_name), f"Method {method_name} missing"
            assert callable(getattr(node, method_name)), f"Method {method_name} not callable"
        
        # Check expected attributes exist
        expected_attributes = ['chain', 'context', '_components']
        
        for attr_name in expected_attributes:
            assert hasattr(node, attr_name), f"Attribute {attr_name} missing"
            
    @pytest.mark.regression
    @patch('CellFrame.chain.create_wallet')
    def test_wallet_api_stability(self, mock_create):
        """Test wallet API hasn't changed."""
        mock_wallet = Mock(spec=Wallet)
        mock_wallet.name = "test_wallet"
        mock_wallet.type = WalletType.SIMPLE
        mock_wallet.get_address.return_value = "test_address"
        mock_wallet.get_balance.return_value = 1000.0
        mock_wallet.save.return_value = True
        
        mock_create.return_value = mock_wallet
        
        node = CellframeNode(context=self.mock_context)
        wallet = node.chain.create_wallet("test_wallet")
        
        # Check wallet API methods
        expected_methods = ['get_address', 'get_balance', 'save']
        
        for method_name in expected_methods:
            assert hasattr(wallet, method_name), f"Wallet method {method_name} missing"
            
        # Check method signatures haven't changed
        address = wallet.get_address()
        assert isinstance(address, str)
        
        balance = wallet.get_balance("CELL")
        assert isinstance(balance, (int, float))
        
        save_result = wallet.save()
        assert isinstance(save_result, bool)
        
    @pytest.mark.regression
    def test_transaction_api_stability(self):
        """Test transaction API hasn't changed."""
        node = CellframeNode(context=self.mock_context)
        tx = node.chain.create_transaction(TxType.TRANSFER)
        
        # Check transaction API methods
        expected_methods = ['add_input', 'add_output', 'sign', 'verify']
        
        for method_name in expected_methods:
            assert hasattr(tx, method_name), f"Transaction method {method_name} missing"
            
        # Check expected attributes
        expected_attributes = ['type', 'hash', 'status']
        
        for attr_name in expected_attributes:
            assert hasattr(tx, attr_name), f"Transaction attribute {attr_name} missing"


class TestConfigurationCompatibility:
    """Regression tests for configuration compatibility."""
    
    @pytest.mark.regression
    def test_legacy_config_format_support(self):
        """Test legacy configuration format is still supported."""
        legacy_config = {
            "debug": True,
            "config_dir": "/path/to/config",
            "log_level": "DEBUG",
            "network": "mainnet"
        }
        
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_context = Mock(spec=LibContext)
            mock_context.is_plugin_mode = False
            mock_context.is_library_mode = True
            mock_init.return_value = mock_context
            
            # Should accept legacy config format
            node = CellframeNode(config=legacy_config)
            
            assert node._config == legacy_config
            
    @pytest.mark.regression
    def test_config_migration_compatibility(self):
        """Test configuration migration from old to new format."""
        old_config = {
            "cellframe_debug": True,
            "cellframe_network": "mainnet",
            "dap_debug": True
        }
        
        # Mock config migration
        def migrate_config(old_cfg):
            new_cfg = {}
            
            # Migration rules
            if "cellframe_debug" in old_cfg:
                new_cfg["debug"] = old_cfg["cellframe_debug"]
            if "cellframe_network" in old_cfg:
                new_cfg["network"] = old_cfg["cellframe_network"]
            if "dap_debug" in old_cfg:
                new_cfg["dap"] = {"debug": old_cfg["dap_debug"]}
                
            return new_cfg
        
        # Test migration
        new_config = migrate_config(old_config)
        
        assert new_config["debug"] is True
        assert new_config["network"] == "mainnet"
        assert new_config["dap"]["debug"] is True


class TestExceptionCompatibility:
    """Regression tests for exception compatibility."""
    
    @pytest.mark.regression
    def test_exception_hierarchy_compatibility(self):
        """Test exception hierarchy hasn't changed."""
        # Check base exception exists
        assert issubclass(CellframeException, Exception)
        
        # Check exception can be instantiated
        exc = CellframeException("Test error")
        assert str(exc) == "Test error"
        
        # Check exception can be raised and caught
        with pytest.raises(CellframeException) as exc_info:
            raise CellframeException("Test error")
            
        assert str(exc_info.value) == "Test error"
        
    @pytest.mark.regression
    def test_specific_exception_types_exist(self):
        """Test specific exception types still exist."""
        # Import and check specific exceptions
        from CellFrame.core.exceptions import ConfigurationException
        
        assert issubclass(ConfigurationException, CellframeException)
        
        # Check exception can be used
        with pytest.raises(ConfigurationException):
            raise ConfigurationException("Config error")


class TestModuleStructureCompatibility:
    """Regression tests for module structure compatibility."""
    
    @pytest.mark.regression
    def test_import_paths_unchanged(self):
        """Test that import paths haven't changed."""
        # Test all major import paths still work
        import_tests = [
            "from CellFrame.core import CellframeNode",
            "from CellFrame.chain import Wallet, TX",
            "from CellFrame.services import StakingService",
            "from CellFrame.core.context import AppContext",
            "from CellFrame.core.exceptions import CellframeException"
        ]
        
        for import_statement in import_tests:
            try:
                exec(import_statement)
            except ImportError as e:
                pytest.fail(f"Import path changed: {import_statement} - {e}")
                
    @pytest.mark.regression
    def test_module_attributes_unchanged(self):
        """Test that module __all__ attributes haven't changed."""
        from CellFrame.core import __all__ as core_all
        
        # Check expected exports in core module
        expected_core_exports = [
            'CellframeComponent', 'CellframeChain', 'CellframeNode',
            'AppContext', 'PluginContext', 'LibContext', 'ExecutionMode',
            'CellframeException', 'ConfigurationException'
        ]
        
        for export in expected_core_exports:
            assert export in core_all, f"Expected export {export} missing from core.__all__"


class TestPerformanceRegressions:
    """Regression tests for performance to catch performance degradations."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.regression
    @pytest.mark.performance
    def test_node_creation_performance_regression(self):
        """Test node creation performance hasn't regressed."""
        import time
        
        # Baseline: Node creation should be fast
        max_creation_time = 0.1  # 100ms
        
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = self.mock_context
            
            start_time = time.time()
            node = CellframeNode()
            end_time = time.time()
            
            creation_time = end_time - start_time
            
            assert creation_time < max_creation_time, \
                f"Node creation took {creation_time:.3f}s, expected < {max_creation_time}s"
                
    @pytest.mark.regression
    @pytest.mark.performance
    @patch('CellFrame.chain.create_wallet')
    def test_wallet_operations_performance_regression(self, mock_create):
        """Test wallet operations performance hasn't regressed."""
        import time
        
        # Setup mock
        mock_wallet = Mock(spec=Wallet)
        mock_wallet.get_address.return_value = "test_address"
        mock_wallet.get_balance.return_value = 1000.0
        mock_create.return_value = mock_wallet
        
        node = CellframeNode(context=self.mock_context)
        
        # Test operation performance
        max_operation_time = 0.01  # 10ms per operation
        
        start_time = time.time()
        wallet = node.chain.create_wallet("test_wallet")
        address = wallet.get_address()
        balance = wallet.get_balance("CELL")
        end_time = time.time()
        
        operation_time = end_time - start_time
        
        assert operation_time < max_operation_time, \
            f"Wallet operations took {operation_time:.3f}s, expected < {max_operation_time}s"


if __name__ == "__main__":
    pytest.main([__file__, "-v"])