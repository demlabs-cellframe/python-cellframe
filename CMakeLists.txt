cmake_minimum_required(VERSION 3.10)
project(DAP_SDK_NATIVE C)

# Disable DAP SDK installation and packaging (we only package python-dap and python-cellframe)
set(INSTALL_DAP_SDK OFF CACHE BOOL "Disable DAP SDK packaging" FORCE)

# Disable json-c packaging explicitly
set(INSTALL_SDK OFF CACHE BOOL "Disable json-c installation" FORCE)

# Set global CPack maintainer (required for component-based DEB packages)
# This is needed for the base package name, but components have their own maintainers
set(CPACK_PACKAGE_CONTACT "support@demlabs.net")

# Отключение SSL по умолчанию - WolfSSL больше не используется
add_definitions(-DDAP_NET_CLIENT_NO_SSL)

set(DAP_SDK_NATIVE_VERSION "3.0-0")

# Build type detection
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Common includes and definitions
include(dap-sdk/cmake/OS_Detection.cmake)
include(dap-sdk/cmake/PackageComponents.cmake)
include(dap-sdk/cmake/ObjectLibraryHelpers.cmake)
include(dap-sdk/cmake/DapLibrary.cmake)

# =========================================
# INTERNAL MODULE CREATION MACRO
# =========================================
# Creates internal OBJECT modules (always OBJECT for combining)
# Usage: dap_add_library(target_name sources... HEADERS headers...)
macro(dap_add_library TARGET_NAME)
    cmake_parse_arguments(DAP_LIB "" "" "HEADERS" ${ARGN})
    
    # Always create modules as OBJECT for combining into final library
    add_library(${TARGET_NAME} OBJECT ${DAP_LIB_UNPARSED_ARGUMENTS} ${DAP_LIB_HEADERS})
    
    # Track modules for final library creation
    if(NOT DEFINED DAP_INTERNAL_MODULES)
        set(DAP_INTERNAL_MODULES "")
    endif()
    list(APPEND DAP_INTERNAL_MODULES ${TARGET_NAME})
    set(DAP_INTERNAL_MODULES ${DAP_INTERNAL_MODULES} CACHE INTERNAL "List of DAP internal modules")
    
    message("[+] DAP Internal Module: ${TARGET_NAME} (object)")
endmacro()

# =========================================
# FINAL LIBRARY CREATION
# =========================================
# Creates final combined DAP SDK library from OBJECT modules
macro(dap_create_final_library)
    if(DEFINED DAP_INTERNAL_MODULES)
        message("[+] Creating final DAP SDK library: dap_sdk")
        message("[+] Internal OBJECT modules to combine: ${DAP_INTERNAL_MODULES}")
        
        # Create object file lists for each module
        set(DAP_ALL_OBJECTS "")
        foreach(MODULE ${DAP_INTERNAL_MODULES})
            list(APPEND DAP_ALL_OBJECTS $<TARGET_OBJECTS:${MODULE}>)
        endforeach()
        
        # Create final library
        add_library(dap_sdk ${DAP_FINAL_LIB_TYPE} ${DAP_ALL_OBJECTS})
        
        # Aggregate include directories from all OBJECT modules
        # For OBJECT libraries, use INTERFACE_INCLUDE_DIRECTORIES (CMake 3.12+) or INCLUDE_DIRECTORIES
        set(DAP_AGGREGATED_INCLUDES "")
        foreach(MODULE ${DAP_INTERNAL_MODULES})
            # Try INTERFACE_INCLUDE_DIRECTORIES first (for proper PUBLIC includes)
            get_target_property(MODULE_INCS ${MODULE} INTERFACE_INCLUDE_DIRECTORIES)
            if(NOT MODULE_INCS OR MODULE_INCS STREQUAL "MODULE_INCS-NOTFOUND")
                # Fallback: get INCLUDE_DIRECTORIES for OBJECT libraries
                get_target_property(MODULE_INCS ${MODULE} INCLUDE_DIRECTORIES)
            endif()
            
            if(MODULE_INCS AND NOT MODULE_INCS STREQUAL "MODULE_INCS-NOTFOUND")
                list(APPEND DAP_AGGREGATED_INCLUDES ${MODULE_INCS})
            endif()
        endforeach()
        
        # Remove duplicates and export to consumers
        if(DAP_AGGREGATED_INCLUDES)
            list(REMOVE_DUPLICATES DAP_AGGREGATED_INCLUDES)
            target_include_directories(dap_sdk INTERFACE ${DAP_AGGREGATED_INCLUDES})
            list(LENGTH DAP_AGGREGATED_INCLUDES INC_COUNT)
            message("[+] DAP SDK: Exported ${INC_COUNT} unique include directories from ${CMAKE_MATCH_COUNT} modules")
        else()
            message("[!] DAP SDK: No include directories found in modules!")
        endif()
        
        # Link with external dependencies
        target_link_libraries(dap_sdk PUBLIC 
            dap_json-c 
            dap_crypto_XKCP 
            dap_crypto_kyber512
            pthread 
            m
        )
        
        message("[+] Final DAP SDK Library: dap_sdk created from real object files with dependencies")
    else()
        message("[!] No DAP internal modules found!")
    endif()
endmacro()

# Options
option(BUILD_SHARED "Build final DAP SDK library as shared (default)" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TESTS "Build tests" OFF)

# Apply ENABLE_TESTING define globally BEFORE adding subdirectories
# This ensures all modules can use test-specific code when BUILD_TESTS is ON
if(BUILD_TESTS OR BUILD_WITH_TEST)
    add_definitions(-DENABLE_TESTING)
    message("[+] ENABLE_TESTING define enabled globally for DAP SDK modules")
endif()

# Internal modules are ALWAYS static, final can be SHARED or STATIC
if(BUILD_SHARED)
    message("[+] DAP SDK: Building final library as SHARED (modules: static internally)")
    set(DAP_FINAL_LIB_TYPE SHARED)
    set(DAP_BUILD_MODE "shared")
else()
    message("[+] DAP SDK: Building final library as STATIC (modules: static internally)")
    set(DAP_FINAL_LIB_TYPE STATIC)
    set(DAP_BUILD_MODE "static")
endif()

# Set default modules if none are specified
if (NOT DEFINED DAPSDK_MODULES OR DAPSDK_MODULES STREQUAL "")
    set(DAPSDK_MODULES "crypto plugin io network-core network-server network-client network-link_manager global-db")
    message("[+] Using default modules: ${DAPSDK_MODULES}")
endif()

# =========================================
# MODULE INCLUDES (AFTER MACRO DEFINITIONS)
# =========================================

# Debug: Check if macro is defined before using it
if(COMMAND dap_add_library)
    message("[DEBUG] dap_add_library macro is DEFINED")
else()
    message("[ERROR] dap_add_library macro is NOT DEFINED!")
endif()

# Disable DAP SDK installation/packaging - we only package python-dap and python-cellframe
# dap-sdk components should NOT be packaged separately
set(INSTALL_DAP_SDK OFF CACHE BOOL "Disable DAP SDK packaging" FORCE)

# Core is always required - UPDATED PATH
add_subdirectory(dap-sdk/module/core)

# Optional modules with UPDATED PATHS
if (DAPSDK_MODULES MATCHES "crypto")
    add_subdirectory(dap-sdk/module/crypto)
    message("[+] Module 'crypto'")
endif()

if (DAPSDK_MODULES MATCHES "plugin")
    add_subdirectory(dap-sdk/module/plugin)
    message("[+] Module 'plugin'")
endif()

if (DAPSDK_MODULES MATCHES "io")
    add_subdirectory(dap-sdk/module/io)
    message("[+] Module 'io'")
endif()

# Network common types (required for all network modules)
add_subdirectory(dap-sdk/module/net/common)

# HTTP client (standalone, used by json_rpc and other modules)
add_subdirectory(dap-sdk/module/net/client_http)

# App CLI (application command-line interface utilities)
add_subdirectory(dap-sdk/module/net/app-cli)

if (DAPSDK_MODULES MATCHES "network-core")
    add_subdirectory(dap-sdk/module/net/stream)
    message("[+] Module 'stream'")
endif()

if (DAPSDK_MODULES MATCHES "network-link_manager")
    add_subdirectory(dap-sdk/module/net/link_manager)
    message("[+] Module 'link manager'")
endif()

if (DAPSDK_MODULES MATCHES "network-client")
    add_subdirectory(dap-sdk/module/net/client)
    message("[+] Module 'client'")
endif()

if (DAPSDK_MODULES MATCHES "network-server")
    add_subdirectory(dap-sdk/module/net/server)
    message("[+] Module 'server'")
endif()

if (DAPSDK_MODULES MATCHES "global-db")
    add_subdirectory(dap-sdk/module/global-db)
    message("[+] Module 'global-db'")
endif()

if (DAPSDK_MODULES MATCHES "avrestream")
    add_subdirectory(dap-sdk/module/avrestream)
    message("[+] Module 'av-restream'")
endif()

# Examples - UPDATED PATH
if(BUILD_EXAMPLES)
    add_subdirectory(dap-sdk/module/examples)
    message("[+] Module 'examples' (enabled via -DBUILD_EXAMPLES=ON)")
endif()

# Test framework - DAP SDK test module
# Must be added before tests if BUILD_TESTS is enabled
if (BUILD_WITH_TEST OR BUILD_TESTS)
    enable_testing()
    add_subdirectory(dap-sdk/module/test)
    message("[+] DAP test framework module")
endif()

# DAP SDK C tests
if(BUILD_TESTS OR BUILD_WITH_TEST)
    enable_testing()
    add_subdirectory(dap-sdk/tests)
    message("[+] DAP SDK C tests enabled")
endif()

# Python tests structure
if(BUILD_TESTS OR BUILD_WITH_TEST)
    add_subdirectory(tests)
    message("[+] Python tests enabled")
endif()

# Add DAP SDK main initialization file
add_library(dap_sdk_init OBJECT dap-sdk/dap_sdk.c)
target_include_directories(dap_sdk_init PRIVATE
    dap-sdk/module/core/include
    dap-sdk/module/crypto/include
    dap-sdk/module/io/include
)
target_link_libraries(dap_sdk_init PUBLIC dap_core)
list(APPEND DAP_INTERNAL_MODULES dap_sdk_init)
set(DAP_INTERNAL_MODULES ${DAP_INTERNAL_MODULES} CACHE INTERNAL "List of DAP internal modules")
message("[+] DAP SDK init module added")

# Create final library from all modules
dap_create_final_library()

message("[+] =========================================")
message("[+] DAP SDK with module/ structure configured")
message("[+] =========================================")

# =========================================
# CELLFRAME SDK
# =========================================
# Cellframe SDK depends on DAP SDK
message("[+] =========================================")
message("[+] Configuring Cellframe SDK (depends on DAP SDK)")
message("[+] =========================================")

# Set DAP SDK path for Cellframe SDK
set(DAP_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk CACHE PATH "Path to DAP SDK")

# Build Cellframe SDK as shared library
set(BUILD_SHARED ON CACHE BOOL "Build Cellframe SDK as shared library" FORCE)
add_subdirectory(cellframe-sdk)

message("[+] Cellframe SDK configured")

# =========================================
# PYTHON-DAP BINDINGS
# =========================================
# Python-DAP depends on DAP SDK
message("[+] =========================================")
message("[+] Configuring Python-DAP bindings (depends on DAP SDK)")
message("[+] =========================================")

# Set DAP SDK information for Python-DAP
set(DAP_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk CACHE PATH "Path to DAP SDK")
set(DAP_SDK_TARGET_NAME "dap_sdk" CACHE STRING "DAP SDK target name")

# Build Python-DAP as MODULE (Python extension)
set(BUILD_SHARED ON CACHE BOOL "Build Python-DAP as shared library" FORCE)
set(LINK_DAP_SDK_STATIC OFF CACHE BOOL "Link DAP SDK dynamically" FORCE)
add_subdirectory(python-dap)

message("[+] Python-DAP bindings configured")

# =========================================
# PYTHON-CELLFRAME BINDINGS  
# =========================================
message("[+] =========================================")
message("[+] Configuring Python-Cellframe bindings")
message("[+] =========================================")

# Find Python
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Python-Cellframe sources
set(PYTHON_CELLFRAME_SOURCES
    src/cellframe.c
    # Chain group
    src/chain/cf_chain.c
    # Network group
    src/network/cf_network.c
    src/network/cf_balancer.c
    src/network/cf_balancer_init.c
    # Node group
    src/node/cf_node.c
    # Consensus group
    src/consensus/cf_consensus.c
    src/consensus/cf_consensus_init.c
    # Stake group
    src/stake/cf_stake.c
    src/stake/cf_stake_init.c
    # Mempool group
    src/mempool/cf_mempool.c
    src/mempool/cf_mempool_init.c
    # Services group
    src/services/cf_services.c
    src/services/cf_services_init.c
    src/services/cf_services_ext.c
    src/services/cf_services_ext_init.c
    # Compose group
    src/compose/cf_compose.c
    src/compose/cf_compose_init.c
    # Ledger group (modularized)
    src/ledger/cf_ledger_init.c
    src/ledger/cf_ledger_lifecycle.c
    src/ledger/cf_ledger_tx.c
    src/ledger/cf_ledger_token.c
    src/ledger/cf_ledger_balance.c
    src/ledger/cf_ledger_tx_lists.c
    src/ledger/cf_ledger_cond.c
    src/ledger/cf_ledger_callbacks.c
    src/ledger/cf_ledger_decree.c
    src/ledger/cf_ledger_anchor.c
    src/ledger/cf_ledger_event.c
    src/ledger/cf_ledger_utils.c
    # Wallet group
    src/wallet/cf_wallet.c
    # TX group
    src/tx/cf_tx.c
)

# Create Python extension module
add_library(python_cellframe MODULE ${PYTHON_CELLFRAME_SOURCES})

# Set proper Python module properties
set_target_properties(python_cellframe PROPERTIES
    PREFIX ""
    OUTPUT_NAME "python_cellframe"
    SUFFIX "${Python3_SOABI}.so"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
)

# Include directories
target_include_directories(python_cellframe PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${Python3_INCLUDE_DIRS}
    # Get include directories from linked libraries
    $<TARGET_PROPERTY:dap_sdk,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:cellframe_sdk,INTERFACE_INCLUDE_DIRECTORIES>
)

# Link libraries
target_link_libraries(python_cellframe PRIVATE
    dap_sdk
    cellframe_sdk
    python_dap
    ${Python3_LIBRARIES}
)

message("[+] Python-Cellframe bindings configured")

# =========================================
# PACKAGING SUPPORT (CPack) for Python-Cellframe
# =========================================
# Install python_cellframe module (component: python-cellframe)
install(TARGETS python_cellframe
    LIBRARY DESTINATION lib/python3/dist-packages
    ARCHIVE DESTINATION lib
    COMPONENT python-cellframe
)

# Explicitly install the library file (CPack sometimes misses MODULE targets)
# Use generator expression to get the actual output file name
install(FILES
    $<TARGET_FILE:python_cellframe>
    DESTINATION lib/python3/dist-packages
    COMPONENT python-cellframe
    OPTIONAL
)

# Install Python CellFrame package files (component: python-cellframe)
install(DIRECTORY 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CellFrame/
    DESTINATION lib/python3/dist-packages/CellFrame
    COMPONENT python-cellframe
    FILES_MATCHING PATTERN "*.py"
)

# Note: Cellframe SDK and DAP SDK shared libraries are NOT included in python-cellframe package
# They must be installed separately via system packages (dap-sdk, cellframe-sdk)
# Dependencies are specified in CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_DEPENDS

# Install documentation files if they exist (component: python-cellframe)
install(FILES
    README.md
    LICENSE  
    DESTINATION share/doc/python-cellframe
    OPTIONAL
    COMPONENT python-cellframe
)

# CPack component configuration for python-cellframe
# Note: CPack is already included by python-dap, so we configure components here
# python-cellframe does NOT include python-dap - it only depends on it via package dependencies

# Define package OS suffix and debug suffix (FAIL-FAST: no fallbacks)
if(NOT DEFINED DAP_PACKAGE_OS_SUFFIX)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/cmake/PackageDetection.cmake)
        message(FATAL_ERROR "PackageDetection.cmake not found at ${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/cmake/PackageDetection.cmake")
    endif()
    include(${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk/cmake/PackageDetection.cmake)
endif()

set(CPACK_COMPONENT_PYTHON-CELLFRAME_DISPLAY_NAME "Python-Cellframe")
set(CPACK_COMPONENT_PYTHON-CELLFRAME_DESCRIPTION "Python bindings for Cellframe SDK")
# No component dependency - python-dap is a package dependency, not a component dependency

# Override package name for python-cellframe component
set(CPACK_COMPONENT_PYTHON-CELLFRAME_PACKAGE_NAME "python-cellframe${DAP_DEBUG_SUFFIX}")

# Debian package specific for python-cellframe component
# Dependencies: python-dap (Python bindings), dap-sdk (C library), cellframe-sdk (C library)
set(CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_NAME "python-cellframe${DAP_DEBUG_SUFFIX}")
# Set file name for component package (format: name_version_arch, without .deb)
set(CPACK_DEBIAN_PYTHON-CELLFRAME_FILE_NAME "python-cellframe${DAP_DEBUG_SUFFIX}_3.0.0_${DAP_PACKAGE_OS_SUFFIX}")
set(CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_DEPENDS "python3 (>= 3.8), python3-dev, python-dap, dap-sdk, cellframe-sdk")
set(CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_SECTION "python")
set(CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_MAINTAINER "support@demlabs.net")
set(CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/cmake/debian/python-cellframe/postinst;${CMAKE_CURRENT_SOURCE_DIR}/cmake/debian/python-cellframe/prerm" OPTIONAL)

# Enable component-based packaging for both python-dap and python-cellframe
# This is set AFTER python-dap has configured its component, so we add python-cellframe
# Note: python-dap sets CPACK_COMPONENTS_ALL to python-dap, we extend it here
if(DEFINED CPACK_COMPONENTS_ALL)
    # Add python-cellframe to existing components
    list(APPEND CPACK_COMPONENTS_ALL python-cellframe)
    # Remove any unwanted components (like dap_json-c which shouldn't be packaged separately)
    list(REMOVE_ITEM CPACK_COMPONENTS_ALL dap_json-c Unspecified)
    list(REMOVE_DUPLICATES CPACK_COMPONENTS_ALL)
else()
    # Fallback if python-dap hasn't set it yet
    set(CPACK_COMPONENTS_ALL python-dap python-cellframe)
endif()

# Ensure component-based DEB packaging is enabled (python-dap may have already set it)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)

# Create separate CPack config for python-cellframe package
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPackConfig-python-cellframe.cmake.in)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPackConfig-python-cellframe.cmake.in
        ${CMAKE_BINARY_DIR}/CPackConfig-python-cellframe.cmake
        @ONLY
    )
    
    # Create custom target for building python-cellframe package only
    add_custom_target(package-python-cellframe
        COMMAND ${CMAKE_CPACK_COMMAND} --config ${CMAKE_BINARY_DIR}/CPackConfig-python-cellframe.cmake -G DEB
        DEPENDS python_cellframe
        COMMENT "Creating python-cellframe DEB package"
    )
else()
    # Fallback: use component-based approach
    add_custom_target(package-python-cellframe
        COMMAND ${CMAKE_CPACK_COMMAND} -G DEB -D CPACK_COMPONENTS_ALL=python-cellframe
        DEPENDS python_cellframe
        COMMENT "Creating python-cellframe DEB package"
    )
endif()

message("[+] CPack: Python-Cellframe component configured")
message("[+] CPack: Use 'make package-python-cellframe' to create python-cellframe package only")
message("[+] CPack: Use 'cpack -G DEB' to create all packages (python-dap + python-cellframe)")
message("[+] CPack: Use 'cpack -G DEB -D CPACK_COMPONENTS_ALL=python-cellframe' to create python-cellframe only")

# Include CPack here, after all components (python-dap and python-cellframe) are configured
# This ensures clean architecture: python-dap doesn't know about python-cellframe
# CPack is configured by python-dap but not included, so we include it here

# Override generators to DEB only (component-based packaging)
# Other generators (STGZ, TGZ, TZ) create empty packages for main project DAP_SDK_NATIVE
# which has no installation targets (we only package python-dap and python-cellframe components)
set(CPACK_GENERATOR "DEB" CACHE STRING "CPack generators" FORCE)

# Ensure components are set (python-dap may have set it, but we override to include both)
set(CPACK_COMPONENTS_ALL "python-dap;python-cellframe" CACHE STRING "CPack components" FORCE)

include(CPack)

# =========================================
# PROJECT SUMMARY
# =========================================
message("[+] =========================================")
message("[+] Python-Cellframe Project Configuration:")
message("[+] - DAP SDK:        libdap_sdk.so (SHARED)")
message("[+] - Cellframe SDK:  libcellframe-sdk.so (SHARED)")
message("[+] - Python-DAP:     MODULE (Python extension)")
message("[+] - Python-Cellframe: MODULE (Python extension)")
message("[+] =========================================")
message("[+] All components configured successfully!")
message("[+] =========================================")