"""
🧪 Pytest Configuration and Fixtures

Global pytest configuration for CellFrame Node Python Plugin testing.
Provides common fixtures and test utilities.
"""

import os
import sys
import json
import pytest
import tempfile
import shutil
from pathlib import Path
from unittest.mock import Mock, MagicMock
from decimal import Decimal
from typing import Generator, Dict, Any, Optional

# Plugin root directory
PLUGIN_ROOT = Path(__file__).parent.parent

# Add python-cellframe to Python path (once, properly)
PYTHON_CELLFRAME_PATH = str(PLUGIN_ROOT / "python-cellframe")
if PYTHON_CELLFRAME_PATH not in sys.path:
    sys.path.insert(0, PYTHON_CELLFRAME_PATH)

# Add python-dap to Python path (once, properly)
PYTHON_DAP_PATH = str(PLUGIN_ROOT / "python-dap") 
if PYTHON_DAP_PATH not in sys.path:
    sys.path.insert(0, PYTHON_DAP_PATH)

# Test configuration
pytest_plugins = ["pytest_asyncio", "pytest_mock", "pytest_benchmark"]


@pytest.fixture(scope="session")
def plugin_root() -> Path:
    """Root directory of the plugin"""
    return PLUGIN_ROOT


@pytest.fixture(scope="session")
def tests_root() -> Path:
    """Root directory of tests"""
    return Path(__file__).parent


@pytest.fixture(scope="session")
def test_data_dir() -> Path:
    """Directory containing test data"""
    return Path(__file__).parent / "fixtures" / "data"


@pytest.fixture(scope="session")
def test_config_dir() -> Path:
    """Directory containing test configuration"""
    return Path(__file__).parent / "config"


@pytest.fixture(scope="session")
def test_plugin_dir() -> Path:
    """Directory containing test plugins"""
    return Path(__file__).parent / "plugin"


@pytest.fixture
def temp_dir() -> Generator[Path, None, None]:
    """Temporary directory for test files"""
    with tempfile.TemporaryDirectory() as tmpdir:
        yield Path(tmpdir)


@pytest.fixture
def mock_cellframe_sdk():
    """Mock CellFrame SDK for isolated testing"""
    with pytest.MonkeyPatch().context() as m:
        # Mock the main SDK modules and functions that exist
        mock_sdk = MagicMock()
        
        # Mock wallet functions from chain module
        m.setattr("CellFrame.chain.wallet.create_wallet", Mock(return_value=mock_sdk))
        m.setattr("CellFrame.chain.wallet.open_wallet", Mock(return_value=mock_sdk))
        m.setattr("CellFrame.chain.wallet.get_all_wallets", Mock(return_value=[]))
        
        # Mock ledger functions from chain module  
        m.setattr("CellFrame.chain.ledger.create_ledger", Mock(return_value=mock_sdk))
        m.setattr("CellFrame.chain.ledger.open_ledger", Mock(return_value=mock_sdk))
        
        # Mock context functions that exist
        m.setattr("CellFrame.core.context.get_context", Mock(return_value=mock_sdk))
        m.setattr("CellFrame.core.context.initialize_context", Mock(return_value=mock_sdk))
        
        yield mock_sdk


@pytest.fixture
def mock_dap_sdk():
    """Mock DAP SDK for isolated testing"""
    with pytest.MonkeyPatch().context() as m:
        mock_dap = MagicMock()
        m.setattr("dap.core.dap_common_init", Mock(return_value=0))
        m.setattr("dap.crypto.dap_crypto_init", Mock(return_value=0))
        yield mock_dap


@pytest.fixture
def sample_wallet_data() -> Dict[str, Any]:
    """Sample wallet data for testing"""
    return {
        "name": "test_wallet",
        "address": "0x1234567890abcdef1234567890abcdef12345678",
        "private_key": "test_private_key",
        "public_key": "test_public_key",
        "balance": 1000.0
    }


@pytest.fixture
def sample_transaction_data() -> Dict[str, Any]:
    """Sample transaction data for testing"""
    return {
        "from_addr": "0x1234567890abcdef1234567890abcdef12345678",
        "to_addr": "0xabcdef1234567890abcdef1234567890abcdef12",
        "amount": 100.0,
        "fee": 1.0,
        "token": "CELL",
        "network": "testnet"
    }


@pytest.fixture
def composer_config() -> Dict[str, Any]:
    """Configuration for transaction composer testing"""
    return {
        "fee_optimization": True,
        "batch_processing": True,
        "max_batch_size": 100,
        "timeout": 30,
        "retry_attempts": 3
    }


@pytest.fixture
def mock_composer():
    """Mock transaction composer for testing"""
    from unittest.mock import Mock
    
    composer = Mock()
    composer.net_name = "testnet"
    composer.wallet_name = "test_wallet"
    composer.create_tx = Mock(return_value={"tx_hash": "test_hash"})
    composer.estimate_fee = Mock(return_value=1.0)
    composer.validate_tx = Mock(return_value=True)
    composer.broadcast_tx = Mock(return_value=True)
    return composer


@pytest.fixture
def conditional_processor_fixtures() -> Dict[str, Any]:
    """Fixtures for conditional processor testing"""
    return {
        "stake_lock": {
            "amount": 1000.0,
            "duration": 30,  # days
            "auto_prolong": True,
            "penalty_rate": 0.1
        },
        "exchange": {
            "token_from": "CELL",
            "token_to": "BTC", 
            "amount": 100.0,
            "rate": 0.0001,
            "order_type": "limit"
        },
        "voting": {
            "proposal_id": "test_proposal",
            "vote": "yes",
            "weight": 100.0
        },
        "delegation": {
            "validator": "test_validator",
            "amount": 500.0,
            "duration": 90
        }
    }


@pytest.fixture(scope="session")
def event_loop():
    """Create an instance of the default event loop for the test session."""
    import asyncio
    loop = asyncio.get_event_loop_policy().new_event_loop()
    yield loop
    loop.close()


@pytest.fixture
def plugin_binary_mock():
    """Mock binary plugin for testing plugin infrastructure"""
    mock_plugin = Mock()
    mock_plugin.init = Mock(return_value=0)
    mock_plugin.deinit = Mock(return_value=None)
    mock_plugin.get_info = Mock(return_value={
        "name": "test_plugin",
        "version": "1.0.0",
        "description": "Test plugin"
    })
    return mock_plugin


@pytest.fixture
def standalone_library_mock():
    """Mock standalone library for testing standalone package"""
    mock_lib = Mock()
    mock_lib.cellframe_init = Mock(return_value=0)
    mock_lib.cellframe_deinit = Mock(return_value=None)
    mock_lib.get_version = Mock(return_value="1.0.0")
    return mock_lib


@pytest.fixture(autouse=True)
def reset_imports():
    """Reset module imports between tests"""
    modules_to_remove = [
        mod for mod in sys.modules.keys() 
        if mod.startswith(('cellframe', 'dap'))
    ]
    for mod in modules_to_remove:
        if mod in sys.modules:
            del sys.modules[mod]


def pytest_configure(config):
    """Pytest configuration"""
    # Add custom markers
    config.addinivalue_line("markers", "unit: Unit tests")
    config.addinivalue_line("markers", "integration: Integration tests")
    config.addinivalue_line("markers", "performance: Performance tests")
    config.addinivalue_line("markers", "plugin: Plugin infrastructure tests")
    config.addinivalue_line("markers", "composer: Transaction composer tests")
    config.addinivalue_line("markers", "conditional: Conditional processor tests")
    config.addinivalue_line("markers", "legacy: Legacy compatibility tests")
    config.addinivalue_line("markers", "mock_only: Tests using only mocks")
    config.addinivalue_line("markers", "slow: Slow running tests")
    
    # Setup test environment
    os.environ.setdefault("CELLFRAME_TEST_MODE", "1")
    os.environ.setdefault("DAP_TEST_MODE", "1")


def pytest_collection_modifyitems(config, items):
    """Modify test collection"""
    for item in items:
        # Add markers based on test path
        if "unit" in str(item.fspath):
            item.add_marker(pytest.mark.unit)
        elif "integration" in str(item.fspath):
            item.add_marker(pytest.mark.integration)
        elif "performance" in str(item.fspath):
            item.add_marker(pytest.mark.performance)
            
        # Add slow marker for tests that take time
        if "performance" in str(item.fspath) or "slow" in item.name:
            item.add_marker(pytest.mark.slow) 