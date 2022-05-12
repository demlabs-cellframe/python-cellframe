//#include "wrapping_dap_chain_net_"
#include "wrapping_dap_chain_net_srv_client_remote.h"

#define WRAPPING_DAP_CHAIN_NET_SRV_CLIENT(a) ((dap_chain_net_srv_client_remote_t*)((PyDapPyDapChainNetSrvClientObject*)a)->srv_client)
#define _PyDapChainNetSrvClient(a) ((PyDapChainNetSrvClient*)a)

PyMethodDef DapChainNetSrvClientRemoteMethods[]={
        {NULL, NULL, 0, NULL}
};

PyGetSetDef DapChaiNetSrvClientRemoteGetsSets[] = {
        {"ch", (getter)wrapping_dap_chain_net_srv_client_remote_get_ch, NULL, NULL, NULL},
        {"tsCreated", (getter)wrapping_dap_chain_net_srv_client_remote_get_ts_created, NULL, NULL, NULL},
        {"created", (getter)wrapping_dap_chain_net_srv_client_remote_get_created, NULL, NULL, NULL},
        {"streamWorker", (getter)wrapping_dap_chain_net_srv_client_remote_get_stream_worker, NULL, NULL, NULL},
        {"sessionId", (getter)wrapping_dap_chain_net_srv_client_remote_get_session_id, NULL, NULL, NULL},
        {"bytesReceived", (getter)wrapping_dap_chain_net_srv_client_remote_get_bytes_received, NULL, NULL, NULL},
        {"bytesSend", (getter)wrapping_dap_chain_net_srv_client_remote_get_bytes_send, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainNetSrvClientRemoteObject_DapChainNetSrvClientRemoteObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetSrvClientRemote",        /* tp_name */
        sizeof(PyDapChainNetSrvClientRemoteObject), /* tp_basicsize */
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
        "Chain net service client remote object",               /* tp_doc */
        0,		                          /* tp_traverse */
        0,		                          /* tp_clear */
        0,		                          /* tp_richcompare */
        0,                                /* tp_weaklistoffset */
        0,		                          /* tp_iter */
        0,		                          /* tp_iternext */
        DapChainNetSrvClientRemoteMethods,        /* tp_methods */
        0,                                /* tp_members */
        DapChaiNetSrvClientRemoteGetsSets,        /* tp_getset */
        0,                                /* tp_base */
        0,                                /* tp_dict */
        0,                                /* tp_descr_get */
        0,                                /* tp_descr_set */
        0,                                /* tp_dictoffset */
        0,                                /* tp_init */
        0,                                /* tp_alloc */
        PyType_GenericNew,                /* tp_new */
};

PyObject *wrapping_dap_chain_net_srv_client_remote_get_ch(PyObject *self, void *closure){
    (void)closure;
    //TODO
    Py_RETURN_NONE;
}
PyObject *wrapping_dap_chain_net_srv_client_remote_get_ts_created(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainNetSrvClientRemoteObject*)self)->srv_client_remote->ts_created);
}
PyObject *wrapping_dap_chain_net_srv_client_remote_get_created(PyObject *self, void *closure){
    (void)closure;
    PyDateTime_IMPORT;
    PyObject *l_obj_long_ts = PyLong_FromDouble(((PyDapChainNetSrvClientRemoteObject*)self)->srv_client_remote->ts_created);
    PyObject *l_obj_tuple = Py_BuildValue("(O)", l_obj_long_ts);
    PyObject *l_obj_dateTime = PyDateTime_FromTimestamp(l_obj_tuple);
    return l_obj_dateTime;
}
PyObject *wrapping_dap_chain_net_srv_client_remote_get_stream_worker(PyObject *self, void *closure){
    (void)closure;
    //TODO
    Py_RETURN_NONE;
}
PyObject *wrapping_dap_chain_net_srv_client_remote_get_session_id(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("i", ((PyDapChainNetSrvClientRemoteObject*)self)->srv_client_remote->session_id);
}
PyObject *wrapping_dap_chain_net_srv_client_remote_get_bytes_received(PyObject *self, void *closure){
    (void)closure;
    dap_chain_net_srv_client_remote_t *l_client = ((PyDapChainNetSrvClientRemoteObject*)self)->srv_client_remote;
    return Py_BuildValue("k", l_client->bytes_received);
}
PyObject *wrapping_dap_chain_net_srv_client_remote_get_bytes_send(PyObject *self, void *closure){
    (void)closure;
    dap_chain_net_srv_client_remote_t *l_client = ((PyDapChainNetSrvClientRemoteObject*)self)->srv_client_remote;
    return Py_BuildValue("k", l_client->bytes_sent);
}
//PyObject *wrapping_dap_chain_net_srv_client_remote_get_bytes_prev(PyObject *self, void *closure){
//    (void)closure;
//}
//PyObject *wrapping_dap_chain_net_srv_client_remote_get_bytes_next(PyObject *self, void *closure){
//    (void)closure;
//    PyDapChain
//}

