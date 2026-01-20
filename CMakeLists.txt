cmake_minimum_required(VERSION 3.12)
project(PYTHON_CELLFRAME C)

# =========================================
# PROJECT METADATA
# =========================================
set(PYTHON_CELLFRAME_VERSION "3.0-0")
set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)

# Disable SSL by default (WolfSSL no longer used)
add_definitions(-DDAP_NET_CLIENT_NO_SSL)

# Build type detection
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# =========================================
# SUBMODULE STRUCTURE
# =========================================
# Project structure:
#   python-cellframe/
#   ├── dap-sdk/          (submodule, builds libdap_sdk.so)
#   ├── cellframe-sdk/    (submodule, builds libcellframe_sdk.so)
#   │   └── dap-sdk/      (nested submodule, used by cellframe-sdk)
#   │   └── tools/dap_tpl (template engine for code generation)
#   ├── python-dap/       (submodule, builds python_dap MODULE)
#   └── src/              (python-cellframe bindings source)

# =========================================
# CMAKE HELPERS FROM DAP-SDK
# =========================================
# Use dap-sdk/cmake helpers for consistent build system
# NOTE: We do NOT use ObjectLibraryHelpers.cmake as it causes recursion issues
# DAP SDK and Cellframe SDK handle their own internal module management
include(dap-sdk/cmake/OS_Detection.cmake)
include(dap-sdk/cmake/PackageComponents.cmake)
include(dap-sdk/cmake/PackageDetection.cmake)
# Do NOT include ObjectLibraryHelpers.cmake - it's broken and causes recursion
# include(dap-sdk/cmake/ObjectLibraryHelpers.cmake)  # BROKEN - DO NOT USE

# =========================================
# DAP_TPL (TEMPLATE ENGINE)
# =========================================
# dap_tpl is used by dap_mock for test code generation
# Located in cellframe-sdk/tools/dap_tpl (centralized)
set(DAP_TPL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cellframe-sdk/tools/dap_tpl" CACHE PATH "Path to dap_tpl template engine")

if(NOT EXISTS "${DAP_TPL_DIR}/dap_tpl.sh")
    message(WARNING "dap_tpl not found at ${DAP_TPL_DIR} - tests may not work correctly")
else()
    message(STATUS "[Python-Cellframe] Using centralized dap_tpl from: ${DAP_TPL_DIR}")
endif()

# =========================================
# PACKAGING CONFIGURATION
# =========================================
# Disable DAP SDK and Cellframe SDK installation/packaging
# Only package python-dap and python-cellframe
set(INSTALL_DAP_SDK OFF CACHE BOOL "Disable DAP SDK packaging" FORCE)
set(INSTALL_CELLFRAME_SDK OFF CACHE BOOL "Disable Cellframe SDK packaging" FORCE)
set(INSTALL_SDK OFF CACHE BOOL "Disable json-c installation" FORCE)

# Set global CPack maintainer
set(CPACK_PACKAGE_CONTACT "support@demlabs.net")

# =========================================
# BUILD OPTIONS
# =========================================
option(BUILD_SHARED "Build SDK libraries as shared (default)" ON)
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

# Apply ENABLE_TESTING define globally for DAP SDK modules
if(BUILD_TESTS OR BUILD_WITH_TEST)
    add_definitions(-DENABLE_TESTING)
    message(STATUS "[Python-Cellframe] ENABLE_TESTING define enabled globally")
endif()

# =========================================
# DAP SDK (SUBMODULE)
# =========================================
message(STATUS "=========================================")
message(STATUS "Configuring DAP SDK (submodule)")
message(STATUS "=========================================")

# Set modules for dap-sdk build
set(DAPSDK_MODULES "crypto plugin io network-core network-server network-client network-link_manager global-db" CACHE STRING "DAP SDK modules to build")

# Build DAP SDK as shared library
set(BUILD_SHARED ON CACHE BOOL "Build DAP SDK as shared library" FORCE)

# Disable DAP SDK tests by default (use BUILD_TESTS to enable)
if(BUILD_TESTS)
    set(BUILD_DAP_SDK_TESTS ON CACHE BOOL "Build DAP SDK tests" FORCE)
endif()

add_subdirectory(dap-sdk)

message(STATUS "DAP SDK configured: libdap_sdk.so")

# =========================================
# CELLFRAME SDK (SUBMODULE)
# =========================================
message(STATUS "=========================================")
message(STATUS "Configuring Cellframe SDK (submodule)")
message(STATUS "=========================================")

# Set DAP SDK path for Cellframe SDK (it has nested dap-sdk submodule)
set(DAP_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cellframe-sdk/dap-sdk" CACHE PATH "Path to DAP SDK for Cellframe")

# Build Cellframe SDK as shared library
set(BUILD_SHARED ON CACHE BOOL "Build Cellframe SDK as shared library" FORCE)

# Disable Cellframe SDK tests by default (use BUILD_TESTS to enable)
if(BUILD_TESTS)
    set(BUILD_CELLFRAME_SDK_TESTS ON CACHE BOOL "Build Cellframe SDK tests" FORCE)
endif()

add_subdirectory(cellframe-sdk)

message(STATUS "Cellframe SDK configured: libcellframe_sdk.so")

# =========================================
# PYTHON-DAP (SUBMODULE)
# =========================================
message(STATUS "=========================================")
message(STATUS "Configuring Python-DAP bindings (submodule)")
message(STATUS "=========================================")

# Set DAP SDK information for Python-DAP
set(DAP_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dap-sdk" CACHE PATH "Path to DAP SDK")
set(DAP_SDK_TARGET_NAME "dap_sdk" CACHE STRING "DAP SDK target name")

# Build Python-DAP as MODULE (Python extension)
set(LINK_DAP_SDK_STATIC OFF CACHE BOOL "Link DAP SDK dynamically" FORCE)

add_subdirectory(python-dap)

message(STATUS "Python-DAP bindings configured")

# =========================================
# PYTHON-CELLFRAME (THIS PROJECT)
# =========================================
message(STATUS "=========================================")
message(STATUS "Configuring Python-Cellframe bindings")
message(STATUS "=========================================")

# Find Python
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Python-Cellframe sources
set(PYTHON_CELLFRAME_SOURCES
    src/cellframe.c
    # Common utilities
    src/common/cf_callbacks_registry.c
    src/common/cf_ledger_callback_registry.c
    src/common/cf_verificator_registry.c
    src/common/cf_dap_json.c
    # Chain group
    src/chain/cf_chain.c
    src/chain/cf_consensus.c
    src/chain/cf_consensus_init.c
    # Ledger group (modularized, includes tx operations)
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
    src/ledger/cf_token_manager.c
    src/ledger/tx/cf_tx.c
    # Phase 3.5: TX Compose Refactoring COMPLETE
    src/ledger/tx/cf_compose.c
    src/ledger/tx/cf_compose_init.c
    # Governance group (Phase 7)
    src/governance/cf_governance_decree.c
    src/governance/cf_governance_manager.c
    # Wallet group
    src/wallet/cf_wallet.c
    # Network group
    src/network/cf_network.c
    src/network/cf_balancer.c
    src/network/cf_balancer_init.c
    src/network/node/cf_node.c
    src/network/services/cf_services.c
    src/network/services/cf_services_init.c
    src/network/services/cf_services_ext.c
    src/network/services/cf_services_ext_init.c
    src/network/stake/cf_stake.c
    src/network/stake/cf_stake_init.c
    src/network/mempool/cf_mempool.c
    src/network/mempool/cf_mempool_init.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${Python3_INCLUDE_DIRS}
    # Get include directories from linked libraries (object libraries propagate includes)
    $<TARGET_PROPERTY:dap_sdk,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:cellframe_sdk,INTERFACE_INCLUDE_DIRECTORIES>
    # Additional Cellframe SDK includes not auto-exported (datum/tx module)
    ${CMAKE_CURRENT_SOURCE_DIR}/cellframe-sdk/modules/datum/tx/include
)

# Link libraries
target_link_libraries(python_cellframe PRIVATE
    dap_sdk
    cellframe_sdk
    python_dap
    ${Python3_LIBRARIES}
)

message(STATUS "Python-Cellframe bindings configured")

# =========================================
# TESTS (OPTIONAL)
# =========================================
if(BUILD_TESTS OR BUILD_WITH_TEST)
    enable_testing()
    add_subdirectory(tests)
    message(STATUS "Python-Cellframe tests enabled")
endif()

# =========================================
# PACKAGING (CPACK)
# =========================================
# Install python_cellframe module
install(TARGETS python_cellframe
    LIBRARY DESTINATION lib/python3/dist-packages
    COMPONENT python-cellframe
)

# Install library file explicitly (CPack sometimes misses MODULE targets)
install(FILES
    $<TARGET_FILE:python_cellframe>
    DESTINATION lib/python3/dist-packages
    COMPONENT python-cellframe
    OPTIONAL
)

# Install Python CellFrame package files
install(DIRECTORY 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CellFrame/
    DESTINATION lib/python3/dist-packages/CellFrame
    COMPONENT python-cellframe
    FILES_MATCHING PATTERN "*.py"
)

# Install documentation
install(FILES
    README.md
    LICENSE  
    DESTINATION share/doc/python-cellframe
    OPTIONAL
    COMPONENT python-cellframe
)

# CPack component configuration
set(CPACK_COMPONENT_PYTHON-CELLFRAME_DISPLAY_NAME "Python-Cellframe")
set(CPACK_COMPONENT_PYTHON-CELLFRAME_DESCRIPTION "Python bindings for Cellframe SDK")
set(CPACK_COMPONENT_PYTHON-CELLFRAME_PACKAGE_NAME "python-cellframe${DAP_DEBUG_SUFFIX}")

# Debian package settings
set(CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_NAME "python-cellframe${DAP_DEBUG_SUFFIX}")
set(CPACK_DEBIAN_PYTHON-CELLFRAME_FILE_NAME "python-cellframe${DAP_DEBUG_SUFFIX}_${PYTHON_CELLFRAME_VERSION}_${DAP_PACKAGE_OS_SUFFIX}")
set(CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_DEPENDS "python3 (>= 3.8), python3-dev, python-dap, dap-sdk, cellframe-sdk")
set(CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_SECTION "python")
set(CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PYTHON-CELLFRAME_PACKAGE_MAINTAINER "support@demlabs.net")

# Enable component-based packaging
if(DEFINED CPACK_COMPONENTS_ALL)
    list(APPEND CPACK_COMPONENTS_ALL python-cellframe)
    list(REMOVE_ITEM CPACK_COMPONENTS_ALL dap_json-c Unspecified)
    list(REMOVE_DUPLICATES CPACK_COMPONENTS_ALL)
else()
    set(CPACK_COMPONENTS_ALL python-dap python-cellframe)
endif()

set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)
set(CPACK_GENERATOR "DEB" CACHE STRING "CPack generators" FORCE)

# Custom target for python-cellframe package only
add_custom_target(package-python-cellframe
    COMMAND ${CMAKE_CPACK_COMMAND} -G DEB -D CPACK_COMPONENTS_ALL=python-cellframe
    DEPENDS python_cellframe
    COMMENT "Creating python-cellframe DEB package"
)

include(CPack)

message(STATUS "CPack: Python-Cellframe component configured")
message(STATUS "CPack: Use 'make package-python-cellframe' for python-cellframe package")
message(STATUS "CPack: Use 'cpack -G DEB' for all packages")

# =========================================
# PROJECT SUMMARY
# =========================================
message(STATUS "=========================================")
message(STATUS "Python-Cellframe Project Configuration:")
message(STATUS "- DAP SDK:          libdap_sdk.so (SHARED)")
message(STATUS "- Cellframe SDK:    libcellframe_sdk.so (SHARED)")
message(STATUS "- Python-DAP:       MODULE (Python extension)")
message(STATUS "- Python-Cellframe: MODULE (Python extension)")
message(STATUS "- dap_tpl:          ${DAP_TPL_DIR}")
message(STATUS "=========================================")
message(STATUS "All components configured successfully!")
message(STATUS "=========================================")
