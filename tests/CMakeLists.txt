cmake_minimum_required(VERSION 3.10)
project(python_cellframe_tests)

# =========================================
# Python-Cellframe Tests Configuration
# =========================================
# This project uses pytest for testing, NOT C tests
# Tests are Python-based and located in:
#   - tests/unit/         (unit tests)
#   - tests/integration/  (integration tests)
#   - tests/functional/   (functional tests)
#   - tests/e2e/          (end-to-end tests)
#   - tests/governance/   (governance tests)
#   - tests/security/     (security tests)
#   - tests/load/         (performance/stress tests)
#   - tests/regression/   (backward compatibility tests)

message(STATUS "=========================================")
message(STATUS "Python-Cellframe Tests Configuration")
message(STATUS "=========================================")

# Find Python for running tests
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Check if pytest is available
execute_process(
    COMMAND ${Python3_EXECUTABLE} -m pytest --version
    RESULT_VARIABLE PYTEST_AVAILABLE
    OUTPUT_QUIET
    ERROR_QUIET
)

if(PYTEST_AVAILABLE EQUAL 0)
    message(STATUS "pytest found: ${Python3_EXECUTABLE} -m pytest")
    
    # Add custom target to run Python tests
    add_custom_target(pytest_tests
        COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR} -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running Python-Cellframe pytest tests"
    )
    
    # Add individual test targets for each category
    add_custom_target(pytest_unit
        COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/unit -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running Python-Cellframe unit tests"
    )
    
    add_custom_target(pytest_integration
        COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/integration -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running Python-Cellframe integration tests"
    )
    
    add_custom_target(pytest_governance
        COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/governance -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running Python-Cellframe governance tests"
    )
    
    # Add to CTest
    enable_testing()
    
    # Set PYTHONPATH to include build directories
    # Note: python-dap builds to python-dap/lib/ not build/python-dap/lib/
    set(TEST_PYTHONPATH 
        "${CMAKE_SOURCE_DIR}/python-dap/lib:${CMAKE_SOURCE_DIR}/python-dap/src:${CMAKE_BINARY_DIR}/lib:${CMAKE_SOURCE_DIR}/src"
    )
    
    # Set LD_LIBRARY_PATH to include native libraries
    set(TEST_LD_LIBRARY_PATH
        "${CMAKE_BINARY_DIR}/dap-sdk:${CMAKE_BINARY_DIR}/cellframe-sdk:${CMAKE_SOURCE_DIR}/python-dap/lib"
    )
    
    
    add_test(
        NAME python_cellframe_unit_tests
        COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/unit -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set_tests_properties(python_cellframe_unit_tests PROPERTIES
        ENVIRONMENT "PYTHONPATH=${TEST_PYTHONPATH};LD_LIBRARY_PATH=${TEST_LD_LIBRARY_PATH}"
    )
    
    add_test(
        NAME python_cellframe_integration_tests
        COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/integration -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set_tests_properties(python_cellframe_integration_tests PROPERTIES
        ENVIRONMENT "PYTHONPATH=${TEST_PYTHONPATH};LD_LIBRARY_PATH=${TEST_LD_LIBRARY_PATH}"
    )
    
    add_test(
        NAME python_cellframe_governance_tests
        COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/governance -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set_tests_properties(python_cellframe_governance_tests PROPERTIES
        ENVIRONMENT "PYTHONPATH=${TEST_PYTHONPATH};LD_LIBRARY_PATH=${TEST_LD_LIBRARY_PATH}"
    )
    
    message(STATUS "Python-Cellframe pytest tests configured")
    message(STATUS "Run with: make pytest_tests")
    message(STATUS "Or: ctest (runs all registered tests)")
    
else()
    message(WARNING "pytest not found - Python tests will not be available")
    message(WARNING "Install pytest: pip3 install pytest pytest-asyncio")
endif()

message(STATUS "=========================================")
