#include "wrapping_dap_http_simple.h"

#define LOG_TAG "wrapping_dap_http_simple"


typedef struct wrapping_dap_http_simple_proc{
    char *url;
    PyObject *func_call;
    UT_hash_handle hh;
}wrapping_dap_http_simple_proc_t;
static wrapping_dap_http_simple_proc_t *s_simple_proc = NULL;

void _w_simple_proc_add(const char *url, PyObject *func_call){
    wrapping_dap_http_simple_proc_t *l_simple_proc = DAP_NEW(wrapping_dap_http_simple_proc_t);
    l_simple_proc->url = dap_strdup(url);
    l_simple_proc->func_call = func_call;
    Py_XINCREF(l_simple_proc->func_call);
    HASH_ADD_STR(s_simple_proc, url, l_simple_proc);
}
PyObject *_w_simple_proc_find(const char *url){
    wrapping_dap_http_simple_proc_t *l_http_simple_proc = NULL;
    HASH_FIND_STR(s_simple_proc, url, l_http_simple_proc);
    return l_http_simple_proc->func_call;
}

void wrapping_dap_http_simple_callback(dap_http_simple_t *sh, void *obj){
    log_it(L_DEBUG, "Handling C module request");
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    PyObject *obj_func = _w_simple_proc_find(sh->http_client->url_path);
    PyDapHttpSimpleObject *obj_http_simple = PyObject_NEW(PyDapHttpSimpleObject, &DapHTTPSimple_DapHTTPSimpleType);
    PyObject *obj_http_status_code = _PyObject_New(&HTTPCode_HTTPCodeType);
    ((PyDapHttpSimpleObject*)obj_http_simple)->sh = sh;
    PyObject_Dir((PyObject*)obj_http_simple);
    http_status_code_t *ret = (http_status_code_t*)obj;
    ((PyHttpStatusCodeObject*)obj_http_status_code)->http_status = *ret;
    PyObject_Dir((PyObject*)obj_http_status_code);
    PyObject *obj_argv = Py_BuildValue("OO", obj_http_simple, obj_http_status_code);
    PyErr_Print();
    PyObject *result = PyObject_CallObject(obj_func, obj_argv);
    if (!result){
        log_it(L_DEBUG, "Function can't called");
        PyErr_Print();
        *ret = Http_Status_InternalServerError;
    }
    *ret = ((PyHttpStatusCodeObject*)obj_http_status_code)->http_status;
    Py_XDECREF(obj_argv);
    Py_XDECREF(obj_http_status_code);
    Py_XDECREF(obj_http_simple);
    PyGILState_Release(gstate);
}

PyObject *dap_http_simple_add_proc_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_server;
    char *url;
    size_t reply_size_max;
    PyObject *func_callback;
    if (!PyArg_ParseTuple(args, "OsKO", &obj_server, &url, &reply_size_max, &func_callback)){
        return NULL;
    } else {
        if (!PyCallable_Check(func_callback)){
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    if (((PyDapServerObject*)obj_server)->t_server){
        _w_simple_proc_add(url, func_callback);
        dap_http_t *l_http = DAP_HTTP(((PyDapServerObject*)obj_server)->t_server);
        dap_http_simple_proc_add(l_http,
                                 url,
                                 reply_size_max,
                                 wrapping_dap_http_simple_callback);
        log_it(L_NOTICE, "Add proc URL %s, python plugin", url);
        return Py_BuildValue("(O)", Py_None);
    } else {
        PyErr_SetString(PyExc_Exception, "The server core object is NULL.");
        return NULL;
    }
}
PyObject *dap_http_simple_module_init_py(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    int res = dap_http_simple_module_init();
    return PyLong_FromLong(res);
}
PyObject *dap_http_simple_module_deinit_py(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    dap_http_simple_module_deinit();
    return Py_BuildValue("(O)", Py_None);
}
PyObject *dap_http_simple_set_supported_user_agents_py(PyObject *self, PyObject *args){
    (void)self;
//    TODO: Implement
    return NULL;
}
PyObject *dap_http_simple_set_pass_unknown_user_agents_py(PyObject *self, PyObject *args){
    (void)self;
    bool res = false;
    if (!PyArg_ParseTuple(args, "p", &res)){
        return NULL;
    }
    dap_http_simple_set_pass_unknown_user_agents(res);
    return Py_BuildValue("(O)", Py_None);
}
PyObject *dap_http_simple_reply_py(PyObject *self, PyObject *args){
    PyObject *l_obj_bytes;
    if (!PyArg_ParseTuple(args, "O", &l_obj_bytes)){
        return  NULL;
    }
    if (!PyBytes_Check(l_obj_bytes)){
        PyErr_SetString(PyExc_TypeError, "The passed parameter is not byte");
        return NULL;
    }
    size_t l_bytes_size = (size_t)PyBytes_Size(l_obj_bytes);
    void *l_bytes = PyBytes_AsString(l_obj_bytes);
    size_t l_size = dap_http_simple_reply(((PyDapHttpSimpleObject*)self)->sh, l_bytes, l_bytes_size);
//    DAP_FREE(l_bytes);
    return PyLong_FromLong(l_size);
}

PyObject *dap_http_simple_method_py(PyDapHttpSimpleObject *self, void *clouser){
    (void)clouser;
    return Py_BuildValue("s", self->sh->http_client->action);
}

PyObject *dap_http_simple_request_py(PyDapHttpSimpleObject *self, void *clouser){
    (void)clouser;
    if (self->sh->request)
        return PyBytes_FromString(self->sh->request);
    else
        return Py_None;
}

PyObject *dap_http_simple_url_path_py(PyDapHttpSimpleObject *self, void *clouser){
    (void)clouser;
    return Py_BuildValue("s", self->sh->http_client->url_path);
}

PyObject *dap_http_simple_query_py(PyDapHttpSimpleObject *self, void *clouser){
    (void)clouser;
    return Py_BuildValue("s", self->sh->http_client->in_query_string);
}
