"""Module-level variable for logging.

This module provides logging functionality that works both with and without CellFrame SDK.
If CellFrame SDK is available, it uses CFLog from node.logging, otherwise provides a fallback.

Attributes:
    log: An instance of the logging class for logging.
"""

try:
    from .node.logging import CFLog
    log = CFLog()
except ImportError:
    # Fallback logging when CellFrame SDK is not available
    import logging
    
    class FallbackLog:
        def __init__(self):
            self.logger = logging.getLogger('pycfhelpers')
            if not self.logger.handlers:
                handler = logging.StreamHandler()
                formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
                handler.setFormatter(formatter)
                self.logger.addHandler(handler)
                self.logger.setLevel(logging.INFO)
        
        def debug(self, message, **kwargs):
            self.logger.debug(message)
        
        def info(self, message, **kwargs):
            self.logger.info(message)
        
        def notice(self, message, **kwargs):
            self.logger.info(f"NOTICE: {message}")
        
        def message(self, message, **kwargs):
            self.logger.info(message)
        
        def warning(self, message, **kwargs):
            self.logger.warning(message)
        
        def error(self, message, **kwargs):
            self.logger.error(message)
        
        def critical(self, message, **kwargs):
            self.logger.critical(message)
    
    log = FallbackLog()
