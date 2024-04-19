from pycfhelpers.node.net import (CFNet, CFChain, CFMempool)
from pycfhelpers.common.types import ChainTypes
from pycfhelpers.node.logging import CFLog
from pycfhelpers.node.types import CFNetState, datum_hash
from pycfhelpers.node.consensus import CFEvent, CFBlock
from pycfhelpers.node.datums import (CFDatumToken, CFDatumEmission,
                                     CFDatum, CFDatumTX)
from typing import Literal
import itertools

# This module provides tools for interacting with CellFrame networks chains,
# ledgers and mempool, including access to network data, fee information,
#  network  state management, and mempool interaction. It enables handling of
# data objects such as blocks, transactions, and emissions, along with
# functionalities for processing them, such as address balance calculation,
# transaction handling, and notifications for mempool and another changes.

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
    message = f"\nName: {net.name}"
    message += f"\nAddress: {net.address}"
    # Since net.id represents CFNetID instance,
    # use method "long" to see a value.
    message += f"\nID: {net.id.long}"
    message += f"\nChains: {[chain.name for chain in net.chains]}"
    message += f"\nGroup alias: {net.group_alias}"
    message += "\nThe network fee data"
    # The fee data can also be output using the command:
    # cellframe-node-cli net -net <name of network> get fee
    # If network is offline, the following values will be
    # shown as zeros, or nones.
    message += f"\nTransaction fee: {net.fee_data.tx_fee}"
    message += f"\nTransaction fee address: {net.fee_data.tx_fee_addr}"
    message += f"\nThe average validator fee: {net.fee_data.validator_avg_fee}"
    message += f"\nThe maximum validator fee: {net.fee_data.validator_max_fee}"
    message += f"\nThe minimum validator fee: {net.fee_data.validator_min_fee}"
    message += f"\nThe native ticker: {net.fee_data.native_ticker}"
    return message

# The chain object (CFChain instance) can be obtained as attribute of
# CFNet object. Each network has two chains: net.main and net.zerochain.
# CFChain instance can also be initialized via the network and the name
# of the chain. for example: chain = CFChain(KELVPN_NET, "main").


# Get information about spesified chain.

def get_chain_info(chain) -> str:
    message = f"\nThe {chain.net.name} {chain.name} data"
    message += f"\nType: {chain.type}"
    return message


# Retrieve information about the atoms represented in the chain.

def get_atoms_info(chain) -> str:

    # Since chain.get_atoms() returns iterator,
    # it is convinient to access it using the module itertools.

    # If the network is offline, then there is no access to it's
    # elements and all generators do not return anything.

    iterator = chain.get_atoms()

    if not any(iterator):
        message = "\nThere are no atoms in chain"
        return message

    # For example, take the fist 5 atoms from the generator.
    message = f"\nThe fisrt {NUM_OF_DATUMS_ATOMS} atoms in chain"

    # For more information, refer to the official documentation:
    # https://docs.python.org/3/library/itertools.html#itertools.islice

    atoms = list(itertools.islice(iterator, NUM_OF_DATUMS_ATOMS))

    # Atoms in chains with esbocs type are always CFBlock instances,
    # in chains with dag_poa type are always CFEvent instances.
    for atom in atoms:
        if isinstance(atom, CFBlock):
            message += f"\nAtom have an CFBlock type with hash {atom.hash}"

        elif isinstance(atom, CFEvent):
            message += f"\nAtom have an CFEvent type with hash {atom.hash}"

    return message


# Retrieve information about the datums with specified type from the chain.

def get_datums_info(chain, type) -> str:

    # chain.get_datums() also returns iterator, refer to it's elements through
    # a loop or use the module itertools. Filter datums by specified type,
    # that should be an instance of CFSubDatum. If the the type is not
    # specified, the generator returns all datums in all atoms in the chain.
    if not any(chain.get_datums(type)):
        message = f"\nThere are no datums in chain with type: {type}"
        return message

    message = f"\nThe fisrt {NUM_OF_DATUMS_ATOMS} datums "
    message += f"in chain with type: {type}"
    iterator = chain.get_datums()
    datums = list(itertools.islice(iterator, NUM_OF_DATUMS_ATOMS))

    for datum in datums:
        message += f"\n{datum.hash}"

    return message


# Get the last CFDatum object with specified type in the chain.

def get_last_datum(chain, type) -> CFDatum:

    if not any(chain.get_datums()):
        message = f"\nThere are no datums in chain with type: {type}"
        log.message(message)
        return

    iterator = chain.get_datums()
    datum = list(itertools.islice(iterator, 1))[0]

    return datum


# Retrieve information about the trasactions from the chain.

def get_transaction_info(chain) -> str:

    # There are no datums with transaction type in DAG chains.
    if chain.type == ChainTypes.dag_poa:
        message = f"\nThere are no transactions in {chain.net.name} {chain.name}"
        return message

    message = f"\nTransactions info in {chain.net.name} {chain.name}"

    iterator = chain.get_transactions()
    transactions = list(itertools.islice(iterator, NUM_OF_DATUMS_ATOMS))

    # The separator is used for the readability of the output.
    for transaction in transactions:
        sep = "$$$$"
        message += f"\n{sep * 13}"
        message += f"\nCreated at{transaction.created_at}"
        message += f"\nTicker: {transaction.ticker}"
        message += f"\nLedger cache response: {transaction.ledger_rc}"
        message += f"\nAccepted: {transaction.accepted}"

    return message


# Get the last CFDatumTX object in the chain.

def get_last_tx(chain) -> CFDatumTX:

    iterator = chain.get_transactions()
    transaction = list(itertools.islice(iterator, 1))[0]
    return transaction


# The CFLedger object can be obtained from net_object.get_ledger().
# Initialization CFLedger not supported via Python.

# Retrieve information about specified transaction from ledger

def tx_info_from_ledger(ledger, tx) -> str:

    tx_ticker = ledger.get_tx_ticker(tx)
    tx_ledger_rc = ledger.get_tx_ledger_rc(tx)
    message = "\nTransaction info from ledger"
    message += f"\nTicker {tx_ticker}"
    message += f"\nLedger cache response: {tx_ledger_rc}"
    return message


# Retrieve information about specified datum from ledger

def get_signs_info(ledger, datum) -> str:

    valid = ledger.token_auth_signs_valid(datum)
    total = ledger.token_auth_signs_total(datum)
    pkeys_hashes = ledger.token_auth_signs_pkey_hashes(datum)
    message = f"\nCheck for signature signs for {datum.hash}"
    message += f"\nThe number of valid signature signs: {valid}"
    message += f"\nThe number of total signature signs: {total}"
    message += "\nThe hashes of public keys for signing ticker emission:"
    for hash in pkeys_hashes:
        message += f"\n{hash}"
    return message


# Obtain balance information by specified address.

def get_balance_info(ledger, adress) -> str:
    balances = ledger.calc_address_balances(adress)
    message = f"\nBalances for address: {adress}:"

    if not balances:
        message += "\nEmpty"
    else:
        for ticker, balance in balances.items():
            message += f"\n{ticker} : {balance}"
    return message


# Check that the emission exists in the ledger
# using the method ledger.has_emission().

def chech_for_emission(ledger, emission) -> str:
    message = "\nEmission with hash:"
    message += f"\n{emission}"
    if ledger.has_emission(emission):
        message += "\nexists in the ledger"
    else:
        message += "\ndoes not exist in the ledger"
    return message


# Get the datums from the mempool and output
# the available information.

def get_datums_from_mempool(mempool) -> str:
    datums = mempool.get_datums()
    if not datums:
        message = "\nMempool is empty"
    else:
        message = "\nDatums hashes from mempool"
        for datum in datums:
            message += f"\n{datum}"
    return message


# If the datum is encoded in bytes, then it can be decoded 
# using get_datum_from_bytes().

def deserialize_datum_from_mempool(mempool, serialized_datum) -> str:
    deserialized_datum = mempool.get_datum_from_bytes(serialized_datum)
    message = "\nDeserialized datum with hash:"
    message += f"\n{deserialized_datum.hash}"
    return message


# Create notification functions and register them in the "init" function.
# For example, output the arguments obtained by this functions.

def gdbsync_notification(_, op_code, group, key, value, *args, net, **kwargs):
    mempool = CFMempool(net.main)
    value = mempool.get_datum_from_bytes(value)
    message = f"\nGDB synchronization in the {net.name} with parametrs:"
    message += f"\n{op_code=},\n{group=},\n{key=},\n{value=}"
    log.message(f"{message}")


def mempool_notification(op_code: Literal["a", "d"], datum: CFDatum | datum_hash, *args, chain: CFChain, **kwargs):
    message = f"\nChanges in the mempool {chain.net.name} {chain.name}:"
    if isinstance(datum, CFDatum):
        hash_str = datum.hash
    else:
        hash_str = datum_hash

    if op_code == "a":
        message += f"\nNew datum with hash {hash_str} added to the mempool."
        log.message(f"{message}")

    elif op_code == "d":
        message += (f"\nDatum with hash {hash_str} deleted from the mempool.")
        log.message(f"{message}")
    else:
        log.message("Unknown operation code")


def atom_notification(atom, size, *args, chain, **kwargs):

    message = f"\nNew atom in {chain.net.name} {chain.name}."
    message += f"\nSize: {size}"
    message += f"\nHash: {atom.hash}"
    log.message(f"{message}")


def ledger_notification(ledger, tx, *args, net, **kwargs):

    # legder here is not CFLedger instance, but CellFrame.Chain.Ledger object.
    message = f"\nNew transaction in {net.name} Ledger:"
    message += f"\nHash: {tx.hash}"


def init():
    separator = "-----" * 6
    # for example set the KELVPN_NET network online
    # RAIDEN_NET.change_state(CFNetState.NET_STATE_ONLINE)
    KELVPN_NET.change_state(CFNetState.NET_STATE_ONLINE)

    # ------------------------------------------------------------------------
    log.message(f"\n{separator}NETWORKS DATA{separator}")

    # Get the list of active CFNet objects, representing a network.
    active_nets = CFNet.active_nets()

    # Output available information about the network.

    for net in active_nets:
        # for each of active network output
        # all available information.
        net_info = get_net_info(net)
        log.message(net_info)

    # Register all notification functions here.
    KELVPN_NET.register_gdbsync_notification_callback(gdbsync_notification)
    for chain in KELVPN_NET.chains:
        chain.register_mempool_notification_callback(mempool_notification)
        chain.register_atom_notification_callback(atom_notification)
    # ------------------------------------------------------------------------
    log.message(f"\n{separator}CHAINS DATA{separator}")

    # Output information about the chains of the specified network.

    for chain in KELVPN_NET.chains:

        chain_info = get_chain_info(chain)
        log.message(chain_info)

        atom_info = get_atoms_info(chain)
        log.message(atom_info)

        datums_token_info = get_datums_info(chain, CFDatumToken)
        log.message(datums_token_info)

        transaction_info = get_transaction_info(chain)
        log.message(transaction_info)
    # ------------------------------------------------------------------------
    log.message(f"\n{separator}LEDGER DATA{separator}")

    # Get the CFLedger instanse as attribube of the spesific network.
    ledger = KELVPN_NET.get_ledger()
    ledger.register_ledger_tx_notification_callback(ledger_notification)

    # Choose spesific chain to check datums info in the ledger.
    chain = KELVPN_NET.main

    # Retrieve last transaction in the network in main chain
    # and output data about it.
    tx = get_last_tx(chain)

    transaction_info = tx_info_from_ledger(ledger, tx)
    log.message(transaction_info)

    # Get the last datum with type CFDatumToken.
    last_datum = get_last_datum(chain, CFDatumToken)
    datum_ledger_info = get_signs_info(ledger, last_datum)
    log.message(datum_ledger_info)

    balances_info = get_balance_info(ledger, wallet_address)
    log.message(balances_info)

    # Get the datum with CFDatumEmission type:
    datum_emission = get_last_datum(chain, CFDatumEmission)
    emission_info = chech_for_emission(ledger, datum_emission)
    log.message(emission_info)

    # ------------------------------------------------------------------------
    log.message(f"\n{separator}MEMPOOL DATA{separator}")

    # Create CFMempool instance with specified chain
    mempool = CFMempool(chain)
    datums_from_mempool = get_datums_from_mempool(mempool)
    log.message(datums_from_mempool)

    # It is possible to extract datum encoded in bytes
    # in bytes from mempool.
    serialized_datum = last_datum.serialize()
    deserialized_datum_info = deserialize_datum_from_mempool(mempool,
                                                             serialized_datum)
    log.message(deserialized_datum_info)

    return 0
