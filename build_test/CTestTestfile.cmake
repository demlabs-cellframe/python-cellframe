# CMake generated Testfile for 
# Source directory: /home/naeper/work/python-cellframe
# Build directory: /home/naeper/work/python-cellframe/build_test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(install_native_module_setup "/usr/bin/cmake" "--build" "/home/naeper/work/python-cellframe/build_test" "--target" "install_native_to_test_env")
set_tests_properties(install_native_module_setup PROPERTIES  FIXTURES_SETUP "install_native_module" TIMEOUT "30" WORKING_DIRECTORY "/home/naeper/work/python-cellframe/build_test" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/CMakeLists.txt;465;add_test;/home/naeper/work/python-cellframe/CMakeLists.txt;0;")
add_test(test_unit "bash" "-c" "source /home/naeper/work/python-cellframe/venv_test/bin/activate_test && python -m pytest unit/ -v --tb=short")
set_tests_properties(test_unit PROPERTIES  FIXTURES_REQUIRED "install_native_module" RUN_SERIAL "TRUE" TIMEOUT "600" WORKING_DIRECTORY "/home/naeper/work/python-cellframe/tests" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/CMakeLists.txt;435;add_test;/home/naeper/work/python-cellframe/CMakeLists.txt;475;add_pytest_test;/home/naeper/work/python-cellframe/CMakeLists.txt;0;")
add_test(test_unit_legacy "bash" "-c" "source /home/naeper/work/python-cellframe/venv_test/bin/activate_test && python -m pytest unit/legacy/ -v --tb=short")
set_tests_properties(test_unit_legacy PROPERTIES  FIXTURES_REQUIRED "install_native_module" RUN_SERIAL "TRUE" TIMEOUT "600" WORKING_DIRECTORY "/home/naeper/work/python-cellframe/tests" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/CMakeLists.txt;435;add_test;/home/naeper/work/python-cellframe/CMakeLists.txt;476;add_pytest_test;/home/naeper/work/python-cellframe/CMakeLists.txt;0;")
add_test(test_unit_composer "bash" "-c" "source /home/naeper/work/python-cellframe/venv_test/bin/activate_test && python -m pytest unit/composer/ -v --tb=short")
set_tests_properties(test_unit_composer PROPERTIES  FIXTURES_REQUIRED "install_native_module" RUN_SERIAL "TRUE" TIMEOUT "600" WORKING_DIRECTORY "/home/naeper/work/python-cellframe/tests" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/CMakeLists.txt;435;add_test;/home/naeper/work/python-cellframe/CMakeLists.txt;477;add_pytest_test;/home/naeper/work/python-cellframe/CMakeLists.txt;0;")
add_test(test_quick_functionality "/usr/bin/cmake" "-E" "env" "PYTHONPATH=/home/naeper/work/python-cellframe:/home/naeper/work/python-cellframe/venv_test/lib/python3.11/site-packages:" "CELLFRAME_TEST_MODE=1" "/home/naeper/work/python-cellframe/quick_test.sh")
set_tests_properties(test_quick_functionality PROPERTIES  RUN_SERIAL "TRUE" TIMEOUT "120" WORKING_DIRECTORY "/home/naeper/work/python-cellframe" _BACKTRACE_TRIPLES "/home/naeper/work/python-cellframe/CMakeLists.txt;501;add_test;/home/naeper/work/python-cellframe/CMakeLists.txt;0;")
subdirs("dap-sdk-build")
subdirs("cellframe-sdk-embedded")
subdirs("python-dap")
