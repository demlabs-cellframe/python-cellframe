"""Module-level variable for logging.

This module provides logging functionality that works both with and without CellFrame SDK.
If CellFrame SDK is available, it uses CFLog from node.logging, otherwise provides a fallback.

Attributes:
    log: An instance of the logging class for logging.
"""

try:
    from .node.logging import CFLog
    log = CFLog()
except ImportError as e:
    raise ImportError(
        "❌ CRITICAL: Native logging module not available!\n"
        "This is a Python bindings library - fallback implementations are not allowed.\n"
        "Required: CellFrame.node.logging must be properly built and installed.\n"
        f"Original error: {e}\n"
        "Please run: cmake .. && make && make install"
    ) from e
