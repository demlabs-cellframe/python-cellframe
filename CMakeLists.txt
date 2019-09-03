project(dap_crypto_python_module C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
#set(SUBMODULES_NO_BUILD ON)

if(NOT (${SUBMODULES_NO_BUILD} MATCHES ON))
    if (NOT (TARGET dap_core))
        add_subdirectory(libdap)
        target_compile_options(
            dap_core PRIVATE 
            "-fpic"
        )
    endif()
endif()
add_subdirectory(libdap-crypto)

file(GLOB CRYPTO_PYTHON_SRCS src/*.c)
file(GLOB CRYPTO_PYTHON_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7)
find_package (PythonLibs REQUIRED)
#find_package(PkgConfig)
#pkg_check_modules(PC_JSON-C REQUIRED json-c)
include_directories(${PYTHON_INCLUDE_DIR} include/)

add_library(${PROJECT_NAME} SHARED ${CRYPTO_PYTHON_SRCS} ${CRYPTO_PYTHON_HEADERS})

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

target_include_directories(${PROJECT_NAME} PUBLIC include/ )

#if(BUILD_DAP_PYTHON_TESTS)
#    add_subdirectory(test)
#    enable_testing()
#endif()

