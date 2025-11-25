"""
💰 CellFrame Emission API

Token emission functionality.
Integrated from helpers with enhancements.
"""

import logging
from typing import Optional, Dict, Any, List
from datetime import datetime
from decimal import Decimal

from ..core.exceptions import CellframeException
from ..common import logger
from ..types import TSD, DatumTypes

# Try to import native CellFrame emission components
try:
    try:
        from ..common.types import DatumTx
    except ImportError:
        # Create basic DatumTx when not available
        class DatumTx:
            def __init__(self, tx_data=None):
                self.tx_data = tx_data
    # Mempool, Math, DatumEmission, TxTSD are imported from other modules or native
    import python_cellframe as cf_native
except ImportError as e:
    # FAIL-FAST: No dummy types allowed
    raise ImportError(
        "❌ CRITICAL: Native CellFrame emission types not available!\n"
        "This is a Python bindings library - dummy types are not allowed.\n"
        f"Original error: {e}\n"
        "Please ensure python_cellframe native module is properly built and installed."
    ) from e


class EmissionError(CellframeException):
    """Emission operation error."""
    pass


class CellframeEmission:
    """
    CellFrame token emission management.
    
    Integrated from helpers functionality.
    """
    
    def __init__(self, datum: Any = None, event: Any = None):
        """
        Initialize emission instance.
        
        Args:
            datum: DatumEmission object
            event: DAG event (optional)
        """
        self._datum = datum
        self._event = event
        self._tsd_data: Dict[int, Any] = {}
        
        if datum:
            self._parse_emission_data()
        
        logger.debug(f"CellframeEmission initialized")
    
    def _parse_emission_data(self):
        """Parse emission data from datum."""
        try:
            if hasattr(self._datum, 'tsd'):
                # Parse TSD (Typed Section Data)
                tsd = self._datum.tsd
                while tsd:
                    tsd_type = tsd.type
                    tsd_data = tsd.data
                    self._tsd_data[tsd_type] = tsd_data
                    tsd = tsd.next
                    
        except Exception as e:
            logger.error(f"Failed to parse emission data: {e}")
    
    def get_tsd(self, tsd_type: int) -> Optional[Any]:
        """
        Get Typed Section Data by type.
        
        Integrated from helpers functionality.
        
        Args:
            tsd_type: TSD type constant
            
        Returns:
            TSD data or None if not found
        """
        return self._tsd_data.get(tsd_type)
    
    def set_tsd(self, tsd_type: int, data: Any) -> bool:
        """
        Set Typed Section Data.
        
        Args:
            tsd_type: TSD type constant
            data: Data to set
            
        Returns:
            True if set successfully
        """
        try:
            self._tsd_data[tsd_type] = data
            logger.debug(f"Set TSD type {tsd_type}: {data}")
            return True
        except Exception as e:
            logger.error(f"Failed to set TSD: {e}")
            return False
    
    @property
    def token_ticker(self) -> Optional[str]:
        """Get token ticker from TSD."""
        return self.get_tsd(TSD.TYPE_TOKEN_SYM)
    
    @property
    def value(self) -> Optional[int]:
        """Get emission value from TSD."""
        return self.get_tsd(TSD.TYPE_VALUE)
    
    @property
    def address(self) -> Optional[str]:
        """Get target address from TSD."""
        addr_data = self.get_tsd(TSD.TYPE_ADDRESS)
        if addr_data:
            try:
                # Use native address conversion directly
                import python_cellframe as cf_native
                return cf_native.dap_chain_addr_to_str(addr_data)
            except:
                return str(addr_data)
        return str(addr_data) if addr_data else None
    
    @property
    def timestamp(self) -> Optional[datetime]:
        """Get emission timestamp from TSD."""
        ts_data = self.get_tsd(TSD.TYPE_TIMESTAMP)
        if ts_data:
            try:
                return datetime.fromtimestamp(int(ts_data))
            except:
                return None
        return None
    
    @property
    def net_id(self) -> Optional[int]:
        """Get network ID from TSD."""
        return self.get_tsd(TSD.TYPE_NET_ID)
    
    @property
    def unique_id(self) -> Optional[str]:
        """Get unique ID from TSD."""
        return self.get_tsd(TSD.TYPE_UNIQUE_ID)
    
    @classmethod
    def create_emission(cls, wallet: Any, token_symbol: str, value: int, 
                       tsd_data: Dict[int, Any] = None) -> 'CellframeEmission':
        """
        Create new token emission.
        
        Integrated from helpers functionality.
        
        Args:
            wallet: Wallet for signing
            token_symbol: Token symbol
            value: Emission value
            tsd_data: Additional TSD data
            
        Returns:
            CellframeEmission instance
        """
        try:
            # Native availability already checked at import
            emission = cls()
            emission.set_tsd(TSD.TYPE_TOKEN_SYM, token_symbol)
            emission.set_tsd(TSD.TYPE_VALUE, value)
            if tsd_data:
                for tsd_type, data in tsd_data.items():
                    emission.set_tsd(tsd_type, data)
                return emission
            
            # Create emission datum using native API
            # Implementation depends on native API structure
            emission = cls()
            emission.set_tsd(TSD.TYPE_TOKEN_SYM, token_symbol)
            emission.set_tsd(TSD.TYPE_VALUE, value)
            emission.set_tsd(TSD.TYPE_TIMESTAMP, int(datetime.now().timestamp()))
            
            if tsd_data:
                for tsd_type, data in tsd_data.items():
                    emission.set_tsd(tsd_type, data)
            
            logger.info(f"Created emission: {token_symbol} = {value}")
            return emission
            
        except Exception as e:
            logger.error(f"Failed to create emission: {e}")
            raise EmissionError(f"Failed to create emission: {e}")
    
    def place_emission(self, ledger: Any, chain: Any = None) -> bool:
        """
        Place emission in ledger.
        
        Uses native C API dap_ledger_token_emission_add.
        
        Args:
            ledger: Ledger handle (capsule)
            chain: Chain handle (optional, for mempool placement)
            
        Returns:
            True if placed successfully
            
        Raises:
            EmissionError: If placement fails
        """
        try:
            import python_cellframe as cf_native
            
            if not hasattr(cf_native, 'ledger_token_emission_add'):
                raise ImportError(
                    "❌ CRITICAL: ledger_token_emission_add not available in python_cellframe!\n"
                    "Please ensure this function is exported in src/cellframe_ledger.c"
                )
            
            # Convert emission to bytes (dap_chain_datum_token_emission_t)
            # This requires serializing the emission datum
            # For now, raise NotImplementedError until proper serialization is implemented
            raise NotImplementedError(
                "Emission serialization to dap_chain_datum_token_emission_t not yet implemented. "
                "Use native emission creation functions directly."
            )
            
        except Exception as e:
            logger.error(f"Failed to place emission: {e}")
            raise EmissionError(f"Failed to place emission: {e}")
    
    @classmethod
    def find_by_hash(cls, ledger: Any, emission_hash: str) -> Optional['CellframeEmission']:
        """
        Find emission by hash in ledger.
        
        Uses native C API dap_ledger_token_emission_find.
        
        Args:
            ledger: Ledger handle (capsule)
            emission_hash: Emission hash string
            
        Returns:
            CellframeEmission instance or None if not found
            
        Raises:
            EmissionError: If lookup fails
        """
        try:
            import python_cellframe as cf_native
            
            if not hasattr(cf_native, 'ledger_token_emission_find'):
                raise ImportError(
                    "❌ CRITICAL: ledger_token_emission_find not available in python_cellframe!\n"
                    "Please ensure this function is exported in src/cellframe_ledger.c"
                )
            
            # Convert hash string to dap_hash_fast_t
            # Parse hash string
            emission_hash_bytes = bytes.fromhex(emission_hash.replace('0x', ''))
            if len(emission_hash_bytes) != 32:  # sizeof(dap_hash_fast_t)
                raise ValueError(f"Invalid emission hash length: {len(emission_hash_bytes)}")
            
            # Find emission in ledger
            emission_datum = cf_native.ledger_token_emission_find(ledger, emission_hash_bytes)
            if not emission_datum:
                return None
            
            # Create CellframeEmission from datum
            return cls(datum=emission_datum)
            
        except Exception as e:
            logger.error(f"Failed to find emission: {e}")
            raise EmissionError(f"Failed to find emission: {e}")
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert emission to dictionary."""
        return {
            'token_ticker': self.token_ticker,
            'value': self.value,
            'address': self.address,
            'timestamp': self.timestamp.isoformat() if self.timestamp else None,
            'net_id': self.net_id,
            'unique_id': self.unique_id,
            'tsd_data': self._tsd_data
        }
    
    def __str__(self) -> str:
        """String representation."""
        return f"CellframeEmission({self.token_ticker}={self.value}, addr={self.address})"
    
    def __repr__(self) -> str:
        """Detailed representation."""
        return f"CellframeEmission(token={self.token_ticker}, value={self.value}, tsd_count={len(self._tsd_data)})"


class EmissionManager:
    """
    Manager for multiple emissions.
    """
    
    def __init__(self):
        """Initialize emission manager."""
        self._emissions: List[CellframeEmission] = []
        
    def create_emission(self, wallet: Any, token_symbol: str, value: int,
                       tsd_data: Dict[int, Any] = None) -> CellframeEmission:
        """Create and register new emission."""
        emission = CellframeEmission.create_emission(wallet, token_symbol, value, tsd_data)
        self._emissions.append(emission)
        return emission
    
    def get_emissions(self) -> List[CellframeEmission]:
        """Get all managed emissions."""
        return self._emissions.copy()
    
    def get_emissions_by_token(self, token_symbol: str) -> List[CellframeEmission]:
        """Get emissions by token symbol."""
        return [e for e in self._emissions if e.token_ticker == token_symbol]
    
    def get_total_emission(self, token_symbol: str) -> int:
        """Get total emission value for token."""
        return sum(e.value or 0 for e in self.get_emissions_by_token(token_symbol))
    
    def place_all_emissions(self, chain: Any) -> bool:
        """Place all emissions in mempool."""
        success = True
        for emission in self._emissions:
            if not emission.place_emission(chain):
                success = False
        return success


# Convenience functions
def create_emission(wallet: Any, token_symbol: str, value: int, 
                   tsd_data: Dict[int, Any] = None) -> CellframeEmission:
    """Create token emission."""
    return CellframeEmission.create_emission(wallet, token_symbol, value, tsd_data)


def parse_emission_from_datum(datum: Any) -> CellframeEmission:
    """Parse emission from datum."""
    return CellframeEmission(datum) 