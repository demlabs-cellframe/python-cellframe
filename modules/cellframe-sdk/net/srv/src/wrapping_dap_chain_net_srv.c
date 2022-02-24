#include "wrapping_dap_chain_net_srv.h"

#define LOG_TAG "wrapping_dap_chain_net_srv"

typedef enum _wrapping_dap_chain_net_srv_type_callbacks{
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_REQUESTED,
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_SUCCESS,
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_ERROR,
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RECEIPT_NEXT_SUCCESS,
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ,
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ_WITH_OUT_DATA,
    WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_CLIENT_SUCCESS
}_wrapping_dap_chain_net_srv_type_callbacks_t;

typedef struct _wrapping_dap_chain_net_srv_callback_struct{
    void *srv;
    PyObject *func;
    UT_hash_handle hh;
}_wrapping_dap_chain_net_srv_callback_struct_t;

_wrapping_dap_chain_net_srv_callback_struct_t *_s_callbacks_stream_ch_read_t = NULL;

_wrapping_dap_chain_net_srv_callback_struct_t *_s_callbacks_stream_ch_read_with_out_data_t = NULL;

_wrapping_dap_chain_net_srv_callback_struct_t *_s_callbacks_requested_t = NULL;

_wrapping_dap_chain_net_srv_callback_struct_t *_s_callbacks_response_success_t = NULL;

_wrapping_dap_chain_net_srv_callback_struct_t *_s_callbacks_response_error_t = NULL;

_wrapping_dap_chain_net_srv_callback_struct_t *_s_callbacks_receipt_next_success_t = NULL;

_wrapping_dap_chain_net_srv_callback_struct_t *_wrapping_dap_chain_net_srv_callbacks_search_el(
        _wrapping_dap_chain_net_srv_type_callbacks_t a_type,
        dap_chain_net_srv_t *a_srv
        ){
    if (!a_srv)
        return NULL;
    _wrapping_dap_chain_net_srv_callback_struct_t *l_callback = NULL;
    switch(a_type){
        case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RECEIPT_NEXT_SUCCESS:
            HASH_FIND_PTR(_s_callbacks_receipt_next_success_t, &a_srv, l_callback);
            break;
        case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_ERROR:
            HASH_FIND_PTR(_s_callbacks_response_error_t, &a_srv, l_callback);
            break;
        case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_SUCCESS:
            HASH_FIND_PTR(_s_callbacks_response_success_t, &a_srv, l_callback);
            break;
        case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_REQUESTED:
            HASH_FIND_PTR(_s_callbacks_requested_t, &a_srv, l_callback);
            break;
        case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ:
            HASH_FIND_PTR(_s_callbacks_stream_ch_read_t, &a_srv, l_callback);
            break;
        case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ_WITH_OUT_DATA:
            HASH_FIND_PTR(_s_callbacks_stream_ch_read_with_out_data_t, &a_srv, l_callback);
            break;
        default:
            return NULL;
    }
    return l_callback;
}

void _wrapping_dap_chain_net_srv_del(
        _wrapping_dap_chain_net_srv_type_callbacks_t a_type,
        dap_chain_net_srv_t *a_srv){
        _wrapping_dap_chain_net_srv_callback_struct_t *a_callback =
                _wrapping_dap_chain_net_srv_callbacks_search_el(a_type, a_srv);
    if (a_callback != NULL) {
        switch (a_type) {
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_REQUESTED:
                HASH_DEL(_s_callbacks_requested_t, a_callback);
                break;
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_SUCCESS:
                HASH_DEL(_s_callbacks_response_success_t, a_callback);
                break;
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_ERROR:
                HASH_DEL(_s_callbacks_response_error_t, a_callback);
                break;
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RECEIPT_NEXT_SUCCESS:
                HASH_DEL(_s_callbacks_receipt_next_success_t, a_callback);
                break;
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ:
                HASH_DEL(_s_callbacks_stream_ch_read_t, a_callback);
                break;
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ_WITH_OUT_DATA:
                HASH_DEL(_s_callbacks_stream_ch_read_with_out_data_t, a_callback);
                break;
            default: break;
        }
//        HASH_DEL(_s_callbacks, a_callback);
        Py_XINCREF(a_callback->func);
    }
}

PyObject* _wrapping_dap_chain_net_srv_callback_search(_wrapping_dap_chain_net_srv_type_callbacks_t a_type,
                                             dap_chain_net_srv_t *a_srv){
    _wrapping_dap_chain_net_srv_callback_struct_t *l_callbacks = _wrapping_dap_chain_net_srv_callbacks_search_el(a_type, a_srv);
    if (l_callbacks == NULL){
        return Py_None;
    } else {
        return l_callbacks->func;
    }
}

int _wrapping_dap_chain_net_srv_add(
                enum _wrapping_dap_chain_net_srv_type_callbacks a_type,
                dap_chain_net_srv_t *a_srv,
                dap_chain_net_srv_uid_t a_uid,
                PyObject *a_obj_func){
    if (PyCallable_Check(a_obj_func)){
        _wrapping_dap_chain_net_srv_callback_struct_t *l_callback = DAP_NEW(_wrapping_dap_chain_net_srv_callback_struct_t);
        l_callback->srv = a_srv;
        l_callback->func = a_obj_func;
        Py_INCREF(a_obj_func);
        switch (a_type) {
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ:
                HASH_ADD_PTR(_s_callbacks_stream_ch_read_t, srv, l_callback);
                break;
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RECEIPT_NEXT_SUCCESS:
                HASH_ADD_PTR(_s_callbacks_receipt_next_success_t, srv, l_callback);
                break;
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_ERROR:
                HASH_ADD_PTR(_s_callbacks_response_error_t, srv, l_callback);
                break;
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_SUCCESS:
                HASH_ADD_PTR(_s_callbacks_response_success_t, srv, l_callback);
                break;
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_REQUESTED:
                HASH_ADD_PTR(_s_callbacks_requested_t, srv, l_callback);
                break;
            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ_WITH_OUT_DATA:
                HASH_ADD_PTR(_s_callbacks_stream_ch_read_with_out_data_t, srv, l_callback);
                break;
            default:
                return -2;
//            case WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_CLIENT_SUCCESS:
//                HASH_ADD(hh, _s_callbacks_, srv, sizeof(dap_chain_net_srv_t), l_callback);
//                break;
        }
        return 0;
    }
    return -1;
}

PyObject *_wrapping_dac_chain_callback_data_t_get_tuple(
        dap_chain_net_srv_t *a_srv, //NOT USAGE
        uint32_t a_usage_id,
        dap_chain_net_srv_client_remote_t * a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    PyDapChainNetSrvClientRemoteObject *l_obj_srv_client = NULL;
    if (a_srv_client == NULL){
        l_obj_srv_client = (PyDapChainNetSrvClientRemoteObject *)Py_None;
    } else {
        l_obj_srv_client = PyObject_New(PyDapChainNetSrvClientRemoteObject,
                                        &DapChainNetSrvClientRemoteObject_DapChainNetSrvClientRemoteObjectType);
        PyObject_Dir((PyObject *) l_obj_srv_client);
        l_obj_srv_client->srv_client_remote = a_srv_client;
    }
    PyObject *l_obj_custom_data = NULL;
    if (a_custom_data == NULL || a_custom_data_size == 0){
        l_obj_custom_data = Py_None;
    }else{
        l_obj_custom_data = PyBytes_FromStringAndSize((char*)a_custom_data, (Py_ssize_t)a_custom_data_size);
    }
    return Py_BuildValue("iOO", a_usage_id, l_obj_srv_client, l_obj_custom_data);
}

int _w_dap_chain_callback_data_t_requested(
        dap_chain_net_srv_t *a_srv,
        uint32_t a_usage_id,
        dap_chain_net_srv_client_remote_t * a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    PyObject *l_func = _wrapping_dap_chain_net_srv_callback_search(WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_REQUESTED, a_srv);
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(a_srv, a_usage_id, a_srv_client, a_custom_data, a_custom_data_size);
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
        dap_chain_net_srv_t *a_srv,
        uint32_t a_usage_id,
        dap_chain_net_srv_client_remote_t * a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    PyObject *l_func = _wrapping_dap_chain_net_srv_callback_search(
            WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_SUCCESS,
            a_srv);
    if (!PyCallable_Check(l_func)){
        log_it(L_ERROR, "Function python is not callabel");
        return -1;
    }
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(a_srv, a_usage_id, a_srv_client, a_custom_data, a_custom_data_size);
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
        dap_chain_net_srv_t *a_srv,
        uint32_t a_usage_id,
        dap_chain_net_srv_client_remote_t * a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    PyObject *l_func = _wrapping_dap_chain_net_srv_callback_search(
            WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_ERROR, a_srv);
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(a_srv, a_usage_id, a_srv_client, a_custom_data, a_custom_data_size);
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
        dap_chain_net_srv_t *a_srv,
        uint32_t a_usage_id,
        dap_chain_net_srv_client_remote_t * a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    PyObject *l_func = _wrapping_dap_chain_net_srv_callback_search(
            WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RECEIPT_NEXT_SUCCESS, a_srv);
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(a_srv, a_usage_id, a_srv_client, a_custom_data, a_custom_data_size);
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
        dap_chain_net_srv_t *a_srv,
        uint32_t a_usage_id,
        dap_chain_net_srv_client_remote_t * a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    PyGILState_STATE state = PyGILState_Ensure();
    PyObject *l_func = _wrapping_dap_chain_net_srv_callback_search(
            WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ, a_srv);
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(a_srv, a_usage_id, a_srv_client, a_custom_data, a_custom_data_size);
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
    PyGILState_Release(state);
    return res_int;
}

void *_w_dap_chain_callback_data_t_stream_ch_read_with_out_data(
        dap_chain_net_srv_t *a_srv,
        uint32_t a_usage_id,
        dap_chain_net_srv_client_remote_t * a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size,
        size_t *a_out_data_size){
    PyGILState_STATE state = PyGILState_Ensure();
    PyObject *l_func = _wrapping_dap_chain_net_srv_callback_search(
            WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ_WITH_OUT_DATA, a_srv);
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(a_srv, a_usage_id, a_srv_client, a_custom_data, a_custom_data_size);
    PyObject *result = PyObject_CallObject(l_func, l_arg);
    if(result == NULL){
        PyErr_Print();
        return NULL;
    }
    Py_XINCREF(l_func);
    Py_XINCREF(l_arg);
    if (!PyBytes_Check(result)){
        return NULL;
    }
    void *l_data = PyBytes_AsString(result);
    if (a_out_data_size)
        *a_out_data_size = (size_t)PyBytes_Size(result);
    PyGILState_Release(state);
    return l_data;
}

int _w_dap_chain_callback_data_t_client_success(
        dap_chain_net_srv_t *a_srv,
        uint32_t a_usage_id,
        dap_chain_net_srv_client_remote_t * a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    return 0;
}

/* working stram ch */
//void wrapping_dap_chain_net_srv_callback_ch_read(dap_chain_net_srv_t *a_srv, uint32_t){
//    a_stream_ch->
//}

//Conructor
int PyDapChainNetSrv_init(PyDapChainNetSrvObject* self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "uid",
            "callbackRequested",
            "callbackResponseSuccess",
            "callbackResponseError",
            "callbackReceiptNextSuccess",
            "callbackInData",
            "callbackInDataAndOutData",
            NULL};
    PyObject *obj_uid;
    PyObject *obj_callback_requested;
    PyObject *obj_callback_response_success;
    PyObject *obj_callback_response_error;
    PyObject *obj_callback_receipt_next_success;
    PyObject *obj_callback_in_data;
    PyObject *obj_callback_in_data_and_out_data;
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "OOOOOOO", (char **)kwlist,
                                    &obj_uid,
                                    &obj_callback_requested,
                                    &obj_callback_response_success,
                                    &obj_callback_response_error,
                                    &obj_callback_receipt_next_success,
                                    &obj_callback_in_data,
                                    &obj_callback_in_data_and_out_data)) {
        return -1;
    }
    if (
            PyDapChainNetSrvUid_Check(obj_uid)||
            PyCallable_Check(obj_callback_requested)||
            PyCallable_Check(obj_callback_response_success)||
            PyCallable_Check(obj_callback_response_error)||
            PyCallable_Check(obj_callback_receipt_next_success)||
            PyCallable_Check(obj_callback_in_data)||
            PyCallable_Check(obj_callback_in_data_and_out_data)) {
        self->srv = dap_chain_net_srv_add(
                ((PyDapChainNetSrvUIDObject*)obj_uid)->net_srv_uid,
                _w_dap_chain_callback_data_t_requested,
                _w_dap_chain_callback_data_t_response_success,
                _w_dap_chain_callback_data_t_response_error,
                _w_dap_chain_callback_data_t_receipt_next_success
                );
        dap_chain_net_srv_set_ch_callbacks(((PyDapChainNetSrvUIDObject*)obj_uid)->net_srv_uid,
                                           NULL,
                                           _w_dap_chain_callback_data_t_stream_ch_read,
                                           _w_dap_chain_callback_data_t_stream_ch_read_with_out_data, NULL, NULL);
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
        _wrapping_dap_chain_net_srv_add(WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ,
                                        self->srv,
                                        ((PyDapChainNetSrvUIDObject*)self)->net_srv_uid,
                                        obj_callback_in_data);
        _wrapping_dap_chain_net_srv_add(WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ_WITH_OUT_DATA,
                                        self->srv,
                                        ((PyDapChainNetSrvUIDObject*)self)->net_srv_uid,
                                        obj_callback_in_data);
        self->original = true;
        return 0;
    }
    return -2;
}

void PyDapChainNetSrv_dealloc(PyDapChainNetSrvObject* self){
    if(self->original == true){
        _wrapping_dap_chain_net_srv_del(WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ, self->srv);
        _wrapping_dap_chain_net_srv_del(WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_STREAM_CH_READ_WITH_OUT_DATA, self->srv);
        _wrapping_dap_chain_net_srv_del(WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RECEIPT_NEXT_SUCCESS, self->srv);
        _wrapping_dap_chain_net_srv_del(WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_ERROR, self->srv);
        _wrapping_dap_chain_net_srv_del(WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_RESPONSE_SUCCESS, self->srv);
        _wrapping_dap_chain_net_srv_del(WRAPPING_DAP_CHAIN_NET_SERV_CALLBACK_DATA_REQUESTED, self->srv);
        dap_chain_net_srv_del(self->srv);
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *wrapping_dap_chain_net_srv_get_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *l_obj_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject, &DapChainNetSrvUIDObject_DapChainNetSrvUIDObjectType);
    PyObject_Dir((PyObject*)l_obj_srv_uid);
    l_obj_srv_uid->net_srv_uid = ((PyDapChainNetSrvObject *)self)->srv->uid;
    return (PyObject*)l_obj_srv_uid;
}

PyObject *wrapping_dap_chain_net_srv_get_grace_period(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainNetSrvObject*)self)->srv->grace_period);
}

/* callbacks stream ch */

PyObject *wrapping_dap_chain_net_srv_set_callback_channel(PyObject *self, PyObject *args){
    PyObject *obj_ch_open, *obj_ch_read, *obj_ch_write, *obj_ch_closed;
    if (!PyArg_ParseTuple(args, "OOOO", &obj_ch_open, &obj_ch_read, &obj_ch_write, &obj_ch_closed))
        return Py_None;
    if (
            !PyCallable_Check(obj_ch_open) ||
            !PyCallable_Check(obj_ch_read) ||
            !PyCallable_Check(obj_ch_write) ||
            !PyCallable_Check(obj_ch_closed)){
        return Py_None;
    }
    return Py_None;
}
