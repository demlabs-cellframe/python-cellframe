#pragma once
#include "Python.h"
#include "dap_json_rpc_response.h"

typedef struct PyDapJSONRPCResponse{
    PyObject_HEAD
    dap_json_rpc_response_t *response;
}PyDapJSONRPCResponseObject;

int wrapping_json_rpc_response_set_result(PyObject *self, PyObject *args, void *closure);
PyObject *wrapping_json_rpc_response_get_result(PyObject *self, void *closure);
PyObject *wrapping_json_rpc_response_get_error(PyObject *self, void *closure);
int wrapping_json_rpc_response_set_error(PyObject *self, PyObject *args, void *closure);
PyObject *wrapping_json_rpc_response_get_id(PyObject *self, void *closure);

static PyMethodDef PyDapJSONRPCResponseMethods[] = {
        {NULL, NULL, 0, NULL}
};

static PyGetSetDef PyDapJSONRPCResponseGetsSets[] = {
    {"ID", (getter)wrapping_json_rpc_response_get_id, NULL, "ID request", NULL},
    {"Error", (getter)wrapping_json_rpc_response_get_error, (setter)wrapping_json_rpc_response_set_error, "", NULL },
    {"Result", (getter)wrapping_json_rpc_response_get_result, (setter)wrapping_json_rpc_response_set_result, "", NULL},
    {NULL, NULL, NULL, NULL, NULL}
};

static PyTypeObject DapJSONRPCResponse_DapJSONRPCResponseType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.JSONRPCResponse",             /* tp_name */
        sizeof(PyDapJSONRPCResponseObject),                   /* tp_basicsize */
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
        "JSONRPC response object",                         /* tp_doc */
        0,                                               /* tp_traverse */
        0,                                               /* tp_clear */
        0,                                               /* tp_richcompare */
        0,                                               /* tp_weaklistoffset */
        0,                                               /* tp_iter */
        0,                                               /* tp_iternext */
        PyDapJSONRPCResponseMethods,                   /* tp_methods */
        0,                                                 /* tp_members */
        PyDapJSONRPCResponseGetsSets,                          /* tp_getset */
        0,                                                 /* tp_base */
        0,                                                 /* tp_dict */
        0,                                                 /* tp_descr_get */
        0,                                                 /* tp_descr_set */
        0,                                                 /* tp_dictoffset */
        0,                                                 /* tp_init */
        0,                                                 /* tp_alloc */
        PyType_GenericNew,                                 /* tp_new */
};
