# libdap-python
Python binding for libdap

[![Build Status](https://travis-ci.com/cellframe/libdap-python.svg?branch=master)](https://travis-ci.com/cellframe/libdap-python)

## Build and Run tests:
```
git submodule update --init
mkdir build
cd build
cmake -DBUILD_DAP_PYTHON_TESTS=ON ../
make
cd test
python3.7 main_test.py
```
