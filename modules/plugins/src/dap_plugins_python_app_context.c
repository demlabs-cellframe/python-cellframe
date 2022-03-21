#include "dap_plugins_python_app_context.h"

dap_plugins_python_app_context_t *s_app_context = NULL;

int dap_plugins_python_app_content_init(dap_server_t *a_server){
    s_app_context = DAP_NEW(dap_plugins_python_app_context_t);
    s_app_context->server = a_server;
    return 0;
}

PyObject *dap_plugins_python_app_context_get_server(PyObject *self, PyObject *args){
    (void)self;
    PyObject *l_obj_serverCore;
    if (!PyArg_ParseTuple(args, "O", &l_obj_serverCore)){
        return NULL;
    }
    ((PyDapServerObject*)l_obj_serverCore)->t_server = s_app_context->server;
    return Py_BuildValue("(O)", Py_None);
}
PyObject *dap_plugins_python_app_context_get_http(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapHttpObject *l_obj_http = PyObject_NEW(PyDapHttpObject, &DapHttpObjectType);
    l_obj_http->http = DAP_HTTP(s_app_context->server);
    return Py_BuildValue("(O)", l_obj_http);
}
