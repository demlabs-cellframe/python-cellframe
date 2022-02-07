#include "libdap_client_python.h"

typedef struct _wrapping_dap_client_callbacks{
    dap_client_t client;
    PyObject *func;
    UT_hash_handle hh;
}_wrapping_dap_client_callbacks_t;

typedef enum _wrapping_dap_client_callbacks_type{
    _STAGE_STATUS_ERROR,
    _STAGE_STATUS,
    _STAGE_STATUS_GO_END,
    _RESPONSE_PROC,
    _RESPONSE_ERROR,
    _ENC_RESPONSE_PROC,
    _ENC_RESPONSE_ERROR
}_wrapping_dap_client_callbacks_type_t;

static _wrapping_dap_client_callbacks_t *_s_callbacks_stage_status_error = NULL;
static _wrapping_dap_client_callbacks_t *_s_callbacks_stage_status = NULL;
static _wrapping_dap_client_callbacks_t *_s_callbacks_stage_status_go_end = NULL;
static _wrapping_dap_client_callbacks_t *_s_callbacks_stage_response_proc = NULL;
static _wrapping_dap_client_callbacks_t *_s_callbacks_stage_response_error = NULL;
static _wrapping_dap_client_callbacks_t *_s_callbacks_stage_enc_response_proc = NULL;
static _wrapping_dap_client_callbacks_t *_s_callbacks_stage_enc_response_error = NULL;

void _wrapping_dap_client_callback_del_element(_wrapping_dap_client_callbacks_type_t a_type,
                                            _wrapping_dap_client_callbacks_t *a_element){
    if (a_element != NULL) {
        switch (a_type) {
            case _STAGE_STATUS_ERROR:
                HASH_DEL(_s_callbacks_stage_status_error, a_element);
                break;
            case _STAGE_STATUS:
                HASH_DEL(_s_callbacks_stage_status, a_element);
                break;
            case _STAGE_STATUS_GO_END:
                HASH_DEL(_s_callbacks_stage_status_go_end, a_element);
                break;
            case _RESPONSE_PROC:
                HASH_DEL(_s_callbacks_stage_response_proc, a_element);
                break;
            case _RESPONSE_ERROR:
                HASH_DEL(_s_callbacks_stage_response_error, a_element);
                break;
            case _ENC_RESPONSE_PROC:
                HASH_DEL(_s_callbacks_stage_enc_response_proc, a_element);
                break;
            case _ENC_RESPONSE_ERROR:
                HASH_DEL(_s_callbacks_stage_enc_response_error, a_element);
                break;
        }
        Py_XDECREF(a_element->func);
    }
}

_wrapping_dap_client_callbacks_t *_wrapping_dap_client_callback_search_element(
        _wrapping_dap_client_callbacks_type_t a_type, dap_client_t *a_client){
    _wrapping_dap_client_callbacks_t *l_callback = NULL;
    switch(a_type){
        case _STAGE_STATUS_ERROR:
            HASH_FIND(hh, _s_callbacks_stage_status_error, a_client, sizeof(dap_client_t), l_callback);
            break;
        case _STAGE_STATUS:
            HASH_FIND(hh, _s_callbacks_stage_status, a_client, sizeof(dap_client_t), l_callback);
            break;
        case _STAGE_STATUS_GO_END:
            HASH_FIND(hh, _s_callbacks_stage_status_go_end, a_client, sizeof(dap_client_t), l_callback);
            break;
        case _RESPONSE_PROC:
            HASH_FIND(hh, _s_callbacks_stage_response_proc, a_client, sizeof(dap_client_t), l_callback);
            break;
        case _RESPONSE_ERROR:
            HASH_FIND(hh, _s_callbacks_stage_response_error, a_client, sizeof(dap_client_t), l_callback);
            break;
        case _ENC_RESPONSE_PROC:
            HASH_FIND(hh, _s_callbacks_stage_enc_response_proc, a_client, sizeof(dap_client_t), l_callback);
            break;
        case _ENC_RESPONSE_ERROR:
            HASH_FIND(hh, _s_callbacks_stage_enc_response_error, a_client, sizeof(dap_client_t), l_callback);
            break;
    }
//    HASH_FIND(hh, a_callbacks, &a_client, sizeof(dap_client_t*), l_callback);
    return l_callback;
}

void _wrapping_dap_client_callback_del(_wrapping_dap_client_callbacks_type_t a_type,
                                                    dap_client_t *a_client){
    _wrapping_dap_client_callbacks_t *l_callback = _wrapping_dap_client_callback_search_element(a_type, a_client);
    _wrapping_dap_client_callback_del_element(a_type, l_callback);
    DAP_FREE(l_callback);
}


PyObject *_wrapping_dap_client_callback_search(_wrapping_dap_client_callbacks_type_t a_type,
                                               dap_client_t *a_client){
    _wrapping_dap_client_callbacks_t *l_callback = _wrapping_dap_client_callback_search_element(a_type, a_client);
    if (l_callback == NULL)
        return NULL;
    return l_callback->func;
}

bool _wrapping_dap_client_callback_add(_wrapping_dap_client_callbacks_type_t a_type,
                                       dap_client_t *a_client, PyObject *a_func){
    if (PyCallable_Check(a_func)) {
        _wrapping_dap_client_callbacks_t *l_callback = DAP_NEW(_wrapping_dap_client_callbacks_t);
        l_callback->client = *a_client;
        l_callback->func = a_func;
        Py_INCREF(a_func);
        switch(a_type){
            case _STAGE_STATUS_ERROR:
                HASH_ADD(hh, _s_callbacks_stage_status_error, client, sizeof(dap_client_t), l_callback);
                break;
            case _STAGE_STATUS:
                HASH_ADD(hh, _s_callbacks_stage_status, client, sizeof(dap_client_t), l_callback);
                break;
            case _STAGE_STATUS_GO_END:
                HASH_ADD(hh, _s_callbacks_stage_status_go_end, client, sizeof(dap_client_t), l_callback);
                break;
            case _RESPONSE_PROC:
                HASH_ADD(hh, _s_callbacks_stage_response_proc, client, sizeof(dap_client_t), l_callback);
                break;
            case _RESPONSE_ERROR:
                HASH_ADD(hh, _s_callbacks_stage_response_error, client, sizeof(dap_client_t), l_callback);
                break;
            case _ENC_RESPONSE_PROC:
                HASH_ADD(hh, _s_callbacks_stage_enc_response_proc, client, sizeof(dap_client_t), l_callback);
                break;
            case _ENC_RESPONSE_ERROR:
                HASH_ADD(hh, _s_callbacks_stage_enc_response_error, client, sizeof(dap_client_t), l_callback);
                break;
        }
        return true;
    }
    return false;
}

int dap_client_init_py(){
    return dap_client_init();
}
void dap_client_deinit_py(){
    dap_client_deinit();
}

void _wrapping_callback_stage_status(dap_client_t *a_client, void *a_data){
    PyGILState_STATE l_state = PyGILState_Ensure();
    PyObject *l_call = _wrapping_dap_client_callback_search(_STAGE_STATUS, a_client);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_obj_args = NULL;
    if (a_data){
        l_obj_args = Py_BuildValue("OO", l_obj_client, a_data);
    } else {
        l_obj_args = Py_BuildValue("(O)", l_obj_client);
    }
    Py_INCREF(l_call);
    Py_INCREF(l_obj_args);
    PyEval_CallObject(l_call, l_obj_args);
    Py_XINCREF(l_call);
    Py_XINCREF(l_obj_args);
    PyGILState_Release(l_state);
}
void _wrapping_callback_stage_status_error(dap_client_t *a_client, void *a_data){
    PyGILState_STATE l_state = PyGILState_Ensure();
    PyObject *l_call = _wrapping_dap_client_callback_search(_STAGE_STATUS_ERROR, a_client);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_obj_args = NULL;
    if (a_data){
        l_obj_args = Py_BuildValue("Oi", l_obj_client, (int)a_data);
    } else {
        l_obj_args = Py_BuildValue("(O)", l_obj_client);
    }
    Py_INCREF(l_call);
    Py_INCREF(l_obj_args);
    PyEval_CallObject(l_call, l_obj_args);
    Py_XINCREF(l_call);
    Py_XINCREF(l_obj_args);
    PyGILState_Release(l_state);
}

void _wrapping_callback_stage_go_end(dap_client_t *a_client, void *a_data){
    PyGILState_STATE l_state = PyGILState_Ensure();
    PyObject *l_call = _wrapping_dap_client_callback_search(_STAGE_STATUS_GO_END, a_client);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    PyObject_Dir(l_obj_client);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_obj_args = NULL;
    if (a_data){
        l_obj_args = Py_BuildValue("OO", l_obj_client, a_data);
    } else {
        l_obj_args = Py_BuildValue("(O)", l_obj_client);
    }
    Py_INCREF(l_obj_args);
    Py_INCREF(l_call);
    PyEval_CallObject(l_call, l_obj_args);
    Py_XDECREF(l_obj_args);
    Py_XDECREF(l_call);
    PyGILState_Release(l_state);
    _wrapping_dap_client_callback_del(_STAGE_STATUS_GO_END, a_client);
}

void _wrapping_callback_response_proc(dap_client_t *a_client, void *a_data, size_t a_data_size){
    PyGILState_STATE l_state = PyGILState_Ensure();
    PyObject *l_call = _wrapping_dap_client_callback_search(_RESPONSE_PROC, a_client);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    PyObject_Dir(l_obj_client);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_obj_args = NULL;
    if (a_data){
        PyObject *obj_bytes = PyBytes_FromStringAndSize(a_data, a_data_size);
        l_obj_args = Py_BuildValue("OO", l_obj_client, obj_bytes);
    } else {
        l_obj_args = Py_BuildValue("(O)", l_obj_client);
    }
    Py_INCREF(l_obj_args);
    Py_INCREF(l_call);
    PyEval_CallObject(l_call, l_obj_args);
    Py_XDECREF(l_obj_args);
    Py_XDECREF(l_call);
    if (a_data) {
        Py_XDECREF(l_obj_args);
    }
    PyGILState_Release(l_state);
    _wrapping_dap_client_callback_del(_RESPONSE_PROC, a_client);
}
void _wrapping_callback_response_error(dap_client_t *a_client, int a_integer){
    PyGILState_STATE l_state = PyGILState_Ensure();
    PyObject *l_call = _wrapping_dap_client_callback_search(_RESPONSE_ERROR, a_client);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    PyObject_Dir(l_obj_client);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_obj_args = NULL;
        l_obj_args = Py_BuildValue("Oi", l_obj_client, a_integer);
    Py_INCREF(l_obj_args);
    Py_INCREF(l_call);
    PyEval_CallObject(l_call, l_obj_args);
    Py_XDECREF(l_obj_args);
    Py_XDECREF(l_call);
    PyGILState_Release(l_state);
    _wrapping_dap_client_callback_del(_RESPONSE_ERROR, a_client);
}

void _wrapping_callback_enc_response_proc(dap_client_t *a_client, void *a_data, size_t a_data_size){
    PyGILState_STATE l_state = PyGILState_Ensure();
    PyObject *l_call = _wrapping_dap_client_callback_search(_RESPONSE_PROC, a_client);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    PyObject_Dir(l_obj_client);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_obj_args = NULL;
    if (a_data){
        PyObject *obj_bytes = PyBytes_FromStringAndSize(a_data, a_data_size);
        l_obj_args = Py_BuildValue("OO", l_obj_client, obj_bytes);
    } else {
        l_obj_args = Py_BuildValue("(O)", l_obj_client);
    }
    Py_INCREF(l_obj_args);
    Py_INCREF(l_call);
    PyEval_CallObject(l_call, l_obj_args);
    Py_XDECREF(l_obj_args);
    Py_XDECREF(l_call);
    if (a_data) {
        Py_XDECREF(l_obj_args);
    }
    PyGILState_Release(l_state);
    _wrapping_dap_client_callback_del(_RESPONSE_PROC, a_client);
}

void _wrapping_callback_enc_response_error(dap_client_t *a_client, int a_integer){
    PyGILState_STATE l_state = PyGILState_Ensure();
    PyObject *l_call = _wrapping_dap_client_callback_search(_ENC_RESPONSE_ERROR, a_client);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    PyObject_Dir(l_obj_client);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_obj_args = NULL;
    l_obj_args = Py_BuildValue("Oi", l_obj_client, a_integer);
    Py_INCREF(l_obj_args);
    Py_INCREF(l_call);
    PyEval_CallObject(l_call, l_obj_args);
    Py_XDECREF(l_obj_args);
    Py_XDECREF(l_call);
    PyGILState_Release(l_state);
    _wrapping_dap_client_callback_del(_ENC_RESPONSE_ERROR, a_client);
}

int dap_client_obj_init(PyDapClientObject *self, PyObject *args, PyObject *kwds){
    char *kwlist[] = {"events", "stage", "stageError", NULL};
    PyObject *obj_events = NULL;
    PyObject *obj_func_stage = NULL;
    PyObject *obj_func_stage_error = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOO", kwlist, &obj_events, &obj_func_stage, &obj_func_stage_error)){
        return -1;
    }
    dap_client_t *l_client = dap_client_new(((PyDapEventsObject*)obj_events)->t_events,
                                            _wrapping_callback_stage_status,
                                            _wrapping_callback_stage_status_error);
    self->client = l_client;
    _wrapping_dap_client_callback_add(_STAGE_STATUS, l_client, obj_func_stage);
    _wrapping_dap_client_callback_add(_STAGE_STATUS_ERROR, l_client, obj_func_stage_error);
    return 0;
}

PyObject *dap_client_set_uplink_unsafe_py(PyObject *self, PyObject *args)
{
    const char *l_addr = NULL;
    uint16_t l_port;
    if (!PyArg_ParseTuple(args, "sI", &l_addr, &l_port)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    dap_client_set_uplink_unsafe(((PyDapClientObject*)self)->client, l_addr, l_port);
    return Py_None;
}

PyObject *dap_client_get_uplink_addr_unsafe_py(PyObject *self, PyObject *args)
{
    (void)args;
    const char *l_addr = dap_client_get_uplink_addr_unsafe(((PyDapClientObject*)self)->client);
    return Py_BuildValue("s", l_addr);
}

PyObject *dap_client_get_uplink_port_unsafe_py(PyObject *self, PyObject *args)
{
    (void)args;
    uint16_t l_port = dap_client_get_uplink_port_unsafe(((PyDapClientObject*)self)->client);
    return Py_BuildValue("I", l_port);
}

PyObject *dap_client_get_key_stream_py(PyObject *self, PyObject *args)
{
    (void)args;
    dap_enc_key_t *l_key = dap_client_get_key_stream(((PyDapClientObject*)self)->client);
    PyCryptoKeyObject *obj_key = (PyCryptoKeyObject*)_PyObject_New(&PyCryptoKeyObject_PyCryptoKeyType);
    obj_key->key = l_key;
    return (PyObject *)obj_key;
}

PyObject *dap_client_go_stage_py(PyObject *self, PyObject *args)
{
    PyObject *l_obj_stage = NULL;
    PyObject *l_obj_stage_callback = NULL;
    if (!PyArg_ParseTuple(args, "OO", &l_obj_stage, &l_obj_stage_callback)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyCallable_Check(l_obj_stage_callback)){
        PyErr_SetString(PyExc_SyntaxError, "Sorry, but the second argument is not a callback function");
        return NULL;
    }
    if (!PyDapClientStageObject_Check(l_obj_stage)){
        PyErr_SetString(PyExc_SyntaxError, "Sorry, but the first argument is not a ClientStage object");
        return NULL;
    }
    _wrapping_dap_client_callback_add(_STAGE_STATUS_GO_END, ((PyDapClientObject*)self)->client, l_obj_stage_callback);
    dap_client_go_stage(((PyDapClientObject*)self)->client, ((PyDapClientStageObject*)l_obj_stage)->stage, _wrapping_callback_stage_go_end);
    return Py_None;
}

void dap_client_delete_mt_py(PyObject *self){
    PyTypeObject *tp = Py_TYPE(self);
    dap_client_delete_mt(((PyDapClientObject*)self)->client);
    tp->tp_free(self);
    Py_DECREF(tp);
}

//PyObject *dap_client_reset_py(PyObject *self, PyObject *args)
//{
//    (void) self;
//    (void) args;
//    /// TODO: Implement it!
//    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
//    return NULL;
//}

PyObject *dap_client_request_enc_unsafe_py(PyObject *self, PyObject *args)
{
    const char *l_path = NULL;
    const char *l_suburl = NULL;
    const char *l_query = NULL;
    PyObject *l_obj_request = NULL;
    PyObject *l_obj_response_proc = NULL;
    PyObject *l_obj_response_error = NULL;
    if (!PyArg_ParseTuple(args, "sssOOO", &l_path, &l_suburl, &l_query, &l_obj_request, &l_obj_response_proc, &l_obj_response_error)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyBytes_Check(l_obj_request)){
        return NULL;
    }
    if (!PyCallable_Check(l_obj_response_proc) && !PyCallable_Check(l_obj_response_error)){
        PyErr_SetString(PyExc_SyntaxError, "Sorry, but the second argument is not a callback function");
        return NULL;
    }
    void *l_request = PyBytes_AsString(l_obj_request);
    size_t l_request_size = (size_t)PyBytes_Size(l_obj_request);
    _wrapping_dap_client_callback_add(_ENC_RESPONSE_PROC,
                                      ((PyDapClientObject*)self)->client, l_obj_response_proc);
    _wrapping_dap_client_callback_add(_ENC_RESPONSE_ERROR,
                                      ((PyDapClientObject*)self)->client, l_obj_response_error);
    dap_client_request_enc_unsafe(((PyDapClientObject*)self)->client,
                                  l_path,
                                  l_suburl,
                                  l_query,
                                  l_request,
                                  l_request_size,
                                  _wrapping_callback_response_proc,
                                  _wrapping_callback_response_error);
    return Py_None;
}

PyObject *dap_client_request_unsafe_py(PyObject *self, PyObject *args)
{
    const char *l_path = NULL;
    const char *l_query = NULL;
    PyObject *l_obj_request = NULL;
    PyObject *l_obj_response_proc = NULL;
    PyObject *l_obj_response_error = NULL;
    if (!PyArg_ParseTuple(args, "ssOOO", &l_path, &l_query, &l_obj_request, &l_obj_response_proc, &l_obj_response_error)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyBytes_Check(l_obj_request)){
        return NULL;
    }
    if (!PyCallable_Check(l_obj_response_proc) && !PyCallable_Check(l_obj_response_error)){
        PyErr_SetString(PyExc_SyntaxError, "Sorry, but the second argument is not a callback function");
        return NULL;
    }
    void *l_request = PyBytes_AsString(l_obj_request);
    size_t l_request_size = (size_t)PyBytes_Size(l_obj_request);
    _wrapping_dap_client_callback_add(_ENC_RESPONSE_PROC,
                                      ((PyDapClientObject*)self)->client, l_obj_response_proc);
    _wrapping_dap_client_callback_add(_ENC_RESPONSE_ERROR,
                                      ((PyDapClientObject*)self)->client, l_obj_response_proc);
    dap_client_request_unsafe(((PyDapClientObject*)self)->client, l_path, l_request, l_request_size, _wrapping_callback_response_proc,
                              _wrapping_callback_response_error);
    return Py_None;
}

//PyObject *dap_client_disconnect_py(PyObject *self, PyObject *args)
//{
//    (void) self;
//    (void) args;
//    /// TODO: Implement it!
//    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
//    return NULL;
//}

PyObject *dap_client_get_stage_str_py(PyObject *self, PyObject *args)
{
    (void)args;
    const char *l_stage_str = dap_client_get_stage_str(((PyDapClientObject*)self)->client);
    return Py_BuildValue("s", l_stage_str);
}

//PyObject *dap_client_stage_str_py(PyObject *self, PyObject *args)
//{
//    (void) self;
//    (void) args;
//    /// TODO: Implement it!
//    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
//    return NULL;
//}

PyObject *dap_client_get_stage_status_str_py(PyObject *self, PyObject *args)
{
    (void)args;
    const char *l_stage_status_str = dap_client_get_stage_status_str(((PyDapClientObject*)self)->client);
    return Py_BuildValue("s", l_stage_status_str);
}

//PyObject *dap_client_stage_status_str_py(PyObject *self, PyObject *args)
//{
//    (void) self;
//    (void) args;
//    /// TODO: Implement it!
//    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
//    return NULL;
//}

//PyObject *dap_client_error_str_py(PyObject *self, PyObject *args)
//{
//    (void) self;
//    (void) args;
//    /// TODO: Implement it!
//    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
//    return NULL;
//}

PyObject *dap_client_get_error_str_py(PyObject *self, PyObject *args)
{
    (void)args;
    const char *l_str_error = dap_client_get_error_str(((PyDapClientObject*)self)->client);
    return Py_BuildValue("s", l_str_error);
}

PyObject *dap_client_get_is_always_reconnect_py(PyObject *self, PyObject *args){
    (void)args;
    bool ret = dap_client_get_is_always_reconnect(((PyDapClientObject*)self)->client);
    return ret ? Py_BuildValue("O", Py_True) : Py_BuildValue("O", Py_False);
}
PyObject *dap_client_set_is_always_reconnect_py(PyObject *self, PyObject *args){
    PyObject *l_obj_bool = NULL;
    if (!PyArg_ParseTuple(args, "O", &l_obj_bool)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyBool_Check(l_obj_bool)){
        PyErr_SetString(PyExc_SyntaxError, "The first argument is incorrected, it must be a boolean value type");
        return NULL;
    }
    bool l_bool = l_obj_bool == Py_True ? true : false;
    dap_client_set_is_always_reconnect(((PyDapClientObject*)self)->client, l_bool);
    return Py_None;
}

PyObject *dap_client_from_esocket_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *l_obj_event_socket = NULL;
    if (!PyArg_ParseTuple(args, "O", &l_obj_event_socket)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyDapEventsSocketObject_Check(l_obj_event_socket)){
        PyErr_SetString(PyExc_SyntaxError, "The first argument is not a DapEventsSocket type");
        return NULL;
    }
    dap_client_t *l_client = dap_client_from_esocket(((PyDapEventsSocketObject*)l_obj_event_socket)->t_events_socket);
    if (l_client){
        PyDapClientObject *l_obj_client = (PyDapClientObject *)_PyObject_New(&dapClientObject_dapClientType);
        l_obj_client->client = l_client;
        return (PyObject *)l_obj_client;
    } else {
        return Py_None;
    }
}

//PyObject *dap_client_get_auth_cookie_py(PyObject *self, PyObject *args)
//{
//    (void)args;
//    const char *l_auth_cookie = dap_client_get_auth_cookie(((PyDapClientObject*)self)->client);
//    return Py_BuildValue("s", l_auth_cookie);
//}

PyObject *dap_client_get_stream_py(PyObject *self, PyObject *args)
{
    (void)args;
    dap_stream_t *l_stream = dap_client_get_stream(((PyDapClientObject*)self)->client);
    PyDapStreamObject *l_obj_stream = (PyDapStreamObject*)_PyObject_New(&DapStream_DapStreamType);
    l_obj_stream->stream = l_stream;
    return (PyObject*)l_obj_stream;
}

PyObject *dap_client_get_stream_ch_unsafe_py(PyObject *self, PyObject *args)
{
    uint8_t l_ch_id;
    if (!PyArg_ParseTuple(args, "I", &l_ch_id)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    dap_stream_ch_t *l_stream_ch = dap_client_get_stream_ch_unsafe(((PyDapClientObject*)self)->client, l_ch_id);
    if (l_stream_ch == NULL){
        return Py_None;
    }else{
        PyDapStreamChObject *l_obj = (PyDapStreamChObject*)_PyObject_New(&dapStreamChObject_dapStreamChType);
        l_obj->stream_ch = l_stream_ch;
        return (PyObject *)l_obj;
    }
}

PyObject *dap_client_get_stream_worker_py(PyObject *self, PyObject *args){
    (void)args;
    dap_stream_worker_t *l_worker = dap_client_get_stream_worker(((PyDapClientObject*)self)->client);
    if (l_worker){
        PyDapStreamWorkerObject *l_obj_worker = (PyDapStreamWorkerObject*)_PyObject_New(&dapStreamWorkerObject_dapStreamWorkerType);
        l_obj_worker->worker = l_worker;
        return (PyObject *)l_obj_worker;
    } else {
        return Py_None;
    }
    return NULL;
}

PyObject *dap_client_get_stream_id_py(PyObject *self, PyObject *args)
{
    (void)args;
    const char *l_stream_id = dap_client_get_stream_id(((PyDapClientObject*)self)->client);
    return Py_BuildValue("s", l_stream_id);
}

PyObject *dap_client_set_active_channels_unsafe_py(PyObject *self, PyObject *args)
{
    const char *l_active_channels = NULL;
    if (!PyArg_ParseTuple(args, "s", &l_active_channels)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    dap_client_set_active_channels_unsafe(((PyDapClientObject*)self)->client, l_active_channels);
    return Py_None;
}

PyObject *dap_client_set_auth_cert_unsafe_py(PyObject *self, PyObject *args){
    PyObject *l_obj_cert = NULL;
    if (!PyArg_ParseTuple(args, "O", &l_obj_cert)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyCryptoCertObject_Check(l_obj_cert)){
        PyErr_SetString(PyExc_SyntaxError, "The first argument is incorrected, it must be a sign value type");
        return NULL;
    }
    dap_client_set_auth_cert_unsafe(((PyDapClientObject*)self)->client, ((PyCryptoCertObject*)l_obj_cert)->cert);
    return Py_None;
}

PyObject *dap_client_get_stage_py(PyObject *self, PyObject *args)
{
    (void)args;
    PyDapClientStageObject* l_obj_stage = (PyDapClientStageObject*)_PyObject_New(&dapClientStageObject_dapClientStageType);
    l_obj_stage->stage = dap_client_get_stage(((PyDapClientObject*)self)->client);
    return (PyObject*)l_obj_stage;
}

PyObject *dap_client_get_stage_status_py(PyObject *self, PyObject *args)
{
    (void)args;
    PyDapClientStageStatusObject *l_obj_stage_status = (PyDapClientStageStatusObject*)_PyObject_New(&dapClientStageStatusObject_dapClientStageStatusType);
    l_obj_stage_status->stage_status = dap_client_get_stage_status(((PyDapClientObject*)self)->client);
    return (PyObject*)l_obj_stage_status;
}
