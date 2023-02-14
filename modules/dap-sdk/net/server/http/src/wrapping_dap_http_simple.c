#include "wrapping_dap_http_simple.h"
#include "python-cellframe_common.h"

#define LOG_TAG "wrapping_dap_http_simple"

PyMethodDef PyDapHttpSimpleMethods[] = {
        //{"addProc", enc_http_add_proc_py, METH_VARARGS | METH_STATIC, ""},
        {"init", dap_http_simple_module_init_py, METH_NOARGS | METH_STATIC, "Initialization module http simple"},
        {"deinit", dap_http_simple_module_deinit_py,  METH_NOARGS | METH_STATIC, "Deinitialization module http simple"},
        {"addProc", dap_http_simple_add_proc_py, METH_VARARGS | METH_STATIC, "Add HTTP URL"},
        {"setPassUnknownUserAgents", dap_http_simple_set_pass_unknown_user_agents_py, METH_VARARGS | METH_STATIC, ""},
        {"replyAdd", dap_http_simple_reply_py, METH_VARARGS, "Reply for request"},
        {NULL, NULL, 0, NULL}
};

PyGetSetDef PyDapHttpSimpleGetSetDef[] = {
        {"action", (getter)dap_http_simple_method_py, NULL, "Return action request", NULL},
        {"request", (getter)dap_http_simple_request_py, NULL, "Return request in view bytes", NULL},
        {"urlPath", (getter)dap_http_simple_url_path_py, NULL, "Return request in view bytes", NULL},
        {"query", (getter)dap_http_simple_query_py, NULL, "Return request in view bytes", NULL},
        {"ipClient", (getter)dap_http_simple_ip_client_py, NULL, ""},
        {NULL}
};

PyTypeObject DapHttpSimpleObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.Met.HttpSimple",             /* tp_name */
        sizeof(PyDapHttpSimpleObject),                   /* tp_basicsize */
        0,                                                 /* tp_itemsize */
        0,                                                 /* tp_dealloc */
        0,                                                 /* tp_print */
        0,                                                 /* tp_getattr */
        0,                                                 /* tp_setattr */
        0,                                                 /* tp_reserved */
        0,                                                 /* tp_repr */
        0,                                                 /* tp_as_number */
        0,                                                 /* tp_as_sequence */
        0,                                                 /* tp_as_mapping */
        0,                                                 /* tp_hash  */
        0,                                                 /* tp_call */
        0,                                                 /* tp_str */
        0,                                                 /* tp_getattro */
        0,                                                 /* tp_setattro */
        0,                                                 /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                           /* tp_flags */
        "Dap http simple object",                         /* tp_doc */
        0,		                                       /* tp_traverse */
        0,                        		               /* tp_clear */
        0,		                                       /* tp_richcompare */
        0,                        		               /* tp_weaklistoffset */
        0,		                                       /* tp_iter */
        0,                        		               /* tp_iternext */
        PyDapHttpSimpleMethods,                   /* tp_methods */
        0,                                                 /* tp_members */
        PyDapHttpSimpleGetSetDef,                          /* tp_getset */
        0,                                                 /* tp_base */
        0,                                                 /* tp_dict */
        0,                                                 /* tp_descr_get */
        0,                                                 /* tp_descr_set */
        0,                                                 /* tp_dictoffset */
        0,                                                 /* tp_init */
        0,                                                 /* tp_alloc */
        PyType_GenericNew,                                 /* tp_new */
};


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
    log_it(L_DEBUG, "Handling C module request ...");
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    PyObject *obj_func = _w_simple_proc_find(sh->http_client->proc->url);
    PyDapHttpSimpleObject *obj_http_simple = PyObject_NEW(PyDapHttpSimpleObject, &DapHttpSimpleObjectType);
    PyObject *obj_http_status_code = _PyObject_New(&DapHttpCodeObjectType);
    ((PyDapHttpSimpleObject*)obj_http_simple)->sh = sh;
    http_status_code_t *ret = (http_status_code_t*)obj;
    ((PyHttpStatusCodeObject*)obj_http_status_code)->http_status = *ret;
    PyObject *obj_argv = Py_BuildValue("OO", obj_http_simple, obj_http_status_code);
//    python_error_in_log_it(LOG_TAG);
    PyObject *result = PyObject_CallObject(obj_func, obj_argv);
    if (!result){
        log_it(L_DEBUG, "Function can't be called");
        python_error_in_log_it(LOG_TAG);
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
            PyErr_SetString(PyExc_TypeError, "Fourth argument must be a callablee");
            return NULL;
        }
    }
    _w_simple_proc_add(url, func_callback);
    dap_server_t *l_server = ((PyDapServerObject*) obj_server)->t_server;
    if (l_server) {
        dap_http_t *l_http = DAP_HTTP(l_server);
        dap_http_simple_proc_add(l_http,
                                 url,
                                 reply_size_max,
                                 wrapping_dap_http_simple_callback);
        log_it(L_NOTICE, "Add processor for \"%s\"", url);
    } else {
        PyErr_SetString(PyExc_SystemError, "It is not possible to add a handler to a non-existent server. Check the configuration.");
        return NULL;
    }
//    } else {
//        log_it(L_ERROR, "");
//    }
    Py_RETURN_NONE;
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
    Py_RETURN_NONE;
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
    Py_RETURN_NONE;
}
PyObject *dap_http_simple_reply_py(PyObject *self, PyObject *args){
    PyObject *l_obj_bytes;
    if (!PyArg_ParseTuple(args, "O", &l_obj_bytes)){
        return  NULL;
    }
    if (!PyBytes_Check(l_obj_bytes)){
        PyErr_SetString(PyExc_TypeError, "Function takes exactly bytes");
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
        Py_RETURN_NONE;
}

PyObject *dap_http_simple_url_path_py(PyDapHttpSimpleObject *self, void *clouser){
    (void)clouser;
    return Py_BuildValue("s", self->sh->http_client->url_path);
}

PyObject *dap_http_simple_query_py(PyDapHttpSimpleObject *self, void *clouser){
    (void)clouser;
    return Py_BuildValue("s", self->sh->http_client->in_query_string);
}

PyObject *dap_http_simple_ip_client_py(PyDapHttpSimpleObject *self, void *clouser){
    (void)clouser;
    return Py_BuildValue("s", self->sh->esocket->hostaddr);
}
