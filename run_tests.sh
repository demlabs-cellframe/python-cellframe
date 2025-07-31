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
