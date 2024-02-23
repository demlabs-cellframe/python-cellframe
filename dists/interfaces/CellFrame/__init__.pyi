from typing import Protocol, Callable, Iterator
from DAP.Crypto import HashFast


# PyModule_AddObject(cellframeModule, "error", CellFrame_error);

# class ChainAtomPtr(Protocol):
#     # no methods
#     # no attrs
#     pass

class ReplyObject(Protocol):
    pass


# DapAppCliObjectType
# TODO: в список вопросов
class AppCli(Protocol):
    @staticmethod
    def main():
        pass


# DapChainNodeCliObjectType
class AppCliServer(Protocol):
    @staticmethod
    def cmdItemCreate(command: str,
                      callback: Callable[[tuple, ReplyObject], None],
                      help_text: str, doc_ex: str, /) -> None:
        """
        Add cli command
        :param callback: Iterator - command arguments
        :param doc_ex: неведомая хрень
        """
        pass

    @staticmethod
    def setReplyText(reply_text: str, reply_obj: ReplyObject) -> None:
        pass

    @staticmethod
    def getByAlias(chain_net, alias: str):
        pass
