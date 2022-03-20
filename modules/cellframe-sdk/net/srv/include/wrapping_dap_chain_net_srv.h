#pragma once

#include "Python.h"
#include "dap_chain_net_srv.h"
#include "dap_chain_net_srv_stream_session.h"
#include "uthash.h"
#include "wrapping_dap_chain_net_srv_client_remote.h"
#include "wrapping_dap_chain_common.h"
#include "wrapping_dap_chain_net_srv_common.h"
#include "wrapping_dap_chain_tx_receipt.h"

typedef struct PyDapChainNetSrv{
    PyObject_HEAD
    dap_chain_net_srv_t *srv;
    PyObject *callbackRequested;
    PyObject *callbackSuccess;
    PyObject *callbackError;
    PyObject *callbackReceiptNext;
    PyObject *callbackReadWithOutData;
}PyDapChainNetSrvObject;

int PyDapChainNetSrv_init(PyDapChainNetSrvObject* self, PyObject *args, PyObject *kwds);
void PyDapChainNetSrv_dealloc(PyDapChainNetSrvObject* self);

PyObject *wrapping_dap_chain_net_srv_get_uid(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_get_abstract(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_get_price_list(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_get_ban_list(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_get_grace_period(PyObject *self, void *closure);

//Function
PyObject *wrapping_dap_chain_net_srv_set_callback_channel(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_issue_receipt(PyObject *self, PyObject *args);

static PyMethodDef DapChainNetSrvMethods[]={
        {"issueReceipt", (PyCFunction)wrapping_dap_chain_net_srv_issue_receipt, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

static PyGetSetDef DapChaiNetSrvGetsSets[] = {
        {"uid", (getter)wrapping_dap_chain_net_srv_get_uid, NULL, NULL, NULL},
        {"gracePeriod", (getter)wrapping_dap_chain_net_srv_get_grace_period, NULL, NULL, NULL},
        {NULL}
};

static PyTypeObject DapChainNetSrvObject_DapChainNetSrvObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetSrv",        /* tp_name */
        sizeof(PyDapChainNetSrvObject), /* tp_basicsize */
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
        Py_TPFLAGS_BASETYPE,          /* tp_flags */
        "Chain net service object",               /* tp_doc */
        0,		                          /* tp_traverse */
        0,		                          /* tp_clear */
        0,		                          /* tp_richcompare */
        0,                                /* tp_weaklistoffset */
        0,		                          /* tp_iter */
        0,		                          /* tp_iternext */
        DapChainNetSrvMethods,        /* tp_methods */
        0,                                /* tp_members */
        DapChaiNetSrvGetsSets,        /* tp_getset */
        0,                                /* tp_base */
        0,                                /* tp_dict */
        0,                                /* tp_descr_get */
        0,                                /* tp_descr_set */
        0,                                /* tp_dictoffset */
        (initproc)PyDapChainNetSrv_init,      /* tp_init */
        0,                                /* tp_alloc */
        PyType_GenericNew,                /* tp_new */
};
