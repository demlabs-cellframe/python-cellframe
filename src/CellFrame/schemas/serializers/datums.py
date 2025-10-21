from datetime import datetime
from typing import Annotated, Union
from pydantic import BaseModel, ConfigDict, field_validator, Field
from .sign import Sign
from .._common import SerializeModel


class Item(SerializeModel):
    type: str


class TxIn(Item):
    prev_hash: str
    prev_idx: int


class TxOut(Item):
    address: str
    value: str


# TX_ITEM_TYPE_OUT_COND
class TxOutCond(Item):
    expires: datetime
    value: str
    subtype: str


class TxSig(Item):
    size: int
    sign: Sign


class TxToken(Item):
    ticker: str
    emission_hash: str
    emission_chain_id: str


class TxOutCondSubTypeSrvStakeLock(Item):
    unlock: datetime
    value: str
    reinvest_percent: int


class TxOutExt(Item):
    address: str
    ticker: str
    value: str


class SubDatum(SerializeModel):
    pass


class DatumTX(SubDatum):
    hash: str
    created_at: datetime
    ticker: str | None
    items: list[TxIn | TxOutExt | TxOut | TxOutCond | TxSig | TxToken | TxOutCondSubTypeSrvStakeLock | Item]

    @classmethod
    def model_validate(cls, *args, **kwargs):
        obj = args[0]
        obj.items = obj.get_items()
        return super().model_validate(*args, **kwargs)


class DatumToken(SubDatum):
    ticker: str
    type: str
    data: dict
    signs: list[Sign]


class DatumEmission(SubDatum):
    hash: str
    version: int
    type: str
    ticker: str
    address: str
    value: str
    data: dict | list[Sign]
    signs: list[Sign]
    tsd: dict
    valid_sign_hashes: int = Field(validation_alias="valid_sign_hashes", serialization_alias="valid_sign_hashes_count")

    @field_validator("valid_sign_hashes", mode="before")
    def date_format(cls, date: list[str]):
        return len(date)


class DatumDecree(SubDatum):
    hash: str
    created_at: str
    type: str
    subtype: str
    signs: list[Sign]


class DatumAnchor(SubDatum):
    hash: str
    created_at: str
    signs: list[Sign]


class DatumCustom(SubDatum):
    data: str


class DatumCorrupted(SubDatum):
    type: str
    hash: str


class Datum(SerializeModel):
    hash: str
    type: str
    version: str
    size: int
    created_at: datetime
    sub_datum: DatumTX | DatumEmission | SubDatum

    # sub_datum: DatumTX | DatumEmission | DatumDecree | DatumToken | DatumAnchor | DatumCustom | DatumCorrupted | SubDatum

    @classmethod
    def model_validate(cls, *args, **kwargs):
        obj = args[0]
        obj.sub_datum = obj.get_sub_datum()
        return super().model_validate(*args, **kwargs)

    # @field_validator("sub_datum", mode="before")
    # def validate_sub_dutum(cls, sub_datum: SubDatum):
    #     schemas_map: dict[str, type[BaseModel]] = {
    #         #     DatumTypes.DATUM_TX: schemas.datums.DatumTX,
    #         #     DatumTypes.DATUM_TOKEN_DECL: schemas.datums.DatumToken,
    #         #     DatumTypes.DATUM_TOKEN_EMISSION: schemas.datums.DatumEmission,
    #         #     DatumTypes.DATUM_ANCHOR: schemas.datums.DatumAnchor,
    #         #     DatumTypes.DATUM_DECREE: schemas.datums.DatumDecree,
    #         #     DatumTypes.DATUM_CUSTOM: schemas.datums.DatumCustom,
    #         # }
