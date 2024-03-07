from typing import Protocol, Any, Literal
from datetime import datetime

from DAP.Crypto import HashFast, Sign, Cert, SignType, Pkey
from DAP.Core import Math
from .Chain import ChainID, ChainAddr, Chain
from .Network import NetID, Net, ServiceUID
from .types import ticker, TSD, SignCheck, ItemTypes


# DapChainDatumTypeIdObjectType
class DatumTypeID(Protocol):
    # no methods
    pass


# DapChainDatumObjectType
class Datum(Protocol):
    @property
    def hash(self) -> HashFast:
        pass

    @property
    def versionStr(self) -> str:
        pass

    @property
    def tsCreated(self) -> datetime:
        pass

    @property
    def raw(self) -> bytes:
        pass

    @property
    def dataRaw(self) -> bytes:
        pass

    def getSize(self) -> int:
        pass

    def isDatumTX(self) -> bool:
        pass

    def getDatumTX(self) -> DatumTx:
        pass

    def isDatumToken(self) -> bool:
        pass

    def getDatumToken(self) -> DatumToken:
        pass

    def isDatumTokenEmission(self) -> bool:
        pass

    def getDatumTokenEmission(self) -> DatumEmission:
        pass

    def isDatumCustom(self) -> bool:
        # get data from dataRaw()
        pass

    def isDatumDecree(self) -> bool:
        pass

    def getDatumDecree(self) -> DatumDecree:
        pass

    def isDatumAnchor(self) -> bool:
        pass

    def getDatumAnchor(self) -> DatumAnchor:
        pass

    def getTypeStr(self) -> Literal[
        "DATUM_TX", "DATUM_TOKEN_DECL", "DATUM_TOKEN_EMISSION",
        "DATUM_CUSTOM", "DATUM_DECREE", "DATUM_ANCHOR"
    ]:
        pass

    def getTypeId(self) -> int:
        pass


# DapChainDatumIterObjectType
# class DatumIter(Protocol):
#     # no methods
#     pass


# DapChainDatumTokenObjectType
class DatumToken(Protocol):
    @property
    def ticker(self) -> ticker:  # token name
        pass

    @property
    def typeStr(self) -> Literal[
        "SIMPLE", "PRIVATE_DECL", "CF20_DECL", "PUBLIC",
        "PRIVATE_UPDATE", "CF20_UPDATE", "UNKNOWN"
    ]:
        pass

    @property
    def data(self) -> dict[str, Any]:
        pass

    @property
    def signs(self) -> list[Sign]:
        pass


# DapChainTxTokenExtType
class DatumTokenExt(Protocol):
    @property
    def version(self) -> int:
        pass

    @property
    def ticker(self) -> ticker:  # token name
        pass

    @property
    def chainId(self) -> ChainID:  # str(ChainID)
        pass

    @property
    def netId(self) -> NetID:
        pass

    @property
    def txHash(self) -> HashFast:
        pass

    @property
    def txOutIdx(self) -> int:
        pass


# DapChainDatumTokenEmissionObjectType
class DatumEmission(Protocol):
    @property
    def hash(self) -> HashFast:
        pass

    @property
    def version(self) -> int:
        pass

    @property
    def typeStr(self) -> Literal[
        "TOKEN_EMISSION_TYPE_UNDEFINED", "TOKEN_EMISSION_TYPE_AUTH", "TOKEN_EMISSION_TYPE_ALGO",
        "TOKEN_EMISSION_TYPE_ATOM_OWNER", "TOKEN_EMISSION_TYPE_SMART_CONTRACT", "UNDEFINED"
    ]:
        pass

    @property
    def ticker(self) -> ticker:  # token name
        pass

    @property
    def addr(self) -> ChainAddr:
        pass

    @property
    def value(self) -> Math:
        pass

    @property
    def data(self) -> dict[str, Any] | list[Sign] | None:
        pass

    @property
    def signCount(self) -> int:
        pass

    @property
    def signs(self) -> list[Sign]:
        pass

    def addSign(self, cert: Cert, /) -> None:
        pass

    def appendSign(self, data: bytes, /) -> bool:  # only True
        pass

    def addTSD(self, type: int, data: bytes, /) -> None:
        pass

    def getTSD(self, type: int, /) -> bytes | None:
        pass


# DapChainDatumDecreeObjectType
class DatumDecree(Protocol):
    # Декрет - управляет настройки сети
    @property
    def hash(self) -> HashFast:
        pass

    @property
    def tsCreated(self) -> datetime:
        pass

    @property
    def type(self) -> int:
        pass

    @property
    def typeStr(self) -> Literal[
        "DAP_CHAIN_DATUM_DECREE_TYPE_COMMON", "DAP_CHAIN_DATUM_DECREE_TYPE_SERVICE", "UNKNOWN"
    ]:
        pass

    @property
    def subtype(self) -> int:
        pass

    @property
    def subtypeStr(self) -> Literal[
        "DECREE_COMMON_SUBTYPE_FEE", "DECREE_COMMON_SUBTYPE_OWNERS", "DECREE_COMMON_SUBTYPE_OWNERS_MIN",
        "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_APPROVE", "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_INVALIDATE",
        "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALUE", "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALIDATORS_COUNT",
        "DECREE_SUBTYPE_UNKNOWN"
    ]:
        pass

    @property
    def signs(self) -> list[Sign]:
        pass

    @property
    def TSD(self) -> list[TSD]:
        pass

    def addSign(self, cert: Cert, /) -> None:
        pass

    @staticmethod
    def createApprove(net: Net, tx_hash: HashFast, cert: Cert, /) -> DatumDecree:
        pass

    def createAnchor(self, net: Net, cert: Cert, chain: Chain, /) -> DatumAnchor:
        pass

    def signCheck(self, /) -> SignCheck:
        pass


# DapChainDatumAnchorObjectType
class DatumAnchor(Protocol):
    @property
    def hash(self) -> HashFast:
        pass

    @property
    def created(self) -> datetime:
        pass

    @property
    def TSD(self) -> list[TSD]:
        pass

    @property
    def signs(self) -> list[Sign]:
        pass

    @property
    def decreeHash(self) -> HashFast:
        pass

    # no methods


# DapChainDatumTxObjectType
class DatumTx(Protocol):
    @property
    def hash(self) -> HashFast:
        pass

    @property
    def dateCreated(self) -> datetime:  # type it is not exactly
        pass

    def getItems(self) -> list[ItemTypes]:
        pass

    def getSize(self, item: bytes) -> int:
        pass

    def addItem(self, item):
        pass

    def addInItem(self, args):
        pass

    def addInCondItem(self, args):
        pass

    def addOutItem(self, args):
        pass

    def addOutCond(self, args):
        pass

    def addSignItem(self, args):
        pass

    def verifySign(self, args):
        pass


# DapChainTxOutCondObjectType

# DapChainTxInObjectType [DatumItems]
# TX_ITEM_TYPE_IN
class TxIn(Protocol):
    # Вход
    @property
    def prevHash(self) -> HashFast:
        pass

    @property
    def prevIdx(self) -> int:
        pass


# DapChainTxInCondObjectType
# TX_ITEM_TYPE_IN_COND
class TxInCond(Protocol):
    # Условный вход[при условии]
    @property
    def prevHash(self) -> HashFast:
        pass

    @property
    def outPrevIdx(self) -> int:
        pass

    @property
    def receiptPrevIdx(self) -> int:
        pass


# DapChainTxOutObjectType
# TX_ITEM_TYPE_OUT
class TxOut(Protocol):
    # выход
    @property
    def addr(self) -> ChainAddr:
        pass

    @property
    def value(self) -> Math:
        pass

# DapChainTxOutCondObjectType
# TX_ITEM_TYPE_OUT_COND
class TxOutCond(Protocol):  # в JSON
    # тип items датума транзакции
    @property
    def tsExpires(self) -> datetime:
        pass

    @property
    def value(self) -> Math:
        pass

    @property
    def typeSubtype(self) -> Literal[
        "DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY","DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE",
        "DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE",
        "DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK", "DAP_CHAIN_TX_OUT_COND_SUBTYPE_FEE",
        "DAP_CHAIN_TX_OUT_COND_SUBTYPE_UNDEFINED"
    ]:
        pass

    @property
    def subtype(self) -> None:
        # Not implemented
        pass


# DapChainTxPkeyObjectType
# TX_ITEM_TYPE_PKEY
class TxPkey(Protocol):
    """public key"""

    @property
    def sigType(self) -> SignType:
        pass

    @property
    def sigSize(self) -> int:
        pass

    @property
    def sequenceNumber(self) -> int:
        pass

    @property
    def pkey(self) -> Pkey:
        pass


# DapChainTxSigObjectType
# TX_ITEM_TYPE_SIG
class TxSig(Protocol):
    # подпись
    @property
    def sign(self) -> Sign:
        pass

    @property
    def sigSize(self) -> int:
        pass


# DapChainTxTokenObjectType
# TX_ITEM_TYPE_IN_EMS
class TxToken(Protocol):
    # тикер
    @property
    def ticker(self) -> str:  # token name
        pass

    @property
    def tokenEmissionHash(self) -> HashFast:
        pass

    @property
    def tokenEmissionChainId(self) -> ChainID:
        pass


# DapChainTxReceiptObjectType
# TX_ITEM_TYPE_RECEIPT
class TxReceipt(Protocol):
    # Чек
    @property
    def size(self) -> int:
        pass

    @property
    def extSize(self) -> int:
        pass

    @property
    def units(self) -> int:
        pass

    @property
    def uid(self) -> int:
        pass

    @property
    def unitsType(self) -> str:
        pass

    @property
    def value(self) -> int:
        pass

    @property
    def provider(self) -> Sign | None:
        pass

    @property
    def client(self) -> Sign | None:
        pass

    def sign(self, cert: Cert) -> TxReceipt:  # self
        pass


# DapChainTxOutExtObjectType
# TX_ITEM_TYPE_OUT_EXT
class TxOutExt(Protocol):
    # расширенный выход
    @property
    def addr(self) -> ChainAddr:
        pass

    @property
    def token(self) -> str:
        # TODO: token и ticker одно и тоже?
        pass

    @property
    def value(self) -> Math:
        pass


# DapChainTxTSDObjectType
# TX_ITEM_TYPE_TSD
class TxTSD(Protocol):
    # ТСД
    @property
    def data(self) -> bytes:
        pass

    @property
    def type(self) -> int:
        pass


# DapChainTxOutCondSubTypeSrvPayObjectType
# DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY
class TxOutCondSubtypeSrvPay(Protocol):
    # TODO: .tp_base = &DapChainTxOutCondObjectType,
    @property
    def unit(self) -> int:
        pass

    @property
    def uid(self) -> ServiceUID:
        pass

    @property
    def pkeyHash(self) -> HashFast:
        pass

    @property
    def maxPrice(self) -> int:
        pass


# DapChainTxOutCondSubTypeSrvStakeLockObjectType
# DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK
class TxOutCondSubtypeSrvStakeLock(Protocol):
    # TODO: .tp_base = &DapChainTxOutCondObjectType
    @property
    def timeUnlock(self) -> datetime:
        pass

    @property
    def flags(self) -> int:
        pass

    @property
    def reinvestPercent(self) -> int:
        pass

    @property
    def hashTokenDelegate(self) -> None:
        pass


# DapChainTxOutCondSubTypeSrvStakePosDelegateObjectType
# DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE
class TxOutCondSubtypeSrvStakePosDelegate(Protocol):
    @property
    def uid(self) -> ServiceUID:
        pass

    @property
    def addr(self) -> ChainAddr:
        pass

    @property
    def value(self) -> None:
        pass


# DapChainTxOutCondSubTypeSrvXchangeObjectType
# DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE
class TxOutCondSubtypeSrvXchange(Protocol):
    @property
    def uid(self) -> ServiceUID:
        pass

    @property
    def netId(self) -> NetID:
        pass

    @property
    def token(self) -> str:
        pass

    @property
    def value(self) -> int:
        pass
