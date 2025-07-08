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

#ifndef _DAP_CHAIN_DATUM_TX_PYTHON_
#define _DAP_CHAIN_DATUM_TX_PYTHON_

#include <Python.h>
#include "dap_common.h"
#include "dap_chain_datum_tx.h"
#include "dap_chain_net_tx.h"
#include "json.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapChainDatumTx{
    PyObject_HEAD
    dap_chain_datum_tx_t *datum_tx;
} PyDapChainDatumTxObject;

// Python wrapper for dap_chain_datum_tx_delete - similar to Java JNI close
PyObject *dap_chain_datum_tx_close_py(PyObject *self, PyObject *args);

// Python wrapper for dap_chain_net_tx_create_by_json - similar to Java JNI fromJSON
PyObject *dap_chain_datum_tx_from_json_py(PyObject *self, PyObject *args);

// Python wrapper for dap_chain_net_tx_to_json - similar to Java JNI toJSON
PyObject *dap_chain_datum_tx_to_json_py(PyObject *self, PyObject *args);

// Initialize datum tx Python module
int dap_chain_datum_tx_init_py(void);

// Deinitialize datum tx Python module
void dap_chain_datum_tx_deinit_py(void);

// Type object for datum tx functions
extern PyTypeObject DapChainDatumTxObjectType;

#ifdef __cplusplus
}
#endif

#endif // _DAP_CHAIN_DATUM_TX_PYTHON_ 