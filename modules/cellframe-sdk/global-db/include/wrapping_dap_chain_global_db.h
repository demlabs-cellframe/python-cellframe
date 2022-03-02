#pragma once

#include "Python.h"
#include "dap_chain_global_db.h"
#include "dap_strfuncs.h"

typedef struct PyDapChainGlobalDB{
    PyObject_HEAD
}PyDapChainGlobalDBObject;

PyObject *wrapping_dap_chain_global_db_gr_get(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_global_db_gr_set(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_global_db_gr_del(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_global_db_gr_pin(PyObject *self, PyObject *args);

static PyMethodDef DapChainGlobalDB_Methods[] = {
        {"get", (PyCFunction)wrapping_dap_chain_global_db_gr_get, METH_VARARGS | METH_STATIC, ""},
        {"set", (PyCFunction)wrapping_dap_chain_global_db_gr_set, METH_VARARGS | METH_STATIC, ""},
        {"delete", (PyCFunction)wrapping_dap_chain_global_db_gr_del, METH_VARARGS | METH_STATIC, ""},
        {"pin", (PyCFunction)wrapping_dap_chain_global_db_gr_pin, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainGlobalDBObject_DapChainDlobalDBType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainGlobalDB",                                            /* tp_name */
        sizeof(PyDapChainGlobalDBObject),                                     /* tp_basicsize */
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
        "Chain GlobalDB object",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        DapChainGlobalDB_Methods,                                      /* tp_methods */
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
