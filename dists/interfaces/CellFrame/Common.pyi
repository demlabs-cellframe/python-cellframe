from typing import Protocol, Any


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

    def getDatumTX(self):
        pass

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


# DapChainTxOutCondObjectType
class TxOutCond(Protocol):
    pass


# DapChainTxOutCondSubTypeSrvPayObjectType
class TxOutCondSubtypeSrvPay(Protocol):
    pass


# DapChainTxOutCondSubTypeSrvStakeLockObjectType
class TxOutCondSubtypeSrvStakeLock(Protocol):
    pass


# DapChainTxOutCondSubTypeSrvStakePosDelegateObjectType
class TxOutCondSubtypeSrvStakePosDelegate(Protocol):
    pass


# DapChainTxOutCondSubTypeSrvXchangeObjectType
class TxOutCondSubtypeSrvXchange(Protocol):
    pass


# DapChainTxInObjectType
class TxIn(Protocol):
    pass


# DapChainTxInCondObjectType
class TxInCond(Protocol):
    pass


# DapChainTxOutObjectType
class TxOut(Protocol):
    pass


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
    pass


# DapChainTxTSDObjectType
class TxTSD(Protocol):
    pass
