"""
Legacy pycfhelpers module for backward compatibility

This module provides compatibility layer for old pycfhelpers imports.
All functionality should be migrated to new CellFrame API.
"""

import warnings
from typing import Any, Dict, List, Optional

# Import legacy warning from parent
from .. import LegacyWarning, _show_legacy_warning


# =========================================
# LOGGER (Legacy)
# =========================================
class LegacyLogger:
    """Legacy logger for pycfhelpers compatibility"""
    
    def __init__(self):
        _show_legacy_warning("pycfhelpers.logger", "Python logging module")
    
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
        _show_legacy_warning("pycfhelpers.helpers.json_dump", "json.dumps")
        import json
        return json.dumps(obj, **kwargs)
    
    @staticmethod
    def find_tx_out(*args, **kwargs):
        """Legacy find transaction output"""
        _show_legacy_warning("pycfhelpers.helpers.find_tx_out", "CellFrame.chain.tx_utils.find_tx_out")
        from ...chain.tx_utils import find_tx_out
        return find_tx_out(*args, **kwargs)
    
    @staticmethod
    def get_tx_items(*args, **kwargs):
        """Legacy get transaction items"""
        _show_legacy_warning("pycfhelpers.helpers.get_tx_items", "CellFrame.chain.tx_utils.get_tx_items")
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
                _show_legacy_warning("pycfhelpers.node.net.CFNet", "CellFrame.network.Net")
                from ...network import Net
                self._net = Net(*args, **kwargs)
        
        class CFChain:
            """Legacy CFChain class"""
            def __init__(self, *args, **kwargs):
                _show_legacy_warning("pycfhelpers.node.net.CFChain", "CellFrame.chain.ChainAtomPtr")
                from ...chain import ChainAtomPtr
                self._chain = ChainAtomPtr()
        
        class CFWalletAddress:
            """Legacy CFWalletAddress class"""
            def __init__(self, *args, **kwargs):
                _show_legacy_warning("pycfhelpers.node.net.CFWalletAddress", "CellFrame.types.Address")
                from ...types import Address
                self._addr = Address(*args, **kwargs)
    
    class consensus:
        """Legacy consensus module"""
        
        class CFBlock:
            """Legacy CFBlock class"""
            def __init__(self, *args, **kwargs):
                _show_legacy_warning("pycfhelpers.node.consensus.CFBlock", "CellFrame.chain.Block")
        
        class CFEvent:
            """Legacy CFEvent class"""
            def __init__(self, *args, **kwargs):
                _show_legacy_warning("pycfhelpers.node.consensus.CFEvent", "CellFrame.chain.Event")
    
    class datums:
        """Legacy datums module"""
        
        class CFDatum:
            """Legacy CFDatum class"""
            def __init__(self, *args, **kwargs):
                _show_legacy_warning("pycfhelpers.node.datums.CFDatum", "CellFrame.chain.Datum")
    
    class logging:
        """Legacy logging module"""
        
        class CFLog:
            """Legacy CFLog class"""
            def __init__(self, *args, **kwargs):
                _show_legacy_warning("pycfhelpers.node.logging.CFLog", "Python logging module")
    
    class crypto:
        """Legacy crypto module"""
        
        class CFGUUID:
            """Legacy CFGUUID class"""
            def __init__(self, *args, **kwargs):
                _show_legacy_warning("pycfhelpers.node.crypto.CFGUUID", "uuid.UUID")
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
                _show_legacy_warning("DAP.Crypto.HashFast", "CellFrame.crypto.Hash")
        
        class Cert:
            """Legacy Cert class"""
            def __init__(self, *args, **kwargs):
                _show_legacy_warning("DAP.Crypto.Cert", "CellFrame.crypto.Certificate")
        
        class Sign:
            """Legacy Sign class"""
            def __init__(self, *args, **kwargs):
                _show_legacy_warning("DAP.Crypto.Sign", "CellFrame.crypto.Signature")
        
        class Pkey:
            """Legacy Pkey class"""
            def __init__(self, *args, **kwargs):
                _show_legacy_warning("DAP.Crypto.Pkey", "CellFrame.crypto.PublicKey")
        
        class GUUID:
            """Legacy GUUID class"""
            def __init__(self, *args, **kwargs):
                _show_legacy_warning("DAP.Crypto.GUUID", "uuid.UUID")
                import uuid
                self._uuid = uuid.uuid4()
    
    class Core:
        """Legacy core module"""
        
        class Math:
            """Legacy Math class"""
            def __init__(self):
                _show_legacy_warning("DAP.Core.Math", "Python math/decimal modules")
        
        @staticmethod
        def logIt(*args, **kwargs):
            """Legacy logging function"""
            _show_legacy_warning("DAP.Core.logIt", "Python logging module")
            import logging
            if args:
                logging.info(args[0] if len(args) == 1 else ' '.join(str(a) for a in args))
        
        class NodeAddr:
            """Legacy NodeAddr class"""
            def __init__(self):
                _show_legacy_warning("DAP.Core.NodeAddr", "CellFrame.types.Address")
        
        class AppContext:
            """Legacy AppContext class"""
            def __init__(self):
                _show_legacy_warning("DAP.Core.AppContext", "CellFrame.core.context.Context")
    
    class Network:
        """Legacy network module"""
        
        class ClusterRoles:
            """Legacy ClusterRoles enum"""
            def __init__(self):
                _show_legacy_warning("DAP.Network.ClusterRoles", "CellFrame.network.ClusterRoles")
        
        class Server:
            """Legacy Server class"""
            def __init__(self):
                _show_legacy_warning("DAP.Network.Server", "CellFrame.node.srv.Server")
        
        class HttpSimple:
            """Legacy HttpSimple class"""
            def __init__(self):
                _show_legacy_warning("DAP.Network.HttpSimple", "CellFrame.node.http.HttpHandler")
        
        class HttpCode:
            """Legacy HttpCode enum"""
            OK = 200
            NOT_FOUND = 404
            SERVER_ERROR = 500
            
            def __init__(self):
                _show_legacy_warning("DAP.Network.HttpCode", "http.HTTPStatus")
        
        class HttpHeader:
            """Legacy HttpHeader class"""
            def __init__(self):
                _show_legacy_warning("DAP.Network.HttpHeader", "http.client.HTTPMessage")
    
    class GlobalDB:
        """Legacy GlobalDB module"""
        
        class DB:
            """Legacy DB class"""
            def __init__(self):
                _show_legacy_warning("DAP.GlobalDB.DB", "CellFrame.gdb.Database")
        
        class Cluster:
            """Legacy Cluster class"""
            def __init__(self):
                _show_legacy_warning("DAP.GlobalDB.Cluster", "CellFrame.gdb.Cluster")
        
        class Instance:
            """Legacy Instance class"""
            def __init__(self):
                _show_legacy_warning("DAP.GlobalDB.Instance", "CellFrame.gdb.Instance")
        
        class MemberRoles:
            """Legacy MemberRoles enum"""
            def __init__(self):
                _show_legacy_warning("DAP.GlobalDB.MemberRoles", "CellFrame.gdb.MemberRoles")

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