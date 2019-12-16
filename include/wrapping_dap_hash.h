#ifndef _WRAPPING_DAP_HASH_
#define _WRAPPING_DAP_HASH_

#include <Python.h>
#include "dap_hash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapHashType{
    PyObject_HEAD
    dap_hash_type_t hash_type;
}PyDapHashTypeObject;

PyObject *DAP_HASH_TYPE_KECCAK_PY();
PyObject *DAP_HASH_TYPE_SLOW_0_PY();

static PyMethodDef DapHashTypeMethods[] = {
    {"DAP_HASH_TYPE_KECCAK", (PyCFunction)DAP_HASH_TYPE_KECCAK_PY, METH_NOARGS | METH_STATIC, ""},
    {"DAP_HASH_TYPE_SLOW_0", (PyCFunction)DAP_HASH_TYPE_SLOW_0_PY, METH_NOARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapHashTypeObject_DapChainHashTypeObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.HashType",          /* tp_name */
    sizeof(PyDapHashTypeObject),   /* tp_basicsize */
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
    "Hash type object",           /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapHashTypeMethods,         /* tp_methods */
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

/*=================*/

/* Chain hash fast */
typedef struct PyDapHashFast{
    PyObject_HEAD
    dap_chain_hash_fast_t *hash_fast;
}PyDapHashFastObject;

PyObject *dap_chain_str_to_hash_fast_py(PyObject *self, PyObject *args);
PyObject *dap_hash_fast_py(PyObject *self, PyObject *args);
PyObject *dap_hash_fast_compare_py(PyObject *self, PyObject *args);
PyObject *dap_hash_fast_is_blank_py(PyObject *self, PyObject *args);
PyObject *dap_chain_hash_fast_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_hash_fast_to_str_new_py(PyObject *self, PyObject *args);

static PyMethodDef DapHashFastMethods[] = {
    {"strToHashFast", (PyCFunction)dap_chain_str_to_hash_fast_py, METH_VARARGS | METH_STATIC, ""},
    {"hashFast", (PyCFunction)dap_hash_fast_py, METH_VARARGS, ""},
    {"compare", (PyCFunction)dap_hash_fast_compare_py, METH_VARARGS | METH_STATIC, ""},
    {"isBlank", (PyCFunction)dap_hash_fast_is_blank_py, METH_VARARGS, ""},
    {"toStr", (PyCFunction)dap_chain_hash_fast_to_str_py, METH_VARARGS, ""},
    {"toStrNew", (PyCFunction)dap_chain_hash_fast_to_str_new_py, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapHashFastObject_DapHashFastObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.HashFast",          /* tp_name */
    sizeof(PyDapHashFastObject),   /* tp_basicsize */
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
    "Hash fast object",           /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapHashFastMethods,         /* tp_methods */
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

#ifdef __cplusplus
}
#endif

#endif // _WRAPPING_DAP_HASH_
