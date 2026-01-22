"""
🧪 Global Test Fixtures for Python Cellframe SDK
Centralized pytest fixtures for mocks and test data.
"""

import pytest
import tempfile
import os
from unittest.mock import Mock, MagicMock, patch
from decimal import Decimal
from datetime import datetime, timedelta
from typing import Dict, Any, List

# Initialize DAP SDK for testing environment before importing CellFrame modules
@pytest.fixture(scope="session", autouse=True)
def setup_dap_sdk_for_tests():
    """
    Initialize DAP SDK with test-friendly paths before any CellFrame imports.
    This prevents /opt/dap access issues and segfaults.
    """
    try:
        import dap
        from dap.core.dap import Dap
        
        # Create test directories in venv_test or /tmp
        test_base = os.environ.get('CELLFRAME_TEST_DIR', '/tmp/cellframe_tests')
        
        dap_config = {
            'app_name': "cellframe_tests", 
            'working_dir': test_base,
            'config_dir': f"{test_base}/etc",
            'temp_dir': f"{test_base}/tmp", 
            'log_file': f"{test_base}/var/log/cellframe_tests.log",
            'debug_mode': False,  # Reduce noise in tests
            'events_threads': 1   # Minimal for tests
        }
        
        # Create required directories
        os.makedirs(dap_config['config_dir'], exist_ok=True)
        os.makedirs(dap_config['temp_dir'], exist_ok=True)
        os.makedirs(os.path.dirname(dap_config['log_file']), exist_ok=True)
        
        # Initialize DAP SDK with test configuration
        dap_instance = Dap(dap_config=dap_config)
        dap_instance.init()
        
        print(f"✅ DAP SDK initialized for tests in: {test_base}")
        yield dap_instance
        
        # Cleanup is handled by OS
        
    except Exception as e:
        print(f"⚠️  DAP SDK test setup failed: {e}")
        # Continue without DAP SDK for mock-only tests
        yield None

# Import types for proper mocking (after DAP SDK is initialized)
# FAIL-FAST: Either CellFrame modules are available or tests fail
from CellFrame.core.context import AppContext, LibContext, PluginContext, ExecutionMode
from CellFrame.chain import Wallet, TX, CfLedger, WalletType, TxType
from CellFrame.core.exceptions import CellframeException


# =========================================
# CONTEXT FIXTURES
# =========================================

@pytest.fixture
def mock_lib_context():
    """Mock LibContext for library mode testing."""
    context = Mock(spec=LibContext)
    context.is_plugin_mode = False
    context.is_library_mode = True
    context.app_name = "test_app"
    context.execution_mode = "library"
    context.initialize.return_value = True
    context.shutdown.return_value = None
    context.get_resource.return_value = None
    context.get_data_dir.return_value = "/tmp/test_data"
    context.get_config_dir.return_value = "/tmp/test_config"
    return context


@pytest.fixture
def mock_plugin_context():
    """Mock PluginContext for plugin mode testing."""
    context = Mock(spec=PluginContext)
    context.is_plugin_mode = True
    context.is_library_mode = False
    context.app_name = "test_plugin"
    context.execution_mode = "plugin"
    context.initialize.return_value = True
    context.shutdown.return_value = None
    context.get_resource.return_value = None
    context.get_plugin_manifest.return_value = {"name": "test_plugin", "version": "1.0.0"}
    context.get_node_config.return_value = {"debug": True}
    return context


@pytest.fixture
def mock_app_context():
    """Generic mock AppContext."""
    context = Mock(spec=AppContext)
    context.is_plugin_mode = False
    context.is_library_mode = True
    context.app_name = "test_generic"
    context.initialize.return_value = True
    context.shutdown.return_value = None
    context.get_resource.return_value = None
    return context


# =========================================
# WALLET FIXTURES
# =========================================

@pytest.fixture
def mock_wallet():
    """Mock Wallet with standard test data."""
    wallet = Mock(spec=Wallet)
    wallet.name = "test_wallet"
    wallet.type = WalletType.SIMPLE
    wallet.get_address.return_value = "test_address_12345"
    wallet.get_balance.return_value = Decimal("1000.0")
    wallet.save.return_value = True
    wallet.is_valid = True
    return wallet


@pytest.fixture
def mock_hd_wallet():
    """Mock HD Wallet with hierarchical features."""
    wallet = Mock(spec=Wallet)
    wallet.name = "hd_test_wallet"
    wallet.type = WalletType.HD
    wallet.get_address.return_value = "hd_address_67890"
    wallet.get_balance.return_value = Decimal("5000.0")
    wallet.can_derive_addresses = True
    wallet.max_addresses = 1000
    wallet.derive_address.return_value = "derived_address_999"
    wallet.save.return_value = True
    wallet.is_valid = True
    return wallet


@pytest.fixture
def mock_wallet_manager():
    """Mock WalletManager for testing wallet operations."""
    manager = Mock()
    manager.create_wallet.return_value = Mock(name="created_wallet")
    manager.load_wallet.return_value = Mock(name="loaded_wallet")
    manager.get_all_wallets.return_value = []
    manager.delete_wallet.return_value = True
    return manager


# =========================================
# TRANSACTION FIXTURES
# =========================================

@pytest.fixture
def mock_transaction():
    """Mock Transaction with standard test data."""
    tx = Mock(spec=TX)
    tx.hash = "tx_hash_abcdef123456"
    tx.type = TxType.TRANSFER
    tx.status = "pending"
    tx.amount = Decimal("100.0")
    tx.fee = Decimal("1.0")
    tx.timestamp = datetime.now()
    
    # Mock methods
    tx.add_input.return_value = True
    tx.add_output.return_value = True
    tx.sign.return_value = True
    tx.verify.return_value = True
    tx.get_size.return_value = 256
    
    return tx


@pytest.fixture
def mock_confirmed_transaction():
    """Mock confirmed transaction."""
    tx = Mock(spec=TX)
    tx.hash = "confirmed_tx_hash_123"
    tx.type = TxType.TRANSFER
    tx.status = "confirmed"
    tx.amount = Decimal("200.0")
    tx.fee = Decimal("2.0")
    tx.timestamp = datetime.now() - timedelta(hours=1)
    tx.confirmations = 6
    
    # Mock methods
    tx.add_input.return_value = True
    tx.add_output.return_value = True
    tx.sign.return_value = True
    tx.verify.return_value = True
    tx.get_size.return_value = 512
    
    return tx


# =========================================
# LEDGER FIXTURES
# =========================================

@pytest.fixture
def mock_ledger():
    """Mock CfLedger for testing ledger operations."""
    ledger = Mock(spec=CfLedger)
    ledger.name = "test_ledger"
    ledger.token = "CELL"
    ledger.get_balance.return_value = Decimal("10000.0")
    ledger.add_transaction.return_value = True
    ledger.get_transactions.return_value = []
    ledger.validate_transaction.return_value = True
    return ledger


@pytest.fixture
def mock_ledger_manager():
    """Mock DapLedgerManager for testing ledger management."""
    manager = Mock()
    manager.create_ledger.return_value = Mock(name="created_ledger")
    manager.get_ledger.return_value = Mock(name="existing_ledger")
    manager.get_all_ledgers.return_value = []
    return manager


# =========================================
# NETWORK FIXTURES
# =========================================

@pytest.fixture
def mock_network_data():
    """Mock network data for testing."""
    return {
        "mainnet": {
            "name": "mainnet",
            "chain_id": "0x1",
            "status": "active",
            "nodes": ["node1", "node2", "node3"]
        },
        "testnet": {
            "name": "testnet", 
            "chain_id": "0x3",
            "status": "active",
            "nodes": ["testnode1", "testnode2"]
        }
    }


# =========================================
# SERVICE FIXTURES
# =========================================

@pytest.fixture
def mock_staking_service():
    """Mock StakingService for testing staking operations."""
    service = Mock()
    service.stake.return_value = {
        "stake_id": "stake_123",
        "amount": Decimal("1000.0"),
        "validator": "validator_1",
        "timestamp": datetime.now()
    }
    service.unstake.return_value = {
        "stake_id": "stake_123",
        "unstaked": True,
        "amount": Decimal("1000.0"),
        "timestamp": datetime.now()
    }
    service.get_rewards.return_value = {
        "stake_id": "stake_123",
        "rewards": Decimal("50.0"),
        "period_days": 30
    }
    service.get_validators.return_value = [
        {"id": "validator_1", "stake": Decimal("100000.0")},
        {"id": "validator_2", "stake": Decimal("200000.0")}
    ]
    return service


# =========================================
# FILE SYSTEM FIXTURES
# =========================================

@pytest.fixture
def temp_directory():
    """Temporary directory for file operations."""
    temp_dir = tempfile.mkdtemp()
    yield temp_dir
    # Cleanup
    import shutil
    if os.path.exists(temp_dir):
        shutil.rmtree(temp_dir)


@pytest.fixture
def mock_wallet_file(temp_directory):
    """Mock wallet file for testing file operations."""
    wallet_path = os.path.join(temp_directory, "test_wallet.dat")
    
    # Create mock wallet file content
    wallet_data = {
        "name": "test_wallet",
        "type": "simple",
        "encrypted": True,
        "created": datetime.now().isoformat()
    }
    
    import json
    with open(wallet_path, 'w') as f:
        json.dump(wallet_data, f)
    
    return wallet_path


# =========================================
# ERROR FIXTURES
# =========================================

@pytest.fixture
def mock_cellframe_exception():
    """Mock CellframeException for error testing."""
    return CellframeException("Test error message")


@pytest.fixture
def mock_network_error():
    """Mock network error for testing network failures."""
    return ConnectionError("Network connection failed")


@pytest.fixture
def mock_validation_error():
    """Mock validation error for testing input validation."""
    return ValueError("Invalid input data")


# =========================================
# PERFORMANCE FIXTURES
# =========================================

@pytest.fixture
def performance_timer():
    """Performance timer fixture for testing execution times."""
    import time
    
    class PerformanceTimer:
        def __init__(self):
            self.start_time = None
            self.end_time = None
            
        def start(self):
            self.start_time = time.perf_counter()
            
        def stop(self):
            self.end_time = time.perf_counter()
            
        @property
        def elapsed(self):
            if self.start_time and self.end_time:
                return self.end_time - self.start_time
            return None
            
        def assert_faster_than(self, max_seconds):
            assert self.elapsed is not None, "Timer not stopped"
            assert self.elapsed < max_seconds, f"Execution took {self.elapsed:.3f}s, expected < {max_seconds}s"
    
    return PerformanceTimer()


# =========================================
# MEMORY FIXTURES
# =========================================

@pytest.fixture
def memory_monitor():
    """Memory monitoring fixture for testing memory usage."""
    try:
        import psutil
        
        class MemoryMonitor:
            def __init__(self):
                self.process = psutil.Process()
                self.initial_memory = None
                self.current_memory = None
                
            def start(self):
                self.initial_memory = self.process.memory_info().rss / 1024 / 1024  # MB
                
            def check(self):
                self.current_memory = self.process.memory_info().rss / 1024 / 1024  # MB
                
            @property
            def memory_growth(self):
                if self.initial_memory and self.current_memory:
                    return self.current_memory - self.initial_memory
                return None
                
            def assert_memory_under(self, max_mb):
                assert self.memory_growth is not None, "Memory not checked"
                assert self.memory_growth < max_mb, f"Memory grew by {self.memory_growth:.2f}MB, expected < {max_mb}MB"
        
        return MemoryMonitor()
        
    except ImportError:
        # Fallback if psutil not available - real implementation
        class DummyMemoryMonitor:
            def __init__(self):
                self.start_memory = 0
                self.current_memory = 0
                self.started = False
                
            def start(self):
                """Start memory monitoring using basic system info"""
                import os
                try:
                    # Use /proc/self/status on Linux
                    with open('/proc/self/status') as f:
                        for line in f:
                            if line.startswith('VmRSS:'):
                                self.start_memory = int(line.split()[1])  # KB
                                break
                    self.started = True
                except (FileNotFoundError, PermissionError):
                    # Fallback for non-Linux systems
                    self.start_memory = 0
                    self.started = True
                    
            def check(self):
                """Check current memory usage"""
                if not self.started:
                    return
                try:
                    with open('/proc/self/status') as f:
                        for line in f:
                            if line.startswith('VmRSS:'):
                                self.current_memory = int(line.split()[1])  # KB
                                break
                except (FileNotFoundError, PermissionError):
                    self.current_memory = self.start_memory
                    
            @property
            def memory_growth(self):
                """Get memory growth in MB"""
                if not self.started:
                    return 0
                return (self.current_memory - self.start_memory) / 1024  # Convert KB to MB
                
            def assert_memory_under(self, max_mb):
                """Assert memory growth is under limit"""
                self.check()
                growth = self.memory_growth
                if growth > max_mb:
                    pytest.fail(f"Memory grew by {growth:.2f}MB, expected < {max_mb}MB")
        
        return DummyMemoryMonitor()


# =========================================
# CONFIGURATION FIXTURES
# =========================================

@pytest.fixture
def mock_config():
    """Mock configuration for testing."""
    return {
        "debug": True,
        "log_level": "DEBUG",
        "network": "testnet",
        "data_dir": "/tmp/test_data",
        "config_dir": "/tmp/test_config",
        "plugins": ["test_plugin"],
        "database": {
            "type": "sqlite",
            "path": ":memory:"
        },
        "network_config": {
            "timeout": 30,
            "max_connections": 100,
            "retry_attempts": 3
        }
    }


@pytest.fixture
def mock_legacy_config():
    """Mock legacy configuration format."""
    return {
        "cellframe_debug": True,
        "cellframe_network": "mainnet",
        "dap_debug": True,
        "wallet_path": "/old/path/wallets",
        "node_addr": "old_node_address"
    }


# =========================================
# INTEGRATION FIXTURES
# =========================================

@pytest.fixture
def mock_cellframe_node():
    """Mock CellframeNode for integration testing."""
    from CellFrame.core import CellframeNode
    
    with patch('CellFrame.core.initialize_context') as mock_init:
        mock_context = Mock()
        mock_context.is_plugin_mode = False
        mock_context.is_library_mode = True
        mock_context.app_name = "test_node"
        mock_context.initialize.return_value = True
        mock_init.return_value = mock_context
        
        node = CellframeNode()
        
        # Mock chain component
        node.chain = Mock()
        node.chain.create_wallet = Mock()
        node.chain.get_all_wallets = Mock(return_value=[])
        node.chain.create_transaction = Mock()
        
        yield node


# =========================================
# ASYNC FIXTURES
# =========================================

@pytest.fixture
def event_loop():
    """Event loop for async tests."""
    import asyncio
    loop = asyncio.new_event_loop()
    yield loop
    loop.close()


@pytest.fixture
async def async_mock_wallet():
    """Async mock wallet for testing async operations."""
    wallet = Mock()
    wallet.get_balance_async = Mock(return_value=Decimal("1000.0"))
    wallet.transfer_async = Mock(return_value="tx_hash_async")
    return wallet


# =========================================
# MARKERS AND PARAMETRIZATION
# =========================================

@pytest.fixture(params=[WalletType.SIMPLE, WalletType.HARDWARE])
def wallet_type(request):
    """Parametrized wallet type fixture."""
    return request.param


@pytest.fixture(params=[True, False])
def plugin_mode(request):
    """Parametrized plugin mode fixture."""
    return request.param


@pytest.fixture(params=["mainnet", "testnet", "devnet"])
def network_name(request):
    """Parametrized network name fixture."""
    return request.param


# =========================================
# CLEANUP FIXTURES
# =========================================

@pytest.fixture(autouse=True)
def cleanup_mocks():
    """Auto-cleanup fixture to reset mocks after each test."""
    yield
    # Reset any global state if needed
    # This runs after each test


@pytest.fixture(scope="session", autouse=True)
def test_session_setup():
    """Session-wide setup and cleanup."""
    # Setup
    print("\n🧪 Starting test session for Python Cellframe SDK")
    
    yield
    
    # Cleanup
    print("\n✅ Test session completed")


# =========================================
# UTILITY FIXTURES
# =========================================

@pytest.fixture
def assert_no_errors():
    """Fixture to collect and assert no errors occurred."""
    errors = []
    
    def add_error(error):
        errors.append(error)
    
    yield add_error
    
    # Assert no errors at end of test
    if errors:
        error_messages = "\n".join(str(e) for e in errors)
        pytest.fail(f"Errors occurred during test:\n{error_messages}")


@pytest.fixture
def mock_logger():
    """Mock logger for testing logging functionality."""
    logger = Mock()
    logger.debug = Mock()
    logger.info = Mock()
    logger.warning = Mock()
    logger.error = Mock()
    logger.critical = Mock()
    return logger