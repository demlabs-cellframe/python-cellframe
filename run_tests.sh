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
