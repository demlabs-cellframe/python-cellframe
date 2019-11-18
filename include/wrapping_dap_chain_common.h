#ifndef _WRAPPING_DAP_CHAIN_COMMON_
#define _WRAPPING_DAP_CHAIN_COMMON_

#include "Python.h"
#include "dap_chain_common.h"
#include "wrapping_dap_chain_common_objects.h"
#include "libdap_crypto_key_python.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapChainCommon{
    PyObject_HEAD
}PyDapChainCommonObject;

PyObject *dap_chain_hash_slow_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_str_to_hash_fast_py(PyObject *self, PyObject *args);
PyObject *dap_chain_addr_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_addr_from_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_addr_fill_py(PyObject *self, PyObject *args);
PyObject *dap_chain_addr_check_sum_py(PyObject *self, PyObject *args);

static PyMethodDef PyDapChainCommonObjectMethods[] ={
    {"hashSlowToStr", dap_chain_hash_slow_to_str_py, METH_VARARGS|METH_STATIC, ""},
    {"strTpHashFast", dap_chain_str_to_hash_fast_py, METH_VARARGS|METH_STATIC, ""},
    {"addrToStr", dap_chain_addr_to_str_py, METH_VARARGS|METH_STATIC, ""},
    {"addrFromStr", dap_chain_addr_from_str_py, METH_VARARGS|METH_STATIC, ""},
    {"addrFill", dap_chain_addr_fill_py, METH_VARARGS|METH_STATIC, ""},
    {"addrCheckSum", dap_chain_addr_check_sum_py, METH_VARARGS|METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject ChainCommonObject_ChainCommonType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.Common",                                      /* tp_name */
    sizeof(PyDapChainCommonObject),                               /* tp_basicsize */
    0,                                                            /* tp_itemsize */
    0,                                                            /* tp_dealloc */
    0,                                                            /* tp_print */
    0,                                                            /* tp_getattr */
    0,                                                            /* tp_setattr */
    0,                                                            /* tp_reserved */
    0,                                                            /* tp_repr */
    0,                                                            /* tp_as_number */
    0,                                                            /* tp_as_sequence */
    0,                                                            /* tp_as_mapping */
    0,                                                            /* tp_hash  */
    0,                                                            /* tp_call */
    0,                                                            /* tp_str */
    0,                                                            /* tp_getattro */
    0,                                                            /* tp_setattro */
    0,                                                            /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                                      /* tp_flags */
    "Chain common objects",                                              /* tp_doc */
    0,		                                                      /* tp_traverse */
    0,		                                                      /* tp_clear */
    0,		                                                      /* tp_richcompare */
    0,		                                                      /* tp_weaklistoffset */
    0,		                                                      /* tp_iter */
    0,		                                                      /* tp_iternext */
    PyDapChainCommonObjectMethods,                                /* tp_methods */
    0,                                                            /* tp_members */
    0,                                                            /* tp_getset */
    0,                                                            /* tp_base */
    0,                                                            /* tp_dict */
    0,                                                            /* tp_descr_get */
    0,                                                            /* tp_descr_set */
    0,                                                            /* tp_dictoffset */
    0,                                                            /* tp_init */
    0,                                                            /* tp_alloc */
    PyType_GenericNew,                                            /* tp_new */

};

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_COMMON_
