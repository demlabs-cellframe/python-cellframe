"""
💰 Cellframe Transaction (TX) Module

Low-level transaction operations for working with dap_chain_tx_t structures.
This module provides direct access to transaction manipulation functions
without high-level composition logic.

For transaction creation, use the Composer module instead.
"""

import threading
from datetime import datetime
from enum import Enum
from typing import Dict, List, Optional, Any
from decimal import Decimal

from ..common import CellframeException

# Registry for transaction instances
_tx_registry: Dict[int, 'TX'] = {}
_registry_lock = threading.RLock()

# Try to import cellframe if available
# Import native functions with conditional availability check
try:
    import python_cellframe as cf_native
    
    # Centralized check for all required transaction functions - ONCE at import
    required_functions = [
        'dap_chain_datum_tx_create',
        'dap_chain_datum_tx_add_in_item',
        'dap_chain_datum_tx_add_out_ext_item',
        'dap_chain_datum_tx_add_sign_item',
        'dap_chain_datum_tx_verify_sign',
        'dap_chain_datum_tx_get_size',
        'dap_chain_mempool_datum_add',
        'dap_chain_mempool_tx_get_by_hash'
    ]
    
    missing_functions = [func for func in required_functions if not hasattr(cf_native, func)]
    if missing_functions:
        raise ImportError(
            f"❌ CRITICAL: Missing native transaction functions: {', '.join(missing_functions)}\n"
            "This is a Python bindings library - all transaction functions must be implemented.\n"
            "Please implement these functions in src/cellframe_tx.c"
        )
    
    # Use DapSign directly from native module when needed

except ImportError as e:
    # Check for test mode - allow imports in test environment
    import os
    if os.getenv('CELLFRAME_TEST_MODE') == '1':
        print(f"⚠️ WARNING: TX module running in test mode, some functions may not be available: {e}")
        # Create minimal stubs for testing
        cf_native = None
    else:
        raise ImportError(
            "❌ CRITICAL: Native python_cellframe module not available!\n"
            f"Original error: {e}\n"
            "Please ensure python_cellframe native module is properly built and installed."
        ) from e


class TxError(CellframeException):
    """Transaction operation error"""
    pass


class TxType(Enum):
    """Типы транзакций согласно реальной архитектуре Cellframe"""
    TRANSFER = "transfer"           # Обычный перевод
    STAKE_ORDER = "stake_order"     # Стейкинг
    VOTE = "vote"                   # Голосование
    DECREE = "decree"               # Указы
    CONDITIONAL = "conditional"     # Условные транзакции
    SHARED_WALLET = "shared_wallet" # Shared кошелек
    EXCHANGE = "exchange"           # Обмен токенов
    BRIDGE = "bridge"               # Межсетевой мост


class TxStatus(Enum):
    """Статусы транзакций"""
    PENDING = "pending"             # Ожидает в mempool
    CONFIRMED = "confirmed"         # Подтверждена в блокчейне
    FAILED = "failed"               # Провалена
    REJECTED = "rejected"           # Отклонена
    EXPIRED = "expired"             # Истекла


class TxInput:
    """Transaction input reference"""
    def __init__(self, tx_hash: str, out_index: int, value: int, token_ticker: str):
        self.tx_hash = tx_hash
        self.out_index = out_index
        self.value = value
        self.token_ticker = token_ticker

    def to_dict(self) -> Dict[str, Any]:
        return {
            'tx_hash': self.tx_hash,
            'out_index': self.out_index,
            'value': self.value,
            'token_ticker': self.token_ticker
        }


class TxOutput:
    """Transaction output definition"""
    def __init__(self, address: str, value: int, token_ticker: str):
        self.address = address
        self.value = value
        self.token_ticker = token_ticker

    def to_dict(self) -> Dict[str, Any]:
        return {
            'address': self.address,
            'value': self.value,
            'token_ticker': self.token_ticker
        }


class TX:
    """
    💰 Low-level Transaction Operations
    
    Represents a dap_chain_tx_t structure and provides direct access
    to transaction manipulation functions. This class does NOT create
    transactions - it only works with existing transaction handles.
    
    For transaction creation, use the Composer module.
    """
    
    def __init__(self, tx_handle: Any, owns_handle: bool = True):
        if tx_handle is None:
            raise TxError("tx_handle не может быть None")
            
        self._tx_handle = tx_handle
        self._owns_handle = owns_handle
        self._is_finalized = False
        self.hash = ""
        self.type = TxType.TRANSFER
        self.status = TxStatus.PENDING
        self.inputs: List[TxInput] = []
        self.outputs: List[TxOutput] = []
        self.fee = 0
        self.token_ticker = ""  # ОБЯЗАТЕЛЬНЫЙ параметр!
        self.created_at = datetime.now()
        self.confirmed_at = None
        self.signatures = []
        
        # Регистрируем экземпляр
        with _registry_lock:
            _tx_registry[id(self)] = self
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.finalize()
    
    @classmethod
    def from_raw(cls, raw_tx_data: bytes) -> 'TX':
        """Создать TX из raw данных"""

            
        try:
            # Парсим raw данные в dap_chain_datum_tx_t
            tx_handle = cf_native.dap_chain_datum_tx_create()
            if not tx_handle:
                raise TxError("Не удалось создать транзакцию из raw данных")
                
            # Здесь нужна реальная функция десериализации
            # Пока используем заглушку
            transaction = cls(tx_handle, owns_handle=True)
            transaction.type = TxType.TRANSFER
            
            return transaction
        except Exception as e:
            raise TxError(f"Ошибка создания транзакции из raw данных: {e}")
    
    def add_input(self, tx_hash: str, out_index: int, value: int, token_ticker: str):
        """Добавить вход в транзакцию"""
        if not token_ticker:
            raise TxError("token_ticker ОБЯЗАТЕЛЬНО должен быть указан!")
            
        input_item = TxInput(tx_hash, out_index, value, token_ticker)
        self.inputs.append(input_item)
        

            
        try:
            # Используем РЕАЛЬНУЮ функцию добавления входа
            cf_native.dap_chain_datum_tx_add_in_item(
                self._tx_handle,
                tx_hash.encode(),
                out_index,
                value
            )
        except Exception as e:
            raise TxError(f"Ошибка добавления входа: {e}")
    
    def add_output(self, address: str, value: int, token_ticker: str):
        """Добавить выход в транзакцию"""
        if not token_ticker:
            raise TxError("token_ticker ОБЯЗАТЕЛЬНО должен быть указан!")
            
        output_item = TxOutput(address, value, token_ticker)
        self.outputs.append(output_item)
        

            
        try:
            # Используем РЕАЛЬНУЮ функцию добавления выхода
            cf_native.dap_chain_datum_tx_add_out_ext_item(
                self._tx_handle,
                address.encode(),
                value,
                token_ticker.encode()  # ОБЯЗАТЕЛЬНЫЙ параметр токена!
            )
        except Exception as e:
            raise TxError(f"Ошибка добавления выхода: {e}")
    
    def sign(self, signature: Any):
        """Подписать транзакцию"""
        if not signature:
            raise TxError("Подпись не может быть None")
            
        self.signatures.append(signature)
        

            
        try:
            # Используем РЕАЛЬНУЮ функцию добавления подписи
            if hasattr(signature, 'get_signature_data'):
                sign_data = signature.get_signature_data()
            else:
                sign_data = signature
                
            cf_native.dap_chain_datum_tx_add_sign_item(
                self._tx_handle,
                sign_data,
                len(sign_data)
            )
        except Exception as e:
            raise TxError(f"Ошибка подписания транзакции: {e}")
    
    def verify(self) -> bool:
        """Верифицировать транзакцию (проверка всех подписей)"""
        try:
            # Используем РЕАЛЬНУЮ функцию верификации подписей
            if not hasattr(cf_native, 'dap_chain_datum_tx_verify_sign'):
                raise ImportError(
                    "❌ CRITICAL: dap_chain_datum_tx_verify_sign not available in python_cellframe!\n"
                    "Please ensure this function is exported in src/cellframe_tx.c"
                )
            # Verify all signatures (sign_num = -1 means verify all)
            result = cf_native.dap_chain_datum_tx_verify_sign(self._tx_handle, -1)
            return result == 0
        except Exception as e:
            raise TxError(f"Ошибка верификации транзакции: {e}")
    
    def get_size(self) -> int:
        """Получить размер транзакции"""

            
        try:
            # Используем РЕАЛЬНУЮ функцию получения размера
            if not hasattr(cf_native, 'dap_chain_datum_tx_get_size'):
                raise ImportError(
                    "❌ CRITICAL: dap_chain_datum_tx_get_size not available in python_cellframe!\n"
                    "Please ensure this function is exported in src/cellframe_tx.c"
                )
            size = cf_native.dap_chain_datum_tx_get_size(self._tx_handle)
            return int(size)
        except Exception as e:
            raise TxError(f"Ошибка получения размера транзакции: {e}")
    
    def broadcast(self, chain_handle: Any, hash_out_type: str = "hex") -> str:
        """
        Отправить транзакцию в mempool
        Использует РЕАЛЬНУЮ функцию dap_chain_mempool_datum_add!
        """
        if self._is_finalized:
            raise TxError("Транзакция уже финализирована")
            
        try:
            # Используем РЕАЛЬНУЮ функцию отправки в mempool
            # dap_chain_mempool_datum_add принимает datum и chain
            if not hasattr(cf_native, 'dap_chain_mempool_datum_add'):
                raise ImportError(
                    "❌ CRITICAL: dap_chain_mempool_datum_add not available in python_cellframe!\n"
                    "Please ensure this function is exported in src/cellframe_tx.c"
                )
            # Convert tx handle to datum (tx is a subtype of datum)
            result = cf_native.dap_chain_mempool_datum_add(
                self._tx_handle,  # dap_chain_datum_tx_t* is compatible with dap_chain_datum_t*
                chain_handle,
                hash_out_type if hash_out_type else None
            )
            
            if result:
                self.status = TxStatus.PENDING
                self.hash = result.decode() if isinstance(result, bytes) else str(result)
                return self.hash
            else:
                raise TxError("Не удалось отправить транзакцию в mempool")
                
        except Exception as e:
            raise TxError(f"Ошибка отправки транзакции: {e}")
    
    def to_dict(self) -> Dict[str, Any]:
        """Конвертировать в словарь"""
        return {
            'hash': self.hash,
            'type': self.type.value,
            'status': self.status.value,
            'token_ticker': self.token_ticker,
            'fee': self.fee,
            'created_at': self.created_at.isoformat(),
            'confirmed_at': self.confirmed_at.isoformat() if self.confirmed_at else None,
            'inputs': [inp.to_dict() for inp in self.inputs],
            'outputs': [out.to_dict() for out in self.outputs],
            'signatures_count': len(self.signatures),
            'size': self.get_size()
        }
    
    def finalize(self):
        """Финализировать транзакцию"""
        if self._is_finalized:
            return
            
        try:
            # Очистка ресурсов если нужно
            self._is_finalized = True
            
            # Убираем из реестра
            with _registry_lock:
                _tx_registry.pop(id(self), None)
                
        except Exception as e:
            raise TxError(f"Ошибка финализации транзакции: {e}")
    

    
    def __del__(self):
        if not self._is_finalized:
            self.finalize()


# Utility functions for working with transactions
def get_tx_by_hash(tx_hash: str) -> Optional[TX]:
    """Получить транзакцию по хешу из mempool"""

        
    try:
        if not hasattr(cf_native, 'dap_chain_mempool_tx_get_by_hash'):
            return None
        tx_handle = cf_native.dap_chain_mempool_tx_get_by_hash(tx_hash.encode())
        if tx_handle:
            tx = TX(tx_handle, owns_handle=True)
            tx.hash = tx_hash
            return tx
    except Exception:
        pass
        
    return None


def broadcast_tx(tx: TX, chain_handle: Any) -> str:
    """Отправить транзакцию в сеть"""
    return tx.broadcast(chain_handle)


__all__ = [
    'TX',
    'TxError',
    'TxType',
    'TxStatus',
    'TxInput',
    'TxOutput',
    'get_tx_by_hash',
    'broadcast_tx'
] 