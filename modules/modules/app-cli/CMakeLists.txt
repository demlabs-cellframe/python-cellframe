project(dap_app_cli_python_module C)
cmake_minimum_required(VERSION 2.8)

add_definitions("-fpic")
add_definitions("-DDAP_LOG_MT")

file(GLOB APP_CLI_PYTHON_SRCS ./*.c)
file(GLOB APP_CLI_PYTHON_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
find_package (PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR} include/)

add_library(${PROJECT_NAME} STATIC ${APP_CLI_PYTHON_SRCS} ${APP_CLI_PYTHON_HEADERS})

target_link_libraries(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} dap_core dap_app_cli )

target_include_directories(${PROJECT_NAME} PUBLIC include/ )

