from typing import NewType, TypedDict
from .Common import TxIn, TxInCond, TxOut, TxOutCond, TxPkey, TxSig, TxToken, TxReceipt, TxOutExt, TxTSD

ticker = NewType("ticker", str)  # --> token name

ItemTypes = TxIn | TxInCond | TxOut | TxOutCond | TxPkey | TxSig | TxToken | TxReceipt | TxOutExt | TxTSD


class TSD(TypedDict):
    type: int
    value: bytes


class SignCheck(TypedDict):
    VERIFY: int
    NOVERIFY: int
