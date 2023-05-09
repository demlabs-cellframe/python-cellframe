#pragma once

#include "Python.h"
#include "dap_chain_global_db.h"
#include "wrapping_dap_global_db_obj.h"
#include "dap_strfuncs.h"

typedef struct PyDapChainGlobalDB{
    PyObject_HEAD
}PyDapChainGlobalDBObject;

PyObject *wrapping_dap_chain_global_db_gr_get(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_global_db_gr_set(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_global_db_gr_del(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_global_db_gr_pin(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_global_db_gr_load(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_global_db_gr_delete_group(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_global_db_add_sync_extra_group(PyObject *self, PyObject *args);

extern PyTypeObject DapChainGlobalDBObjectType;
