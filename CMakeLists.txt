project(CellFrame C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
set(SUBMODULES_NO_BUILD ON)

if(UNIX)
  add_definitions ("-DDAP_OS_LINUX -DDAP_OS_UNIX")
# add_definitions ("-DDAP_LOG_MT")
  if(DAP_RELEASE)
    set(_CCOPT " -Wall -O3 -fPIC -fno-ident -ffast-math -ftree-vectorize -fno-asynchronous-unwind-tables -ffunction-sections -Wl,--gc-sections -Wl,--strip-all -std=gnu11
")
  else()
    set(_CCOPT "-Wall -pg -g3 -ggdb -fno-eliminate-unused-debug-symbols")
    set(_LOPT "-pg")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
  endif()

  if (ANDROID)
      set(_CCOPT "${_CCOPT} -fforce-enable-int128 -std=gnu11")
      add_definitions ("-DDAP_OS_ANDROID -DDAP_OS_UNIX")
  endif()
    
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_CCOPT}")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${_LOPT}")
  
  if (ANDROID)
    add_subdirectory(lib/json-c)
    add_subdirectory(lib/sqlite3)
    add_subdirectory(lib/libev)
    include_directories(lib/)
  else()
  endif(ANDROID)
  
endif()
  


if(WIN32)

  add_definitions ("-DUNDEBUG")
  add_definitions ("-DNDEBUG")
  add_definitions ("-DWIN32")
  add_definitions ("-D_WINDOWS")
  add_definitions ("-D__WINDOWS__")
  add_definitions ("-D_CRT_SECURE_NO_WARNINGS")
  add_definitions ("-DCURL_STATICLIB")
  add_definitions("-DHAVE_PREAD")
  add_definitions("-DHAVE_MMAP")
  add_definitions("-DHAVE_STRNDUP")
  add_compile_definitions(WINVER=0x0600 _WIN32_WINNT=0x0600)

  if(DAP_RELEASE)
    set(_CCOPT "-static -std=gnu11 -Wall -O3 -fno-ident -ffast-math -ftree-vectorize -mfpmath=sse -mmmx -msse2 -fno-asynchronous-unwind-tables -ffunction-sections -Wl,--
gc-sections -Wl,--strip-all")
  else()
    set(_CCOPT "-mconsole -static -Wall -pg")
    set(_LOPT "-mconsole -static -pg")
  endif()

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_CCOPT}")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${_LOPT}")

  include_directories(libdap/src/win32/)
  include_directories(3rdparty/uthash/src/)
  include_directories(3rdparty/libjson-c/)
  include_directories(3rdparty/curl/include/)
  include_directories(3rdparty/libsqlite3/)
  include_directories(sources/wepoll/)
  include_directories(libdap-chain-net-srv-vpn/)
endif()


add_definitions("-DDAP_LOG_MT")
#add_definitions("-DNODE_NETNAME=\"cellframe\"")
#add_definitions("-DDAP_APP_NAME=\"pythonCellframe\"")

add_subdirectory(cellframe-sdk/libdap)
target_compile_options(dap_core PRIVATE "-fpic")
add_subdirectory(cellframe-sdk/libdap-crypto)
target_compile_options(dap_crypto PRIVATE "-fpic")
add_subdirectory(cellframe-sdk/libdap-chain)
target_compile_options(dap_chain PRIVATE "-fpic")
add_subdirectory(cellframe-sdk/libdap-chain-common)
target_compile_options(dap_chain_common PRIVATE "-fpic")
add_subdirectory(cellframe-sdk-python/libdap-chain-common-python)
target_compile_options(dap_chain_common_python_module PRIVATE "-fpic")
add_subdirectory(cellframe-sdk/libdap-chain-crypto)
add_subdirectory(cellframe-sdk/libdap-chain-wallet)
target_compile_options(dap_chain_wallet PRIVATE "-fpic" )
target_compile_options(dap_chain_crypto PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-chain-mempool)
target_compile_options(dap_chain_mempool PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-chain-net)
target_compile_options(dap_chain_net PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-server-core)
target_compile_options(dap_server_core PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk-python/libdap-server-core-python)
add_subdirectory(cellframe-sdk/libdap-chain-global-db)
target_compile_options(dap_chain_global_db PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-client)
target_compile_options(dap_client PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-server)
#target_compile_options(libdap-server PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-stream)
target_compile_options(dap_stream PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-stream-ch)
target_compile_options(dap_stream_ch PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-stream-ch-chain-net)
target_compile_options(dap_stream_ch_chain_net PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-stream-ch-chain)
target_compile_options(dap_stream_ch_chain PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-server-udp)
target_compile_options(dap_udp_server PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-chain-net-srv)
target_compile_options(dap_chain_net_srv PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-chain-net-srv-vpn)
target_compile_options(dap_chain_net_srv_vpn PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk/libdap-chain-gdb)
target_compile_options(dap_chain_gdb PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk-python/libdap-python)
target_compile_options(dap_python_module PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk-python/libdap-crypto-python)
target_compile_options(dap_crypto_python_module PRIVATE "-fpic" )
add_subdirectory(cellframe-sdk-python/libdap-chain-net-python)
target_compile_options(dap_chain_net_python_module PRIVATE "-fpic")
target_compile_options(dap_enc_server PRIVATE "-fpic")
target_compile_options(dap_http_server PRIVATE "-fpic")
target_compile_options(dap_session PRIVATE "-fpic")
add_subdirectory(cellframe-sdk/libdap-stream-ch-chain-net-srv)
target_compile_options(dap_stream_ch_chain_net_srv PRIVATE "-fpic")
add_subdirectory(cellframe-sdk-python/libdap-chain-gdb-python)
target_compile_options(dap_chain_gdb_python_module PRIVATE "-fpic")

add_subdirectory(cellframe-sdk-python/libdap-chain-python)
target_compile_options(dap_chain_python_module PRIVATE "-fpic")

add_subdirectory(cellframe-sdk-python/libdap-client-python)
target_compile_options(dap_client_python_module PRIVATE "-fpic")

add_subdirectory(cellframe-sdk/libdap-chain-cs-dag)
target_compile_options(dap_chain_cs_dag PRIVATE "-fpic")
add_subdirectory(cellframe-sdk/libdap-chain-cs-dag-poa)
target_compile_options(dap_chain_cs_dag_poa PRIVATE "-fpic")
add_subdirectory(cellframe-sdk/libdap-chain-cs-dag-pos)
target_compile_options(dap_chain_cs_dag_pos PRIVATE "-fpic")

file(GLOB PYTHON_CELLFRAME_SRCS src/*.c)
file(GLOB PYTHON_CELLFRAME_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
find_package (PythonLibs REQUIRED)

include_directories(${PYTHON_INCLUDE_DIR} include/)

if (SKBUILD)
    message(STATUS "The project is built using scikit-build")
    find_package(PythonExtensions REQUIRED)
    add_library(${PROJECT_NAME} MODULE ${PYTHON_CELLFRAME_SRCS} ${PYTHON_CELLFRAME_HEADERS})
    python_extension_module(${PROJECT_NAME})
    python_extension_module(${PROJECT_NAME} LINKED_MODULES_VAR dap_python_module)
    python_extension_module(${PROJECT_NAME} LINKED_MODULES_VAR dap_crypto_python_module)
    python_extension_module(${PROJECT_NAME} LINKED_MODULES_VAR DapServerCore)
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION CellFrame)
    install(FILES src/__init__.py DESTINATION CellFrame)
#    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libdap-python/libdap_python_module.so DESTINATION CellFrame/lib)
#    install(TARGETS dap_python_module LIBRARY DESTINATION CellFrame)
#    install(TARGETS dap_crypto_python_module LIBRARY DESTINATION CellFrame)
#    install(TARGETS DapServerCore LIBRARY DESTINATION CellFrame)
else()
    add_library(${PROJECT_NAME} SHARED ${PYTHON_CELLFRAME_SRCS} ${PYTHON_CELLFRAME_HEADERS})
endif()

    target_link_libraries(${PROJECT_NAME})

    target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})

#target_compile_options(
#    dap_core PRIVATE 
#    "-fpic"
#)

#target_compile_options(
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

target_link_libraries(${PROJECT_NAME} dap_python_module 
                      dap_crypto_python_module 
                      DapServerCore
                      dap_chain_python_module
                      dap_chain_net_python_module
                      dap_chain_gdb
                      dap_chain_gdb_python_module

                      dap_chain_cs_dag
                      dap_chain_cs_dag_pos
                      dap_chain_cs_dag_poa
                      dap_stream_ch_chain_net_srv
                     )
#target_link_libraries(${PROJECT_NAME} dap_core dap_crypto)

target_include_directories(${PROJECT_NAME} INTERFACE include/)


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
