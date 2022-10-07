#include "dap_plugins_python_app_context.h"

dap_plugins_python_app_context_t *s_app_context = NULL;

static PyMethodDef DapAppContextMethods[] = {
    {"getServer", (PyCFunction)dap_plugins_python_app_context_get_server, METH_VARARGS | METH_STATIC, "Get main server from node"},
    {"getHttp", (PyCFunction)dap_plugins_python_app_context_get_http, METH_NOARGS | METH_STATIC, "Get main server from node"},
    {NULL, NULL, 0, NULL}
};


PyTypeObject DapAppContextObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "DAP.AppContext",             /* tp_name */
    sizeof(PyDapAppContextObject),                   /* tp_basicsize */
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
    "Dap App Context object",                         /* tp_doc */
    0,		                                       /* tp_traverse */
    0,                        		               /* tp_clear */
    0,		                                       /* tp_richcompare */
    0,                        		               /* tp_weaklistoffset */
    0,		                                       /* tp_iter */
    0,                        		               /* tp_iternext */
    DapAppContextMethods,                   /* tp_methods */
    0,                                                 /* tp_members */
    0,                                                 /* tp_getset */
    0,                                                 /* tp_base */
    0,                                                 /* tp_dict */
    0,                                                 /* tp_descr_get */
    0,                                                 /* tp_descr_set */
    0,                                                 /* tp_dictoffset */
    0,                                                 /* tp_init */
    0,                                                 /* tp_alloc */
    PyType_GenericNew,                                 /* tp_new */
};

int dap_plugins_python_app_content_init(dap_server_t *a_server){
    s_app_context = DAP_NEW(dap_plugins_python_app_context_t);
    s_app_context->server = a_server;
    return 0;
}

PyObject *dap_plugins_python_app_context_get_server(PyObject *self, PyObject *args){
    (void)self;
    PyObject *l_obj_server;
    if (!PyArg_ParseTuple(args, "O", &l_obj_server)){
        return NULL;
    }
    ((PyDapServerObject*)l_obj_server)->t_server = s_app_context->server;
    Py_RETURN_NONE;
}
PyObject *dap_plugins_python_app_context_get_http(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapHttpObject *l_obj_http = PyObject_NEW(PyDapHttpObject, &DapHttpObjectType);
    l_obj_http->http = DAP_HTTP(s_app_context->server);
    return Py_BuildValue("O", l_obj_http);
}
