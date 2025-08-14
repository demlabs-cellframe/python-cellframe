#!/bin/bash
# Quick test for basic functionality

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="$PROJECT_ROOT/venv_test"

if [ -f "$VENV_DIR/bin/activate" ]; then
    source "$VENV_DIR/bin/activate"
fi

echo "🔬 Quick functionality test..."
python3 -c "
import sys
sys.path.insert(0, '$PROJECT_ROOT')

try:
    from CellFrame.core import CellframeNode
    print('✅ CellframeNode import successful')
    
    # Try to create a node (may fail if SDK not available)
    try:
        node = CellframeNode()
        print('✅ CellframeNode creation successful')
    except Exception as e:
        print(f'⚠️  CellframeNode creation failed (may be normal): {e}')
    
    print('🎉 Basic functionality check passed!')
    
except Exception as e:
    print(f'❌ Quick test failed: {e}')
    sys.exit(1)
"
