cmake_minimum_required(VERSION 3.0)

if (SUPPORT_PYTHON_PLUGINS)
    project(API_CellFrame C)
else()
    project(CellFrame C)
endif()
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)


set(Python_ADDITIONAL_VERSIONS 3.9 3.8 3.7 3.6 3.5 3.4)
find_package (PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR} include/)
file(GLOB PYTHON_CELLFRAME_SRCS CellFrame/*.c)
file(GLOB PYTHON_CELLFRAME_HEADERS include/*.h)

add_subdirectory(common)
add_subdirectory(chain)
add_subdirectory(net)
add_subdirectory(gdb)
add_subdirectory(app-cli)
add_subdirectory(wallet)
target_compile_options(dap_app_cli PRIVATE "-fpic")
target_compile_options(dap_chain PRIVATE "-fpic")
target_compile_options(dap_chain_global_db PRIVATE "-fpic")
target_compile_options(dap_chain_wallet PRIVATE "-fpic")
target_compile_options(dap_chain_net PRIVATE "-fpic")
target_compile_options(dap_chain_net_srv PRIVATE "-fpic")
target_compile_options(dap_chain_mempool PRIVATE "-fpic")
target_compile_options(dap_chain_cs_none PRIVATE "-fpic")
target_compile_options(dap_stream_ch_chain_net_srv PRIVATE "-fpic")
target_compile_options(dap_notify_srv PRIVATE "-fpic")
target_compile_options(dap_chain_common PRIVATE "-fpic")

if (SUPPORT_PYTHON_PLUGINS)
    add_library(${PROJECT_NAME} STATIC ${PYTHON_CELLFRAME_SRCS} ${PYTHON_CELLFRAME_HEADERS})
else()
    add_library(${PROJECT_NAME} SHARED ${PYTHON_CELLFRAME_SRCS} ${PYTHON_CELLFRAME_HEADERS})
endif()

target_link_libraries(${PROJECT_NAME} cellframe-sdk
                                      dap_chain_common_python_module
                                      dap_chain_python_module
                                      dap_chain_net_python_module
                                      dap_chain_gdb_python_module
                                      dap_app_cli_python_module
                                      dap_chain_wallet_python_module
                                      ${PYTHON_LIBRARIES})

target_include_directories(${PROJECT_NAME} INTERFACE include/)
