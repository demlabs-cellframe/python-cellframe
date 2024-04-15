from typing import Protocol


# DapCryptoAlgoObjectType
class Algo(Protocol):
    pass


# DapCryptoCertObjectType
class Cert(Protocol):
    pass


# DapCryproSignTypeObjectType
class SignType(Protocol):
    pass


# DapCryptoSignObjectType
class Sign(Protocol):
    pass


# DapCryptoKeyTypeObjectType
class CryptoKeyType(Protocol):
    pass


# DapCryptoDataTypeObjectType
class CryptoDataType(Protocol):
    pass


# DapChainHashFastObjectType
class HashFast(Protocol):
    pass

class GUUID(Protocol):
    def __init__(self, hex_GUUID : str) -> None: ...
    @staticmethod
    def generate() -> GUUID: ...
    @staticmethod
    def compose(net_id : int, service_td : int) ->GUUID: ...
    def __str__(self) -> str: ...
