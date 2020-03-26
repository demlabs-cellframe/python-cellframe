project(CellFrame C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

SET( CPACK_GENERATOR "DEB")
SET( CPACK_PACKAGE_NAME  "${PROJECT_NAME}")
SET( CPACK_PACKAGE_VERSION_MAJOR 0)
SET( CPACK_PACKAGE_VERSION_MINOR 9)
SET( CPACK_PACKAGE_VERSION_PATCH 10)

add_definitions("-DDAP_VERSION_MAJOR=\"${CPACK_PACKAGE_VERSION_MAJOR}\"")
add_definitions("-DDAP_VERSION_MINOR=\"${CPACK_PACKAGE_VERSION_MINOR}\"")
add_definitions("-DDAP_VERSION_PATCH=\"${CPACK_PACKAGE_VERSION_PATCH}\"")
add_definitions("-DDAP_VERSION=\"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}-${CPACK_PACKAGE_VERSION_PATCH}\"")
set(SUBMODULES_NO_BUILD ON)

add_subdirectory(cellframe-sdk)

target_compile_options(dap_core PRIVATE "-fpic")
target_compile_options(dap_app_cli PRIVATE "-fpic")
target_compile_options(dap_crypto PRIVATE "-fpic")
target_compile_options(dap_chain PRIVATE "-fpic")
target_compile_options(dap_chain_common PRIVATE "-fpic")
target_compile_options(dap_chain_wallet PRIVATE "-fpic" )
target_compile_options(dap_chain_crypto PRIVATE "-fpic" )
target_compile_options(dap_chain_mempool PRIVATE "-fpic" )
target_compile_options(dap_chain_net PRIVATE "-fpic" )
target_compile_options(dap_server_core PRIVATE "-fpic" )
target_compile_options(dap_chain_global_db PRIVATE "-fpic" )
target_compile_options(dap_client PRIVATE "-fpic" )
#target_compile_options(libdap-server PRIVATE "-fpic" )
target_compile_options(dap_stream PRIVATE "-fpic" )
target_compile_options(dap_stream_ch PRIVATE "-fpic" )
target_compile_options(dap_stream_ch_chain_net PRIVATE "-fpic" )
target_compile_options(dap_stream_ch_chain PRIVATE "-fpic" )
target_compile_options(dap_udp_server PRIVATE "-fpic" )
target_compile_options(dap_chain_net_srv PRIVATE "-fpic" )
target_compile_options(dap_chain_net_srv_vpn PRIVATE "-fpic" )
target_compile_options(dap_chain_gdb PRIVATE "-fpic" )
target_compile_options(dap_enc_server PRIVATE "-fpic")
target_compile_options(dap_http_server PRIVATE "-fpic")
target_compile_options(dap_session PRIVATE "-fpic")
target_compile_options(dap_stream_ch_chain_net_srv PRIVATE "-fpic")
target_compile_options(dap_chain_cs_dag PRIVATE "-fpic")
target_compile_options(dap_chain_cs_dag_poa PRIVATE "-fpic")
target_compile_options(dap_chain_cs_dag_pos PRIVATE "-fpic")

add_subdirectory(cellframe-sdk-python/libdap-server-core-python)
add_subdirectory(cellframe-sdk-python/libdap-python)
add_subdirectory(cellframe-sdk-python/libdap-crypto-python)
add_subdirectory(cellframe-sdk-python/libdap-chain-common-python)
add_subdirectory(cellframe-sdk-python/libdap-chain-net-python)
add_subdirectory(cellframe-sdk-python/libdap-chain-gdb-python)
add_subdirectory(cellframe-sdk-python/libdap-chain-python)
add_subdirectory(cellframe-sdk-python/libdap-app-cli-python)
add_subdirectory(cellframe-sdk-python/libdap-client-python)

target_compile_options(dap_chain_common_python_module PRIVATE "-fpic")
target_compile_options(dap_python_module PRIVATE "-fpic" )
target_compile_options(dap_crypto_python_module PRIVATE "-fpic" )
target_compile_options(dap_chain_net_python_module PRIVATE "-fpic")
target_compile_options(dap_chain_gdb_python_module PRIVATE "-fpic")
target_compile_options(dap_chain_python_module PRIVATE "-fpic")
target_compile_options(dap_app_cli_python_module PRIVATE "-fpic")
target_compile_options(dap_client_python_module PRIVATE "-fpic")

file(GLOB PYTHON_CELLFRAME_SRCS src/*.c)
file(GLOB PYTHON_CELLFRAME_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
find_package (PythonLibs REQUIRED)

include_directories(${PYTHON_INCLUDE_DIR} include/)

#if (SKBUILD)
message(STATUS "Make python extension")
#find_package(PythonExtensions REQUIRED)
add_library(CellFrame MODULE ${PYTHON_CELLFRAME_SRCS} ${PYTHON_CELLFRAME_HEADERS})
#python_extension_module(${PROJECT_NAME})
#python_extension_module(${PROJECT_NAME} LINKED_MODULES_VAR dap_python_module)
#python_extension_module(${PROJECT_NAME} LINKED_MODULES_VAR dap_crypto_python_module)
#python_extension_module(${PROJECT_NAME} LINKED_MODULES_VAR DapServerCore)
install(TARGETS CellFrame LIBRARY DESTINATION CellFrame)
install(FILES src/__init__.py DESTINATION CellFrame)
install(FILES src/PublicNetworks/* DESTINATION CellFrame)
#    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libdap-python/libdap_python_module.so DESTINATION CellFrame/lib)
#    install(TARGETS dap_python_module LIBRARY DESTINATION CellFrame)
#    install(TARGETS dap_crypto_python_module LIBRARY DESTINATION CellFrame)
#    install(TARGETS DapServerCore LIBRARY DESTINATION CellFrame)
#else()
#    add_library(${PROJECT_NAME} SHARED ${PYTHON_CELLFRAME_SRCS} ${PYTHON_CELLFRAME_HEADERS})
#endif()

target_link_libraries(CellFrame ${PYTHON_LIBRARIES})

#target_compile_options(
#    dap_core PRIVATE 
#    "-fpic"
#)

#target_compile_options(a
#    dap_python_module PRIVATE 
#    "-fpic"
#)

#target_compile_options(
#    dap_crypto_python_module PRIVATE 
#    "-fpic"
#)

#target_compile_options(
#    DapServerCore PRIVATE 
#    "-fpic"
#)

target_link_libraries(CellFrame cellframe-sdk dap_python_module
                      dap_crypto_python_module
                      dap_server_core_python_module
                      dap_chain_python_module
                      dap_chain_net_python_module
                      dap_chain_gdb_python_module
                      dap_app_cli_python_module
                     )
#target_link_libraries(${PROJECT_NAME} dap_core dap_crypto)

target_include_directories(CellFrame INTERFACE include/)


#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/libdapConnector.py
#        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/main_test.py
#            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

#if(BUILD_DAP_TESTS)
#    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/main_test.py
#            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
#    enable_testing()
    #add_subdirectory(test)
#endif()
