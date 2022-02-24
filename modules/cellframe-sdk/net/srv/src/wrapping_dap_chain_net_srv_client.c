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
    if (a_type != DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_DATA) {
        log_it(L_ERROR, "Can't process packet type %u", a_type);
        return;
    }
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
    log_it(L_NOTICE, "Callback srv client pkt_it");
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
            dap_chain_net_srv_client_create_n_connect(((PyDapChainNetObject*)py_net)->chain_net, (char *)addr, port, &callbacks, py_arg);
    Py_INCREF(self);
    self->srv_client = l_client;
    self->callback_connected = py_cb_conn;
    self->callback_disconnected = py_cb_disc;
    self->callback_deleted = py_cb_del;
    self->callback_read = py_cb_read;
    l_client->_inheritor = self;
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
    l_pkt_hdr.srv_uid = ((PyDapChainNetSrvUIDObject*)obj_srv_uid)->net_srv_uid;
    //Generate packet
    size_t l_bytes_size = PyBytes_Size(obj_bytes);
    l_pkt_hdr.data_size = (uint16_t)l_bytes_size;
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
