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

#ifndef _DAP_SIGNER_PYTHON_
#define _DAP_SIGNER_PYTHON_

#include <Python.h>
#include "dap_common.h"
#include "dap_enc_key.h"
#include "dap_chain_wallet.h"
#include "dap_chain_net.h"
#include "dap_chain_datum_tx.h"
#include "dap_chain_datum_tx_python.h"
#include "dap_enc_key_python.h"

#ifdef __cplusplus
extern "C"{
#endif

// Python wrapper for dap_chain_datum_tx_add_sign_item - similar to Java JNI signDatumTx
PyObject *dap_signer_sign_datum_tx_py(PyObject *self, PyObject *args);

// Initialize signer Python module
int dap_signer_init_py(void);

// Deinitialize signer Python module
void dap_signer_deinit_py(void);

// Type object for signer functions
extern PyTypeObject DapSignerObjectType;

#ifdef __cplusplus
}
#endif

#endif // _DAP_SIGNER_PYTHON_ 