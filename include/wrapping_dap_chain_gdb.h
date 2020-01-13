#ifndef _WRAPPING_DAP_CHAIN_GDB_
#define _WRAPPING_DAP_CHAIN_GDB_
#include <Python.h>
#include "dap_chain_gdb.h"
#include "libdap-chain-python.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapChainGDB{
    PyObject_HEAD
}PyDapChainGDBObject;

int dap_chain_gdb_init_py(void);

PyObject* dap_chain_gdb_new_py(PyObject* self, PyObject *args);

PyObject *dap_chain_gdb_delete_py(PyObject *self, PyObject *args);
PyObject *dap_chain_gdb_get_group_py(PyObject *self, PyObject *args);

PyMethodDef DapChainGDBMethod[] = {
    {"new", (PyCFunction)dap_chain_gdb_new_py, METH_VARARGS | METH_STATIC, ""},
    {"delete", (PyCFunction)dap_chain_gdb_delete_py, METH_VARARGS | METH_STATIC, ""},
    {"getGroup", (PyCFunction)dap_chain_gdb_get_group_py, METH_VARARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainGDBObject_DapChainGDBType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainGDB",                                            /* tp_name */
    sizeof(PyDapChainGDBObject),                                     /* tp_basicsize */
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
    "Chain gdb object",                                              /* tp_doc */
    0,		                                                      /* tp_traverse */
    0,		                                                      /* tp_clear */
    0,		                                                      /* tp_richcompare */
    0,		                                                      /* tp_weaklistoffset */
    0,		                                                      /* tp_iter */
    0,		                                                      /* tp_iternext */
    DapChainGDBMethod,                                              /* tp_methods */
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
#endif // _WRAPPING_DAP_CHAIN_GDB_
