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

PyObject *dao_client_set_uplink_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_uplink_addr_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_uplink_port_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_key_stream_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_go_stage_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_reset_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_request_enc_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_request_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_disconnect_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_stage_str_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_stage_str_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_stage_status_str_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_stage_status_str_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_error_str_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_error_str_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_auth_cookie_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_stream_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_stream_ch_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_stream_id_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_set_active_channels_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_stage_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_client_get_stage_status_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
