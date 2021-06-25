project(dap_server_core_python_module C)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
add_definitions("-fpic")

if(NOT (${SUBMODULES_NO_BUILD} MATCHES ON))
    set(SUBMODULES_NO_BUILD ON)
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
    if (NOT (TARGET dap-server-core))
       add_subdirectory(libdap-server-core)
       target_compile_options(
           dap_server_core PRIVATE
           "-fpic"
       )
    endif()
endif()



file(GLOB SERVER_CORE_PYTHON_SRCS src/*.c)
file(GLOB SERVER_CORE_PYTHON_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
find_package (PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR} include/)

add_library(${PROJECT_NAME} STATIC ${SERVER_CORE_PYTHON_SRCS} ${SERVER_CORE_PYTHON_HEADERS})

target_link_libraries(${PROJECT_NAME})


target_link_libraries(${PROJECT_NAME} dap_core dap_crypto dap_server_core)

target_include_directories(${PROJECT_NAME} PUBLIC include/ )

