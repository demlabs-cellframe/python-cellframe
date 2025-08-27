"""
Common types used across CellFrame SDK
"""

from enum import Enum
from typing import Union, Optional, Dict, Any


# Predefined consensus types (just strings for flexibility)
CONSENSUS_BITCOIN = "bitcoin"
CONSENSUS_ETHEREUM = "ethereum" 
CONSENSUS_CELLFRAME = "cellframe"
CONSENSUS_DAG = "dag"
CONSENSUS_POW = "pow"
CONSENSUS_POS = "pos"
CONSENSUS_STAKE_POS = "stake_pos"
CONSENSUS_ESBOCS = "esbocs"
CONSENSUS_DAG_POA = "dag_poa"

# Type alias for consensus names - just a string
ConsensusName = str


class TSD:
    """
    Types of Typed section data.

    Attributes:
        TYPE_UNKNOWN: Unknown type.
        TYPE_TIMESTAMP: Timestamp type.
        TYPE_ADDRESS: Address type.
        TYPE_VALUE: Value type.
        TYPE_CONTRACT: Contract type.
        TYPE_NET_ID: Network ID type.
        TYPE_BLOCK_NUM: Block number type.
        TYPE_TOKEN_SYM: Token symbol type.
        TYPE_OUTER_TX_HASH: Transaction output hash type.
        TYPE_SOURCE: Source type
        TYPE_SOURCE_SUBTYPE: Source subtype type.
        TYPE_DATA: Data type.
        TYPE_SENDER: Sender type.
        TYPE_TOKEN_ADDRESS: Token adress type.
        TYPE_SIGNATURS: Signaturs type.
        TYPE_UNIQUE_ID: Unique ID type.
        TYPE_BASE_TX_HASH: Base trancaction hash type.
        TYPE_EMISSION_CENTER_UID: type of the unique identifier of
            the emission center.
        TYPE_EMISSION_CENTER_VER: type of the verificator/version of
            the emission center.
    """

    TYPE_UNKNOWN = 0x0000
    TYPE_TIMESTAMP = 0x0001
    TYPE_ADDRESS = 0x0002
    TYPE_VALUE = 0x0003
    TYPE_CONTRACT = 0x0004
    TYPE_NET_ID = 0x0005
    TYPE_BLOCK_NUM = 0x0006
    TYPE_TOKEN_SYM = 0x0007
    TYPE_OUTER_TX_HASH = 0x0008
    TYPE_SOURCE = 0x0009
    TYPE_SOURCE_SUBTYPE = 0x000A
    TYPE_DATA = 0x000B
    TYPE_SENDER = 0x000C
    TYPE_TOKEN_ADDRESS = 0x000D
    TYPE_SIGNATURS = 0x000E
    TYPE_UNIQUE_ID = 0x000F
    TYPE_BASE_TX_HASH = 0x0010
    TYPE_EMISSION_CENTER_UID = 0x0011
    TYPE_EMISSION_CENTER_VER = 0x0012


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
