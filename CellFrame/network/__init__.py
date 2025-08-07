"""
🌐 Cellframe Network Module

Async network operations:
- NetworkClient: High-level network interface
- PeerManager: Peer discovery and management
- ConnectionPool: Connection management
"""

# Will be implemented in Phase 3
from ..core.exceptions import NetworkException

# Stub class for Net - will be replaced by native implementation
class Net:
    """Stub class for network - to be implemented in native module"""
    pass

# Stub class for NetID
class NetID:
    """Stub class for network ID - to be implemented in native module"""
    def __init__(self, id=0):
        self.id = id

__all__ = ['NetworkException', 'Net', 'NetID'] 