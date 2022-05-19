#pragma once

#include "Python.h"
#include "dap_chain_global_db.h"

typedef struct PyDapChainGlobalDBObj{
    PyObject_HEAD
    dap_global_db_obj_t obj;
}PyDapChainGlobalDBContainerObject;


//Attributes
PyObject *wrapping_dap_chain_global_db_obj_get_id(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_global_db_obj_get_key(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_global_db_obj_get_value(PyObject *self, void *closure);

extern PyTypeObject DapChainGlobalDBContainerObjectType;
