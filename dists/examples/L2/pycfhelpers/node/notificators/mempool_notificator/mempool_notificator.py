from typing import Literal
from pycfhelpers.node.notificators import CFNets
from pycfhelpers.node.net import CFNet, CFChain, CFBlock, CFEvent
from pycfhelpers.node.logging import CFLog
from pycfhelpers.node.datums import CFDatum
from pycfhelpers.common.types import datum_hash

#datum_hash = NewType("datum_hash", str)

log = CFLog()

# The node.notificators module provides a CTNets class, with methods,
# designed for registering notificators, when changing in mempool, atom, 
# global data base synchronization and transaction in the ledger are detected.

# To register the handler function, use the appropriate decorator,
# depending on what changes need to be track.

# Register the handler function outside the "init" function.

# Create a list of CFNet objects, in order to indicate
# which networks will be tracked for changes.

NETS = [CFNet("raiden"), CFNet("Backbone"), CFNet("KelVPN")]

# Mempool changes.
# Create a CFNets instanse through the declared list of neworks
# and use mempool_notificator method, to wrap the handler fuction.
# Specify the desired chain in the argument.


@CFNets(NETS).mempool_notificator(chain_name="all")
def on_mempool_change(op_code: Literal["a", "d"], datum: CFDatum | datum_hash, *args, chain: CFChain, **kwargs):
    """
    Callback function. Called when changes are commited to the mempool.

    Args:
        op_code (Literal["a", "d"]): Operation code. "a" when adding, "d" when deleting.
        datum (CFDatum | datum_hash): The data that was added or removed from the mempool.
        *args: Positional arguments.
        chain (CFChain): The chain object for which the callback is invoked.
        **kwargs: Keyword arguments.
    """
    # Process the received data.

    message = "Mempool change detected! Operation:"

    chain_name = chain.name

    if isinstance(datum, CFDatum):
        datum_hash_str = datum.hash
    else:
        datum_hash_str = datum_hash

    if op_code == "a":
        message += f" {op_code}"
        message += f"\nNew datum with hash {datum_hash_str} added to mempool in chain: {chain_name}"
        log.message(f"{message}")

    elif op_code == "d":
        message += f" {op_code}"
        message += (f"\nDatum with hash {datum_hash_str} deleted from mempool in chain: {chain_name}")
        log.message(f"{message}")
    else:
        log.message("Unknown operation code")

    # This processing order is for demonstration purposes.
    # Before each call to the handler function? a message,
    # with the arguments passed to the Mempool callback
    # will be called in the logs.


# The over handler functions are registered in a similar way,
# but each through it's own function.

# The new 
@CFNets(NETS).atom_notificator(chain_name="main")
def on_new_atom(atom: CFBlock | CFEvent, size: int, *args, chain: CFChain, **kwargs):
    """Callback function. Called when changes are commited to the atom.

    Args:
        atom (CFBlock | CFEvent): a Block at the blockchain level, or Event if used in a DAG
        size (int): size of atom
        chain (CFChain): The chain object for which the callback is invoked.
    """

    message = f"Atom with hash: {atom.hash} in {chain.name}."
    message += f"\nsize of atom is {size}"
    log.message(f"{message}")

    # Unlike the previous example, the logs do not automatically
    # show a message about the passed function parametrs.


@CFNets(NETS).gdbsync_notificator("to_args", key="to_kwargs")
def on_new_table_record(op_code, group, key, value, *args, net: CFNet, **kwargs):
    """Callback function. Called when changes are commited to the global database.

    Args:
        op_code (): Operation code
        group (): ...
        key (): ...
        value (): ...
        net (CFNet): ...
    """

    message = "GDB callback with parametrs:"
    message += f" {op_code=}, {group=}, {key=}, {value=}, {net=}"
    log.message(f"{message}")


@CFNets(NETS).ledger_tx_notificator("to_args", key="to_kwargs")
def on_new_ledger_transaction(ledger, tx, *args, net: CFNet, **kwargs):
    """Callback function. Called when changes are commited to the ledger.

    Args:
        ledger (): ...
        tx (): ...
        net (CFNet): ...
    """

    message = "Ledger callback with parametrs:"
    message += f" {ledger=}, {tx=}, {net=}"
    log.message(f"{message}")


def init():

    return 0

# Start the CellFrame node and check  the logs.
# For notifications to work, check that networks are online
