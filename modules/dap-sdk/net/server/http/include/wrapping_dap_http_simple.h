#pragma once

#include "dap_common.h"
#include "dap_strfuncs.h"
#include "dap_http_simple.h"
#include "Python.h"
#include "wrapping_http.h"
#include "wrapping_http_status_code.h"
#include "libdap-server-core-python.h"
#include "uthash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapHttpSimple{
    PyObject_HEAD
    dap_http_simple_t *sh;
}PyDapHttpSimpleObject;

PyObject *dap_http_simple_add_proc_py(PyObject *self, PyObject *args);
PyObject *dap_http_simple_module_init_py(PyObject *self, PyObject *args);
PyObject *dap_http_simple_module_deinit_py(PyObject *self, PyObject *args);
PyObject *dap_http_simple_set_supported_user_agents_py(PyObject *self, PyObject *args);
PyObject *dap_http_simple_set_pass_unknown_user_agents_py(PyObject *self, PyObject *args);
PyObject *dap_http_simple_reply_py(PyObject *self, PyObject *args);

static PyMethodDef PyDapHttpSimpleMethods[] = {
    //{"addProc", enc_http_add_proc_py, METH_VARARGS | METH_STATIC, ""},
    {"init", dap_http_simple_module_init_py, METH_NOARGS | METH_STATIC, "Initialization module http simple"},
    {"deinit", dap_http_simple_module_deinit_py,  METH_NOARGS | METH_STATIC, "Deinitialization module http simple"},
    {"addProc", dap_http_simple_add_proc_py, METH_VARARGS | METH_STATIC, "Add HTTP URL"},
    {"setPassUnknownUserAgents", dap_http_simple_set_pass_unknown_user_agents_py, METH_VARARGS | METH_STATIC, ""},
    {"replyAdd", dap_http_simple_reply_py, METH_VARARGS, "Reply for request"},
    {NULL, NULL, 0, NULL}
};

/* Attributes */
PyObject *dap_http_simple_method_py(PyDapHttpSimpleObject *self, void *clouser);
PyObject *dap_http_simple_request_py(PyDapHttpSimpleObject *self, void *clouser);
PyObject *dap_http_simple_url_path_py(PyDapHttpSimpleObject *self, void *clouser);
PyObject *dap_http_simple_query_py(PyDapHttpSimpleObject *self, void *clouser);

static PyGetSetDef PyDapHttpSimpleGetSetDef[] = {
    {"action", (getter)dap_http_simple_method_py, NULL, "Return action request", NULL},
    {"request", (getter)dap_http_simple_request_py, NULL, "Return request in view bytes", NULL},
    {"urlPath", (getter)dap_http_simple_url_path_py, NULL, "Return request in view bytes", NULL},
    {"query", (getter)dap_http_simple_query_py, NULL, "Return request in view bytes", NULL},
    {NULL}
};

/*------------*/

static PyTypeObject DapHTTPSimple_DapHTTPSimpleType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "DAP.HttpSimple",             /* tp_name */
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

#ifdef __cplusplus
}
#endif
