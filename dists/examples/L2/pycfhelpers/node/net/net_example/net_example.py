from pycfhelpers.node.net import (CFNet, CFChain, CFMempool)
from pycfhelpers.common.types import ChainTypes, ItemTypes
from pycfhelpers.node.logging import CFLog
from pycfhelpers.node.types import CFNetState, datum_hash
from pycfhelpers.node.consensus import CFEvent, CFBlock
from pycfhelpers.node.datums import (CFDatumToken, CFDatumEmission,
                                     CFDatum, CFDatumTX)
from typing import Literal
import itertools



# Below is an example of working with the described classes and their methods,
# as well as the sequence of actions for registering notificators.

# The basic information will be presented in the form of logs.
log = CFLog()

# Configure example according to your needs.
NUM_OF_DATUMS_ATOMS = 5
KELVPN_NET = CFNet("KelVPN")
wallet_address = "jrmnGqeeds4Dp67Ace1RavHGPwUUkcqYmLzdDxR6FhwfxTq7uDWP2rQKZpKCoUaTCQnRcACyUwgfCtPsZfZiRNmvq1YWmiyRFZ1meVD7"

# Next, write the functions corresponding to your scenarios.
# In this example, basic information about all the features of the
# module will be displayed


# Get all available information for the specified network.

def get_net_info(net) -> str:
    log.notice(f"Name: {net.name}")
    log.notice(f"My addr: {net.address}")
    # Since net.id represents CFNetID instance,
    # use method "long" to see a value.
    log.notice(f"ID: {net.id.long}")
    log.notice(f"Chains: {[chain.name for chain in net.chains]}")
    log.notice(f"GDB Group alias: {net.group_alias}")
    log.notice("The network fee data")
    # The fee data can also be output using the command:
    # cellframe-node-cli net -net <name of network> get fee
    # If network is offline, the following values will be
    # shown as zeros, or nones.
    log.notice(f"Transaction fee: {net.fee_data.tx_fee}")
    log.notice(f"Transaction fee address: {net.fee_data.tx_fee_addr}")
    log.notice(f"The average validator fee: {net.fee_data.validator_avg_fee}")
    log.notice(f"The maximum validator fee: {net.fee_data.validator_max_fee}")
    log.notice(f"The minimum validator fee: {net.fee_data.validator_min_fee}")
    log.notice(f"The native ticker: {net.fee_data.native_ticker}")

# The chain object (CFChain instance) can be obtained as attribute of
# CFNet object. Each network has two chains: net.main and net.zerochain.
# CFChain instance can also be initialized via the network and the name
# of the chain. for example: chain = CFChain(KELVPN_NET, "main").


# Get information about spesified chain.

def get_chain_info(chain) -> str:
    log.notice(f"The {chain.net.name} {chain.name} data")
    log.notice(f"Type: {chain.type}")


# Retrieve information about the atoms represented in the chain.

def get_atoms_info(chain) -> str:

    # Since chain.get_atoms() returns iterator,
    # it is convinient to access it using the module itertools.

    # If the network is offline, then there is no access to it's
    # elements and all generators do not return anything.

    iterator = chain.get_atoms()

    if not any(iterator):
        log.notice("There are no atoms in chain")
        return

    # For example, take the fist 5 atoms from the generator.
    log.notice(f"The fisrt {NUM_OF_DATUMS_ATOMS} atoms in chain")

    # For more information, refer to the official documentation:
    # https://docs.python.org/3/library/itertools.html#itertools.islice

    atoms = list(itertools.islice(iterator, NUM_OF_DATUMS_ATOMS))

    # Atoms in chains with esbocs type are always CFBlock instances,
    # in chains with dag_poa type are always CFEvent instances.
    for atom in atoms:
        if isinstance(atom, CFBlock):
            log.notice(f"Atom have an CFBlock type with hash {atom.hash}")

        elif isinstance(atom, CFEvent):
            log.notice(f"Atom have an CFEvent type with hash {atom.hash}")


# Retrieve information about the datums with specified type from the chain.

def get_datums_info(chain, type) -> str:

    # chain.get_datums() also returns iterator, refer to it's elements through
    # a loop or use the module itertools. Filter datums by specified type,
    # that should be an instance of CFSubDatum. If the the type is not
    # specified, the generator returns all datums in all atoms in the chain.
    if not any(chain.get_datums(type)):
        log.notice(f"There are no datums in chain with type: {type}")
        return

    log.notice(f"The fisrt {NUM_OF_DATUMS_ATOMS} datums")
    log.notice(f"in chain with type: {type}")
    iterator = chain.get_datums()
    datums = list(itertools.islice(iterator, NUM_OF_DATUMS_ATOMS))

    for datum in datums:
        log.notice(f"{datum.hash}")


# Get the last CFDatum object with specified type in the chain.

def get_last_datum(chain, type) -> CFDatum:

    if not any(chain.get_datums()):
        log.notice(f"There are no datums in chain with type: {type}")
        return

    iterator = chain.get_datums()
    datum = list(itertools.islice(iterator, 1))[0]

    return datum


# Retrieve information about the trasactions from the chain.

def get_transaction_info(chain) -> str:

    # There are no datums with transaction type in DAG chains.
    if chain.type == ChainTypes.dag_poa:
        log.notice(f"There are no transactions in {chain.net.name} {chain.name}")
        return

    log.notice(f"Transactions info in {chain.net.name} {chain.name}")

    iterator = chain.get_transactions()
    transactions = list(itertools.islice(iterator, NUM_OF_DATUMS_ATOMS))

    # The separator is used for the readability of the output.
    for transaction in transactions:
        sep = "$$$$"
        log.notice(f"{sep * 13}")
        log.notice(f"Created at{transaction.created_at}")
        log.notice(f"Ticker: {transaction.ticker}")
        log.notice(f"Ledger cache response: {transaction.ledger_rc}")
        log.notice(f"Accepted: {transaction.accepted}")


# Get the last CFDatumTX object in the chain.

def get_last_tx(chain) -> CFDatumTX:

    iterator = chain.get_transactions()

    if not any(iterator):
        log.notice("There are no transactions in chain")
        return

    transaction = list(itertools.islice(iterator, 1))[0]
    return transaction


    # *** нужно сделать итерацию по айтемам и их вывод***

def get_items_info(datum_tx: CFDatumTX):

    # It is possible to get all items associated with specified
    # transaction trough the property CFDatumTX.items.
    # The items are enclosed in a list.
    all_items_list = datum_tx.items

    if not any(all_items_list):
        log.notice(f"There are no items in the specified datum")
        return
    else:
        for item in all_items_list:
            log.notice(f"Item type {item.type}")


# To get items of only a certain type,
# use the CFDatumTX.get_items(type) method:

def get_items_with_type(datum_tx, type):

    items_list = datum_tx.get_items(type)
    # All item types are available as 
    # attributes of the ItemTypes class.
    if not any(items_list):
        log.notice(f"There are no items with type: {type} in the datum")
        return
    else:
        num = len(items_list)
        log.notice(f"The number of items with type: {type} - {num}")


# The CFLedger object can be obtained from net_object.get_ledger().
# Initialization CFLedger not supported via Python.

# Retrieve information about specified transaction from ledger

def tx_info_from_ledger(ledger, tx) -> str:

    tx_ticker = ledger.get_tx_ticker(tx)
    tx_ledger_rc = ledger.get_tx_ledger_rc(tx)
    log.notice("Transaction info from ledger")
    log.notice(f"Ticker {tx_ticker}")
    log.notice(f"Ledger cache response: {tx_ledger_rc}")


# Retrieve information about specified datum from ledger

def get_signs_info(ledger, datum) -> str:

    valid = ledger.token_auth_signs_valid(datum)
    total = ledger.token_auth_signs_total(datum)
    pkeys_hashes = ledger.token_auth_signs_pkey_hashes(datum)
    log.notice(f"Check for signature signs for {datum.hash}")
    log.notice(f"The number of valid signature signs: {valid}")
    log.notice(f"The number of total signature signs: {total}")
    log.notice("The hashes of public keys for signing ticker emission:")
    for hash in pkeys_hashes:
        log.notice(f"{hash}")


# Obtain balance information by specified address.

def get_balance_info(ledger, adress) -> str:
    balances = ledger.calc_address_balances(adress)
    log.notice(f"Balances for address: {adress}:")

    if not balances:
        log.notice("Empty")
    else:
        for ticker, balance in balances.items():
            log.notice(f"{ticker} : {balance}")


# Check that the emission exists in the ledger
# using the method ledger.has_emission().

def chech_for_emission(ledger, emission) -> str:
    log.notice("Emission with hash:")
    log.notice(f"{emission}")
    if ledger.has_emission(emission):
        log.notice("exists in the ledger")
    else:
        log.notice("does not exist in the ledger")


# Create notification functions and register them in the "init" function.
# For example, output the arguments obtained by this functions.

def gdbsync_notification(_, op_code, group, key, value, *args, net, **kwargs):
    # For groups related to a mempool, it is 
    # possible to decode the values using 
    # CFMempool.get_datum_from_bytes(value) method:
    try:
        mempool = CFMempool(net.main)
        value = mempool.get_datum_from_bytes(value)
    except AttributeError:
        value = value
    log.notice(f"GDB synchronization in the {net.name} with parametrs:")
    log.notice(f"{op_code=},{group=},{key=},{value=}")


def mempool_notification(op_code: Literal["a", "d"], datum: CFDatum | datum_hash, *args, chain: CFChain, **kwargs):
    log.notice(f"Changes in the mempool {chain.net.name} {chain.name}:")
    if isinstance(datum, CFDatum):
        hash_str = datum.hash
    else:
        hash_str = datum_hash

    if op_code == "a":
        log.notice(f"New datum with hash {hash_str} added to the mempool.")

    elif op_code == "d":
        log.notice(f"Datum with hash {hash_str} deleted from the mempool.")
    else:
        log.notice("Unknown operation code")


def atom_notification(atom, size, *args, chain, **kwargs):

    log.notice(f"New atom in {chain.net.name} {chain.name}.")
    log.notice(f"Size: {size}")
    log.notice(f"Hash: {atom.hash}")


def ledger_notification(ledger, tx, *args, net, **kwargs):

    # legder here is not CFLedger instance, but CellFrame.Chain.Ledger object.
    log.notice(f"New transaction in {net.name} Ledger:")
    log.notice(f"Hash: {tx.hash}")


def init():
    separator = "-----" * 6
    # for example set the KELVPN_NET network online
    KELVPN_NET.change_state(CFNetState.NET_STATE_ONLINE)

    # ------------------------------------------------------------------------
    log.notice(f"\n{separator}NETWORKS DATA{separator}")

    # Get the list of active CFNet objects, representing a network.
    active_nets = CFNet.active_nets()

    # Output available information about the network.

    for net in active_nets:
        # for each of active network output
        # all available information.
        get_net_info(net)

    # Register all notification functions here.
    KELVPN_NET.register_gdbsync_notification_callback(gdbsync_notification)
    for chain in KELVPN_NET.chains:
        chain.register_mempool_notification_callback(mempool_notification)
        chain.register_atom_notification_callback(atom_notification)
    # ------------------------------------------------------------------------
    log.notice(f"{separator}CHAINS DATA{separator}")

    # Output information about the chains of the specified network.

    for chain in KELVPN_NET.chains:

        get_chain_info(chain)

        get_atoms_info(chain)

        get_datums_info(chain, CFDatumToken)

        get_transaction_info(chain)
    # ------------------------------------------------------------------------
    log.notice(f"{separator}LEDGER DATA{separator}")

    # Get the CFLedger instanse as attribube of the spesific network.
    ledger = KELVPN_NET.get_ledger()
    ledger.register_ledger_tx_notification_callback(ledger_notification)

    # Choose spesific chain to check datums info in the ledger.
    chain = KELVPN_NET.main

    # Retrieve last transaction in the network in main chain
    # and output data about it.
    tx = get_last_tx(chain)
    if tx is not None:
        tx_info_from_ledger(ledger, tx)

    get_items_info(tx)
    get_items_with_type(tx, ItemTypes.TX_ITEM_TYPE_IN)

    # Get the last datum with type CFDatumToken.
    last_datum = get_last_datum(chain, CFDatumToken)
    if last_datum is not None:
        get_signs_info(ledger, last_datum)

    get_balance_info(ledger, wallet_address)

    # Get the datum with CFDatumEmission type:
    datum_emission = get_last_datum(chain, CFDatumEmission)
    if datum_emission is not None:
        chech_for_emission(ledger, datum_emission)

    return 0
