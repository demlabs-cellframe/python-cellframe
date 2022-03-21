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
PyObject *dap_chain_python_add_mempool_notify_callback(PyObject *self, PyObject *args);

extern PyTypeObject DapChainObjectType;

static inline bool PyDapChain_Check(PyObject* self){
    return PyObject_TypeCheck(self, &DapChainObjectType);
}


