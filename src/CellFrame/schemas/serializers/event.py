from datetime import datetime
from pydantic import Field, field_validator
from .._common import SerializeModel, date_format
from .sign import Sign
from .chain import Chain


class Event(SerializeModel):
    hash: str
    chain: Chain
    created_at: str
    version: int
    cell_id: str
    signs: list[Sign]
    hashes: list[str]

    @field_validator("created_at", mode="before")
    def date_format(cls, date: datetime):
        return date_format(date)
