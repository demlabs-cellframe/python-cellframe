project(dap_notify_srv_python_module C)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
add_definitions("-fpic")

file(GLOB NOTIFY_SRV_SRCS src/*.c)
file(GLOB NOTIFY_SRV_HEADERS include/*.h)

include_directories(${PYTHON_INCLUDE_DIR} include/ )

add_library(${PROJECT_NAME} STATIC ${NOTIFY_SRV_SRCS} ${NOTIFY_SRV_HEADERS})

target_link_libraries(${PROJECT_NAME}
        dap_core
        dap_notify_srv
        dap_python_module
        ${PYTHON_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC include/ )

# Ensure python-core headers (libdap-python.h) are visible
target_include_directories(${PROJECT_NAME} PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../core/src)


