#include "wrapping_dap_sign.h"

/* Sign type*/
PyTypeObject DapCryproSignTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.Crypto.SignType",       /* tp_name */
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

PyObject *PyDapSignType_to_str(PyObject *self){
    return Py_BuildValue("s", dap_sign_type_to_str(*((PyDapSignTypeObject*)self)->sign_type));
}

/* Sign */
PyGetSetDef DapSignObjectGetsSetsDef[] = {
        {"type", (getter)wrapping_dap_sign_get_type, NULL, NULL, NULL},
        {"pkey", (getter)wrapping_dap_sign_get_pkey, NULL, NULL, NULL},
        {"pkeyHash", (getter)wrapping_dap_sign_get_pkey_hash, NULL,NULL},
        {"size", (getter)wrapping_dap_sign_get_size, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapCryptoSignObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.Crypto.Sign",       /* tp_name */
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

PyObject *wrapping_dap_sign_get_type(PyObject *self, void *closure){
    (void)closure;
    PyDapSignTypeObject *obj_type_sign = PyObject_New(PyDapSignTypeObject, &DapCryproSignTypeObjectType);
    PyObject_Dir((PyObject*)obj_type_sign);
    obj_type_sign->sign_type = &((PyDapSignObject*)self)->sign->header.type;
    return (PyObject*)obj_type_sign;
}
PyObject *wrapping_dap_sign_get_pkey(PyObject *self, void *closure){
    (void)closure;
    PyDapPkeyObject *obj_pkey = PyObject_New(PyDapPkeyObject, &DapPkeyObject_DapPkeyObjectType);
    PyObject_Dir((PyObject*)obj_pkey);
    obj_pkey->pkey = (dap_pkey_t*)((PyDapSignObject*)self)->sign->pkey_n_sign;
    return (PyObject*)obj_pkey;
}

PyObject *wrapping_dap_sign_get_pkey_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    PyObject_Dir((PyObject*)obj_hash);
    obj_hash->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    dap_sign_get_pkey_hash(((PyDapSignObject*)self)->sign, obj_hash->hash_fast);
    return (PyObject*)obj_hash;
}

PyObject *wrapping_dap_sign_get_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapSignObject*)self)->sign->header.sign_size);
}
