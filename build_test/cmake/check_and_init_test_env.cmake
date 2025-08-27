# check_and_init_test_env.cmake
# CMake script to check and initialize test environment before running CTest
# Called automatically by CTestCustom.cmake via CTEST_CUSTOM_PRE_TEST

message(STATUS "CTest Pre-Test Hook: Checking test environment...")

# Get parameters from command line
if(NOT DEFINED SOURCE_DIR)
    message(FATAL_ERROR "SOURCE_DIR not defined")
endif()

if(NOT DEFINED VENV_DIR)
    message(FATAL_ERROR "VENV_DIR not defined")
endif()

if(NOT DEFINED INIT_SCRIPT)
    message(FATAL_ERROR "INIT_SCRIPT not defined")
endif()

# Check if virtual environment exists and is properly initialized
set(VENV_PYTHON "${VENV_DIR}/bin/python")
set(VENV_ACTIVATE "${VENV_DIR}/bin/activate_test")

# Function to check if environment needs initialization
function(check_environment_status)
    set(NEEDS_INIT FALSE PARENT_SCOPE)
    
    # Check if venv directory exists
    if(NOT EXISTS "${VENV_DIR}")
        message(STATUS "  Virtual environment directory not found")
        set(NEEDS_INIT TRUE PARENT_SCOPE)
        return()
    endif()
    
    # Check if python executable exists
    if(NOT EXISTS "${VENV_PYTHON}")
        message(STATUS "  Virtual environment python not found")
        set(NEEDS_INIT TRUE PARENT_SCOPE)
        return()
    endif()
    
    # Check if activation script exists
    if(NOT EXISTS "${VENV_ACTIVATE}")
        message(STATUS "  Test activation script not found")
        set(NEEDS_INIT TRUE PARENT_SCOPE)
        return()
    endif()
    
    # Check if required packages are installed (quick test)
    execute_process(
        COMMAND "${VENV_PYTHON}" -c "import CellFrame; print('CellFrame OK')"
        RESULT_VARIABLE CELLFRAME_CHECK_RESULT
        OUTPUT_VARIABLE CELLFRAME_CHECK_OUTPUT
        ERROR_VARIABLE CELLFRAME_CHECK_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )
    
    if(NOT CELLFRAME_CHECK_RESULT EQUAL 0)
        message(STATUS "  CellFrame module not available in test environment")
        message(STATUS "    Error: ${CELLFRAME_CHECK_ERROR}")
        set(NEEDS_INIT TRUE PARENT_SCOPE)
        return()
    endif()
    
    # Check if pytest is available
    execute_process(
        COMMAND "${VENV_PYTHON}" -c "import pytest; print('pytest OK')"
        RESULT_VARIABLE PYTEST_CHECK_RESULT
        OUTPUT_QUIET
        ERROR_QUIET
    )
    
    if(NOT PYTEST_CHECK_RESULT EQUAL 0)
        message(STATUS "  pytest not available in test environment")
        set(NEEDS_INIT TRUE PARENT_SCOPE)
        return()
    endif()
    
    message(STATUS "  Test environment appears to be properly initialized")
    set(NEEDS_INIT FALSE PARENT_SCOPE)
endfunction()

# Check current environment status
check_environment_status()

if(NEEDS_INIT)
    message(STATUS "CTest Pre-Test Hook: Initializing test environment...")
    
    # Check if initialization script exists
    if(NOT EXISTS "${INIT_SCRIPT}")
        message(FATAL_ERROR "Initialization script not found: ${INIT_SCRIPT}")
    endif()
    
    # Run initialization script
    execute_process(
        COMMAND bash "${INIT_SCRIPT}"
        WORKING_DIRECTORY "${SOURCE_DIR}"
        RESULT_VARIABLE INIT_RESULT
        OUTPUT_VARIABLE INIT_OUTPUT
        ERROR_VARIABLE INIT_ERROR
    )
    
    if(NOT INIT_RESULT EQUAL 0)
        message(FATAL_ERROR "Test environment initialization failed!\n"
                           "Exit code: ${INIT_RESULT}\n"
                           "Output: ${INIT_OUTPUT}\n"
                           "Error: ${INIT_ERROR}")
    endif()
    
    message(STATUS "CTest Pre-Test Hook: Test environment initialized successfully")
    
    # Verify initialization worked
    check_environment_status()
    if(NEEDS_INIT)
        message(FATAL_ERROR "Test environment initialization completed but verification failed")
    endif()
else()
    message(STATUS "CTest Pre-Test Hook: Test environment already initialized - skipping")
endif()

message(STATUS "CTest Pre-Test Hook: Environment check completed successfully")