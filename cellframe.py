"""
Alias module for backward compatibility.
Maps 'cellframe' imports to 'CellFrame' package.
"""

# Import everything from CellFrame and re-export
from CellFrame import *

# Keep the same module structure
import sys
sys.modules['cellframe'] = sys.modules['CellFrame']

# For submodules
import CellFrame.chain
import CellFrame.composer  
import CellFrame.core
import CellFrame.helpers
import CellFrame.legacy
import CellFrame.migration
import CellFrame.network
import CellFrame.node
import CellFrame.schemas
import CellFrame.services
import CellFrame.wallet

sys.modules['cellframe.chain'] = CellFrame.chain
sys.modules['cellframe.composer'] = CellFrame.composer
sys.modules['cellframe.core'] = CellFrame.core
sys.modules['cellframe.helpers'] = CellFrame.helpers
sys.modules['cellframe.legacy'] = CellFrame.legacy
sys.modules['cellframe.migration'] = CellFrame.migration
sys.modules['cellframe.network'] = CellFrame.network
sys.modules['cellframe.node'] = CellFrame.node
sys.modules['cellframe.schemas'] = CellFrame.schemas
sys.modules['cellframe.services'] = CellFrame.services
sys.modules['cellframe.wallet'] = CellFrame.wallet
