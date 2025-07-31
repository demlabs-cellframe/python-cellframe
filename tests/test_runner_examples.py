"""
🏃 Test Runner Examples and Commands
Examples of how to run different types of tests.
"""

# =========================================
# BASIC TEST RUNNING COMMANDS
# =========================================

"""
# Run all tests
./run_tests.sh

# Run specific test file
./run_tests.sh tests/unit/core/test_cellframe_node.py

# Run specific test class
./run_tests.sh tests/unit/core/test_cellframe_node.py::TestCellframeNode

# Run specific test method
./run_tests.sh tests/unit/core/test_cellframe_node.py::TestCellframeNode::test_node_creation_with_context

# Run tests by marker
./run_tests.sh -m unit
./run_tests.sh -m integration
./run_tests.sh -m "unit and not slow"

# Run tests with coverage
./run_coverage.sh

# Quick functionality test
./quick_test.sh
"""

# =========================================
# CMAKE/CTEST INTEGRATION COMMANDS
# =========================================

"""
# Build project with tests
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON -DBUILD_WITH_COVERAGE=ON

# Setup test environment
make setup_test_env

# Run all tests via ctest
make test
# or
ctest

# Run specific test suite
ctest -R test_unit
ctest -R test_integration
ctest -R test_e2e

# Run tests in parallel
ctest -j4

# Run with verbose output
ctest --output-on-failure

# Run custom test target
make run_all_tests
"""

# =========================================
# PYTEST ADVANCED OPTIONS
# =========================================

"""
# Run tests with specific fixtures
pytest -v --fixtures

# Run tests with fixture scope information
pytest --fixtures-per-test

# Run with detailed output
pytest -v -s

# Run with profiling
pytest --profile

# Run with memory profiling (requires memory_profiler)
pytest --profile-svg

# Filter tests by expression
pytest -k "test_wallet and not slow"
pytest -k "TestCellframeNode or TestWallet"

# Run failed tests from last run
pytest --lf

# Run failed tests first
pytest --ff

# Stop on first failure
pytest -x

# Run specific number of tests
pytest --maxfail=3

# Parallel execution (requires pytest-xdist)
pytest -n 4

# Generate JUnit XML report
pytest --junitxml=test_results.xml

# Generate HTML report
pytest --html=report.html

# Run with timeout (requires pytest-timeout)
pytest --timeout=300
"""

# =========================================
# MARKER EXAMPLES
# =========================================

"""
# Run only unit tests
pytest -m unit

# Run unit and integration tests
pytest -m "unit or integration"

# Skip slow tests
pytest -m "not slow"

# Run only security tests
pytest -m security

# Run performance and load tests
pytest -m "performance or load"

# Run tests excluding certain markers
pytest -m "not (slow or stress)"

# Run specific test categories
pytest -m "functional and not regression"
"""

# =========================================
# FIXTURE USAGE EXAMPLES
# =========================================

"""
# Show available fixtures
pytest --fixtures tests/

# Show fixture setup/teardown
pytest -v --setup-show

# Run tests with specific fixture scope
pytest --fixtures-per-test

# Debug fixture dependencies
pytest --collect-only --quiet
"""

# =========================================
# COVERAGE EXAMPLES
# =========================================

"""
# Run with coverage (basic)
pytest --cov=CellFrame

# Coverage with missing lines
pytest --cov=CellFrame --cov-report=term-missing

# Coverage with HTML report
pytest --cov=CellFrame --cov-report=html:htmlcov

# Coverage with XML report (for CI)
pytest --cov=CellFrame --cov-report=xml

# Fail if coverage below threshold
pytest --cov=CellFrame --cov-fail-under=80

# Coverage for specific modules
pytest --cov=CellFrame.core --cov=CellFrame.chain

# Show coverage in browser
pytest --cov=CellFrame --cov-report=html:htmlcov
# Then: open htmlcov/index.html
"""

# =========================================
# DEBUG AND PROFILING
# =========================================

"""
# Debug mode (don't capture output)
pytest -s

# Drop into debugger on failure
pytest --pdb

# Drop into debugger on first failure
pytest --pdb -x

# Use specific debugger
pytest --pdbcls=IPython.terminal.debugger:TerminalPdb

# Profile test execution
pytest --profile

# Memory profiling
pytest --memprof

# Time individual test phases
pytest --durations=10

# Show slowest tests
pytest --durations=0
"""

# =========================================
# CI/CD INTEGRATION
# =========================================

"""
# CI-friendly output
pytest --tb=short --quiet

# JUnit XML for CI systems
pytest --junitxml=junit.xml

# Coverage for CI
pytest --cov=CellFrame --cov-report=xml --cov-report=term

# Parallel execution in CI
pytest -n auto

# Retry flaky tests (requires pytest-rerunfailures)
pytest --reruns 3

# Timeout for CI (requires pytest-timeout)
pytest --timeout=600

# Full CI command example
pytest \
    --tb=short \
    --quiet \
    --cov=CellFrame \
    --cov-report=xml \
    --cov-report=term \
    --junitxml=junit.xml \
    --timeout=600 \
    -n auto
"""

# =========================================
# INTEGRATION WITH DEVELOPMENT WORKFLOW
# =========================================

"""
# Watch mode (requires pytest-watch)
ptw tests/

# Watch specific directory
ptw tests/unit/

# Auto-run on file changes
pytest-watch --runner "pytest -x"

# Integration with VS Code
# Add to .vscode/settings.json:
{
    "python.testing.pytestEnabled": true,
    "python.testing.pytestArgs": [
        "tests"
    ],
    "python.testing.unittestEnabled": false
}

# Integration with PyCharm
# Run Configuration:
# - Script path: pytest
# - Parameters: tests/ -v
# - Environment variables: PYTHONPATH=.
"""

# =========================================
# PERFORMANCE TESTING COMMANDS
# =========================================

"""
# Run only performance tests
pytest -m performance

# Run load tests
pytest -m load

# Run stress tests (might take long)
pytest -m stress

# Run scalability tests
pytest -m scalability

# Performance tests with profiling
pytest -m performance --profile

# Memory profiling for performance tests
pytest -m "performance or load" --memprof
"""

# =========================================
# EXAMPLE TEST EXECUTION SCENARIOS
# =========================================

"""
# Development workflow:
1. Quick test while developing:
   pytest tests/unit/core/ -v

2. Test specific feature:
   pytest -k "wallet" -v

3. Run affected tests:
   pytest tests/unit/chain/ tests/integration/ -v

4. Pre-commit check:
   pytest -m "not slow" --cov=CellFrame --cov-fail-under=75

5. Full test suite:
   ./run_tests.sh

6. CI pipeline:
   pytest --tb=short --cov=CellFrame --cov-report=xml --junitxml=junit.xml -n auto

# Debugging workflow:
1. Find failing test:
   pytest --lf -v

2. Debug specific test:
   pytest tests/unit/core/test_cellframe_node.py::TestCellframeNode::test_node_creation -s --pdb

3. Profile slow test:
   pytest tests/load/test_performance_stress.py::TestLoadTesting::test_concurrent_node_creation --profile

# Release validation:
1. All unit and integration tests:
   pytest -m "unit or integration"

2. Regression tests:
   pytest -m regression

3. Performance validation:
   pytest -m "performance and not stress"

4. Security validation:
   pytest -m security

5. Full validation:
   ./run_coverage.sh && pytest -m "not (stress or slow)"
"""