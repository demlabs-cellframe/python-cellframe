#pragma once

#include <Python.h>
#include "wrapping_dap_chain_tx_out_ext.h"
#include "wrapping_dap_chain_common.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapChainTXOutExt{
    PyObject_HEAD
    dap_chain_tx_out_ext_t *out_ext;
}PyDapChainTXOutExtObject;

PyObject *wrapping_dap_chain_tx_out_ext_get_addr(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_out_ext_get_token(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_out_ext_get_value(PyObject *self, void *closure);

static PyGetSetDef DapChainTxOutExtGetsSetsDef[] = {
        {"addr", (getter)wrapping_dap_chain_tx_out_ext_get_addr, NULL, NULL, NULL},
        {"token", (getter)wrapping_dap_chain_tx_out_ext_get_token, NULL, NULL, NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_ext_get_value, NULL, NULL, NULL},
        {NULL}
};

static PyMethodDef PyDapChainTxOutExtObjectMethods[] ={
        {NULL, NULL, 0, NULL}
};


static PyTypeObject DapChainTxOutExtObject_DapChainTxOutExtTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxOutExt",        /* tp_name */
        sizeof(PyDapChainTXOutExtObject), /* tp_basicsize */
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
        "Chain tx out ext object",        /* tp_doc */
        0,		                            /* tp_traverse */
        0,		                            /* tp_clear */
        0,		                            /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,		                            /* tp_iter */
        0,		                            /* tp_iternext */
        PyDapChainTxOutExtObjectMethods,  /* tp_methods */
        0,                                  /* tp_members */
        DapChainTxOutExtGetsSetsDef,                                  /* tp_getset */
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
