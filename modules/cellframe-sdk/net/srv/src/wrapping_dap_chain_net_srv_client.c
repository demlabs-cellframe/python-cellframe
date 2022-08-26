#include "wrapping_dap_chain_net_srv_client.h"
#include "dap_time.h"

#define LOG_TAG "wrapping_dap_chain_net_srv_client"

PyMethodDef DapChainNetSrvClientMethods[]={
        {"check", (PyCFunction)wrapping_dap_chain_net_srv_client_check, METH_VARARGS, ""},
        {"request", (PyCFunction)wrapping_dap_chain_net_srv_client_request, METH_VARARGS, ""},
        {"write", (PyCFunction)wrapping_dap_chain_net_srv_client_write, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainNetSrvClientObjectType = {
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

static void _wrapping_dap_chain_net_srv_client_callback_connected(dap_chain_net_srv_client_t* a_client, void *a_arg){
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)a_client->_inheritor;
    PyObject *l_call = py_client->callback_connected;
    if (PyCallable_Check(l_call)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject *l_args = Py_BuildValue("OO", py_client, a_arg);
        PyEval_CallObject(l_call, l_args);
        Py_DECREF(l_args);
        PyGILState_Release(state);
    } else {
        log_it(L_ERROR, "Can't call a python handler on connected event");
    }
}

static void _wrapping_dap_chain_net_srv_client_callback_disconnected(dap_chain_net_srv_client_t* a_client, void *a_arg){
    UNUSED(a_client);
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)a_client->_inheritor;
    PyObject *l_call = py_client->callback_disconnected;
    if (PyCallable_Check(l_call)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject *l_args = Py_BuildValue("OO", py_client, a_arg);
        PyEval_CallObject(l_call, l_args);
        Py_DECREF(l_args);
        PyGILState_Release(state);
    } else {
        log_it(L_ERROR, "Can't call a python handler on disconnected event");
    }
}

static void _wrapping_dap_chain_net_srv_client_callback_deleted(dap_chain_net_srv_client_t* a_client, void *a_arg){
    UNUSED(a_client);
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)a_client->_inheritor;
    PyObject *l_call = py_client->callback_deleted;
    if (PyCallable_Check(l_call)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject *l_args = Py_BuildValue("OO", py_client, a_arg);
        PyEval_CallObject(l_call, l_args);
        Py_DECREF(l_args);
        PyGILState_Release(state);
    } else {
        log_it(L_ERROR, "Can't call a python handler on delete event");
    }
}


static void _wrapping_dap_chain_net_srv_client_callback_check(dap_chain_net_srv_client_t *a_srv_client,
                                                              dap_stream_ch_chain_net_srv_pkt_test_t *a_pkt,
                                                              void *a_arg) {
    UNUSED(a_pkt);
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)a_srv_client->_inheritor;
    PyObject *l_call = py_client->callback_check;
    if (PyCallable_Check(l_call)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject *l_args = Py_BuildValue("OO", py_client, (PyObject *)a_arg);
        PyEval_CallObject(l_call, l_args);
        Py_DECREF(l_args);
        PyGILState_Release(state);
    } else {
        log_it(L_ERROR, "Can't call a python handler on check response event");
    }
}

static dap_chain_datum_tx_receipt_t * _wrapping_dap_chain_net_srv_client_callback_sign(
                                                             dap_chain_net_srv_client_t *a_srv_client,
                                                             dap_chain_datum_tx_receipt_t *a_receipt,
                                                             void *a_arg) {
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)a_srv_client->_inheritor;
    PyObject *l_call = py_client->callback_sign;
    PyDapChainTXReceiptObject *py_ret = NULL;
    if (PyCallable_Check(l_call)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyDapChainTXReceiptObject *py_receipt = PyObject_New(PyDapChainTXReceiptObject,
                                                             &DapChainTxReceiptObjectType);
        py_receipt->tx_receipt = a_receipt;
        PyObject *l_args = Py_BuildValue("OOO", py_client, py_receipt, (PyObject *)a_arg);
        py_ret = (PyDapChainTXReceiptObject *)PyEval_CallObject(l_call, l_args);
        Py_DECREF(l_args);
        PyGILState_Release(state);
    } else {
        log_it(L_ERROR, "Can't call handler Python in sign callback");
    }
    if (!py_ret || (PyObject *)py_ret == Py_None || !py_ret->tx_receipt)
        return NULL;
    return DAP_DUP_SIZE(py_ret->tx_receipt, py_ret->tx_receipt->size);
}

static void _wrapping_dap_chain_net_srv_client_callback_success(dap_chain_net_srv_client_t *a_srv_client,
                                                                dap_stream_ch_chain_net_srv_pkt_success_t *a_pkt,
                                                                size_t a_pkt_size,
                                                                void *a_arg) {
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)a_srv_client->_inheritor;
    PyObject *l_call = py_client->callback_success;
    if (PyCallable_Check(l_call)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyDapHashFastObject *py_cond_hash = PyObject_New(PyDapHashFastObject,
                                                         &DapHashTypeObject_DapChainHashTypeObjectType);
        if (a_pkt_size == sizeof(dap_stream_ch_chain_net_srv_pkt_success_t) + sizeof(dap_chain_hash_fast_t)) {
            py_cond_hash->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
            memcpy(py_cond_hash->hash_fast, a_pkt->custom_data, sizeof(dap_chain_hash_fast_t));
        } else
            py_cond_hash->hash_fast = NULL;
        PyObject *l_args = Py_BuildValue("OOO", py_client, py_cond_hash, (PyObject *)a_arg);
        PyEval_CallObject(l_call, l_args);
        Py_DECREF(l_args);
        PyGILState_Release(state);
    } else {
        log_it(L_ERROR, "Can't call handler Python in success callback");
    }
}

static void _wrapping_dap_chain_net_srv_client_callback_error(dap_chain_net_srv_client_t *a_srv_client,
                                                              int a_error_code,
                                                              void *a_arg) {
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)a_srv_client->_inheritor;
    PyObject *l_call = py_client->callback_error;
    if (PyCallable_Check(l_call)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject *l_args = Py_BuildValue("OiO", py_client, a_error_code, (PyObject *)a_arg);
        PyEval_CallObject(l_call, l_args);
        Py_DECREF(l_args);
        PyGILState_Release(state);
    } else {
        log_it(L_ERROR, "Can't call handler Python in error callback");
    }
}

static void _wrapping_dap_chain_net_srv_client_callback_data(dap_chain_net_srv_client_t *a_srv_client,
                                                             uint8_t *a_data,
                                                             size_t a_data_size,
                                                             void *a_arg) {
    PyDapChainNetSrvClientObject *py_client = (PyDapChainNetSrvClientObject *)a_srv_client->_inheritor;
    PyObject *l_call = py_client->callback_data;
    if (PyCallable_Check(l_call)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject *l_data = PyBytes_FromStringAndSize((char *)a_data, a_data_size);
        PyObject *l_args = Py_BuildValue("OOO", py_client, l_data, (PyObject *)a_arg);
        PyEval_CallObject(l_call, l_args);
        Py_DECREF(l_args);
        PyGILState_Release(state);
    } else {
        log_it(L_ERROR, "Can't call handler Python in data callback");
    }
}

int PyDapChainNetSrvClient_init(PyDapChainNetSrvClientObject* self, PyObject *args, PyObject *kwds) {
    const char *kwlist[] = {
            "net",
            "addr",
            "port",
            "callback_connected",
            "callback_disconnected",
            "callback_deleted",
            "callback_check",
            "callback_sign",
            "callback_success",
            "callback_error",
            "callback_data",
            "callback_arg",
            NULL
    };
    PyDapChainNetObject *py_net;
    PyObject *py_cb_conn, *py_cb_disc, *py_cb_del, *py_cb_check, *py_cb_sign;
    PyObject *py_cb_success, *py_cb_error, *py_cb_data, *py_cb_arg;
    const char *addr;
    uint16_t port;
    if (!PyArg_ParseTupleAndKeywords(
                args, kwds, "OsHOOOOOOOOO", (char **)kwlist,
                &py_net, &addr, &port, &py_cb_conn,
                &py_cb_disc, &py_cb_del, &py_cb_check,
                &py_cb_sign, &py_cb_success, &py_cb_error,
                &py_cb_data, &py_cb_arg
                )){
        return -1;
    }
    if (!PyDapChainNet_Check(py_net))
       return -2;
    if (!PyCallable_Check(py_cb_conn) ||
            !PyCallable_Check(py_cb_disc) ||
            !PyCallable_Check(py_cb_del) ||
            !PyCallable_Check(py_cb_check) ||
            !PyCallable_Check(py_cb_sign) ||
            !PyCallable_Check(py_cb_success) ||
            !PyCallable_Check(py_cb_error) ||
            !PyCallable_Check(py_cb_data)) {
        return -3;
    }
    dap_chain_net_srv_client_callbacks_t callbacks = {0};
    callbacks.connected = _wrapping_dap_chain_net_srv_client_callback_connected;
    callbacks.disconnected = _wrapping_dap_chain_net_srv_client_callback_disconnected;
    callbacks.deleted = _wrapping_dap_chain_net_srv_client_callback_deleted;
    callbacks.check = _wrapping_dap_chain_net_srv_client_callback_check;
    callbacks.sign = _wrapping_dap_chain_net_srv_client_callback_sign;
    callbacks.success = _wrapping_dap_chain_net_srv_client_callback_success;
    callbacks.error = _wrapping_dap_chain_net_srv_client_callback_error;
    callbacks.data = _wrapping_dap_chain_net_srv_client_callback_data;
    dap_chain_net_srv_client_t *l_client =
            dap_chain_net_srv_client_create_n_connect(py_net->chain_net,
                                                      (char *)addr, port, &callbacks, py_cb_arg);
    self->srv_client = l_client;
    self->callback_connected = py_cb_conn;
    self->callback_disconnected = py_cb_disc;
    self->callback_deleted = py_cb_del;
    self->callback_check = py_cb_check;
    self->callback_sign = py_cb_sign;
    self->callback_success = py_cb_success;
    self->callback_error = py_cb_error;
    self->callback_data = py_cb_data;
    Py_INCREF(self);
    l_client->_inheritor = self;
    return 0;
}


PyObject *wrapping_dap_chain_net_srv_client_check(PyObject *self, PyObject *args) {
    PyDapChainNetIdObject *obj_net_id;
    PyDapChainNetSrvUIDObject *obj_srv_uid;
    PyObject *obj_bytes;
    if (!PyArg_ParseTuple(args, "OOO", &obj_net_id, &obj_srv_uid, &obj_bytes)) {
        Py_RETURN_NONE;
    }
    if (!PyDapChainNetSrvUid_Check(obj_srv_uid))
        Py_RETURN_NONE;
    if (PyObject_TypeCheck(obj_net_id, &DapChainNetIdObjectType))
        Py_RETURN_NONE;
    if (!PyBytes_Check(obj_bytes)) {
        Py_RETURN_NONE;
    }
    //Generate packet
    size_t l_bytes_size = PyBytes_Size(obj_bytes);
    void *l_bytes = PyBytes_AsString(obj_bytes);
    size_t l_request_size = sizeof(dap_stream_ch_chain_net_srv_pkt_test_t) + l_bytes_size;
    dap_stream_ch_chain_net_srv_pkt_test_t *l_request = DAP_NEW_S_SIZE(dap_stream_ch_chain_net_srv_pkt_test_t,
                                                                       l_request_size);
    memset(l_request, 0, sizeof(dap_stream_ch_chain_net_srv_pkt_test_t));
    l_request->net_id.uint64 = obj_net_id->net_id.uint64;
    l_request->srv_uid.uint64 = obj_srv_uid->net_srv_uid.uint64;
    l_request->data_size_send = l_request->data_size_recv = l_bytes_size;
    l_request->data_size = l_bytes_size;
    l_request->send_time1 = dap_gdb_time_now();
    memcpy(l_request->data, l_bytes, l_bytes_size);
    dap_hash_fast(l_request->data, l_request->data_size, &l_request->data_hash);
    ssize_t l_res = dap_chain_net_srv_client_write(
                        ((PyDapChainNetSrvClientObject*)self)->srv_client,
                        DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_CHECK_REQUEST,
                        l_request, l_request_size);
    return Py_BuildValue("L", l_res);
}

PyObject *wrapping_dap_chain_net_srv_client_request(PyObject *self, PyObject *args) {
    PyDapChainNetObject *obj_net;
    PyDapChainNetSrvUIDObject *obj_srv_uid;
    PyDapHashFastObject *obj_tx_cond_hash;
    if (!PyArg_ParseTuple(args, "OOO", &obj_net, &obj_srv_uid, &obj_tx_cond_hash))
        Py_RETURN_NONE;
    if (!PyDapChainNetSrvUid_Check(obj_srv_uid))
        Py_RETURN_NONE;
    if (!PyDapChainNet_Check(obj_net))
        Py_RETURN_NONE;
    if (!PyDapHashFast_Check(obj_tx_cond_hash)) {
        Py_RETURN_NONE;
    }
    //Generate packet
    dap_stream_ch_chain_net_srv_pkt_request_hdr_t l_hdr = {};
    l_hdr.net_id = obj_net->chain_net->pub.id;
    l_hdr.srv_uid = obj_srv_uid->net_srv_uid;
    memcpy(&l_hdr.tx_cond, obj_tx_cond_hash->hash_fast, sizeof(dap_chain_hash_fast_t));
    ssize_t l_res = dap_chain_net_srv_client_write(
                        ((PyDapChainNetSrvClientObject*)self)->srv_client,
                        DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_REQUEST,
                        &l_hdr, sizeof(l_hdr));
    return Py_BuildValue("L", l_res);
}

PyObject *wrapping_dap_chain_net_srv_client_write(PyObject *self, PyObject *args) {
    PyDapChainNetSrvUIDObject *obj_srv_uid;
    PyObject *obj_bytes;
    if (!PyArg_ParseTuple(args, "OO", &obj_srv_uid, &obj_bytes)) {
        Py_RETURN_NONE;
    }
    if (!PyDapChainNetSrvUid_Check(obj_srv_uid)) {
        Py_RETURN_NONE;
    }
    if (!PyBytes_Check(obj_bytes)) {
        Py_RETURN_NONE;
    }
    //Generate packet
    size_t l_bytes_size = PyBytes_Size(obj_bytes);
    void *l_bytes = PyBytes_AsString(obj_bytes);
    dap_stream_ch_chain_net_srv_pkt_data_t *l_data = DAP_NEW_S_SIZE(void,
                                                        sizeof(dap_stream_ch_chain_net_srv_pkt_data_t) + l_bytes_size);
    l_data->hdr.version = 1;
    l_data->hdr.data_size = (uint16_t)l_bytes_size;
    l_data->hdr.usage_id = 0;
    l_data->hdr.srv_uid = obj_srv_uid->net_srv_uid;
    memcpy(l_data->data, l_bytes, l_bytes_size);
    ssize_t l_res = dap_chain_net_srv_client_write(
                        ((PyDapChainNetSrvClientObject*)self)->srv_client,
                        DAP_STREAM_CH_CHAIN_NET_SRV_PKT_TYPE_DATA,
                        l_data, sizeof(*l_data) + l_bytes_size);
    return Py_BuildValue("L", l_res);
}
