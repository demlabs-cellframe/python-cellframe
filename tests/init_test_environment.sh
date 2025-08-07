#!/bin/bash
# 🚀 Full Test Environment Initialization Script
# Создает полное тестовое окружение с установкой всех пакетов и зависимостей
#
# Использование:
#   ./tests/init_test_environment.sh           # Обычный режим
#   VERBOSE=true ./tests/init_test_environment.sh   # Подробные логи
#   CI=true ./tests/init_test_environment.sh        # CI режим
#
# Переменные окружения:
#   VERBOSE=true  - показывать подробные логи pip и venv
#   CI=true       - CI режим (автоматически включает verbose)
#   FORCE_REBUILD=true - принудительная пересборка всех пакетов

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VENV_DIR="$PROJECT_ROOT/venv_test"
PYTHON_DAP_DIR="$PROJECT_ROOT/python-dap"
BUILD_DIR="$PROJECT_ROOT/build"

echo -e "${CYAN}🚀 Python Cellframe SDK - Full Test Environment Initialization${NC}"
echo "=========================================================================="
echo -e "${BLUE}Project root:${NC} $PROJECT_ROOT"
echo -e "${BLUE}Virtual environment:${NC} $VENV_DIR"
echo -e "${BLUE}Build directory:${NC} $BUILD_DIR"
echo -e "${BLUE}Python DAP directory:${NC} $PYTHON_DAP_DIR"
echo "=========================================================================="

# Function to print status
print_status() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_step() {
    echo -e "\n${PURPLE}🔸 $1${NC}"
}

print_substep() {
    echo -e "${CYAN}   🔹 $1${NC}"
}

# Determine verbose mode
if [ "${CI:-false}" = "true" ] || [ "${VERBOSE:-false}" = "true" ]; then
    VERBOSE_MODE=true
    PIP_FLAGS="--verbose"
    echo -e "${YELLOW}🔍 VERBOSE mode enabled - detailed logging activated${NC}"
else
    VERBOSE_MODE=false
    PIP_FLAGS="--progress-bar on"
    echo -e "${BLUE}📊 Normal mode - progress bars enabled${NC}"
fi

# Step 1: Setup basic virtual environment
print_step "Setting up Python virtual environment"

# Remove existing venv if FORCE_REBUILD or if activation fails
if [ "${FORCE_REBUILD:-false}" = "true" ] || [ -d "$VENV_DIR" ]; then
    if [ "${FORCE_REBUILD:-false}" = "true" ]; then
        print_substep "FORCE_REBUILD=true - removing existing environment"
    else
        print_substep "Environment exists - recreating for clean state"
    fi
    rm -rf "$VENV_DIR"
fi

# Create new virtual environment
print_substep "Creating virtual environment with Python $(python3 --version | cut -d' ' -f2)"
echo -e "${YELLOW}⏳ This may take a few moments...${NC}"

if [ "$VERBOSE_MODE" = "true" ]; then
    python3 -m venv --clear --upgrade-deps "$VENV_DIR" 2>&1 | while IFS= read -r line; do
        echo -e "${YELLOW}   venv: $line${NC}"
    done
else
    python3 -m venv --clear --upgrade-deps "$VENV_DIR"
fi

# Verify venv creation
if [ -d "$VENV_DIR" ] && [ -f "$VENV_DIR/bin/activate" ]; then
    print_status "Virtual environment created successfully"
    echo -e "${GREEN}   📍 Location: $VENV_DIR${NC}"
    echo -e "${GREEN}   🐍 Python: $($VENV_DIR/bin/python --version)${NC}"
else
    print_error "Failed to create virtual environment"
    exit 1
fi

# Step 2: Activate virtual environment and upgrade pip
print_step "Activating virtual environment and upgrading tools"
source "$VENV_DIR/bin/activate"
print_status "Virtual environment activated"

print_substep "Upgrading pip, setuptools, and wheel"
if [ "$VERBOSE_MODE" = "true" ]; then
    pip install --upgrade --verbose pip setuptools wheel
else
    pip install --upgrade --progress-bar on pip setuptools wheel
fi
print_status "pip upgraded to version $(pip --version | cut -d' ' -f2)"

# Step 3: Install build and test dependencies
print_step "Installing build and test dependencies"

print_substep "Installing build tools"
if [ "$VERBOSE_MODE" = "true" ]; then
    pip install --verbose build wheel setuptools twine
else
    pip install --progress-bar on build wheel setuptools twine
fi

print_substep "Installing core dependencies"
if [ "$VERBOSE_MODE" = "true" ]; then
    pip install --verbose pydantic typing-extensions
else
    pip install --progress-bar on pydantic typing-extensions
fi

print_substep "Installing testing frameworks"
if [ "$VERBOSE_MODE" = "true" ]; then
    pip install --verbose pytest pytest-cov pytest-mock pytest-asyncio pytest-xdist pytest-benchmark
else
    pip install --progress-bar on pytest pytest-cov pytest-mock pytest-asyncio pytest-xdist pytest-benchmark
fi

print_substep "Installing code quality tools"
if [ "$VERBOSE_MODE" = "true" ]; then
    pip install --verbose hypothesis coverage mypy black isort flake8
else
    pip install --progress-bar on hypothesis coverage mypy black isort flake8
fi

print_substep "Installing profiling tools"
if [ "$VERBOSE_MODE" = "true" ]; then
    pip install --verbose memory-profiler psutil
else
    pip install --progress-bar on memory-profiler psutil
fi

print_status "All dependencies installed successfully"
echo -e "${GREEN}   🧪 Testing: pytest, coverage, hypothesis${NC}"
echo -e "${GREEN}   🔍 Quality: mypy, black, isort, flake8${NC}"
echo -e "${GREEN}   📊 Profiling: memory-profiler, psutil${NC}"
echo -e "${GREEN}   🔧 Build: build, wheel, twine${NC}"

# Step 4: Install python-dap (dependency) from CMake build
print_step "Installing python-dap dependency from CMake build"

if [ -d "$PYTHON_DAP_DIR" ]; then
    print_substep "Found python-dap at $PYTHON_DAP_DIR"
    
    # Check if built libraries exist
    if [ -d "$BUILD_DIR/python-dap" ]; then
        print_substep "Using pre-built python-dap from CMake"
        cd "$BUILD_DIR"
        
        # Try to use CMake install target if available
        if make -q python-dap/install_venv 2>/dev/null; then
            print_substep "Using CMake install_venv target"
            VIRTUAL_ENV="$VENV_DIR" make python-dap/install_venv
        else
            print_substep "Building and installing python-dap wheel"
            cd "$PYTHON_DAP_DIR"
            
            # Clean previous builds
            rm -rf dist/ build/ *.egg-info/
            
            # Build wheel with CMake libraries
            if [ "$VERBOSE_MODE" = "true" ]; then
                python -m build --wheel --verbose
            else
                python -m build --wheel
            fi
            
            # Install wheel package (not editable mode to avoid dependency issues)
            if [ "$VERBOSE_MODE" = "true" ]; then
                pip install --force-reinstall --verbose --find-links dist python-dap
            else
                pip install --force-reinstall --progress-bar on --find-links dist python-dap
            fi
        fi
    else
        print_warning "No CMake build found for python-dap, building with Python build tools"
        cd "$PYTHON_DAP_DIR"
        
        # Clean previous builds
        print_substep "Cleaning previous builds"
        rm -rf dist/ build/ *.egg-info/
        
        print_substep "Building python-dap wheel package"
        if [ "$VERBOSE_MODE" = "true" ]; then
            python -m build --wheel --verbose
        else
            python -m build --wheel
        fi
        
        print_substep "Installing python-dap in test environment"
        if [ "$VERBOSE_MODE" = "true" ]; then
            pip install --force-reinstall --verbose --find-links dist python-dap
        else
            pip install --force-reinstall --progress-bar on --find-links dist python-dap
        fi
    fi
    
    print_status "python-dap installed successfully"
    echo -e "${GREEN}   📦 Installed from: $PYTHON_DAP_DIR${NC}"
    cd "$PROJECT_ROOT"
else
    print_warning "python-dap directory not found at $PYTHON_DAP_DIR"
    print_warning "Will try to install from requirements.txt if available"
fi

# Step 5: Install python-cellframe from CMake build
print_step "Installing python-cellframe package from CMake build"

cd "$PROJECT_ROOT"

# Check if native libraries are built
if [ ! -d "$BUILD_DIR" ]; then
    print_error "Build directory not found!"
    print_error "You must run CMake build first:"
    print_error "  mkdir -p build && cd build"
    print_error "  cmake .. -DBUILD_TESTS=ON"
    print_error "  make -j\$(nproc)"
    exit 1
else
    print_substep "Build directory found - using CMake built libraries"
fi

# Check if python-cellframe library was built
if [ -f "$BUILD_DIR/libpython_cellframe.so" ]; then
    print_substep "Building and installing python-cellframe from CMake build"
    
    # Clean previous builds
    rm -rf dist/ build/ *.egg-info/
    
    # Build wheel with CMake libraries
    if [ "$VERBOSE_MODE" = "true" ]; then
        python -m build --wheel --verbose
    else
        python -m build --wheel
    fi
    
    # Install wheel package
    if [ "$VERBOSE_MODE" = "true" ]; then
        pip install --force-reinstall --verbose --find-links dist --find-links "$PYTHON_DAP_DIR/dist" python-cellframe
    else
        pip install --force-reinstall --progress-bar on --find-links dist --find-links "$PYTHON_DAP_DIR/dist" python-cellframe
    fi
    
    print_status "python-cellframe installed from wheel (using CMake libraries)"
    echo -e "${GREEN}   📦 Installed from: $PROJECT_ROOT${NC}"
    echo -e "${GREEN}   🔗 Using native library: $BUILD_DIR/libpython_cellframe.so${NC}"
else
    print_warning "No python-cellframe library found in build directory"
    print_warning "Falling back to Python build tools"
    
    # Clean previous builds
    print_substep "Cleaning previous builds"
    rm -rf dist/ build/ *.egg-info/
    
    print_substep "Building python-cellframe wheel package"
    if [ "$VERBOSE_MODE" = "true" ]; then
        python -m build --wheel --verbose
    else
        python -m build --wheel
    fi
    
    print_substep "Installing python-cellframe in test environment"
    if [ "$VERBOSE_MODE" = "true" ]; then
        pip install --force-reinstall --verbose --find-links dist python-cellframe
    else
        pip install --force-reinstall --progress-bar on --find-links dist python-cellframe
    fi
    
    print_status "python-cellframe installed from wheel"
    echo -e "${GREEN}   📦 Installed from: $PROJECT_ROOT${NC}"
fi

# Step 6: Setup native libraries and environment
print_step "Setting up native libraries and test environment"

if [ -d "$BUILD_DIR" ]; then
    print_substep "Setting up library paths and environment"
    
    # Create enhanced activation script with all necessary paths
    cat > "$VENV_DIR/bin/activate_test" << EOF
#!/bin/bash
# Enhanced activation script for Python Cellframe SDK testing
# This script sets up all necessary environment variables and library paths

# Activate virtual environment
source "$VENV_DIR/bin/activate"

# Set up library paths for native libraries
export LD_LIBRARY_PATH="$BUILD_DIR:$BUILD_DIR/dap-sdk-build:$BUILD_DIR/cellframe-sdk-embedded:$BUILD_DIR/python-dap:\$LD_LIBRARY_PATH"

# Set up Python paths
export PYTHONPATH="$PROJECT_ROOT:$BUILD_DIR:\$PYTHONPATH"

# Set test mode flags
export CELLFRAME_TEST_MODE=1
export DAP_SDK_TEST_MODE=1

# Set DAP SDK to use temp directories for tests
export DAP_SDK_WORKING_DIR="$VENV_DIR/tmp/dap_sdk"
mkdir -p "$DAP_SDK_WORKING_DIR"

# Make CMake build libraries available
if [ -f "$BUILD_DIR/libpython_cellframe.so" ]; then
    export PYTHON_CELLFRAME_LIBRARY_PATH="$BUILD_DIR"
fi

if [ -f "$BUILD_DIR/python-dap/libpython_dap.so" ]; then
    export PYTHON_DAP_LIBRARY_PATH="$BUILD_DIR/python-dap"
fi

# Set DAP SDK to use temp directories for tests
export DAP_SDK_WORKING_DIR="$VENV_DIR/tmp/dap_sdk"
mkdir -p "\$DAP_SDK_WORKING_DIR"

# Print status
echo "✅ Test environment activated with CMake library paths"
echo "   📂 Build directory: $BUILD_DIR"
echo "   🐍 Python path: \$PYTHONPATH"
echo "   📚 Library path: \$LD_LIBRARY_PATH"
echo "   🧪 Test mode: CELLFRAME_TEST_MODE=1, DAP_SDK_TEST_MODE=1"
echo "   📁 DAP SDK working dir: \$DAP_SDK_WORKING_DIR"
EOF
    chmod +x "$VENV_DIR/bin/activate_test"
    print_status "Enhanced test environment activation script created"
    
    # Optionally copy libraries to venv site-packages (backup method)
    print_substep "Setting up backup library locations"
    
    # Copy python-cellframe native library
    if [ -f "$BUILD_DIR/libpython_cellframe.so" ]; then
        cp -f "$BUILD_DIR"/libpython_cellframe.so* "$VENV_DIR"/lib/python*/site-packages/ 2>/dev/null || true
        print_status "python-cellframe native library copied to venv"
    fi
    
    # Copy python-dap native library if available
    if [ -f "$BUILD_DIR/python-dap/libpython_dap.so" ]; then
        cp -f "$BUILD_DIR"/python-dap/libpython_dap.so* "$VENV_DIR"/lib/python*/site-packages/ 2>/dev/null || true
        print_status "python-dap native library copied to venv"
    fi
    
    # Copy DAP SDK library
    if [ -f "$BUILD_DIR/dap-sdk-build/libdap_sdk.so" ]; then
        cp -f "$BUILD_DIR"/dap-sdk-build/libdap_sdk.so* "$VENV_DIR"/lib/python*/site-packages/ 2>/dev/null || true
        print_status "DAP SDK library copied to venv"
    fi
    
else
    print_error "Build directory not found - cannot set up native libraries"
    print_error "Run CMake build first!"
    exit 1
fi

# Step 7: Verify installation
print_step "Verifying test environment"

print_substep "Testing Python imports with CMake environment"

# Test imports using the enhanced activation script
source "$VENV_DIR/bin/activate_test"

python -c "
import sys
import os
print(f'✅ Python version: {sys.version}')
print(f'✅ Virtual environment: {sys.prefix}')

# Show environment variables
print(f'🔧 CELLFRAME_TEST_MODE: {os.getenv(\"CELLFRAME_TEST_MODE\", \"Not set\")}')
print(f'🔧 DAP_SDK_TEST_MODE: {os.getenv(\"DAP_SDK_TEST_MODE\", \"Not set\")}')
print(f'🔧 LD_LIBRARY_PATH contains build dir: {\"$BUILD_DIR\" in os.getenv(\"LD_LIBRARY_PATH\", \"\")}')

# Test basic testing framework imports
try:
    import pytest
    print('✅ pytest available')
except ImportError as e:
    print(f'❌ pytest import failed: {e}')

try:
    import coverage
    print('✅ coverage available')
except ImportError as e:
    print(f'❌ coverage import failed: {e}')

# Test DAP SDK import (python-dap)
try:
    import dap
    print('✅ dap module available (python-dap)')
    print(f'   dap.__file__ = {dap.__file__}')
    # Test some basic DAP functionality
    funcs = [f for f in dir(dap) if not f.startswith('_')]
    print(f'   Available functions: {len(funcs)}')
except ImportError as e:
    print(f'❌ dap import failed: {e}')
    print('   This means python-dap is not properly installed')

# Test high-level CellFrame import
try:
    import CellFrame
    print('✅ CellFrame module available (python-cellframe)')
    print(f'   CellFrame.__file__ = {CellFrame.__file__}')
    
    # Test some CellFrame functionality
    try:
        from CellFrame import is_sdk_available
        print(f'   SDK availability check: {is_sdk_available()}')
    except Exception as e:
        print(f'   SDK availability check failed: {e}')
        
except ImportError as e:
    print(f'❌ CellFrame import failed: {e}')
    print('   This means python-cellframe is not properly installed')

print('\\n🎯 Import test completed')
"

# Deactivate the test environment after verification
deactivate 2>/dev/null || true

# Step 8: Create convenient test runner script
print_step "Creating test runner utilities"

cat > "$PROJECT_ROOT/run_tests.sh" << 'EOF'
#!/bin/bash
# 🧪 Convenient test runner for Python Cellframe SDK
# Automatically activates test environment and runs tests

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="$PROJECT_ROOT/venv_test"

if [ ! -f "$VENV_DIR/bin/activate_test" ]; then
    echo "❌ Test environment not initialized!"
    echo "Run: ./tests/init_test_environment.sh"
    exit 1
fi

echo "🧪 Activating test environment..."
source "$VENV_DIR/bin/activate_test"

echo "🚀 Running tests..."
cd "$PROJECT_ROOT/build"
ctest --output-on-failure "$@"
EOF

chmod +x "$PROJECT_ROOT/run_tests.sh"
print_status "Test runner script created: ./run_tests.sh"

# Step 9: Final summary
print_step "🎉 Test environment initialization completed!"

echo ""
echo "=========================================================================="
echo -e "${GREEN}✅ Test environment successfully initialized!${NC}"
echo "=========================================================================="
echo ""
echo -e "${BLUE}📋 What was set up:${NC}"
echo -e "${GREEN}   🐍 Virtual environment:${NC} $VENV_DIR"
echo -e "${GREEN}   📦 python-dap package:${NC} Built and installed"
echo -e "${GREEN}   📦 python-cellframe package:${NC} Built and installed"
echo -e "${GREEN}   🔧 Native libraries:${NC} Copied to venv"
echo -e "${GREEN}   🧪 Test dependencies:${NC} pytest, coverage, etc."
echo -e "${GREEN}   🔍 Code quality tools:${NC} mypy, black, flake8"
echo ""
echo -e "${BLUE}📋 How to run tests:${NC}"
echo -e "${CYAN}   Quick way:${NC}"
echo -e "     ./run_tests.sh"
echo ""
echo -e "${CYAN}   Manual way:${NC}" 
echo -e "     source $VENV_DIR/bin/activate_test"
echo -e "     cd build && ctest --output-on-failure"
echo ""
echo -e "${BLUE}📋 Environment variables:${NC}"
echo -e "${GREEN}   CELLFRAME_TEST_MODE=1${NC} - Test mode enabled"
echo -e "${GREEN}   LD_LIBRARY_PATH${NC} - Native libraries accessible"
echo -e "${GREEN}   PYTHONPATH${NC} - Project modules accessible"
echo ""

# Deactivate virtual environment
deactivate 2>/dev/null || true

print_status "Ready to run tests! Use: ./run_tests.sh"