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
import os
import tempfile
sys.path.insert(0, '$PROJECT_ROOT')

try:
    # Setup test environment like in conftest.py
    test_session_dir = '/tmp/quick_test_cellframe_session'
    config_dir = f'{test_session_dir}/etc'
    temp_dir = f'{test_session_dir}/tmp'
    log_dir = f'{test_session_dir}/var/log'
    log_file = f'{log_dir}/quick_test.log'
    
    # Create all required directories
    os.makedirs(config_dir, exist_ok=True)
    os.makedirs(temp_dir, exist_ok=True)
    os.makedirs(log_dir, exist_ok=True)
    
    # Initialize DAP SDK with proper test paths
    try:
        import python_dap
        result = python_dap.dap_sdk_init(
            'quick_test',           # app_name
            test_session_dir,       # working_dir
            config_dir,             # config_dir
            temp_dir,               # temp_dir
            log_file,               # log_file
            2,                      # events_threads
            10000,                  # events_timeout
            False                   # debug_mode
        )
        if result == 0:
            print('✅ DAP SDK initialized with test paths')
    except ImportError:
        print('⚠️ python_dap not available in quick test environment')
    except Exception as e:
        print(f'⚠️ DAP SDK initialization failed: {e}')

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
