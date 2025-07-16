"""
🔗 Cellframe Chain Module

Comprehensive blockchain interaction framework providing unified interfaces 
for wallet management, transaction composition, and ledger operations.

Usage:
    from cellframe.chain import Wallet
    from cellframe.composer import Composer
    
    # Open wallet and create transaction
    wallet = Wallet.open("/path/to/wallet", password="your_password")
    with Composer(net_name="mainnet", wallet=wallet) as composer:
        tx = composer.create_tx(dest_addr, amount, "CELL", fee)
"""

from .wallet import (
    Wallet,
    WalletAddress,
    WalletAccessType,
    WalletType,
    WalletError,
    InvalidAddressError,
    InsufficientFundsError,
    WalletManager,
    create_wallet,
    open_wallet,
    get_wallet,
    close_wallet,
    get_all_wallets,
    close_all_wallets
)

from .ledger import (
    DapLedger,
    DapLedgerType,
    DapLedgerError,
    DapTokenInfo,
    DapAccount,
    DapLedgerManager,
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
    # Core wallet functionality
    'Wallet',
    'WalletAddress', 
    'WalletAccessType',
    'WalletType',
    'WalletError',
    'InvalidAddressError',
    'InsufficientFundsError',
    'WalletManager',
    'create_wallet',
    'open_wallet',
    'get_wallet',
    'close_wallet',
    'get_all_wallets',
    'close_all_wallets',
    
    # Ledger operations
    'DapLedger',
    'DapLedgerType',
    'DapLedgerError',
    'DapTokenInfo',
    'DapAccount',
    'DapLedgerManager',
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