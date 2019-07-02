project(dap_crypto_python_module C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
set(SUBMODULES_NO_BUILD ON)

add_subdirectory(libdap)
add_subdirectory(libdap-crypto)

file(GLOB CRYPTO_SRCS src/*.c)
file(GLOB CRYPTO_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7)
find_package (PythonLibs REQUIRED)
#find_package(PkgConfig)
#pkg_check_modules(PC_JSON-C REQUIRED json-c)
include_directories(${PYTHON_INCLUDE_DIR} include/)

add_library(${PROJECT_NAME} SHARED ${CRYPTO_SRCS} ${CRYPTO_HEADERS})

target_link_libraries(${PROJECT_NAME})

target_compile_options(
    dap_core PRIVATE 
    "-fpic"
)
target_compile_options(
    dap_crypto PRIVATE 
    "-fpic"
)


target_link_libraries(${PROJECT_NAME} dap_core dap_crypto)

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/libdapConnector.py
#        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/test.py
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

if(BUILD_DAP__TESTS)
#    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/main_test.py
#            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    enable_testing()
endif()

