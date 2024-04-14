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
    def __init__(self) -> None: ...
    def __init__(self, string_GUUID : str) -> None: ...
    def __init__(self, net_id : int, service_id : int) -> None: ...
    def __str__(self) -> str: ...
    pass
