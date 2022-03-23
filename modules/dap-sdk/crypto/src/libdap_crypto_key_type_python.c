#include "libdap_crypto_key_type_python.h"

PyMethodDef PyCryptoKeyTypeObjectMethods[] = {
        {"DAP_ENC_KEY_TYPE_IAES", (PyCFunction)get_ENC_KEY_TYPE_IAES, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_OAES", (PyCFunction)get_ENC_KEY_TYPE_OAES, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_RLWE_NEWHOPE", (PyCFunction)get_ENC_KEY_TYPE_RLWE_NEWHOPE, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIDH_CLN16", (PyCFunction)get_ENC_KEY_TYPE_SIDH_CLN16, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_DEFEO", (PyCFunction)get_ENC_KEY_TYPE_DEFEO, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_MSRLN", (PyCFunction)get_ENC_KEY_TYPE_MSRLN, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_MSRLN16", (PyCFunction)get_ENC_KEY_TYPE_MSRLN16, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_RLWE_BCNS15", (PyCFunction)get_ENC_KEY_TYPE_RLWE_BCNS15, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_LWE_FRODO", (PyCFunction)get_ENC_KEY_TYPE_LWE_FRODO, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIDH_IQC_REF", (PyCFunction)get_ENC_KEY_TYPE_SIDH_IQC_REF, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_CODE_MCBITS", (PyCFunction)get_ENC_KEY_TYPE_CODE_MCBITS, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_NTRU", (PyCFunction)get_ENC_KEY_TYPE_NTRU, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_MLWE_KYBER", (PyCFunction)get_ENC_KEY_TYPE_MLWE_KYBER, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_PICNIC", (PyCFunction)get_ENC_KEY_TYPE_SIG_PICNIC, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_BLISS", (PyCFunction)get_ENC_KEY_TYPE_SIG_BLISS, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_TESLA", (PyCFunction)get_ENC_KEY_TYPE_SIG_TESLA, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_SIG_DILITHIUM", (PyCFunction)get_ENC_KEY_TYPE_SIG_DILITHIUM, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_KEY_TYPE_NULL", (PyCFunction)get_ENC_KEY_TYPE_NULL, METH_NOARGS | METH_STATIC, ""},


        {NULL, NULL, 0, NULL}
};

PyTypeObject DapCryptoKeyTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.Crypto.KeyType",           /* tp_name */
        sizeof(PyCryptoKeyTypeObject),       /* tp_basicsize */
        0,		                         /* tp_itemsize */
        0,		   			 /* tp_dealloc */
        0,            		         /* tp_print */
        0,           		         /* tp_getattr */
        0,                         		 /* tp_setattr */
        0,                         		 /* tp_reserved */
        0,                         		 /* tp_repr */
        0,                                   /* tp_as_number */
        0,                                   /* tp_as_sequence */
        0,                                   /* tp_as_mapping */
        0,                                   /* tp_hash  */
        0,                                   /* tp_call */
        0,                                   /* tp_str */
        0,                                   /* tp_getattro */
        0,                                   /* tp_setattro */
        0,                                   /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,             /* tp_flags */
        "Crypto keys type objects",          /* tp_doc */
        0,                                   /* tp_traverse */
        0,		                         /* tp_clear */
        0,		               		 /* tp_richcompare */
        0,                                   /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        PyCryptoKeyTypeObjectMethods,        /* tp_methods */
        0,                                   /* tp_members */
        0,                                   /* tp_getset */
        0,                                   /* tp_base */
        0,                                   /* tp_dict */
        0,                                   /* tp_descr_get */
        0,                                   /* tp_descr_set */
        0,                                   /* tp_dictoffset */
        0,                                   /* tp_init */
        0,                                   /* tp_alloc */
        PyType_GenericNew,                   /* tp_new */
};

PyObject *get_ENC_KEY_TYPE_IAES(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_IAES);
}
PyObject *get_ENC_KEY_TYPE_OAES(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_OAES);
}
PyObject *get_ENC_KEY_TYPE_RLWE_NEWHOPE(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_RLWE_NEWHOPE_CPA_KEM );
}
PyObject *get_ENC_KEY_TYPE_SIDH_CLN16(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIDH_CLN16);
}
PyObject *get_ENC_KEY_TYPE_DEFEO(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_DEFEO);
}
PyObject *get_ENC_KEY_TYPE_MSRLN(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_MSRLN);
}
PyObject *get_ENC_KEY_TYPE_MSRLN16(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_RLWE_MSRLN16);
}
PyObject *get_ENC_KEY_TYPE_RLWE_BCNS15(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_RLWE_BCNS15);
}
PyObject *get_ENC_KEY_TYPE_LWE_FRODO(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_LWE_FRODO);
}
PyObject *get_ENC_KEY_TYPE_SIDH_IQC_REF(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIDH_IQC_REF);
}
PyObject *get_ENC_KEY_TYPE_CODE_MCBITS(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_CODE_MCBITS);
}
PyObject *get_ENC_KEY_TYPE_NTRU(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_NTRU);
}
PyObject *get_ENC_KEY_TYPE_MLWE_KYBER(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_MLWE_KYBER);
}
PyObject *get_ENC_KEY_TYPE_SIG_PICNIC(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIG_PICNIC);
}
PyObject *get_ENC_KEY_TYPE_SIG_BLISS(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIG_BLISS);
}
PyObject *get_ENC_KEY_TYPE_SIG_TESLA(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIG_TESLA);
}
PyObject *get_ENC_KEY_TYPE_SIG_DILITHIUM(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_SIG_DILITHIUM);
}
PyObject *get_ENC_KEY_TYPE_NULL(){
    return PyLong_FromLong(DAP_ENC_KEY_TYPE_NULL);
}
