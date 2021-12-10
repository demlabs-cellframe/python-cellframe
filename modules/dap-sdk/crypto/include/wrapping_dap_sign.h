#ifndef _WRAPPING_DAP_SIGN_
#define _WRAPPING_DAP_SIGN_

#include <Python.h>
#include "dap_sign.h"
#include "wrapping_dap_pkey.h"

typedef struct PyDapSignType{
    PyObject_HEAD
    dap_sign_type_t *sign_type;
}PyDapSignTypeObject;

PyObject *PyDapSignType_to_str(PyObject *self);

static PyTypeObject DapSignTypeObject_DapSignTypeObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.SignType",       /* tp_name */
    sizeof(PyDapSignTypeObject),/* tp_basicsize */
    0,                               /* tp_itemsize */
    0,                               /* tp_dealloc */
    0,                               /* tp_print */
    0,                               /* tp_getattr */
    0,                               /* tp_setattr */
    0,                               /* tp_reserved */
    0,                               /* tp_repr */
    0,                               /* tp_as_number */
    0,                               /* tp_as_sequence */
    0,                               /* tp_as_mapping */
    0,                               /* tp_hash  */
    0,                               /* tp_call */
    PyDapSignType_to_str,                               /* tp_str */
    0,                               /* tp_getattro */
    0,                               /* tp_setattro */
    0,                               /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
    "Sign type object",        /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    0,                               /* tp_methods */
    0,                               /* tp_members */
    0,                               /* tp_getset */
    0,                               /* tp_base */
    0,                               /* tp_dict */
    0,                               /* tp_descr_get */
    0,                               /* tp_descr_set */
    0,                               /* tp_dictoffset */
    0,                               /* tp_init */
    0,                               /* tp_alloc */
    PyType_GenericNew,               /* tp_new */
};

typedef struct PyDapSign{
    PyObject_HEAD
    dap_sign_t *sign;
}PyDapSignObject;

PyObject *wrapping_dap_sign_get_type(PyObject *self, void *closure);
PyObject *wrapping_dap_sign_get_pkey(PyObject *self, void *closure);
PyObject *wrapping_dap_sign_get_size(PyObject *self, void *closure);

static PyGetSetDef DapSignObjectGetsSetsDef[] = {
        {"type", (getter)wrapping_dap_sign_get_type, NULL, NULL, NULL},
        {"pkey", (getter)wrapping_dap_sign_get_pkey, NULL, NULL, NULL},
        {"size", (getter)wrapping_dap_sign_get_size, NULL, NULL, NULL},
        {NULL}
};

static PyTypeObject DapSignObject_DapSignObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.Sign",       /* tp_name */
        sizeof(PyDapSignObject),/* tp_basicsize */
        0,                               /* tp_itemsize */
        0,                               /* tp_dealloc */
        0,                               /* tp_print */
        0,                               /* tp_getattr */
        0,                               /* tp_setattr */
        0,                               /* tp_reserved */
        0,                               /* tp_repr */
        0,                               /* tp_as_number */
        0,                               /* tp_as_sequence */
        0,                               /* tp_as_mapping */
        0,                               /* tp_hash  */
        0,                               /* tp_call */
        0,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Sign object",        /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        0,                               /* tp_methods */
        0,                               /* tp_members */
        DapSignObjectGetsSetsDef,                               /* tp_getset */
        0,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        0,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

static bool PyDapSignObject_Check(PyObject *self){
    return PyObject_TypeCheck(self, &DapSignObject_DapSignObjectType);
}


#endif // _WRAPPING_DAP_SIGN_
