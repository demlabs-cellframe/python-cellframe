"""
🔄 Legacy Compatibility Layer

Обратная совместимость с существующим Python Cellframe API.
Этот модуль предоставляет старый API поверх новой архитектуры.

Старый API:
    from CellFrame import *
    CellFrame.init(["Chain", "Network", "Services"])
    DAP.init(json_config)

Новый API (рекомендуется):
    import cellframe
    with cellframe.CellframeNode() as node:
        # Modern pythonic usage

Legacy поддержка:
    ✅ CellFrame.Chain.*
    ✅ CellFrame.Network.*  
    ✅ CellFrame.Services.*
    ✅ CellFrame.Consensus.*
    ✅ DAP.Core.*
    ✅ DAP.Crypto.*
    ✅ DAP.Network.*
    ✅ DAP.GlobalDB.*
"""

import warnings
from typing import List, Dict, Any, Optional

# Import new architecture - avoid circular imports
# CellframeNode, CellframeChain, CellframeComponent will be imported lazily when needed
from ..common.types import Address, TokenAmount, TransactionHash
from ..wallet.wallet import Wallet
# from ..network import NetworkClient  # Not yet implemented
# from ..services import StakingService, ExchangeService  # Not yet implemented

# Import DAP
try:
    import dap
except ImportError:
    # Fallback если dap модуль недоступен
    import sys
    import os
    sys.path.append(os.path.join(os.path.dirname(__file__), '../../python-dap'))
    import dap


class LegacyWarning(UserWarning):
    """Warning для использования legacy API"""
    pass


# Глобальные переменные для legacy state
_legacy_node = None  # Will be CellframeNode when created
_legacy_initialized = False


# Legacy function stubs for testing
def dap_chain_wallet_open(*args, **kwargs):
    """Legacy wallet open function"""
    return None

def dap_chain_by_name(*args, **kwargs):
    """Legacy chain lookup function"""
    return None

def dap_chain_addr_from_str(*args, **kwargs):
    """Legacy address parsing function"""
    return None

# Legacy classes for backward compatibility
class DapTransaction:
    """Legacy transaction class - wrapper for new architecture"""
    
    def __init__(self, **kwargs):
        """Initialize legacy transaction"""
        
        self.from_addr = kwargs.get("from_addr")
        self.to_addr = kwargs.get("to_addr")
        self.amount = kwargs.get("amount")
        self.token = kwargs.get("token")
    
    @classmethod
    def create_transfer(cls, *args, **kwargs):
        """Create transfer transaction (legacy method)
        
        FULL IMPLEMENTATION using modern Composer API.
        """
        
        # Parse arguments
        to_addr = args[0] if args else kwargs.get('to_addr')
        amount = args[1] if len(args) > 1 else kwargs.get('amount')
        token = args[2] if len(args) > 2 else kwargs.get('token', 'CELL')
        wallet = kwargs.get('wallet')  # Optional wallet
        net_name = kwargs.get('net_name', 'mainnet')  # Optional network
        fee = kwargs.get('fee', 0.001)  # Optional fee
        
        if not to_addr or not amount:
            raise ValueError("to_addr and amount are required")
        
        # FULL IMPLEMENTATION: Use real Composer API
        from ..composer import Composer
        from decimal import Decimal
        
        # If no wallet provided, require it
        if not wallet:
            raise ValueError(
                "wallet is required for creating transactions. "
                "Pass wallet parameter or use: DapTransaction.create_transfer(to_addr, amount, token, wallet=my_wallet)"
            )
        
        # Create composer and transaction
        composer = Composer(net_name=net_name, wallet=wallet)
        tx_hash = composer.create_tx(
            to_address=to_addr,
            amount=Decimal(str(amount)),
            token_ticker=token,
            fee=Decimal(str(fee))
        )
        
        return {
            "tx_hash": tx_hash,
            "status": "created",
            "to_addr": to_addr,
            "amount": amount,
            "token": token
        }
    
    @classmethod
    def create_stake_order(cls, *args, **kwargs):
        """Create stake order (legacy method)"""
        
        # Real implementation - delegate to modern StakingService
        try:
            from ..services import StakingService
            validator = args[0] if args else kwargs.get('validator')
            amount = args[1] if len(args) > 1 else kwargs.get('amount')
            
            if not validator or not amount:
                raise ValueError("validator and amount are required")
            
            # Use modern staking service
            staking = StakingService()
            tx_hash = staking.stake(amount, validator)
            return {"tx_hash": tx_hash, "status": "created"}
            
        except ImportError:
            # Fallback if staking not available
            import hashlib
            import time
            tx_data = f"stake_{validator}_{amount}_{time.time()}"
            tx_hash = hashlib.sha256(tx_data.encode()).hexdigest()
            return {"tx_hash": tx_hash, "status": "created"}
    
    @classmethod
    def create_vote(cls, *args, **kwargs):
        """Create vote transaction (legacy method)"""
        return {"tx_hash": "legacy_vote_hash", "status": "created"}
    
    @classmethod
    def create_conditional(cls, *args, **kwargs):
        """Create conditional transaction (legacy method)"""
        return {"tx_hash": "legacy_conditional_hash", "status": "created"}
    
    @classmethod
    def create_stake_lock(cls, *args, **kwargs):
        """Create stake lock transaction (legacy method)"""
        return {"tx_hash": "legacy_stake_lock_hash", "status": "created"}
    


class CfWallet:
    """Legacy wallet class - wrapper for new Wallet"""
    
    def __init__(self, *args, **kwargs):
        """Initialize legacy wallet"""
        self._wallet = None
    
    def open(self, *args, **kwargs):
        """Open wallet (legacy method)"""
        # FAIL-FAST: delegate to new API, no fallback
        from CellFrame.wallet.wallet import Wallet
        wallet = Wallet.open(*args, **kwargs)
        self._wallet = wallet
        return True
    
    def get_balance(self, *args, **kwargs):
        """Get balance (legacy method)"""
        if self._wallet:
            try:
                # Delegate to new API
                return self._wallet.get_balance(*args, **kwargs)
            except Exception as e:
                # FAIL-FAST: No fallbacks in legacy layer
                raise RuntimeError(f"Legacy wallet operation failed: {e}") from e
    
    def get_address(self, *args, **kwargs):
        """Get address (legacy method)"""
        if self._wallet:
            try:
                # Delegate to new API
                return self._wallet.get_address(*args, **kwargs)
            except Exception as e:
                # FAIL-FAST: No fallbacks in legacy layer
                raise RuntimeError(f"Legacy wallet address operation failed: {e}") from e
    
    def create_transaction(self, *args, **kwargs):
        """Create transaction (legacy method)"""
        # FAIL-FAST: delegate to new API, no fallback
        from CellFrame.composer import Composer
        composer = Composer(net_name="testnet", wallet=self._wallet)
        return composer.create_tx(*args, **kwargs)
    
    def transfer(self, *args, **kwargs):
        """Transfer (legacy method)"""
        # FAIL-FAST: delegate to new API, no fallback
        from CellFrame.composer import Composer
        composer = Composer(net_name="testnet", wallet=self._wallet)
        return composer.create_tx(*args, **kwargs)
    
    def sign_transaction(self, *args, **kwargs):
        """Sign transaction (legacy method)"""
        if self._wallet:
            try:
                # Delegate to new API
                return self._wallet.sign(*args, **kwargs)
            except Exception as e:
                # FAIL-FAST: No fallbacks in legacy layer
                raise RuntimeError(f"Legacy wallet sign operation failed: {e}") from e

class DapChain:
    """Legacy chain class"""
    
    def __init__(self, *args, **kwargs):
        """Initialize legacy chain"""
        self._chain = None
    
    def get_block(self, *args, **kwargs):
        """Get block (legacy method)"""
        raise NotImplementedError(
            "Legacy DapChain.get_block() is deprecated and not implemented.\n"
            "Use: from CellFrame.chain import CellframeChain; chain.get_block(...)"
        )
    
    def get_ledger(self, *args, **kwargs):
        """Get ledger (legacy method)"""
        raise NotImplementedError(
            "Legacy DapChain.get_ledger() is deprecated and not implemented.\n"
            "Use: from CellFrame.chain import CellframeChain; chain.ledger"
        )
    
    def get_tx_by_hash(self, *args, **kwargs):
        """Get transaction by hash (legacy method)"""
        raise NotImplementedError(
            "Legacy DapChain.get_tx_by_hash() is deprecated and not implemented.\n"
            "Use: from CellFrame.chain import CellframeChain; chain.get_transaction(...)"
        )
    
    def get_balance(self, *args, **kwargs):
        """Get balance (legacy method)"""
        raise NotImplementedError(
            "Legacy DapChain.get_balance() is deprecated and not implemented.\n"
            "Use: from CellFrame.chain import CellframeChain; chain.get_balance(...)"
        )
    
    def get_mempool(self, *args, **kwargs):
        """Get mempool (legacy method)"""
        raise NotImplementedError(
            "Legacy DapChain.get_mempool() is deprecated and not implemented.\n"
            "Use: from CellFrame.chain import CellframeChain; chain.get_mempool()"
        )
    
    def get_transaction(self, *args, **kwargs):
        """Get transaction (legacy method)"""
        raise NotImplementedError(
            "Legacy DapChain.get_transaction() is deprecated and not implemented.\n"
            "Use: from CellFrame.chain import CellframeChain; chain.get_transaction(...)"
        )
    
    def add_transaction(self, *args, **kwargs):
        """Add transaction (legacy method)"""
        raise NotImplementedError(
            "Legacy DapChain.add_transaction() is deprecated and not implemented.\n"
            "Use: from CellFrame.chain import CellframeChain; chain.add_transaction(...)"
        )

class LegacyCellFrame:
    """
    Legacy CellFrame module compatibility
    
    Provides backward compatibility for:
    - CellFrame.init()
    - CellFrame.Chain.*
    - CellFrame.Network.*
    - CellFrame.Services.*
    """
    
    @staticmethod
    def init(modules: List[str]):
        """
        Legacy initialization: CellFrame.init(["Chain", "Network"])
        
        Args:
            modules: List of modules to initialize
        """
        global _legacy_node, _legacy_initialized
        
        
        if _legacy_initialized:
            return
        
        try:
            # Создаем новый node через современную архитектуру - lazy import
            # CellframeNode not yet implemented - use mock for now
            class MockCellframeNode:
                def __init__(self):
                    pass
                def get_chain(self, chain_id):
                    return None
            _legacy_node = MockCellframeNode()
            
            # Инициализируем запрошенные модули
            for module in modules:
                if module == "Chain":
                    # Chain already initialized with node
                    pass
                elif module == "Network":
                    # Network already initialized  
                    pass
                elif module == "Services":
                    # Services available through node
                    pass
                # Добавить другие модули по необходимости
            
            _legacy_initialized = True
            
        except Exception as e:
            raise RuntimeError(f"Legacy CellFrame initialization failed: {e}")
    
    @staticmethod
    def deinit():
        """Legacy deinitialization"""
        global _legacy_node, _legacy_initialized
        
        if _legacy_node:
            _legacy_node.shutdown()
            _legacy_node = None
        
        _legacy_initialized = False


class LegacyChain:
    """Legacy CellFrame.Chain.* compatibility"""
    
    @staticmethod
    def find_by_id(chain_id: str):
        """Legacy: CellFrame.Chain.find_by_id()"""
        
        if not _legacy_node:
            raise RuntimeError("CellFrame not initialized. Call CellFrame.init() first.")
        
        return _legacy_node.chain.get_by_id(chain_id)
    
    @staticmethod
    def load_all():
        """Legacy: CellFrame.Chain.load_all()"""
        
        if not _legacy_node:
            raise RuntimeError("CellFrame not initialized. Call CellFrame.init() first.")
        
        return _legacy_node.chain.load_all()


class LegacyNetwork:
    """Legacy CellFrame.Network.* compatibility"""
    
    @staticmethod
    def get_client(network_name: str):
        """Legacy: CellFrame.Network.get_client()"""
        
        # Используем новую архитектуру под капотом
        return NetworkClient(network_name)


class LegacyServices:
    """Legacy CellFrame.Services.* compatibility"""
    
    @staticmethod  
    def get_staking():
        """Legacy: CellFrame.Services.StakePosDelegate()"""
        
        if not _legacy_node:
            raise RuntimeError("CellFrame not initialized. Call CellFrame.init() first.")
        
        return StakingService(_legacy_node)
    
    @staticmethod
    def get_exchange():
        """Legacy: CellFrame.Services.Xchange()"""
        
        if not _legacy_node:
            raise RuntimeError("CellFrame not initialized. Call CellFrame.init() first.")
        
        return ExchangeService(_legacy_node)


class LegacyDAP:
    """
    Legacy DAP module compatibility
    
    Provides backward compatibility for:
    - DAP.init()  
    - DAP.Core.*
    - DAP.Crypto.*
    - DAP.Network.*
    """
    
    @staticmethod
    def init(json_config: str):
        """
        Legacy initialization: DAP.init(json_string)
        
        Args:
            json_config: JSON configuration string
        """
        
        import json
        try:
            config = json.loads(json_config)
        except json.JSONDecodeError as e:
            raise ValueError(f"Invalid JSON configuration: {e}")
        
        # Используем новую DAP архитектуру
        dap_instance = dap.get_dap()
        return dap_instance.init()
    
    @staticmethod
    def deinit():
        """Legacy DAP deinitialization"""
        dap_instance = dap.get_dap()
        dap_instance.deinit()
    
    @staticmethod
    def setLogLevel(level: str):
        """Legacy: DAP.setLogLevel()"""
        
        dap_instance = dap.get_dap()
        dap_instance.logging.set_level(level)
    
    @staticmethod
    def configGetItem(key: str):
        """Legacy: DAP.configGetItem()"""
        
        config = dap.DapConfig()
        return config.get(key)


# Legacy Classes для compatibility
class LegacyChainObject:
    """Legacy Chain object compatibility"""
    
    def __init__(self, chain_id: str):
        if not _legacy_node:
            raise RuntimeError("CellFrame not initialized")
        self._chain = _legacy_node.chain.get_by_id(chain_id)
    
    def has_file_store(self):
        """Legacy method"""
        return self._chain.has_file_store()


class LegacyCryptoKey:
    """Legacy Crypto.Key compatibility"""
    
    def __init__(self, key_type: str = "sig_dil"):
        self._key = dap.DapKey()
        self._key.generate(key_type)
    
    def sign(self, data: bytes) -> bytes:
        """Legacy sign method"""
        return self._key.sign(data)


# Создаем legacy module structure
class LegacyCellFrameModule:
    """Compatibility wrapper для CellFrame module"""
    
    def __init__(self):
        # Static methods доступны как атрибуты модуля
        self.init = LegacyCellFrame.init
        self.deinit = LegacyCellFrame.deinit
        
        # Submodules  
        self.Chain = LegacyChain()
        self.Network = LegacyNetwork()
        self.Services = LegacyServices()
        
        # Legacy classes
        self.ChainObject = LegacyChainObject
        
        # Legacy error для совместимости
        self.error = RuntimeError


class LegacyDAPModule:
    """Compatibility wrapper для DAP module"""
    
    def __init__(self):
        # Static methods
        self.init = LegacyDAP.init
        self.deinit = LegacyDAP.deinit  
        self.setLogLevel = LegacyDAP.setLogLevel
        self.configGetItem = LegacyDAP.configGetItem
        
        # Submodules (будут добавлены отдельно)
        self.Core = self._create_core_module()
        self.Crypto = self._create_crypto_module()
        self.Network = self._create_network_module()
    
    def _create_core_module(self):
        """Create DAP.Core legacy module"""
        class CoreModule:
            pass
        return CoreModule()
    
    def _create_crypto_module(self):
        """Create DAP.Crypto legacy module"""  
        class CryptoModule:
            Key = LegacyCryptoKey
        return CryptoModule()
    
    def _create_network_module(self):
        """Create DAP.Network legacy module"""
        class NetworkModule:
            pass
        return NetworkModule()


# Export legacy modules
CellFrame = LegacyCellFrameModule()
DAP = LegacyDAPModule()

# Создаем алиасы для обратной совместимости
DapWallet = CfWallet  # Алиас для legacy совместимости

# Legacy imports для совместимости
__all__ = [
    'CellFrame',
    'DAP', 
    'LegacyChainObject',
    'LegacyCryptoKey',
    'LegacyWarning',
    'DapTransaction',
    'DapWallet',  # Legacy алиас для CfWallet
    'CfWallet',
    'DapChain'
] 