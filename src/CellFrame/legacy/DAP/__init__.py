"""Legacy DAP module"""
from CellFrame.legacy.pycfhelpers import DAP as _DAP
# Re-export submodules
Crypto = _DAP.Crypto
Core = _DAP.Core
Network = _DAP.Network
GlobalDB = _DAP.GlobalDB

# Legacy functions
def configGetItem(section, key, default=None):
    """Legacy config get item"""
    # Return mock config value
    return default if default is not None else ""

def configGetSysDir(dir_type):
    """Legacy config get system directory"""
    # Return mock directories
    dirs = {
        "data": "/var/lib/cellframe",
        "config": "/etc/cellframe",
        "log": "/var/log/cellframe",
        "temp": "/tmp/cellframe"
    }
    return dirs.get(dir_type, "/tmp")