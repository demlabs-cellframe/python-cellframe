#pragma once

#include <Python.h>
#include "dap_chain_global_db_driver.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapStore{
    PyObject_HEAD
    dap_store_obj_t *store_obj;
}PyDapStoreObj;

PyObject *dap_store_obj_copy_py(PyObject *self, PyObject *args);
PyObject *dap_store_obj_free_py(PyObject *self, PyObject *args);

PyObject *dap_chain_global_db_driver_read_last_py(PyObject *self, PyObject *argv);
PyObject *dap_chain_global_db_driver_cond_read_py(PyObject *self, PyObject *argv);//(const char *a_group, uint64_t id, size_t *a_count_out);
//PyObject *dap_chain_global_db_driver_read_py(const char *a_group, const char *a_key, size_t *count_out);

static PyMethodDef DapStoreMethods[] = {
    {"copy", dap_store_obj_copy_py, METH_VARARGS | METH_STATIC, ""},
    {"free", dap_store_obj_free_py, METH_VARARGS | METH_STATIC, ""},
    {"readLast", dap_chain_global_db_driver_read_last_py, METH_VARARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapStoreObject_DapStoreType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.DapStore",                                            /* tp_name */
        sizeof(PyDapStoreObj),                                     /* tp_basicsize */
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
        "Dap store object",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        DapStoreMethods,                                              /* tp_methods */
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
