from __future__ import annotations
from typing import TYPE_CHECKING, Optional
from CellFrame.legacy.DAP.Crypto import Sign, Pkey, Cert

try:
    from CellFrame.legacy.DAP.Crypto import GUUID
    HAVE_GUUID = True
except:
    print("Not GUID module!")
    HAVE_GUUID = False

if TYPE_CHECKING:
    from .net import CFNet

# from DAP import Crypto
from CellFrame.legacy.pycfhelpers.logger import log

# log.message(f"{dir(Crypto)=}")


class CFSign:
    """The CellFrame signature.

    Provides methods to work with CellFrame signatures.

    Attributes:
        net (Optional[CFNet]): The CFNet object associated with the signature, defaults to None.
        type (str): The type of the signature.
        pkey (CFPkey): The public key associated with the signature.
        size (int): The size of the signature.
        pkey_hash (str): The hash of the public key associated with the signature.
        address (str): The address derived from the signature.
    """

    # __slots__ = ["_origin_sign", "net", "type", "pkey", "size", "pkey_hash", "address"]

    def __init__(self, sign: Sign, net: CFNet | None = None):
        """Initialize a CFSign object with the given sign and net.

        Args:
            sign (Sign): The Sign object representing the signature.
            net (Optional[CFNet]): The CFNet object associated with the signature, defaults to None.
        """

        from CellFrame.legacy.pycfhelpers.node.net import CFWalletAddress
        self._origin_sign = sign
        self.net = net
        self.type = str(sign.type)
        self.pkey = CFPkey(sign.pkey)
        self.size = sign.size
        self.pkey_hash = str(self._origin_sign.pkeyHash)

        if self.net:
            self.address = CFWalletAddress(str(self._origin_sign.getAddr(self.net._origin_net)))

    def serialize(self) -> bytes:
        """Serialize the signature.

        Returns:
            bytes: The serialized signature.
        """
        return self._origin_sign.toBytes()

    @staticmethod
    def deserialize(data: bytes, net: CFNet) -> CFSign:
        """Deserialize the signature.

        Args:
            data (bytes): The serialized signature data.
            net (CFNet): The CFNet object associated with the signature.

        Returns:
            CFSign: The deserialized CFSign object.
        """
        return CFSign(Sign.fromBytes(data), net)


class CFPkey:
    """The CellFrame public key.

    Provides methods to work with CellFrame public keys.

    Attributes:
        _origin_pkey (Pkey): The origin public key.
        type (str): The type of the public key.
        size (int): The size of the public key.
        hash (str): The hash of the public key.
    """

    def __init__(self, pkey: Pkey):
        """Initialize a CFPkey object with the given pkey.

        Args:
            pkey (Pkey): The Pkey object representing the public key.
        """
        self._origin_pkey = pkey
        self.type = pkey.type
        self.size = pkey.size
        self.hash = str(pkey.hash)

    def _to_bytes(self) -> bytes:
        """Convert the public key to bytes.

        Returns:
            bytes: The bytes representation of the public key.
        """
        return self._origin_pkey.toBytes()

    @staticmethod
    def _from_bytes(data: bytes, /) -> CFPkey:
        """Create a CFPkey object from bytes.

        Args:
            data (bytes): The bytes representation of the public key.

        Returns:
            CFPkey: The CFPkey object created from bytes.
        """
        return CFPkey(Pkey.fromBytes(data))


class CFKey:
    """The CellFrame key.

    Attributes:
        _origin_key (Key): The original key.
    """
    def __init__(self, key: 'Key'):
        """Initialize a CFKey object with the given key.

        Args:
            key (Key): The original key.
        """
        self._origin_key = key

# data -> hash | Cert.private_key -> Sign.hash
# sign.pkey == cert.pkey


class CFCertificate:
    """The CellFrame certificate.

    Provides methods to work with CellFrame certificates.

    Attributes:
        pkey (CFPkey): The public key associated with the certificate.
        key (CFKey): The private key associated with the certificate.
        _origin_certificate (Cert): The origin certificate.
    """

    def __init__(self, certificate_name: str):
        """Initialize a CFCertificate object with the given certificate name.

        Args:
            certificate_name (str): The name of the certificate.

        Raises:
            ValueError: If the certificate with the given name is not found.
        """
        certificate = Cert.load(certificate_name)
        if certificate is None:
            raise ValueError(f"Certificate with name={certificate_name} not found")
        self._origin_certificate = certificate
        self.pkey = CFPkey(certificate.pkey())  # public
        self.key = certificate.key  # private

    @property
    def private_key(self) -> CFKey | None:
        """The private key associated with the certificate.

        Returns:
            CFKey | None: The private key, or None if not available.
        """
        return self.key

    @property
    def public_key(self) -> CFPkey:
        """The public key associated with the certificate.

        Returns:
            CFPkey: The public key.
        """
        return self.pkey

    def delete(self) -> None:
        """Delete the certificate."""
        self._origin_certificate.delete()


class CFGUUID:
    """The CellFrame globally unique user identifier.

    Attributes:
        _guuid (str): The string representation of the GUUID.
    """
    def __init__(self, hex_GUUID: str) -> None:
        """Initialize a CFGUUID object with the given GUUID string.

        Args:
            hex_GUUID (str): The string representation of the GUUID.
        """
        self._guuid = GUUID(hex_GUUID)

    @staticmethod
    def generate() -> CFGUUID:
        """Generate a new CFGUUID.

        Returns:
            CFGUUID: The generated CFGUUID object.
        """
        guuid = GUUID.generate()
        return CFGUUID(str(guuid))

    @staticmethod
    def compose(net_id: int, service_id: int) -> CFGUUID:
        """Compose a new CFGUUID from net_id and service_id.

        Args:
            net_id (int): The network ID.
            service_id (int): The service user ID.

        Returns:
            CFGUUID: The composed CFGUUID object.
        """
        guuid = GUUID.compose(net_id, service_id)
        return CFGUUID(str(guuid))

    def __str__(self) -> str:
        """Return the string representation of the CFGUUID.

        Returns:
            str: The string representation of the GUUID.
        """
        return str(self._guuid)
