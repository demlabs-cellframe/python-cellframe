"""
🌐 Cellframe Network Module

Network operations and client interfaces.
All network functionality requires native C implementation.
"""

from ..core.exceptions import NetworkException

# Import network functionality from dedicated module
from .net import *

__all__ = ['NetworkException'] 