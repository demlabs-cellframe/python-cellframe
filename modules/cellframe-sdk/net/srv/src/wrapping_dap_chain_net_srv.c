#include "wrapping_dap_chain_net_srv.h"

enum _wrapping_dap_chain_net_srv_type_callbacks{
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_REQUESTED,
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_SUCCESS,
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_ERROR,
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RECEIPT_NEXT_SUCCESS,
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ,
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_CLIENT_SUCCESS
};

typedef struct _wrapping_dap_chain_net_srv_callbacks_key{
    enum _wrapping_dap_chain_net_srv_type_callbacks type;
    dap_chain_net_srv_t  *srv;
}_wrapping_dap_chain_net_srv_callbacks_key_t;

typedef struct _wrapping_dap_chain_net_srv_callbacks{
    _wrapping_dap_chain_net_srv_callbacks_key_t *key;
    dap_chain_net_srv_uid_t uid;
    PyObject *func;
    UT_hash_handle hh;
}_wrapping_dap_chain_net_srv_callbacks_t;

static _wrapping_dap_chain_net_srv_callbacks_t *_s_callbacks = NULL;

void _wrapping_dap_chain_net_srv_del(_wrapping_dap_chain_net_srv_callbacks_t *a_callback){
    HASH_DEL(_s_callbacks, a_callback);
    Py_XINCREF(a_callback->func);
}

PyObject* _wrapping_dap_chain_net_srv_search(_wrapping_dap_chain_net_srv_callbacks_key_t *a_key){
    _wrapping_dap_chain_net_srv_callbacks_t *callbacks = NULL;
    HASH_FIND(hh, _s_callbacks, a_key, sizeof(_wrapping_dap_chain_net_srv_callbacks_key_t), callbacks);
    if (callbacks == NULL){
        return Py_None;
    } else {
        return callbacks->func;
    }
}

int _wrapping_dap_chain_net_srv_add(
                enum _wrapping_dap_chain_net_srv_type_callbacks a_type,
                dap_chain_net_srv_t *a_srv,
                dap_chain_net_srv_uid_t a_uid,
                PyObject *a_obj_func){
    if (PyCallable_Check(a_obj_func)){
        _wrapping_dap_chain_net_srv_callbacks_t *callbacks = DAP_NEW(_wrapping_dap_chain_net_srv_callbacks_t);
        _wrapping_dap_chain_net_srv_callbacks_key_t *l_key = DAP_NEW(_wrapping_dap_chain_net_srv_callbacks_key_t);
        l_key->type = a_type;
        l_key->srv = a_srv;
        callbacks->key = l_key;
        callbacks->uid = a_uid;
        callbacks->func = a_obj_func;
        Py_XINCREF(a_obj_func);
        HASH_ADD(hh, _s_callbacks, key, sizeof(_wrapping_dap_chain_net_srv_callbacks_key_t), callbacks);
        return 0;
    }
    return -1;
}

PyObject *_wrapping_dac_chain_callback_data_t_get_tuple(
        dap_chain_net_srv_t *net_srv,
        uint32_t u32,
        dap_chain_net_srv_client_t * net_srv_client,
        const void *a4,
        size_t a5){
    //
    return 0;
}

int _w_dap_chain_callback_data_t_requested(
        dap_chain_net_srv_t *net_srv,
        uint32_t u32,
        dap_chain_net_srv_client_t * net_srv_client,
        const void *a4,
        size_t a5){
    _wrapping_dap_chain_net_srv_callbacks_key_t  *l_key = DAP_NEW(_wrapping_dap_chain_net_srv_callbacks_key_t);
    l_key->srv = net_srv;
    l_key->type = WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_REQUESTED;
    PyObject *l_func = _wrapping_dap_chain_net_srv_search(l_key);
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(net_srv, u32, net_srv_client, a4, a5);
    PyObject *result = PyObject_CallObject(l_func, l_arg);
    if(result == NULL){
        PyErr_Print();
        return -1;
    }
    Py_XINCREF(l_func);
    Py_XINCREF(l_arg);
    if (!PyLong_Check(result)){
        return -1;
    }
    int res_int = _PyLong_AsInt(result);
    return res_int;
}
int _w_dap_chain_callback_data_t_response_success(
        dap_chain_net_srv_t *net_srv,
        uint32_t u32,
        dap_chain_net_srv_client_t * net_srv_client,
        const void *a4,
        size_t a5){
    _wrapping_dap_chain_net_srv_callbacks_key_t  *l_key = DAP_NEW(_wrapping_dap_chain_net_srv_callbacks_key_t);
    l_key->srv = net_srv;
    l_key->type = WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_SUCCESS;
    PyObject *l_func = _wrapping_dap_chain_net_srv_search(l_key);
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(net_srv, u32, net_srv_client, a4, a5);
    PyObject *result = PyObject_CallObject(l_func, l_arg);
    if(result == NULL){
        PyErr_Print();
        return -1;
    }
    Py_XINCREF(l_func);
    Py_XINCREF(l_arg);
    if (!PyLong_Check(result)){
        return -1;
    }
    int res_int = _PyLong_AsInt(result);
    return res_int;
    return 0;
}
int _w_dap_chain_callback_data_t_response_error(
        dap_chain_net_srv_t *net_srv,
        uint32_t u32,
        dap_chain_net_srv_client_t * net_srv_client,
        const void *a4,
        size_t a5){
    _wrapping_dap_chain_net_srv_callbacks_key_t  *l_key = DAP_NEW(_wrapping_dap_chain_net_srv_callbacks_key_t);
    l_key->srv = net_srv;
    l_key->type = WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_ERROR;
    PyObject *l_func = _wrapping_dap_chain_net_srv_search(l_key);
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(net_srv, u32, net_srv_client, a4, a5);
    PyObject *result = PyObject_CallObject(l_func, l_arg);
    if(result == NULL){
        PyErr_Print();
        return -1;
    }
    Py_XINCREF(l_func);
    Py_XINCREF(l_arg);
    if (!PyLong_Check(result)){
        return -1;
    }
    int res_int = _PyLong_AsInt(result);
    return res_int;
    return 0;
}
int _w_dap_chain_callback_data_t_receipt_next_success(
        dap_chain_net_srv_t *net_srv,
        uint32_t u32,
        dap_chain_net_srv_client_t * net_srv_client,
        const void *a4,
        size_t a5){
    _wrapping_dap_chain_net_srv_callbacks_key_t  *l_key = DAP_NEW(_wrapping_dap_chain_net_srv_callbacks_key_t);
    l_key->srv = net_srv;
    l_key->type = WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RECEIPT_NEXT_SUCCESS;
    PyObject *l_func = _wrapping_dap_chain_net_srv_search(l_key);
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(net_srv, u32, net_srv_client, a4, a5);
    PyObject *result = PyObject_CallObject(l_func, l_arg);
    if(result == NULL){
        PyErr_Print();
        return -1;
    }
    Py_XINCREF(l_func);
    Py_XINCREF(l_arg);
    if (!PyLong_Check(result)){
        return -1;
    }
    int res_int = _PyLong_AsInt(result);
    return res_int;
    return 0;
}
int _w_dap_chain_callback_data_t_stream_ch_read(
        dap_chain_net_srv_t *net_srv,
        uint32_t u32,
        dap_chain_net_srv_client_t * net_srv_client,
        const void *a4,
        size_t a5){
    return 0;
}
int _w_dap_chain_callback_data_t_client_success(
        dap_chain_net_srv_t *net_srv,
        uint32_t u32,
        dap_chain_net_srv_client_t * net_srv_client,
        const void *a4,
        size_t a5){
    return 0;
}

//Conructor
int PyDapChainNetSrv_init(PyDapChainNetSrvObject* self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "uid",
            "callbackRequested",
            "callbackResponseSuccess",
            "callbackResponseError",
            "callbackReceiptNextSuccess",
            NULL};
    PyObject *obj_uid;
    PyObject *obj_callback_requested;
    PyObject *obj_callback_response_success;
    PyObject *obj_callback_response_error;
    PyObject *obj_callback_receipt_next_success;
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "OOOOO", kwlist,
                                    &obj_uid,
                                    &obj_callback_requested,
                                    &obj_callback_response_success,
                                    &obj_callback_response_error,
                                    &obj_callback_receipt_next_success)) {
        return -1;
    }
    if (
            PyDapChainNetSrvUid_Check(obj_uid)||
            PyCallable_Check(obj_callback_requested)||
            PyCallable_Check(obj_callback_response_success)||
            PyCallable_Check(obj_callback_response_error)||
            PyCallable_Check(obj_callback_receipt_next_success)) {
        self->srv = dap_chain_net_srv_add(
                ((PyDapChainNetSrvUIDObject*)obj_uid)->net_srv_uid,
                _w_dap_chain_callback_data_t_requested,
                _w_dap_chain_callback_data_t_response_success,
                _w_dap_chain_callback_data_t_response_error,
                _w_dap_chain_callback_data_t_receipt_next_success
                );
        if (self->srv == NULL){
            return -3;
        }
        _wrapping_dap_chain_net_srv_add(
                WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_REQUESTED,
                self->srv,
                ((PyDapChainNetSrvUIDObject*)self)->net_srv_uid,
                obj_callback_requested);
        _wrapping_dap_chain_net_srv_add(
                WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_SUCCESS,
                self->srv,
                ((PyDapChainNetSrvUIDObject*)self)->net_srv_uid,
                obj_callback_response_success);
        _wrapping_dap_chain_net_srv_add(
                WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_ERROR,
                self->srv,
                ((PyDapChainNetSrvUIDObject*)self)->net_srv_uid,
                obj_callback_response_error);
        _wrapping_dap_chain_net_srv_add(
                WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RECEIPT_NEXT_SUCCESS,
                self->srv,
                ((PyDapChainNetSrvUIDObject*)self)->net_srv_uid,
                obj_callback_receipt_next_success);
        return 0;
    }
    return -2;
}

PyObject *wrapping_dap_chain_net_srv_get_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *l_obj_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject, &DapChainNetSrvUIDObject_DapChainNetSrvUIDObjectType);
    PyObject_Dir((PyObject*)l_obj_srv_uid);
    l_obj_srv_uid->net_srv_uid = ((PyDapChainNetSrvObject *)self)->srv->uid;
    return (PyObject*)l_obj_srv_uid;
}
