from typing import Protocol, Any, Literal, NewType, Optional, Callable
from .Common import Datum, DatumEmission, DatumTx, DatumToken, DatumDecree, DatumAnchor
from .Network import NetID, Net, ServicePriceUnitUID
from .Consensus import DAG, DagPoa, ChainCSDag

from DAP.Crypto import Sign, SignType, HashFast, Pkey, Cert
from DAP.Core import Math

from .types import ticker


# chainModule
# DapChainObjectType
class Chain(Protocol):
    @staticmethod
    def findById(net_id: NetID, chain_id: 'ChainID', /) -> Chain:
        pass

    @staticmethod
    def loadFromCfg(ledger: 'Ledger', net_name: str, net_id: NetID, cfg_name: str, /) -> Chain:
        pass

    def __str__(self):  # chain_name
        pass

    def hasFileStore(self) -> bool:
        # проверка на то что чейн не inmemory (такой тоже может быть)
        pass

    def saveAll(self) -> int:
        # зафлагить в файл все изменения которые могли быть сделаны в чейне извне.
        # Принудительно на диск сохранить, если он hasFileStore
        pass

    def loadAll(self) -> int:
        pass

    def createAtomIter(self, with_treshold: bool, /) -> 'ChainAtomIter':
        # Если там true, то итератор будет обходить также трешхолды - временные хранилища эвентов,
        # которые пока не удовлетворяют условиям попадания в общий список, но со временем могут сделать это
        pass

    def atomIterGetFirst(self, iter: 'ChainAtomIter', /) -> tuple['ChainAtomPtr', int] | None:
        pass

    def atomGetDatums(self, atom: 'ChainAtomPtr', /) -> list[Datum]:
        pass

    def atomIterGetNext(self, iter: 'ChainAtomIter', /) -> tuple['ChainAtomPtr', int] | tuple[None, int]:
        pass

    def getDag(self) -> ChainCSDag:
        pass

    # callback
    def addMempoolNotify(self, callback: Callable[[Literal["a", "d"], str, str, bytes, tuple], None], args: tuple,
                         /) -> None:
        """
        Callable[op_code, table_name, hash/key, value, add_args
        """

        pass

    # callback
    def addAtomNotify(self, callback: Callable, args: tuple, /) -> None:
        pass

    def atomFindByHash(self, iter: 'ChainAtomIter', hash: HashFast, /):
        pass

    def countAtom(self) -> int:
        pass

    def getAtoms(self, count: int, page: int, reverse: bool, /) -> list[ChainAtomPtr] | None:
        pass

    def countTx(self) -> int:
        pass

    def getTransactions(self, count: int, page: int, reverse: bool, /) -> list[DatumTx] | None:
        pass

    def getCSName(self) -> Literal["esbocs", "dag_poa"]:
        # esbocs || block_poa
        pass


# DapChainTypeObjectType
class ChainType(Protocol):
    @staticmethod
    def CHAIN_TYPE_FIRST():
        pass

    @staticmethod
    def CHAIN_TYPE_TOKEN():
        pass

    @staticmethod
    def CHAIN_TYPE_EMISSION():
        pass

    @staticmethod
    def CHAIN_TYPE_TX():
        pass

    @staticmethod
    def CHAIN_TYPE_LAST():
        pass


# DapChainAtomIterObjectType
class ChainAtomIter(Protocol):
    # no methods
    pass


# DapChainAtomPtrObjectType
# Atom
class ChainAtomPtr(Protocol):
    # no methods
    # Хранит адрес сишной структуры для получения Block/DAG(Event)
    pass


# DapChainCellObjectType
class ChainCell(Protocol):
    @staticmethod
    def load(obj_chain, cell_file_path=None):
        pass

    def update(self):
        pass

    def append(self, atom_bytes, atom_size=None):
        pass


# DapChainIdObjectType
class ChainID(Protocol):
    # no methods
    pass


# DapChainCellIdObjectType
class ChainCellID(Protocol):
    # no methods
    pass


# DapChainHashSlowObjectType
class ChainHashSlow(Protocol):
    @staticmethod
    def toStr(hash_slow, string, string_max):
        pass


# DapChainHashSlowKindObjectType
class ChainHashSlowKind(Protocol):
    # no methods
    pass


# DapChainAddrObjectType
class ChainAddr(Protocol):
    def toStr(self, chain_address: ChainAddr, /) -> str:
        pass

    def __str__(self) -> str:
        pass

    @staticmethod
    def fromStr(address: str, /) -> ChainAddr:
        pass

    @staticmethod
    def fill(sign_type: SignType, pkey_hash: HashFast, net_id: NetID, /) -> ChainAddr:
        pass

    @staticmethod
    # TODO: CryptoKeyObject - WTF
    def fillFromKey(key: 'CryptoKeyObject', net_id: NetID, /) -> ChainAddr:
        pass

    def checkSum(self) -> int:
        pass

    def getNetId(self) -> NetID:
        pass


# DapChainCsObjectType
class ChainCS(Protocol):
    # TODO: что за сущность?
    def csAdd(self, cs: str, callback: Callable, /) -> None:  # return 0
        pass

    def csCreate(self, chain: Chain) -> int:
        pass

    def classAdd(self, cs: str, callback: Callable, /) -> None:  # return 0
        pass

    def classCreate(self, chain: Chain) -> int:
        pass


# DapChainGlobalDBContainerObjectType
class ChainGlobalDBContainer(Protocol):
    @property
    def id(self) -> int:
        pass

    @property
    def key(self) -> str:
        pass

    @property
    def value(self) -> bytes:
        pass


# DapChainGlobalDBObjectType
class GlobalDB(Protocol):
    @staticmethod
    def get(key: str, group: str, /) -> bytes:  # 'json'
        pass

    @staticmethod
    def set(key: str, group: str, obj_byte: bytes, is_pinned: bool = False, /) -> bool:
        pass

    # deprecated
    @staticmethod
    def set_sync(key: str, group: str, obj_byte: bytes, is_pinned: bool = False, /) -> bool:
        """
        Блокирующий set
        """
        pass

    @staticmethod
    def delete(key: str, group: str, /) -> bool:
        pass

    @staticmethod
    def pin(key: str, group: str, /) -> bool:
        """
        Блокировка записей от авто-удаления трехчасовым таймером
        """
        pass

    @staticmethod
    def unpin(key: str, group: str, /) -> bool:
        pass

    @staticmethod
    def grLoad(group: str, /) -> list[ChainGlobalDBContainer]:
        pass

    @staticmethod
    def addSyncExtraGroup(net_name: str, group_mask: str, callback: Callable,
                          callback_args: list, /):
        """
        add to white-list current node(sync table)
        """
        pass


# DapChainWalletObjectType
# Parent task
# Task
# path_wallets - убрать и сделать как cli
class Wallet(Protocol):

    def __new__(cls, wallet_name: str, path_wallets: str, sign_type: SignType) -> Wallet | None:
        pass

    # TODO: разобраться с типами
    @staticmethod
    def getPath(self) -> str:
        pass

    @staticmethod
    def createWithSeed(wallet_name: str, path_wallets: str, sig_type: SignType, seed: bytes) -> Wallet:
        pass

    @staticmethod
    def openFile(file_path: str, password: str = None, /) -> Wallet:
        pass

    @staticmethod
    def open(wallet_name: str, wallet_path: str, /) -> Wallet:
        pass

    def save(self) -> int:
        pass

    @staticmethod
    def certToAddr(cert: Cert, net_id: object, /) -> 'ChainAddr':
        pass

    def getAddr(net_id: object) -> 'ChainAddr':
        pass

    def getCertsNumber(self) -> int:
        pass

    def getPKey(self, key_idx: int) -> 'DapPkey':
        pass

    def getKey(self, key_idx: int) -> 'CryptoKey':
        pass


# DapChainMempoolObjectType
class Mempool(Protocol):
    @staticmethod
    def proc(hash_datum: str, chain: Chain, /) -> None:
        # обработать этот датум
        pass

    @staticmethod
    def emissionPlace(chain: Chain, emission: DatumEmission, /) -> str | None:  # str - DatumHash
        """
        Get DatumEmission hash
        """
        pass

    # TODO: emissionGet emissionExtract - делают примерно одно и тоже.
    @staticmethod
    def emissionGet(chain: Chain, emission_hash: str, /) -> DatumEmission | None:
        pass

    @staticmethod
    def emissionExtract(chain: Chain, value: bytes, /) -> DatumEmission | None:
        pass

    @staticmethod
    def datumExtract(value: bytes, /) -> Datum | None:
        pass

    @staticmethod
    def datumGet(chain: Chain, emission_hash: str, /) -> Datum | None:
        pass

    @staticmethod
    def txCreate(chain: Chain, key_from: 'CryptoKeyObject', addr_from: ChainAddr, addr_to: ChainAddr,
                 token_ticker: ticker, value: int, value_fee: int) -> HashFast | str:
        pass

    @staticmethod
    def baseTxCreate(chain: Chain, emi_hash: HashFast, emi_chain: Chain, emission_value: Math, ticker: ticker,
                     addr_to: ChainAddr, value_fee: Math, wallet_or_cert: Wallet | Cert) -> HashFast | None:
        pass

    @staticmethod
    # TODO: value то строка, то int, то Math. unit: ServicePriceUnitUID - уточнить
    def txCreateCond(net: Net, key_from: 'CryptoKeyObject', key_cond: Pkey, token_ticker: ticker, value: str,
                     value_per_unit_max: str, unit: ServicePriceUnitUID, srv_uid: Any, fee: str,
                     condition: bytes) -> HashFast | None:
        pass

    @staticmethod
    def txCreateCondInput(net: Net, tx_prev_hash: HashFast, addr_to: ChainAddr, key_tx_sign: Sign,
                          receipt: Any) -> HashFast | None:
        pass

    @staticmethod
    # TODO: hash - то HashFast, то str - задумка или халтура?
    def remove(chain: Chain, hash_str: str, /) -> bool:
        pass

    @staticmethod
    def list(net: Net, chain: Chain, /) -> dict[str, Datum]:
        pass

    @staticmethod
    def addDatum(chain: Chain, data: Datum | DatumDecree | DatumAnchor, /) -> str | None:
        pass


# DapChainLedgerObjectType
class Ledger(Protocol):
    """
    Unified distributed registry of confirmed transactions
    """

    def setLocalCellId(self, local_cell_id: ChainCellID, /):
        # TODO: WTF
        pass

    def nodeDatumTxCalcHash(self, tx: DatumTx, /) -> HashFast:
        pass

    # def txAdd(self, tx: DatumTx, /) -> Literal[-1, 0]:
    #     pass

    def tokenAdd(self, token: DatumTx, token_size: int, /) -> Literal[-1, 0]:
        pass

    def tokenEmissionLoad(self, token_emission: DatumEmission, token_emission_size: int, /) -> Literal[-1, 0]:
        pass

    def tokenEmissionFind(self, hash_fast: HashFast, /) -> DatumEmission | None:
        pass

    def tokenAuthSignsTotal(self, token_ticker: ticker, /) -> int | None:
        # TODO: WTF
        pass

    def tokenAuthSignsValid(self, token_ticker: ticker, /) -> int | None:
        # TODO: WTF
        pass

    def tokenAuthPkeysHashes(self, token_ticker: ticker, /) -> list[HashFast]:
        pass

    def txGetTokenTickerByHash(self, hash: HashFast) -> ticker | None:
        pass

    def addrGetTokenTickerAll(self, chain_address: ChainAddr) -> list[ticker]:
        pass

    def txCacheCheck(self, *args):
        pass

    def datumTxCacheCheck(self, *args):
        pass

    def txRemove(self, *args):
        pass

    def purge(self, *args):
        pass

    def count(self, *args):
        pass

    def countFromTo(self, ts_from: int = 0, ts_to: int = 0, /) -> int:
        # Количество транзакций за промежуток времени. ts - timestamp
        pass

    def txHashIsUsedOutItem(self, *args):
        pass

    def calcBalance(self, chain_address: ChainAddr, token_ticker: ticker, /) -> Math:  # DapMathObject
        # Получить баланс из кэша Ledger'а (это не точно)
        pass

    def calcBalanceFull(self, chain_address: ChainAddr, token_ticker: ticker, /) -> Math:
        # Пересчет баланса по всем транз (это не точно)
        pass

    def txFindByHash(self, hash: HashFast, /) -> DatumTx | None:
        pass

    def txFindByAddr(self, *args):
        pass

    def txFindByPkey(self, *args):
        pass

    def txCacheFindOutCond(self, *args):
        pass

    def txCacheGetOutCondValue(self, *args):
        pass

    def getTransactions(self, count: int, page: int, reverse: bool, unspent: bool) -> list[DatumTx]:
        pass

    def txAddNotify(self, *args):
        pass

    def bridgedTxNotifyAdd(self, *args):
        pass
