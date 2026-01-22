"""
📝 Examples of Using Test Fixtures
Examples showing how to refactor existing tests to use centralized fixtures.
"""

import pytest
from decimal import Decimal
from unittest.mock import patch

# Example of how to refactor tests to use fixtures from fixtures/conftest.py


class TestWalletWithFixtures:
    """Example: Refactored test using centralized fixtures."""
    
    def test_wallet_creation_with_fixture(self, mock_lib_context, mock_wallet):
        """Example using context and wallet fixtures."""
        from CellFrame.core import CellframeNode
        
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = mock_lib_context
            
            with patch('CellFrame.chain.create_wallet') as mock_create:
                mock_create.return_value = mock_wallet
                
                node = CellframeNode()
                wallet = node.chain.create_wallet("test_wallet")
                
                assert wallet.name == "test_wallet"
                assert wallet.get_address() == "test_address_12345"
                assert wallet.get_balance() == Decimal("1000.0")
    
    def test_multiple_wallets_with_factory(self, mock_lib_context):
        """Example using mock factory for multiple wallets."""
        from tests.fixtures.mock_factories import MockWalletFactory
        from CellFrame.core import CellframeNode
        
        # Create multiple wallets using factory
        wallets = MockWalletFactory.create_wallet_list(count=3, prefix="factory")
        
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = mock_lib_context
            
            with patch('CellFrame.chain.get_all_wallets') as mock_get_all:
                mock_get_all.return_value = wallets
                
                node = CellframeNode()
                all_wallets = node.chain.get_all_wallets()
                
                assert len(all_wallets) == 3
                assert all(w.name.startswith("factory") for w in all_wallets)
    
    def test_transaction_with_fixtures(self, mock_lib_context, mock_transaction):
        """Example using transaction fixture."""
        from CellFrame.core import CellframeNode
        
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = mock_lib_context
            
            node = CellframeNode()
            
            # Mock transaction creation
            with patch.object(node.chain, 'create_transaction') as mock_create_tx:
                mock_create_tx.return_value = mock_transaction
                
                tx = node.chain.create_transaction("transfer")
                
                assert tx.hash == "tx_hash_abcdef123456"
                assert tx.status == "pending"
                assert tx.amount == Decimal("100.0")
    
    def test_performance_with_timer(self, mock_lib_context, performance_timer):
        """Example using performance timer fixture."""
        from CellFrame.core import CellframeNode
        
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = mock_lib_context
            
            performance_timer.start()
            
            # Create many nodes rapidly
            nodes = []
            for i in range(100):
                node = CellframeNode()
                nodes.append(node)
            
            performance_timer.stop()
            
            # Assert performance requirement
            performance_timer.assert_faster_than(1.0)  # Should take less than 1 second
    
    def test_memory_usage_with_monitor(self, mock_lib_context, memory_monitor):
        """Example using memory monitor fixture."""
        from CellFrame.core import CellframeNode
        
        with patch('CellFrame.core.initialize_context') as mock_init:
            mock_init.return_value = mock_lib_context
            
            memory_monitor.start()
            
            # Create objects that might use memory
            nodes = []
            for i in range(50):
                node = CellframeNode()
                nodes.append(node)
            
            memory_monitor.check()
            
            # Assert memory usage is reasonable
            memory_monitor.assert_memory_under(100)  # Less than 100MB growth
    
    def test_with_temp_directory(self, temp_directory, mock_wallet_file):
        """Example using temporary directory and file fixtures."""
        import os
        import json
        
        # Verify temp directory exists
        assert os.path.exists(temp_directory)
        
        # Verify mock wallet file exists
        assert os.path.exists(mock_wallet_file)
        
        # Read and verify wallet file content
        with open(mock_wallet_file, 'r') as f:
            wallet_data = json.load(f)
        
        assert wallet_data["name"] == "test_wallet"
        assert wallet_data["type"] == "simple"
        assert wallet_data["encrypted"] is True
    
    def test_parametrized_wallet_types(self, mock_lib_context, wallet_type):
        """Example using parametrized wallet type fixture."""
        from tests.fixtures.mock_factories import MockWalletFactory
        
        # Create wallet based on parametrized type
        if wallet_type.name if hasattr(wallet_type, 'name') else wallet_type == "simple":
            wallet = MockWalletFactory.create_simple_wallet("param_wallet")
        else:
            wallet = MockWalletFactory.create_hd_wallet("param_wallet")
        
        assert wallet.name == "param_wallet"
        assert wallet.type in ["simple", "hd"]
    
    def test_error_collection(self, assert_no_errors):
        """Example using error collection fixture."""
        
        # Simulate operations that might have errors
        try:
            result = 1 / 1  # This should work
            assert result == 1.0
        except Exception as e:
            assert_no_errors(e)
        
        try:
            value = "test".upper()
            assert value == "TEST"
        except Exception as e:
            assert_no_errors(e)
        
        # The fixture will automatically assert no errors at the end


class TestAsyncOperationsWithFixtures:
    """Example: Async operations with fixtures."""
    
    @pytest.mark.asyncio
    async def test_async_wallet_operations(self, mock_lib_context, async_mock_wallet):
        """Example using async wallet fixture."""
        
        # Test async balance check
        balance = await async_mock_wallet.get_balance_async("CELL")
        assert balance == Decimal("1000.0")
        
        # Test async transfer
        tx_hash = await async_mock_wallet.transfer_async(
            to="recipient_address",
            amount=Decimal("100.0"),
            token="CELL"
        )
        assert tx_hash == "tx_hash_async"


class TestServiceOperationsWithFixtures:
    """Example: Service operations with fixtures."""
    
    def test_staking_service_with_fixture(self, mock_staking_service):
        """Example using staking service fixture."""
        
        # Test staking
        stake_result = mock_staking_service.stake(
            amount=Decimal("1000.0"),
            validator="validator_1"
        )
        
        assert stake_result["amount"] == Decimal("1000.0")
        assert stake_result["validator"] == "validator_1"
        assert "stake_id" in stake_result
        
        # Test getting rewards
        rewards = mock_staking_service.get_rewards(stake_result["stake_id"])
        assert "rewards" in rewards
        assert rewards["period_days"] == 30
    
    def test_network_data_with_fixture(self, mock_network_data):
        """Example using network data fixture."""
        
        # Test mainnet data
        mainnet = mock_network_data["mainnet"]
        assert mainnet["name"] == "mainnet"
        assert mainnet["status"] == "active"
        assert len(mainnet["nodes"]) == 3
        
        # Test testnet data  
        testnet = mock_network_data["testnet"]
        assert testnet["name"] == "testnet"
        assert len(testnet["nodes"]) == 2


class TestFactoryPatternsExample:
    """Example: Using factory patterns for complex test data."""
    
    def test_transaction_batch_factory(self, mock_lib_context):
        """Example using transaction factory for batch testing."""
        from tests.fixtures.mock_factories import MockTransactionFactory
        
        # Create mixed status transactions
        transactions = MockTransactionFactory.create_transaction_batch(
            count=10, 
            status_mix=True
        )
        
        # Verify we have different statuses
        statuses = {tx.status for tx in transactions}
        assert "confirmed" in statuses
        assert "pending" in statuses
        assert "failed" in statuses
        
        # Test confirmed transactions
        confirmed_txs = [tx for tx in transactions if tx.status == "confirmed"]
        assert all(tx.confirmations > 0 for tx in confirmed_txs)
        
        # Test pending transactions
        pending_txs = [tx for tx in transactions if tx.status == "pending"]
        assert all(tx.confirmations == 0 for tx in pending_txs)
    
    def test_network_factory(self):
        """Example using network factory."""
        from tests.fixtures.mock_factories import MockNetworkFactory
        
        # Create test network
        network = MockNetworkFactory.create_network("testnet", "active")
        
        assert network["name"] == "testnet"
        assert network["status"] == "active"
        assert len(network["nodes"]) == 5
        assert network["consensus"] == "proof_of_stake"
        
        # Create node info
        node_info = MockNetworkFactory.create_node_info("test_node_1")
        
        assert node_info["id"] == "test_node_1"
        assert "address" in node_info
        assert "port" in node_info
        assert node_info["version"] == "2.0.0"
    
    def test_data_factory_utilities(self):
        """Example using data factory utilities."""
        from tests.fixtures.mock_factories import MockDataFactory
        
        # Create addresses
        addr1 = MockDataFactory.create_address("wallet")
        addr2 = MockDataFactory.create_address("contract")
        
        assert addr1.startswith("wallet_")
        assert addr2.startswith("contract_")
        assert addr1 != addr2
        
        # Create token data
        token = MockDataFactory.create_token_data("CELL")
        
        assert token["symbol"] == "CELL"
        assert token["name"] == "CELL Token"
        assert token["decimals"] == 18
        
        # Create block data
        block = MockDataFactory.create_block_data(height=12345)
        
        assert block["height"] == 12345
        assert "hash" in block
        assert "transactions" in block
        assert len(block["transactions"]) > 0


# HOW TO MIGRATE EXISTING TESTS:
# 
# BEFORE (without fixtures):
# def test_wallet_creation(self):
#     mock_context = Mock(spec=LibContext)
#     mock_context.is_plugin_mode = False
#     mock_context.is_library_mode = True
#     # ... lots of mock setup ...
#
# AFTER (with fixtures):
# def test_wallet_creation(self, mock_lib_context, mock_wallet):
#     # Just use the pre-configured fixtures!
#     # No repetitive mock setup needed
#
# FACTORY PATTERN:
# Instead of creating individual mocks each time:
# wallet = MockWalletFactory.create_simple_wallet("test")
# transactions = MockTransactionFactory.create_transaction_batch(5)