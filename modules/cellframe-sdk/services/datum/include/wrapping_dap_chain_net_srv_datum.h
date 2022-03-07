#pragma once

#include "Python.h"
#include "dap_chain_net_srv_datum.h"
#include "libdap-chain-python.h"

typedef struct PyDapChainNetSrvDatum{
    PyObject_HEAD
}PyDapChainNetSrvDatumObject;

PyObject *wrapping_dap_chain_net_srv_datum_create(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_datum_read(PyObject *self, PyObject *args);

static PyMethodDef DapChainNetSrvDatum_method[] = {
        {"create", (PyCFunction) wrapping_dap_chain_net_srv_datum_create, METH_VARARGS | METH_STATIC, ""},
        {"read", (PyCFunction) wrapping_dap_chain_net_srv_datum_read, METH_VARARGS|METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainNetSrvDatumObject_DapChainNetSrvDatumObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetSrvDatum",            /* tp_name */
        sizeof(PyDapChainNetSrvDatumObject),     /* tp_basicsize */
        0,                               /* tp_itemsize */
        0,                               /* tp_dealloc */
        0,                               /* tp_print */
        0,                               /* tp_getattr */
        0,                               /* tp_setattr */
        0,                               /* tp_reserved */
        0,                               /* tp_repr */
        0,                               /* tp_as_number */
        0,                               /* tp_as_sequence */
        0,                               /* tp_as_mapping */
        0,                               /* tp_hash  */
        0,                               /* tp_call */
        0,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Chain net srv datum object",              /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapChainNetSrvDatum_method,       /* tp_methods */
        0,                               /* tp_members */
        0,                               /* tp_getset */
        0,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        0,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};
