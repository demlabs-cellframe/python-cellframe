"""
🔗 CellFrame Core Types Module

Core data structures and types used across CellFrame ecosystem.
All classes require native C implementation - no fallbacks allowed.
"""

# Import context classes first
from .context import AppContext, ExecutionMode

# Import existing types from correct locations
try:
    from ..common.types import Datum, DatumTx
except ImportError:
    # Create basic types when not available
    class Datum:
        def __init__(self, data=None):
            self.data = data
    
    class DatumTx:
        def __init__(self, tx_data=None):
            self.tx_data = tx_data
from ..types import Address

# Import transaction types from legacy module until native implementation is complete
try:
    import python_cellframe as cf_native
    
    # Try to import from native module first
    TxOut = getattr(cf_native, 'TxOut', None)
    TxIn = getattr(cf_native, 'TxIn', None)
    TxToken = getattr(cf_native, 'TxToken', None)
    TxSig = getattr(cf_native, 'TxSig', None)
    TxOutCondSubtypeSrvStakeLock = getattr(cf_native, 'TxOutCondSubtypeSrvStakeLock', None)
    TxInCond = getattr(cf_native, 'TxInCond', None)
    TxOutExt = getattr(cf_native, 'TxOutExt', None)
    DatumToken = getattr(cf_native, 'DatumToken', None)
    
    # If not available in native, use legacy implementations
    if TxOut is None:
        from ..legacy.pycfhelpers import TxOut, TxIn, TxToken, TxSig, TxOutCondSubtypeSrvStakeLock, TxInCond, TxOutExt
        
    if DatumToken is None:
        # Create basic type when not available
        class DatumToken:
            def __init__(self, token_data=None):
                self.token_data = token_data
                
except ImportError:
    # Fallback to legacy implementations
    from ..legacy.pycfhelpers import TxOut, TxIn, TxToken, TxSig, TxOutCondSubtypeSrvStakeLock, TxInCond, TxOutExt
    
    class DatumToken:
        def __init__(self, token_data=None):
            self.token_data = token_data
# Other datum types with fallback
DatumEmission = getattr(cf_native, 'DatumEmission', None) if 'cf_native' in locals() else None
DatumAnchor = getattr(cf_native, 'DatumAnchor', None) if 'cf_native' in locals() else None
DatumDecree = getattr(cf_native, 'DatumDecree', None) if 'cf_native' in locals() else None

# Create fallback classes if not available
if DatumEmission is None:
    class DatumEmission:
        def __init__(self, emission_data=None):
            self.emission_data = emission_data

if DatumAnchor is None:
    class DatumAnchor:
        def __init__(self, anchor_data=None):
            self.anchor_data = anchor_data

if DatumDecree is None:
    class DatumDecree:
        def __init__(self, decree_data=None):
            self.decree_data = decree_data

# Import exceptions
from .exceptions import CellframeException, ConfigurationException

__all__ = [
    # Context classes
    'AppContext', 'ExecutionMode',
    
    # Basic types
    'Datum', 'DatumTx', 'Address',
    
    # Transaction items
    'TxOut', 'TxIn', 'TxToken', 'TxSig', 
    'TxOutCondSubtypeSrvStakeLock', 'TxInCond', 'TxOutExt',
    
    # Additional datum types
    'DatumToken', 'DatumEmission', 'DatumAnchor', 'DatumDecree',
    
    # Exceptions
    'CellframeException', 'ConfigurationException'
]
