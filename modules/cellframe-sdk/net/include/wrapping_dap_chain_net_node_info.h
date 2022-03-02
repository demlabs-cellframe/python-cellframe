#ifndef _WRAPPING_DAP_CHAIN_NODE_INFO
#define _WRAPPING_DAP_CHAIN_NODE_INFO
#include <Python.h>
#include "dap_chain_node.h"
#include "libdap_chain_net_python.h"
#include "wrapping_dap_chain_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapChainNodeInfo{
    PyObject_HEAD
    dap_chain_node_info_t *node_info;
}PyDapChainNodeInfoObject;

PyObject *dap_chain_node_info_save_py(PyObject *self, PyObject *args);
PyObject *dap_chain_node_info_read_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainNetNodeInfoMethods[] = {
    {"save", dap_chain_node_info_save_py, METH_VARARGS, ""},
    {"read", dap_chain_node_info_read_py, METH_VARARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainNodeInfoObject_DapChainNodeInfoObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainNodeInfo",            /* tp_name */
    sizeof(PyDapChainNodeInfoObject),     /* tp_basicsize */
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
    "Chain net node info object",              /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainNetNodeInfoMethods,              /* tp_methods */
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

#endif //_WRAPPING_DAP_CHAIN_NODE_INFO
