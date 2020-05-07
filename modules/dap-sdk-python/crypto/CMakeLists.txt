project(dap_crypto_python_module C)
cmake_minimum_required(VERSION 2.8)

add_definitions("-fpic")
add_definitions("-DDAP_LOG_MT")

file(GLOB CRYPTO_PYTHON_SRCS ./*.c)
file(GLOB CRYPTO_PYTHON_HEADERS include/*.h)

set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
find_package (PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR} include/)

add_library(${PROJECT_NAME} STATIC ${CRYPTO_PYTHON_SRCS} ${CRYPTO_PYTHON_HEADERS})

target_link_libraries(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} cellframe-sdk) #dap_core dap_crypto)

target_include_directories(${PROJECT_NAME} PUBLIC include/ )

if(BUILD_DAP_CRYPTO_PYTHON_TESTS)
    add_subdirectory(test)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/test_b58.py 
                    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/test_b64.py
                    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/test_iaes256_cbc.py
                    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/test_iaes256_cbc.py
                    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/test_oaes.py
                    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)


endif()

