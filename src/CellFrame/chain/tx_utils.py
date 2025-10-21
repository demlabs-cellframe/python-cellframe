"""
🔧 CellFrame Transaction Utilities

Transaction processing utilities.
Integrated from helpers with enhancements.
"""

import logging
from typing import Any, Optional, List, Dict, Iterator, Union

from ..core.exceptions import TransactionException
from ..common import logger
from ..legacy.pycfhelpers import TxOutCondSubtypeSrvStakeLock

# Try to import native CellFrame transaction components
try:
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
    # TxOut, TxIn, TxToken, TxSig types are defined in native C module
    import python_cellframe as cf_native
except ImportError as e:
    # FAIL-FAST: No dummy classes allowed
    raise ImportError(
        "❌ CRITICAL: Native CellFrame types not available!\n"
        "This is a Python bindings library - dummy classes are not allowed.\n"
        f"Original error: {e}\n"
        "Please ensure python_cellframe native module is properly built and installed."
    ) from e


def find_tx_out(tx: Any, out_type: Any) -> Optional[Any]:
    """
    Find a transaction output of the specified type in a transaction.
    
    Enhanced version of the original helpers function.
    
    Args:
        tx: The transaction to search
        out_type: The type of transaction output to find
        
    Returns:
        Transaction output of the specified type, or None if not found
    """
    try:
        if not tx or not hasattr(tx, 'getOutByType'):
            logger.warning("Invalid transaction object")
            return None
        
        tx_out = tx.getOutByType(out_type)
        if tx_out:
            logger.debug(f"Found transaction output of type {out_type}")
            return tx_out
        else:
            logger.debug(f"No transaction output of type {out_type} found")
            return None
            
    except Exception as e:
        logger.error(f"Error finding transaction output: {e}")
        return None


def get_tx_items(tx: Any) -> List[Dict[str, Any]]:
    """
    Get all items (inputs and outputs) from a transaction.
    
    Enhanced version of the original helpers function.
    
    Args:
        tx: The transaction to process
        
    Returns:
        List of transaction items with metadata
    """
    items = []
    
    try:
        if not tx:
            return items
        
        # Get transaction inputs
        if hasattr(tx, 'getInCount'):
            in_count = tx.getInCount()
            for i in range(in_count):
                try:
                    tx_in = tx.getIn(i)
                    if tx_in:
                        item = {
                            'type': 'input',
                            'index': i,
                            'data': tx_in,
                            'metadata': extract_tx_in_metadata(tx_in)
                        }
                        items.append(item)
                except Exception as e:
                    logger.error(f"Error processing input {i}: {e}")
        
        # Get transaction outputs
        if hasattr(tx, 'getOutCount'):
            out_count = tx.getOutCount()
            for i in range(out_count):
                try:
                    tx_out = tx.getOut(i)
                    if tx_out:
                        item = {
                            'type': 'output',
                            'index': i,
                            'data': tx_out,
                            'metadata': extract_tx_out_metadata(tx_out)
                        }
                        items.append(item)
                except Exception as e:
                    logger.error(f"Error processing output {i}: {e}")
        
        logger.debug(f"Extracted {len(items)} items from transaction")
        return items
        
    except Exception as e:
        logger.error(f"Error getting transaction items: {e}")
        return []


def extract_tx_in_metadata(tx_in: Any) -> Dict[str, Any]:
    """
    Extract metadata from transaction input.
    
    Args:
        tx_in: Transaction input
        
    Returns:
        Metadata dictionary
    """
    metadata = {}
    
    try:
        if hasattr(tx_in, 'getHash'):
            metadata['hash'] = str(tx_in.getHash())
        
        if hasattr(tx_in, 'getIdx'):
            metadata['idx'] = tx_in.getIdx()
        
        if hasattr(tx_in, 'getSig'):
            sig = tx_in.getSig()
            if sig:
                metadata['signature'] = str(sig)
        
        logger.debug(f"Extracted input metadata: {metadata}")
        
    except Exception as e:
        logger.error(f"Error extracting input metadata: {e}")
    
    return metadata


def extract_tx_out_metadata(tx_out: Any) -> Dict[str, Any]:
    """
    Extract metadata from transaction output.
    
    Args:
        tx_out: Transaction output
        
    Returns:
        Metadata dictionary
    """
    metadata = {}
    
    try:
        if hasattr(tx_out, 'getValue'):
            metadata['value'] = tx_out.getValue()
        
        if hasattr(tx_out, 'getAddr'):
            addr = tx_out.getAddr()
            if addr:
                metadata['address'] = str(addr)
        
        if hasattr(tx_out, 'getType'):
            metadata['type'] = tx_out.getType()
        
        if hasattr(tx_out, 'getTokenTicker'):
            metadata['token'] = tx_out.getTokenTicker()
        
        logger.debug(f"Extracted output metadata: {metadata}")
        
    except Exception as e:
        logger.error(f"Error extracting output metadata: {e}")
    
    return metadata


def find_tx_outputs_by_type(tx: Any, output_types: List[Any]) -> List[Any]:
    """
    Find all transaction outputs of specified types.
    
    Args:
        tx: The transaction to search
        output_types: List of output types to find
        
    Returns:
        List of matching transaction outputs
    """
    outputs = []
    
    for out_type in output_types:
        tx_out = find_tx_out(tx, out_type)
        if tx_out:
            outputs.append(tx_out)
    
    return outputs


def get_tx_value_by_token(tx: Any, token_ticker: str) -> int:
    """
    Get total value of specific token in transaction.
    
    Args:
        tx: The transaction to analyze
        token_ticker: Token ticker symbol
        
    Returns:
        Total token value
    """
    total_value = 0
    
    try:
        items = get_tx_items(tx)
        
        for item in items:
            if item['type'] == 'output':
                metadata = item.get('metadata', {})
                if metadata.get('token') == token_ticker:
                    value = metadata.get('value', 0)
                    if isinstance(value, (int, float)):
                        total_value += int(value)
        
        logger.debug(f"Total {token_ticker} value in transaction: {total_value}")
        
    except Exception as e:
        logger.error(f"Error calculating token value: {e}")
    
    return total_value


def is_stake_lock_tx(tx: Any) -> bool:
    """
    Check if transaction contains stake lock operations.
    
    Args:
        tx: The transaction to check
        
    Returns:
        True if transaction contains stake lock
    """
    try:
        stake_out = find_tx_out(tx, TxOutCondSubtypeSrvStakeLock)
        return stake_out is not None
        
    except Exception as e:
        logger.error(f"Error checking stake lock: {e}")
        return False


def get_tx_signatures(tx: Any) -> List[str]:
    """
    Get all signatures from transaction.
    
    Args:
        tx: The transaction to process
        
    Returns:
        List of signature strings
    """
    signatures = []
    
    try:
        items = get_tx_items(tx)
        
        for item in items:
            if item['type'] == 'input':
                metadata = item.get('metadata', {})
                sig = metadata.get('signature')
                if sig:
                    signatures.append(sig)
        
        logger.debug(f"Found {len(signatures)} signatures in transaction")
        
    except Exception as e:
        logger.error(f"Error getting signatures: {e}")
    
    return signatures


def validate_tx_structure(tx: Any) -> bool:
    """
    Validate basic transaction structure.
    
    Args:
        tx: The transaction to validate
        
    Returns:
        True if structure is valid
    """
    try:
        if not tx:
            return False
        
        # Check if transaction has required methods
        required_methods = ['getInCount', 'getOutCount']
        for method in required_methods:
            if not hasattr(tx, method):
                logger.error(f"Transaction missing required method: {method}")
                return False
        
        # Check input/output counts
        in_count = tx.getInCount() if hasattr(tx, 'getInCount') else 0
        out_count = tx.getOutCount() if hasattr(tx, 'getOutCount') else 0
        
        if in_count == 0 and out_count == 0:
            logger.error("Transaction has no inputs or outputs")
            return False
        
        logger.debug(f"Transaction structure valid: {in_count} inputs, {out_count} outputs")
        return True
        
    except Exception as e:
        logger.error(f"Error validating transaction structure: {e}")
        return False


def format_tx_summary(tx: Any) -> Dict[str, Any]:
    """
    Create a summary of transaction information.
    
    Args:
        tx: The transaction to summarize
        
    Returns:
        Transaction summary dictionary
    """
    summary = {
        'valid': False,
        'inputs': 0,
        'outputs': 0,
        'items': [],
        'signatures': [],
        'stake_lock': False,
        'tokens': {}
    }
    
    try:
        # Validate structure
        summary['valid'] = validate_tx_structure(tx)
        
        if not summary['valid']:
            return summary
        
        # Count inputs and outputs
        if hasattr(tx, 'getInCount'):
            summary['inputs'] = tx.getInCount()
        
        if hasattr(tx, 'getOutCount'):
            summary['outputs'] = tx.getOutCount()
        
        # Get items
        summary['items'] = get_tx_items(tx)
        
        # Get signatures
        summary['signatures'] = get_tx_signatures(tx)
        
        # Check stake lock
        summary['stake_lock'] = is_stake_lock_tx(tx)
        
        # Calculate token totals
        for item in summary['items']:
            if item['type'] == 'output':
                metadata = item.get('metadata', {})
                token = metadata.get('token', 'unknown')
                value = metadata.get('value', 0)
                
                if token in summary['tokens']:
                    summary['tokens'][token] += value
                else:
                    summary['tokens'][token] = value
        
        logger.debug(f"Created transaction summary: {summary['inputs']} inputs, {summary['outputs']} outputs")
        
    except Exception as e:
        logger.error(f"Error creating transaction summary: {e}")
    
    return summary


# Convenience functions for backward compatibility
def find_tx_output(tx: Any, out_type: Any) -> Optional[Any]:
    """Alias for find_tx_out."""
    return find_tx_out(tx, out_type)


def get_transaction_items(tx: Any) -> List[Dict[str, Any]]:
    """Alias for get_tx_items."""
    return get_tx_items(tx) 