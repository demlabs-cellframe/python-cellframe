#pragma once

#include <Python.h>
#include "dap_chain.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyChainAtomPtr{
    PyObject_HEAD
    dap_chain_atom_ptr_t ptr;
}PyChainAtomPtrObject;

static PyMethodDef DapChainAtomPtrMethods[] = {
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainAtomPtr_DapChainAtomPtrType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Chain.AtomPtr",     /* tp_name */
        sizeof(PyChainAtomPtrObject),       /* tp_basicsize */
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
        "Chain atom ptr objects",           /* tp_doc */
        0,		                            /* tp_traverse */
        0,		                            /* tp_clear */
        0,		                            /* tp_richcompare */
        0,		                            /* tp_weaklistoffset */
        0,		                            /* tp_iter */
        0,		                            /* tp_iternext */
        DapChainAtomPtrMethods,             /* tp_methods */
        0,                                  /* tp_members */
        0,                                  /* tp_getset */
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
}
#endif