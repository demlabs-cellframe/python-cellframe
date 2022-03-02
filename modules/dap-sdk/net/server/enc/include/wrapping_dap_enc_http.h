#pragma once

#include "dap_common.h"
#include "dap_enc_http.h"
#include "dap_server_python.h"
#include "dap_http.h"
#include "dap_http_simple.h"
#include "wrapping_dap_http_simple.h"
#include "Python.h"

typedef struct PyDapEncServer{
    PyObject_HEAD
    enc_http_delegate_t *enc_http_delegate;
}PyDapEncServerObject;

PyObject *enc_http_reply_py(PyObject *self, PyObject *args);
PyObject *enc_http_request_decode_py(PyObject *self, PyObject *args);
PyObject *enc_http_is_null_py(PyObject *self, PyObject *args);
PyObject *enc_http_reply_encode_py(PyObject *self, PyObject *args);
void enc_http_delegate_delete_py(PyObject *self);
PyObject *enc_http_add_proc_py(PyObject *self, PyObject *args);

static PyMethodDef PyDapEncServerMethods[] = {
    {"requestDecode", enc_http_request_decode_py, METH_VARARGS | METH_STATIC, ""},
    {"isNull", enc_http_is_null_py, METH_NOARGS, ""},
    {"reply", enc_http_reply_py, METH_VARARGS, ""},
    {"replyEncode", enc_http_reply_encode_py, METH_VARARGS, ""},
    {"addProc", enc_http_add_proc_py, METH_VARARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

PyObject *enc_http_get_action_py(PyDapEncServerObject *self, void *clouser);
PyObject *enc_http_get_request_py(PyDapEncServerObject *self, void *clouser);
PyObject *enc_http_get_url_path_py(PyDapEncServerObject *self, void *clouser);
PyObject *enc_http_get_in_query_py(PyDapEncServerObject *self, void *clouser);

static PyGetSetDef PyDapEncServerGetSet[] = {
    {"action", (getter)enc_http_get_action_py, NULL, "Get an action request", NULL},
    {"request", (getter)enc_http_get_request_py, NULL, "Get a request", NULL},
    {"urlPath", (getter)enc_http_get_url_path_py, NULL, "Get an URL string", NULL},
    {"query", (getter)enc_http_get_in_query_py, NULL, "Get a query string", NULL},
    {NULL}
};

static PyTypeObject DapEncServer_DapEncServerType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.EncHttp",             /* tp_name */
    sizeof(PyDapEncServerObject),                   /* tp_basicsize */
    0,                                                 /* tp_itemsize */
    (destructor)enc_http_delegate_delete_py,           /* tp_dealloc */
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
    "Encrypting server object",                         /* tp_doc */
    0,		                                       /* tp_traverse */
    0,                        		               /* tp_clear */
    0,		                                       /* tp_richcompare */
    0,                        		               /* tp_weaklistoffset */
    0,		                                       /* tp_iter */
    0,                        		               /* tp_iternext */
    PyDapEncServerMethods,                   /* tp_methods */
    0,                                                 /* tp_members */
    PyDapEncServerGetSet,                              /* tp_getset */
    0,                                                 /* tp_base */
    0,                                                 /* tp_dict */
    0,                                                 /* tp_descr_get */
    0,                                                 /* tp_descr_set */
    0,                                                 /* tp_dictoffset */
    0,                                                 /* tp_init */
    0,                                                 /* tp_alloc */
    PyType_GenericNew,                                 /* tp_new */
};
