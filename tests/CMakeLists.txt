cmake_minimum_required(VERSION 3.10)
project(dap_sdk_tests C)

# Test configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories for DAP SDK
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../module/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../module/crypto/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../module/test/include
)

# Link directories
link_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/../module/core
    ${CMAKE_CURRENT_BINARY_DIR}/../module/crypto
)

message("[+] DAP SDK Tests structure:")
message("[+] - Unit tests: ${CMAKE_CURRENT_SOURCE_DIR}/unit")
message("[+] - Integration tests: ${CMAKE_CURRENT_SOURCE_DIR}/integration") 
message("[+] - Performance tests: ${CMAKE_CURRENT_SOURCE_DIR}/performance")
message("[+] - Security tests: ${CMAKE_CURRENT_SOURCE_DIR}/security")
message("[+] - Fixtures: ${CMAKE_CURRENT_SOURCE_DIR}/fixtures")

# Add test subdirectories if they contain CMakeLists.txt
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/unit/CMakeLists.txt")
    add_subdirectory(unit)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/integration/CMakeLists.txt")
    add_subdirectory(integration)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/performance/CMakeLists.txt")
    add_subdirectory(performance)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/security/CMakeLists.txt")
    add_subdirectory(security)
endif()

# Enable testing
enable_testing()

message("[+] Tests configuration completed")
