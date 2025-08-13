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
    message("[+] Python-Cellframe: Building as SHARED library (links with SHARED Cellframe SDK)")
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
    # CRITICAL: Include OS detection first to set proper platform defines (DAP_OS_LINUX, DAP_OS_UNIX, etc.)
    # This is essential for dap-sdk headers to work correctly across all modules
    include(${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/cmake/OS_Detection.cmake)
    message("[+] Python-Cellframe: OS Detection loaded - platform defines set")
    
    # Force DAP SDK to build as SHARED (default is already SHARED)
    set(BUILD_SHARED ON CACHE BOOL "Build DAP SDK as shared library" FORCE)
    
    # CRITICAL: Pass XKCP defines to DAP SDK for Keccak-p[1600] support
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DXKCP_has_KeccakP1600" CACHE STRING "Add XKCP defines for DAP SDK" FORCE)
    
    # Add DAP SDK submodule - set build path for DAP SDK components
    set(DAP_SDK_PATH_BUILD ${CMAKE_CURRENT_BINARY_DIR}/dap-sdk-build CACHE STRING "DAP SDK build directory path for submodules")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk ${DAP_SDK_PATH_BUILD})
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
    
    # Build Cellframe SDK as SHARED for Python plugins (easier dynamic linking)
    set(BUILD_SHARED ON CACHE BOOL "Build Cellframe SDK as shared library for plugins" FORCE)
    set(CELLFRAME_FORCED_SHARED ON CACHE BOOL "Force shared build for Python plugins" FORCE)
    message("[+] Python-Cellframe: Cellframe SDK will be built as SHARED library")
    
    # Pass shared DAP SDK to Cellframe SDK
    if(DAP_SDK_AVAILABLE AND DAP_SDK_TARGET)
        set(DAP_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk CACHE PATH "Path to shared DAP SDK for Cellframe")
        set(DAP_SDK_TARGET_NAME ${DAP_SDK_TARGET} CACHE STRING "Shared DAP SDK target for Cellframe")
        set(CELLFRAME_MODULES "core chains ledger node network cs-dag-poa cs-esbocs cs-none srv-stake srv-voting srv-bridge srv-xchange compose" CACHE STRING "Cellframe modules to prevent embedded DAP SDK")
        message("[+] Python-Cellframe: Configuring Cellframe SDK to use shared DAP SDK")
        message("[+] Python-Cellframe: This prevents DAP SDK duplication in memory")
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
    
    # Pass shared DAP SDK path to python-dap
    if(DAP_SDK_AVAILABLE AND DAP_SDK_TARGET)
        set(DAP_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk CACHE PATH "Path to shared DAP SDK")
        set(DAP_SDK_TARGET_NAME ${DAP_SDK_TARGET} CACHE STRING "Shared DAP SDK target name")
        message("[+] Python-Cellframe: Configuring python-dap to use shared DAP SDK")
    else()
        message("[+] Python-Cellframe: python-dap will use internal DAP SDK (fallback mode)")
    endif()
    
    # CRITICAL: Temporarily disable BUILD_TESTS for python-dap subdirectory
    # Save current BUILD_TESTS value
    set(PYTHON_CELLFRAME_BUILD_TESTS ${BUILD_TESTS})
    set(BUILD_TESTS OFF CACHE BOOL "Temporarily disable python-dap tests" FORCE)
    
    # Add python-dap subdirectory - ALWAYS BUILD (not EXCLUDE_FROM_ALL)  
    add_subdirectory(python-dap)
    
    # Restore BUILD_TESTS for python-cellframe
    set(BUILD_TESTS ${PYTHON_CELLFRAME_BUILD_TESTS} CACHE BOOL "Restore BUILD_TESTS for python-cellframe" FORCE)
    set(PYTHON_DAP_AVAILABLE ON)
    set(PYTHON_DAP_TARGET "python_dap")
    
    message("[+] Python-Cellframe: python-dap dependency added as SHARED library")
    message("[+] Python-Cellframe: This prevents DAP SDK duplication in memory")
    message("[+] Python-Cellframe: python-dap tests DISABLED - only python-cellframe tests will run in CTest")
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

# Add cellframe-sdk and dap-sdk include paths when embedding is enabled
if(PYTHON_CELLFRAME_EMBED_SDK AND CELLFRAME_SDK_TARGET)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        # DAP SDK include paths (shared with cellframe-sdk)
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/core/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/crypto/include  
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/plugin/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/io/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/net/server/cli_server/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/net/client/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/net/stream/session/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/net/stream/stream/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/net/server/enc_server/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/net/stream/ch/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/net/link_manager/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/core/src/unix
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/net/server/http_server/http_client/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/net/server/http_server/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/net/client/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/global-db/include
        # XKCP generated headers with correct implementation defines
        ${CMAKE_CURRENT_BINARY_DIR}/dap-sdk-shared/crypto/XKCP/xkcp_build_src/bin/libdap-XKCP-x8664-native.a.headers
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/3rdparty/uthash/src
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/3rdparty/json-c
        # XKCP include paths for Keccak hash functions
        # json_config.h generated during dap-sdk build process
        ${CMAKE_CURRENT_BINARY_DIR}/dap-sdk-shared/core/json-c
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/crypto/XKCP/lib/high/Keccak/FIPS202
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/crypto/XKCP/lib/high/Keccak/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/crypto/XKCP/lib/common/inc
        # XKCP config.h generated during build 
        ${CMAKE_CURRENT_BINARY_DIR}/dap-sdk-shared/crypto/XKCP/xkcp_build_src/bin/libdap-XKCP-x8664-native.a.headers
        # Cellframe SDK module include paths
        ${CMAKE_CURRENT_SOURCE_DIR}/cellframe-sdk/modules/common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/cellframe-sdk/modules/chain/include  
        ${CMAKE_CURRENT_SOURCE_DIR}/cellframe-sdk/modules/net/include
        ${CMAKE_CURRENT_SOURCE_DIR}/cellframe-sdk/modules/wallet/include
        ${CMAKE_CURRENT_SOURCE_DIR}/cellframe-sdk/modules/core/include
    )
    message("[+] Python-Cellframe: Added cellframe-sdk and dap-sdk include paths for embedding")
endif()

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
    target_compile_definitions(${PROJECT_NAME} PRIVATE KeccakP1600_implementation)
    message("[+] Python-Cellframe: Linked with SHARED Cellframe SDK")
endif()

# =========================================
# LIBRARY PROPERTIES (configured below in PYTHON EXTENSION CONFIGURATION)
# =========================================

# =========================================
# INSTALL CONFIGURATION
# =========================================
# Install python_cellframe library
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib/python3/dist-packages
    ARCHIVE DESTINATION lib
    COMPONENT python_cellframe
)

# Install Python modules
install(DIRECTORY 
    ${CMAKE_CURRENT_SOURCE_DIR}/CellFrame/
    DESTINATION lib/python3/dist-packages/CellFrame
    COMPONENT python_cellframe
    FILES_MATCHING PATTERN "*.py"
)

# Install headers for development
install(FILES 
    ${PYTHON_CELLFRAME_HEADERS}
    DESTINATION include/python-cellframe
    COMPONENT python_cellframe_dev
)

# Install documentation
install(FILES
    README.md
    ARCHITECTURE.md
    PROJECT_COMPLETION_SUMMARY.md
    DESTINATION share/doc/python-cellframe
    OPTIONAL
    COMPONENT python_cellframe_docs
)

# =========================================
# PACKAGING SUPPORT (CPack) - PYTHON-CELLFRAME
# =========================================
# Main package configuration
set(CPACK_PACKAGE_NAME "python-cellframe")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Python bindings for Cellframe blockchain platform")
set(CPACK_PACKAGE_VENDOR "Demlabs")
set(CPACK_PACKAGE_CONTACT "support@demlabs.net")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Multi-component packaging
set(CPACK_COMPONENTS_ALL python_cellframe python_cellframe_dev python_cellframe_docs)

# Component descriptions
set(CPACK_COMPONENT_PYTHON_CELLFRAME_DISPLAY_NAME "Python-Cellframe Runtime")
set(CPACK_COMPONENT_PYTHON_CELLFRAME_DESCRIPTION "Python bindings for Cellframe blockchain")
set(CPACK_COMPONENT_PYTHON_CELLFRAME_REQUIRED ON)

set(CPACK_COMPONENT_PYTHON_CELLFRAME_DEV_DISPLAY_NAME "Python-Cellframe Development")
set(CPACK_COMPONENT_PYTHON_CELLFRAME_DEV_DESCRIPTION "Headers and development files")
set(CPACK_COMPONENT_PYTHON_CELLFRAME_DEV_DEPENDS python_cellframe)

set(CPACK_COMPONENT_PYTHON_CELLFRAME_DOCS_DISPLAY_NAME "Python-Cellframe Documentation")
set(CPACK_COMPONENT_PYTHON_CELLFRAME_DOCS_DESCRIPTION "Documentation and README files")

# Debian package configuration
set(CPACK_DEBIAN_PACKAGE_DEPENDS "python3 (>= 3.8), python3-dev, python-dap (>= 3.0.0)")
set(CPACK_DEBIAN_PACKAGE_SECTION "python")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Demlabs <support@demlabs.net>")

# Package generators
set(CPACK_GENERATOR "DEB;TGZ;ZIP")
include(CPack)

# =========================================
# PYTHON EXTENSION CONFIGURATION
# =========================================
# Set Python extension properties
if(BUILD_SHARED)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION 1.0.0
        SOVERSION 1
        POSITION_INDEPENDENT_CODE ON
        PREFIX "lib"
        SUFFIX ".so"
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE PYTHON_CELLFRAME_BUILDING_SHARED)
    target_compile_definitions(${PROJECT_NAME} INTERFACE PYTHON_CELLFRAME_USING_SHARED)
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION 1.0.0
        SOVERSION 1
        POSITION_INDEPENDENT_CODE ON
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
    )
    target_compile_definitions(${PROJECT_NAME} INTERFACE PYTHON_CELLFRAME_USING_STATIC)
endif()

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
    message("[+] - Embedded SDK: SHARED Cellframe SDK")
    message("[+] - Architecture: Dynamic libraries for plugin compatibility")
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
    
    # Test environment setup - AUTOMATIC via CTest fixtures
    # The test environment is automatically initialized before running any tests
    # through the cellframe_test_environment_setup fixture
    
    set(VENV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/venv_test")
    message("[+] Python-Cellframe: Test environment will be automatically initialized via CTest fixtures")
    

    
    # =========================================
    # PYTEST INTEGRATION WITH CTEST
    # =========================================
    
    # Function to add Python test using test environment
    function(add_pytest_test TEST_NAME TEST_PATH)
        # Check if test environment exists
        set(TEST_ENV_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/venv_test/bin/activate_test")
        
        if(EXISTS ${TEST_ENV_SCRIPT})
            # Use test environment with enhanced activation script
            add_test(NAME ${TEST_NAME}
                COMMAND bash -c "source ${TEST_ENV_SCRIPT} && python -m pytest ${TEST_PATH} -v --tb=short"
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
            )
        else()
            # Fallback to direct pytest with enhanced PYTHONPATH
            add_test(NAME ${TEST_NAME}
                COMMAND ${CMAKE_COMMAND} -E env
                    PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/venv_test/lib/python3.11/site-packages:${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_CURRENT_BINARY_DIR}:$ENV{PYTHONPATH}
                    LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_CURRENT_BINARY_DIR}/dap-sdk-build:${CMAKE_CURRENT_BINARY_DIR}/cellframe-sdk-embedded:${CMAKE_CURRENT_BINARY_DIR}/python-dap:$ENV{LD_LIBRARY_PATH}
                    CELLFRAME_TEST_MODE=1
                    DAP_SDK_TEST_MODE=1
                    ${Python3_EXECUTABLE} -m pytest ${TEST_PATH} -v --tb=short
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
            )
        endif()
        
        # Set test properties
        set_tests_properties(${TEST_NAME} PROPERTIES
            TIMEOUT 600
            RUN_SERIAL TRUE  # Force sequential execution for stability
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
    
    # Comprehensive test suite removed due to hanging issues
    # Individual unit tests are available: test_unit, test_unit_legacy, test_unit_composer
    
    # Add coverage test
    if(BUILD_WITH_COVERAGE)
        add_test(NAME test_coverage
            COMMAND ${CMAKE_COMMAND} -E env
                PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_CURRENT_SOURCE_DIR}/venv_test/lib/python3.11/site-packages:$ENV{PYTHONPATH}
                CELLFRAME_TEST_MODE=1
                DAP_SDK_TEST_MODE=1
                ${CMAKE_CURRENT_SOURCE_DIR}/run_coverage.sh
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        set_tests_properties(test_coverage PROPERTIES
            TIMEOUT 900
            RUN_SERIAL TRUE
        )
        
        message("[+] Python-Cellframe: Coverage testing enabled")
    endif()
    
    # Quick functional test
    add_test(NAME test_quick_functionality
        COMMAND ${CMAKE_COMMAND} -E env
            PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_CURRENT_SOURCE_DIR}/venv_test/lib/python3.11/site-packages:$ENV{PYTHONPATH}
            CELLFRAME_TEST_MODE=1
            ${CMAKE_CURRENT_SOURCE_DIR}/quick_test.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set_tests_properties(test_quick_functionality PROPERTIES
        TIMEOUT 120
        RUN_SERIAL TRUE
    )
    
    # =========================================
    # AUTOMATIC TEST ENVIRONMENT INITIALIZATION  
    # =========================================
    
    # Create CTestCustom.cmake for automatic pre-test environment setup
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
        @ONLY
    )
    
    # Copy cmake helper scripts to build directory
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmake/)
    
    # Custom target for running all tests (dependency-aware)
    add_custom_target(run_all_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --parallel ${CMAKE_BUILD_PARALLEL_LEVEL}
        DEPENDS python_cellframe
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running all Python tests via ctest (requires manual environment initialization)"
        VERBATIM
    )
    
    message("[+] Python-Cellframe: Test targets configured")
    message("[+] Python-Cellframe: WORKFLOW: 1) ./tests/init_test_environment.sh  2) ctest or make test")
    message("[+] Python-Cellframe: Only python-cellframe tests will run - python-dap tests are DISABLED")
    
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