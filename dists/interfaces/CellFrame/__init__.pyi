from typing import Protocol, Callable, Iterator, NewType
from DAP.Crypto import HashFast

# PyModule_AddObject(cellframeModule, "error", CellFrame_error);

# class ChainAtomPtr(Protocol):
#     # no methods
#     # no attrs
#     pass

ReplyId = NewType("ReplyId", int)


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
                      callback: Callable[[list, ReplyId], None],
                      help_text: str, doc_ex: str, /) -> None:
        """
        Add cli command
        :param callback: Iterator - command arguments
        :param doc_ex: неведомая хрень
        """
        pass

    @staticmethod
    def setReplyText(reply_text: str, reply_obj: ReplyId) -> None:
        pass

    @staticmethod
    def getByAlias(chain_net, alias: str):
        pass

# /python/interfaces -> feature-... merge -> master