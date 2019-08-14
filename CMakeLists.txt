project(DapServerCore C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
set(SUBMODULES_NO_BUILD ON)

add_subdirectory(libdap)
add_subdirectory(libdap-crypto)
add_subdirectory(libdap-server-core)

file(GLOB SERVER_CORE_PYTHON_SRCS src/*.c)
file(GLOB SERVER_CORE_PYTHON_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7)
find_package (PythonLibs REQUIRED)
#find_package(PkgConfig)
#pkg_check_modules(PC_JSON-C REQUIRED json-c)
include_directories(${PYTHON_INCLUDE_DIR} include/)

add_library(${PROJECT_NAME} SHARED ${SERVER_CORE_PYTHON_SRCS} ${SERVER_CORE_PYTHON_HEADERS})

target_link_libraries(${PROJECT_NAME})

target_compile_options(
    dap_server_core PRIVATE
    "-fpic"
)

target_compile_options(
    dap_core PRIVATE
    "-fpic"
)
target_compile_options(
    dap_crypto PRIVATE
    "-fpic"
)


#target_link_libraries(${PROJECT_NAME} dap_core dap_crypto)
#targat_link_libraries(${PROJECT_NAME} dap_server_core)
target_link_libraries(${PROJECT_NAME} dap_core dap_crypto dap_server_core)

#if(BUILD_DAP_PYTHON_TESTS)
#    add_subdirectory(test)
#    enable_testing()
#endif()

