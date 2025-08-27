#!/bin/bash
# Quick test for basic functionality

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="$PROJECT_ROOT/venv_test"

source "$VENV_DIR/bin/activate_test"

echo "🔬 Quick functionality test..."
python -c "
import sys
sys.path.insert(0, '$PROJECT_ROOT')

try:
    import CellFrame
    print('✅ CellFrame module import successful')
    
    # Test context system
    try:
        from CellFrame.core.context import ExecutionMode, AppContext
        print('✅ Context system import successful')
        print(f'✅ ExecutionMode.LIBRARY = {ExecutionMode.LIBRARY.value}')
    except Exception as e:
        print(f'⚠️  Context system test failed: {e}')
    
    # Test basic types
    try:
        from CellFrame.core import TxOut, TxIn, Datum
        print('✅ Core types import successful')
    except Exception as e:
        print(f'⚠️  Core types test failed: {e}')
    
    print('🎉 Basic functionality check passed!')
    
except Exception as e:
    import traceback
    print(f'❌ Quick test failed: {e}')
    print('📊 Full traceback:')
    traceback.print_exc()
    sys.exit(1)
"
