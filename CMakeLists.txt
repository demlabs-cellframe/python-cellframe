project(CellFrame C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_C_STANDARD 11)
set(SUBMODULES_NO_BUILD ON)

add_subdirectory(libdap)
add_subdirectory(libdap-python)
add_subdirectory(libdap-crypto-python)
add_subdirectory(libdap-server-core-python)
#add_subdirectory(libdap)
#add_subdirectory(libdap-crypto)

file(GLOB PYTHON_CELLFRAME_SRCS src/*.c)
file(GLOB PYTHON_CELLFRAME_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7)
find_package (PythonLibs REQUIRED)
#find_package(PkgConfig)
#pkg_check_modules(PC_JSON-C REQUIRED json-c)
include_directories(${PYTHON_INCLUDE_DIR} include/)

add_library(${PROJECT_NAME} SHARED ${PYTHON_CELLFRAME_SRCS} ${PYTHON_CELLFRAME_HEADERS})

target_link_libraries(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})

#target_compile_options(
#    dap_core PRIVATE 
#    "-fpic"
#)

target_link_libraries(${PROJECT_NAME} dap_python_module 
                      dap_crypto_python_module 
                      DapServerCore
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

