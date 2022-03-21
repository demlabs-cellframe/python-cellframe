#pragma once

#include "dap_common.h"
#include "dap_server.h"
#include "dap_server_python.h"
#include "wrapping_http.h"
#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dap_plugins_python_app_context{
    dap_server_t *server;
}dap_plugins_python_app_context_t;

int dap_plugins_python_app_content_init(dap_server_t *a_server);

typedef  struct PyDapAppContext{
    PyObject_HEAD
}PyDapAppContextObject;

PyObject *dap_plugins_python_app_context_get_server(PyObject *self, PyObject *args);
PyObject *dap_plugins_python_app_context_get_http(PyObject *self, PyObject *args);

static PyMethodDef DapAppContextMethods[] = {
    {"getServer", (PyCFunction)dap_plugins_python_app_context_get_server, METH_VARARGS | METH_STATIC, "Get main server from node"},
    {"getHttp", (PyCFunction)dap_plugins_python_app_context_get_http, METH_NOARGS | METH_STATIC, "Get main server from node"},
    {NULL, NULL, 0, NULL}
};


static PyTypeObject DapAppContextObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.AppContext",             /* tp_name */
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

#ifdef __cplusplus
}
#endif
