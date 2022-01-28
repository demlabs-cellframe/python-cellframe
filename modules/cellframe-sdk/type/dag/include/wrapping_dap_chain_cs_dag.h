#pragma once

#include <Python.h>
#include "dap_chain_cs_dag.h"
#include "wrapping_dap_chain_cs_dag_event.h"
#include "wrapping_dap_hash.h"
#include "wrapping_dap_chain_atom_ptr.h"
#include "libdap_chain_atom_iter_python.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapChainCsDag {
    PyObject_HEAD
    dap_chain_cs_dag_t *dag;
} PyDapChainCsDagObject;

PyObject *dap_chain_cs_dag_find_event_by_hash_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainCsDagMethods[] = {
        {"findByHash", (PyCFunction)dap_chain_cs_dag_find_event_by_hash_py, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainCsDag_DapChainCsDagType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainCsDag",                                            /* tp_name */
        sizeof(PyDapChainCsDagObject),                                     /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        0,                         /* tp_dealloc */
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
        "Chain cs dag objects",                                              /* tp_doc */
        0,                                                              /* tp_traverse */
        0,                                                              /* tp_clear */
        0,                                                              /* tp_richcompare */
        0,                                                              /* tp_weaklistoffset */
        0,                                                              /* tp_iter */
        0,                                                            /* tp_iternext */
        DapChainCsDagMethods,                                        /* tp_methods */
        0,                                                          /* tp_members */
        0,                                                           /* tp_getset */
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
};
#endif
