#pragma once

#include <Python.h>
#include "dap_chain_datum_decree.h"

typedef struct PyDapChainDatumDecree{
    PyObject_HEAD
    dap_chain_datum_decree_t* decree;
}PyDapChainDatumDecreeObject;

PyObject* wrapping_dap_chain_datum_decree_get_ts_created(PyObject *self, void* closure);
PyObject* wrapping_dap_chain_datum_decree_get_type(PyObject *self, void* closure);
PyObject* wrapping_dap_chain_datum_decree_get_type_str(PyObject *self, void* closure);
PyObject* wrapping_dap_chain_datum_decree_get_subtype(PyObject *self, void* closure);
PyObject* wrapping_dap_chain_datum_decree_get_subtype_str(PyObject *self, void* closure);
PyObject* wrapping_dap_chain_datum_decree_get_data(PyObject *self, void* closure);
PyObject* wrapping_dap_chain_datum_decree_get_signs(PyObject *self, void* closure);

PyObject *wrapping_dap_chain_datum_decree_add_sign(PyObject *self, PyObject *args);

extern PyTypeObject DapChainDatumDecreeObjectType;