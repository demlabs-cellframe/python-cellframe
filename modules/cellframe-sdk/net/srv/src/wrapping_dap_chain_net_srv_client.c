#include "wrapping_dap_chain_net_srv_client.h"

#define LOG_TAG "wrapping_dap_chain_net_srv_client"

#define _TYPE_CALLBACK_CONNECTED 0
#define _TYPE_CALLBACK_DISCONNECTED 1
#define _TYPE_CALLBACK_DELETE 2

void _wrapping_dap_chain_net_srv_client_callback_connected(dap_chain_net_srv_client_t* a_client, void *a_arg){
    if(PyCallable_Check(a_arg)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject *l_call = (PyObject*)a_arg;
        PyDapChainNetSrvClientObject *client = PyObject_New(PyDapChainNetSrvClientObject,
                                                            &DapChainNetSrvClientObject_DapChainNetSrvClientObjectType);
        PyObject_Dir((PyObject *) client);
        client->srv_client = a_client;
        PyObject *l_args = Py_BuildValue("Oi", client, _TYPE_CALLBACK_CONNECTED);
        Py_INCREF(l_args);
        Py_INCREF(l_call);
        PyEval_CallObject(l_call, l_args);
        Py_XDECREF(l_args);
        Py_XDECREF(l_call);
        PyGILState_Release(state);
    }else{
        log_it(L_ERROR, "Can't called handler Python in callback connected");
    }
}
void _wrapping_dap_chain_net_srv_client_callback_disconnected(dap_chain_net_srv_client_t* a_client, void *a_arg){
    if(PyCallable_Check(a_arg)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject *l_call = (PyObject*)a_arg;
        PyDapChainNetSrvClientObject *client = PyObject_New(PyDapChainNetSrvClientObject,
                                                            &DapChainNetSrvClientObject_DapChainNetSrvClientObjectType);
        PyObject_Dir((PyObject *) client);
        client->srv_client = a_client;
        PyObject *l_args = Py_BuildValue("Oi", client, _TYPE_CALLBACK_DISCONNECTED);
        Py_INCREF(l_args);
        Py_INCREF(l_call);
        PyEval_CallObject(l_call, l_args);
        Py_XDECREF(l_args);
        Py_XDECREF(l_call);
        PyGILState_Release(state);
    }else{
        log_it(L_ERROR, "Can't called handler Python in callback disconnected");
    }
}
void _wrapping_dap_chain_net_srv_client_callback_deleted(dap_chain_net_srv_client_t* a_client, void *a_arg){
    if(PyCallable_Check(a_arg)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject *l_call = (PyObject*)a_arg;
        PyDapChainNetSrvClientObject *client = PyObject_New(PyDapChainNetSrvClientObject,
                                                            &DapChainNetSrvClientObject_DapChainNetSrvClientObjectType);
        PyObject_Dir((PyObject *) client);
        client->srv_client = a_client;
        PyObject *l_args = Py_BuildValue("Oi", client, _TYPE_CALLBACK_DELETE);
        Py_INCREF(l_args);
        Py_INCREF(l_call);
        PyEval_CallObject(l_call, l_args);
        Py_XDECREF(l_args);
        Py_XDECREF(l_call);
        PyGILState_Release(state);
    }else{
        log_it(L_ERROR, "Can't called handler Python in callback delete");
    }
}
void _wrapping_dap_chain_net_srv_client_callback_pkt_it(void *a_ch_chain, uint8_t a_pkt_type, dap_stream_ch_pkt_t *a_pkt, void *a_arg){
    log_it(L_ERROR, "Test reading");
}

int PyDapChainNetSrvClient_init(PyDapChainNetSrvClientObject* self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "net",
            "addr",
            "port",
            "handler",
            NULL
    };
    PyObject *obj_net, *obj_hander;
    const char *addr;
    uint16_t port;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OsHO", kwlist, &obj_net, &addr, &port, &obj_hander)){
        return -1;
    }
    if (!PyCallable_Check(obj_hander)){
        return -1;
    }
    dap_chain_net_srv_client_callbacks_t callbacks = {0};
    callbacks.connected = _wrapping_dap_chain_net_srv_client_callback_connected;
    callbacks.disconnected = _wrapping_dap_chain_net_srv_client_callback_disconnected;
    callbacks.deleted = _wrapping_dap_chain_net_srv_client_callback_deleted;
    callbacks.pkt_in = _wrapping_dap_chain_net_srv_client_callback_pkt_it;
    self->srv_client = dap_chain_net_srv_client_create_n_connect(((PyDapChainNetObject*)obj_net)->chain_net, addr, port, &callbacks, obj_hander);
    return 0;
}
PyObject *wrapping_dap_chain_net_srv_client_write(PyObject *self, PyObject *args){
    uint8_t typePKT;
    PyObject *obj_in_data;
    return Py_None;
}

PyObject *wrapping_dap_chain_net_srv_client_send_data(PyObject *self, PyObject *args){
    uint8_t id;   // Channel id
    uint8_t enc_type; // Zero if not encrypted
    uint8_t type; // general, command, info, signal and etc
    uint64_t seq_id; // Sequence id or position id
//    uint64_t seq
    uint32_t size;
}

PyObject *wrapping_dap_chain_net_srv_client_test(PyObject *self, PyObject *args){
    (void)self;
    const int l_test_size = 64;
    size_t l_request_size = l_test_size + sizeof(dap_stream_ch_chain_net_srv_pkt_test_t);
    dap_stream_ch_chain_net_srv_pkt_test_t *l_request = DAP_NEW(dap_stream_ch_chain_net_srv_pkt_test_t);
    l_request->data_size = l_test_size;
    char *test_data = DAP_NEW_SIZE(uint8_t, l_test_size);
    memcpy(test_data, "TRNMLK01U1trCFLO71ZRTTTEFDANHTHHCFADKEROKMnfheklosyteAsmMHTZSPLE", l_test_size);
    memcpy(l_request->data, test_data, l_test_size);
    dap_hash_fast(l_request->data, l_test_size, &l_request->data_hash);
    dap_chain_net_srv_client_write(((PyDapChainNetSrvClientObject*)self)->srv_client,
                                   DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_CHECK_REQUEST,l_request, l_request_size);
//    &l_request->data = DAP_NEW_SIZE(uint8_t, l_test_size);

}