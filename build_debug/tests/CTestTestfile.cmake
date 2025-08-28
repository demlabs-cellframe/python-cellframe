# CMake generated Testfile for 
# Source directory: /home/naeper/work/python-cellframe/dap-sdk/tests
# Build directory: /home/naeper/work/python-cellframe/build_debug/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_unit_dap_json "/home/naeper/work/python-cellframe/build_debug/tests/bin/test_unit_dap_json")
set_tests_properties(test_unit_dap_json PROPERTIES  ENVIRONMENT "DAP_LOG_LEVEL=DEBUG" LABELS "unit;core" TIMEOUT "300" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;81;add_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;101;register_dap_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;0;")
add_test(test_unit_crypto_sha3 "/home/naeper/work/python-cellframe/build_debug/tests/bin/test_unit_crypto_sha3")
set_tests_properties(test_unit_crypto_sha3 PROPERTIES  ENVIRONMENT "DAP_LOG_LEVEL=DEBUG" LABELS "unit;crypto;hash" TIMEOUT "300" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;81;add_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;107;register_dap_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;0;")
add_test(test_integration_crypto_network "/home/naeper/work/python-cellframe/build_debug/tests/bin/test_integration_crypto_network")
set_tests_properties(test_integration_crypto_network PROPERTIES  ENVIRONMENT "DAP_LOG_LEVEL=DEBUG" LABELS "integration;crypto;network" TIMEOUT "300" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;81;add_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;115;register_dap_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;0;")
add_test(test_e2e_complete_workflow "/home/naeper/work/python-cellframe/build_debug/tests/bin/test_e2e_complete_workflow")
set_tests_properties(test_e2e_complete_workflow PROPERTIES  ENVIRONMENT "DAP_LOG_LEVEL=DEBUG" LABELS "e2e;workflow" TIMEOUT "600" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;81;add_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;123;register_dap_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;0;")
add_test(test_performance_crypto "/home/naeper/work/python-cellframe/build_debug/tests/bin/test_performance_crypto")
set_tests_properties(test_performance_crypto PROPERTIES  ENVIRONMENT "DAP_LOG_LEVEL=DEBUG" LABELS "performance;crypto;benchmark" TIMEOUT "1200" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;81;add_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;134;register_dap_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;0;")
add_test(test_functional_api "/home/naeper/work/python-cellframe/build_debug/tests/bin/test_functional_api")
set_tests_properties(test_functional_api PROPERTIES  ENVIRONMENT "DAP_LOG_LEVEL=DEBUG" LABELS "functional;api" TIMEOUT "300" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;81;add_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;145;register_dap_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;0;")
add_test(test_regression_known_issues "/home/naeper/work/python-cellframe/build_debug/tests/bin/test_regression_known_issues")
set_tests_properties(test_regression_known_issues PROPERTIES  ENVIRONMENT "DAP_LOG_LEVEL=DEBUG" LABELS "regression;bugfix" TIMEOUT "300" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;81;add_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;153;register_dap_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;0;")
add_test(test_security_memory "/home/naeper/work/python-cellframe/build_debug/tests/bin/test_security_memory")
set_tests_properties(test_security_memory PROPERTIES  ENVIRONMENT "DAP_LOG_LEVEL=DEBUG" LABELS "security;memory" TIMEOUT "300" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;81;add_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;161;register_dap_test;/home/naeper/work/python-cellframe/dap-sdk/tests/CMakeLists.txt;0;")
