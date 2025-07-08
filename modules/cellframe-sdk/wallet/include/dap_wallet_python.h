/*
 * Authors:
 * Converted from Java JNI implementation
 * CellFrame       https://cellframe.net
 * Sources         https://gitlab.demlabs.net/cellframe
 * Copyright  (c) 2017-2022
 * All rights reserved.

 This file is part of DAP (Distributed Applications Platform) the open source project

    DAP (Distributed Applications Platform) is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DAP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with any DAP based project.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _DAP_WALLET_PYTHON_
#define _DAP_WALLET_PYTHON_

#include <Python.h>
#include "dap_common.h"
#include "dap_chain_wallet.h"
#include "dap_chain_net.h"
#include "dap_sign.h"
#include "dap_enc_key.h"
#include "dap_chain_addr.h"
#include "utils_python.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapWallet{
    PyObject_HEAD
    dap_chain_wallet_t *wallet;
} PyDapWalletObject;

// Python wrapper for dap_chain_wallet_open_file - similar to Java JNI openFile
PyObject *dap_wallet_open_file_py(PyObject *self, PyObject *args);

// Python wrapper for dap_chain_wallet_close - similar to Java JNI close
PyObject *dap_wallet_close_py(PyObject *self, PyObject *args);

// Python wrapper for dap_chain_wallet_get_certs_number - similar to Java JNI getCertsNumber
PyObject *dap_wallet_get_certs_number_py(PyObject *self, PyObject *args);

// Python wrapper for dap_chain_wallet_get_addr - similar to Java JNI getAddress
PyObject *dap_wallet_get_address_py(PyObject *self, PyObject *args);

// Python wrapper for dap_chain_wallet_create - similar to Java JNI createNative
PyObject *dap_wallet_create_py(PyObject *self, PyObject *args);

// Python wrapper for dap_chain_wallet_get_key - similar to Java JNI getEncKey
PyObject *dap_wallet_get_enc_key_py(PyObject *self, PyObject *args);

// Initialize wallet Python module
int dap_wallet_init_py(void);

// Deinitialize wallet Python module
void dap_wallet_deinit_py(void);

// Type object for wallet functions
extern PyTypeObject DapWalletObjectType;

#ifdef __cplusplus
}
#endif

#endif // _DAP_WALLET_PYTHON_ 