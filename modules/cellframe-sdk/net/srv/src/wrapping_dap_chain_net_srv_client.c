//#include "wrapping_dap_chain_net_"
#include "wrapping_dap_chain_net_srv_client.h"

#define WRAPPING_DAP_CHAIN_NET_SRV_CLIENT(a) ((dap_chain_net_srv_client_t*)((PyDapPyDapChainNetSrvClientObject*)a)->srv_client)
#define _PyDapChainNetSrvClient(a) ((PyDapChainNetSrvClient*)a)

PyObject *wrapping_dap_chain_net_srv_client_get_ch(PyObject *self, void *closure){
    (void)closure;
    //TODO
    return Py_None;
}
PyObject *wrapping_dap_chain_net_srv_client_get_ts_created(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainNetSrvClientObject*)self)->srv_client->ts_created);
}
PyObject *wrapping_dap_chain_net_srv_client_get_created(PyObject *self, void *closure){
    (void)closure;
    PyDateTime_IMPORT;
    PyObject *l_obj_long_ts = PyLong_FromDouble(((PyDapChainNetSrvClientObject*)self)->srv_client->ts_created);
    PyObject *l_obj_tuple = Py_BuildValue("(O)", l_obj_long_ts);
    PyObject *l_obj_dateTime = PyDateTime_FromTimestamp(l_obj_tuple);
    return l_obj_dateTime;
}
PyObject *wrapping_dap_chain_net_srv_client_get_stream_worker(PyObject *self, void *closure){
    (void)closure;
    //TODO
    return Py_None;
}
PyObject *wrapping_dap_chain_net_srv_client_get_session_id(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("i", ((PyDapChainNetSrvClientObject*)self)->srv_client->session_id);
}
PyObject *wrapping_dap_chain_net_srv_client_get_bytes_received(PyObject *self, void *closure){
    (void)closure;
    dap_chain_net_srv_client_t *l_client = ((PyDapChainNetSrvClientObject*)self)->srv_client;
//    PyBytes_FromString(l_client->)
}
PyObject *wrapping_dap_chain_net_srv_client_get_bytes_send(PyObject *self, void *closure){
    (void)closure;
}
//PyObject *wrapping_dap_chain_net_srv_client_get_bytes_prev(PyObject *self, void *closure){
//    (void)closure;
//}
//PyObject *wrapping_dap_chain_net_srv_client_get_bytes_next(PyObject *self, void *closure){
//    (void)closure;
//    PyDapChain
//}
