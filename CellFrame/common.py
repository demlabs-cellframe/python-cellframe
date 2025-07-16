"""
Common functionality for Python-CellFrame
This module provides fallback implementations when native modules are not available
"""
import logging
from typing import Optional, Any, Dict, List

logger = logging.getLogger(__name__)


class CellFrameCommon:
    """
    Common CellFrame functionality
    Fallback implementation when native module is not available
    """
    
    def __init__(self):
        """Initialize common functionality"""
        self.is_initialized = False
        self.version = "1.0.0-fallback"
        logger.warning("Using fallback CellFrameCommon - native module not available")
    
    def init(self) -> bool:
        """Initialize CellFrame common functionality"""
        try:
            self.is_initialized = True
            logger.info("CellFrame common fallback initialized")
            return True
        except Exception as e:
            logger.error(f"Failed to initialize CellFrame common: {e}")
            return False
    
    def deinit(self) -> None:
        """Deinitialize CellFrame common functionality"""
        self.is_initialized = False
        logger.info("CellFrame common fallback deinitialized")
    
    def get_version(self) -> str:
        """Get version string"""
        return self.version
    
    def is_ready(self) -> bool:
        """Check if CellFrame is ready"""
        return self.is_initialized


class CellFrameConfig:
    """
    Configuration management for CellFrame
    """
    
    def __init__(self):
        """Initialize configuration"""
        self._config = {
            "debug": False,
            "log_level": "INFO",
            "network": "testnet",
            "node_role": "full",
            "plugins_enabled": True
        }
        logger.warning("Using fallback CellFrameConfig - native module not available")
    
    def get_item_bool(self, key: str, default: bool = False) -> bool:
        """Get boolean configuration item"""
        value = self._config.get(key, default)
        return bool(value)
    
    def get_item_str(self, key: str, default: str = "") -> str:
        """Get string configuration item"""
        value = self._config.get(key, default)
        return str(value)
    
    def get_item_int(self, key: str, default: int = 0) -> int:
        """Get integer configuration item"""
        value = self._config.get(key, default)
        return int(value)
    
    def set_item(self, key: str, value: Any) -> None:
        """Set configuration item"""
        self._config[key] = value
        logger.info(f"Set config item: {key} = {value}")
    
    def get_all_items(self) -> Dict[str, Any]:
        """Get all configuration items"""
        return self._config.copy()


class CellFrameLogger:
    """
    Logging functionality for CellFrame
    """
    
    def __init__(self):
        """Initialize logger"""
        self.logger = logging.getLogger("cellframe")
        logger.warning("Using fallback CellFrameLogger - native module not available")
    
    def debug(self, message: str) -> None:
        """Log debug message"""
        self.logger.debug(message)
    
    def info(self, message: str) -> None:
        """Log info message"""
        self.logger.info(message)
    
    def warning(self, message: str) -> None:
        """Log warning message"""
        self.logger.warning(message)
    
    def error(self, message: str) -> None:
        """Log error message"""
        self.logger.error(message)
    
    def critical(self, message: str) -> None:
        """Log critical message"""
        self.logger.critical(message)


class CellFrameNode:
    """
    Node functionality for CellFrame
    """
    
    def __init__(self):
        """Initialize node"""
        self.is_running = False
        self.node_id = "fallback-node-001"
        self.networks = ["testnet"]
        logger.warning("Using fallback CellFrameNode - native module not available")
    
    def start(self) -> bool:
        """Start the node"""
        try:
            self.is_running = True
            logger.info("CellFrame node fallback started")
            return True
        except Exception as e:
            logger.error(f"Failed to start CellFrame node: {e}")
            return False
    
    def stop(self) -> None:
        """Stop the node"""
        self.is_running = False
        logger.info("CellFrame node fallback stopped")
    
    def get_status(self) -> Dict[str, Any]:
        """Get node status"""
        return {
            "running": self.is_running,
            "node_id": self.node_id,
            "networks": self.networks,
            "version": "1.0.0-fallback"
        }
    
    def get_network_list(self) -> List[str]:
        """Get list of available networks"""
        return self.networks.copy()


class CellFrameChain:
    """
    Chain functionality for CellFrame
    """
    
    def __init__(self, chain_name: str = "testnet"):
        """Initialize chain"""
        self.chain_name = chain_name
        self.is_loaded = False
        self.blocks = []
        logger.warning("Using fallback CellFrameChain - native module not available")
    
    def load(self) -> bool:
        """Load the chain"""
        try:
            self.is_loaded = True
            logger.info(f"CellFrame chain {self.chain_name} fallback loaded")
            return True
        except Exception as e:
            logger.error(f"Failed to load CellFrame chain {self.chain_name}: {e}")
            return False
    
    def get_block_count(self) -> int:
        """Get number of blocks in chain"""
        return len(self.blocks)
    
    def get_last_block(self) -> Optional[Dict[str, Any]]:
        """Get last block in chain"""
        if not self.blocks:
            return None
        return self.blocks[-1]
    
    def add_block(self, block_data: Dict[str, Any]) -> bool:
        """Add block to chain"""
        try:
            self.blocks.append(block_data)
            logger.info(f"Added block to chain {self.chain_name}")
            return True
        except Exception as e:
            logger.error(f"Failed to add block to chain {self.chain_name}: {e}")
            return False


# Global instances
_common = CellFrameCommon()
_config = CellFrameConfig()
_logger = CellFrameLogger()
_node = CellFrameNode()

# Compatibility functions
def dap_config_get_item_bool_default(key: str, default: bool = False) -> bool:
    """Get boolean config item with default"""
    return _config.get_item_bool(key, default)

def dap_config_get_item_str_default(key: str, default: str = "") -> str:
    """Get string config item with default"""
    return _config.get_item_str(key, default)

def dap_config_get_item_int_default(key: str, default: int = 0) -> int:
    """Get integer config item with default"""
    return _config.get_item_int(key, default)

def dap_log_level_set(level: str) -> None:
    """Set log level"""
    _logger.logger.setLevel(getattr(logging, level.upper(), logging.INFO))

def dap_log_level_get() -> str:
    """Get current log level"""
    return logging.getLevelName(_logger.logger.level)

def cellframe_node_init() -> bool:
    """Initialize CellFrame node"""
    return _common.init() and _node.start()

def cellframe_node_deinit() -> None:
    """Deinitialize CellFrame node"""
    _node.stop()
    _common.deinit()

def cellframe_node_get_status() -> Dict[str, Any]:
    """Get node status"""
    return _node.get_status()

# Export main classes and functions
__all__ = [
    'CellFrameCommon',
    'CellFrameConfig', 
    'CellFrameLogger',
    'CellFrameNode',
    'CellFrameChain',
    'dap_config_get_item_bool_default',
    'dap_config_get_item_str_default',
    'dap_config_get_item_int_default',
    'dap_log_level_set',
    'dap_log_level_get',
    'cellframe_node_init',
    'cellframe_node_deinit',
    'cellframe_node_get_status'
] 