from enum import Enum


# from datetime import datetime

class MixinHasValue:
    _value2member_map_ = None

    @classmethod
    def has_value(cls, value) -> bool:
        """
        check value in Enum
        example: ExampleEnum.has_value("test-string")
        """
        return value in cls._value2member_map_


# For backward compatibility, keep the enum-like interface
class ChainTypes(MixinHasValue, str, Enum):
    esbocs = "esbocs"
    dag_poa = "dag_poa"


class DatumTypes(MixinHasValue, str, Enum):
    DATUM_TX = "DATUM_TX"
    DATUM_TOKEN_DECL = "DATUM_TOKEN_DECL"
    DATUM_TOKEN_EMISSION = "DATUM_TOKEN_EMISSION"
    DATUM_CUSTOM = "DATUM_CUSTOM"
    DATUM_DECREE = "DATUM_DECREE"
    DATUM_ANCHOR = "DATUM_ANCHOR"


class ItemTypes(MixinHasValue, str, Enum):
    TX_ITEM_TYPE_IN = "TX_ITEM_TYPE_IN"
    TX_ITEM_TYPE_IN_COND = "TX_ITEM_TYPE_IN_COND"
    TX_ITEM_TYPE_OUT = "TX_ITEM_TYPE_OUT"
    TX_ITEM_TYPE_OUT_COND = "TX_ITEM_TYPE_OUT_COND"
    TX_ITEM_TYPE_PKEY = "TX_ITEM_TYPE_PKEY"
    TX_ITEM_TYPE_SIG = "TX_ITEM_TYPE_SIG"
    TX_ITEM_TYPE_IN_EMS = "TX_ITEM_TYPE_IN_EMS"  # TxToken
    TX_ITEM_TYPE_RECEIPT = "TX_ITEM_TYPE_RECEIPT"
    TX_ITEM_TYPE_OUT_EXT = "TX_ITEM_TYPE_OUT_EXT"
    TX_ITEM_TYPE_TSD = "TX_ITEM_TYPE_TSD"
    DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY = "DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY"
    DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK = "DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK"
    DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE = "DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE"
    DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE = "DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE"
