from typing import Iterator
import CellFrame
from CellFrame.Chain import ChainAtomPtr
from CellFrame.Network import Net
from CellFrame.Common import Datum, DatumTx, DatumToken, DatumEmission, DatumAnchor, DatumDecree
from DAP import Crypto
from DAP.Crypto import Cert, Sign
from CellFrame.Chain import Mempool, Wallet, Chain
from CellFrame.Consensus import DAG, Block
from CellFrame.Common import TxOut, TxIn, TxToken, TxSig, TxOutCondSubtypeSrvStakeLock, TxInCond, \
    TxOutExt
from DAP.Crypto import HashFast
from DAP.Core import logIt
from datetime import datetime
from CellFrame.Chain import ChainAddr
import hashlib
import sys
import json
from pycfhelpers.helpers import json_dump, find_tx_out, get_tx_items


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


class CellframeEmission:
    """
    Represents a Cellframe emission.

    Attributes:
        datum (DatumEmission): The emission datum.
        event (DAG | None, optional): The DAG event associated with 
            the emission. Defaults to None.
        hash (str): The hash of the emission.
        uid (int): The unique ID of the emission.
    """

    def __init__(self, datum: DatumEmission, event: DAG = None):
        """
        Initialize a CellframeEmission instance.

        Args:
            datum (DatumEmission): The emission datum.
            event (DAG | None, optional): The DAG event associated with the emission. Defaults to None.
        """
        self.datum = datum
        self.hash = str(self.datum.hash)
        self.event = event

        m = hashlib.sha256()
        addr = datum.getTSD(TSD.TYPE_ADDRESS)
        btx = datum.getTSD(TSD.TYPE_BASE_TX_HASH)
        otx = datum.getTSD(TSD.TYPE_OUTER_TX_HASH)
        src = datum.getTSD(TSD.TYPE_SOURCE)
        stp = datum.getTSD(TSD.TYPE_SOURCE_SUBTYPE)
        ts = datum.getTSD(TSD.TYPE_TIMESTAMP)
        data = datum.getTSD(TSD.TYPE_DATA)
        uid = datum.getTSD(TSD.TYPE_UNIQUE_ID)

        m.update(str(addr).encode("utf-8"))
        m.update(str(btx).encode("utf-8"))
        m.update(str(otx).encode("utf-8"))
        m.update(str(src).encode("utf-8"))
        m.update(str(stp).encode("utf-8"))
        m.update(str(data).encode("utf-8"))
        m.update(str(ts).encode("utf-8"))
        m.update(str(uid).encode("utf-8"))
        m.update(str(data).encode("utf-8"))

        self.uid = m.hexdigest()

    def getTSD(self, type):
        """
        Get the TSD value for a given type.

        Args:
            type (int): The type of TSD to retrieve.

        Returns:
            (str | None, optional) : The TSD value if found decoded in
                the format utf-8, otherwise None.
        """
        tsd = self.datum.getTSD(type)
        if tsd:
            try:
                return tsd.decode("utf-8")
            except:
                pass
        return None

    def setTSD(self, type, data):
        """
        Set the TSD value for a given type.

        Args:
            type (int): The type of TSD to set.
            data (str): The data to set for the TSD.
        """
        self.datum.addTSD(type, data)


# if datum not base-tx - exception
class CellframeBaseTransactionDatum:
    """
    The Cellframe base transaction datum.

    Attributes:
        datum (DatumEmission): The base transaction datum.
        block (Any): The block associated with the datum.
        hash (str): The hash of the datum.
        created (datetime): The creation date of the datum.
        net (Net): The Cellframe network associated with the datum.
        to_address (): The recipient address of the transaction.
        amount (int): The amount of the transaction.
        emission_hash (str): The hash of the associated emission.
        emission_ (): The CellframeEmission instance associated 
            with the emission.
    """

    def __init__(self, datum, net, block=None):
        """Initialize a CellframeBaseTransactionDatum instance.

        Args:
            datum (DatumEmission): The base transaction datum.
            net (Net): The Cellframe network associated with the datum.
            block (Any | None, optional): The block associated with the
                datum. Defaults to None.

        Raises:
            RuntimeError: If datum not base of transaction.
        """
        self.block = block
        self.datum = datum
        self.hash = str(datum.hash)
        self.created = datum.dateCreated
        self.net = net
        # base tx : has txToken item
        if not self.tx_token():
            raise RuntimeError("Datum {} not base tx".format(self.datum))

        self.to_address = str(self.tx_out().addr)
        self.amount = self.tx_out().value

        self.emission_hash = str(self.tx_token().tokenEmissionHash)

        self.emission_ = None

    def tx_out(self):
        """
        Retrieve the transaction output associated with the base transaction datum.

        Returns:
            (TxOut | None, optional): The transaction output, if present,
                otherwise None.
        """
        try:
            return next(filter(lambda x: isinstance(x, (TxOut,)), self.datum.getItems()))
        except:
            return None

    def tx_in(self):
        """
        Retrieve the transaction input associated with the base transaction datum.

        Returns:
            (TxIn | None, optional): The transaction input, if present,
                otherwise None.
        """
        try:
            return next(filter(lambda x: isinstance(x, (TxIn,)), self.datum.getItems()))
        except:
            return None

    def tx_token(self):
        """
        Retrieve the token associated with the base transaction datum.

        Returns:
            (TxToken | None, optional): The token, if present, otherwise None.
        """
        try:
            return next(filter(lambda x: isinstance(x, (TxToken,)), self.datum.getItems()))
        except:
            return None

    def tx_sig(self):
        """
        Retrieve the signature associated with the base transaction datum.

        Returns:
            (TxSig | None, optional): The signature, if present,
                otherwise None.
        """
        try:
            return next(filter(lambda x: isinstance(x, (TxSig,)), self.datum.getItems()))
        except:
            return None

    def emission(self):
        """
        Retrieve the emission associated with the base transaction datum.

        Returns:
            (CellframeEmission | None, optional): The emission, if present,
                otherwise None.
        """
        if not self.emission_:
            # tiker = str(self.tx_token().ticker)
            hf = HashFast.fromString(str(self.tx_token().tokenEmissionHash))
            ledger = self.net.getLedger()
            ems = ledger.tokenEmissionFind(tiker, hf)
            if not ems:
                return None
            self.emission_ = CellframeEmission(ems)

        return self.emission_


class CellframeNetwork:
    """
    The Cellframe network.

    Attributes:
        main (Chain): The main chain of the network.
        zerochain (Chain): The zerochain of the network.
        name (str): The name of the network.
        net (Net): The Cellframe network instance.
        commision_wallet (Any): The commission wallet associated
            with the network.
    """

    main: Chain
    zerochain: Chain

    def __init__(self, name, chains, group_alias=None, commision_wallet=None):
        """
        Initializes a CellframeNetwork instance.

        Args:
            name (str): The name of the network.
            chains (List[str]): The list of chain names associated
                with the network.
            group_alias (Any | None, optional): The group alias of the network.
                Defaults to None.
            commision_wallet (Any | None, optional): The commission wallet
                associated with the network. Defaults to None.

        Raises:
            RuntimeError: If network with specified name does not exist.
        """

        self.name = name
        self.net = Net.byName(name)
        # self.group_alias = group_alias or name
        self.commision_wallet = commision_wallet

        if not self.net:
            raise RuntimeError("No such net: {}".format(name))

        for chain in chains:
            setattr(self, chain, self.net.getChainByName(chain))

    @property
    def group_alias(self) -> str:
        """
        Retrieve the group alias of the network.

        Returns:
            str: The group alias.
        """
        return self.net.gdb_group_alias

    @staticmethod
    def wallet_from_signature(sigbytes):
        """
        Retrieve the wallet address from the given signature bytes.

        Args:
            sigbytes (bytes): The signature bytes.

        Returns:
            str: The wallet address.
        """
        sign = Sign.fromBytes(sigbytes)
        return sign.getAddr()

    @staticmethod
    def netid_from_wallet(wallet):
        """
        Retrieve the net ID from the given wallet address.

        Args:
            wallet (str): The wallet address.

        Returns:
            int: The net ID.
        """
        return ChainAddr.fromStr(str(wallet)).getNetId().long()

    def tx_sender_wallet(self, tx):
        """
        Retrieve the sender wallet address from the given transaction.

        Args:
            tx (Any): The transaction.

        Returns:
           (Sign | None, optional): The sender wallet address,
            if found, otherwise None.
        """
        sigitem = get_tx_items(tx, TxSig)
        if not sigitem:
            return None

        # first signature is a sender signature
        return sigitem[0].sign.getAddr(self.net)

    def ledger_tx_by_hash(self, txh):
        """
        Retrieve the ledger transaction by its hash.

        Args:
            txh (str): The transaction hash.

        Returns:
            tx (str): The ledger transaction.
        """
        hf = HashFast.fromString(txh)

        # only ledger-accepted
        ledger = self.net.getLedger()
        tx = ledger.txFindByHash(hf)

        return tx

    def netid(self):
        """
        Retrieve the net ID of the network.

        Returns:
            int: The net ID.
        """
        return self.net.id.long()

    def set_mempool_notification_callback(self, chain, callback):
        """
        Set the mempool notification callback for the given chain.

        Args:
            chain (Chain): The chain.
            callback (Callable): The callback function.
        """
        callback_name = "{}".format(self.name)
        logIt.notice("New mempool notifier for {}".format(callback_name))

        def callback_wraper(op_code, group, key, value, net_name):
            # op_code a | d
            # group - table name
            # key - record hash
            #
            callback(op_code, group, key, value, net_name, self, chain)

        chain.addMempoolNotify(callback_wraper, callback_name)

    def set_gdbsync_notification_callback(self, callback):
        """
        Set the Global data base synchronization notification callback.

        Args:
            callback (Callable): The callback function.
        """
        # Any table in GDB
        callback_name = "{}".format(self.name)

        logIt.notice("New gdb notifier for {}".format(callback_name))

        def callback_wraper(op_code, group, key, value, net_name):
            callback(self, op_code, group, key, value, net_name)

        self.net.addNotify(callback_wraper, self.name)

    def set_atom_notification_callback(self, chain, callback):
        """
        Set the atom notification callback for the given chain.

        Args:
            chain (Chain): The chain.
            callback (Callable): The callback function.
        """
        # New atom Block | Event
        #
        callback_name = "{}".format(self.name)
        logIt.notice("New atom notifier for {}".format(callback_name))

        def callback_wraper(atom, size, callback_name):
            callback(atom, size, callback_name, self, chain)

        chain.addAtomNotify(callback_wraper, callback_name)

    def set_ledger_tx_notification_callback(self, callback):
        """
        Set the ledger transaction notification callback.

        Args:
            callback (Callable): The callback function.
        """
        ledger = self.net.getLedger()

        def callback_wrapper(ledger, tx, argv):
            callback(ledger, tx, argv, self)

        ledger.txAddNotify(callback_wrapper, self.net)

    def set_ledger_bridge_tx_notification_callback(self, callback):
        """
        Set the ledger bridge transaction notification callback.

        Args:
            callback (Callable): The callback function.
        """
        ledger = self.net.getLedger()

        def callback_wrapper(ledger, tx, argv):
            callback(ledger, tx, argv, self)

        logIt.notice("New bridgedTxNotify for {}".format(self.net))
        ledger.bridgedTxNotifyAdd(callback_wrapper, self.net)

    def load_cert(certname):
        """
        Load the certificate with the given name.

        Args:
            certname (str): The name of the certificate.

        Returns:
            Any: The loaded certificate.
        """
        return Crypto.Cert.load(certname)

    def extract_emission_from_mempool_nofitication(self, chain, value):
        """
        Extract the emission from the mempool notification.

        Args:
            chain (Chain): The chain.
            value (Any): The value.

        Returns:
            (CellframeEmission | None, optional): The extracted emission,
                if found, otherwise None.
        """
        ems = Mempool.emissionExtract(chain, value)
        if ems:
            return CellframeEmission(ems)
        else:
            return None

    def create_base_transaction(self, emission, certs, fee, native_tw=None):
        """
        Create a base transaction.

        Args:
            emission (CellframeEmission): The emission associated with
                the transaction.
            certs (str): The certificates to use for the transaction.
            fee (int): The fee for the transaction.
            native_tw (str | None, optional): The native transaction wallet.
                Defaults to None.

        Returns:
            The created base transaction.
        """
        if native_tw:
            w = Wallet.openFile(native_tw)
            return Mempool.baseTxCreate(self.main, emission.datum.hash, self.zerochain, emission.datum.value,
                                        emission.datum.ticker,
                                        emission.datum.addr, fee, w)
        else:
            return Mempool.baseTxCreate(self.main, emission.datum.hash, self.zerochain, emission.datum.value,
                                        emission.datum.ticker,
                                        emission.datum.addr, fee, certs)

    def get_emission_by_tsd(self, tsd_dict):
        """
        Retrieve the emission by the given TSD dictionary.

        Args:
            tsd_dict (Dict[str, Any]): The TSD dictionary.

        Returns:
            Dict[str, CellframeEmission]: The retrieved emissions.
        """
        atom_count = self.zerochain.countAtom()
        atoms = self.zerochain.getAtoms(atom_count, 1, True)

        emissions = {}

        for atom in atoms:

            # event = DAG.fromAtom(atom[0], atom[1])
            event = DAG.fromAtom(atom)
            if not event.datum.isDatumTokenEmission():
                continue

            token_emission = event.datum.getDatumTokenEmission()
            if not token_emission:
                continue

            results = []
            for key, value in tsd_dict.items():

                tsd = token_emission.getTSD(key)

                if not tsd and value == None:
                    results.append(True)
                    continue
                try:
                    if tsd and tsd.decode("utf-8") == value:
                        results.append(True)
                        continue
                except:
                    pass

                results.append(False)

            if all(results):
                emissions[str(event.datum.hash)] = CellframeEmission(token_emission, event)

        return emissions

    def get_emission_from_mempool_by_tsd(self, tsd_dict):
        """
        Retrieve the emission from the mempool by the given TSD dictionary.

        Args:
            tsd_dict (Dict[str, Any]): The TSD dictionary.

        Returns:
            Dict[str, CellframeEmission]: The retrieved emissions.
        """
        datums = Mempool.list(self.net, self.zerochain).values()

        emissions = {}

        for datum in datums:

            if not datum.isDatumTokenEmission():
                continue

            token_emission = datum.getDatumTokenEmission()
            if not token_emission:
                continue

            results = []
            for key, value in tsd_dict.items():

                tsd = token_emission.getTSD(key)

                if not tsd and value == None:
                    results.append(True)
                    continue
                try:
                    if tsd and tsd.decode("utf-8") == value:
                        results.append(True)
                        continue
                except:
                    pass

                results.append(False)

            if all(results):
                emissions[str(datum.hash)] = CellframeEmission(token_emission)

        return emissions

    def base_transactions_from_blocks(self, emission_hash=None):
        """
        Retrieve base transactions from the blocks.

        Args:
            emission_hash (str | None, optional): The emission hash to filter
                the transactions, defaults to None.

        Yelds:
            Iterator[CellframeBaseTransactionDatum]: An iterator over the
                base transactions.

        Example:

        """
        iterator = self.main.createAtomItem(False)

        ptr = self.main.atomIterGetFirst(iterator)

        if not ptr:
            logIt.error("Can't iterate over blocks in {}!".format(self.name))
            return []

        aptr, size = ptr
        # iterate over blocks: atom-pointer should not be none, and size shoud be >0
        while aptr:

            if size <= 0:  # skip such blocks
                aptr, size = self.main.atomIterGetNext(iter)
                continue

            block = Block.fromAtom(aptr, size)

            if not block.datums:
                aptr, size = self.main.atomIterGetNext(iterator)
                continue

            for datum in block.datums:
                if datum.isDatumTX():
                    try:
                        # if emshash provided - filter items
                        basedatum = CellframeBaseTransactionDatum(datum.getDatumTX(), block)
                        if emission_hash:
                            if basedatum.emission_hash == emission_hash:
                                yield basedatum
                            else:
                                continue
                        else:
                            yield basedatum

                    except:
                        continue

            aptr, size = self.main.atomIterGetNext(iterator)

    def get_transactions_to_wallet_from_blocks(self, address):
        """
        Retrieve transactions to the given wallet address from the blocks.

        Args:
            address (str): The wallet address.

        Returns:
            List[CellframeBaseTransactionDatum]: The list of transactions
                to the wallet address.
        """

        def nextBlockDatums():
            iterator = self.createAtomItem(False)
            aptr, size = self.main.atomIterGetFirst(iterator)
            # iterate over blocks: atom-pointer should not be none, and size shoud be >0
            while aptr:

                if size <= 0:  # skip such blocks
                    aptr, size = self.main.atomIterGetNext(iter)
                    continue

                block = Block.fromAtom(aptr, size)

                if block.datums:
                    yield block, block.datums

                aptr, size = self.main.atomIterGetNext(iterator)

        def isDatumToAddress(datum_with_block):
            try:
                txn_out = next(filter(lambda x: isinstance(x, (TxOut,)), datum_with_block.datum.getItems()))
                return str(txn_out.addr) == address
            except Exception as e:
                return False

        transactions_to_wallet = []

        class DatumWithBlock:
            def __init__(self, datum, block):
                self.datum = datum
                self.block = block

        for block, datums in nextBlockDatums():
            tx_datums = [DatumWithBlock(datum.getDatumTX(), block) for datum in
                         filter(lambda datum: datum.isDatumTX(), datums)]
            transactions_to_wallet.extend(list(filter(isDatumToAddress, tx_datums)))

        return transactions_to_wallet

    def create_emission(self, wallet, token_symbol, value, tsd):
        """
        Create an emission.

        Args:
            wallet (str): The wallet address.
            token_symbol (str): The token symbol.
            value (str): The emission value.
            tsd (Dict[str, Any]): The TSD dictionary.

        Returns:
            CellframeEmission: The created emission.
        """
        addr = CellFrame.Chain.ChainAddr.fromStr(wallet)
        ems = DatumEmission(str(value), token_symbol, addr)

        for key, value in tsd.items():
            if isinstance(value, dict):
                ems.addTSD(key, json_dump(value).encode("utf-8"))
            elif isinstance(value, list):
                ems.addTSD(key, json_dump(value).encode("utf-8"))
            else:
                ems.addTSD(key, str(value).encode("utf-8"))

        return CellframeEmission(ems)

    def place_emission(self, ems, chain):
        """
        Place the emission in the mempool of the given chain.

        Args:
            ems (DatumEmission): The emission.
            chain (Chain): The chain.

        Returns:
            ???
        """
        return Mempool.emissionPlace(chain, ems.datum)

    def place_datum(self, datum, chain):
        """
        Place the datum in the mempool of the given chain.

        Args:
            datum (DatumEmission): The datum.
            chain (Chain): The chain.

        Returns:
            bool: True if successful, False otherwise.
        """
        return Mempool.addDatum(chain, datum)

    def remove_key_from_mempool(self, key, chain):
        """
        Remove the key from the mempool of the given chain.

        Args:
            key (Any): The key.
            chain (Chain): The chain.
        """
        Mempool.remove(chain, key)

    def mempool_list(self, chain):
        """
        List the mempool of the given chain.

        Args:
            chain (Chain): The chain.

        Returns:
            List[???]: The mempool.
        """
        return Mempool.list(self.net, chain)

    def mempool_get_emission(self, key):
        """
        Retrieve the emission from the mempool by the given key.

        Args:
            key (Any): The key.

        Returns:
           (CellframeEmission ???): The retrieved emission.
        """
        return Mempool.emissionGet(self.zerochain, key)

    def mempool_proc(self, hash, chain):
        """
        Process the mempool of the given chain.

        Args:
            hash (str): The hash.
            chain (Chain): The chain.
        """
        Mempool.proc(hash, chain)

    def all_tx_from_ledger(self):
        """
        Retrieve all transactions from the ledger.

        Returns:
            Tuple[List, Any]: A tuple containing the list of
                transactions and the ledger.
        """
        res = []
        legder = self.net.getLedger()
        count = legder.count()

        txs = legder.getTransactions(count, 1, False)

        if not txs:
            return [], legder

        return txs, legder

    @staticmethod
    def get_datums_from_atom(chain: Chain, atom: ChainAtomPtr) -> list[Datum]:
        """
        Retrieve datums from the atom.

        Args:
            chain (Chain): The chain.
            atom (Any): The atom.

        Returns:
            List: The retrieved datums.
        """
        # logIt.message(f"{chain.getCSName()=}")
        if chain.getCSName() == "esbocs":
            block = Block.fromAtom(atom)
            return block.datums

        if chain.getCSName() == "dag_poa":
            event = DAG.fromAtom(atom)
            return [event.datum]

    def get_datums_from_chains(self, chains: tuple[str] = ("main", "zerochain")) -> Iterator[Datum]:
        """
        Get datums from multiple chains.

        Args:
            chains (Tuple[str, ...]): The chains to retrieve datums from, 
                defaults to ("main", "zerochain").

        Yields:
           Iterator: An iterator over the retrieved datums.

        Example:

        """
        logIt.warning("get_datums_from_chains()")
        for chain_name in chains:
            chain: Chain = getattr(self, chain_name)

            iterator = chain.createAtomIter(False)
            ptr = chain.atomIterGetFirst(iterator)

            if not ptr:
                logIt.message("not ptr")
                return []

            atom, size = ptr
            logIt.message("...")
            while atom:
                if size <= 0:
                    atom, size = chain.atomIterGetNext(iterator)
                    continue
                datums = self.get_datums_from_atom(chain, atom)

                if not datums:
                    atom, size = chain.atomIterGetNext(iterator)
                    continue

                for datum in datums:
                    yield datum

                atom, size = chain.atomIterGetNext(iterator)

    def all_tx_from_blocks(self) -> Iterator[DatumTx]:
        """
        Retrieve all transactions from the blocks.

        Yelds:
            Iterator: An iterator over the retrieved transactions.

        Example:

        """
        logIt.warning("all_tx_from_blocks()")
        for datum in self.get_datums_from_chains(chains=("main",)):
            if datum.isDatumTX():
                yield datum.getDatumTX()
            else:
                continue
