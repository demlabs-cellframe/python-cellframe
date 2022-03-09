#include "wrapping_dap_chain_net_srv.h"

#define LOG_TAG "wrapping_dap_chain_net_srv"

PyObject *_wrapping_dac_chain_callback_data_t_get_tuple(
        dap_chain_net_srv_t *a_srv,
        uint32_t a_usage_id,
        dap_chain_net_srv_client_remote_t * a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    PyDapChainNetSrvObject *pyNetSrvObj = (PyDapChainNetSrvObject *)a_srv->_inheritor;
    PyDapChainNetSrvClientRemoteObject *l_obj_srv_client = NULL;
    if (a_srv_client == NULL){
        l_obj_srv_client = (PyDapChainNetSrvClientRemoteObject *)Py_None;
    } else {
        l_obj_srv_client = PyObject_New(PyDapChainNetSrvClientRemoteObject,
                                        &DapChainNetSrvClientRemoteObject_DapChainNetSrvClientRemoteObjectType);
        l_obj_srv_client->srv_client_remote = a_srv_client;
    }
    PyObject *l_obj_custom_data = NULL;
    if (a_custom_data == NULL || a_custom_data_size == 0){
        l_obj_custom_data = Py_None;
    }else{
        l_obj_custom_data = PyBytes_FromStringAndSize((char*)a_custom_data, (Py_ssize_t)a_custom_data_size);
    }
    return Py_BuildValue("OiOO", pyNetSrvObj, a_usage_id, l_obj_srv_client, l_obj_custom_data);
}

int _w_dap_chain_callback_data_t_requested(
        dap_chain_net_srv_t *a_srv,
        uint32_t a_usage_id,
        dap_chain_net_srv_client_remote_t * a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    PyDapChainNetSrvObject *pyNetSrvObj = (PyDapChainNetSrvObject *)a_srv->_inheritor;
    PyObject *l_func = pyNetSrvObj->callbackRequested;
    if (!PyCallable_Check(l_func)){
        log_it(L_ERROR, "Python function is not callable");
        return -1;
    }
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(a_srv, a_usage_id, a_srv_client, a_custom_data, a_custom_data_size);
    PyGILState_STATE state = PyGILState_Ensure();
    PyObject *result = PyObject_CallObject(l_func, l_arg);
    PyGILState_Release(state);
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
        dap_chain_net_srv_client_remote_t *a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    PyDapChainNetSrvObject *pyNetSrvObj = (PyDapChainNetSrvObject *)a_srv->_inheritor;
    PyObject *l_func = pyNetSrvObj->callbackSuccess;
    if (!PyCallable_Check(l_func)){
        log_it(L_ERROR, "Python function is not callable");
        return -1;
    }
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(a_srv, a_usage_id, a_srv_client, a_custom_data, a_custom_data_size);
    PyGILState_STATE state = PyGILState_Ensure();
    PyObject *result = PyObject_CallObject(l_func, l_arg);
    PyGILState_Release(state);
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
        dap_chain_net_srv_client_remote_t *a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    PyDapChainNetSrvObject *pyNetSrvObj = (PyDapChainNetSrvObject *)a_srv->_inheritor;
    PyObject *l_func = pyNetSrvObj->callbackError;
    if (!PyCallable_Check(l_func)){
        log_it(L_ERROR, "Python function is not callable");
        return -1;
    }
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(a_srv, a_usage_id, a_srv_client, a_custom_data, a_custom_data_size);
    PyGILState_STATE state = PyGILState_Ensure();
    PyObject *result = PyObject_CallObject(l_func, l_arg);
    PyGILState_Release(state);
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
        dap_chain_net_srv_client_remote_t *a_srv_client,
        const void *a_custom_data,
        size_t a_custom_data_size){
    PyDapChainNetSrvObject *pyNetSrvObj = (PyDapChainNetSrvObject *)a_srv->_inheritor;
    PyObject *l_func = pyNetSrvObj->callbackReceiptNext;
    if (!PyCallable_Check(l_func)){
        log_it(L_ERROR, "Python function is not callable");
        return -1;
    }
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(a_srv, a_usage_id, a_srv_client, a_custom_data, a_custom_data_size);
    PyGILState_STATE state = PyGILState_Ensure();
    PyObject *result = PyObject_CallObject(l_func, l_arg);
    PyGILState_Release(state);
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

void *_w_dap_chain_callback_data_t_custom_data(dap_chain_net_srv_t *a_srv,
                                               dap_chain_net_srv_usage_t *a_usage,
                                               const void *a_custom_data,
                                               size_t a_custom_data_size,
                                               size_t *a_out_data_size){
    PyDapChainNetSrvObject *pyNetSrvObj = (PyDapChainNetSrvObject *)a_srv->_inheritor;
    PyObject *l_func = pyNetSrvObj->callbackReadWithOutData;
    if (!PyCallable_Check(l_func)){
        log_it(L_ERROR, "Python function is not callable");
        return NULL;
    }
    Py_INCREF(l_func);
    PyObject *l_arg = _wrapping_dac_chain_callback_data_t_get_tuple(a_srv, a_usage ? a_usage->id : 0,
                                                                    a_usage ? a_usage->client : NULL,
                                                                    a_custom_data, a_custom_data_size);
    PyGILState_STATE state = PyGILState_Ensure();
    PyObject *result = PyObject_CallObject(l_func, l_arg);
    PyGILState_Release(state);
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

//Constructor
int PyDapChainNetSrv_init(PyDapChainNetSrvObject* self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "uid",
            "section",
            "callbackRequested",
            "callbackResponseSuccess",
            "callbackResponseError",
            "callbackReceiptNextSuccess",
            "callbackCustomData",
            NULL};
    PyDapChainNetSrvUIDObject *obj_uid;
    const char *l_section;
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "OsOOOOO", (char **)kwlist,
                                    &obj_uid,
                                    &l_section,
                                    &self->callbackRequested,
                                    &self->callbackSuccess,
                                    &self->callbackError,
                                    &self->callbackReceiptNext,
                                    &self->callbackReadWithOutData)) {
        return -1;
    }
    if (PyDapChainNetSrvUid_Check(obj_uid) &&
            PyCallable_Check(self->callbackRequested) &&
            PyCallable_Check(self->callbackSuccess) &&
            PyCallable_Check(self->callbackError) &&
            PyCallable_Check(self->callbackReceiptNext) &&
            PyCallable_Check(self->callbackReadWithOutData)) {
        self->srv = dap_chain_net_srv_add(
                obj_uid->net_srv_uid,
                l_section,
                _w_dap_chain_callback_data_t_requested,
                _w_dap_chain_callback_data_t_response_success,
                _w_dap_chain_callback_data_t_response_error,
                _w_dap_chain_callback_data_t_receipt_next_success,
                _w_dap_chain_callback_data_t_custom_data
                );
        if (self->srv == NULL){
            return -3;
        }
        self->srv->_inheritor = self;
        Py_INCREF(self);
        return 0;
    }
    return -2;
}

void PyDapChainNetSrv_dealloc(PyDapChainNetSrvObject* self){
    dap_chain_net_srv_del(self->srv);
    Py_XDECREF(self);
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
    PyObject *obj_ch_open, *obj_ch_write, *obj_ch_closed;
    if (!PyArg_ParseTuple(args, "OOO", &obj_ch_open, &obj_ch_write, &obj_ch_closed))
        return Py_None;
    if (
            !PyCallable_Check(obj_ch_open) ||
            !PyCallable_Check(obj_ch_write) ||
            !PyCallable_Check(obj_ch_closed)){
        // TODO wrap channel callbacks
        return Py_None;
    }
    return Py_None;
}
