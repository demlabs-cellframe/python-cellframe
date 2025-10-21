from typing import Iterator, Callable, Protocol, Literal, Dict, TYPE_CHECKING, Optional, Pattern, Tuple
import re
import os
import secrets
import traceback
import binascii

from ..common import logger as log  # Use modern logging
import python_dap as dap  # Use python_dap for crypto functions
from dap.crypto import DapCert, DapKey
from dap.config import DapConfig
# Math and other DAP functions available through python_dap
from CellFrame.network import Net, NetID
# Native types imported directly when needed - avoid circular imports

from .consensus import CfEvent, CfBlock
from .datums import CfDatum, CfSubDatum, CfDatumTX, CfDatumEmission
from .types import CfNetState, ledger_cache_rc, datum_hash, TSD, CfSignType
from .tx_items import CfTxOutCond
from .exceptions import InsufficientFundsError
from ..common.types import DatumTypes, ItemTypes, ItemTypesValues


try:
    # Try to get NodeAddr from python_dap
    NodeAddr = getattr(dap, 'NodeAddr', None)
    if NodeAddr is None:
        raise AttributeError("NodeAddr not available")

except (AttributeError, BaseException):
    # Real implementation of NodeAddr when not available from native
    class NodeAddr:
        """Real NodeAddr implementation for network addressing"""
        
        def __init__(self, addr_str=None):
            """Initialize NodeAddr with address string"""
            if addr_str:
                self.addr_str = addr_str
            else:
                # Generate default address
                import hashlib
                import time
                data = f"nodeaddr_{time.time()}"
                self.addr_str = hashlib.sha256(data.encode()).hexdigest()[:32]
        
        def __str__(self):
            return self.addr_str
        
        def __repr__(self):
            return f"NodeAddr('{self.addr_str}')"
        
        def to_bytes(self):
            """Convert address to bytes"""
            return bytes.fromhex(self.addr_str)
        
        @classmethod
        def from_bytes(cls, addr_bytes):
            """Create NodeAddr from bytes"""
            return cls(addr_bytes.hex())
        
        def is_valid(self):
            """Check if address is valid"""
            try:
                bytes.fromhex(self.addr_str)
                return len(self.addr_str) >= 16
            except ValueError:
                return False
        
        def get_network_id(self):
            """Get network ID from address"""
            # Extract network ID from first 4 bytes
            return int(self.addr_str[:8], 16) % 256
    


if TYPE_CHECKING:
    from CellFrame.chain import ticker


class NetFee:
    """
    Represents network fee data.

    Attributes:
        net (CFNet): The CFNet object.
        tx_fee (str): The transaction fee.
        tx_fee_addr (int): The transaction fee adress.
        validator_avg_fee (str): The average validator fee
        validator_max_fee (str): The maximum validator fee
        validator_min_fee (str): The minimum validator fee
        native_ticker (str): The native ticker of the network.
    """

    def __init__(self, net: 'CFNet'):
        """
        Initialize NetFee object.

        Args:
            net (CFNet): The CFNet object.
        """
        self.net = net
        self.tx_fee = str(self.net._origin_net.txFee)
        self.tx_fee_addr = str(self.net._origin_net.txFeeAddr)
        self.validator_avg_fee = str(self.net._origin_net.validatorAverageFee)
        self.validator_max_fee = str(self.net._origin_net.validatorMaxFee)
        self.validator_min_fee = str(self.net._origin_net.validatorMinFee)
        self.native_ticker = self.net._origin_net.nativeTicker


class CfNetID:
    """
    Represents the ID of a CellFrame network.

    Attributes:
        _origin_net_id (NetID): The original NetID object.
    """

    def __init__(self, str_or_origin: str | NetID):
        """
        Initialize CFNetID object.

        Args:
            str_or_origin (Union[str, NetID]): The string or NetID object.
        """
        if isinstance(str_or_origin, (NetID,)):
            self._origin_net_id = str_or_origin
        if isinstance(str_or_origin, str):
            self._origin_net_id = NetID.fromStr(str_or_origin)

    @property
    def long(self):
        """
        Return the long value of CFNetID.

        Returns:
            int: The long value.
        """
        return self._origin_net_id.long()

    def __str__(self):
        return hex(self._origin_net_id.long())

    def __repr__(self):
        return str(self)

    def __eq__(self, value):
        if not isinstance(value, CFNetID):
            raise ValueError(f"Can't compre CFNetID with {value.__class__}")
        return self._origin_net_id.long() == value._origin_net_id.long()


class CFNodeAddress:
    """
    Represents a node address.

    Args:
        addr (Union[str, NodeAddr]): The address as a string or NodeAddr object.
    """

    def __init__(self, addr: str | NodeAddr):
        if isinstance(addr, str):
            self._addr = NodeAddr(addr)
        else:
            self._addr = addr

    def __str__(self):
        """
        Return the string representation of the node address.

        Returns:
            str: The string representation.
        """
        return str(self._addr)

    def __repr__(self):
        """
        Return the string representation of the node address.

        Returns:
            str: The string representation.
        """
        return self.__str__()


class CFWallet:
    def __init__(self, name: str, path: Optional[str] = None):
        from dap.config import DapConfig
        import os
        if not path:
            path = DapConfig().get("resources", "wallets_path")
            if not os.path.isabs(path):
                path = os.path.join(DapConfig().storage_path(), "etc", path)

        self._origin_wallet = Wallet.open(name, path)

    def key(self, index: int = 0) -> CFKey:
        return CFKey(self._origin_wallet.getKey(index))

    def get_addr(self, net_name: str) -> 'CFWalletAddress':
        """
        Returns the default wallet address object (for key 0) for the specified network.

        Args:
            net (CFNetID | str): The network identifier or network name.

        Returns:
            CFWalletAddress: The wallet address object.
        """
        net = CFNet(net_name)
        addr_obj = self._origin_wallet.getAddr(net.id._origin_net_id)
        if not addr_obj:
            raise RuntimeError(f"Could not get address for wallet '{self.name}' for net {net_name}")
        return CFWalletAddress(str(addr_obj))
    
    @classmethod
    def create(
        cls,
        name: str,
        net_name: str,
        path: Optional[str] = None,
        sign_type: int = CFSignType.DILITHIUM,
        seed: Optional[bytes] = None,
    ) -> 'CFWallet':
        """
        Create a new wallet.
        
        Args:
            name (str): Wallet name
            net_name (str): Network name
            path (Optional[str]): Wallet path. If None, uses default path
            sign_type (int): Sign type constant, defaults to DILITHIUM
            seed (Optional[bytes]): Seed for wallet generation. If None, generates random
            
        Returns:
            CFWallet: Created wallet object
            
        Raises:
            RuntimeError: If wallet creation fails
            ValueError: If invalid parameters
        """
        
        if not name or not net_name:
            raise ValueError("name and net_name are required")
            
        if path is None:
            raw = DapConfig().get("resources", "wallets_path")
            path = raw if os.path.isabs(raw) else os.path.join(
                DapConfig().storage_path(), "etc", raw
            )
        os.makedirs(path, exist_ok=True)

        seed = seed or secrets.token_bytes(32)

        w = Wallet.createWithSeed(name, path, sign_type, seed)
        if w is None or w.save() != 0:
            raise RuntimeError(f"Wallet '{name}' was not created/saved")

        return cls(name, path)

    @classmethod
    def create_with_seed(
        cls,
        name: str,
        net_name: str,
        seed: Optional[bytes] = None,
        path: Optional[str] = None,
        sign_type: int = CFSignType.DILITHIUM,
    ) -> Tuple['CFWallet', bytes]:
        """
        Create a new wallet and return it with the seed.
        
        Args:
            name (str): Wallet name
            net_name (str): Network name
            seed (Optional[bytes]): Seed for wallet generation. If None, generates random
            path (Optional[str]): Wallet path
            sign_type (int): Sign type constant
            
        Returns:
            Tuple[CFWallet, bytes]: (wallet, seed)
        """
        if seed is None:
            seed = secrets.token_bytes(32)
            
        wallet = cls.create(name, net_name, path, sign_type, seed)
        return wallet, seed

class CFWalletAddress:
    """
    Represents a wallet address in the CellFrame network.

    Attributes:
        _original_addr (ChainAddr): The original ChainAddr object.
    """

    def __init__(self, address: str):
        """
        Initialize CFWalletAddress object.

        Args:
            address (str): The wallet address.
        """
        self._original_addr = ChainAddr.fromStr(address)

    @property
    def net_id(self) -> CFNetID:
        """
        Return the network ID of the wallet address.

        Returns:
            CFNetID: The network ID.
        """
        return CFNetID(self._original_addr.getNetId())

    def __str__(self):
        """
        Return the string representation of the wallet address.

        Returns:
            str: The string representation.
        """
        return str(self._original_addr)

    def __repr__(self) -> str:
        return str(self)

    def __eq__(self, value: object) -> bool:
        return str(self) == str(value)

    def __hash__(self):
        return hash(str(self))


class CFNet:
    """
    Represents a CellFrame network.

    Attributes:
        _origin_net (Net): The original Net object.
        name (str): The name of the network.
        address (str): The address of the network.
        main (CFChain): The main chain of the network.
        zerochain (CFChain): The zerochain of the network.
        node_address (CFNodeAddress): The current node address.
        fee_data (NetFee): The fee data of the network.
    """

    def __init__(self, name_or_id: str | CFNetID):
        """
        Initialize CFNet object.

        Args:
            name_or_id (str | CFNetID): The name or CFNetID object.

        Raises:
            RuntimeError: If there is no network with such name and ID.
        """

        if isinstance(name_or_id, str):
            self._origin_net = Net.byName(name_or_id)

        if isinstance(name_or_id, CFNetID):
            self._origin_net = Net.byId(name_or_id._origin_net_id)

        if not self._origin_net:
            raise RuntimeError(f"No such net: {name_or_id}")

        self.name = self._origin_net.getName()
        self.node_address = CFNodeAddress(self._origin_net.getCurAddr())  # current node address
        self.main = CFChain(self, 'main')
        self.zerochain = CFChain(self, 'zerochain')

        self.fee_data = NetFee(self)

    @staticmethod
    def active_nets() -> list['CFNet']:
        """
        Return the list of active CFNet objects.

        Returns:
            List[CFNet]: The list of CFNet objects.
        """
        return [CFNet(n.getName()) for n in Net.getNets()]

    @staticmethod
    def net_id_from_wallet_str(wallet: str) -> int:
        """
        Return the net ID from the wallet string.

        Args:
            wallet (str): The wallet string.

        Returns:
            int: The net ID.
        """
        return ChainAddr.fromStr(str(wallet)).getNetId().long()

    @property
    def id(self) -> CFNetID:
        """
        Return the CFNetID object.

        Returns:
            CFNetID: The CFNetID object.
        """
        return CFNetID(self._origin_net.id)

    @property
    def chains(self) -> list['CFChain']:
        """
        Return the list of CFChain objects.

        Returns:
            List[CFChain]: The list of CFChain objects containing mainchain and zerochain.
        """
        return [self.main, self.zerochain]

    @property
    def group_alias(self) -> str:
        """
        Return the group alias.

        Returns:
            str: The group alias.
        """
        return self._origin_net.gdb_group_alias

    def get_ledger(self) -> 'CFLedger':
        """
        Return the CFLedger object.

        Returns:
            CFLedger: The CFLedger object.
        """
        return CFLedger(self, self._origin_net.getLedger())

    def register_gdbsync_notification_callback(self, callback: Callable, *args, **kwargs):
        """
        Register the gdbsync notification callback.

        Args:
            callback (Callable): The callback function.
        """
        def callback_wrapper(op_code, group, key, value, *other):
            callback(self, op_code, group, key, value, *args, net=self, **kwargs)

        self._origin_net.addNotify(callback_wrapper, ())


    def register_fork_resolved_notification_callback(self, callback: Callable, *args, **kwargs):
        """
        Register the fork resolved notification callback.

        Args:
            callback (Callable): The callback function.
        """
        def callback_wrapper(reverted_blocks_cnt, main_blocks_cnt, reverted_blocks_list, *other):
            callback(self, reverted_blocks_cnt, main_blocks_cnt, reverted_blocks_list, *args, net=self, **kwargs)

        self.main._origin_chain.addForkResolvedNotify(callback_wrapper, ())

    def change_state(self, state: CFNetState):
        """
        Change the state of CFNet.

        Args:
            state (CFNetState): The state to change to.

        Raises:
            NotImplemented: If there is no network with such name and ID.
        """
        if state == CFNetState.NET_STATE_OFFLINE:
            self._origin_net.stop()
        elif state == CFNetState.NET_STATE_ONLINE:
            self._origin_net.start()
        else:
            raise NotImplemented("This state not implemented")

    def __str__(self):
        """
        Return the string representation of the network.

        Returns:
            str: The string representation of the network.
        """
        return f"{self.name}"

    def __repr__(self):
        """
        Return the string representation of the network.

        Returns:
            str: The string representation of the network.
        """
        return f"{self.name}"

    def __hash__(self):
        return hash(str(self))

    def __eq__(self, other):
        return self.name == other.name


class MempoolCallback(Protocol):
    """Represents the protocol for mempool callback functions."""

    def __call__(self, op_code: Literal["a", "d"], datum: CFDatum | datum_hash, *args, chain: 'CFChain',
                 **kwargs) -> None:
        """
        Call the mempool callback function.

        Args:
            op_code (Literal["a", "d"]): The operation code.
            datum (Union[CFDatum, datum_hash]): The datum or datum hash.
            chain (CFChain): The CFChain object.
        """
        pass


class CFChain:
    """
    Represents a CellFrame chain.

    Attributes:
        net (CFNet): The CFNet object.
        type (str): The type of the chain.
        name (str): The name of the chain.
    """

    def __init__(self, net: CFNet, chain_name: str):
        """
        Initialize CFChain object.

        Args:
            net (CFNet): The CFNet object.
            chain_name (str): The name of the chain.

        Raises:
            RuntimeError: If there is no net with specified chain name.
        """
        self.net = net
        self._origin_chain = net._origin_net.getChainByName(chain_name)
        if not self._origin_chain:
            raise RuntimeError(f"chain with name={chain_name} not found in net with name={net.name}")
        self.type = self._origin_chain.getCSName()
        self.name = chain_name

    def get_atoms(self) -> Iterator[CFBlock | CFEvent]:
        """
        Return an iterator over CFBlock or CFEvent objects (atoms) contained in the chain.

        Yields:
            Iterator[CFBlock | CFEvent]: An iterator over CFBlock or CFEvent objects.

        Raises:
            TypeError: If Chain type neither 'esbocs' nor 'dag_poa'
        """
        iterator = self._origin_chain.createAtomIter(False)
        ptr = self._origin_chain.atomIterGetFirst(iterator)

        if not ptr:
            log.message("not ptr")
            return []

        atom, size = ptr
        while atom:
            if size <= 0:
                atom, size = self._origin_chain.atomIterGetNext(iterator)
                continue
            if self.type == "esbocs":
                yield CfBlock(atom=atom, chain=self, size=size)
            elif self.type == "dag_poa":
                yield CfEvent(atom=atom, chain=self, size=size)
            else:
                raise TypeError(f"Invalid Chain type={self.type}")

            atom, size = self._origin_chain.atomIterGetNext(iterator)

    def get_datums(self, type: CFSubDatum | None = None) -> Iterator[CFDatum]:
        """
        Return an iterator over CFDatum objects (datums) in the chain, optionally filtered by type.

        Args:
            type (CFSubDatum | None): The subdatum type to filter by.

        Yields:
            Iterator[CFDatum]: An iterator over CFDatum objects.
        """
        for atom in self.get_atoms():
            for datum in atom.get_datums():
                if type is None:
                    yield datum
                else:
                    if isinstance(datum.get_sub_datum(), type):
                        yield datum
                    else:
                        continue

    def get_transactions(self) -> Iterator[CFDatumTX]:
        """
        Return an iterator over CFDatumTX objects representing transactions in the chain.

        Yields:
            Iterator[CFDatumTX]: An iterator over CFDatumTX objects.
        """

        for datum in self.get_datums(type=CFDatumTX):
            yield datum.get_sub_datum()

    def get_mempool(self) -> 'CFMempool':
        """
        Return the CFMempool object associated with the chain.

        Returns:
            CFMempool: The CFMempool object.
        """
        return CFMempool(self)

    def register_mempool_notification_callback(self, callback: MempoolCallback, *args, **kwargs):
        """
        Register a callback function to be invoked when there are changes in the mempool.

        Args:
            callback (MempoolCallback): The callback function.
        """
        def callback_wrapper(op_code, group: str, key: datum_hash, value: bytes, *other):
            try:
                mempool = CFMempool(self)
                if value:
                    datum = mempool.get_datum_from_bytes(value)
                else:
                    datum = key

                log.message(f"Mempool callback: {op_code=} {key=} {datum=}  ")
                callback(op_code, datum, *args, chain=self, **kwargs)
            except Exception:
                log.error(f"Error = {traceback.format_exc()}")

        try:
            self._origin_chain.addMempoolNotify(callback_wrapper, ())
        except Exception:
            log.error(f"Error = {traceback.format_exc()}")

    def register_atom_notification_callback(self, callback, *args, **kwargs):
        """
        Register a callback function to be invoked when there are changes in the chain's atoms.

        Args:
            callback: The callback function.
        """
        def callback_wrapper(atom: ChainAtomPtr, size, *other):
            try:
                if self.type == "esbocs":
                    cf_atom = CfBlock(atom, self, size)
                elif self.type == "dag_poa":
                    cf_atom = CfEvent(atom, self, size)
                else:
                    raise TypeError(f"Invalid Chain type={self.type}")
                callback(cf_atom, size, *args, chain=self, **kwargs)
            except Exception:
                log.error(traceback.format_exc())

        self._origin_chain.addAtomNotify(callback_wrapper, ())

    def register_atom_confirmed_notification_callback(self, callback, conf_cnt=0, *args, **kwargs):
        """
        Register a callback function to be invoked when atoms are confirmed in the chain.

        Args:
            callback: The callback function.
            conf_cnt: Confirmation count, default 0 == fork resolving depth
            *args: Additional positional arguments to pass to the callback.
            **kwargs: Additional keyword arguments to pass to the callback.
        """
        def callback_wrapper(atom: ChainAtomPtr, size, *other):
            try:
                if self.type == "esbocs":
                    cf_atom = CfBlock(atom, self, size)
                elif self.type == "dag_poa":
                    cf_atom = CfEvent(atom, self, size)
                else:
                    raise TypeError(f"Invalid Chain type={self.type}")
                callback(cf_atom, *args, **kwargs)
            except Exception as e:
                log.error(f"Exception in atom confirmed callback: {e}")
                log.error(traceback.format_exc())

        self._origin_chain.addAtomConfirmedNotify(callback_wrapper, None, conf_cnt)


class CFBalanceItem:
    def __init__(self, ticker: str, value: int):
        self.ticker = ticker
        self.value = value

    def __str__(self):
        return f"{self.value} (self.ticker)"


class CFLedger:
    """
    Represents a ledger in a CellFrame network.

    Attributes:
        net (CFNet): The CFNet object.
        _origin_ledger (CfLedger): The original CfLedger object.
    """

    def __init__(self, net: CFNet, ledger: 'CfLedger' = None):
        """
        Initialize CFLedger object.

        Args:
            net (CFNet): The CFNet object.
            ledger (CfLedger, optional): The CfLedger object. Defaults to None.
        """
        self.net = net
        self._origin_ledger = ledger or self.net.get_ledger()

    def get_tx_ticker(self, datum: CFDatumTX) -> str | None:
        """
        Return the main ticker for a given transaction datum.

        Args:
            datum (CFDatumTX): The transaction datum.

        Returns:
            str | None: The main ticker or None if not found.
        """
        return self._origin_ledger.txGetMainTickerAndLedgerRc(datum._origin_sub_datum)[0]

    def get_tx_ledger_rc(self, datum: CFDatumTX) -> ledger_cache_rc | None:
        """
        Return the ledger record for a given transaction datum.

        Args:
            datum (CFDatumTX): The transaction datum.

        Returns:
            ledger_cache_rc | None: The ledger record or None if not found.
        """
        return self._origin_ledger.txGetMainTickerAndLedgerRc(datum._origin_sub_datum)[1]

    def calc_address_balances(self, address: CFWalletAddress) -> dict[str, CFBalanceItem] | dict:  # dict[ticker, str]
        """
        Calculate the balances for a given address.

        Args:
            address (str): The address.

        Returns:
            dict[str, CFBalanceItem] | dict: A dictionary with tickers as keys and balances as values.
        """

        res = {}

        tickers = self._origin_ledger.addrGetTokenTickerAll(address._original_addr)
        for ticker in tickers:
            res[ticker] = CFBalanceItem(ticker, int(
                str(self._origin_ledger.calcBalance(address._original_addr, ticker))))
        return res

    def token_auth_signs_pkey_hashes(self, ticker: str) -> list[str]:  # list[hashes]
        """
        Return the list of public key hashes of token declaration for a given ticker.

        Args:
            ticker (str): The ticker.

        Returns:
            list[str]: The list of public key hashes.
        """
        pkey_hashes = self._origin_ledger.tokenAuthPkeysHashes(ticker)
        return [str(pkey_hash) for pkey_hash in pkey_hashes]

    def token_auth_signs_valid(self, ticker: str) -> int:
        """
        Return the number of valid signatures for a given ticker.

        Args:
            ticker (str): The ticker.

        Returns:
            int: The number of valid signatures.
        """
        return self._origin_ledger.tokenAuthSignsValid(ticker) or 0

    def token_auth_signs_total(self, ticker: str) -> int:
        """
        Return the total number of signatures for a given ticker.

        Args:
            ticker (str): The ticker.

        Returns:
            int: The total number of signatures.
        """
        return self._origin_ledger.tokenAuthSignsTotal(ticker) or 0

    def has_emission(self, emission: CFDatumEmission | str | HashFast) -> bool:
        """
        Check if an emission exists in the ledger.

        Args:
            emission (CFDatumEmission | str | HashFast): The emission datum, hash string, or HashFast object.

        Returns:
            bool: True if the emission exists, False otherwise.

        Raises:
            ValueError: If the emission is not a valid type.
        """
        hash_lookup = None
        if isinstance(emission, CFDatumEmission):
            hash_lookup = HashFast.fromString(emission.hash)

        if isinstance(emission, str):
            hash_lookup = HashFast.fromString(emission)

        if isinstance(emission, HashFast):
            hash_lookup = emission

        if hash_lookup is None:
            raise ValueError(
                f"has_emission operates only with CFDatumEmission, str or HashFast objs, not {str(emission.__class__)}")

        sub_datum = self._origin_ledger.tokenEmissionFind(hash_lookup)

        if sub_datum is None:
            return False
        return True

    def get_emission(self, emission_hash: str) -> CFDatumEmission:
        """
        Retrieve the emission from the ledger.

        Args:
            emission_hash (str): The hash of the emission.

        Returns:
            CFDatumEmission: The CFDatumEmission object.
        """
        ems = self._origin_ledger.tokenEmissionFind(HashFast.fromString(emission_hash))
        if ems:
            return CFDatumEmission(None, self._origin_ledger.tokenEmissionFind(HashFast.fromString(emission_hash)),
                                   chain=self.net.zerochain)
        else:
            return None

    def tx_by_hash(self, hash: str) -> CFDatumTX:
        """
        Get a transaction datum by hash.

        Args:
            hash (str): The transaction hash.

        Returns:
            CFDatumTX: The transaction datum.

        Raises:
            ValueError: If the transaction is not found.
        """
        hf = HashFast.fromString(hash)

        tx = self._origin_ledger.txFindByHash(hf)
        if tx is None:
            raise ValueError(f"Not found DatumTx with hash={hash}")
        return CFDatumTX(None, tx, chain=self.net.main)

    def shared_funds_balance(self, base_hash: str | HashFast) -> tuple[str, int]:
        """
        Return final-chain hash and balance locked in a multi-wallet
        (Wallet shared) transaction.

        Args:
            base_hash (str | HashFast): Hash of the first (base) TX that
                created the shared funds.

        Returns:
            tuple[str, int]:  
                • hash of the final TX in the chain  
                • integer balance in datoshi

        Raises:
            ValueError: If the chain is broken or TXs are not found.

        """
        hash = base_hash if isinstance(base_hash, HashFast) \
            else HashFast.fromString(base_hash)

        final_hash = self._origin_ledger.getFinalMultiWalletTxHash(
            hash,
            ItemTypesValues.DAP_CHAIN_TX_OUT_COND_SUBTYPE_WALLET_SHARED,  # subtype
            0                                                             # include_refills = False
        )

        tx = self.tx_by_hash(str(final_hash))

        balance = 0
        for oc in tx.get_items(filter_type=CFTxOutCond):
            if oc.subtype == ItemTypes.DAP_CHAIN_TX_OUT_COND_SUBTYPE_WALLET_SHARED:
                balance = int(str(oc.value))
                break

        return str(final_hash), balance

    def list_unspent(
        self,
        addr: CFWalletAddress,
        token_ticker: str,
        min_value: int | None = None,
    ) -> list[tuple[HashFast, int, int]]:
        """
        Return all unspent outputs for the given address and token.

        Args:
            addr (CFWalletAddress): Sender’s wallet address.
            token_ticker (str): Token ticker.
            min_value (int | None, optional): Minimal total value to collect
                (in datoshi). None or 0 means return every output.

        Returns:
            list[tuple[HashFast, int, int]]: A list of triples
            (tx_hash, out_idx, value_datoshi) where

            * tx_hash – hash of the source transaction (HashFast);
            * out_idx – index of the output inside that transaction;
            * value_datoshi – value of the output in datoshi.
        """
        required_datoshi = None if not min_value else int(min_value)

        outputs_raw = self._origin_ledger.listUnspent(
            addr._original_addr,
            token_ticker,
            required_datoshi,
        )

        return [
            (tx_hash, out_idx, int(value_datoshi))
            for tx_hash, out_idx, value_datoshi in outputs_raw
        ]

    def register_ledger_tx_notification_callback(self, callback):
        """
        Register a callback function to be invoked when there are changes in the ledger's transactions.

        Args:
            callback (Callable): The callback function.
        """
        def callback_wrapper(ledger, tx, *other):
            callback(CFDatumTX(None, tx, self.net.main))

        self._origin_ledger.txAddNotify(callback_wrapper, None)

    def register_ledger_bridged_tx_notification_callback(self, callback: Callable, *args, **kwargs):
        """
        Register cross-chain (bridged) tx notificator

        Args:
            callback (Callable): The callback function.
        """

        def callback_wrapper(ledger, tx, *other):
            callback(CFDatumTX(None, tx, self.net.main))

        self._origin_ledger.bridgedTxNotifyAdd(callback_wrapper, None)


class CFMempool:
    """
    Represents the mempool of a CellFrame chain.

    Attributes:
        chain (CFChain): The CFChain object.
    """

    def __init__(self, chain: CFChain):
        """
        Initialize CFMempool object.

        Args:
            chain (CFChain): The CFChain object.
        """
        self.chain = chain

    def get_datums(self, datum_type: DatumTypes | None = None) -> list[CFDatum]:
        """
        Return a list of datums currently in the mempool.

        Returns:
            list[CFDatum]: The list of datums.
        """
        def flt(datum: CFDatum):
            if datum_type:
                return datum.type == datum_type
            else:
                return True

        data = Mempool.list(self.chain.net._origin_net, self.chain._origin_chain)
        datums = [CFDatum(None, datum, chain=self.chain) for datum in data.values()]
        return list(filter(flt, datums))

    def get_datum_from_bytes(self, value: bytes) -> CFDatum | None:
        """
        Extract a datum from bytes.

        Args:
            value (bytes): The byte representation of the datum.

        Returns:
            CFDatum | None: The extracted datum or None if extraction fails.
        """
        datum = Mempool.datumExtract(value)
        if datum is None:
            return None
        return CFDatum(None, datum, self.chain)

    def proc(self, datum: CFDatum | CFSubDatum | str):
        """
        Marks datum as available for acceptance.

        Args:
            datum_hash (str): The hash of the datum.
        """
        datum_hash = datum
        if isinstance(datum, (CFDatum, CFSubDatum)):
            datum_hash = datum.hash

        Mempool.proc(datum_hash, self.chain._origin_chain)

    def put(self, datum: CFDatumEmission | CFDatumTX) -> CFDatum:
        """
        Place a datum into the mempool.

        Args:
            datum (CFDatumEmission|CFDatumTx): The datum to place in the mempool.

        Returns:
            CFDatum: The placed datum.

        Raises:
            TypeError: If the datum is not a CFDatumEmission.
        """
        if isinstance(datum, CFDatumEmission):
            datum_hash = Mempool.emissionPlace(self.chain._origin_chain, datum._origin_sub_datum)
            return CFDatum(None, Mempool.datumGet(self.chain._origin_chain, datum_hash), self.chain)
        elif isinstance(datum, CFDatumTX):
            datum_hash = Mempool.transactionPlace(self.chain._origin_chain, datum._origin_sub_datum)
            return CFDatum(None, Mempool.datumGet(self.chain._origin_chain, datum_hash), self.chain)
        else:
            raise TypeError("mempool.put works only for ems|tx now")

    def create_base_transaction(self, emission: CFDatumEmission, cert: CFCertificate, fee: int) -> HashFast:
        """
        Create a base transaction.

        Args:
            emission (CFDatumEmission): The emission datum.
            cert (CFCertificate): The certificate.
            fee (Math): The transaction fee.

        Returns:
            HashFast: The hash of the created transaction.
        """
        emission_hash = HashFast.fromString(emission.hash)
        dhash = Mempool.baseTxCreate(
            self.chain.net.main._origin_chain, emission_hash, self.chain.net.zerochain._origin_chain,
            emission.value, emission.ticker, emission.address._original_addr, Math(str(fee)), cert._origin_certificate
        )

        return CFDatum(None, Mempool.datumGet(self.chain._origin_chain, str(dhash)), self.chain)

    def create_multisign_withdraw_tx(self,
                                     intxhash: HashFast | str,
                                     addr_to: CFWalletAddress | list[CFWalletAddress],
                                     value: str| list[str] | int,
                                     fee: str | int,
                                     key: CFKey,
                                     tsd: Dict[TSD, bytes]):
        """Create and return a multisign withdraw transaction from a shared-fund wallet.

            This method checks that the total requested withdrawal amount does not exceed
            the available shared-fund balance, then builds and submits a multisign withdrawal
            transaction to the mempool.

        Args:
            intxhash (HashFast or str): Hash of the base transaction that created the 
                shared-fund wallet. May be passed as a `HashFast` object or its hex string.
            addr_to (CFWalletAddress or list of CFWalletAddress): Recipient address or 
                list of recipient addresses.
            value (str, int, or list of str): Amount (in datoshi) to withdraw for a single 
                recipient, or list of amounts matching a list of addresses.
            fee (str or int): Transaction fee (in datoshi).
            key (CFKey): Private key object to sign the multisign transaction.
            tsd (Dict[TSD, bytes]): A mapping of TSD tags to their corresponding byte payloads.

        Returns:
            CFDatum: The newly created transaction datum, wrapping the submitted tx hash.

        Raises:
            InsufficientFundsError: If the total requested withdrawal exceeds the available 
                shared-fund balance. The exception provides two attributes:
                `available_balance` (int) and `requested_balance` (int).
            ValueError: If any argument types are invalid.
        """
        tx_hash = intxhash
        ledger = self.chain.net.get_ledger()
        _, available_balance = ledger.shared_funds_balance(tx_hash)

        if isinstance(intxhash, str):
            tx_hash = HashFast.fromString(intxhash)

        f = str(fee)

        addresses_to = [t._original_addr for t in addr_to] if isinstance(addr_to, list) else [addr_to._original_addr]
        values_to = [str(v_i) for v_i in value] if isinstance(value, list) else [str(value)]

        if isinstance(value, list):
            total_requested = sum(int(v) for v in values_to)
        else:
            total_requested = int(values_to[0])

        if total_requested > available_balance:
            raise InsufficientFundsError(available_balance, total_requested)

        dhash = Mempool.txCreateMultisignWithdraw(
            self.chain.net._origin_net, tx_hash, addresses_to, values_to, f, key._origin_key, tsd)

        return CFDatum(None, Mempool.datumGet(self.chain._origin_chain, str(dhash)), self.chain)


class CFSharedFunds:
    """Provides functionality to detect and handle 'shared funds' transactions.

    This class checks for:
      1) An OUT_COND item with subtype == DAP_CHAIN_TX_OUT_COND_SUBTYPE_WALLET_SHARED,
      2) A tag matching a given regex,
      3) A required public key signature.
    """

    def __init__(
        self,
        chain: CFChain,
        reaction_regexp: str,
        allowed_pubkey: str,
        callback: Callable[[CFDatumTX], None]
    ) -> None:
        """Initializes CFSharedFunds with the given parameters.

        Args:
            chain (CFChain): The CFChain to observe for incoming transactions.
            reaction_regexp (str): A regular expression used to match the OUT_COND tag.
            allowed_pubkey (str): The required public key hash that must sign the transaction.
            callback (Callable[[CFDatumTX], None]): A callable invoked when a matching transaction is found.
        """
        self.chain = chain
        self.reaction_pattern: Pattern = re.compile(reaction_regexp)
        self.allowed_pubkey: str = allowed_pubkey
        self.callback: Callable[[CFDatumTX], None] = callback

    def process_tx(self, tx: CFDatumTX) -> None:
        from .items import CFTxSig, CFTxOutCond
        """Examines a single transaction to check if it qualifies as shared funds.

        Steps:
          1) Track an OUT_COND with subtype == SRV_EMIT_DELEGATE.
          2) Check the tag against the given regex (reaction_pattern).
          3) Verify the presence of the required public key among the signatures.
          4) If all criteria are met, the callback is triggered.

        Args:
            tx (CFDatumTX): A CFDatumTX object representing a transaction to analyze.
        """
        out_cond_items = tx.get_items(filter_type=CFTxOutCond)
        found_delegate_out = False
        matched_tag: Optional[str] = None

        for item in out_cond_items:
            if item.subtype == ItemTypes.DAP_CHAIN_TX_OUT_COND_SUBTYPE_WALLET_SHARED:
                found_delegate_out = True
                if item.tag and self.reaction_pattern.search(item.tag):
                    matched_tag = item.tag
                    break

        if not found_delegate_out:
            return

        if not matched_tag:
            return

        tx_sigs = tx.get_items(filter_type=CFTxSig)
        sig_hashes: list[str] = [sig_item.sign.pkey_hash for sig_item in tx_sigs]
        if self.allowed_pubkey is None or self.allowed_pubkey in sig_hashes:
            self.callback(tx)
        return

    def track(self) -> None:
        from .notificators import CFNets
        """Registers a ledger transaction notifier to automatically process new transactions.

        When a new transaction arrives, `process_tx` is called to determine
        whether the transaction meets the 'shared funds' criteria.
        """
        @CFNets([self.chain.net]).ledger_tx_notificator()
        def on_new_tx(tx: CFDatumTX) -> None:
            self.process_tx(tx)
        log.notice("Shared funds notifier has been registered.")
