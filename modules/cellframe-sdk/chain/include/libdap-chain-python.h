#pragma once

#define PY_SSIZE_T_CLEAN

#include "Python.h"
//#define DAP_APP_NAME "BINDING_CHAIN_PYTHON"
#include "dap_chain.h"
#include "dap_chain_node_client.h"
#include "wrapping_dap_chain_ledger.h"
#include "wrapping_dap_chain_common.h"
#include "libdap_chain_atom_iter_python.h"
#include "wrapping_dap_chain_atom_ptr.h"
#include "wrapping_dap_chain_datum.h"
#include "wrapping_dap_chain_cell.h"
#include "wrapping_dap_chain_cs_dag.h"


typedef struct PyDapChain{
    PyObject_HEAD
    dap_chain_t *chain_t;
} PyDapChainObject;

int init_chain_py();
void deinit_chain_py();

void PyDapChainObject_dealloc(PyDapChainObject* chain);
PyObject *PyDapChainObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds);


PyObject *dap_chain_find_by_id_py(PyObject *self, PyObject *args);
//PyObject *dap_chain_load_from_cfg_py(PyObject *self, PyObject *args);
PyObject *dap_chain_has_file_store_py(PyObject *self, PyObject *args);
PyObject *dap_chain_save_all_py(PyObject *self, PyObject *args);
PyObject *dap_chain_load_all_py(PyObject *self, PyObject *args);
PyObject *dap_chain_load_from_cfg_py(PyObject *self, PyObject *args);
//PyObject *dap_chain_init_net_cfg_name_py(PyObject *self, PyObject *args); //dap_chain_init_net_cfg_name
//PyObject *dap_chain_close_py(PyObject *self, PyObject *args);
//PyObject *dap_chain_info_dump_log_py(PyObject *self, PyObject *args); //dap_chain_info_dump_log
PyObject *dap_chain_python_create_atom_iter(PyObject *self, PyObject *args);
PyObject *dap_chain_python_atom_iter_get_first(PyObject *self, PyObject *args);
PyObject *dap_chain_python_atom_get_datums(PyObject *self, PyObject *args);
PyObject *dap_chain_python_atom_iter_get_next(PyObject *self, PyObject *args);
PyObject *dap_chain_python_atom_iter_get_dag(PyObject *self, PyObject *args);

PyObject *dap_chain_python_get_count_tx(PyObject *self, PyObject *args);
PyObject *dap_chain_python_get_txs(PyObject *self, PyObject *args);

static PyMethodDef DapChainMethods[] = {
    {"findById", (PyCFunction)dap_chain_find_by_id_py, METH_VARARGS|METH_STATIC, ""},
    {"loadFromCfg", (PyCFunction)dap_chain_load_from_cfg_py, METH_VARARGS|METH_STATIC, ""},
    {"hasFileStore", (PyCFunction)dap_chain_has_file_store_py, METH_NOARGS, ""},
    {"saveAll", (PyCFunction) dap_chain_save_all_py, METH_NOARGS, ""},
    {"loadAll", (PyCFunction)dap_chain_load_all_py, METH_NOARGS, ""},
    {"createAtomItem", (PyCFunction) dap_chain_python_create_atom_iter, METH_VARARGS, ""},
    {"atomIterGetFirst", (PyCFunction) dap_chain_python_atom_iter_get_first, METH_VARARGS, ""},
    {"atomGetDatums", (PyCFunction) dap_chain_python_atom_get_datums, METH_VARARGS, ""},
    {"atomIterGetNext", (PyCFunction)dap_chain_python_atom_iter_get_next, METH_VARARGS, ""},
    {"getDag", (PyCFunction)dap_chain_python_atom_iter_get_dag, METH_NOARGS, ""},
    {"countTx", (PyCFunction)dap_chain_python_get_count_tx, METH_NOARGS, ""},
    {"getTransactions", (PyCFunction)dap_chain_python_get_txs, METH_VARARGS, ""},
    //{"close", (PyCFunction)dap_chain_close_py, METH_NOARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject dapChainObject_dapChainType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain",                                            /* tp_name */
    sizeof(PyDapChainObject),                                     /* tp_basicsize */
    0,                                                            /* tp_itemsize */
    (destructor)PyDapChainObject_dealloc,                         /* tp_dealloc */
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
    "Chain objects",                                              /* tp_doc */
    0,		                                                      /* tp_traverse */
    0,		                                                      /* tp_clear */
    0,		                                                      /* tp_richcompare */
    0,		                                                      /* tp_weaklistoffset */
    0,		                                                      /* tp_iter */
    0,		                                                      /* tp_iternext */
    DapChainMethods,                                              /* tp_methods */
    0,                                                            /* tp_members */
    0,                                                            /* tp_getset */
    0,                                                            /* tp_base */
    0,                                                            /* tp_dict */
    0,                                                            /* tp_descr_get */
    0,                                                            /* tp_descr_set */
    0,                                                            /* tp_dictoffset */
    0,                                                            /* tp_init */
    0,                                                            /* tp_alloc */
    PyDapChainObject_new,                                            /* tp_new */

};


