from collections import namedtuple
from CellFrame.Common import (
    TxIn, TxInCond, TxOut, TxOutCond, TxPkey, TxSig, TxToken, TxReceipt,
    TxReceiptOld, TxOutExt, TxTSD,
    TxOutCondSubtypeSrvPay, TxOutCondSubtypeSrvXchange,
    TxOutCondSubtypeSrvStakeLock, TxOutCondSubtypeSrvStakePosDelegate,
    TxVoting, TxVote, TxOutStd
)

from ..common.types import ItemTypes, DatumTypes
from .items import (
    CFItem, CFTxIn, CFTxInCond, CFTxOut, CFTxOutCond, CFTxPkey, CFTxSig,
    CFTxToken, CFTxReceipt, CFTxReceiptOld, CFTxOutExt, CFTxTSD,
    CFTxOutCondSubtypeSrvPay, CFTxOutCondSubtypeSrvXchange,
    CFTxOutCondSubtypeSrvStakeLock, CFTxOutCondSubtypeSrvStakePosDelegate,
    CFTxVoting, CFTxVote, CFTxOutStd
)

from .datums import (
    CFDatumTX, CFDatumToken, CFDatumEmission, CFDatumCustom,
    CFDatumDecree, CFDatumAnchor
)

ItemMeta = namedtuple('ItemMapping', ['cf_wrapper_cls', 'type'])

ITEM_WRAPPER_MAPPING = {
    TxIn: ItemMeta(CFTxIn, ItemTypes.TX_ITEM_TYPE_IN),
    TxInCond: ItemMeta(CFTxInCond, ItemTypes.TX_ITEM_TYPE_IN_COND),
    TxOut: ItemMeta(CFTxOut, ItemTypes.TX_ITEM_TYPE_OUT),
    TxOutCond: ItemMeta(CFTxOutCond, ItemTypes.TX_ITEM_TYPE_OUT_COND),
    TxPkey: ItemMeta(CFTxPkey, ItemTypes.TX_ITEM_TYPE_PKEY),
    TxSig: ItemMeta(CFTxSig, ItemTypes.TX_ITEM_TYPE_SIG),
    TxToken: ItemMeta(CFTxToken, ItemTypes.TX_ITEM_TYPE_IN_EMS),
    TxReceipt: ItemMeta(CFTxReceipt, ItemTypes.TX_ITEM_TYPE_RECEIPT),
    TxReceiptOld: ItemMeta(CFTxReceiptOld, ItemTypes.TX_ITEM_TYPE_RECEIPT_OLD),
    TxOutExt: ItemMeta(CFTxOutExt, ItemTypes.TX_ITEM_TYPE_OUT_EXT),
    TxTSD: ItemMeta(CFTxTSD, ItemTypes.TX_ITEM_TYPE_TSD),
    TxOutCondSubtypeSrvPay: ItemMeta(
        CFTxOutCondSubtypeSrvPay, 
        ItemTypes.DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY
    ),
    TxOutCondSubtypeSrvXchange: ItemMeta(
        CFTxOutCondSubtypeSrvXchange,
        ItemTypes.DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE
    ),
    TxOutCondSubtypeSrvStakeLock: ItemMeta(
        CFTxOutCondSubtypeSrvStakeLock,
        ItemTypes.DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK
    ),
    TxOutCondSubtypeSrvStakePosDelegate: ItemMeta(
        CFTxOutCondSubtypeSrvStakePosDelegate,
        ItemTypes.DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE
    ),
    TxVoting: ItemMeta(CFTxVoting, ItemTypes.TX_ITEM_TYPE_VOTING),
    TxVote: ItemMeta(CFTxVote, ItemTypes.TX_ITEM_TYPE_VOTE),
    TxOutStd: ItemMeta(CFTxOutStd, ItemTypes.TX_ITEM_TYPE_OUT_STD),
}


class CFItemMapper:
    """Maps item types to corresponding CellFrame item wrapper classes."""

    @staticmethod
    def build(origin_item, chain) -> CFItem:
        """Build a CellFrame item based on the provided origin item.

        Args:
            origin_item: The original item.
            net: The CFNet instance representing the network context.

        Returns:
            CFItem: The CellFrame item.
        """
        mapping = ITEM_WRAPPER_MAPPING.get(type(origin_item))

        return mapping.cf_wrapper_cls(origin_item, mapping.type, chain)


SubDatumMeta = namedtuple('SubDatumMapping', ['origin_method_name', 'cf_wrapper_cls'])

SUB_DATUM_WRAPPER_MAPPING = {
    DatumTypes.DATUM_TX: SubDatumMeta("getDatumTX", CFDatumTX),
    DatumTypes.DATUM_TOKEN_DECL: SubDatumMeta("getDatumToken", CFDatumToken),
    DatumTypes.DATUM_TOKEN: SubDatumMeta("getDatumToken", CFDatumToken),
    DatumTypes.DATUM_TOKEN_EMISSION: SubDatumMeta("getDatumTokenEmission", 
                                                  CFDatumEmission),
    DatumTypes.DATUM_CUSTOM: SubDatumMeta("getDatumCustom", CFDatumCustom),
    DatumTypes.DATUM_DECREE: SubDatumMeta("getDatumDecree", CFDatumDecree),
    DatumTypes.DATUM_ANCHOR: SubDatumMeta("getDatumAnchor", CFDatumAnchor),
}


class CFSubDatumBuilder:
    """Build sub-datums for CellFrame based on datum type."""

    def __init__(self, type):
        """
        Initialize a new CFSubDatumBuilder instance.

        Args:
            type (DatumTypes): The type of datum for which sub-datum is being 
                built. Should be one of the enumeration values from DatumTypes.
        """
        self.type = type

    def build(self, parent_datum):
        """
        Build a sub-datum for CellFrame based on the parent datum.

        Args:
            parent_datum: The parent datum.

        Returns:
            CFDatum: The sub-datum.
        """
        method = getattr(
            parent_datum._origin_datum, 
            SUB_DATUM_WRAPPER_MAPPING[self.type].origin_method_name
        )
        sub_datum = method()
        return SUB_DATUM_WRAPPER_MAPPING[self.type].cf_wrapper_cls(
            parent_datum, sub_datum
        )
