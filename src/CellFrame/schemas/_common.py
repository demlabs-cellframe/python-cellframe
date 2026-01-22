from datetime import datetime
from pydantic import BaseModel, ConfigDict


# object --> dict | json
class SerializeModel(BaseModel):
    model_config = ConfigDict(from_attributes=True)


def date_format(date: datetime) -> str:
    return str(int(date.timestamp()))
