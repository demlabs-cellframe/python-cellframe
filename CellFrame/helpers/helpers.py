import json

# Graceful import of CellFrame components
try:
    from CellFrame.Common import Datum, DatumTx, TxOut, TxIn, TxToken, TxSig, TxOutCondSubtypeSrvStakeLock, TxInCond, TxOutExt
    CELLFRAME_AVAILABLE = True
except ImportError:
    CELLFRAME_AVAILABLE = False
    # Create dummy classes to maintain API compatibility
    class _DummyType:
        pass
    Datum = DatumTx = TxOut = TxIn = TxToken = TxSig = TxOutCondSubtypeSrvStakeLock = TxInCond = TxOutExt = _DummyType


def json_dump(data):
    """Serialize Python object to a JSON formatted string.

    Args:
        data: The Python object to be serialized.

    Returns:
        str: The JSON formatted string representing the serialized object.
    """
    return json.dumps(data)


def json_load(data):
    """Deserialize JSON formatted string to a Python object.

    Args:
        data (str): The JSON formatted string to be deserialized.

    Returns:
        Any: The Python object deserialized from the JSON string, or None if deserialization fails.
    """
    try:
        return json.loads(data)
    except:
        return None


def find_tx_out(tx, out_type):
    """Find a transaction output of the specified type in a transaction.

    Args:
        tx: The transaction to search.
        out_type: The type of transaction output to find.

    Returns:
        TxOut or None: The transaction output of the specified type, or None if not found.
    """
    if not CELLFRAME_AVAILABLE:
        return None
    try:
        return next(filter(lambda x: isinstance(x, out_type), tx.getItems()))
    except:
        return None


def get_tx_items(tx, out_type):
    """Get a list of transaction items of the specified type from a transaction.

    Args:
        tx: The transaction to retrieve items from.
        out_type: The type of transaction items to retrieve.

    Returns:
        list or None: A list of transaction items of the specified type,
            or None if not found.
    """
    if not CELLFRAME_AVAILABLE:
        return None
    try:
        return list(filter(lambda x: isinstance(x, out_type), tx.getItems()))
    except:
        return None    


def get_tsd_data(tsd_list, type_tsd):
    """Get data of a specific type from a list of TSDs.

    Args:
        tsd_list (list): The list of TSDs.
        type_tsd: The type of TSD data to retrieve.

    Returns:
        Any: The data of the specified type from the list of TSDs,
            or None if not found.
    """
    try:
        return next(filter(lambda x: x.type == type_tsd, tsd_list)).data
    except:
        return None


def get_tx_outs(tx, out_type):
    """Get a list of transaction outputs of the specified type from a transaction.

    Args:
        tx: The transaction to retrieve outputs.
        out_type: The type of transaction outputs to retrieve.

    Returns:
        list or None: A list of transaction outputs of the specified type, 
            or None if not found.
    """
    if not CELLFRAME_AVAILABLE:
        return None
    try:
        return list(filter(lambda x: isinstance(x, out_type), tx.getItems()))
    except:
        return None

class ListCache:
    """A cache implementation using a list.

    This class provides a caching mechanism using a list with a specified capacity.

    Attributes:
        capacity (int): The maximum capacity of the cache.
        data (list): The list to store cached items.
    """

    def __init__(self, capacity):
        """Initialize a ListCache object with the given capacity.

        Args:
            capacity (int): The maximum capacity of the cache.
        """
        self.capacity = capacity
        self.data = []

    def add(self, data):
        """Add data to the cache.

        Args:
            data: The data to add to the cache.
        """
        self.data.append(data)
        if len(self.data) > self.capacity:
            self.data.pop(0)

    def data(self):
        """Get the data stored in the cache.

        Returns:
            list: The data stored in the cache.
        """
        return self.data


def net_by_name(NETS, netname):
    """Get network by name from a list of networks.

    Args:
        NETS (list): A list of networks.
        netname (str): The name of the network to find.

    Returns:
        Any: The network object with the specified name, or None if not found.
    """
    for net in NETS:
        if net.name == netname:
            return net
