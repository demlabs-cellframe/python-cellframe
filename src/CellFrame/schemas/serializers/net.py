from .._common import SerializeModel


class Net(SerializeModel):
    id: int
    name: str
    address: str
    group_alias: str
