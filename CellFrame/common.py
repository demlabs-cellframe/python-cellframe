"""
Common functionality for Python-CellFrame
This module requires native python_cellframe C extension module.
Fallback implementations are NOT ALLOWED in Python bindings.
"""

# Fail-fast approach: Import required native module or raise exception
try:
    # First ensure python_dap is imported (required for python_cellframe)
    try:
        import python_dap
    except ImportError:
        pass  # python_dap may not be available in stub mode
    
    import python_cellframe
    if not python_cellframe.is_sdk_available():
        raise RuntimeError("Native SDK not properly initialized")
except ImportError as e:
    raise ImportError(
        "❌ CRITICAL: Native python_cellframe module not available!\n"
        "This is a Python bindings library - fallback implementations are not allowed.\n"
        "Required: python_cellframe native C extension must be properly built and installed.\n"
        f"Original error: {e}\n"
        "Please run: cmake .. && make && make install"
    ) from e

import logging

# Create a logger for the CellFrame package
logger = logging.getLogger('CellFrame')
logger.setLevel(logging.INFO)

# Create console handler if no handlers exist
if not logger.handlers:
    handler = logging.StreamHandler()
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)

# All functionality comes from native module - no fallback classes needed
__all__ = ['python_cellframe', 'logger']