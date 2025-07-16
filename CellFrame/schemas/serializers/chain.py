from schemas._common import SerializeModel
from .net import Net


class Chain(SerializeModel):
    net: Net
    name: str
    type: str
