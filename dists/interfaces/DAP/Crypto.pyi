from typing import Protocol
from CellFrame.Common import *


# DapCryptoAlgoObjectType
class Algo(Protocol):
    pass


# 1. Создать сертификат
# 2. Сохранить приватный сертификат(save())
# 3. Извлечь публичный сертификат(not implemetnted)
# 4. Сохранить публичный сертификат(not implemetnted)


# DapCryptoCertObjectType
class Cert(Protocol):
    @property
    def key(self) -> Key | None:  # private  Key --> EKey
        pass

    def pkey(self) -> Pkey:  # public
        pass

    # def extract_public_cert() -> Cert:

    @staticmethod
    def generate(cert_name: str, key_type: CryptoKeyType = KeyTypes.SIG_DILITHIUM, seed_string: str | None = None,
                 /) -> Cert | None:
        """Generate from seed or randomly the new certificate"""
        pass

    @staticmethod
    def find(self, *args, **kwargs):
        raise NotImplemented

    @staticmethod
    def folderAdd(self, *args, **kwargs):
        """Add folders with .dcert files in it"""
        raise NotImplemented

    @staticmethod
    def folderGet(self, folder_path: str, /) -> int:
        """Get folder by number or the default one"""
        pass

    @staticmethod
    def load(cert_name: str, /) -> Cert | None:
        pass

    def dump(self):
        raise NotImplemented

    def sign(self):
        raise NotImplemented

    def cert_sign_add(self):
        raise NotImplemented

    def cert_signs(self, *args, **kwargs):
        raise NotImplemented

    def compare(self, *args, **kwargs):
        raise NotImplemented

    def save(self) -> int:
        """Save to the first directory in cert folders list"""  # var/lib
        pass

    def delete(self) -> None:
        pass


# DapCryproSignTypeObjectType
class SignType(Protocol):
    def __str__(self):
        pass


# DapCryptoSignObjectType
class Sign(Protocol):
    @property
    def type(self) -> SignType:
        pass

    @property
    def pkey(self) -> Pkey:
        pass

    @property
    def pkeyHash(self) -> HashFast:
        pass

    @property
    def size(self) -> int:
        pass

    def verify(self, *args):
        pass

    @staticmethod
    def fromBytes(*args):
        pass

    def toBytes(self):
        pass

    @staticmethod
    def fromBase64(*args):
        pass

    def toBase64(self):
        pass

    def getAddr(self, *args):
        pass


# PyDapCryptoKeyTypesObject
# DapCryptoKeyTypeObjectType
# Task: создание ключа
class CryptoKeyType(Protocol):
    def __str__(self):
        pass

    def __eq__(self, other: CryptoKeyType):
        pass


# PyCryptoKeyTypesObjectGetSetDef
class KeyTypes(Protocol):
    @property
    def IAES(self):
        pass

    @property
    def OAES(self):
        pass

    @property
    def BF_CBC(self):
        pass

    @property
    def BF_OFB(self):
        pass

    @property
    def GOST_OFB(self):
        pass

    @property
    def KUZN_OFB(self):
        pass

    @property
    def SALSA2012(self):
        pass

    @property
    def SEED_OFB(self):
        pass

    @property
    def RLWE_NEWHOPE_CPA_KEM(self):
        pass

    @property
    def MSRLN(self):
        pass

    @property
    def RLWE_MSRLN16(self):
        pass

    @property
    def RLWE_BCNS15(self):
        pass

    @property
    def LWE_FRODO(self):
        pass

    @property
    def CODE_MCBITS(self):
        pass

    @property
    def NTRU(self):
        pass

    @property
    def MLWE_KYBER(self):
        pass

    @property
    def SIG_PICNIC(self):
        pass

    @property
    def SIG_BLISS(self):
        pass

    @property
    def SIG_TESLA(self):
        pass

    @property
    def SIG_DILITHIUM(self):
        pass

    @property
    def SIG_RINGCT20(self):
        pass

    @property
    def KEM_KYBER512(self):
        pass

    @property
    def SIG_FALCON(self):
        pass


# DapCryptoDataTypeObjectType
class CryptoDataType(Protocol):
    pass


# DapChainHashFastObjectType
class HashFast(Protocol):
    def __init__(self, data: bytes | DatumTx | DatumEmission | DatumEmission):
        pass

    @staticmethod
    def fromString(hash: str, /) -> HashFast:
        pass

    @staticmethod
    def compare(hash1: HashFast, hash2: HashFast) -> bool:
        pass

    def __eq__(self, other):
        ...

    # def hashFast(self, bytes: bytes, size: int, /) -> bool:
    #     # TODO: WTF
    #     pass

    def isBlank(self) -> bool:
        pass

    # def toStr(self, string: str, string_max: int):
    #     pass

    # def toStrNew(self) -> str:
    #     pass

    def __str__(self):
        pass


# DapHashFastObjectType
# TODO: не нашел явного импорта в python
# class HashType(Protocol):
#     @staticmethod
#     def DAP_HASH_TYPE_KECCAK():
#         pass
#
#     @staticmethod
#     def DAP_HASH_TYPE_SLOW_0():
#         pass

# PyCryptoKeyObject
# private key
class Key(Protocol):
    pass


# class PkeyType(Protocol):
#     def __str__(self):
#         pass


# DapPkeyObject_DapPkeyObjectType
class Pkey(Protocol):
    def toBytes(self) -> bytes:
        pass

    @staticmethod
    def fromBytes(self, data: bytes, /) -> Pkey:
        pass

    def encrypt(self, data: bytes):
        pass

    @property
    def hash(self) -> HashFast:
        pass

    @property
    def type(self) -> str:
        pass

    @property
    def size(self) -> int:
        pass
