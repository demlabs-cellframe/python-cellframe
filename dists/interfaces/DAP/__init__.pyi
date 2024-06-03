from typing import Protocol


# PyModule_AddObject(dapModule, "error", PyErr_NewException("DAP.error", NULL, NULL));

def configGetItem(section_path: str, item_name: str) -> str:
    """Get an item from a configuration"""
    pass


def configGetItemDefault(section_path: str, item_name: str, default_value: str) -> str:
    """Get an item from a configuration with a default value"""
    pass
