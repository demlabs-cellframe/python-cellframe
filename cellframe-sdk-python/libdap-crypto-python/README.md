# libdap-crypto-python
libdap-crypto python binding

[![Build Status](https://travis-ci.com/cellframe/libdap-crypto-python.svg?branch=master)](https://travis-ci.com/cellframe/libdap-crypto-python)

## Build and Run tests:
```
mkdir build
cd build
cmake -BUILD_DAP_CRYPTO_PYTHON_TESTS=ON ../
make
cd test
python3.7 test_b58.py
python3.7 test_b64.py
python3.7 test_iaes256_cbc.py
python3.7 test_oaes.py
```
