from pydantic import Field
from .._common import SerializeModel


class Pkey(SerializeModel):
    type: str
    size: int


class Sign(SerializeModel):
    type: str
    pkey: Pkey
    pkey_hash: str
    size: int
    address: str

