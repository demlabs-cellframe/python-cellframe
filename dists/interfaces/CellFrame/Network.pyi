from typing import Any, Protocol, Literal, Callable, NewType
from .Chain import Ledger, Chain, ChainAddr, ChainCellID, ChainType
from .Common import DatumTx

from DAP.Core import Math
from DAP.Crypto import HashFast

mempool_group = str
# net_name = NewType("net_name", str)
net_name = str

# === Chain node ===
# DapChainNodeObjectType
class Node(Protocol):
    pass


# DapChainNodeInfoObjectType
class NodeInfo(Protocol):
    pass


# DapChainNodeClientObjectType
class NodeClient(Protocol):
    pass


# DapChainNodeAddrObjectType
class NodeAddr(Protocol):
    pass


# === Chain net ===
# DapChainNetObjectType
class Net(Protocol):
    @property
    def id(self) -> NetID:
        pass

    @property
    def chains(self) -> list[Chain]:
        pass

    @property
    def txFee(self) -> Math | None:
        pass

    @property
    def txFeeAddr(self) -> ChainAddr | None:
        pass

    @property
    def validatorMaxFee(self) -> Math:
        pass

    @property
    def validatorAverageFee(self) -> Math:
        pass

    @property
    def validatorMinFee(self) -> Math:
        pass

    @property
    def nativeTicker(self) -> str:
        # TODO: выяснить, что такое native ticker
        pass

    @property
    def autoproc(self) -> bool:
        # TODO: выяснить, что делает метод. Подразумевается mempool_autoproc
        pass

    @property
    def gdb_group_alias(self) -> str:
        # TODO: выяснить, что делает метод. Подразумевается mempool_autoproc
        pass

    # methods
    def stateGoTo(self, net_state: NetState, /) -> Literal[-1, 0]:
        # TODO: интерпретировать -1 как Exception
        pass

    def start(self) -> Literal[-1, 0]:
        # TODO: WTF
        pass

    def stop(self) -> Literal[1, 0]:
        # TODO: WTF
        pass

    def linksEstablish(self):
        # go_to(a_net,NET_STATE_LINKS_ESTABLISHED)
        pass

    def syncChains(self):
        # go_to(a_net,NET_STATE_SYNC_CHAINS)
        pass

    def syncGdb(self, args):
        # go_to(a_net,NET_STATE_SYNC_GDB
        pass

    def syncAll(self, args):
        # go_to(a_net,NET_STATE_SYNC_CHAINS)
        pass

    # def procDatapool(self, args):
    #     pass

    @staticmethod
    def loadAll():
        # TODO: WTF
        pass

    @staticmethod
    def byName(net_name: str) -> Net:
        pass

    @staticmethod
    def getNets() -> list[Net]:
        # TODO: зачем сети передаем через hostsettings?
        pass

    @staticmethod
    def byId(net_id: NetID, /) -> Net | None:
        pass

    @staticmethod
    def idByName(net_id: NetID, /) -> Net | None:
        pass

    @staticmethod
    def ledgerByNetName(net_name: str, /) -> Ledger | None:
        pass

    @staticmethod
    def getGdbGroupMempool(chain: Chain, /) -> mempool_group:
        # TODO: что такое группы мемпула?
        pass

    def getChainByName(self, chain_name: str, /) -> Chain | None:
        pass

    def getCurAddr(self) -> NodeAddr:
        # TODO: видимо адрес текущей ноды в сети. Уточнить
        pass

    def getCurCell(self) -> ChainCellID:
        pass

    def getGdbGroupMempoolByChainType(self, chain_type: ChainType, /) -> mempool_group:
        pass

    # def linksConnect(self, args):
    #     pass

    def getChainByChainType(self, chain_type: ChainType, /) -> Chain:
        pass

    def getLedger(self) -> Ledger:
        pass

    def getName(self) -> str:
        pass

    def getTxByHash(self, hash: HashFast, /) -> DatumTx:
        pass

    def addNotify(self, callback: Callable[[int, str, str, Any, net_name], None], args: tuple=(), /) -> None:
        """
        CellframeNetwork.gdbsync_notification_callback
        Callable[['op_code', 'group', 'key', 'value', 'net_name']
        """
        pass


# DapChainNetIdObjectType
class NetID(Protocol):
    @staticmethod
    def fromStr(net_str: str) -> NetID | None:
        pass

    def long(self) -> int:
        pass


# typedef enum dap_chain_net_state{
#     NET_STATE_OFFLINE = 0,
#     NET_STATE_LINKS_PREPARE,
#     NET_STATE_LINKS_CONNECTING,
#     NET_STATE_LINKS_ESTABLISHED,
#     NET_STATE_ADDR_REQUEST, // Waiting for address assign
#     NET_STATE_SYNC_GDB,
#     NET_STATE_SYNC_CHAINS,
#     NET_STATE_ONLINE
# } dap_chain_net_state_t;

# DapChainNetStateObjectType
class NetState(Protocol):
    @staticmethod
    def NET_STATE_OFFLINE():
        pass

    @staticmethod
    def NET_STATE_LINKS_PREPARE():
        pass

    @staticmethod
    def NET_STATE_LINKS_CONNECTING():
        pass

    @staticmethod
    def NET_STATE_LINKS_ESTABLISHED():
        pass

    @staticmethod
    def NET_STATE_ADDR_REQUEST():
        pass

    @staticmethod
    def NET_STATE_SYNC_GDB():
        pass

    @staticmethod
    def NET_STATE_SYNC_CHAINS():
        pass


# === Chain net srv ===

# DapChainNetSrvObjectType
class Service(Protocol):
    pass


# DapChainNetSrvClientObjectType
class ServiceClient(Protocol):
    pass


# DapChainNetStateObjectType
class ServicePrice(Protocol):
    pass


# DapChainNetSrvOrderObjectType
class ServiceOrder(Protocol):
    pass


# DapChainNetSrvOrderDirectionObjectType
class ServiceOrderDirection(Protocol):
    pass


# DapChainNetSrvUidObjectType
class ServiceUID(Protocol):
    pass


# DapChainNetSrvPriceUnitUidObjectType
class ServicePriceUnitUID(Protocol):
    pass
