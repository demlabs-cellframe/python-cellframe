cmake_minimum_required(VERSION 2.8)
project(dap_chain_plugins_python C)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
add_definitions("-fpic")
add_definitions("-DDAP_LOG_MT")
add_subdirectory(python-cellframe)
target_compile_options(API_CellFrame PRIVATE "-fpic")

if(UNIX)
    add_definitions("-DDAP_OS_UNIX")
endif()

file(GLOB CHAIN_PLUGINS_PYTHON_SRCS src/*.c)
file(GLOB CHAIN_PLUGINS_PYTHON_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
find_package (PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR} include/)

add_library(${PROJECT_NAME} STATIC ${CHAIN_PLUGINS_PYTHON_SRCS} ${CHAIN_PLUGINS_PYTHON_HEADERS})

target_link_libraries(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} dap_core  API_CellFrame)

target_include_directories(${PROJECT_NAME} PUBLIC include/ )

