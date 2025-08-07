"""
⚙️ CellFrame Common Module

Common data structures and utilities used across CellFrame SDK.
"""

from dataclasses import dataclass
from typing import Any, Dict, List, Optional, Union
from datetime import datetime
from decimal import Decimal
from enum import Enum

# Common data structures

@dataclass
class Datum:
    """Base datum class"""
    type: str
    size: int
    data: bytes
    hash: Optional[str] = None
    
    def serialize(self) -> bytes:
        """Serialize datum to bytes"""
        return self.data
    
    @classmethod
    def deserialize(cls, data: bytes) -> 'Datum':
        """Deserialize datum from bytes"""
        return cls(type="generic", size=len(data), data=data)


@dataclass
class DatumTx:
    """Transaction datum"""
    version: int
    items: List[Any]
    hash: str
    timestamp: datetime
    
    def add_item(self, item: Any):
        """Add item to transaction"""
        self.items.append(item)
    
    def calculate_hash(self) -> str:
        """Calculate transaction hash"""
        # Simplified hash calculation
        import hashlib
        data = f"{self.version}:{len(self.items)}:{self.timestamp}"
        return hashlib.sha256(data.encode()).hexdigest()


@dataclass
class DatumToken:
    """Token datum"""
    ticker: str
    type: str
    total_supply: Decimal
    decimals: int
    flags: int = 0
    metadata: Optional[Dict[str, Any]] = None
    
    @property
    def is_native(self) -> bool:
        """Check if token is native"""
        return self.flags & 0x01 != 0
    
    @property
    def is_transferable(self) -> bool:
        """Check if token is transferable"""
        return self.flags & 0x02 != 0


@dataclass
class DatumDecree:
    """Decree datum"""
    version: int
    type: str
    params: Dict[str, Any]
    timestamp: datetime
    signature: Optional[bytes] = None
    
    def is_valid(self) -> bool:
        """Check if decree is valid"""
        return self.signature is not None and len(self.signature) > 0


@dataclass
class DatumAnchor:
    """Anchor datum for cross-chain references"""
    chain_id: str
    block_hash: str
    block_height: int
    tx_hash: str
    timestamp: datetime
    
    def to_string(self) -> str:
        """Convert anchor to string representation"""
        return f"{self.chain_id}:{self.block_hash}:{self.tx_hash}"


@dataclass
class DatumEmission:
    """Token emission datum"""
    version: int
    type: str
    token: str
    amount: Decimal
    address: str
    comment: Optional[str] = None
    
    @property
    def is_stake_emission(self) -> bool:
        """Check if this is stake emission"""
        return self.type == "stake"
    
    @property
    def is_base_emission(self) -> bool:
        """Check if this is base emission"""
        return self.type == "base"


class TxTSD:
    """Transaction time-stamped data"""
    
    def __init__(self):
        self.data = {}
        self.timestamp = datetime.now()
    
    def add(self, key: str, value: Any):
        """Add data to TSD"""
        self.data[key] = {
            'value': value,
            'timestamp': datetime.now()
        }
    
    def get(self, key: str) -> Optional[Any]:
        """Get data from TSD"""
        if key in self.data:
            return self.data[key]['value']
        return None
    
    def has(self, key: str) -> bool:
        """Check if key exists in TSD"""
        return key in self.data
    
    def remove(self, key: str) -> bool:
        """Remove data from TSD"""
        if key in self.data:
            del self.data[key]
            return True
        return False
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert TSD to dictionary"""
        return {
            'timestamp': self.timestamp.isoformat(),
            'data': self.data
        }


# Common exceptions
class CommonError(Exception):
    """Base exception for common module"""
    pass


class ValidationError(CommonError):
    """Validation error"""
    pass


class SerializationError(CommonError):
    """Serialization/deserialization error"""
    pass


# Import and setup logger
import logging
logger = logging.getLogger('CellFrame')

# Import types
from .types import *

# Export all common types
__all__ = [
    'Datum',
    'DatumTx',
    'DatumToken',
    'DatumDecree',
    'DatumAnchor',
    'DatumEmission',
    'TxTSD',
    'CommonError',
    'ValidationError',
    'SerializationError',
    'logger',
    # Re-export types
    'ChainTypes',
    'NetTypes',
    'TxTypes',
    'DatumTypes',
    'CryptoKeyTypes',
    'HashTypes',
    'Address',
    'TxHash',
    'BlockHash',
    'Amount',
    'Timestamp'
]
