"""
💰 Cellframe Wallet Module

Secure wallet operations:
- Wallet: HD wallet implementation
- WalletManager: Multi-wallet management
- Transaction signing and sending
"""

# Import from common (previously core)
from ..common.exceptions import WalletException

# Import wallet class if available
try:
    from .wallet import Wallet
    __all__ = ['WalletException', 'Wallet']
except ImportError:
    __all__ = ['WalletException'] 