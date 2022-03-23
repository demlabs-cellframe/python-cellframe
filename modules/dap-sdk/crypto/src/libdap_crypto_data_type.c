#include "libdap_crypto_data_type.h"

PyObject *get_ENC_DATA_TYPE_RAW(){
    return PyLong_FromLong(DAP_ENC_DATA_TYPE_RAW);
}
PyObject *get_ENC_DATA_TYPE_B64(){
    return PyLong_FromLong(DAP_ENC_DATA_TYPE_B64);
}
PyObject *get_ENC_DATA_TYPE_B64_URLSAFE(){
    return PyLong_FromLong(DAP_ENC_DATA_TYPE_B64_URLSAFE);
}

PyMethodDef PyCryptoDataTypeObjectMethods[] = {
        {"DAP_ENC_DATA_TYPE_RAW", (PyCFunction)get_ENC_DATA_TYPE_RAW, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_DATA_TYPE_B64", (PyCFunction)get_ENC_DATA_TYPE_B64, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_DATA_TYPE_B64_URLSAFE", (PyCFunction)get_ENC_DATA_TYPE_B64_URLSAFE, METH_NOARGS | METH_STATIC, ""},

        {NULL, NULL, 0, NULL}
};

PyTypeObject DapCryptoDataTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.Crypto.DataType",      /* tp_name */
        sizeof(PyCryptoDataTypeObjecy),  /* tp_basicsize */
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
        "Crypto data type objects",      /* tp_doc */
        0,		                     /* tp_traverse */
        0,		                     /* tp_clear */
        0,		                     /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                     /* tp_iter */
        0,		                     /* tp_iternext */
        PyCryptoDataTypeObjectMethods,   /* tp_methods */
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
