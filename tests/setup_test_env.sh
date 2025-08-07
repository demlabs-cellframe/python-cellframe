#!/bin/bash
# 🧪 Test Environment Setup Script for Python Cellframe SDK
# Создает виртуальное окружение и устанавливает все зависимости
#
# Использование:
#   ./tests/setup_test_env.sh           # Обычный режим
#   VERBOSE=true ./tests/setup_test_env.sh   # Подробные логи
#   CI=true ./tests/setup_test_env.sh        # CI режим (автоматически verbose)
#
# Переменные окружения:
#   VERBOSE=true  - показывать подробные логи pip и venv
#   CI=true       - CI режим (автоматически включает verbose)

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VENV_DIR="$PROJECT_ROOT/venv_test"
PYTHON_DAP_DIR="$PROJECT_ROOT/python-dap"

echo -e "${BLUE}🚀 Python Cellframe SDK - Test Environment Setup${NC}"
echo "Project root: $PROJECT_ROOT"
echo "Virtual environment: $VENV_DIR"

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

# Check Python version
echo -e "\n${BLUE}🐍 Checking Python version...${NC}"
python_version=$(python3 --version 2>&1 | awk '{print $2}' | cut -d. -f1-2)
required_version="3.8"

if [ "$(printf '%s\n' "$required_version" "$python_version" | sort -V | head -n1)" = "$required_version" ]; then
    print_status "Python $python_version is compatible (>= $required_version)"
else
    print_error "Python $python_version is too old. Required: >= $required_version"
    exit 1
fi

# Create virtual environment
echo -e "\n${BLUE}📦 Creating virtual environment...${NC}"
if [ -d "$VENV_DIR" ]; then
    print_warning "Virtual environment already exists at $VENV_DIR"
    # Non-interactive: always recreate for clean environment
    print_status "Recreating virtual environment for clean test environment"
    echo -e "${YELLOW}🗑️  Removing existing virtual environment...${NC}"
    rm -rf "$VENV_DIR"
    print_status "Removed existing virtual environment"
fi

if [ ! -d "$VENV_DIR" ]; then
    echo -e "${BLUE}🔨 Creating new virtual environment with Python $(python3 --version | cut -d' ' -f2)...${NC}"
    echo -e "${YELLOW}⏳ This may take a few moments, please wait...${NC}"
    
    # Create venv with verbose output in CI or if explicitly requested
    if [ "${CI:-false}" = "true" ] || [ "${VERBOSE:-false}" = "true" ]; then
        echo -e "${BLUE}🔍 Running: python3 -m venv --clear --upgrade-deps \"$VENV_DIR\"${NC}"
        python3 -m venv --clear --upgrade-deps "$VENV_DIR" 2>&1 | while IFS= read -r line; do
            echo -e "${YELLOW}   venv: $line${NC}"
        done
    else
        python3 -m venv --clear --upgrade-deps "$VENV_DIR"
    fi
    
    # Verify creation was successful
    if [ -d "$VENV_DIR" ] && [ -f "$VENV_DIR/bin/activate" ]; then
        print_status "Virtual environment created successfully"
        echo -e "${GREEN}   📍 Location: $VENV_DIR${NC}"
        echo -e "${GREEN}   🐍 Python: $(\"$VENV_DIR/bin/python\" --version)${NC}"
    else
        print_error "Failed to create virtual environment"
        echo -e "${RED}Expected files:${NC}"
        echo -e "${RED}  - $VENV_DIR/bin/activate${NC}"
        echo -e "${RED}  - $VENV_DIR/bin/python${NC}"
        exit 1
    fi
fi

# Activate virtual environment
echo -e "\n${BLUE}🔧 Activating virtual environment...${NC}"
source "$VENV_DIR/bin/activate"
print_status "Virtual environment activated"

# Upgrade pip
echo -e "\n${BLUE}📊 Upgrading pip...${NC}"
echo -e "${YELLOW}⏳ Upgrading pip, setuptools, and wheel...${NC}"
if [ "${CI:-false}" = "true" ] || [ "${VERBOSE:-false}" = "true" ]; then
    pip install --upgrade --verbose pip setuptools wheel
else
    pip install --upgrade --progress-bar on pip setuptools wheel
fi
print_status "pip upgraded to version $(pip --version | cut -d' ' -f2)"

# Install test dependencies
echo -e "\n${BLUE}🧪 Installing test dependencies...${NC}"
echo -e "${YELLOW}📦 Installing testing frameworks...${NC}"
if [ "${CI:-false}" = "true" ] || [ "${VERBOSE:-false}" = "true" ]; then
    pip install --verbose pytest pytest-cov pytest-mock pytest-asyncio pytest-xdist
    echo -e "${YELLOW}📦 Installing code quality tools...${NC}"
    pip install --verbose hypothesis coverage mypy black isort flake8
    echo -e "${YELLOW}📦 Installing profiling and build tools...${NC}"
    pip install --verbose memory-profiler psutil
    pip install --verbose build wheel setuptools
else
    pip install --progress-bar on pytest pytest-cov pytest-mock pytest-asyncio pytest-xdist
    echo -e "${YELLOW}📦 Installing code quality tools...${NC}"
    pip install --progress-bar on hypothesis coverage mypy black isort flake8
    echo -e "${YELLOW}📦 Installing profiling and build tools...${NC}"
    pip install --progress-bar on memory-profiler psutil
    pip install --progress-bar on build wheel setuptools
fi
print_status "Test dependencies installed"
echo -e "${GREEN}   🧪 Testing: pytest, coverage, hypothesis${NC}"
echo -e "${GREEN}   🔍 Quality: mypy, black, isort, flake8${NC}"
echo -e "${GREEN}   📊 Profiling: memory-profiler, psutil${NC}"

# Install Python DAP SDK if available
echo -e "\n${BLUE}🔗 Checking for python-dap dependency...${NC}"
if [ -d "$PYTHON_DAP_DIR" ]; then
    print_status "Found python-dap at $PYTHON_DAP_DIR"
    cd "$PYTHON_DAP_DIR"
    
    # Install python-dap in development mode
    echo -e "${BLUE}📦 Installing python-dap in development mode...${NC}"
    echo -e "${YELLOW}⏳ Building and installing python-dap package...${NC}"
    if [ "${CI:-false}" = "true" ] || [ "${VERBOSE:-false}" = "true" ]; then
        pip install --verbose -e .
    else
        pip install --progress-bar on -e .
    fi
    print_status "python-dap installed in development mode"
    echo -e "${GREEN}   📍 Installed from: $PYTHON_DAP_DIR${NC}"
    
    cd "$PROJECT_ROOT"
else
    print_warning "python-dap directory not found. Will try to install from requirements.txt"
    echo -e "${YELLOW}   Expected location: $PYTHON_DAP_DIR${NC}"
fi

# Install project dependencies
echo -e "\n${BLUE}📋 Installing project dependencies...${NC}"
if [ -f "$PROJECT_ROOT/requirements.txt" ]; then
    echo -e "${YELLOW}⏳ Installing from requirements.txt...${NC}"
    if [ "${CI:-false}" = "true" ] || [ "${VERBOSE:-false}" = "true" ]; then
        pip install --verbose -r "$PROJECT_ROOT/requirements.txt"
    else
        pip install --progress-bar on -r "$PROJECT_ROOT/requirements.txt"
    fi
    print_status "Requirements installed"
else
    print_warning "requirements.txt not found"
    echo -e "${YELLOW}   Expected location: $PROJECT_ROOT/requirements.txt${NC}"
fi

# Install python-cellframe in development mode  
echo -e "\n${BLUE}📦 Installing python-cellframe in development mode...${NC}"
cd "$PROJECT_ROOT"
echo -e "${YELLOW}⏳ Building and installing python-cellframe package...${NC}"
if [ "${CI:-false}" = "true" ] || [ "${VERBOSE:-false}" = "true" ]; then
    pip install --verbose -e .
else
    pip install --progress-bar on -e .
fi
print_status "python-cellframe installed in development mode"
echo -e "${GREEN}   📍 Installed from: $PROJECT_ROOT${NC}"

# Verify installation
echo -e "\n${BLUE}🔍 Verifying installation...${NC}"
python -c "
try:
    import sys
    print(f'Python version: {sys.version}')
    print('Available packages:')
    
    try:
        import CellFrame
        print('✅ CellFrame module imported successfully')
    except ImportError as e:
        print(f'❌ CellFrame import failed: {e}')
    
    try:
        import python_dap
        print('✅ python_dap module imported successfully')
    except ImportError as e:
        print('⚠️  python_dap import failed (may be normal if not built): {}'.format(e))
    
    import pytest
    print(f'✅ pytest version: {pytest.__version__}')
    
except Exception as e:
    print(f'❌ Verification failed: {e}')
    exit(1)
"

# Set up environment variables
echo -e "\n${BLUE}🌍 Setting up environment variables...${NC}"
cat > "$VENV_DIR/bin/activate_test" << EOF
#!/bin/bash
# Activate test environment with all necessary paths

source "$VENV_DIR/bin/activate"

# Set Python path to include project modules
export PYTHONPATH="$PROJECT_ROOT:\$PYTHONPATH"

# Set environment for tests
export CELLFRAME_TEST_MODE=1
export DAP_SDK_TEST_MODE=1

# Set test directory for DAP SDK initialization (avoids /opt/dap access issues)
export CELLFRAME_TEST_DIR="$VENV_DIR/tmp/cellframe_tests"

# Set log level for tests
export CELLFRAME_LOG_LEVEL=DEBUG

# Ensure native libraries are found at runtime
export LD_LIBRARY_PATH="/usr/local/lib:\$LD_LIBRARY_PATH"

echo "🧪 Test environment activated!"
echo "PYTHONPATH: \$PYTHONPATH"
echo "Virtual env: $VENV_DIR"
echo ""
echo "To run tests:"
echo "  cd $PROJECT_ROOT/tests"
echo "  pytest"
echo ""
echo "To deactivate:"
echo "  deactivate"
EOF

chmod +x "$VENV_DIR/bin/activate_test"
print_status "Test activation script created at $VENV_DIR/bin/activate_test"

# Create convenience scripts
echo -e "\n${BLUE}📋 Creating convenience scripts...${NC}"

# Test runner script
cat > "$PROJECT_ROOT/run_tests.sh" << 'EOF'
#!/bin/bash
# Convenience script to run tests

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="$PROJECT_ROOT/venv_test"

if [ ! -d "$VENV_DIR" ]; then
    echo "❌ Virtual environment not found. Run tests/setup_test_env.sh first"
    exit 1
fi

# Activate test environment
source "$VENV_DIR/bin/activate_test"

# Change to tests directory
cd "$PROJECT_ROOT/tests"

# Run tests with provided arguments or default
if [ $# -eq 0 ]; then
    echo "🧪 Running all tests..."
    pytest
else
    echo "🧪 Running tests with args: $@"
    pytest "$@"
fi
EOF

chmod +x "$PROJECT_ROOT/run_tests.sh"
print_status "Test runner script created: $PROJECT_ROOT/run_tests.sh"

# Coverage script
cat > "$PROJECT_ROOT/run_coverage.sh" << 'EOF'
#!/bin/bash
# Generate coverage report

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="$PROJECT_ROOT/venv_test"

source "$VENV_DIR/bin/activate_test"
cd "$PROJECT_ROOT/tests"

echo "🧪 Running tests with coverage..."
pytest --cov=../CellFrame --cov-report=html:htmlcov --cov-report=term-missing --cov-fail-under=75

echo "📊 Coverage report generated in tests/htmlcov/"
EOF

chmod +x "$PROJECT_ROOT/run_coverage.sh"
print_status "Coverage script created: $PROJECT_ROOT/run_coverage.sh"

# Quick test script
cat > "$PROJECT_ROOT/quick_test.sh" << 'EOF'
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
EOF

chmod +x "$PROJECT_ROOT/quick_test.sh"
print_status "Quick test script created: $PROJECT_ROOT/quick_test.sh"

echo -e "\n${GREEN}🎉 Test environment setup completed successfully!${NC}"
echo -e "\n${BLUE}📋 Next steps:${NC}"
echo "1. Activate test environment:"
echo "   source $VENV_DIR/bin/activate_test"
echo ""
echo "2. Run quick test:"
echo "   ./quick_test.sh"
echo ""
echo "3. Run all tests:"
echo "   ./run_tests.sh"
echo ""
echo "4. Run tests with coverage:"
echo "   ./run_coverage.sh"
echo ""
echo "5. Run specific tests:"
echo "   ./run_tests.sh -k test_name"
echo "   ./run_tests.sh tests/unit/composer/"
echo ""
echo -e "${YELLOW}💡 Pro tip: Use './run_tests.sh --help' to see all pytest options${NC}"