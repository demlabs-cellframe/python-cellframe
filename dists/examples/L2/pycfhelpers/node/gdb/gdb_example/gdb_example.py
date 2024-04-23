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

class GdbGroupJson(GDBGroup):
    """Represent a group to use json files.

    Attributes:
        group (str): The name of the group.
    """

    def set_json(self, data: str) -> bool:
        """Deserialize json string to a python dictionary and
        set the value associated with the key.

        Args:
            data (str): json string.

        Returns:
            bool: True if successful, False otherwise.
        """
        # Deserialize json data to python dictionary:
        # It also can be loaded fron the json file by
        # using the json.load(data, file_path) function.
        try:
            data_json = json.loads(data)
            # Go through all the key-value pairs and call the
            # "set()" method of the parent class for each one.
            for key, value in data_json.items():
                # Encode values to bytes.
                super().set(key, value.encode('utf-8'))
        # Configure the exception output:
        except json.JSONDecodeError:
            log.error(f"Error: Invalid JSON format in data {data}.")

    def get_json(self, key):
        """Retrieve the value associated with the given key from the group
        and serialize it to a json string.

        Args:
            key (str): The key.

        Returns:
            json_string (str): Serialized data from the group.
        """
        # This function serialize only one key-value pair
        # from GdbGroupJson to json object by given key.
        try:
            data = {}
            value = super().get(key)
            # Create a dictionary from the received key and value.
            # Decode values from bytes.
            data[key] = value.decode('utf-8')
            # Serialize dictionary to a json object.
            # It also can be dumped to a json file by
            # using the json.dump(data, file_path) method
            json_string = json.dumps(data)
            return json_string
        # Configure the exception output:
        except Exception as e:
            log.error(f"Error: {e}")

    def group_json(self):
        """Retrieve the list of key-value pairs in the group
        and serialize it to a json string.

        Returns:
            json_string (str): Serialized data from the group.
        """
        # This function serialize every key-value pair
        # that contains in GdbGroupJson to one json object.
        try:
            data = {}
            # Use the parent group_list() method to get a list
            # of dictionaries of all key-value pair.
            for item in super().group_list():
                for key, value in item.items():
                    # Decode values from bytes.
                    data[key] = value.decode('utf-8')
                # Serialize dictionary to a json object.
                json_string = json.dumps(data)
            return json_string
        # Configure the exception output:
        except Exception as e:
            log.error(f"Error: {e}")


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
    log.message(value_from_group)

    # It is possible to view all key - value pairs using
    # the method group_list() for the designated group:
    group_list = my_group.group_list()
    log.message(f"\nGroup {my_group.group} list: {group_list}")

    # Get a list of keys and values for
    # kelvpn.chain-main.mempool group.
    kel_group_list = kel_main_mem_group.group_list()

    # Since each value in mempool group is a datum, they can be
    # deserialized using the CFMempool().get_datum_from_bytes() method:

    mempool = CFMempool(KELVPN_NET.main)
    message = f"\nGroup {kel_main_mem_group.group} list:"

    for item in kel_group_list:
        for key, value in item.items():
            value = mempool.get_datum_from_bytes(value)
            message += f"\nKey: {key}, Value: {value}"
    log.message(message)

    # Create GdbGroupJson object:
    json_group = GdbGroupJson("json")

    # Create a test json to string:
    json_data = """{
    "key_1": "value_1",
    "key_2": "value_2"
    }"""

    # Process the methods of the created class:
    json_group.set_json(json_data)

    get_json = json_group.get_json("key_1")
    get_message = f"\nType of get_json() output: {type(get_json)}"
    get_message += f"\nContent: {get_json}"

    log.message(get_message)

    get_group = json_group.group_json()
    group_message = f"\nType of group_json() output: {type(get_group)}"
    group_message += f"\nGroup {json_group.group} content: {get_group}"
    log.message(group_message)

    # Since GdbGroupJson inherited from GDBGroup
    # where is the delete() method defined, it also available
    # as a method of GdbGroupJson instance.
    return 0
