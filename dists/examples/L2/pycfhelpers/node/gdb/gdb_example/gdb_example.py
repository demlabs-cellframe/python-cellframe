from pycfhelpers.node.logging import CFLog
from pycfhelpers.node.gdb import GDBGroup
from pycfhelpers.node.net import CFMempool, CFNet
from pycfhelpers.node.types import CFNetState

import json  # To work with GdbGroupJson - custom class example.

# The module GDB provides basic classes for working with groups in a
# global database (Global DB). GDBGroupBase is an abstract base
# class representing a group in GlobalDB, with methods for setting,
# retrieving, deleting values by key and retrieving a key-value list
# in a group. GDbgroup is a specific class that implements the GDBGroupBase
# functionality for working with bytes type data. It provides methods for
# working with groups, such as setting, getting, deleting values by key, as
# well as getting a list of all key-values in a group.

# For example, there will be a KelVPN mempool group.
KELVPN_NET = CFNet("KelVPN")

# Output the basic information in the logs.
log = CFLog()


# It is possible to create a custom class, that inherits
# from the GDBGroup class to change the logic of it's methods.
# This class provides serialization and deserialization from json
# objects to python objects.

class GDBGroupJson(GDBGroup):
    """Represent a group to use json files.

    Attributes:
        group (str): The name of the group.
    """
    def set(self, key, value: dict | list):
        """Serialize a value to a json format and set it to the group
        associated with the key.

        Args:
            data (str): json string.

        Returns:
            bool: True if successful, False otherwise.
        """
        # Dump a byte-encoded json file to the database
        # using the parent set(key, value) method.
        super().set(key, json.dumps(value).encode("utf-8"))

    def get(self, key, default=None):
        """Retrieve a value associated with the given key from the group
        and deserialize it from json format.

        Args:
            key (_type_): The key.
            default (_type_, optional): The default value to return if the key is not found. Defaults to None.

        Returns:
            value:The value associated with the key.
        """
        # get a value by specified key using
        # parent method get(key, default):
        value = super().get(key, default=default)
        # Deserialze and return a value:
        return json.loads(value) if value else default

    def group_dict(self):
        """Retrieve the list of key-value pairs in the group,
        convert them to a dictioanary and deserialize from a json format.

        Returns:
            dict: The dictionary containing all key-valeu pairs.
        """
        # get a list off all key-value pairs using
        # the parent group_list() method:
        group_data = super().group_list()
        dict = {}
        # Convert data to a dictionary:
        for item in group_data:
            for key, value in item.items():
                try:
                    # deserialize values if necessary:
                    decoded_value = json.loads(value)
                    dict[key] = decoded_value
                except json.JSONDecodeError:
                    # Otherwise, decode it:
                    dict[key] = value.decode("utf-8")
        return dict


# The data from the mempool can be deserialized
# using the get_datum_from_bytes() method.

def deserialize_datum_from_mempool(mempool, serialized_datum):
    deserialized_datum = mempool.get_datum_from_bytes(serialized_datum)
    return deserialized_datum


def init():
    KELVPN_NET.change_state(CFNetState.NET_STATE_ONLINE)

    # Create an instance of the class GDBGroup to
    # connect to the global data base.
    my_group = GDBGroup("local.mygroup")

    # Consider working with non-local groups:
    kel_main_mem_group = GDBGroup("kelvpn.chain-main.mempool")

    # Set or recieve the key and it's corresponding value.
    my_key = "my_key"
    my_value = "my_value"

    # Set the preset value encoded in bytes.
    my_group.set(my_key, my_value.encode("utf-8"))

    # Get the value for the specified key, and decode it back:
    value_from_group = my_group.get(my_key).decode("utf-8")
    log.notice(value_from_group)

    # It is possible to view all key - value pairs using
    # the method group_list() for the designated group:
    group_list = my_group.group_list()
    log.notice(f"Group {my_group.group} list: {group_list}")

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

    # Create GdbGroupJson object:
    json_group = GDBGroupJson("json")

    # Process the methods of the created class.
    # Set in the group diffrent data:
    json_group.set("key_1", {"data_int": 1, "data_bool": True})

    json_group.set("key_2", {"data_str": "Hello, world!", "data_list": [1, 2]})
    # Output result of get() and group_dict() methods:
    log.notice(f"get() result:{json_group.get('key_1')}")
    log.notice(f"group_dict() result:{json_group.group_dict()}")

    # Since GdbGroupJson inherited from GDBGroup
    # where is the delete() method defined, it also available
    # as a method of GdbGroupJson instance.
    return 0
