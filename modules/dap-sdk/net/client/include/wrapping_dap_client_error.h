#pragma once

#include "dap_client.h"
#include "Python.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapClientError{
    PyObject_HEAD
    dap_client_error_t error;
}PyDapClientErrorObject;

PyObject* GET_ERROR_NO_ERROR(PyObject *self, PyObject *args);
PyObject* GET_ERROR_OUT_OF_MEMORY(PyObject *self, PyObject *args);
PyObject* GET_ERROR_ENC_NO_KEY(PyObject *self, PyObject *args);
PyObject* GET_ERROR_ENC_WRONG_KEY(PyObject *self, PyObject *args);
PyObject* GET_ERROR_ENC_SESSION_CLOSED(PyObject *self, PyObject *args);
PyObject* GET_ERROR_STREAM_CTL_ERROR(PyObject *self, PyObject *args);
PyObject* GET_ERROR_STREAM_CTL_ERROR_AUTH(PyObject *self, PyObject *args);
PyObject* GET_ERROR_STREAM_CTL_ERROR_RESPONSE_FORMAT(PyObject *self, PyObject *args);
PyObject* GET_ERROR_STREAM_CONNECT(PyObject *self, PyObject *args);
PyObject* GET_ERROR_STREAM_RESPONSE_WRONG(PyObject *self, PyObject *args);
PyObject* GET_ERROR_STREAM_RESPONSE_TIMEOUT(PyObject *self, PyObject *args);
PyObject* GET_ERROR_STREAM_FREEZED(PyObject *self, PyObject *args);
PyObject* GET_ERROR_NETWORK_CONNECTION_REFUSE(PyObject *self, PyObject *args);
PyObject* GET_ERROR_NETWORK_CONNECTION_TIMEOUT(PyObject *self, PyObject *args);

static PyMethodDef DapClientErrorMethods[] = {
        {"GET_ERROR_NO_ERROR", (PyCFunction)GET_ERROR_NO_ERROR, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_OUT_OF_MEMORY", (PyCFunction)GET_ERROR_OUT_OF_MEMORY, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_ENC_NO_KEY", (PyCFunction)GET_ERROR_ENC_NO_KEY, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_ENC_WRONG_KEY", (PyCFunction)GET_ERROR_ENC_WRONG_KEY, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_ENC_SESSION_CLOSED", (PyCFunction)GET_ERROR_ENC_SESSION_CLOSED, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_STREAM_CTL_ERROR", (PyCFunction)GET_ERROR_STREAM_CTL_ERROR, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_STREAM_CTL_ERROR_AUTH", (PyCFunction)GET_ERROR_STREAM_CTL_ERROR_AUTH, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_STREAM_CTL_ERROR_RESPONSE_FORMAT", (PyCFunction)GET_ERROR_STREAM_CTL_ERROR_RESPONSE_FORMAT, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_STREAM_CONNECT", (PyCFunction)GET_ERROR_STREAM_CONNECT, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_STREAM_RESPONSE_WRONG", (PyCFunction)GET_ERROR_STREAM_RESPONSE_WRONG, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_STREAM_RESPONSE_TIMEOUT", (PyCFunction)GET_ERROR_STREAM_RESPONSE_TIMEOUT, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_STREAM_FREEZED", (PyCFunction)GET_ERROR_STREAM_FREEZED, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_NETWORK_CONNECTION_REFUSE", (PyCFunction)GET_ERROR_NETWORK_CONNECTION_REFUSE, METH_NOARGS | METH_STATIC, ""},
        {"GET_ERROR_NETWORK_CONNECTION_TIMEOUT", (PyCFunction)GET_ERROR_NETWORK_CONNECTION_TIMEOUT, METH_NOARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyObject *dap_client_error_str_getter(PyDapClientErrorObject *self, void *closure);
PyObject *dap_client_error_int_getter(PyDapClientErrorObject *self, void *closure);

static PyGetSetDef DapClientErrorGetSets[] = {
    {"Name", (getter)dap_client_error_str_getter, NULL, NULL, NULL},
    {"Value", (getter)dap_client_error_int_getter, NULL, NULL, NULL},
    {NULL}
};

static PyTypeObject dapClientErrorObject_dapClientErrorType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ClientError",                                      /* tp_name */
        sizeof(PyDapClientErrorObject),                               /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        0,                                                            /* tp_dealloc */
        0,                                                            /* tp_print */
        0,                                                            /* tp_getattr */
        0,                                                            /* tp_setattr */
        0,                                                            /* tp_reserved */
        0,                                                            /* tp_repr */
        0,                                                            /* tp_as_number */
        0,                                                            /* tp_as_sequence */
        0,                                                            /* tp_as_mapping */
        0,                                                            /* tp_hash  */
        0,                                                            /* tp_call */
        0,                                                            /* tp_str */
        0,                                                            /* tp_getattro */
        0,                                                            /* tp_setattro */
        0,                                                            /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                                      /* tp_flags */
        "Client error objects",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        DapClientErrorMethods,                                              /* tp_methods */
        0,                                                            /* tp_members */
        DapClientErrorGetSets,                                        /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */

};

#ifdef __cplusplus
}
#endif
