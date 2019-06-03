project(dap-python C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
#set(SUBMODULES_NO_BUILD ON)

add_subdirectory(libdap)

file(GLOB CORE_SRCS src/*.c)
file(GLOB CORE_HEADERS src/*.h)

set(Python_ADDITIONAL_VERSIONS 2.7)
find_package (PythonLibs)
include_directories(${PYTHON_INCLUDE_DIR})

add_library(${PROJECT_NAME} STATIC ${CORE_SRCS} ${CORE_UNIX_SRCS})

if(BUILD_DAP_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

