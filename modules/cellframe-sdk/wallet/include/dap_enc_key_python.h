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

#ifndef _DAP_ENC_KEY_PYTHON_
#define _DAP_ENC_KEY_PYTHON_

#include <Python.h>
#include "dap_common.h"
#include "dap_enc_key.h"
#include "dap_chain_wallet.h"
#include "utils_python.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapEncKey{
    PyObject_HEAD
    dap_enc_key_t *enc_key;
} PyDapEncKeyObject;

// Python wrapper for dap_enc_key_delete - similar to Java JNI close
PyObject *dap_enc_key_close_py(PyObject *self, PyObject *args);

// Python wrapper for getting key type - similar to Java JNI nativeGetType
PyObject *dap_enc_key_get_type_py(PyObject *self, PyObject *args);

// Python wrapper for dap_enc_key_new_generate - similar to Java JNI createFromSeed
PyObject *dap_enc_key_create_from_seed_py(PyObject *self, PyObject *args);

// Initialize enc key Python module
int dap_enc_key_init_py(void);

// Deinitialize enc key Python module
void dap_enc_key_deinit_py(void);

// Type object for enc key functions
extern PyTypeObject DapEncKeyObjectType;

#ifdef __cplusplus
}
#endif

#endif // _DAP_ENC_KEY_PYTHON_ 