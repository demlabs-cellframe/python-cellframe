#!/bin/bash
# Install built modules into test environment

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
VENV_DIR="${PROJECT_ROOT}/venv_test"
SITE_PACKAGES="${VENV_DIR}/lib/python3.11/site-packages"

echo "🔧 Installing Python modules into test environment..."
echo "📁 Project root: ${PROJECT_ROOT}"
echo "📁 Build dir: ${BUILD_DIR}"
echo "📁 Site packages: ${SITE_PACKAGES}"

# Create directories if they don't exist
mkdir -p "${SITE_PACKAGES}/dap"

# Function to copy file with fallback locations
copy_with_fallback() {
    local file_pattern="$1"
    local target="$2"
    local description="$3"
    
    echo "📦 Installing ${description}..."
    
    # Try multiple locations
    for location in "${BUILD_DIR}" "${PROJECT_ROOT}" "${BUILD_DIR}/python-dap/lib" "${PROJECT_ROOT}/python-dap/lib"; do
        if find "${location}" -name "${file_pattern}" -type f 2>/dev/null | head -1 | xargs -I {} cp {} "${target}" 2>/dev/null; then
            echo "✅ ${description} installed from ${location}"
            return 0
        fi
    done
    
    echo "⚠️ ${description} not found, may need manual build"
    return 1
}

# Install DAP SDK shared library
copy_with_fallback "libdap_sdk.so*" "${SITE_PACKAGES}/" "DAP SDK library"

# Install Cellframe SDK shared library  
copy_with_fallback "libcellframe_sdk.so*" "${SITE_PACKAGES}/" "Cellframe SDK library"

# Install python-dap module
copy_with_fallback "*python_dap*.so" "${SITE_PACKAGES}/dap/python_dap.so" "python-dap module"

# Install python-cellframe module
copy_with_fallback "*python_cellframe*.so" "${SITE_PACKAGES}/python_cellframe.so" "python-cellframe module"

echo ""
echo "🧪 Testing module imports..."
cd "${PROJECT_ROOT}"
"${VENV_DIR}/bin/python3" -c "
import sys
print('Python executable:', sys.executable)
print('Site packages check:')

try:
    import dap.python_dap
    print('✅ dap.python_dap: OK')
except Exception as e:
    print(f'❌ dap.python_dap: {e}')

try:
    import python_cellframe
    print('✅ python_cellframe: OK')
except Exception as e:
    print(f'❌ python_cellframe: {e}')
"

echo ""
echo "📊 Installed modules:"
ls -la "${SITE_PACKAGES}/"*cellframe* "${SITE_PACKAGES}/"*dap* "${SITE_PACKAGES}/dap/" 2>/dev/null || echo "Some modules missing"

echo "✅ Installation complete!"