#pragma once

#include "Python.h"
#include "dap_chain_net_srv_datum.h"
#include "libdap-chain-python.h"

typedef struct PyDapChainNetSrvDatum{
    PyObject_HEAD
}PyDapChainNetSrvDatumObject;

PyObject *wrapping_dap_chain_net_srv_datum_create(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_datum_read(PyObject *self, PyObject *args);

extern PyTypeObject DapChainNetSrvDatumObject_DapChainNetSrvDatumObjectType;
