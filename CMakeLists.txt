project(dap_chain_net_python_module C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
set(SUBMODULES_NO_BUILD ON)
add_definitions("-fpic")

#if(NOT (${SUBMODULES_NO_BUILD} MATCHES ON))
#    if (NOT (TARGET dap_core))
        add_subdirectory(libdap)
#        target_compile_options(
#            dap_core PRIVATE 
#            "-fpic"
#        )
#    endif()
#    if (NOT (TARGET dap_crypto))
#        add_subdirectory(libdap-crypto)
#        target_compile_options(
#            dap_crypto PRIVATE
#            "-fpic"
 #       )
 #   endif()
#endif()
#add_definitions("-DDAP_APP_NAME=\"TestPRJ\" -DSYSTEM_CONFIGS_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"")
add_definitions ("-DNODE_NETNAME=\"cellframe\"")
if(UNIX)
    add_definitions("-DDAP_OS_UNIX")
endif()

add_subdirectory(libdap-crypto)
add_subdirectory(libdap-chain)
add_subdirectory(libdap-chain-crypto)
add_subdirectory(libdap-chain-mempool)
add_subdirectory(libdap-chain-net)
add_subdirectory(libdap-server-core)
add_subdirectory(libdap-chain-global-db)
add_subdirectory(libdap-client)
add_subdirectory(libdap-server)
add_subdirectory(libdap-stream)
add_subdirectory(libdap-stream-ch)
add_subdirectory(libdap-server-udp)
add_subdirectory(libdap-stream-ch-chain-net)
add_subdirectory(libdap-stream-ch-chain)
add_subdirectory(libdap-chain-wallet)
add_subdirectory(libdap-chain-net-srv)
add_subdirectory(libdap-chain-net-srv-vpn)
add_subdirectory(libdap-chain-gdb)
add_subdirectory(libdap-chain-python)
add_subdirectory(libdap-crypto-python)
add_subdirectory(libdap-python)

file(GLOB CHAIN_NET_PYTHON_SRCS src/*.c)
file(GLOB CHAIN_NET_PYTHON_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7)
find_package (PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR} include/)

add_library(${PROJECT_NAME} STATIC ${CHAIN_NET_PYTHON_SRCS} ${CHAIN_NET_PYTHON_HEADERS})

target_link_libraries(${PROJECT_NAME})

#target_compile_options(
#    dap_core PRIVATE 
#    "-fpic"
#)
#target_compile_options(
#    dap_crypto PRIVATE 
#    "-fpic"
#)


target_link_libraries(${PROJECT_NAME} dap_core dap_crypto dap_chain dap_chain_crypto dap_server_core dap_chain_net dap_chain_python_module)
#target_link_libraries(${PROJECT_NAME} dap_core dap_crypto dap_chain dap_chain_global_db dap_chain_crypto dap_chain_mempool
#                         
#                     ) #dap_chain_crypto dap_chain_mempool dap_chain_global_db )
#target_link_libraries(${PROJECT_NAME} dap_core dap_crypto)

target_include_directories(${PROJECT_NAME} PUBLIC include/ )

#if(BUILD_DAP_CRYPTO_PYTHON_TESTS)
#    add_subdirectory(test)
#    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/test_b58.py 
#                    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)
#    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/test_b64.py
#                    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)
#    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/test_iaes256_cbc.py
#                    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)
#    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/test_iaes256_cbc.py
#                    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)
#    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/test_oaes.py
#                    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)


#endif()

