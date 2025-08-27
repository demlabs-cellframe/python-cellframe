from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from .net import CFNet
    from .types import OriginItemTypes
    from ..common.types import ItemTypes

import python_dap as dap
from CellFrame.legacy.pycfhelpers.logger import log

class CfTxItem:
    """
    The item in CellFrame.

    Attributes:
        type (str): The type of the item.
        net (CFNet): The CFNet instance.
    """

    def __init__(self, origin_item: OriginItemTypes, type: ItemTypes,
                 net: CFNet):
        """
        Initialize a new CfTxItem instance.

        Args:
            origin_item (OriginItemTypes): The original item.
            type (ItemTypes): The type of the item.
            net (CFNet): The CFNet instance.
        """
        self._origin_item = origin_item
        self.type = type.value
        self.net = net


class CfTxIn(CfTxItem):
    """
    The transaction input in CellFrame.

    Attributes:
        prev_hash (str): The hash of the previous transaction.
        prev_idx (int): The index of the previous output in the previous
            transaction.
    """

    def __init__(self, *args, **kwargs):
        """Initialize a new CfTxIn instance."""
        super().__init__(*args, **kwargs)
        self.prev_hash = str(self._origin_item.prevHash)
        self.prev_idx = self._origin_item.prevIdx


class CfTxInCond(CfTxItem):
    def __init__(self, *args, **kwargs):
        """Initialize a new CfTxInCond instance."""
        super().__init__(*args, **kwargs)
        self.prev_hash = str(self._origin_item.prevHash)
        self.out_prev_idx = self._origin_item.outPrevIdx
        self.receipt_prev_idx = self._origin_item.receiptPrevIdx


class CfTxOut(CfTxItem):
    """
    The transaction output in CellFrame.

    Attributes:
        address (str): The address of the recipient.
        value (int): The value of the output.
    """

    def __init__(self, *args, **kwargs):
        """Initialize a new CfTxOut instance."""
        from CellFrame.legacy.pycfhelpers.node.net import CFWalletAddress

        super().__init__(*args, **kwargs)
        self.address = CFWalletAddress(str(self._origin_item.addr))
        self.value = self._origin_item.value

class CfTxOutCond(CfTxItem):
    """
    The conditional transaction output in CellFrame.

    Attributes:
        expires (int): The expiration timestamp of the conditional output.
        value (int): The value of the output.
        subtype (str): The subtype of the output.
        tag (str | None): The optional tag if present in the conditional output.
    """

    def __init__(self, *args, **kwargs):
        """Initialize a new CfTxOutCond instance."""
        super().__init__(*args, **kwargs)
        self.expires = self._origin_item.tsExpires
        self.value = self._origin_item.value
        self.subtype = str(self._origin_item.typeSubtype)
        self.tag = getattr(self._origin_item, 'tag', None)


class CfTxPkey(CfTxItem):
    """The transaction public key in CellFrame."""

    pass


class CfTxSig(CfTxItem):
    """
    The transaction signature in CellFrame.

    Attributes:
        size (int): The size of the signature.
        sign (CFSign): The cryptographic signature.
    """

    def __init__(self, *args, **kwargs):
        """Initialize a new CfTxSig instance."""
        super().__init__(*args, **kwargs)
        self.size = self._origin_item.sigSize
        self.sign = CFSign(self._origin_item.sign, self.net)


class CfTxToken(CfTxItem):
    """
    The token transaction in CellFrame.

    Attributes:
        ticker (str): The ticker symbol of the token.
        emission_hash (str): The hash of the token emission.
        emission_chain_id (str): The chain ID of the token emission.
    """

    def __init__(self, *args, **kwargs):
        """Initialize a new CfTxToken instance."""
        super().__init__(*args, **kwargs)
        self.ticker = self._origin_item.ticker
        self.emission_hash = str(self._origin_item.tokenEmissionHash)
        self.emission_chain_id = str(self._origin_item.tokenEmissionChainId)


class CfTxReceipt(CfTxItem):
    """
    The transaction receipt in CellFrame.

    Attributes:
        size (int): The size of the transaction receipt.
        extSize (int): The extended size.
        units (int): The number of units.
        uid (int): The unique identifier.
        unitsType (str): The type of units in the transaction receipt.
        value (int): The value of the transaction receipt.
        provider (Sign | None): The signature provider of the transaction receipt.
        client (Sign | None): The client signature of the transaction receipt.
    """
    def __init__(self, *args, **kwargs):
        """Initialize a CfTxReceipt instance."""
        super().__init__(*args, **kwargs)
        self.size = self._origin_item.size
        self.extSize = self._origin_item.extSize
        self.units = self._origin_item.units
        self.uid = self._origin_item.uid
        self.value = self._origin_item.value
        self._provider = self._origin_item.provider
        self._client = self._origin_item.client

    @property
    def provider(self):
        if self._provider is None:
            return None
        return CFSign(self._provider, self.net)

    @property
    def client(self):
        if self._client is None:
            return None
        return CFSign(self._client, self.net)

class CfTxReceiptOld(CfTxItem):
    """
    The old transaction receipt in CellFrame.
    """
    def __init__(self, *args, **kwargs):
        """Initialize a new CfTxReceiptOld instance."""
        super().__init__(*args, **kwargs)
      
      
class CfTxOutExt(CfTxItem):
    """
    The extended transaction output in CellFrame.

    Attributes:
        address (str): The address of the recipient.
        ticker (str): The ticker symbol of the token.
        value (int): The value of the output.
    """

    def __init__(self, *args, **kwargs):
        try:
            from CellFrame.legacy.pycfhelpers.node.net import CFWalletAddress
            """Initialize a new CfTxOutExt instance."""
            super().__init__(*args, **kwargs)
            self.address = CFWalletAddress(str(self._origin_item.addr))
            self.ticker = self._origin_item.token
            self.value = self._origin_item.value
        except Exception as e:
            log.error(f"Failed to construct CfTxOutExt: {e}")
            raise 


class CfTxTSD(CfTxItem):
    """
    The transaction TSD in CellFrame.

    Attributes:

        data (bytes): The data of a certain type.
        type (int): The TSD type.
    """
    def __init__(self, *args, **kwargs):
        """Initialize a new CfTxTSD instance."""
        super().__init__(*args, **kwargs)
        self.data_value = self._origin_item.data
        self.data_type = self._origin_item.type


class CfTxOutCondSubtypeSrvPay(CfTxItem):
    """The server payment subtype of a conditional transaction output in
    CellFrame."""

    pass


class CfTxOutCondSubtypeSrvXchange(CfTxItem):
    """The server exchange subtype of a conditional transaction output in
    CellFrame."""

    pass


class CfTxOutCondSubtypeSrvStakeLock(CfTxItem):
    """
    The server stake lock subtype of a conditional transaction output in
    CellFrame.

    Attributes:
        unlock (int): The unlock time for the stake.
        value (int): The value of the stake.
        reinvest_percent (int): The reinvestment percentage for the stake.
    """

    def __init__(self, *args, **kwargs):
        """Initialize a new CfTxOutCondSubtypeSrvStakeLock instance."""
        super().__init__(*args, **kwargs)
        self.unlock = self._origin_item.timeUnlock
        self.value = self._origin_item.value
        self.reinvest_percent = self._origin_item.reinvestPercent


class CfTxOutCondSubtypeSrvStakePosDelegate(CfTxItem):
    """The server stake pos delegate subtype of a conditional transaction
    output in CellFrame."""

    pass


class CfTxVoting(CfTxItem):
    """The transaction voting in CellFrame."""

    pass


class CfTxVote(CfTxItem):
    """The transaction vote in CellFrame."""

    pass

class CfTxOutStd(CfTxItem):
    """
    The standard transaction output in CellFrame.

    Attributes:
        address (CFWalletAddress): The recipient address.
        ticker (str): Token symbol.
        value (int): Amount in base units.
        version (int): Version of output format.
        time_lock (int): Unlock time for the output.
        used_by (str | None): Hash of transaction that spent this output, if any.
    """

    def __init__(self, *args, **kwargs):
        try:
            from CellFrame.legacy.pycfhelpers.node.net import CFWalletAddress

            super().__init__(*args, **kwargs)

            self.address = CFWalletAddress(str(self._origin_item.addr))
            self.ticker = self._origin_item.token
            self.value = self._origin_item.value
            self.version = self._origin_item.version
            self.time_lock = self._origin_item.timeLock
            self.used_by = None
        except Exception as e:
            log.error(f"Failed to construct CfTxOutExt: {e}")
            raise 
