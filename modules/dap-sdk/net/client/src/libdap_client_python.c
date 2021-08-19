#include "libdap_client_python.h"

int dap_client_init_py(){
    return dap_client_init();
}
void dap_client_deinit_py(){
    dap_client_deinit();
}

void _wrapping_callback_stage_status(dap_client_t *a_client, void *a_data){
    struct dap_client_call_hash *l_call;
    HASH_FIND_PTR(g_client_hash_table, a_client, l_call);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_obj_func = l_call->call_func_stage_status;
    PyObject *l_obj_args = NULL;
    if (a_data){
        l_obj_args = Py_BuildValue("OO", l_obj_client, a_data);
    } else {
        l_obj_args = Py_BuildValue("O", l_obj_client);
    }
    PyEval_CallObject(l_obj_func, l_obj_args);
}
void _wrapping_callback_stage_status_error(dap_client_t *a_client, void *a_data){
    struct dap_client_call_hash *l_call;
    HASH_FIND_PTR(g_client_hash_table, a_client, l_call);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_obj_func = l_call->call_func_stage_status_error;
    PyObject *l_obj_args = NULL;
    if (a_data){
        l_obj_args = Py_BuildValue("OO", l_obj_client, a_data);
    } else {
        l_obj_args = Py_BuildValue("O", l_obj_client);
    }
    PyEval_CallObject(l_obj_func, l_obj_args);
}

void _wrapping_callback_stage_go_end(dap_client_t *a_client, void *a_data){
    struct dap_client_call_hash *l_call;
    HASH_FIND_PTR(g_client_hash_table_go_stage_end, a_client, l_call);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_obj_func = l_call->call_func_stage_status_error;
    PyObject *l_obj_args = NULL;
    if (a_data){
        l_obj_args = Py_BuildValue("OO", l_obj_client, a_data);
    } else {
        l_obj_args = Py_BuildValue("O", l_obj_client);
    }
    PyEval_CallObject(l_obj_func, l_obj_args);
}

void _wrapping_callback_response_proc(dap_client_t *a_client, void *a_data, size_t a_data_size){
    struct dap_client_call_hash *l_call;
    HASH_FIND_PTR(g_client_hash_table_go_stage_end, a_client, l_call);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_obj_bytes = PyBytes_FromStringAndSize(a_data, a_data_size);
    PyObject *l_args = Py_BuildValue("OO", l_obj_client, l_obj_bytes);
    PyEval_CallObject(l_call->call_func_stage_status, l_args);
    HASH_DEL(g_client_hash_table_response, l_call);
    DAP_FREE(l_call);
}
void _wrapping_callback_response_error(dap_client_t *a_client, int a_integer){
    struct dap_client_call_hash *l_call;
    HASH_FIND_PTR(g_client_hash_table_go_stage_end, a_client, l_call);
    PyObject *l_obj_client = _PyObject_New(&dapClientObject_dapClientType);
    ((PyDapClientObject*)l_obj_client)->client = a_client;
    PyObject *l_args = Py_BuildValue("Oi", l_obj_client, a_integer);
    PyEval_CallObject(l_call->call_func_stage_status, l_args);
    HASH_DEL(g_client_hash_table_response, l_call);
    DAP_FREE(l_call);
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
    struct dap_client_call_hash *l_call = DAP_NEW(struct dap_client_call_hash);
    l_call->client = l_client;
    l_call->call_func_stage_status = obj_func_stage;
    l_call->call_func_stage_status_error = obj_func_stage_error;
    HASH_ADD_PTR(g_client_hash_table, client, l_call);
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
    dap_client_go_stage(((PyDapClientObject*)self)->client, ((PyDapClientStageObject*)l_obj_stage)->stage, _wrapping_callback_stage_go_end);
    struct dap_client_call_hash *l_call = DAP_NEW(struct dap_client_call_hash);
    l_call->client = ((PyDapClientObject*)self)->client;
    l_call->call_func_stage_status = l_obj_stage_callback;
    l_call->call_func_stage_status_error = NULL;
    HASH_ADD_PTR(g_client_hash_table_go_stage_end, client, l_call);
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
    struct dap_client_call_responce *l_call = DAP_NEW(struct dap_client_call_responce);
    l_call->client = ((PyDapClientObject*)self)->client;
    l_call->call_func_response_proc = l_obj_response_proc;
    l_call->call_func_response_error = l_obj_response_error;
    HASH_ADD_PTR(g_client_hash_table_response, client, l_call);
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
