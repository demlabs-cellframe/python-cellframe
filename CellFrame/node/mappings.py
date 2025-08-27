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
    CfItem, CfTxIn, CfTxInCond, CfTxOut, CfTxOutCond, CfTxPkey, CfTxSig,
    CfTxToken, CfTxReceipt, CfTxReceiptOld, CfTxOutExt, CfTxTSD,
    CfTxOutCondSubtypeSrvPay, CfTxOutCondSubtypeSrvXchange,
    CfTxOutCondSubtypeSrvStakeLock, CfTxOutCondSubtypeSrvStakePosDelegate,
    CfTxVoting, CfTxVote, CfTxOutStd
)

from .datums import (
    CfDatumTX, CfDatumToken, CfDatumEmission, CfDatumCustom,
    CfDatumDecree, CfDatumAnchor
)

ItemMeta = namedtuple('ItemMapping', ['cf_wrapper_cls', 'type'])

ITEM_WRAPPER_MAPPING = {
    TxIn: ItemMeta(CfTxIn, ItemTypes.TX_ITEM_TYPE_IN),
    TxInCond: ItemMeta(CfTxInCond, ItemTypes.TX_ITEM_TYPE_IN_COND),
    TxOut: ItemMeta(CfTxOut, ItemTypes.TX_ITEM_TYPE_OUT),
    TxOutCond: ItemMeta(CfTxOutCond, ItemTypes.TX_ITEM_TYPE_OUT_COND),
    TxPkey: ItemMeta(CfTxPkey, ItemTypes.TX_ITEM_TYPE_PKEY),
    TxSig: ItemMeta(CfTxSig, ItemTypes.TX_ITEM_TYPE_SIG),
    TxToken: ItemMeta(CfTxToken, ItemTypes.TX_ITEM_TYPE_IN_EMS),
    TxReceipt: ItemMeta(CfTxReceipt, ItemTypes.TX_ITEM_TYPE_RECEIPT),
    TxReceiptOld: ItemMeta(CfTxReceiptOld, ItemTypes.TX_ITEM_TYPE_RECEIPT_OLD),
    TxOutExt: ItemMeta(CfTxOutExt, ItemTypes.TX_ITEM_TYPE_OUT_EXT),
    TxTSD: ItemMeta(CfTxTSD, ItemTypes.TX_ITEM_TYPE_TSD),
    TxOutCondSubtypeSrvPay: ItemMeta(
        CfTxOutCondSubtypeSrvPay, 
        ItemTypes.DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY
    ),
    TxOutCondSubtypeSrvXchange: ItemMeta(
        CfTxOutCondSubtypeSrvXchange,
        ItemTypes.DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE
    ),
    TxOutCondSubtypeSrvStakeLock: ItemMeta(
        CfTxOutCondSubtypeSrvStakeLock,
        ItemTypes.DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_LOCK
    ),
    TxOutCondSubtypeSrvStakePosDelegate: ItemMeta(
        CfTxOutCondSubtypeSrvStakePosDelegate,
        ItemTypes.DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE_POS_DELEGATE
    ),
    TxVoting: ItemMeta(CfTxVoting, ItemTypes.TX_ITEM_TYPE_VOTING),
    TxVote: ItemMeta(CfTxVote, ItemTypes.TX_ITEM_TYPE_VOTE),
    TxOutStd: ItemMeta(CfTxOutStd, ItemTypes.TX_ITEM_TYPE_OUT_STD),
}


class CfItemMapper:
    """Maps item types to corresponding CellFrame item wrapper classes."""

    @staticmethod
    def build(origin_item, chain) -> CfItem:
        """Build a CellFrame item based on the provided origin item.

        Args:
            origin_item: The original item.
            net: The CfNet instance representing the network context.

        Returns:
            CfItem: The CellFrame item.
        """
        mapping = ITEM_WRAPPER_MAPPING.get(type(origin_item))

        return mapping.cf_wrapper_cls(origin_item, mapping.type, chain)


SubDatumMeta = namedtuple('SubDatumMapping', ['origin_method_name', 'cf_wrapper_cls'])

SUB_DATUM_WRAPPER_MAPPING = {
    DatumTypes.DATUM_TX: SubDatumMeta("getDatumTX", CfDatumTX),
    DatumTypes.DATUM_TOKEN_DECL: SubDatumMeta("getDatumToken", CfDatumToken),
    DatumTypes.DATUM_TOKEN: SubDatumMeta("getDatumToken", CfDatumToken),
    DatumTypes.DATUM_TOKEN_EMISSION: SubDatumMeta("getDatumTokenEmission", 
                                                  CfDatumEmission),
    DatumTypes.DATUM_CUSTOM: SubDatumMeta("getDatumCustom", CfDatumCustom),
    DatumTypes.DATUM_DECREE: SubDatumMeta("getDatumDecree", CfDatumDecree),
    DatumTypes.DATUM_ANCHOR: SubDatumMeta("getDatumAnchor", CfDatumAnchor),
}


class CfSubDatumBuilder:
    """Build sub-datums for CellFrame based on datum type."""

    def __init__(self, type):
        """
        Initialize a new CfSubDatumBuilder instance.

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
            CfDatum: The sub-datum.
        """
        method = getattr(
            parent_datum._origin_datum, 
            SUB_DATUM_WRAPPER_MAPPING[self.type].origin_method_name
        )
        sub_datum = method()
        return SUB_DATUM_WRAPPER_MAPPING[self.type].cf_wrapper_cls(
            parent_datum, sub_datum
        )
