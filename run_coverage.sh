#!/bin/bash
# Generate coverage report

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="$PROJECT_ROOT/venv_test"

source "$VENV_DIR/bin/activate_test"
cd "$PROJECT_ROOT/tests"

echo "🧪 Running tests with coverage..."
pytest --cov=../CellFrame --cov-report=html:htmlcov --cov-report=term-missing --cov-fail-under=75

echo "📊 Coverage report generated in tests/htmlcov/"
