import logging
import traceback
import sys
from enum import IntEnum
from dataclasses import dataclass

try:
    from DAP.Core import logIt
except ImportError:
    pass


class LogLevels(IntEnum):
    """Enumeration of log levels."""
    NOTSET = 0
    DEBUG = 10
    INFO = 20
    NOTICE = 30
    MESSAGE = 40
    DAP = 50
    WARNING = 60
    ATT = 70
    ERROR = 80
    CRITICAL = 90


@dataclass
class Emission:
    hash: str = "0x15DE774E75828781FD7F9B3930417E5CDC7225513E29018BA3A6572932368A78"


class CFLog:
    """
    A logging class to handle logging messages with different levels and formats.

    Attributes:
        _logger (Any): The logging output, defaults to `logIt` if not provided.
        _level (LogLevels): The logging level, defaults to `LogLevels.NOTSET`.
        _message_format (str): The format for log messages, defaults to "{message}".
    """
    def __init__(self,
                 level: LogLevels = LogLevels.NOTSET,
                 message_format: str = "{message}", out=None):
        """
        Initialize the CFLog instance.

        Args:
            level (LogLevels): The logging level, defaults to `LogLevels.NOTSET`.
            message_format (str): The format for log messages, defaults to "{message}".
            out (Any): The logging output, defaults to `logIt` if not provided.

        Default format params:
            message
            log_level
            message_level

        Example:
            log = CFLog(out=sys.stdout, message_format="Emission hash={ems.hash} {message}")
            log.message("Not found", ems=emission)
            Output:
            Emission hash=0x15DE774E75828781FD7F9B3930417E5CDC7225513E29018BA3A6572932368A78 Not found
        """
        self._logger = out or logIt
        self._level = level
        self._message_format = message_format
        # self._format_params = format_params

    @property
    def log_level(self) -> str:
        return self._level.name

    def __log(self, message, level, name, format_params: dict) -> None:
        origin_format = format_params.copy()
        if self._level <= level:
            # self._format_params = format_params or {"message": message}
            format_params["log_level"] = self.log_level
            format_params["message_level"] = level.name
            if not format_params.get("message"):
                format_params["message"] = message
            try:
                total_message = self._message_format.format(**format_params)
            except KeyError:
                total_message = message
            # except Exception:
            #     log.error(traceback.format_exc())
            #     return
            if isinstance(self._logger, type(sys.stdout)):
                print(total_message)
                self._format_params = origin_format
                return
            getattr(self._logger, name)(total_message)
            self._format_params = origin_format

    def debug(self, message: str, **kwargs):
        """Log a debug message.

        Args:
            message (str): The message to log.
            **kwargs: Additional format parameters.
        """
        self.__log(message, LogLevels.DEBUG, "debug", format_params=kwargs)

    def info(self, message: str, **kwargs):
        """Log an info message.

        Args:
            message (str): The message to log.
            **kwargs: Additional format parameters.
        """
        self.__log(message, LogLevels.INFO, "info", format_params=kwargs)

    def notice(self, message: str, **kwargs):
        """Log a notice message.

        Args:
            message (str): The message to log.
            **kwargs: Additional format parameters.
        """
        self.__log(message, LogLevels.NOTICE, "notice", format_params=kwargs)

    def message(self, message: str, **kwargs):
        """Log a general message.

        Args:
            message (str): The message to log.
            **kwargs: Additional format parameters.
        """
        self.__log(message, LogLevels.MESSAGE, "message", format_params=kwargs)

    def dap(self, message: str, **kwargs):
        """Log a DAP-specific message.

        Args:
            message (str): The message to log.
            **kwargs: Additional format parameters.
        """
        self.__log(message, LogLevels.DAP, "dap", format_params=kwargs)

    def warning(self, message: str, **kwargs):
        """Log a warning message.

        Args:
            message (str): The message to log.
            **kwargs: Additional format parameters.
        """
        self.__log(message, LogLevels.WARNING, "warning", format_params=kwargs)

    def att(self, message: str, **kwargs):
        """Log an attestation message.

        Args:
            message (str): The message to log.
            **kwargs: Additional format parameters.
        """
        self.__log(message, LogLevels.ATT, "att", format_params=kwargs)

    def error(self, message: str, **kwargs):
        """Log an error message.

        Args:
            message (str): The message to log.
            **kwargs: Additional format parameters.
        """
        self.__log(message, LogLevels.ERROR, "error", format_params=kwargs)

    def critical(self, message: str, **kwargs):
        """Log a critical message.

        Args:
            message (str): The message to log.
            **kwargs: Additional format parameters.
        """
        self.__log(message, LogLevels.CRITICAL, "critical", format_params=kwargs)

class CFLogRecordable(CFLog):
    
    def __init__(self):
        self.message_buffer = []
        self.recording = False

        selfsup = super()
        selfsup.__init__()
        
        #set pre-processors for earch of messag-methods
        slots = ["debug", "info", "notice", "message", "dap", "warning", "att", "error", "critical"]
        
        def make_caller(fname):
            return lambda x: self.__proc_msg(x) or getattr(selfsup, fname)(x)
        
        lambdas = [make_caller(fname) for fname in slots]
        
        for i,l in enumerate(lambdas):
            setattr(self, slots[i],  l)
            
    
        
    def start_record(self):
        self.cleanup_record()
        self.recording = True
    
    def stop_record(self):
        self.recording = False

    def cleanup_record(self):
        self.message_buffer = []

    def __proc_msg(self, msg):
        if self.recording:
            self.message_buffer.append( msg )

    @property
    def log_recorded(self):
        return self.message_buffer



log = CFLogRecordable()

if __name__ == "__main__":
    # log = CFLog(out=sys.stdout)
    # log.message(f"Test message")
    #
    #
    # log = CFLog(out=sys.stdout, message_format="[{level}] {message}")
    # log.debug("Debug message")
    # log.message("Simple message")
    # log.error("AAAAA message")
    emission = Emission()

    log = CFLog(out=sys.stdout, message_format="Emission hash={ems.hash} {message}")
    log.message("Not found", ems=emission)
    log.message("Not found")

    # log = CFLog(out=sys.stdout, message_format="[{level}] {message}", level=LogLevels.MESSAGE)
    # log.debug("Debug message")
    # log.message("Simple message")
    # log.error("AAAAA message")
