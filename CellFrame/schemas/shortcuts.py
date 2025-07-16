import schemas
from dataclasses import dataclass
from pydantic import BaseModel

try:
    from pycfhelpers.node.net import CFNet, CFChain
    from pycfhelpers.node.consensus import CFBlock, CFEvent
    from pycfhelpers.node.datums import CFDatum
    from pycfhelpers.node.logging import CFLog
except ImportError:
    import sys

    print("Warning: shortcuts available only in cellframe-node environment")
    sys.exit(1)

DEFAULT_SERIALIZERS = {
    CFNet: schemas.Net,
    CFChain: schemas.Chain,
    CFBlock: schemas.Block,
    CFEvent: schemas.Event,
    CFDatum: schemas.Datum,
}

log = CFLog()


def dump(obj, schema: BaseModel = None) -> dict:
    if schema is None:
        try:
            schema = DEFAULT_SERIALIZERS[type(obj)]
        except KeyError:
            log.error(f"Default schema not found for object type={type(obj)}")

    return schema.model_validate(obj).model_dump()
