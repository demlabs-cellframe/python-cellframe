#pragma once

#include <Python.h>
#include "dap_common.h"
#include "dap_cert.h"



typedef struct PyCryptoCert{
    PyObject_HEAD;
    dap_cert_t * cert;
}PyCryptoCertObject;

int dap_cert_init_py(void);
void dap_cert_deinit_py(void);
PyObject* dap_cert_add_folder_py(PyObject *self, PyObject *args);

static PyMethodDef CryptoCertMethods[] = {
        {"addFolder", dap_cert_add_folder_py, METH_VARARGS | METH_STATIC, "Add folders with .dcert files in it"},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject dapCrypto_dapCryptoCertType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.CryptoCert",             /* tp_name */
    sizeof(PyCryptoCertObject),         /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,                         /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Crypto objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    CryptoCertMethods,             /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,         /* tp_new */
};

