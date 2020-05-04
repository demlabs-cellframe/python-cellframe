project(dap_python_module C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
add_definitions("-fpic")

file(GLOB CORE_SRCS ./*.c)
file(GLOB CORE_HEADERS ./*.h)

set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
find_package (PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR} ./ )

add_library(${PROJECT_NAME} STATIC ${CORE_SRCS} ${CORE_UNIX_SRCS})

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})

#target_compile_options(
#    dap_core PRIVATE
#    "-fpic"
#)

target_link_libraries(${PROJECT_NAME} dap_core ${PYTHON_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ./ )
