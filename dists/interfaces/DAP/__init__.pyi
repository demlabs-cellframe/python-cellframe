from typing import Protocol, Any


# PyModule_AddObject(dapModule, "error", PyErr_NewException("DAP.error", NULL, NULL));

def configGetItem(section_path: str, item_name: str, default_value : Any = None) -> str | bool | int | list[str]:
    """Get an item from a configuration"""
    pass

