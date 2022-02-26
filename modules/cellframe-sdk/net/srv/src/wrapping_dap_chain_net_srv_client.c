#include "wrapping_dap_chain_net_srv_client.h"

#define LOG_TAG "wrapping_dap_chain_net_srv_client"

static void _wrapping_dap_chain_net_srv_client_callback_connected(dap_chain_net_srv_client_t* a_client, void *a_arg){
    UNUSED(a_client);
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)a_client->_inheritor;
    PyObject *l_call = py_client->callback_connected;
    if(PyCallable_Check(l_call)) {
        PyObject *l_args = Py_BuildValue("OO", py_client, a_arg);
        PyGILState_STATE state = PyGILState_Ensure();
        Py_INCREF(l_args);
        Py_INCREF(l_call);
        PyEval_CallObject(l_call, l_args);
        Py_XDECREF(l_args);
        Py_XDECREF(l_call);
        PyGILState_Release(state);
    }else{
        log_it(L_ERROR, "Can't call handler Python in callback connected");
    }
}

static void _wrapping_dap_chain_net_srv_client_callback_disconnected(dap_chain_net_srv_client_t* a_client, void *a_arg){
    UNUSED(a_client);
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)a_client->_inheritor;
    PyObject *l_call = py_client->callback_disconnected;
    if(PyCallable_Check(l_call)) {
        PyObject *l_args = Py_BuildValue("OO", py_client, a_arg);
        PyGILState_STATE state = PyGILState_Ensure();
        Py_INCREF(l_args);
        Py_INCREF(l_call);
        PyEval_CallObject(l_call, l_args);
        Py_XDECREF(l_args);
        Py_XDECREF(l_call);
        PyGILState_Release(state);
    }else{
        log_it(L_ERROR, "Can't call handler Python in callback disconnected");
    }
}

static void _wrapping_dap_chain_net_srv_client_callback_deleted(dap_chain_net_srv_client_t* a_client, void *a_arg){
    UNUSED(a_client);
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)a_client->_inheritor;
    PyObject *l_call = py_client->callback_deleted;
    if(PyCallable_Check(l_call)) {
        PyObject *l_args = Py_BuildValue("OO", py_client, a_arg);
        PyGILState_STATE state = PyGILState_Ensure();
        Py_INCREF(l_args);
        Py_INCREF(l_call);
        PyEval_CallObject(l_call, l_args);
        Py_XDECREF(l_args);
        Py_XDECREF(l_call);
        PyGILState_Release(state);
    }else{
        log_it(L_ERROR, "Can't call handler Python in callback delete");
    }
}

static void _wrapping_dap_chain_net_srv_client_callback_pkt_in(void *a_chain_net_srv,
                                                        uint8_t a_type,
                                                        dap_stream_ch_pkt_t *a_pkt,
                                                        void *a_arg){
    UNUSED(a_chain_net_srv);
    dap_chain_net_srv_client_t *l_srv_client = (dap_chain_net_srv_client_t *)a_arg;
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)l_srv_client->_inheritor;

    switch (a_type) {
    case DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_CHECK_RESPONSE:
    case DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_SIGN_REQUEST:
    case DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_RESPONSE_SUCCESS:
    case DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_RESPONSE_ERROR:
        break;
    case DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_RESPONSE_DATA: {
        dap_stream_ch_chain_net_srv_pkt_data_t *l_response = (dap_stream_ch_chain_net_srv_pkt_data_t *)a_pkt->data;
        PyObject *l_call = py_client->callback_read;
        if(PyCallable_Check(l_call)) {
            PyObject *l_args = Py_BuildValue("Os", py_client, l_response->data);
            PyGILState_STATE state = PyGILState_Ensure();
            Py_INCREF(l_args);
            Py_INCREF(l_call);
            PyEval_CallObject(l_call, l_args);
            Py_XDECREF(l_args);
            Py_XDECREF(l_call);
            PyGILState_Release(state);
        }else{
            log_it(L_ERROR, "Can't call handler Python in callback read");
        }
    } break;
    default:
        log_it(L_ERROR, "Can't process packet type %u", a_type);
        return;
    }
    log_it(L_NOTICE, "Callback srv client pkt_in");
}

int PyDapChainNetSrvClient_init(PyDapChainNetSrvClientObject* self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "net",
            "addr",
            "port",
            "callback_connected",
            "callback_read",
            "callback_disconnected",
            "callback_deleted",
            "callback_arg",
            NULL
    };
    PyObject *py_net, *py_cb_conn, *py_cb_disc, *py_cb_del, *py_cb_read, *py_arg;
    const char *addr;
    uint16_t port;
    if (!PyArg_ParseTupleAndKeywords(
                args, kwds, "OsHOOOOO", (char **)kwlist,
                &py_net, &addr, &port, &py_cb_conn,
                &py_cb_read, &py_cb_disc, &py_cb_del, &py_arg)){
        return -1;
    }
    if (!PyCallable_Check(py_cb_conn) || !PyCallable_Check(py_cb_disc) ||
            !PyCallable_Check(py_cb_del) || !PyCallable_Check(py_cb_read)){
        return -1;
    }
    dap_chain_net_srv_client_callbacks_t callbacks = {0};
    callbacks.connected = _wrapping_dap_chain_net_srv_client_callback_connected;
    callbacks.disconnected = _wrapping_dap_chain_net_srv_client_callback_disconnected;
    callbacks.deleted = _wrapping_dap_chain_net_srv_client_callback_deleted;
    callbacks.pkt_in = _wrapping_dap_chain_net_srv_client_callback_pkt_in;
    dap_chain_net_srv_client_t *l_client =
            dap_chain_net_srv_client_create_n_connect(((PyDapChainNetObject*)py_net)->chain_net,
                                                      (char *)addr, port, &callbacks, py_arg);
    self->srv_client = l_client;
    self->callback_connected = py_cb_conn;
    self->callback_disconnected = py_cb_disc;
    self->callback_deleted = py_cb_del;
    self->callback_read = py_cb_read;
    Py_INCREF(self);
    l_client->_inheritor = self;
    return 0;
}

PyObject *wrapping_dap_chain_net_srv_client_request(PyObject *self, PyObject *args){
    PyDapChainNetIdObject *obj_net_id;
    PyDapChainNetSrvUIDObject *obj_srv_uid;
    PyDapHashFastObject *obj_tx_cond_hash;
    const char *l_ticker;
    if (!PyArg_ParseTuple(args, "OOOs", &obj_net_id, &obj_srv_uid, &obj_tx_cond_hash, &l_ticker))
        return Py_None;
    if (!PyDapChainNetSrvUid_Check(obj_srv_uid))
        return Py_None;
    if (!PyObject_TypeCheck(obj_net_id, &DapChainNetIdObject_DapChainNetIdObjectType))
       return Py_None;
    if (!PyObject_TypeCheck(obj_tx_cond_hash, &DapHashTypeObject_DapChainHashTypeObjectType)){
        return Py_None;
    }
    //Generate packet
    dap_stream_ch_chain_net_srv_pkt_request_hdr_t l_hdr = {};
    l_hdr.net_id = obj_net_id->net_id;
    l_hdr.srv_uid = obj_srv_uid->net_srv_uid;
    memcpy(&l_hdr.tx_cond, obj_tx_cond_hash->hash_fast, sizeof(dap_chain_hash_fast_t));
    strncpy(l_hdr.token, l_ticker, DAP_CHAIN_TICKER_SIZE_MAX);
    l_hdr.token[DAP_CHAIN_TICKER_SIZE_MAX - 1] = '\0';
    ssize_t l_res = dap_chain_net_srv_client_write(
                        ((PyDapChainNetSrvClientObject*)self)->srv_client,
                        DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_REQUEST,
                        &l_hdr, sizeof(l_hdr));
    return Py_BuildValue("L", l_res);
}

PyObject *wrapping_dap_chain_net_srv_client_write(PyObject *self, PyObject *args){
    uint32_t usage_id;
    PyDapChainNetSrvUIDObject *obj_srv_uid;
    PyObject *obj_bytes;
    if (!PyArg_ParseTuple(args, "IOO", &usage_id, &obj_srv_uid, &obj_bytes)){
        return Py_None;
    }
    if (!PyDapChainNetSrvUid_Check(obj_srv_uid)){
        return Py_None;
    }
    if (!PyBytes_Check(obj_bytes)){
        return Py_None;
    }
    //Generate packet
    size_t l_bytes_size = PyBytes_Size(obj_bytes);
    void *l_bytes = PyBytes_AsString(obj_bytes);
    dap_stream_ch_chain_net_srv_pkt_data_t *l_data = DAP_NEW_S_SIZE(void,
                                                        sizeof(dap_stream_ch_chain_net_srv_pkt_data_t) + l_bytes_size);
    l_data->hdr.version = 1;
    l_data->hdr.data_size = (uint16_t)l_bytes_size;
    l_data->hdr.usage_id = usage_id;
    l_data->hdr.srv_uid = obj_srv_uid->net_srv_uid;
    memcpy(l_data->data, l_bytes, l_bytes_size);
    ssize_t l_res = dap_chain_net_srv_client_write(
                        ((PyDapChainNetSrvClientObject*)self)->srv_client,
                        DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_DATA,
                        l_data, sizeof(*l_data) + l_bytes_size);
    return Py_BuildValue("L", l_res);
}
