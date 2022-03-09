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

#ifndef _WRAPPING_DAP_CHAIN_TX_IN_
#define _WRAPPING_DAP_CHAIN_TX_IN_

#include <Python.h>
#include "dap_chain_datum_tx_in.h"
#include "wrapping_dap_hash.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapChainTXIn{
    PyObject_HEAD
    dap_chain_tx_in_t *tx_in;
}PyDapChainTXInObject;

PyObject *wrapping_dap_chain_tx_in_get_prev_hash(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_in_get_out_prev_idx(PyObject *self, void *closure);

static PyGetSetDef DapChainTxGetsSetsDef[] = {
        {"prevHash", (getter)wrapping_dap_chain_tx_in_get_prev_hash, NULL, NULL, NULL},
        {"prevIdx", (getter)wrapping_dap_chain_tx_in_get_out_prev_idx, NULL, NULL, NULL},
        {NULL}
};

static PyTypeObject DapChainTxInObject_DapChainTxInTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxIn",        /* tp_name */
        sizeof(PyDapChainTXInObject), /* tp_basicsize */
        0,                                  /* tp_itemsize */
        0,                                  /* tp_dealloc */
        0,                                  /* tp_print */
        0,                                  /* tp_getattr */
        0,                                  /* tp_setattr */
        0,                                  /* tp_reserved */
        0,                                  /* tp_repr */
        0,                                  /* tp_as_number */
        0,                                  /* tp_as_sequence */
        0,                                  /* tp_as_mapping */
        0,                                  /* tp_hash  */
        0,                                  /* tp_call */
        0,                                  /* tp_str */
        0,                                  /* tp_getattro */
        0,                                  /* tp_setattro */
        0,                                  /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
        "Chain transaction in object",        /* tp_doc */
        0,		                            /* tp_traverse */
        0,		                            /* tp_clear */
        0,		                            /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,		                            /* tp_iter */
        0,		                            /* tp_iternext */
        0,  /* tp_methods */
        0,                                  /* tp_members */
        DapChainTxGetsSetsDef,                                  /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        0,                                  /* tp_init */
        0,                                  /* tp_alloc */
        PyType_GenericNew,                  /* tp_new */
};

#ifdef __cplusplus
extern "C"{
#endif

#endif //_WRAPPING_DAP_CHAIN_TX_IN_