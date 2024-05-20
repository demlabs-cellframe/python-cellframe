from pycfhelpers.node.logging import CFLog
from pycfhelpers.node.gdb import GDBGroup

import json  # To work with GDBGroupJson - custom class example.

# Output the basic information in the logs.
log = CFLog()

# It is possible to create a custom class that inherits
# from the GDBGroup class to change the logic of its methods.
# This class provides serialization and deserialization from JSON
# objects to Python objects.


class GDBGroupJson(GDBGroup):
    """Represent a group that stores JSON-serialized data as values.

    Attributes:
        group (str): The name of the group.
    """
    def set(self, key, value: dict | list):
        """Serialize a value to a JSON format and set it to the group
        associated with the key.

        Args:
            key (str): Table key.
            value (dict | list): Value to set.
        """
        # Dump dict/list as a byte-encoded string and write it to a GDB group
        # using the parent set(key, value) method.
        super().set(key, json.dumps(value).encode("utf-8"))

    def get(self, key, default=None):
        """Retrieve a value associated with the given key from the group
        and deserialize it from JSON format.

        Args:
            key (str): The key.
            default (str, optional): The default value to return if the key is not found. Defaults to None.

        Returns:
            value: The value associated with the key.
        """
        # Get a value by specified key using
        # parent method get(key, default):
        value = super().get(key, default=default)
        # Deserialize and return the value:
        return json.loads(value) if value else default

    def group_dict(self):
        """Retrieve the list of key-value pairs in the group,
        convert them to a dictionary, and deserialize from a JSON format.

        Returns:
            dict: The dictionary containing all key-value pairs.
        """
        # Get a list of all key-value pairs using
        # the parent group_list() method:
        group_data = super().group_list()
        result_dict = {}
        # Convert data to a dictionary:
        for item in group_data:
            for key, value in item.items():
                try:
                    # Deserialize values if necessary:
                    decoded_value = json.loads(value)
                    result_dict[key] = decoded_value
                except json.JSONDecodeError:
                    # Not JSON, error
                    pass
        return result_dict


def init():
    # Create an instance of the class GDBGroup to
    # connect to the global database.
    my_group = GDBGroup("local.mygroup")

    # Set or receive the key and its corresponding value.
    my_key = "my_key"
    my_value = "my_value"

    # Set the preset value encoded in bytes.
    my_group.set(my_key, my_value.encode("utf-8"))

    # Get the value for the specified key and decode it back:
    value_from_group = my_group.get(my_key).decode("utf-8")
    log.notice(value_from_group)

    # It is possible to view all key-value pairs using
    # the method group_list() for the designated group:
    group_list = my_group.group_list()
    log.notice(f"Group {my_group.group} list: {group_list}")

    # Create GDBGroupJson object:
    json_group = GDBGroupJson("json")

    # Process the methods of the created class.
    # Set in the group different data:
    json_group.set("key_1", {"data_int": 1, "data_bool": True})
    json_group.set("key_2", {"data_str": "Hello, world!", "data_list": [1, 2]})

    # Output result of get() and group_dict() methods:
    log.notice(f"get() result: {json_group.get('key_1')}")
    log.notice(f"group_dict() result: {json_group.group_dict()}")

    # Since GDBGroupJson inherited from GDBGroup,
    # the delete() method defined in GDBGroup is also available
    # as a method of GDBGroupJson instance.
    return 0
