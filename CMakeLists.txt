cmake_minimum_required(VERSION 3.10)
project(DAP_SDK_NATIVE C)

# Отключение SSL по умолчанию - WolfSSL больше не используется
add_definitions(-DDAP_NET_CLIENT_NO_SSL)

set(DAP_SDK_NATIVE_VERSION "3.0-0")

# Build type detection
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Common includes and definitions
include(cmake/OS_Detection.cmake)

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
        
        message("[+] Final DAP SDK Library: dap_sdk created from real object files")
    else()
        message("[!] No DAP internal modules found!")
    endif()
endmacro()

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
        
        message("[+] Final DAP SDK Library: dap_sdk created from real object files")
    else()
        message("[!] No DAP internal modules found!")
    endif()
endmacro()

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
        
        message("[+] Final DAP SDK Library: dap_sdk created from real object files")
    else()
        message("[!] No DAP internal modules found!")
    endif()
endmacro()

# Options
option(BUILD_SHARED "Build final DAP SDK library as shared (default)" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TESTS "Build tests" OFF)

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

# Core is always required - UPDATED PATH
add_subdirectory(module/core)

# Optional modules with UPDATED PATHS
if (DAPSDK_MODULES MATCHES "crypto")
    add_subdirectory(module/crypto)
    message("[+] Module 'crypto'")
endif()

if (DAPSDK_MODULES MATCHES "plugin")
    add_subdirectory(module/plugin)
    message("[+] Module 'plugin'")
endif()

if (DAPSDK_MODULES MATCHES "io")
    add_subdirectory(module/io)
    message("[+] Module 'io'")
endif()

if (DAPSDK_MODULES MATCHES "network-core")
    add_subdirectory(module/net/stream)
    message("[+] Module 'stream'")
endif()

if (DAPSDK_MODULES MATCHES "network-link_manager")
    add_subdirectory(module/net/link_manager)
    message("[+] Module 'link manager'")
endif()

if (DAPSDK_MODULES MATCHES "network-client")
    add_subdirectory(module/net/client)
    message("[+] Module 'client'")
endif()

if (DAPSDK_MODULES MATCHES "network-server")
    add_subdirectory(module/net/server)
    message("[+] Module 'server'")
endif()

if (DAPSDK_MODULES MATCHES "global-db")
    add_subdirectory(module/global-db)
    message("[+] Module 'global-db'")
endif()

if (DAPSDK_MODULES MATCHES "avrestream")
    add_subdirectory(module/avrestream)
    message("[+] Module 'av-restream'")
endif()

# Examples - UPDATED PATH
if(BUILD_EXAMPLES)
    add_subdirectory(module/examples)
    message("[+] Module 'examples'")
endif()

# Test framework - UPDATED PATH
if (BUILD_WITH_TEST)
    enable_testing()
    add_subdirectory(module/test)
    message("[+] Testing framework")
endif()

# New tests structure - UPDATED PATHS
if(BUILD_TESTS OR BUILD_WITH_TEST)
    add_subdirectory(tests)
    message("[+] Tests enabled with new structure")
endif()

# Create final library from all modules
dap_create_final_library()

# Create final library from all modules
dap_create_final_library()

# Create final library from all modules
dap_create_final_library()

message("[+] =========================================")
message("[+] DAP SDK with module/ structure configured")
message("[+] =========================================")