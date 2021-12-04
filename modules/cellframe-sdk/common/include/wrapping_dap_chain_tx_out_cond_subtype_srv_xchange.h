#pragma once

#include <Python.h>
#include "wrapping_dap_chain_tx_out_cond.h"
#include "wrapping_dap_chain_common.h"

#ifdef __cplusplus
extern "C"{
#endif

PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_uid(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_net_id(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_token(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_value(PyObject *self, void *closure);

static PyGetSetDef DapChainTxOutCondSubtypeSrvXchangeGetsSetsDef[]={
        {"uid", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_uid,NULL,NULL,NULL},
        {"netId", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_net_id,NULL,NULL,NULL},
        {"token", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_token,NULL,NULL,NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_value,NULL,NULL,NULL},
        {NULL}
};

static PyTypeObject DapChainTxOutCondSubTypeSrvXchange_DapChainTxOutCondSubTypeSrvXchangeObject = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxOutCondSubTypeSrvXchange",       /* tp_name */
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
        "Chain tx cond subtype srv xchange object",             /* tp_doc */
        0,                                 /* tp_traverse */
        0,                                 /* tp_clear */
        0,                                 /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,                                 /* tp_iter */
        0,                                 /* tp_iternext */
        0,       /* tp_methods */
        0,                               /* tp_members */
        DapChainTxOutCondSubtypeSrvXchangeGetsSetsDef,                               /* tp_getset */
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

