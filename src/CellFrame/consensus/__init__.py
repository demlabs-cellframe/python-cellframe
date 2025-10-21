"""
🔗 CellFrame Consensus Module

Consensus mechanisms and blockchain structures.
"""

# Native C bindings integration
try:
    import python_cellframe as cf_native
    
    # Centralized check for all required consensus functions - ONCE at import
    required_functions = [
        'dap_chain_cs_dag_new', 
        'dap_chain_cs_dag_add_node', 
        'dap_chain_cs_dag_get_tips',
        'dap_chain_block_new', 
        'dap_chain_block_add_tx', 
        'dap_chain_block_get_tx_count', 
        'dap_chain_block_validate'
    ]
    
    missing_functions = [func for func in required_functions if not hasattr(cf_native, func)]
    if missing_functions:
        raise ImportError(
            f"❌ CRITICAL: Missing native consensus functions: {', '.join(missing_functions)}\n"
            "This is a Python bindings library - all consensus functions must be implemented.\n"
            "Please implement these functions in src/cellframe_consensus.c"
        )
except ImportError as e:
    raise ImportError(
        "❌ CRITICAL: Native CellFrame consensus module not available!\n"
        "This is a Python bindings library - native C extension is required.\n"
        f"Original error: {e}\n"
        "Please ensure python_cellframe native module is properly built and installed."
    ) from e

class CfDAG:
    """DAG consensus - requires native C implementation"""
    
    def __init__(self, chain_name: str = None):
        """Initialize DAG consensus for specified chain."""
        # Native availability already checked at import
        
        self.chain_name = chain_name
        self._native_dag = cf_native.dap_chain_cs_dag_new(chain_name)
        if not self._native_dag:
            raise RuntimeError(f"Failed to create native DAG for chain: {chain_name}")
        
    def add_node(self, node_id: str, data: dict = None) -> bool:
        """Add node to DAG using native implementation."""
        return cf_native.dap_chain_cs_dag_add_node(self._native_dag, node_id, data)
        
    def get_tips(self) -> list:
        """Get DAG tip nodes using native implementation."""
        return cf_native.dap_chain_cs_dag_get_tips(self._native_dag)

class CfBlock:
    """Blockchain block - requires native C implementation"""
    
    def __init__(self, block_hash: str = None, previous_hash: str = None):
        """Initialize block."""
        # Native availability already checked at import
        
        self.block_hash = block_hash
        self.previous_hash = previous_hash
        self._native_block = cf_native.dap_chain_block_new(block_hash, previous_hash)
        if not self._native_block:
            raise RuntimeError(f"Failed to create native block: {block_hash}")
        
    def add_transaction(self, tx_hash: str) -> bool:
        """Add transaction to block using native implementation."""
        return cf_native.dap_chain_block_add_tx(self._native_block, tx_hash)
        
    def get_transaction_count(self) -> int:
        """Get number of transactions using native implementation."""
        return cf_native.dap_chain_block_get_tx_count(self._native_block)
        
    def validate(self) -> bool:
        """Validate block using native implementation."""
        return cf_native.dap_chain_block_validate(self._native_block)

__all__ = ['DAG', 'Block']
