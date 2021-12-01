#pragma once

#include <Python.h>
#include "dap_chain_datum_tx_token.h"
#include "wrapping_dap_chain_common.h"
#include "wrapping_dap_hash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapChainTxTokenExt{
    PyObject_HEAD
    dap_chain_tx_token_ext_t *token_ext;
}PyDapChainTxTokenExtObject;


PyObject *wrapping_dap_chain_tx_token_ext_get_version(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_token_ext_get_ticker(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_token_get_chain_id(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_token_get_net_id(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_token_get_tx_hash(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_token_get_tx_out_idx(PyObject *self, void *closure);

static PyGetSetDef PyDapChainTxTokenExtGetsSetsDef[] = {
        {"version", (getter)wrapping_dap_chain_tx_token_ext_get_version, NULL, NULL, NULL},
        {"ticker", (getter)wrapping_dap_chain_tx_token_ext_get_ticker, NULL, NULL, NULL},
        {"chainId", (getter)wrapping_dap_chain_tx_token_get_chain_id, NULL, NULL, NULL},
        {"netId", (getter)wrapping_dap_chain_tx_token_get_net_id, NULL,NULL, NULL},
        {"txHash", (getter)wrapping_dap_chain_tx_token_get_tx_hash, NULL, NULL, NULL},
        {"txOutIdx", (getter)wrapping_dap_chain_tx_token_get_tx_out_idx, NULL, NULL, NULL},
        {NULL}
};

static PyTypeObject DapChainTxTokenExt_DapChainTxTokenExtType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxTokenExt",       /* tp_name */
        sizeof(PyDapChainTxTokenExtObject),      /* tp_basicsize */
        0,                                       /* tp_itemsize */
        0,                                       /* tp_dealloc */
        0,                                       /* tp_print */
        0,                                       /* tp_getattr */
        0,                                       /* tp_setattr */
        0,                                       /* tp_reserved */
        0,                                       /* tp_repr */
        0,                                       /* tp_as_number */
        0,                                       /* tp_as_sequence */
        0,                                       /* tp_as_mapping */
        0,                                       /* tp_hash  */
        0,                                       /* tp_call */
        0,                                       /* tp_str */
        0,                                       /* tp_getattro */
        0,                                       /* tp_setattro */
        0,                                       /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                 /* tp_flags */
        "Chain tx token ext object",              /* tp_doc */
        0,		                                 /* tp_traverse */
        0,		                                 /* tp_clear */
        0,		                                 /* tp_richcompare */
        0,                                       /* tp_weaklistoffset */
        0,		                                 /* tp_iter */
        0,		                                 /* tp_iternext */
        0,                                       /* tp_methods */
        0,                                       /* tp_members */
        PyDapChainTxTokenExtGetsSetsDef,                                       /* tp_getset */
        0,                                       /* tp_base */
        0,                                       /* tp_dict */
        0,                                       /* tp_descr_get */
        0,                                       /* tp_descr_set */
        0,                                       /* tp_dictoffset */
        0,                                       /* tp_init */
        0,                                       /* tp_alloc */
        PyType_GenericNew,                       /* tp_new */
};

#ifdef __cplusplus
}
#endif