#pragma once
#include <Python.h>
#include "wrapping_dap_http_simple.h"
#include "uthash.h"
#include "dap_common.h"
#include "dap_strfuncs.h"
#include "dap_json_rpc_request_handler.h"
#include "dap_json_rpc_params.h"
#include "wrapping_dap_json_rpc_response.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDAPJsonRPCRequest {
    PyObject_HEAD
    dap_json_rpc_request_t *request;
} PyDAPJsonRPCRequestObject;

struct _w_json_rpc_handler{
    char *method;
    PyObject *call_func;
    UT_hash_table *hh;
};

PyObject* dap_json_roc_request_send_py(PyObject *self, PyObject *args);
PyObject* dap_json_rpc_request_reg_handler_py(PyObject *self, PyObject *args);

static PyMethodDef PyDapJSONRPCRequestMethods[] = {
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapJSONRPCRequest_DapJSONRPCRequestType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.JSONRPC.Request",             /* tp_name */
        sizeof(PyDAPJsonRPCRequestObject),                   /* tp_basicsize */
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
        "Dap JSON RPC request object",                         /* tp_doc */
        0,                                               /* tp_traverse */
        0,                                               /* tp_clear */
        0,                                               /* tp_richcompare */
        0,                                               /* tp_weaklistoffset */
        0,                                               /* tp_iter */
        0,                                               /* tp_iternext */
        PyDapJSONRPCRequestMethods,                   /* tp_methods */
        0,                                                 /* tp_members */
        0,                          /* tp_getset */
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