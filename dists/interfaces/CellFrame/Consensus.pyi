from typing import Protocol
from datetime import datetime

from CellFrame.Chain import ChainAtomPtr, ChainID, ChainCellID
from CellFrame.Common import Datum
from DAP.Crypto import HashFast, Sign


# DapChainCsDagPoaObjectType
class DagPoa(Protocol):
    pass


# DapChainCsBlockType
class Block(Protocol):
    @property
    def hash(self) -> HashFast:
        pass

    @property
    def version(self) -> int:
        pass

    @property
    def cellId(self) -> ChainCellID:
        pass

    @property
    def chainId(self) -> ChainID:
        pass

    @property
    def created(self) -> datetime:
        pass

    @property
    def metaData(self) -> dict:
        pass

    @property
    def datums(self) -> list[Datum] | None:
        pass

    @property
    def signs(self) -> list[Sign]:
        pass

    @staticmethod
    def fromAtom(atom: ChainAtomPtr) -> Block:
        pass

    @staticmethod
    def ledgerRetCode(*args):
        pass

    @staticmethod
    def byTxHash(*args):
        pass


# DapChainCsDagType
class ChainCSDag(Protocol):
    def findByHash(self, hash: HashFast) -> DAG:
        pass


# DapChainCsDagEventType
class DAG(Protocol):
    @property
    def hash(self)-> HashFast:
        pass

    @property
    def version(self) -> int:
        pass

    @property
    def roundId(self) -> int:
        pass

    @property
    def created(self) -> datetime:
        pass

    @property
    def chainId(self) -> ChainID:
        pass

    @property
    def cellId(self):
        pass

    @property
    def hashCount(self) -> int:
        pass

    @property
    def signsCount(self) -> int:
        pass

    @property
    def links(self) -> list[HashFast]:
        pass

    @property
    def datum(self) -> Datum:
        pass

    @property
    def signs(self) -> list[Sign]:
        pass

    @staticmethod
    def fromAtom(atom: ChainAtomPtr) -> DAG:
        pass


# DapChainGdbObjectType
class GDB(Protocol):
    pass
