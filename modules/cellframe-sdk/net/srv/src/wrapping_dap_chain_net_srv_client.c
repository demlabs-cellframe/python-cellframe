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
void _wrapping_dap_chain_net_srv_client_callback_pkt_it(void *a_arg,
                                                        uint8_t a_type,
                                                        dap_stream_ch_pkt_t *a_pkt,
                                                        void *a_pkt_size){
    log_it(L_NOTICE, "Callback srv client pkt_it");
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
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OsHO", (char **)kwlist, &obj_net, &addr, &port, &obj_hander)){
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
    dap_chain_net_srv_client_create_n_connect(((PyDapChainNetObject*)obj_net)->chain_net, (char *)addr, port, &callbacks, obj_hander);
    return 0;
}

PyObject *wrapping_dap_chain_net_srv_client_write(PyObject *self, PyObject *args){
    uint8_t version;
    uint32_t usage_id;
    PyObject *obj_srv_uid;
    PyObject *obj_bytes;
    if (!PyArg_ParseTuple(args, "bIOO", &version, &usage_id, &obj_srv_uid, &obj_bytes)){
        return NULL;
    }
    if (!PyDapChainNetSrvUid_Check(obj_srv_uid)){
        return NULL;
    }
    if (!PyBytes_Check(obj_bytes)){
        return NULL;
    }
    //Generate HDR
    dap_stream_ch_chain_net_srv_pkt_data_hdr_t l_pkt_hdr;
    l_pkt_hdr.version = version;
    l_pkt_hdr.usage_id = usage_id;
    l_pkt_hdr.offset[0] = 0;
    l_pkt_hdr.offset[1] = 0;
    l_pkt_hdr.offset[2] = 0;
    l_pkt_hdr.srv_uid = ((PyDapChainNetSrvUIDObject*)obj_srv_uid)->net_srv_uid;
    //Generate packet
    size_t l_bytes_size = PyBytes_Size(obj_bytes);
    void *l_bytes = PyBytes_AsString(obj_bytes);
    dap_stream_ch_chain_net_srv_pkt_data_t *l_data = DAP_NEW_S_SIZE(void, sizeof(l_pkt_hdr) + l_bytes_size);
    l_data->hdr = l_pkt_hdr;
    memcpy(l_data->data, l_bytes, l_bytes_size);
    dap_chain_net_srv_client_write(
            ((PyDapChainNetSrvClientObject*)self)->srv_client,
            DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_DATA,
            l_data, sizeof(l_pkt_hdr) + l_bytes_size);
    return Py_None;
}
