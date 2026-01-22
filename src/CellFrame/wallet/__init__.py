"""
💰 Cellframe Wallet Module

Secure wallet operations:
- Wallet: HD wallet implementation
- WalletManager: Multi-wallet management
- Transaction signing and sending
"""

# Import from common (previously core)
from ..common.exceptions import WalletException

# Import wallet classes if available
try:
    from .wallet import (
        Wallet, 
        WalletManager, 
        WalletError, 
        WalletAddress,
        WalletAccessType,
        WalletType
    )
    __all__ = [
        'WalletException', 
        'Wallet', 
        'WalletManager', 
        'WalletError', 
        'WalletAddress',
        'WalletAccessType',
        'WalletType'
    ]
except ImportError as e:
    # Fallback if wallet module not available
    __all__ = ['WalletException'] 