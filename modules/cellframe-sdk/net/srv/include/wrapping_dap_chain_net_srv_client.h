//
// Created by blus on 05.02.2022.
//

#include "Python.h"
#include "datetime.h"
#include "dap_chain_net_srv_client.h"

#ifndef WRAPPING_DAP_CHAIN_NET_SRV_CLIENT_H
#define WRAPPING_DAP_CHAIN_NET_SRV_CLIENT_H

typedef struct PyDapChainNetSrvClient{
    PyObject_HEAD
    dap_chain_net_srv_client_t *srv_client;
}PyDapChainNetSrvClientObject;

PyObject *wrapping_dap_chain_net_srv_client_get_ch(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_client_get_ts_created(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_client_get_created(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_client_get_stream_worker(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_client_get_session_id(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_client_get_bytes_received(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_client_get_bytes_send(PyObject *self, void *closure);
//PyObject *wrapping_dap_chain_net_srv_client_get_bytes_prev(PyObject *self, void *closure);
//PyObject *wrapping_dap_chain_net_srv_client_get_bytes_next(PyObject *self, void *closure);

static PyMethodDef DapChainNetSrvClientMethods[]={
        {NULL, NULL, 0, NULL}
};

static PyGetSetDef DapChaiNetSrvClientGetsSets[] = {
        {"ch", (getter)wrapping_dap_chain_net_srv_client_get_ch, NULL, NULL, NULL},
        {"tsCreated", (getter)wrapping_dap_chain_net_srv_client_get_ts_created, NULL, NULL, NULL},
        {"created", (getter)wrapping_dap_chain_net_srv_client_get_created, NULL, NULL, NULL},
        {"streamWorker", (getter)wrapping_dap_chain_net_srv_client_get_stream_worker, NULL, NULL, NULL},
        {"sessionId", (getter)wrapping_dap_chain_net_srv_client_get_session_id, NULL, NULL, NULL},
        {"received", (getter)wrapping_dap_chain_net_srv_client_get_bytes_received, NULL, NULL, NULL},
        {"send", (getter)wrapping_dap_chain_net_srv_client_get_bytes_send, NULL, NULL, NULL},
        {NULL}
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
        Py_TPFLAGS_BASETYPE,          /* tp_flags */
        "Chain net srv client object",               /* tp_doc */
        0,		                          /* tp_traverse */
        0,		                          /* tp_clear */
        0,		                          /* tp_richcompare */
        0,                                /* tp_weaklistoffset */
        0,		                          /* tp_iter */
        0,		                          /* tp_iternext */
        DapChainNetSrvClientMethods,        /* tp_methods */
        0,                                /* tp_members */
        DapChaiNetSrvClientGetsSets,        /* tp_getset */
        0,                                /* tp_base */
        0,                                /* tp_dict */
        0,                                /* tp_descr_get */
        0,                                /* tp_descr_set */
        0,                                /* tp_dictoffset */
        0,                                /* tp_init */
        0,                                /* tp_alloc */
        PyType_GenericNew,                /* tp_new */
};

#endif //WRAPPING_DAP_CHAIN_NET_SRV_CLIENT_H