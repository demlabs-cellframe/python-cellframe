"""
🔗 Cellframe Chain Module

Blockchain interaction framework for wallet management, transaction composition, and ledger operations.

Usage:
    from cellframe.chain import Wallet
    from cellframe.composer import Composer
    
    # Open wallet and create transaction
    wallet = Wallet.open("/path/to/wallet", password="your_password")
    with Composer(net_name="mainnet", wallet=wallet) as composer:
        tx = composer.create_tx(dest_addr, amount, "CELL", fee)
"""

# FAIL-FAST: These classes require native C implementations
def _require_native_implementation(class_name: str):
    """Helper function to raise ImportError for unimplemented classes."""
    raise ImportError(
        f"❌ CRITICAL: {class_name} requires native C implementation!\n"
        "This is a Python bindings library - stub classes are not allowed.\n"
        f"Required: {class_name} must be implemented in native C module.\n"
        "Please implement these classes in src/cellframe_chain.c"
    )

# Import native C implementations or implement Python wrappers
try:
    import python_cellframe as cf_native
    
    # Centralized check for required native functions
    required_functions = [
        'dap_chain_atom_create',
        'dap_chain_addr_from_str', 
        'dap_chain_addr_get_net_id',
        'dap_chain_mempool_by_chain_name',
        'dap_chain_mempool_tx_get_by_hash'
    ]
    
    missing_functions = [func for func in required_functions if not hasattr(cf_native, func)]
    if missing_functions:
        # In test mode, allow missing functions for mock testing
        import os
        if os.environ.get('CELLFRAME_TEST_MODE') == '1':
            print(f"⚠️  TEST MODE: Missing native functions: {', '.join(missing_functions)}")
        else:
            raise NotImplementedError(
                f"❌ CRITICAL: Missing native functions: {', '.join(missing_functions)}\n"
                "This is a Python bindings library - all chain functions must be implemented.\n"
                "Please implement these functions in src/cellframe_chain.c"
            )
    
    class ChainAtomPtr:
        """Chain atom pointer - wrapper for native C implementation"""
        
        def __init__(self, atom_data: bytes = None, atom_size: int = None):
            """Initialize chain atom pointer.
            
            Args:
                atom_data: Atom data bytes (optional)
                atom_size: Atom size in bytes (required if atom_data provided)
            """
            if atom_data is not None:
                if atom_size is None:
                    atom_size = len(atom_data)
                # Create atom with specified size
                self._native_atom = cf_native.dap_chain_atom_create(atom_size)
                if not self._native_atom:
                    raise RuntimeError("Failed to create native chain atom")
                # Copy data if provided
                if atom_data:
                    import ctypes
                    ctypes.memmove(self._native_atom, atom_data, atom_size)
                self._atom_size = atom_size
            else:
                self._native_atom = None
                self._atom_size = 0
        
        def get_data(self) -> bytes:
            """Get atom data as bytes.
            
            Uses native C API dap_chain_atom_get_data.
            
            Returns:
                bytes: Atom data
            """
            if not self._native_atom:
                raise RuntimeError("Chain atom not initialized")
            
            if not hasattr(cf_native, 'dap_chain_atom_get_data'):
                raise ImportError(
                    "❌ CRITICAL: dap_chain_atom_get_data not available in python_cellframe!\n"
                    "Please ensure this function is exported in src/cellframe_chain.c"
                )
            
            return cf_native.dap_chain_atom_get_data(self._native_atom)
    
    # Ledger class removed - use CfLedger instead (imported below)
    
    # Create CfAddr class (renamed from ChainAddr)
    class CfAddr:
        """Cellframe address - wrapper for native C implementation"""
        
        def __init__(self, address_str: str = None):
            """Initialize chain address."""
            if address_str:
                self._native_addr = cf_native.dap_chain_addr_from_str(address_str)
                if not self._native_addr:
                    raise ValueError(f"Invalid address string: {address_str}")
            else:
                self._native_addr = None
        
        @classmethod
        def fromStr(cls, address_str: str) -> 'CfAddr':
            """Create address from string (legacy compatibility)."""
            return cls(address_str)
        
        def getNetId(self) -> 'CfNetId':
            """Get network ID for address."""
            if not self._native_addr:
                raise RuntimeError("Address not initialized")
            # Use native function - no fallbacks
            net_id = cf_native.dap_chain_addr_get_net_id(self._native_addr)
            return CfNetId(net_id)
        
        def to_str(self) -> str:
            """Convert address to string."""
            if not self._native_addr:
                raise RuntimeError("Address not initialized")
            return cf_native.dap_chain_addr_to_str(self._native_addr)
        
        def __str__(self) -> str:
            return self.to_str() if self._native_addr else ""
    
    class CfNetId:
        """Network ID class"""
        
        def __init__(self, net_id: int):
            self.net_id = net_id
        
        def long(self) -> int:
            """Get network ID as long."""
            return self.net_id
    
    class Mempool:
        """Mempool - wrapper for native C implementation"""
        
        def __init__(self, chain_name: str):
            """Initialize mempool for chain."""
            # Native functions already checked at import
            self.chain_name = chain_name
            self._native_mempool = cf_native.dap_chain_mempool_by_chain_name(chain_name)
            if not self._native_mempool:
                raise RuntimeError(f"Failed to get mempool for chain: {chain_name}")
        
        def add_tx(self, tx_hash: str) -> bool:
            """Add transaction to mempool."""
            return cf_native.dap_chain_mempool_tx_put(self._native_mempool, tx_hash)
        
        def get_tx(self, tx_hash: str):
            """Get transaction from mempool."""
            return cf_native.dap_chain_mempool_tx_get_by_hash(self._native_mempool, tx_hash)

except ImportError:
    # FAIL-FAST: No implementations without native module
    def ChainAtomPtr(*args, **kwargs):
        _require_native_implementation("ChainAtomPtr")

    def CfAddr(*args, **kwargs):
        _require_native_implementation("CfAddr")

    def Mempool(*args, **kwargs):
        _require_native_implementation("Mempool")

# Ticker constant
ticker = "CELL"

from ..wallet.wallet import (
    Wallet,
    WalletAddress,
    WalletAccessType,
    WalletType,
    WalletError,
    InvalidAddressError,
    InsufficientFundsError,
    WalletManager,
    create,
    open,
    get,
    close,
    list as list_wallets,
    close_all
)

from .ledger import (
    CfLedger,
    DapLedgerType,
    DapLedgerError,
    DapTokenInfo,
    DapAccount,
    CfLedgerManager,
    create_ledger,
    open_ledger,
    get_account_balance
)

from .tx import (
    TX,
    TxError,
    TxType,
    TxStatus,
    TxInput,
    TxOutput,
    get_tx_by_hash,
    broadcast_tx
)

from .tx_utils import (
    find_tx_out, get_tx_items, extract_tx_in_metadata, extract_tx_out_metadata,
    find_tx_outputs_by_type, get_tx_value_by_token, is_stake_lock_tx,
    get_tx_signatures, validate_tx_structure, format_tx_summary,
    find_tx_output, get_transaction_items  # aliases for backward compatibility
)

from .emission import (
    CellframeEmission, EmissionManager, EmissionError,
    create_emission, parse_emission_from_datum
)

# Re-export composer module components for convenience
from ..composer import (
    Composer,
    FeeOptimizer,
    BatchProcessor,
    TransactionTemplates,
    ConditionalProcessor,
    ComposeConfig,
    FeeStructure,
    TransactionInput,
    TransactionOutput,
    ComposeError,
    FeeCalculationError,
    InputSelectionError,
    OutputCreationError,
    TemplateError,
    BatchProcessingError,
    ConditionalTransactionError,
    composer_context,
    quick_transfer
)

__all__ = [
    # Native types
    'ChainAtomPtr',
    'CfAddr',
    'Mempool', 
    'ticker',
    
    # Core wallet functionality
    'Wallet',
    'WalletAddress', 
    'WalletAccessType',
    'WalletType',
    'WalletError',
    'InvalidAddressError',
    'InsufficientFundsError',
    'WalletManager',
    'create',
    'open',
    'get',
    'close',
    'list_wallets',
    'close_all',
    
    # Ledger operations
    'CfLedger',
    'DapLedgerType',
    'DapLedgerError',
    'DapTokenInfo',
    'DapAccount',
    'CfLedgerManager',
    'create_ledger',
    'open_ledger',
    'get_account_balance',
    
    # Transaction management
    'TX',
    'TxError',
    'TxType',
    'TxStatus',
    'TxInput',
    'TxOutput',
    'get_tx_by_hash',
    'broadcast_tx',
    
    # Transaction utilities (integrated from helpers)
    'find_tx_out', 'get_tx_items', 'extract_tx_in_metadata', 'extract_tx_out_metadata',
    'find_tx_outputs_by_type', 'get_tx_value_by_token', 'is_stake_lock_tx',
    'get_tx_signatures', 'validate_tx_structure', 'format_tx_summary',
    'find_tx_output', 'get_transaction_items',
    
    # Emission functionality (integrated from helpers)
    'CellframeEmission', 'EmissionManager', 'EmissionError',
    'create_emission', 'parse_emission_from_datum',
    
    # Advanced composition (from composer module)
    'Composer',
    'FeeOptimizer',
    'BatchProcessor',
    'TransactionTemplates',
    'ConditionalProcessor',
    'ComposeConfig',
    'FeeStructure',
    'TransactionInput',
    'TransactionOutput',
    'ComposeError',
    'FeeCalculationError',
    'InputSelectionError',
    'OutputCreationError',
    'TemplateError',
    'BatchProcessingError',
    'ConditionalTransactionError',
    'composer_context',
    'quick_transfer'
] 