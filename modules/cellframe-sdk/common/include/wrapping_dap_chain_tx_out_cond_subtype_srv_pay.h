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

#pragma once

#include <Python.h>
#include "wrapping_dap_chain_tx_out_cond.h"
#include "wrapping_dap_chain_common.h"

#ifdef __cplusplus
extern "C"{
#endif

PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_unit(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_uid(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_pkey(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_map_price(PyObject *self, void *closure);

    static PyGetSetDef DapChainTxOutCondSubtypeStvPayGetsSetsDef[]={
        {"unit", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_unit,NULL,NULL,NULL},
        {"uid", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_uid,NULL,NULL,NULL},
        {"pkeyHash", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_pkey,NULL,NULL,NULL},
        {"maxPrice", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_map_price,NULL,NULL,NULL},
        {NULL}
};

static PyTypeObject DapChainTxOutCondSubTypeSrvPay_DapChainTxOutCondSubTypeSrvPayObject = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxOutCondSubTypeSrvPay",       /* tp_name */
        sizeof(PyDapChainTxOutCondObject),/* tp_basicsize */
        0,                               /* tp_itemsize */
        0,                               /* tp_dealloc */
        0,                               /* tp_print */
        0,                               /* tp_getattr */
        0,                               /* tp_setattr */
        0,                               /* tp_reserved */
        0,                               /* tp_repr */
        0,                               /* tp_as_number */
        0,                               /* tp_as_sequence */
        0,                               /* tp_as_mapping */
        0,                               /* tp_hash  */
        0,                               /* tp_call */
        0,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASE_EXC_SUBCLASS,         /* tp_flags */
        "Chain transaction condition subtype service pay object",             /* tp_doc */
        0,                                 /* tp_traverse */
        0,                                 /* tp_clear */
        0,                                 /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,                                 /* tp_iter */
        0,                                 /* tp_iternext */
        0,       /* tp_methods */
        0,                               /* tp_members */
        DapChainTxOutCondSubtypeStvPayGetsSetsDef,                               /* tp_getset */
        &DapChainTxOutCond_DapChainTxOutCondType,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        0,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

#ifdef __cplusplus
};
#endif
