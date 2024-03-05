from typing import Protocol


# DapLogitObjectType
class logIt(Protocol):
    @staticmethod
    def debug(message: str, name_plugin: str = "", /):
        """
        Log a message with the DEBUG level.
        """
        pass

    @staticmethod
    def info(message: str, name_plugin: str = "", /):
        """
        Log a message with the INFO level.
        """
        pass

    @staticmethod
    def notice(message: str, name_plugin: str = "", /):
        """
        Log a message with the NOTICE level.
        """
        pass

    @staticmethod
    def message(message: str, name_plugin: str = "", /):
        """
        Log a message with the MESSAGE level.
        """
        pass

    @staticmethod
    def dap(message: str, name_plugin: str = "", /):
        """
        Log a message with the DAP level.
        """
        pass

    @staticmethod
    def warning(message: str, name_plugin: str = "", /):
        """
        Log a message with the WARNING level.
        """
        pass

    @staticmethod
    def att(message: str, name_plugin: str = "", /):
        """
        Log a message with the ATT level.
        """
        pass

    @staticmethod
    def error(message: str, name_plugin: str = "", /):
        """
        Log a message with the ERROR level.
        """
        pass

    @staticmethod
    def critical(message: str, name_plugin: str = "", /):
        """
        Log a message with the CRITICAL level.
        """
        pass


# DapCommonObjectType
class Common(Protocol):
    pass


# DapMathObjectType
class Math(Protocol):
    @property
    def coins(self) -> str: # balance/18
        pass

    @property
    def balance(self) -> str: # datoshi тоже, что и __str__
        pass

    @staticmethod
    def balanceToCoins(balance: int) -> str:
        """The function calculates the number of coins from the number of datoshi."""
        pass

    def __str__(self) -> str: # тоже, что и balance
        pass


#DapAppContextObjectType
class AppContext(Protocol):
    @staticmethod
    def getServer(server, /):
        """Get main server from node"""
        pass

    @staticmethod
    def getHttp(server, /):
        pass
