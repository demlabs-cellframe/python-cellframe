#ifndef WRAPPING_CERT_METADATA_H
#define WRAPPING_CERT_METADATA_H
#define PY_SSIZE_T_CLEAN

#include "Python.h"
#include "dap_cert.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapCertMetadataType{
    PyObject_HEAD
    dap_cert_metadata_type_t type;
}PyDapCertMetadataTypeObject;

PyObject *GET_DAP_CERT_META_STRING(PyObject *self, PyObject *args);
PyObject *GET_DAP_CERT_META_BOOL(PyObject *self, PyObject *args);
PyObject *GET_DAP_CERT_META_INT(PyObject *self, PyObject *args);
PyObject *GET_DAP_CERT_META_DATETIME(PyObject *self, PyObject *args);
PyObject *GET_DAP_CERT_META_DATETIME_PERIOD(PyObject *self, PyObject *args);
PyObject *GET_DAP_CERT_META_SIGN(PyObject *self, PyObject *args);
PyObject *GET_DAP_CERT_META_CUSTOM(PyObject *self, PyObject *args);
PyObject *getName(PyObject *self, PyObject *args);
PyObject *getValue(PyObject *self, PyObject *args);

static PyMethodDef g_crypto_cert_metadata_type_methods_py[] = {
        {"GET_DAP_CERT_META_STRING", (PyCFunction)GET_DAP_CERT_META_STRING, METH_NOARGS | METH_STATIC, ""},
        {"GET_DAP_CERT_META_BOOL", (PyCFunction)GET_DAP_CERT_META_BOOL, METH_NOARGS | METH_STATIC, ""},
        {"GET_DAP_CERT_META_INT", (PyCFunction)GET_DAP_CERT_META_INT, METH_NOARGS | METH_STATIC, ""},
        {"GET_DAP_CERT_META_DATETIME", (PyCFunction)GET_DAP_CERT_META_DATETIME, METH_NOARGS | METH_STATIC, ""},
        {"GET_DAP_CERT_META_DATETIME_PERIOD", (PyCFunction)GET_DAP_CERT_META_DATETIME_PERIOD, METH_NOARGS | METH_STATIC, ""},
        {"GET_DAP_CERT_META_SIGN", (PyCFunction)GET_DAP_CERT_META_SIGN, METH_NOARGS | METH_STATIC, ""},
        {"GET_DAP_CERT_META_CUSTOM", (PyCFunction)GET_DAP_CERT_META_CUSTOM, METH_NOARGS | METH_STATIC, ""},
        {"Name", (PyCFunction)getName, METH_NOARGS | METH_STATIC, ""},
        {"Value", (PyCFunction)getValue, METH_NOARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject g_crypto_cert_type_metadata_type_py = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "DAP.CertMetadataType",             /* tp_name */
    sizeof(PyDapCertMetadataTypeObject),         /* tp_basicsize */
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
    "Crypto cert metadata type object",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    g_crypto_cert_metadata_type_methods_py,             /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,           /* tp_new */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

typedef struct PyDapCertMetadata{
    PyObject_HEAD
    dap_cert_metadata_t *metadata;
} PyDapCertMetadataObject;

PyObject *dap_cert_new_meta_py(PyObject *self, PyObject *args);
PyObject *dap_cert_add_meta_py(PyObject *self, PyObject *args);
PyObject *dap_cert_add_meta_scalar_py(PyObject *self, PyObject *args);

PyObject *dap_cert_add_meta_string_py(PyObject *self, PyObject *args);
PyObject *dap_cert_add_meta_sign_py(PyObject *self, PyObject *args);
PyObject *dap_cert_add_meta_custom_py(PyObject *self, PyObject *args);
PyObject *dap_cert_add_meta_bool_py(PyObject *self, PyObject *args);
PyObject *dap_cert_add_meta_int_py(PyObject *self, PyObject *args);
PyObject *dap_cert_add_meta_time_py(PyObject *self, PyObject *args);
PyObject *dap_cert_add_meta_period_py(PyObject *self, PyObject *args);

PyObject *dap_cert_get_meta_py(PyObject *self, PyObject *args);
PyObject *dap_cert_get_meta_string_py(PyObject *self, PyObject *args);
PyObject *dap_cert_get_meta_bool_py(PyObject *self, PyObject *args);
PyObject *dap_cert_get_meta_int_py(PyObject *self, PyObject *args);
PyObject *dap_cert_get_meta_time_py(PyObject *self, PyObject *args);
PyObject *dap_cert_get_meta_period_py(PyObject *self, PyObject *args);
PyObject *dap_cert_get_meta_sign_py(PyObject *self, PyObject *args);
PyObject *dap_cert_get_meta_custom_py(PyObject *self, PyObject *args);

static PyMethodDef g_crypto_cert_metadata_methods_py[] = {
        {"new", dap_cert_new_meta_py, METH_VARARGS | METH_STATIC, ""},
        {"add", dap_cert_add_meta_py, METH_VARARGS | METH_STATIC, ""},
        {"addScalar", dap_cert_add_meta_scalar_py, METH_VARARGS | METH_STATIC, ""},
        {"addString", dap_cert_add_meta_string_py, METH_VARARGS | METH_STATIC, ""},
        {"addSign", dap_cert_add_meta_sign_py, METH_VARARGS | METH_STATIC, ""},
        {"addCustom", dap_cert_add_meta_custom_py, METH_VARARGS | METH_STATIC, ""},
        {"addBool", dap_cert_add_meta_bool_py, METH_VARARGS | METH_STATIC, ""},
        {"addInt", dap_cert_add_meta_int_py, METH_VARARGS | METH_STATIC, ""},
        {"addTime", dap_cert_add_meta_time_py, METH_VARARGS | METH_STATIC, ""},
        {"addPeriod", dap_cert_add_meta_period_py, METH_VARARGS | METH_STATIC, ""},
        {"get", dap_cert_get_meta_py, METH_VARARGS | METH_STATIC, ""},
        {"getString", dap_cert_get_meta_string_py, METH_VARARGS | METH_STATIC, ""},
        {"getBool", dap_cert_get_meta_bool_py, METH_VARARGS | METH_STATIC, ""},
        {"getInt", dap_cert_get_meta_int_py, METH_VARARGS | METH_STATIC, ""},
        {"getTime", dap_cert_get_meta_time_py, METH_VARARGS | METH_STATIC, ""},
        {"getPeriod", dap_cert_get_meta_period_py, METH_VARARGS | METH_STATIC, ""},
        {"getSign", dap_cert_get_meta_sign_py, METH_VARARGS | METH_STATIC, ""},
        {"getCustom", dap_cert_get_meta_custom_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject g_crypto_cert_metadata_type_py = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "DAP.CertMetadata",             /* tp_name */
    sizeof(PyDapCertMetadataObject),         /* tp_basicsize */
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
    "Crypto cert metadata object",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    g_crypto_cert_metadata_methods_py,             /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,           /* tp_new */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

#ifdef __cplusplus
}
#endif

#endif // WRAPPING_CERT_METADATA_H
