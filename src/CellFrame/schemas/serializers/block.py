from datetime import datetime

from pydantic import field_validator

from .._common import date_format, SerializeModel
from .datums import Datum
from .chain import Chain
from .sign import Sign


class Block(SerializeModel):
    chain: Chain
    hash: str
    created_at: str
    version: int
    cell_id: str
    signs: list[Sign]
    meta: dict

    @field_validator("created_at", mode="before")
    def date_format(cls, date: datetime):
        return date_format(date)
