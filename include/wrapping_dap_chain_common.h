#ifndef _WRAPPING_DAP_CHAIN_COMMON_
#define _WRAPPING_DAP_CHAIN_COMMON_
#include <Python.h>
#include "dap_chain_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Chain hash slow */
typedef struct PyDapChainHashSlow{
    PyObject_HEAD
    dap_chain_hash_slow_t *hash_slow;
}PyDapChainHashSlowObject;

PyObject *dap_chain_hash_slow_to_str_py(PyObject *self, PyObject *args);

PyMethodDef DapChainHashSlowMethod[] = {
    {"toStr", (PyCFunction)dap_chain_hash_slow_to_str_py, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainHashSlowObject_DapChainHashSlowObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.HashSlow",       /* tp_name */
    sizeof(PyDapChainHashSlowObject),/* tp_basicsize */
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
    "Chain hash slow object",        /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainHashSlowMethod,                               /* tp_methods */
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

/* Chain addr */
typedef struct PyDapChainAddr{
    PyObject_HEAD
    dap_chain_addr_t *addr;
}PyDapChainAddrObject;

PyObject *dap_chain_addr_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_addr_from_str_py(PyObject *self, PyObject *args);

PyMethodDef DapChainAddrMethods[] = {
    {"toStr", (PyCFunction)dap_chain_addr_to_str_py, METH_VARARGS, ""},
    {"fromStr", (PyCFunction)dap_chain_addr_from_str_py, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainAddrObject_DapChainAddrObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.Addr",       /* tp_name */
    sizeof(PyDapChainAddrObject),/* tp_basicsize */
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
    "Chain addr object",             /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainAddrMethods,             /* tp_methods */
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

/* Chain net id */

typedef struct PyDapChainNetId{
    PyObject_HEAD
    dap_chain_net_id_t net_id;
}PyDapChainNetIdObject;

PyObject *dap_chain_net_id_from_str_py(PyObject *self, PyObject *args);

PyMethodDef DapChainNetIdObject[] = {
    {"fromStr", (PyCFunction)dap_chain_net_id_from_str_py, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainNetIdObject_DapChainNetIdObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.NetID",          /* tp_name */
    sizeof(PyDapChainNetIdObject),   /* tp_basicsize */
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
    "Chain net id object",           /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    0,                               /* tp_methods */
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


#ifdef __cplusplus
}
#endif
#endif //_WRAPPING_DAP_CHAIN_COMMON_
