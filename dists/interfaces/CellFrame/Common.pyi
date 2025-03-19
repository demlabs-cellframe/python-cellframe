from typing import Protocol, Any
from Network import Net
from DAP.Crypto import HashFast
from DAP.Core import Math
from Chain import ChainAddr
from Network import ServiceUID
from Network import NetID
from datetime import datetime


# commonModule

# DapChainDatumTypeIdObjectType
class DatumTypeID(Protocol):
    pass


# DapChainDatumObjectType
class Datum(Protocol):
    hash: str
    versionStr: str
    tsCreated: Any
    raw: Any
    dataRaw: Any

    def getSize(self):
        pass

    def isDatumTX(self):
        pass

    def getDatumTX(self, net : Net) -> DatumTx: ...

    def isDatumToken(self):
        pass

    def getDatumToken(self):
        pass

    def isDatumTokenEmission(self):
        pass

    def getDatumTokenEmission(self):
        pass

    def isDatumCustom(self):
        pass

    def isDatumDecree(self):
        pass

    def getDatumDecree(self):
        pass

    def isDatumAnchor(self):
        pass

    def getDatumAnchor(self):
        pass

    def getTypeStr(self):
        pass

    def getTypeId(self):
        pass


# DapChainDatumIterObjectType
class DatumIter(Protocol):
    pass


# DapChainDatumTokenObjectType
class DatumToken(Protocol):
    pass


# DapChainTxTokenExtType
class DatumTokenExt(Protocol):
    pass


# DapChainDatumTokenEmissionObjectType
class DatumEmission(Protocol):
    hash: Any
    version: Any
    typeStr: Any
    ticker: Any
    addr: Any
    value: Any
    data: Any
    signCount: Any
    signs: Any

    def __init__(self):
        pass

    # Методы объекта
    def addSign(self, args):
        pass

    def appendSign(self, args):
        pass

    def addTSD(self, args):
        pass

    def getTSD(self, args):
        pass


# DapChainDatumDecreeObjectType
class DatumDecree(Protocol):
    pass


# DapChainDatumAnchorObjectType
class DatumAnchor(Protocol):
    pass


# DapChainTxItemTypeObjectType
class TxItemType(Protocol):
    pass


# DapChainDatumTxObjectType
class DatumTx(Protocol):
    hash: Any
    dateCreated: Any

    def __init__(self):
        pass

    def getItems(self):
        pass

    def getSize(self, args):
        pass

    def addItem(self, args):
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

class ChainTxOutCondSubType(Protocol):
    def __str__(self) -> str: ...

# DapChainTxOutCondObjectType
class TxOutCond(Protocol):
    @property
    def tsExpires(self) -> datetime: ...
    @property
    def value(self) -> Math: ...
    @property
    def typeSubtype(self) -> ChainTxOutCondSubType: ...
    @property
    def usedBy(self) -> HashFast | None: ...
    @property
    def tag(self) -> str | None: ...


# DapChainTxOutCondSubTypeSrvPayObjectType
class TxOutCondSubtypeSrvPay(Protocol):
    @property
    def unit(self) -> int: ...
    @property
    def uid(self) -> ServiceUID: ...
    @property
    def pkeyHash(self) -> HashFast: ...
    @property
    def maxPrice(self) -> int: ...
    @property
    def usedBy(self) -> HashFast | None: ...


# DapChainTxOutCondSubTypeSrvStakeLockObjectType
class TxOutCondSubtypeSrvStakeLock(Protocol):
    @property
    def timeUnlock(self) -> datetime:...
    @property
    def flags(self) -> int: ...
    @property
    def reinvestPercent(self) -> int: ...
    @property
    def hashTokenDelegate(self) -> None: ...
    @property
    def usedBy(self) -> HashFast | None: ...

# DapChainTxOutCondSubTypeSrvStakePosDelegateObjectType
class TxOutCondSubtypeSrvStakePosDelegate(Protocol):
    @property
    def uid(self) -> ServiceUID: ...
    @property
    def addr(self) -> ChainAddr: ...
    @property
    def value(self) -> None: ...
    @property
    def usedBy(self) -> HashFast | None: ...


# DapChainTxOutCondSubTypeSrvXchangeObjectType
class TxOutCondSubtypeSrvXchange(Protocol):
    @property
    def uid(self) -> ServiceUID: ...
    @property
    def netId(self) -> NetID: ...
    @property
    def token(self) -> str: ...
    @property
    def value(self) -> Math: ...
    @property
    def usedBy(self) -> HashFast | None: ...


# DapChainTxInObjectType
class TxIn(Protocol):
    pass


# DapChainTxInCondObjectType
class TxInCond(Protocol):
    pass


# DapChainTxOutObjectType
class TxOut(Protocol):
    @property
    def addr(self) -> ChainAddr : ...
    @property
    def value(self) -> Math: ...
    @property
    def usedBy(self) -> HashFast | None: ...


# DapChainTxPkeyObjectType
class TxPkey(Protocol):
    pass


# DapChainTxSigObjectType
class TxSig(Protocol):
    pass


# DapChainTxTokenObjectType
class TxToken(Protocol):
    pass


# DapChainTxReceiptObjectType
class TxReceipt(Protocol):
    pass


# DapChainTxOutExtObjectType
class TxOutExt(Protocol):
    @property
    def addr(self) -> ChainAddr : ...
    @property
    def token(self) -> str : ...
    @property
    def value(self) -> Math: ...
    @property
    def usedBy(self) -> HashFast | None: ...


# DapChainTxTSDObjectType
class TxTSD(Protocol):
    pass
