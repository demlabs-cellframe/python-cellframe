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

static PyMethodDef PyDapChainTxItemTypeObjectMethods[] ={
    {"TX_ITEM_TYPE_IN", (PyCFunction)TX_ITEM_TYPE_IN_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_OUT", (PyCFunction)TX_ITEM_TYPE_OUT_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_PKEY", (PyCFunction)TX_ITEM_TYPE_PKEY_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_SIG", (PyCFunction)TX_ITEM_TYPE_SIG_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_TOKEN", (PyCFunction)TX_ITEM_TYPE_TOKEN_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_IN_COND", (PyCFunction)TX_ITEM_TYPE_IN_COND_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_OUT_COND", (PyCFunction)TX_ITEM_TYPE_OUT_COND_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_RECEIPT", (PyCFunction)TX_ITEM_TYPE_RECEIPT_PY,
                                                            METH_NOARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainTxItemObject_DapChainTxItemTypeObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainTxItemType",        /* tp_name */
    sizeof(PyDapChainTxItemTypeObject), /* tp_basicsize */
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
    "Chain transaction item type object",        /* tp_doc */
    0,		                            /* tp_traverse */
    0,		                            /* tp_clear */
    0,		                            /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,		                            /* tp_iter */
    0,		                            /* tp_iternext */
    PyDapChainTxItemTypeObjectMethods,  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    PyType_GenericNew,                  /* tp_new */
};

/* -------------------------------------- */

/* DAP chain tx cond type */
typedef struct PyDapChainTxCondType{
    PyObject_HEAD
    dap_chain_tx_cond_type_t tx_cond_type_t;
}PyDapChainTxCondTypeObject;

PyObject *COND_SERVICE_PROVIDE_PY();
PyObject *COND_SERVICE_BILL_PY();

static PyMethodDef DapChainTxCondTypeMethods[] = {
    {"COND_SERVICE_PROVIDE", COND_SERVICE_PROVIDE_PY, METH_NOARGS | METH_STATIC, ""},
    {"COND_SERVICE_BILL", COND_SERVICE_BILL_PY, METH_NOARGS | METH_STATIC, ""},
    {NULL, NULL,0, NULL}
};

static PyTypeObject DapChainTxCondType_DapChainTxCondTypeObject = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainTxCondType",       /* tp_name */
    sizeof(PyDapChainTxCondTypeObject),/* tp_basicsize */
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
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
    "Chain transaction condition type object",             /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainTxCondTypeMethods,       /* tp_methods */
    0,                               /* tp_members */
    0,                               /* tp_getset */
    0,                               /* tp_base */
    0,                               /* tp_dict */
    0,                               /* tp_descr_get */
    0,                               /* tp_descr_set */
    0,                               /* tp_dictoffset */
    0,                               /* tp_init */
    0,                               /* tp_alloc */
    PyType_GenericNew,               /* tp_new */
};

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

static PyGetSetDef PyDaoChainDatumTxObjectGetsSets[] = {
        {"hash", (getter) wrapping_dap_chain_datum_tx_get_hash, NULL, NULL, NULL},
        {"dateCreated", (getter) wrapping_dap_chain_datum_tx_get_tsCreated, NULL, NULL, NULL},
        {NULL}
};

static PyMethodDef PyDapChainDatumTxObjectMethods[] ={
    {"getSize", (PyCFunction)dap_chain_datum_tx_get_size_py, METH_VARARGS, ""},
    {"addItem", (PyCFunction)dap_chain_datum_tx_add_item_py, METH_VARARGS, ""},
    {"addInItem", (PyCFunction)dap_chain_datum_tx_add_in_item_py, METH_VARARGS, ""},
    {"addInCondItem", (PyCFunction)dap_chain_datum_tx_add_in_cond_item_py, METH_VARARGS, ""},
    {"addOutItem", (PyCFunction)dap_chain_datum_tx_add_out_item_py, METH_VARARGS, ""},
    {"addOutCond", (PyCFunction)dap_chain_datum_tx_add_out_cond_item_py, METH_VARARGS, ""},
    {"addSignItem", (PyCFunction)dap_chain_datum_tx_add_sign_item_py, METH_VARARGS, ""},
    {"verifySign", (PyCFunction)dap_chain_datum_tx_verify_sign_py, METH_VARARGS, ""},
    {"getItems", (PyCFunction)wrapping_dap_chain_datum_tx_get_items, METH_NOARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainDatumTx_DapChainDatumTxObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainDatumTx",                      /* tp_name */
    sizeof(PyDapChainDatumTxObject),               /* tp_basicsize */
    0,                                             /* tp_itemsize */
    (destructor)PyDapChainDatumTxObject_delete,    /* tp_dealloc */
    0,                                              /* tp_print */
    0,                                              /* tp_getattr */
    0,                                              /* tp_setattr */
    0,                                              /* tp_reserved */
    0,                                              /* tp_repr */
    0,                                              /* tp_as_number */
    0,                                              /* tp_as_sequence */
    0,                                              /* tp_as_mapping */
    0,                                              /* tp_hash  */
    0,                                              /* tp_call */
    0,                                              /* tp_str */
    0,                                              /* tp_getattro */
    0,                                              /* tp_setattro */
    0,                                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                        /* tp_flags */
    "Chain datum transaction object",                        /* tp_doc */
    0,		                                        /* tp_traverse */
    0,		                                        /* tp_clear */
    0,		                                        /* tp_richcompare */
    0,                                              /* tp_weaklistoffset */
    0,		                                        /* tp_iter */
    0,		                                        /* tp_iternext */
    PyDapChainDatumTxObjectMethods,                 /* tp_methods */
    0,                                              /* tp_members */
    PyDaoChainDatumTxObjectGetsSets,                /* tp_getset */
    0,                                              /* tp_base */
    0,                                              /* tp_dict */
    0,                                              /* tp_descr_get */
    0,                                              /* tp_descr_set */
    0,                                              /* tp_dictoffset */
    0,                                              /* tp_init */
    0,                                              /* tp_alloc */
    PyDapChainDatumTxObject_create,                 /* tp_new */
};

/* -------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_DATUM_TX_
