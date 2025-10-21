"""
Common types used across CellFrame SDK
"""

from enum import Enum
from typing import Union, Optional, Dict, Any

# Import ConsensusName from core.types
ConsensusName = str




class NetTypes(Enum):
    """Network types"""
    MAINNET = "mainnet"
    TESTNET = "testnet"
    DEVNET = "devnet"
    REGTEST = "regtest"
    
    def __str__(self) -> str:
        return self.value


class TxTypes(Enum):
    """Transaction types"""
    TRANSFER = "transfer"
    STAKE = "stake"
    UNSTAKE = "unstake"
    DELEGATE = "delegate"
    UNDELEGATE = "undelegate"
    TOKEN_CREATE = "token_create"
    TOKEN_BURN = "token_burn"
    CONTRACT_CALL = "contract_call"
    CONTRACT_DEPLOY = "contract_deploy"
    
    def __str__(self) -> str:
        return self.value


class DatumTypes(Enum):
    """Datum types"""
    TX = "tx"
    TOKEN = "token"
    EMISSION = "emission"
    DECREE = "decree"
    ANCHOR = "anchor"
    CUSTOM = "custom"
    
    def __str__(self) -> str:
        return self.value


class CryptoKeyTypes(Enum):
    """Cryptographic key types"""
    DILITHIUM = "sig_dilithium"
    FALCON = "sig_falcon"
    PICNIC = "sig_picnic"
    TESLA = "sig_tesla"
    BLISS = "sig_bliss"
    ECDSA = "sig_ecdsa"
    
    def __str__(self) -> str:
        return self.value


class HashTypes(Enum):
    """Hash algorithm types"""
    SHA256 = "sha256"
    SHA3_256 = "sha3_256"
    BLAKE2B = "blake2b"
    KECCAK256 = "keccak256"
    
    def __str__(self) -> str:
        return self.value


# Type aliases for common uses
Address = str
TxHash = str
BlockHash = str
Amount = Union[int, float, str]
Timestamp = Union[int, float]


class ItemTypes(Enum):
    """Transaction item types"""
    TX_IN = "tx_in"
    TX_IN_COND = "tx_in_cond"
    TX_OUT = "tx_out"
    TX_OUT_COND = "tx_out_cond"
    TX_PKEY = "tx_pkey"
    TX_SIG = "tx_sig"
    TX_TOKEN = "tx_token"
    TX_RECEIPT = "tx_receipt"
    TX_OUT_EXT = "tx_out_ext"
    TX_TSD = "tx_tsd"
    
    def __str__(self) -> str:
        return self.value


# Item types values as a separate type for compatibility
ItemTypesValues = {
    "TX_IN": 0x10,
    "TX_IN_COND": 0x11,
    "TX_OUT": 0x20,
    "TX_OUT_COND": 0x21,
    "TX_PKEY": 0x30,
    "TX_SIG": 0x40,
    "TX_TOKEN": 0x50,
    "TX_RECEIPT": 0x60,
    "TX_OUT_EXT": 0x22,
    "TX_TSD": 0x70
}


# Export all types
__all__ = [
    'ConsensusName',
    'NetTypes',
    'TxTypes',
    'DatumTypes',
    'CryptoKeyTypes',
    'HashTypes',
    'ItemTypes',
    'ItemTypesValues',
    'Address',
    'TxHash',
    'BlockHash',
    'Amount',
    'Timestamp'
]
