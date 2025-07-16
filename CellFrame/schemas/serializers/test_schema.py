from .._common import SerializeModel


class TestValue(SerializeModel):
    value: int


class TestSchema(SerializeModel):
    id: int
    name: str
    values: list[TestValue]
