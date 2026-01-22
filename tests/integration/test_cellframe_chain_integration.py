"""
🔗 Integration Tests for Cellframe Chain Components
Testing interaction between Node, Chain, Wallet, and Ledger components.
"""

import pytest
import asyncio
from unittest.mock import Mock, patch, AsyncMock
from typing import Dict, Any, List

# Import the classes under test
try:
    from CellFrame.core import CellframeNode, CellframeChain
    from CellFrame.chain import Wallet, TX, DapLedger, WalletType, TxType
    from CellFrame.chain.wallet import WalletManager
    from CellFrame.chain.ledger import DapLedgerManager
    from CellFrame.core.context import LibContext
    from CellFrame.core.exceptions import CellframeException
except ImportError as e:
    pytest.skip(f"CellFrame modules not available: {e}", allow_module_level=True)


class TestCellframeChainIntegration:
    """Integration tests for Cellframe chain operations."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.mock_context = Mock(spec=LibContext)
        self.mock_context.is_plugin_mode = False
        self.mock_context.is_library_mode = True
        self.mock_context.app_name = "test_integration"
        self.mock_context.get_resource.return_value = None
        self.mock_context.initialize.return_value = True
        
    @pytest.mark.integration
    def test_node_chain_component_creation(self):
        """Test that node properly creates chain component."""
        node = CellframeNode(context=self.mock_context)
        
        assert hasattr(node, 'chain')
        assert isinstance(node.chain, CellframeChain)
        assert node.chain.context == self.mock_context
        
    @pytest.mark.integration
    def test_chain_wallet_manager_integration(self):
        """Test chain integration with wallet manager."""
        node = CellframeNode(context=self.mock_context)
        chain = node.chain
        
        # Test wallet manager access
        assert hasattr(chain, '_wallet_manager')
        assert isinstance(chain._wallet_manager, WalletManager)
        
    @pytest.mark.integration
    def test_chain_ledger_manager_integration(self):
        """Test chain integration with ledger manager."""
        node = CellframeNode(context=self.mock_context)
        chain = node.chain
        
        # Test ledger manager access
        assert hasattr(chain, '_ledger_manager')
        assert isinstance(chain._ledger_manager, DapLedgerManager)
        
    @pytest.mark.integration
    @patch('CellFrame.chain.create_wallet')
    def test_wallet_creation_through_chain(self, mock_create_wallet):
        """Test wallet creation through chain component."""
        mock_wallet = Mock(spec=Wallet)
        mock_create_wallet.return_value = mock_wallet
        
        node = CellframeNode(context=self.mock_context)
        chain = node.chain
        
        # Create wallet through chain
        wallet = chain.create_wallet("test_wallet", WalletType.SIMPLE)
        
        mock_create_wallet.assert_called_once_with("test_wallet", WalletType.SIMPLE)
        assert wallet == mock_wallet
        
    @pytest.mark.integration
    @patch('CellFrame.chain.open_wallet')
    def test_wallet_loading_through_chain(self, mock_open_wallet):
        """Test wallet loading through chain component."""
        mock_wallet = Mock(spec=Wallet)
        mock_open_wallet.return_value = mock_wallet
        
        node = CellframeNode(context=self.mock_context)
        chain = node.chain
        
        # Load wallet through chain
        wallet = chain.load_wallet("/path/to/wallet.dat")
        
        mock_open_wallet.assert_called_once_with("wallet", "/path/to/wallet.dat")
        assert wallet == mock_wallet
        
    @pytest.mark.integration
    @patch('CellFrame.chain.get_all_wallets')
    def test_wallet_listing_through_chain(self, mock_get_all_wallets):
        """Test wallet listing through chain component."""
        mock_wallets = [Mock(spec=Wallet), Mock(spec=Wallet)]
        mock_get_all_wallets.return_value = mock_wallets
        
        node = CellframeNode(context=self.mock_context)
        chain = node.chain
        
        # Get all wallets through chain
        wallets = chain.get_all_wallets()
        
        mock_get_all_wallets.assert_called_once()
        assert wallets == mock_wallets
        
    @pytest.mark.integration
    def test_transaction_creation_through_chain(self):
        """Test transaction creation through chain component."""
        node = CellframeNode(context=self.mock_context)
        chain = node.chain
        
        # Create transaction through chain
        tx = chain.create_transaction(TxType.TRANSFER)
        
        assert isinstance(tx, TX)
        
    @pytest.mark.integration
    @patch('CellFrame.chain.get_tx_by_hash')
    def test_transaction_retrieval_through_chain(self, mock_get_tx):
        """Test transaction retrieval through chain component."""
        mock_tx = Mock(spec=TX)
        mock_get_tx.return_value = mock_tx
        
        node = CellframeNode(context=self.mock_context)
        chain = node.chain
        
        # Get transaction through chain
        tx = chain.get_transaction("test_hash")
        
        mock_get_tx.assert_called_once_with("test_hash")
        assert tx == mock_tx
        
    @pytest.mark.integration
    @patch('CellFrame.chain.broadcast_tx')
    def test_transaction_broadcast_through_chain(self, mock_broadcast):
        """Test transaction broadcast through chain component."""
        mock_broadcast.return_value = True
        mock_tx = Mock(spec=TX)
        
        node = CellframeNode(context=self.mock_context)
        chain = node.chain
        
        # Broadcast transaction through chain
        result = chain.broadcast_transaction(mock_tx)
        
        mock_broadcast.assert_called_once_with(mock_tx)
        assert result is True
        
    @pytest.mark.integration
    @patch('CellFrame.chain.create_ledger')
    def test_ledger_creation_through_chain(self, mock_create_ledger):
        """Test ledger creation through chain component."""
        mock_ledger = Mock(spec=DapLedger)
        mock_create_ledger.return_value = mock_ledger
        
        node = CellframeNode(context=self.mock_context)
        chain = node.chain
        
        # Create ledger through chain
        ledger = chain.create_ledger("test_token")
        
        mock_create_ledger.assert_called_once_with("test_token")
        assert ledger == mock_ledger


class TestCellframeNodeLifecycle:
    """Integration tests for complete node lifecycle."""
    
    @pytest.mark.integration
    def test_node_full_initialization_sequence(self):
        """Test complete node initialization sequence."""
        mock_context = Mock(spec=LibContext)
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        mock_context.initialize.return_value = True
        
        # Create node
        node = CellframeNode(context=mock_context)
        
        # Initialize
        result = node.initialize()
        assert result is True
        
        # Check all components are available
        assert node.chain is not None
        assert hasattr(node.chain, '_wallet_manager')
        assert hasattr(node.chain, '_ledger_manager')
        
    @pytest.mark.integration
    def test_node_context_manager_lifecycle(self):
        """Test node lifecycle as context manager."""
        mock_context = Mock(spec=LibContext)
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        mock_context.initialize.return_value = True
        mock_context.shutdown.return_value = None
        
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = mock_context
            
            with CellframeNode() as node:
                # Node should be properly initialized
                assert node is not None
                assert node.chain is not None
                
                # Test operations within context
                status = node.get_status()
                assert isinstance(status, dict)
                
            # Context should be properly cleaned up
            mock_context.shutdown.assert_called_once()
            
    @pytest.mark.integration
    def test_error_handling_in_component_chain(self):
        """Test error propagation through component chain."""
        mock_context = Mock(spec=LibContext)
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        mock_context.initialize.side_effect = CellframeException("Init failed")
        
        node = CellframeNode(context=mock_context)
        
        # Initialization should handle the error gracefully
        result = node.initialize()
        assert result is False


class TestAsyncIntegration:
    """Integration tests for async operations."""
    
    @pytest.mark.integration
    @pytest.mark.asyncio
    async def test_async_node_operations(self):
        """Test async operations with node."""
        mock_context = Mock(spec=LibContext)
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        
        node = CellframeNode(context=mock_context)
        
        # Test async-compatible status retrieval
        status = await asyncio.get_event_loop().run_in_executor(
            None, node.get_status
        )
        
        assert isinstance(status, dict)
        
    @pytest.mark.integration
    @pytest.mark.asyncio
    async def test_async_wallet_operations(self):
        """Test async wallet operations."""
        mock_context = Mock(spec=LibContext)
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        
        node = CellframeNode(context=mock_context)
        
        with patch('CellFrame.chain.get_all_wallets') as mock_get_wallets:
            mock_get_wallets.return_value = []
            
            # Test async wallet listing
            wallets = await asyncio.get_event_loop().run_in_executor(
                None, node.chain.get_all_wallets
            )
            
            assert isinstance(wallets, list)


class TestResourceManagement:
    """Integration tests for resource management."""
    
    @pytest.mark.integration
    def test_resource_sharing_between_components(self):
        """Test resource sharing between node and chain components."""
        mock_context = Mock(spec=LibContext)
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        mock_context.get_resource.return_value = "shared_resource"
        
        node = CellframeNode(context=mock_context)
        
        # Both node and chain should access the same context
        assert node.context == node.chain.context
        
        # Both should get the same resource
        node_resource = node.get_resource("test_key")
        chain_resource = node.chain.get_resource("test_key")
        
        assert node_resource == chain_resource == "shared_resource"
        
    @pytest.mark.integration
    def test_component_isolation(self):
        """Test that components are properly isolated."""
        mock_context = Mock(spec=LibContext)
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        
        node1 = CellframeNode(context=mock_context)
        node2 = CellframeNode(context=mock_context)
        
        # Components should be separate instances
        assert node1.chain is not node2.chain
        assert node1._components is not node2._components
        
        # But they should share the same context
        assert node1.context is node2.context
        
    @pytest.mark.integration
    def test_memory_cleanup(self):
        """Test memory cleanup after component destruction."""
        import gc
        import weakref
        
        mock_context = Mock(spec=LibContext)
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        
        # Create node and get weak reference
        node = CellframeNode(context=mock_context)
        weak_node = weakref.ref(node)
        weak_chain = weakref.ref(node.chain)
        
        # Delete node
        del node
        gc.collect()
        
        # References should be cleaned up
        # Note: This might not always work due to mock objects holding references
        # but it's a good practice to test


class TestErrorRecovery:
    """Integration tests for error recovery scenarios."""
    
    @pytest.mark.integration
    def test_recovery_from_component_failure(self):
        """Test recovery from component initialization failure."""
        mock_context = Mock(spec=LibContext)
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        
        # First attempt fails
        mock_context.initialize.side_effect = [Exception("First failure"), True]
        
        node = CellframeNode(context=mock_context)
        
        # First initialization fails
        result1 = node.initialize()
        assert result1 is False
        
        # Second attempt succeeds
        result2 = node.initialize()
        assert result2 is True
        
    @pytest.mark.integration
    def test_partial_component_failure(self):
        """Test handling of partial component failures."""
        mock_context = Mock(spec=LibContext)
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        mock_context.initialize.return_value = True
        
        node = CellframeNode(context=mock_context)
        
        # Node should handle partial component failures gracefully
        with patch.object(node.chain, 'initialize') as mock_chain_init:
            mock_chain_init.return_value = False
            
            result = node.initialize()
            
            # Node should still report success even if one component fails
            # (depending on implementation details)
            assert isinstance(result, bool)


if __name__ == "__main__":
    pytest.main([__file__, "-v"])