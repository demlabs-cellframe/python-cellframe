from pycfhelpers.node.net import (CFNet, CFChain, CFMempool)
from pycfhelpers.node.logging import CFLog
from pycfhelpers.node.gdb import GDBGroup
from pycfhelpers.node.types import CFNetState, datum_hash
from pycfhelpers.node.datums import (CFDatumToken, CFDatum)
from typing import Literal
import itertools

log = CFLog()

# Configure example according to your needs.
KELVPN_NET = CFNet("KelVPN")


# Get the datums from the mempool and output
# the available information:

def get_datums_from_mempool(mempool) -> str:

    # CFMempool.get_datums() returns a list of all datums
    # (CFDatum objects)located in the mempool.
    datums = mempool.get_datums()
    # Consider the situation when the mempool is empty:
    if not datums:
        log.notice("Mempool is empty")
    else:
        # For instance, output the hashes of the datums:
        log.notice("Datums hashes from mempool")
        for datum in datums:
            log.notice(f"{datum.hash}")


# If the datum is encoded in bytes, then it can be decoded
# using get_datum_from_bytes().

def deserialize_datum_from_mempool(mempool, serialized_datum) -> str:
    deserialized_datum = mempool.get_datum_from_bytes(serialized_datum)
    log.notice("Deserialized datum with hash:")
    log.notice(f"{deserialized_datum.hash}")


# CFChain.get_datums() is an
# inerator over all datums tha are there.
# For convenient handling, use the module itertools.

# If the network is offline, then there is no access to it's
# elements and all generators do not return anything.

def get_last_datum(chain, type) -> CFDatum:

    if not any(chain.get_datums()):
        log.notice(f"There are no datums in chain with type: {type}")
        return

    iterator = chain.get_datums()
    datum = list(itertools.islice(iterator, 1))[0]

    # For more information, refer to the official documentation:
    # https://docs.python.org/3/library/itertools.html#itertools.islice

    return datum


# It is also possible to set notifications when new entries
# appear in the mempool. In this case, the handler function
# is described. Register it in the "init" function.

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


def init():
    separator = "-----" * 6
    # for example set the specified network online
    KELVPN_NET.change_state(CFNetState.NET_STATE_ONLINE)
    # Select a chain:
    chain = KELVPN_NET.main
    # Register the notification handler function via
    # CFChain.register_mempool_notification_callback(handler_func) method.
    chain.register_mempool_notification_callback(mempool_notification)
    # Create CFMempool instance with specified chain:
    mempool = CFMempool(chain)

    log.notice(f"{separator}MEMPOOL DATA{separator}")
    # Output the datums data from mempool:
    get_datums_from_mempool(mempool)

    # It is possible to extract datum encoded in bytes from mempool:
    last_datum = get_last_datum(chain, CFDatumToken)

    if last_datum is not None:
        serialized_datum = last_datum.serialize()
        deserialize_datum_from_mempool(mempool, serialized_datum)

    # There is a type of groups related to the mempool.
    # For example:

    kel_main_mem_group = GDBGroup("kelvpn.chain-main.mempool")

    # Get a list of keys and values for
    # kelvpn.chain-main.mempool group.
    kel_group_list = kel_main_mem_group.group_list()

    # Since each value in mempool group is a datum, they can be
    # deserialized using the CFMempool().get_datum_from_bytes() method:

    mempool = CFMempool(KELVPN_NET.main)
    log.notice(f"Group {kel_main_mem_group.group} list:")

    for item in kel_group_list:
        for key, value in item.items():
            value = mempool.get_datum_from_bytes(value)
            log.notice(f"Key: {key}, Value: {value}")

    return 0
