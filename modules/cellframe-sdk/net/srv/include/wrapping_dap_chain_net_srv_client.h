#pragma once

#include "Python.h"
#include "dap_chain_net_srv_client.h"
#include "libdap_chain_net_python.h"

typedef struct PyDapChainNetSrvClient{
    PyObject_HEAD
    dap_chain_net_srv_client_t *srv_client;
    PyObject *callback_connected;
    PyObject *callback_disconnected;
    PyObject *callback_deleted;
    PyObject *callback_check;
    PyObject *callback_sign;
    PyObject *callback_success;
    PyObject *callback_error;
    PyObject *callback_data;
}PyDapChainNetSrvClientObject;

int PyDapChainNetSrvClient_init(PyDapChainNetSrvClientObject* self, PyObject *args, PyObject *kwds);
PyObject *wrapping_dap_chain_net_srv_client_check(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_client_request(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_client_write(PyObject *self, PyObject *args);

static PyMethodDef DapChainNetSrvClientMethods[]={
        {"check", (PyCFunction)wrapping_dap_chain_net_srv_client_check, METH_VARARGS, ""},
        {"request", (PyCFunction)wrapping_dap_chain_net_srv_client_request, METH_VARARGS, ""},
        {"write", (PyCFunction)wrapping_dap_chain_net_srv_client_write, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainNetSrvClientObject_DapChainNetSrvClientObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetSrvClient",        /* tp_name */
        sizeof(PyDapChainNetSrvClientObject), /* tp_basicsize */
        0,                                /* tp_itemsize */
        0,                                /* tp_dealloc */
        0,                                /* tp_print */
        0,                                /* tp_getattr */
        0,                                /* tp_setattr */
        0,                                /* tp_reserved */
        0,                                /* tp_repr */
        0,                                /* tp_as_number */
        0,                                /* tp_as_sequence */
        0,                                /* tp_as_mapping */
        0,                                /* tp_hash  */
        0,                                /* tp_call */
        0,                                /* tp_str */
        0,                                /* tp_getattro */
        0,                                /* tp_setattro */
        0,                                /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,              /* tp_flags */
        "Chain net service client object",/* tp_doc */
        0,		                          /* tp_traverse */
        0,		                          /* tp_clear */
        0,		                          /* tp_richcompare */
        0,                                /* tp_weaklistoffset */
        0,		                          /* tp_iter */
        0,		                          /* tp_iternext */
        DapChainNetSrvClientMethods,      /* tp_methods */
        0,                                /* tp_members */
        0,                                /* tp_getset */
        0,                                /* tp_base */
        0,                                /* tp_dict */
        0,                                /* tp_descr_get */
        0,                                /* tp_descr_set */
        0,                                /* tp_dictoffset */
        (initproc)PyDapChainNetSrvClient_init,      /* tp_init */
        0,                                /* tp_alloc */
        PyType_GenericNew,                /* tp_new */
};
