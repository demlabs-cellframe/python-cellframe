from typing import NewType
from optparse import OptionParser
from pycfhelpers.node.cli import ReplyObject, CFCliCommand
from pycfhelpers.node.logging import CFLog


ReplyId = NewType("ReplyId", int)

# The node.cli module provides a set of classes
# for registering CLI commands.


# Create a simple proccesing function, which adds
# two numbers from the command prompt

def my_callback(arg1, arg2, reply_object: ReplyObject, **kwargs):
    """Simple proccesing function.

    Args:
        arg1 (str): the first argument from the command line.
        arg2 (str): the second argument from the command line.
        reply_object (ReplyObject): ReplyObject to provide a message about the result.
    """
    option_verbose = kwargs.get("verbose")
    result = int(arg1) + int(arg2)

    # Process the logic in the case when the command
    # is called with and without option.
    # The returned response is determined by reply_object.reply("result")
    # function, where "result" - the message being returned.

    if option_verbose:
        result_message = "Verbose message: Command executed successfully."
        result_message += f"\nThe result of the execution is {result}."
        reply_object.reply(f"{result_message}")
    else:
        reply_object.reply(f"Command executed successfully.\n{result}")


# Create a more complex processing function.
# Let the function check for a hash in the dictionary and
# return its keys dependig on the options provided.

hsh_dict = {
    "hash1": {"nonce": 123, "gas": 10000},
    "hash2": {"nonce": 456, "gas": 20000},
    }


def hash_callback(hsh, reply_object: ReplyObject, nonce, gas):
    """Processing function based on a provided hash.

    Args:
        hsh (str): hash from the command line.
        reply_object (ReplyObject): ReplyObject to provide a message about the result.
        nonce : nonce option.
        gas : gas option.
    """

    # Process the logic in the case when the command
    # is called with and without option.

    if hsh in hsh_dict:
        nonce_value = hsh_dict[hsh]["nonce"]
        gas_value = hsh_dict[hsh]["gas"]

        if nonce and gas:
            reply_object.reply(f"Hash {hsh} found. Nonce: {nonce_value}, Gas: {gas_value}")
        elif nonce and not gas:
            reply_object.reply(f"Hash {hsh} found. Nonce: {nonce_value}")
        elif gas and not nonce:
            reply_object.reply(f"Hash {hsh} found. Gas: {gas_value}")
        else:
            reply_object.reply(f"Hash {hsh} found.")
    else:
        reply_object.reply(f"Hash {hsh} not found in the dictionary.")


def init():
    # Create a command object.
    # Specify an alias, a processing function
    # and help text, if necessary.

    my_command = CFCliCommand("my_command", my_callback,
                              "This is a sample command.")

    # Add options to the command.
    # my_command.parser is an OptionParser instance, therefore,
    # configure the options via the add_option method.
    # For more complex scenarios, see the official documentation:
    # https://docs.python.org/3/library/optparse.html

    my_command.parser.add_option("-v", "--verbose",
                                 action="store_true",
                                 dest="verbose",
                                 default=False,
                                 help="Print verbose output")

    # Register a command
    my_command.register()

    # Set up and register the other callback
    # functions in the same way.

    proc_command = CFCliCommand("proc", hash_callback, "proc bridge tx")

    proc_command.parser.add_option("-n", "--nonce",
                                   action="store_true",
                                   dest="nonce",
                                   default=False)
    proc_command.parser.add_option("-g", "--gas",
                                   action="store_true",
                                   dest="gas",
                                   default=False)
    proc_command.register()

    return 0

# To check the operability of the commands, activate the Cellframe-node.
# Enter commands in the command prompt with the addition of
# the prefix "cellframe-node-cli".

# To check this examples, enter:

# cellframe-node-cli my_command 1 2
# > Command executed successfully.
# > 3

# cellframe-node-cli my_command 1 2 -v
# > Verbose message: Command executed successfully.
# > The result of the execution is 3.

# cellframe-node-cli proc a1b2c3
# > Hash a1b2c3 not found in the dictionary.

# cellframe-node-cli proc hash1
# > Hash hash1 found.

# cellframe-node-cli proc hash1 -g -n
# > Hash hash1 found. Nonce: 123, Gas: 10000

# And so on.
