from typing import Protocol, NewType

import traceback
from CellFrame import AppCliServer
from pycfhelpers.logger import log
from optparse import OptionParser

ReplyId = NewType("ReplyId", int)


class ReplyObject:
    """An object representing a reply for CLI commands.

    Provides a mechanism to manage replies for CLI commands.

    Attributes:
        reply_id (ReplyId): The ID of the reply.
    """

    def __init__(self, reply_id: ReplyId):
        """Initialize a ReplyObject instance with the specified reply ID.

        Args:
            reply_id (ReplyId): The unique identifier for the reply.
        """
        self.reply_id = reply_id

    def reply(self, message: str):
        """Send a reply message.

        Args:
            message (str): The message to be sent as a reply.
        """
        AppCliServer.setReplyText(message, self.reply_id)


class CliCallback(Protocol):
    """A protocol for CLI command callbacks."""

    def __call__(self, *args, reply_object: ReplyObject, **kwargs) -> None:
        """Call method signature for CLI command callbacks."""
        pass


class CFCliCommand:
    """The CellFrame CLI command.

    Provides methods to register and handle CLI commands.

    Attributes:
        command (str): The CLI command.
        callback (CliCallback): The callback function to be executed whe the command is invoked.
        help_text (str): The help text for the command.
        parser (OptionParser): The option parser for the command.
    """

    def __init__(self, command: str, callback: CliCallback, help_text: str = ""):
        """Initialize a CFCliCommand object with the given parametrs.

        Args:
            command (str): The alias for CLI command.
            callback (CliCallback): The callback function to be executed when the command is invoked.
            help_text (str, optional): The help text for the command. Defaults to "".
        """
        self.command = command
        self.callback = callback
        self.help_text = help_text
        self.parser = OptionParser()

    def register(self):
        """Register the CLI command."""
        def callback_wrapper(argv: list[str], reply_id: ReplyId):
            """Wrapp function for the callback.

            This function serves as a wrapper for the actual callback function
            of a CLI command. It handles parsing command-line arguments using
            an OptionParser, executes the callback function,
            and provides error handling.

            Args:
                argv (List[str]): The list of command-line arguments.
                reply_id (ReplyId): The ID of the reply object.

            Raises:
                SystemExit: If an error occurs during parsing of command-line
                            arguments, the function exits with SystemExit.
            """
            try:
                print(argv)
                (options, args) = self.parser.parse_args(argv[1:])
                print(options,args)
            except SystemExit:
                help_text = self.help_text or self.parser.format_help()
                AppCliServer.setReplyText(help_text, reply_id)
                return
            self.callback(*args, reply_object=ReplyObject(reply_id), **options.__dict__)

        help_text = self.help_text or self.parser.format_help()
        try:
            AppCliServer.cmdItemCreate(self.command, callback_wrapper, help_text, "")
        except Exception:
            log.error(f"Error = {traceback.format_exc()}")
