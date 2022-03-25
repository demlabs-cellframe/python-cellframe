/*
 * Authors:
 * Alexey V. Stratulat <alexey.stratulat@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://cellframe.net
 * Sources         https://gitlab.demlabs.net/cellframe
 * Copyright  (c) 2017-2021
 * All rights reserved.

 This file is part of DAP (Deus Applications Prototypes) the open source project

    DAP (Deus Applicaions Prototypes) is free software: you can redistribute it and/or modify
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

#ifndef _WRAPPING_DAP_CHAIN_DATUM_TX_
#define _WRAPPING_DAP_CHAIN_DATUM_TX_

#include "Python.h"
#include "datetime.h"
#include "wrapping_dap_chain_common.h"
#include "libdap_crypto_key_python.h"
#include "dap_chain_datum_tx_out_cond.h"
#include "wrapping_dap_hash.h"
#include "dap_chain_datum_tx_items.h"
#include "wrapping_dap_hash.h"
#include "wrapping_dap_chain_tx_in.h"
#include "wrapping_dap_chain_tx_in_cond.h"
#include "wrapping_dap_chain_tx_out.h"
#include "wrapping_dap_chain_tx_out_cond.h"
#include "wrapping_dap_chain_tx_out_cond_subtype_srv_pay.h"
#include "wrapping_dap_chain_tx_out_cond_subtype_srv_stake.h"
#include "wrapping_dap_chain_tx_out_cond_subtype_srv_xchange.h"
#include "wrapping_dap_chain_tx_out_ext.h"
#include "wrapping_dap_chain_tx_pkey.h"
#include "wrapping_dap_chain_tx_sig.h"
#include "wrapping_dap_chain_tx_receipt.h"
#include "wrapping_dap_chain_tx_token.h"
#include "wrapping_dap_chain_tx_token_ext.h"
#include "libdap_crypto_key_python.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DAP chain tx iter type */
typedef struct PyDapChainTxItemType{
    PyObject_HEAD
}PyDapChainTxItemTypeObject;

PyObject *TX_ITEM_TYPE_IN_PY(void);
PyObject *TX_ITEM_TYPE_OUT_PY(void);
PyObject *TX_ITEM_TYPE_PKEY_PY(void);
PyObject *TX_ITEM_TYPE_SIG_PY(void);
PyObject *TX_ITEM_TYPE_TOKEN_PY(void);
PyObject *TX_ITEM_TYPE_IN_COND_PY(void);
PyObject *TX_ITEM_TYPE_OUT_COND_PY(void);
PyObject *TX_ITEM_TYPE_RECEIPT_PY(void);

extern PyTypeObject DapChainTxItemTypeObjectType;

/* -------------------------------------- */

/* DAP chain tx cond type */
typedef struct PyDapChainTxCondType{
    PyObject_HEAD
    dap_chain_tx_cond_type_t tx_cond_type_t;
}PyDapChainTxCondTypeObject;

PyObject *COND_SERVICE_PROVIDE_PY();
PyObject *COND_SERVICE_BILL_PY();

extern PyTypeObject DapChainTxCondTypeObjectType;

/* -------------------------------------- */

/* DAP chain datum tx */
typedef struct PyDapChainDatumTx{
    PyObject_HEAD
    dap_chain_datum_tx_t *datum_tx;
    bool original;
}PyDapChainDatumTxObject;

PyObject *PyDapChainDatumTxObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds);
void PyDapChainDatumTxObject_delete(PyDapChainDatumTxObject* datumTx);
PyObject *dap_chain_datum_tx_get_size_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_add_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_add_in_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_add_in_cond_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_add_out_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_add_out_cond_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_add_sign_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_verify_sign_py(PyObject *self, PyObject *args);

PyObject *wrapping_dap_chain_datum_tx_get_items(PyObject *self, PyObject *args);

PyObject *wrapping_dap_chain_datum_tx_get_hash(PyObject *self, void* closure);
PyObject *wrapping_dap_chain_datum_tx_get_tsCreated(PyObject *self, void* closure);

PyObject *wrapping_dap_chain_datum_tx_out_create(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_datum_tx_sign_create(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_datum_tx_out_cond_create(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_datum_tx_out_ext_create(PyObject *self, PyObject *args);

extern PyTypeObject DapChainDatumTxObjectType;

/* -------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_DATUM_TX_
