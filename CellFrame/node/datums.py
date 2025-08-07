from __future__ import annotations

from typing import TypeVar, TYPE_CHECKING
import json
import binascii
from datetime import datetime
from abc import ABC, abstractmethod

from CellFrame.legacy.pycfhelpers.logger import log

from .crypto import CFCertificate
from CellFrame.common import Datum, DatumTx, DatumToken, DatumDecree, DatumAnchor, DatumEmission, TxTSD
from CellFrame.chain import ChainAddr
from CellFrame.legacy.DAP.Core import Math

if TYPE_CHECKING:
    from .net import CFNet, CFChain, CFWalletAddress
    from .consensus import CFBlock, CFEvent
    from CellFrame.types import ticker

from .items import CFItem, CFTxOut, CFTxOutCond, CFTxToken, CFTxSig, CFTxTSD
from .crypto import CFSign, CFKey
from .types import TSD, CFLedgerCacheResponse
from .exceptions import InsufficientFundsError


class CFDatum:
    """
    The Datum in the CellFrame.

    Attributes:
        hash (str): The hash of the datum.
        type (str): The type of the datum.
        version (str): The version of the datum.
        size (int): The size of the datum.
        created_at (datetime): The creation timestamp of the datum.
        atom (CFBlock | CFEvent | None): The parent atom of the datum.
        net (CFNet | None): The CFNet instance.
        """

    # __slots__ = ["_origin_datum", "hash", "type", "created_at", "atom", "size", "version", "net"]

    def __init__(self, atom: CFBlock | CFEvent | None, datum: Datum, chain: CFChain | None = None):
        """Initialize a new CFDatum instance.

        Args:
            atom (CFBlock | CFEvent | None): The parent atom of the datum.
            datum (Datum): The datum object.
            chain (CFChain | None): The CFChain instance. Defaults to None.

        Raises:
            AttributeError: If atom is None and net is None.
        """
        self._origin_datum = datum
        self.hash = str(datum.hash)
        self.type = datum.getTypeStr()
        self.version = datum.versionStr
        self.size = datum.getSize()
        try:
            self.created_at = datum.tsCreated
        except OSError:
            self.created_at = datetime.fromtimestamp(0)
            log.error(f"Datum [{datum.hash}] has invalid timestamp!")
        if atom is None and chain is None:
            raise AttributeError("A datum without a parent atom requires a net")
        self.atom = atom  # atom == None - datum in mempool

        self.chain = chain
        self.net = self.chain.net

    def get_sub_datum(
            self) -> CFDatumTX | CFDatumToken | CFDatumEmission | CFDatumAnchor | CFDatumDecree | CFDatumCustom:
        """Retrieve the sub-datum associated with this datum.

        Returns:
            Union[CFDatumTX, CFDatumToken, CFDatumEmission, CFDatumAnchor, CFDatumDecree, CFDatumCustom]: The sub-datum object.
        """
        from .mappings import CFSubDatumBuilder
        sub_datum = CFSubDatumBuilder(self.type).build(self)
        return sub_datum

    @property
    def signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the datum.

        Returns:
            list[CFSign]: The list of signatures.
        """
        return [sign for sign in self.get_sub_datum().get_signs()]
    
    def serialize(self) -> bytes:
        """Serialize the datum to bytes.

        Returns:
            bytes: The serialized datum.
        """
        return self._origin_datum.raw

    def __repr__(self):
        """Return a string representation of the datum.

        Returns:
            str: The string representation of the datum containing
            type and hash.
        """
        return f"{self.type}:{self.hash}"


class CFSubDatum(ABC):
    """
    Represent a sub-datum in CellFrame.

    Attributes:
        hash (str): The hash value of the sub-datum.
        type (str): The type of the sub-datum, defaults to "DEFAULT"
    """

    def __init__(self, parent_datum: CFDatum | None,
                 sub_datum: DatumTx | DatumToken | DatumEmission | DatumDecree | DatumAnchor | bytes,
                 chain: CFChain | None = None):
        """Initialize a new CFSubDatum instance.

        Args:
            parent_datum (CFDatum | None): The parent datum associated with the sub-datum, or None if there's no parent datum.
            sub_datum (Union[DatumTx, DatumToken, DatumEmission, DatumDecree, DatumAnchor, bytes]):
            The sub-datum object or its bytes representation.
            chain (CFChain | None): The CFChain instance representing the chain, defaults to None.
        """
        self._parent_datum = parent_datum
        self._origin_sub_datum = sub_datum
        self.type = "DEFAULT"
        self.chain = chain
        if self._parent_datum and self._parent_datum.atom is None:  # mempool datum
            self._net = self._parent_datum.net
            self.chain = self._parent_datum.chain
        elif self._parent_datum:  # from chains
            self._net = self._parent_datum.atom.chain.net
            self.chain = self._parent_datum.atom.chain
        else:  # some custom created emission with no parent.
            if self.chain:
                self._net = chain.net
            else:
                self._net = None

        if sub_datum is None:
            self.hash = parent_datum.hash
            self.type = "CORRUPTED"
            # log.error(f"Datum type:{parent_datum.type} hash:{parent_datum.hash} is CORRUPTED")

    @property
    def net(self) -> 'CFNet':
        """Get the CFNet instance representing the network.

        Returns:
            CFNet: The CFNet instance.
        """
        return self._net
    
    @abstractmethod
    def get_signs(self) -> list[CFSign]:
        pass
    
T = TypeVar('T', bound=CFItem)


# Flag to designate a base transaction
# @is_base_tx -> bool - looks for TxToken[hash for which emission]
# base transaction that has an item of TxToken type
class CFDatumTX(CFSubDatum):
    """
    The transaction sub-datum in CellFrame.

    Attributes:
        created_at (datetime): The datetime when the sub-datum was created.
        ticker (str): The ticker associated with the transaction sub-datum.
        ledger_rc (CFLedgerCacheResponse): The response code from the ledger cache.
        accepted (bool): Indicates whether the transaction is accepted or not.

    Args:
        CFSubDatum (CFSubDatum): a sub-datum in CellFrame
    """

    def __init__(self, parent_datum: CFDatum | None, sub_datum: DatumTx, chain: CFChain | None = None):
        """Initialize a new CFDatumTX instance.

        Args:
            parent_datum (CFDatum | None): The parent datum associated with the sub-datum, or None if there's no parent datum.
            sub_datum (DatumTx): The transaction sub-datum object.
            net (CFChain | None): The CFChain instance representing the
            network context, defaults to None.

        """
        super().__init__(parent_datum, sub_datum, chain=chain)
        if self.type == "CORRUPTED":
            return

        self.hash = str(sub_datum.hash)
        self.created_at = sub_datum.dateCreated
        ledger = self.net.get_ledger()
        self.ticker = ledger.get_tx_ticker(self)
        self.ledger_rc = ledger.get_tx_ledger_rc(self)

        self.accepted = (self.ledger_rc == CFLedgerCacheResponse.DAP_LEDGER_CHECK_ALREADY_CACHED) or (
            self.ledger_rc == CFLedgerCacheResponse.DAP_LEDGER_CHECK_OK)

        self.service_tags = self._origin_sub_datum.getServiceTags(ledger._origin_ledger)

    @property
    def items(self) -> list[CFItem]:
        """Get the list of items associated with this transaction.

        Returns:
            list[CFItem]: The list of items.
        """
        return self.get_items()

    @property
    def is_base_tx(self) -> bool:
        """Check if the transaction is a base transaction.

        Returns:
            bool: True if the transaction is a base transaction, False otherwise.
        """
        return bool(self.get_items(filter_type=CFTxToken))

    def get_signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the transaction.

        Returns:
            list[CFSign]: The list of signatures.
        """
        return [sign.sign for sign in self.get_items(filter_type=CFTxSig)]
    
    @property
    def signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the transaction.

        Returns:
            list[CFSign]: The list of signatures.
        """
        return self.get_signs()
    
    @property
    def address_from(self) -> 'CFWalletAddress' | None:
        """Get the source address for the transaction.

        Returns:
            CFWalletAddress | None: The source address for the transaction.
        """
        signs = self.get_items(filter_type=CFTxSig)
        
        if not signs:
            return None
        return signs[0].sign.address

    @property
    def is_base_tx(self) -> bool:
        try:
            items = self.get_items(filter_type=CFTxToken)
            return bool(items)
        except Exception as e:
            log.error(f"Error in is_base_tx for transaction {self.hash}: {e}")
            return False
        
    def get_items(self, filter_type: type[T] | None = None) -> list[T]:
        """Get the list of items associated with this transaction, optionally filtered by type.

        Args:
            filter_type (Optional[type[T]]): The type to filter items by, defaults to None.

        Returns:
            list[T]: The list of items, optionally filtered by type.
        """
        from .mappings import CFItemMapper
        all_items = [CFItemMapper.build(item, self.net) for item in self._origin_sub_datum.getItems()
                     if item is not None]
        if filter_type:
            return [item for item in all_items if isinstance(item, filter_type)]
        else:
            return all_items
        
    def get_tsd(self, filter_tsd: TSD | None = None) -> dict[str, any] | any | None:
        """
        Returns the typed section data associated with this transaction in decoded form.
        
        If no filter is provided, returns a dictionary mapping TSD type names (e.g. "TYPE_SOURCE")
        to decoded values.
        
        If filter_tsd is provided, returns the decoded value for that specific TSD key (or None if not found).

        Args:
            filter_type (Optional[TSD]): The type to filter TSD by, defaults to None.

        Returns:
            Optional[dict[str, any]]: The decoded value for TSD key.

        """
        tsd_items = self.get_items(filter_type=CFTxTSD)
        
        if filter_tsd is not None:
            for tsd_item in tsd_items:
                if tsd_item.data_type == filter_tsd.value:
                    try:
                        decoded = tsd_item.data_value.decode('utf-8')
                    except UnicodeDecodeError:
                        decoded = binascii.hexlify(tsd_item.data_value).decode('utf-8')
                    if filter_tsd == TSD.TYPE_DATA:
                        try:
                            decoded = json.loads(decoded)
                        except Exception:
                            pass
                    return decoded
            return None

        tsd_data = {}
        for tsd_item in tsd_items:
            tsd_type_name = None
            for tsd_enum in TSD:
                if tsd_enum.value == tsd_item.data_type:
                    tsd_type_name = tsd_enum.name
                    break
            if not tsd_type_name:
                tsd_type_name = str(tsd_item.data_type)
            try:
                decoded = tsd_item.data_value.decode('utf-8')
            except UnicodeDecodeError:
                decoded = binascii.hexlify(tsd_item.data_value).decode('utf-8')
            except Exception:
                log.error(f"Incorrect TSD data. Skip TSD with type={tsd_type_name}")
                continue
            if tsd_type_name == "TYPE_DATA":
                try:
                    decoded = json.loads(decoded)
                except Exception:
                    pass
            tsd_data[tsd_type_name] = decoded
        return tsd_data

    def sign(self, key: CFKey):
        self._origin_sub_datum.addSignItem(key._origin_key)
        sign_items = self.get_items(CFTxSig)
        return sign_items[-1]

    def append_sign(self, sign: CFSign):
        res = self._origin_sub_datum.appendSignItem(sign._origin_sign)
        return sign if res else None

    @staticmethod
    def create(
        sender_addr:      'CFWalletAddress',
        recipient_addr:   'CFWalletAddress',
        amount_tokens:    int | str,
        token_ticker:     str,
        chain:            'CFChain',
        tsd_data:         dict | None = None,
        validator_fee:    int | str = 0,
    ) -> 'CFDatumTX':
        """Build an unsigned transaction.

        Args:
            sender_addr:    Address funds are taken from.
            recipient_addr: Address funds are sent to.
            amount_tokens:  Transfer amount in datoshi.
            token_ticker:   Token ticker.
            chain:          Target chain.
            tsd_data:       Optional map {tsd_type: value}.
            validator_fee:  Fee paid to the block-producer
                            (0 → use network average).

        Raises:
            InsufficientFundsError: Balance is not enough to cover
                transfer amount and all required fees.

        Returns:
            Unsigned :class:`CFDatumTX`.
        """
        import json

        net = chain.net
        fee_profile = net.fee_data

        transfer_datoshi = int(str(amount_tokens))
        network_fee_datoshi = int(str(fee_profile.tx_fee))
        validator_fee_input = int(str(validator_fee))
        validator_fee_datoshi = (
            validator_fee_input
            if validator_fee_input > 0
            else int(str(fee_profile.validator_avg_fee))
        )
        required_total_datoshi = (
            transfer_datoshi + network_fee_datoshi + validator_fee_datoshi
        )

        ledger = net.get_ledger()
        inputs = ledger.list_unspent(
            sender_addr,
            token_ticker,
            required_total_datoshi,
        )
        if not inputs:
            raise InsufficientFundsError(0, required_total_datoshi)

        tx = DatumTx()
        gathered_datoshi = 0
        for tx_hash, out_idx, value_datoshi in inputs:
            tx.addInItem(tx_hash, out_idx)
            gathered_datoshi += value_datoshi
            if gathered_datoshi >= required_total_datoshi:
                break

        if gathered_datoshi < required_total_datoshi:
            raise InsufficientFundsError(
                gathered_datoshi,
                required_total_datoshi,
            )

        tx.addOutStdItem(
            recipient_addr._original_addr,
            transfer_datoshi,
            token_ticker,
        )

        tx.addFeeItem(validator_fee_datoshi)

        from .net import CFWalletAddress
        fee_net_addr = CFWalletAddress(fee_profile.tx_fee_addr)
        tx.addOutStdItem(
            fee_net_addr._original_addr,
            network_fee_datoshi,
            token_ticker,
        )

        change_datoshi = gathered_datoshi - required_total_datoshi
        if change_datoshi:
            tx.addOutStdItem(
                sender_addr._original_addr,
                change_datoshi,
                token_ticker,
            )

        if tsd_data:
            for tsd_key, tsd_value in tsd_data.items():
                tx.addTSDItem(tsd_key, tsd_value.encode("utf-8"))

        return CFDatumTX(None, tx, chain)

    def __str__(self):
        return self.hash

    def __repr__(self):
        return str(self)


class CFDatumToken(CFSubDatum):
    """
    Represents a token sub-datum in CellFrame.

    Attributes:
        hash (str): The hash value of the token sub-datum.
        type (str): The type of the token sub-datum.
        ticker (str): The ticker symbol of the token.
        data (str): Additional data associated with the token.
    Properties:
        signs (list[CFSign]): List of cryptographic signatures
            associated with the token.
    """

    def __init__(self, parent_datum: CFDatum | None, sub_datum: DatumToken):
        """Initialize a new CFDatumToken instance.

        Args:
            parent_datum (CFDatum | None): The parent datum associated with the sub-datum, or None if there's no parent datum.
            sub_datum (DatumToken): The token sub-datum object.
        """
        super().__init__(parent_datum, sub_datum)
        if self.type == "CORRUPTED":
            return
        self.ticker = sub_datum.ticker
        self.type = sub_datum.typeStr
        self.data = sub_datum.data
        if parent_datum:
            self.hash = str(parent_datum.hash)
        else:
            self.hash = str(sub_datum.hash)

    def get_signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the token.

        Returns:
            list[CFSign]: The list of signatures.
        """
        try:
            return [CFSign(sign, self.net) for sign in self._origin_sub_datum.signs]
        except AttributeError:
            return []
    
    @property   
    def signs(self) -> list[CFSign]:
        """Get the list of signature hashes associated with the token.

        Returns:
            list[str]: The list of signature hashes.
        """
        return self.get_signs()
    
class CFDatumEmission(CFSubDatum):
    """
    The emission sub-datum in CellFrame.

    Attributes:
        hash (str): The hash value of the emission sub-datum.
        type (str): The type of the emission sub-datum.
        ticker (str): The ticker symbol associated with the token.
        version (str): The version of the emission.
        address (str): The address associated with the emission.
        value (int): The value of the emission, representing a quantity of some unit.
        data (list[Union[CFSign, dict, str]]): Additional data associated with the emission, including cryptographic signatures, dictionaries, or strings.
        tsd (dict[str, Union[str, dict[str, Any]]]): Typed section data associated with the emission, containing various information in a structured format.
    """

    def __init__(self, parent_datum: CFDatum | None, sub_datum: DatumEmission, chain: CFChain | None = None):
        """Initialize a new CFDatumEmission instance.

        Args:
            parent_datum (CFDatum | None): The parent datum associated with the sub-datum, or None if there's no parent datum.
            sub_datum (DatumEmission): The emission sub-datum object.
            net (CFNet | None): The CFNet instance representing the network context, defaults to None.
        """
        from CellFrame.legacy.pycfhelpers.node.net import CFWalletAddress
        super().__init__(parent_datum, sub_datum, chain=chain)
        if self.type == "CORRUPTED":
            return

        self.version = sub_datum.version
        self.type = sub_datum.typeStr
        self.ticker: ticker = sub_datum.ticker
        self.address = CFWalletAddress(str(sub_datum.addr))
        self.hash = str(sub_datum.hash)
        # TODO: Math --> CFMath
        self.value: Math = sub_datum.value
        if self.type == "TOKEN_EMISSION_TYPE_AUTH":
            self.data = [CFSign(sign, self.net) for sign in sub_datum.signs]
        else:
            self.data = sub_datum.data

        self.tsd = {}
        for tsd_type in TSD:
            tsd = self._origin_sub_datum.getTSD(tsd_type.value)
            if tsd is None:
                continue
            try:
                tsd_data = tsd.decode('utf-8')
            except UnicodeDecodeError:
                tsd_data = binascii.hexlify(tsd).decode('utf-8')
            except Exception:
                log.error(f"Incorrect TSD data. Skip TSD with type={tsd_type}")
                continue

            if tsd_type == TSD.TYPE_DATA:
                try:
                    tsd_data = json.loads(tsd_data)
                except Exception:
                    pass

            # FIXME: [tsd_type.name] --> [tsd_type]
            self.tsd[tsd_type] = tsd_data

    def get_signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the emission.

        Returns:
            list[CFSign]: The list of signatures.
        """
        if self.type == "CORRUPTED":
            return []
        return [CFSign(sign, self.net) for sign in self._origin_sub_datum.signs]
    
    @property
    def signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the emission.

        Returns:
            list[CFSign]: The list of signatures.
        """
        return self.get_signs()

    def __repr__(self):
        """Return a string representation of the datum.

        Returns:
            str: The string representation of the datum containing type and hash.
        """
        return f"{self.hash}"

    @property
    def valid_sign_hashes(self) -> list[str]:
        """Get the list of valid signature hashes associated with the emission.

        Returns:
            list[str]: The list of valid signature hashes.
        """
        token_auth_signs_pkey_hashes = self.net.get_ledger().token_auth_signs_pkey_hashes(self.ticker)
        return [sign.pkey_hash for sign in self.signs if
                sign.pkey_hash in token_auth_signs_pkey_hashes]

    def add_sign(self, certificate_or_sign: CFCertificate | CFSign) -> CFSign:
        """Add a new signature to the emission.

        Args:
            certificate (CFCertificate | CFSign): The certificate or signature to add.

        Returns:
            CFSign: The added signature.
        """
        if isinstance(certificate_or_sign, CFCertificate):
            self._origin_sub_datum.addSign(certificate_or_sign._origin_certificate)
            return self.signs[-1]
        if isinstance(certificate_or_sign, CFSign):
            self._origin_sub_datum.appendSign(certificate_or_sign.serialize())
            return self.signs[-1]

    @staticmethod
    def create(wallet: 'CFWalletAddress', token_symbol: str, value: int,
               tsd_data: dict, chain: CFChain | None = None) -> CFDatumEmission:
        """Create a new CFDatumEmission instance.

        Args:
            wallet (CFWalletAddress): The wallet address.
            token_symbol (str): The token symbol.
            value (int): The value of the emission.
            tsd_data (dict): The typed section data.
            net (CFChain | None): The CFChain instance, defaults to None.

        Returns:
            CFDatumEmission: The created CFDatumEmission instance.
        """

        emission = DatumEmission(str(value), token_symbol, wallet._original_addr)

        for key, value in tsd_data.items():
            if isinstance(value, dict):
                emission.addTSD(key, json.dumps(value).encode("utf-8"))
            elif isinstance(value, list):
                emission.addTSD(key, json.dumps(value).encode("utf-8"))
            else:
                emission.addTSD(key, str(value).encode("utf-8"))

        return CFDatumEmission(parent_datum=None, sub_datum=emission, chain=chain)


class CFDatumDecree(CFSubDatum):
    """
    Represents a decree sub-datum in CellFrame.

    Attributes:
        hash (str): The hash value of the decree sub-datum.
        type (str): The type of the decree sub-datum.
        subtype (str): The subtype of the decree sub-datum.
        created_at (datetime): The creation timestamp of the decree sub-datum.
        signs (list[CFSign]): List of cryptographic signatures associated with the decree.
    """

    def __init__(self, parent_datum: CFDatum | None, sub_datum: DatumDecree):
        """Initialize a new CFDatumDecree instance.

        Args:
            parent_datum (CFDatum | None): The parent datum associated with the sub-datum, or None if there's no parent datum.
            sub_datum (DatumDecree): The decree sub-datum object.
        """
        super().__init__(parent_datum, sub_datum)
        if self.type == "CORRUPTED":
            return
        self.hash = str(sub_datum.hash)
        self.created_at = sub_datum.tsCreated
        self.type = sub_datum.typeStr
        self.subtype = sub_datum.subtypeStr
    
    def get_signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the decree.

        Returns:
            list[CFSign]: The list of signatures.
        """
        return [CFSign(sign, self.net) for sign in self._origin_sub_datum.signs]
    
    @property
    def signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the decree.

        Returns:
            list[CFSign]: The list of signatures.
        """
        return self.get_signs()


class CFDatumAnchor(CFSubDatum):
    """
    Represents an anchor sub-datum in CellFrame.

    Attributes:
        hash (str): The hash value of the anchor sub-datum.
        created_at (datetime): The creation timestamp of the anchor sub-datum.
        signs (list[CFSign]): List of cryptographic signatures associated with the anchor.
    """

    def __init__(self, parent_datum: CFDatum | None, sub_datum: DatumAnchor):
        """Initialize a new CFDatumAnchor instance.

        Args:
            parent_datum (CFDatum | None): The parent datum associated with the sub-datum, or None if there's no parent datum.
            sub_datum (DatumAnchor): The anchor sub-datum object.
        """
        super().__init__(parent_datum, sub_datum)
        if self.type == "CORRUPTED":
            return
        self.hash = str(sub_datum.hash)
        self.created_at = sub_datum.created

    def get_signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the anchor.

        Returns:
            list[CFSign]: The list of signatures.
        """
        return [CFSign(sign, self.net) for sign in self._origin_sub_datum.signs]
    
    @property
    def signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the anchor.

        Returns:
            list[CFSign]: The list of signatures.   
        """
        return self.get_signs()

class CFDatumCustom(CFSubDatum):
    """
    Represents a custom sub-datum in CellFrame.

    Attributes:
        hash (str): The hash value of the custom sub-datum.
        type (str): The type of the custom sub-datum.
        data (str): The raw data associated with the custom sub-datum, represented as hexadecimal string.
    """

    def __init__(self, parent_datum: CFDatum | None, sub_datum: bytes):
        """Initialize a new CFDatumCustom instance.

        Args:
            parent_datum (CFDatum | None): The parent datum associated with the sub-datum, or None if there's no parent datum.
            sub_datum (bytes): The raw data of the custom sub-datum.
        """
        super().__init__(parent_datum, sub_datum)
        if self.type == "CORRUPTED":
            return
        self._origin_sub_datum = None
        self.data = sub_datum.hex()
        
    def get_signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the custom sub-datum.

        Returns:
            list[CFSign]: The list of signatures.
        """
        return []   
    
    @property
    def signs(self) -> list[CFSign]:
        """Get the list of signatures associated with the custom sub-datum.

        Returns:
            list[CFSign]: The list of signatures.
        """
        return self.get_signs()

