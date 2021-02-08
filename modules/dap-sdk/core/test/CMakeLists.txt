project(TPO C)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)

set(Python_ADDITIONAL_VERSIONS 3.7)
find_package (PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR} include/)

file(GLOB TPO_INCLUDES include/*.h)
file(GLOB TPO_SRCS src/*.c)

add_library(${PROJECT_NAME} SHARED ${TPO_INCLUDES} ${TPO_SRCS} )

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})

target_link_libraries(${PROJECT_NAME} dap_python_module)
