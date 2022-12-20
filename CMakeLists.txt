cmake_minimum_required(VERSION 3.0)

project(CellFrame C)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)


set(Python_ADDITIONAL_VERSIONS 3.9 3.8 3.7 3.6 3.5 3.4)
find_package (PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR} include/)
file(GLOB PYTHON_CELLFRAME_SRCS CellFrame/*.c)
file(GLOB PYTHON_CELLFRAME_HEADERS include/*.h)

set(SUBMODULES_NO_BUILD ON)

if (NOT (SUPPORT_PYTHON_PLUGINS))
    if (NOT(WIN32))
        set(CELLFRAME_MODULES "core chains network cs-none srv")
    else()
        set(CELLFRAME_MODULES "core chains network cs-none srv")
    endif()
    include(cellframe-sdk/cmake/OS_Detection.cmake)
    add_subdirectory(cellframe-sdk)
else()
    add_compile_options(-Wno-missing-field-initializers)
    add_subdirectory(modules/plugins)
    target_compile_options(dap_plugins_python PRIVATE "-fpic")
endif()

add_subdirectory(modules/dap-sdk/core)
add_subdirectory(modules/dap-sdk/crypto)
add_subdirectory(modules/dap-sdk/net/core)
add_subdirectory(modules/dap-sdk/net/client)
add_subdirectory(modules/dap-sdk/net/server/enc)
add_subdirectory(modules/dap-sdk/net/server/http)
add_subdirectory(modules/dap-sdk/net/server/json_rpc)
add_subdirectory(modules/dap-sdk/net/stream/stream)
add_subdirectory(modules/cellframe-sdk/common)
add_subdirectory(modules/cellframe-sdk/mempool)
add_subdirectory(modules/cellframe-sdk/net)
add_subdirectory(modules/cellframe-sdk/net/srv)
add_subdirectory(modules/cellframe-sdk/gdb)
add_subdirectory(modules/cellframe-sdk/chain)
add_subdirectory(modules/cellframe-sdk/app-cli)
add_subdirectory(modules/cellframe-sdk/wallet)
add_subdirectory(modules/cellframe-sdk/type/dag)
add_subdirectory(modules/cellframe-sdk/type/blocks)
add_subdirectory(modules/cellframe-sdk/consensus/dag_poa)
add_subdirectory(modules/cellframe-sdk/global-db)

    target_compile_options(dap_chain_common_python_module PRIVATE "-fpic")
    target_compile_options(dap_python_module PRIVATE "-fpic" )
    target_compile_options(dap_crypto_python_module PRIVATE "-fpic" )
    target_compile_options(dap_chain_net_python_module PRIVATE "-fpic")
    target_compile_options(dap_chain_net_srv_python_module PRIVATE "-fpic")
    target_compile_options(dap_chain_gdb_python_module PRIVATE "-fpic")
    target_compile_options(dap_chain_python_module PRIVATE "-fpic")
    target_compile_options(dap_app_cli_python_module PRIVATE "-fpic")
    target_compile_options(dap_client_python_module PRIVATE "-fpic")
    target_compile_options(dap_chain_wallet_python_module PRIVATE "-fpic")


target_compile_options(dap_core PRIVATE "-fpic")
if(NOT WIN32)
    target_compile_options(dap_core_unix PRIVATE "-fpic")
endif()
target_compile_options(dap_app_cli PRIVATE "-fpic")
target_compile_options(dap_crypto PRIVATE "-fpic")
target_compile_options(dap_chain PRIVATE "-fpic")
target_compile_options(dap_chain_common PRIVATE "-fpic")
target_compile_options(dap_chain_wallet PRIVATE "-fpic" )
#target_compile_options(dap_chain_crypto PRIVATE "-fpic" )
#target_compile_options(dap_chain_mempool PRIVATE "-fpic" )
target_compile_options(dap_chain_net PRIVATE "-fpic" )
target_compile_options(dap_server_core PRIVATE "-fpic" )
target_compile_options(dap_chain_global_db PRIVATE "-fpic" )
target_compile_options(dap_client PRIVATE "-fpic" )
#target_compile_options(libdap-server PRIVATE "-fpic" )
target_compile_options(dap_stream PRIVATE "-fpic" )
target_compile_options(dap_stream_ch PRIVATE "-fpic" )
target_compile_options(dap_stream_ch_chain_net PRIVATE "-fpic" )
target_compile_options(dap_stream_ch_chain PRIVATE "-fpic" )
#target_compile_options(dap_chain_net_srv PRIVATE "-fpic" )
#target_compile_options(dap_chain_net_srv_stake PRIVATE "-fpic")
if(NOT WIN32)
#    target_compile_options(dap_chain_net_srv_vpn PRIVATE "-fpic" )
endif()
#target_compile_options(dap_chain_cs_none PRIVATE "-fpic" )
target_compile_options(dap_enc_server PRIVATE "-fpic")
target_compile_options(dap_http_server PRIVATE "-fpic")
target_compile_options(dap_session PRIVATE "-fpic")
target_compile_options(dap_stream_ch_chain_net_srv PRIVATE "-fpic")
#target_compile_options(dap_chain_cs_dag PRIVATE "-fpic")
#target_compile_options(dap_chain_cs_dag_poa PRIVATE "-fpic")
#target_compile_options(dap_chain_cs_dag_pos PRIVATE "-fpic")



if (SUPPORT_PYTHON_PLUGINS)
    file (GLOB APP_CONTEXT_HEADER
        modules/plugins/include/dap_plugins_python_app_context.h)

    add_definitions("-fpic")
    add_definitions("-DDAP_SUPPORT_PYTHON_PLUGINS")
    add_library(${PROJECT_NAME} STATIC ${PYTHON_CELLFRAME_SRCS} ${APP_CONTEXT_HEADER} ${PYTHON_CELLFRAME_HEADERS})

else()

    SET( CPACK_GENERATOR "DEB")
    SET( CPACK_PACKAGE_NAME  "${PROJECT_NAME}")
    SET( CPACK_PACKAGE_VERSION_MAJOR 0)
    SET( CPACK_PACKAGE_VERSION_MINOR 9)
    SET( CPACK_PACKAGE_VERSION_PATCH 15)

    add_definitions("-DDAP_VERSION_MAJOR=\"${CPACK_PACKAGE_VERSION_MAJOR}\"")
    add_definitions("-DDAP_VERSION_MINOR=\"${CPACK_PACKAGE_VERSION_MINOR}\"")
    add_definitions("-DDAP_VERSION_PATCH=\"${CPACK_PACKAGE_VERSION_PATCH}\"")
    add_definitions("-DDAP_VERSION=\"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}-${CPACK_PACKAGE_VERSION_PATCH}\"")
    set(SUBMODULES_NO_BUILD ON)


    #if (SKBUILD)
    message(STATUS "Make python extension")
    #find_package(PythonExtensions REQUIRED)
    add_library(CellFrame MODULE ${PYTHON_CELLFRAME_SRCS} ${PYTHON_CELLFRAME_HEADERS})
    #python_extension_module(${PROJECT_NAME})
    #python_extension_module(${PROJECT_NAME} LINKED_MODULES_VAR dap_python_module)
    #python_extension_module(${PROJECT_NAME} LINKED_MODULES_VAR dap_crypto_python_module)
    #python_extension_module(${PROJECT_NAME} LINKED_MODULES_VAR DapServerCore)
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${PROJECT_NAME})
    install(FILES CellFrame/__init__.py DESTINATION ${PROJECT_NAME})
    install(FILES CellFrame/PublicNetworks/* DESTINATION ${PROJECT_NAME})
    #    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libdap-python/libdap_python_module.so DESTINATION CellFrame/lib)
    #    install(TARGETS dap_python_module LIBRARY DESTINATION CellFrame)
    #    install(TARGETS dap_crypto_python_module LIBRARY DESTINATION CellFrame)
    #    install(TARGETS DapServerCore LIBRARY DESTINATION CellFrame)
    #else()
    #    add_library(${PROJECT_NAME} SHARED ${PYTHON_CELLFRAME_SRCS} ${PYTHON_CELLFRAME_HEADERS})
    #endif()

endif()

target_compile_options(dap_core PRIVATE "-fpic")
target_compile_options(dap_app_cli PRIVATE "-fpic")
target_compile_options(dap_crypto PRIVATE "-fpic")
target_compile_options(dap_chain PRIVATE "-fpic")
target_compile_options(dap_chain_common PRIVATE "-fpic")
target_compile_options(dap_chain_wallet PRIVATE "-fpic" )
#target_compile_options(dap_chain_crypto PRIVATE "-fpic" )
#target_compile_options(dap_chain_mempool PRIVATE "-fpic" )
target_compile_options(dap_chain_net PRIVATE "-fpic" )
target_compile_options(dap_server_core PRIVATE "-fpic" )
target_compile_options(dap_chain_global_db PRIVATE "-fpic" )
target_compile_options(dap_client PRIVATE "-fpic" )
#target_compile_options(libdap-server PRIVATE "-fpic" )
target_compile_options(dap_stream PRIVATE "-fpic" )
target_compile_options(dap_stream_ch PRIVATE "-fpic" )
target_compile_options(dap_stream_ch_chain_net PRIVATE "-fpic" )
target_compile_options(dap_stream_ch_chain PRIVATE "-fpic" )
#target_compile_options(dap_chain_net_srv PRIVATE "-fpic" )
if (NOT WIN32)
#target_compile_options(dap_chain_net_srv_vpn PRIVATE "-fpic" )
endif()
#target_compile_options(dap_chain_cs_none PRIVATE "-fpic" )
target_compile_options(dap_enc_server PRIVATE "-fpic")
target_compile_options(dap_http_server PRIVATE "-fpic")
target_compile_options(dap_session PRIVATE "-fpic")
target_compile_options(dap_stream_ch_chain_net_srv PRIVATE "-fpic")
#target_compile_options(dap_chain_cs_dag PRIVATE "-fpic")
#target_compile_options(dap_chain_cs_dag_poa PRIVATE "-fpic")
#target_compile_options(dap_chain_cs_dag_pos PRIVATE "-fpic")
target_compile_options(dap_server_core_python_module PRIVATE "-fpic")


if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")
endif()


target_link_libraries(${PROJECT_NAME} cellframe-sdk dap_python_module
                      dap_crypto_python_module
                      dap_server_core_python_module
                      dap_enc_server_python_module
                      dap_server_http_python_module
                      dap_stream_python_module
                      dap_chain_python_module
                      dap_chain_net_python_module
                      dap_chain_net_srv_python_module
                      dap_mempool_python_module
                      dap_chain_gdb_python_module
                      dap_app_cli_python_module
                      dap_chain_wallet_python_module
                      dap_server_json_rpc_python_module
                      dap_chain_cs_dag_poa_python_module
                      dap_chain_cs_blocks_python_module
                      dap_chain_global_db_python_module
                      ${PYTHON_LIBRARIES}
                     )

target_include_directories(${PROJECT_NAME} INTERFACE include/)
