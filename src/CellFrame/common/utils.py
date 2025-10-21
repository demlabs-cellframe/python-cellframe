"""
Core utilities for CellFrame SDK
"""

import json
from typing import Any, Optional, List


def json_dump(data: Any) -> str:
    """Serialize Python object to a JSON formatted string.

    Args:
        data: The Python object to be serialized.

    Returns:
        str: The JSON formatted string representing the serialized object.
    """
    return json.dumps(data)


def json_load(data: str) -> Optional[Any]:
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


class ListCache:
    """A cache implementation using a list.

    This class provides a caching mechanism using a list with a specified capacity.

    Attributes:
        capacity (int): The maximum capacity of the cache.
        data (list): The list to store cached items.
    """

    def __init__(self, capacity: int):
        """Initialize a ListCache object with the given capacity.

        Args:
            capacity (int): The maximum capacity of the cache.
        """
        self.capacity = capacity
        self.data = []

    def add(self, data: Any):
        """Add data to the cache.

        Args:
            data: The data to add to the cache.
        """
        self.data.append(data)
        if len(self.data) > self.capacity:
            self.data.pop(0)

    def get_data(self) -> List[Any]:
        """Get the data stored in the cache.

        Returns:
            list: The data stored in the cache.
        """
        return self.data


def get_tsd_data(tsd_list: List[Any], type_tsd: Any) -> Optional[Any]:
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


def net_by_name(nets: List[Any], netname: str) -> Optional[Any]:
    """Get network by name from a list of networks.

    Args:
        nets (list): A list of networks.
        netname (str): The name of the network to find.

    Returns:
        Any: The network object with the specified name, or None if not found.
    """
    for net in nets:
        if hasattr(net, 'name') and net.name == netname:
            return net
    return None





