project(dap_python_module C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
set(SUBMODULES_NO_BUILD ON)

add_subdirectory(libdap)
add_subdirectory(libdap-crypto)

file(GLOB CORE_SRCS src/*.c)
file(GLOB CORE_HEADERS src/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7)
find_package (PythonLibs REQUIRED)
find_package(PkgConfig)
pkg_check_modules(PC_JSON-C REQUIRED json-c)
include_directories(${PYTHON_INCLUDE_DIR})

#set(CMAKE_C_FLAGS "-fpic")
#set_property(GLOBAL PROPERTY CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -fpic")

#target_link_libraries(${PROJECT_NAME} dap_core)

#add_executable(${PROJECT_NAME} ${CORE_SRCS} ${CORE_UNIX_SRCS})
add_library(${PROJECT_NAME} SHARED ${CORE_SRCS} ${CORE_UNIX_SRCS})
#add_library(${PROJECT_NAME} STATIC ${CORE_SRCS} ${CORE_UNIX_SRCS})

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})

target_compile_options(
    dap_core PRIVATE 
    "-fpic"
)

target_link_libraries(${PROJECT_NAME} dap_core dap_crypto)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/libdapConnector.py
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

if(BUILD_DAP_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

