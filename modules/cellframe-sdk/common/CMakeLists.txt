project(dap_chain_common_python_module C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
add_definitions("-fpic")

if(UNIX)
    add_definitions("-DDAP_OS_UNIX")
endif()

if (NOT (${SUBMODULES_NO_BUILD} MATCHES ON))
    set (SUBMODULES_NO_BUILD ON)
    if (NOT (TARGET dap_core))
        add_subdirectory(libdap)
        target_compile_options(
            dap_core PRIVATE 
            "-fpic"
        )
    endif()
    if (NOT (TARGET dap_crypto))
        add_subdirectory(libdap-crypto)
        target_compile_options(
            dap_crypto PRIVATE
            "-fpic"
       )
   endif()
   if (NOT (TARGET dap_chain_common))
        add_subdirectory(libdap-chain-common)
        target_compile_options(
            dap_chain_common PRIVATE
            "-fpic"
       )
   endif()
    if (NOT (TARGET (dap_crypto_python_module)))
        add_subdirectory(libdap-crypto-python)
        target_compile_options(
              dap_crypto_python_module PRIVATE
              "-fpic"
        )
    endif()
endif()


file(GLOB CHAIN_COMMON_PYTHON_SRCS src/*.c)
file(GLOB CHAIN_COMMON_PYTHON_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
find_package (PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR} include/)

add_library(${PROJECT_NAME} STATIC ${CHAIN_COMMON_PYTHON_SRCS} ${CHAIN_COMMON_PYTHON_HEADERS})

target_link_libraries(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} dap_chain_common dap_core dap_crypto dap_crypto_python_module)

target_include_directories(${PROJECT_NAME} PUBLIC include/ )

