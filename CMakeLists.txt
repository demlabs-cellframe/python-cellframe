cmake_minimum_required(VERSION 3.10)
project(python_cellframe)

# =========================================
# UNIVERSAL BUILD SYSTEM - PYTHON CELLFRAME
# =========================================
# Conditional SDK embedding architecture:
# - If BUILD_SHARED=ON → embed static Cellframe SDK  
# - If BUILD_SHARED=OFF → no SDK embedding (for plugin integration)

# Default to shared library
option(BUILD_SHARED "Build python_cellframe as shared library" ON)

if(BUILD_SHARED)
    set(PYTHON_CELLFRAME_LIB_TYPE SHARED)
    set(PYTHON_CELLFRAME_EMBED_SDK ON)
    set(PYTHON_CELLFRAME_BUILD_MODE "dynamic")
    message("[+] Python-Cellframe: Building as SHARED library (embeds static Cellframe SDK)")
else()
    set(PYTHON_CELLFRAME_LIB_TYPE STATIC)
    set(PYTHON_CELLFRAME_EMBED_SDK OFF)
    set(PYTHON_CELLFRAME_BUILD_MODE "static") 
    message("[+] Python-Cellframe: Building as STATIC library (NO SDK embedding)")
endif()

# Export build mode for consumers
set(PYTHON_CELLFRAME_BUILD_MODE ${PYTHON_CELLFRAME_BUILD_MODE} CACHE STRING "Python-Cellframe build mode")
set(PYTHON_CELLFRAME_EMBED_SDK ${PYTHON_CELLFRAME_EMBED_SDK} CACHE BOOL "Whether to embed Cellframe SDK")

# Find Python3
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# =========================================
# DAP SDK SHARED LIBRARY INTEGRATION
# =========================================
# Build DAP SDK as shared library (foundation for all components)
message("[+] Python-Cellframe: Adding DAP SDK as shared library...")

# Check for dap-sdk submodule  
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/CMakeLists.txt)
    # Force DAP SDK to build as SHARED (default is already SHARED)
    set(BUILD_SHARED ON CACHE BOOL "Build DAP SDK as shared library" FORCE)
    
    # Set build path for DAP SDK components (used by json-c and other internal modules)
    set(DAP_SDK_PATH_BUILD ${CMAKE_CURRENT_BINARY_DIR}/dap-sdk-shared CACHE STRING "Build path for DAP SDK")
    
    # Pass the build path to dap-sdk for internal use (e.g., json-c include paths)
    set(DAP_SDK_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/dap-sdk-shared CACHE STRING "DAP SDK binary directory for submodules")
    
    # Add DAP SDK submodule
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk ${CMAKE_CURRENT_BINARY_DIR}/dap-sdk-shared)
    set(DAP_SDK_TARGET "dap_sdk")
    set(DAP_SDK_AVAILABLE ON)
    
    message("[+] Python-Cellframe: DAP SDK added as shared library")
    message("[+] Python-Cellframe: This provides single DAP SDK instance for all components")
else()
    message(WARNING "Python-Cellframe: dap-sdk submodule not found, falling back to system packages")
    set(DAP_SDK_AVAILABLE OFF)
    set(DAP_SDK_TARGET "")
endif()

# =========================================
# CONDITIONAL CELLFRAME SDK INTEGRATION
# =========================================
if(PYTHON_CELLFRAME_EMBED_SDK)
    message("[+] Python-Cellframe: Enabling Cellframe SDK embedding...")
    
    # Build Cellframe SDK as static for embedding
    set(BUILD_SHARED OFF CACHE BOOL "Force Cellframe SDK static for embedding" FORCE)
    set(CELLFRAME_FORCED_STATIC ON CACHE BOOL "Force static build" FORCE)
    
    # Pass shared DAP SDK to Cellframe SDK
    if(DAP_SDK_AVAILABLE AND DAP_SDK_TARGET)
        set(DAP_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk CACHE PATH "Path to shared DAP SDK for Cellframe")
        set(DAP_SDK_TARGET_NAME ${DAP_SDK_TARGET} CACHE STRING "Shared DAP SDK target for Cellframe")
        message("[+] Python-Cellframe: Configuring Cellframe SDK to use shared DAP SDK")
    else()
        message(WARNING "Python-Cellframe: No shared DAP SDK available for Cellframe SDK")
    endif()
    
    # Add Cellframe SDK submodule
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cellframe-sdk/CMakeLists.txt)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cellframe-sdk ${CMAKE_CURRENT_BINARY_DIR}/cellframe-sdk-embedded)
        set(CELLFRAME_SDK_TARGET "cellframe_sdk")
        message("[+] Python-Cellframe: Cellframe SDK embedded from submodule")
    else()
        message(WARNING "Python-Cellframe: Cellframe SDK submodule not found, embedding disabled")
        set(PYTHON_CELLFRAME_EMBED_SDK OFF)
    endif()
else()
    message("[+] Python-Cellframe: SDK embedding disabled (static mode for plugin integration)")
    set(CELLFRAME_SDK_TARGET "")
endif()

# =========================================
# SOURCE FILES
# =========================================
# C extension sources for Cellframe integration
set(PYTHON_CELLFRAME_SOURCES
    src/python_cellframe_module.c
    src/cellframe_chain.c
    src/cellframe_node.c
    src/cellframe_wallet.c
    src/cellframe_network.c
)

set(PYTHON_CELLFRAME_HEADERS
    include/python_cellframe.h
    include/cellframe_chain.h
    include/cellframe_node.h
    include/cellframe_wallet.h
    include/cellframe_network.h
)

# =========================================
# PYTHON-DAP DEPENDENCY (ALWAYS REQUIRED TO AVOID DAP SDK DUPLICATION)
# =========================================
# Check for python-dap dependency
set(PYTHON_DAP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/python-dap")
if(EXISTS "${PYTHON_DAP_DIR}/CMakeLists.txt")
    message("[+] Python-Cellframe: Adding python-dap as SHARED DEPENDENCY (avoiding DAP SDK duplication)")
    
    # Configure python-dap build - SHARED to avoid DAP SDK duplication
    set(BUILD_SHARED ON CACHE BOOL "Build python-dap as shared to avoid DAP SDK duplication" FORCE)
    set(PYTHON_DAP_BUILD_TESTS OFF CACHE BOOL "Don't build python-dap tests" FORCE)
    
    # Pass shared DAP SDK path to python-dap
    if(DAP_SDK_AVAILABLE AND DAP_SDK_TARGET)
        set(DAP_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk CACHE PATH "Path to shared DAP SDK")
        set(DAP_SDK_TARGET_NAME ${DAP_SDK_TARGET} CACHE STRING "Shared DAP SDK target name")
        message("[+] Python-Cellframe: Configuring python-dap to use shared DAP SDK")
    else()
        message("[+] Python-Cellframe: python-dap will use internal DAP SDK (fallback mode)")
    endif()
    
    # Add python-dap subdirectory - ALWAYS BUILD (not EXCLUDE_FROM_ALL)
    add_subdirectory(python-dap)
    set(PYTHON_DAP_AVAILABLE ON)
    set(PYTHON_DAP_TARGET "python_dap")
    
    message("[+] Python-Cellframe: python-dap dependency added as SHARED library")
    message("[+] Python-Cellframe: This prevents DAP SDK duplication in memory")
else()
    message(FATAL_ERROR "Python-Cellframe: python-dap REQUIRED but not found at ${PYTHON_DAP_DIR}")
    set(PYTHON_DAP_AVAILABLE OFF)
endif()

# Create main library
add_library(${PROJECT_NAME} ${PYTHON_CELLFRAME_LIB_TYPE} ${PYTHON_CELLFRAME_SOURCES} ${PYTHON_CELLFRAME_HEADERS})

# =========================================
# INCLUDE DIRECTORIES
# =========================================
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Python3_INCLUDE_DIRS}
)

# =========================================
# LINKING AND DEPENDENCIES
# =========================================
# Always link with Python
target_link_libraries(${PROJECT_NAME} PRIVATE ${Python3_LIBRARIES})

# Link with python-dap to avoid DAP SDK duplication
if(PYTHON_DAP_AVAILABLE AND PYTHON_DAP_TARGET)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PYTHON_DAP_TARGET})
    target_compile_definitions(${PROJECT_NAME} PRIVATE PYTHON_DAP_LINKED)
    message("[+] Python-Cellframe: Linked with python-dap (shared DAP SDK)")
endif()

# Conditional Cellframe SDK linking
if(PYTHON_CELLFRAME_EMBED_SDK AND CELLFRAME_SDK_TARGET)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CELLFRAME_SDK_TARGET})
    target_compile_definitions(${PROJECT_NAME} PRIVATE CELLFRAME_SDK_EMBEDDED)
    message("[+] Python-Cellframe: Linked with static Cellframe SDK")
endif()

# =========================================
# LIBRARY PROPERTIES
# =========================================
if(BUILD_SHARED)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        VERSION 2.0.0
        SOVERSION 2
        PREFIX "lib"
        SUFFIX ".so"
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE PYTHON_CELLFRAME_BUILDING_SHARED)
    target_compile_definitions(${PROJECT_NAME} INTERFACE PYTHON_CELLFRAME_USING_SHARED)
else()
    target_compile_definitions(${PROJECT_NAME} INTERFACE PYTHON_CELLFRAME_USING_STATIC)
endif()

# =========================================
# PYTHON EXTENSION CONFIGURATION
# =========================================
# Set Python extension properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

# Platform-specific configuration
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".so")
endif()

# =========================================
# ARCHITECTURE SUMMARY
# =========================================
message("[+] ===========================================")
message("[+] PYTHON-CELLFRAME BUILD ARCHITECTURE:")
message("[+] - Library type: ${PYTHON_CELLFRAME_LIB_TYPE}")
message("[+] - Build mode: ${PYTHON_CELLFRAME_BUILD_MODE}")
message("[+] - SDK embedding: ${PYTHON_CELLFRAME_EMBED_SDK}")
if(PYTHON_CELLFRAME_EMBED_SDK)
    message("[+] - Embedded SDK: static Cellframe SDK")
else()
    message("[+] - Embedded SDK: none (for plugin integration)")
endif()
message("[+] - Python version: ${Python3_VERSION}")
message("[+] ===========================================")

# =========================================
# TESTING INTEGRATION
# =========================================
option(BUILD_TESTS "Build and run tests" ON)
option(BUILD_WITH_COVERAGE "Build with coverage support" OFF)
option(SETUP_VENV "Setup virtual environment for tests" ON)

# Enable testing if requested
if(BUILD_TESTS)
    enable_testing()
    message("[+] Python-Cellframe: Testing enabled")
    
    # Setup virtual environment for testing
    if(SETUP_VENV)
        set(VENV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/venv_test")
        
        # Custom target to setup test environment
        add_custom_target(setup_test_env
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/setup_test_env.sh
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Setting up Python test environment"
            VERBATIM
        )
        
        message("[+] Python-Cellframe: Virtual environment setup target created")
    endif()
    

    
    # =========================================
    # PYTEST INTEGRATION WITH CTEST
    # =========================================
    
    # Function to add Python test
    function(add_pytest_test TEST_NAME TEST_PATH)
        add_test(NAME ${TEST_NAME}
            COMMAND ${CMAKE_COMMAND} -E env
                PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}:$ENV{PYTHONPATH}
                CELLFRAME_TEST_MODE=1
                DAP_SDK_TEST_MODE=1
                ${Python3_EXECUTABLE} -m pytest ${TEST_PATH} -v
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
        )
        
        # Set test properties
        set_tests_properties(${TEST_NAME} PROPERTIES
            TIMEOUT 300
            ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}:$ENV{PYTHONPATH}"
        )
        
        # Add dependency on python_cellframe target
        if(TARGET python_cellframe)
            set_tests_properties(${TEST_NAME} PROPERTIES
                DEPENDS python_cellframe
            )
        endif()
    endfunction()
    
    # Add test categories
    add_pytest_test(test_unit "unit/")
    add_pytest_test(test_unit_legacy "unit/legacy/")
    add_pytest_test(test_unit_composer "unit/composer/")
    
    # Add comprehensive test suite
    add_test(NAME test_all_python
        COMMAND ${CMAKE_COMMAND} -E env
            PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}:$ENV{PYTHONPATH}
            CELLFRAME_TEST_MODE=1
            DAP_SDK_TEST_MODE=1
            ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Add coverage test
    if(BUILD_WITH_COVERAGE)
        add_test(NAME test_coverage
            COMMAND ${CMAKE_COMMAND} -E env
                PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}:$ENV{PYTHONPATH}
                CELLFRAME_TEST_MODE=1
                DAP_SDK_TEST_MODE=1
                ${CMAKE_CURRENT_SOURCE_DIR}/run_coverage.sh
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        
        message("[+] Python-Cellframe: Coverage testing enabled")
    endif()
    
    # Quick functional test
    add_test(NAME test_quick_functionality
        COMMAND ${CMAKE_COMMAND} -E env
            PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}:$ENV{PYTHONPATH}
            CELLFRAME_TEST_MODE=1
            ${CMAKE_CURRENT_SOURCE_DIR}/quick_test.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Custom target for running all tests
    add_custom_target(run_all_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --parallel ${CMAKE_BUILD_PARALLEL_LEVEL}
        DEPENDS setup_test_env
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running all Python tests via ctest"
        VERBATIM
    )
    
    message("[+] Python-Cellframe: Test targets configured - use 'make test' or 'make run_all_tests'")
    
endif()

# =========================================
# INSTALLATION
# =========================================
if(INSTALL_PYTHON_CELLFRAME)
    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib/python/site-packages
        ARCHIVE DESTINATION lib/python/site-packages
        RUNTIME DESTINATION bin
    )
    
    install(FILES ${PYTHON_CELLFRAME_HEADERS}
        DESTINATION include/python-cellframe
    )
endif() 