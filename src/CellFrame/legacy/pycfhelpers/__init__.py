"""
Legacy pycfhelpers module for backward compatibility

This module provides compatibility layer for old pycfhelpers imports.
All functionality should be migrated to new CellFrame API.
"""

import warnings
from typing import Any, Dict, List, Optional


# =========================================
# LOGGER (Legacy)
# =========================================
class LegacyLogger:
    """Legacy logger for pycfhelpers compatibility"""
    
    def __init__(self):
        pass
    
    def debug(self, msg: str, *args, **kwargs):
        """Legacy debug logging"""
        import logging
        logging.debug(msg, *args, **kwargs)
    
    def info(self, msg: str, *args, **kwargs):
        """Legacy info logging"""
        import logging
        logging.info(msg, *args, **kwargs)
    
    def warning(self, msg: str, *args, **kwargs):
        """Legacy warning logging"""
        import logging
        logging.warning(msg, *args, **kwargs)
    
    def error(self, msg: str, *args, **kwargs):
        """Legacy error logging"""
        import logging
        logging.error(msg, *args, **kwargs)
    
    def critical(self, msg: str, *args, **kwargs):
        """Legacy critical logging"""
        import logging
        logging.critical(msg, *args, **kwargs)

# Global logger instance
logger = LegacyLogger()
log = logger


# =========================================
# HELPERS (Legacy)
# =========================================
class LegacyHelpers:
    """Legacy helpers for pycfhelpers compatibility"""
    
    @staticmethod
    def json_dump(obj: Any, **kwargs) -> str:
        """Legacy JSON dump"""
        import json
        return json.dumps(obj, **kwargs)
    
    @staticmethod
    def find_tx_out(*args, **kwargs):
        """Legacy find transaction output"""
        from ...chain.tx_utils import find_tx_out
        return find_tx_out(*args, **kwargs)
    
    @staticmethod
    def get_tx_items(*args, **kwargs):
        """Legacy get transaction items"""
        from ...chain.tx_utils import get_tx_items
        return get_tx_items(*args, **kwargs)

helpers = LegacyHelpers()


# =========================================
# NODE MODULES (Legacy)
# =========================================
class LegacyNode:
    """Legacy node modules container"""
    
    class net:
        """Legacy net module"""
        
        class CFNet:
            """Legacy CFNet class"""
            def __init__(self, *args, **kwargs):
                from ...network import Net
                self._net = Net(*args, **kwargs)
        
        class CFChain:
            """Legacy CFChain class"""
            def __init__(self, *args, **kwargs):
                from ...chain import ChainAtomPtr
                self._chain = ChainAtomPtr()
        
        class CFWalletAddress:
            """Legacy CFWalletAddress class"""
            def __init__(self, *args, **kwargs):
                from ...types import Address
                self._addr = Address(*args, **kwargs)
    
    class consensus:
        """Legacy consensus module"""
        
        class CFBlock:
            """Legacy CFBlock class"""
            def __init__(self, *args, **kwargs):
                pass
        
        class CFEvent:
            """Legacy CFEvent class"""
            def __init__(self, *args, **kwargs):
                pass
    
    class datums:
        """Legacy datums module"""
        
        class CFDatum:
            """Legacy CFDatum class"""
            def __init__(self, *args, **kwargs):
                pass
    
    class logging:
        """Legacy logging module"""
        
        class CFLog:
            """Legacy CFLog class"""
            def __init__(self, *args, **kwargs):
                pass
    
    class crypto:
        """Legacy crypto module"""
        
        class CFGUUID:
            """Legacy CFGUUID class"""
            def __init__(self, *args, **kwargs):
                import uuid
                self._uuid = uuid.uuid4()

node = LegacyNode()


# =========================================
# DAP MODULE (Legacy)
# =========================================
class LegacyDAP:
    """Legacy DAP module for compatibility"""
    
    class Crypto:
        """Legacy crypto module"""
        
        class HashFast:
            """Legacy HashFast class"""
            def __init__(self, *args, **kwargs):
                pass
        
        class Cert:
            """Legacy Cert class"""
            def __init__(self, *args, **kwargs):
                pass
        
        class Sign:
            """Legacy Sign class"""
            def __init__(self, *args, **kwargs):
                pass
        
        class Pkey:
            """Legacy Pkey class"""
            def __init__(self, *args, **kwargs):
                pass
        
        class GUUID:
            """Legacy GUUID class"""
            def __init__(self, *args, **kwargs):
                import uuid
                self._uuid = uuid.uuid4()
    
    class Core:
        """Legacy core module"""
        
        class Math:
            """Legacy Math class"""
            def __init__(self):
                pass
        
        @staticmethod
        def logIt(*args, **kwargs):
            """Legacy logging function"""
            import logging
            if args:
                logging.info(args[0] if len(args) == 1 else ' '.join(str(a) for a in args))
        
        class NodeAddr:
            """Legacy NodeAddr class"""
            def __init__(self):
                pass
        
        class AppContext:
            """Legacy AppContext class"""
            def __init__(self):
                pass
    
    class Network:
        """Legacy network module"""
        
        class ClusterRoles:
            """Legacy ClusterRoles enum"""
            def __init__(self):
                pass
        
        class Server:
            """Legacy Server class"""
            def __init__(self):
                pass
        
        class HttpSimple:
            """Legacy HttpSimple class"""
            def __init__(self):
                pass
        
        class HttpCode:
            """Legacy HttpCode enum"""
            OK = 200
            NOT_FOUND = 404
            SERVER_ERROR = 500
            
            def __init__(self):
                pass
        
        class HttpHeader:
            """Legacy HttpHeader class"""
            def __init__(self):
                pass
    
    class GlobalDB:
        """Legacy GlobalDB module"""
        
        class DB:
            """Legacy DB class"""
            def __init__(self):
                pass
        
        class Cluster:
            """Legacy Cluster class"""
            def __init__(self):
                pass
        
        class Instance:
            """Legacy Instance class"""
            def __init__(self):
                pass
        
        class MemberRoles:
            """Legacy MemberRoles enum"""
            def __init__(self):
                pass

DAP = LegacyDAP()

# Add logIt as direct function for imports like "from DAP.Core import logIt"
logIt = DAP.Core.logIt


# Export all legacy components
# Transaction types for compatibility
class TxIn:
    """Transaction input"""
    pass

class TxInCond:
    """Conditional transaction input"""
    pass

class TxOut:
    """Transaction output"""
    pass

class TxOutCond:
    """Conditional transaction output"""
    pass

class TxPkey:
    """Transaction public key"""
    pass

class TxSig:
    """Transaction signature"""
    pass

class TxToken:
    """Transaction token"""
    pass

class TxReceipt:
    """Transaction receipt"""
    pass

class TxOutExt:
    """Extended transaction output"""
    pass

class TxOutCondSubtypeSrvStakeLock:
    """Stake lock output condition"""
    pass

class TxOutStd:
    """Standard transaction output"""
    pass

__all__ = [
    'logger',
    'log', 
    'helpers',
    'node',
    'DAP',
    'logIt',  # Direct export for "from DAP.Core import logIt"
    # Transaction types
    'TxIn',
    'TxInCond',
    'TxOut',
    'TxOutCond',
    'TxPkey',
    'TxSig',
    'TxToken',
    'TxReceipt',
    'TxOutExt',
    'TxOutCondSubtypeSrvStakeLock',
    'TxOutStd'
]