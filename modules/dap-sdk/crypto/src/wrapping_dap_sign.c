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
        wrapping_dap_sign_create,                               /* tp_init */
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

int wrapping_dap_sign_create(PyObject *self, PyObject* args, PyObject *kwds){
    //DATUM, DATUM_TX,  DATUM_TOKEN, Bytes
    const char *kwlist[] = {
            "key",
            "data",
            NULL
    };
    PyObject *obj_key;
    PyObject *obj_data;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO", (char**)kwlist, &obj_key, &obj_data))
        return -1;
    if (!PyCryptoKeyObject_check(obj_key)){
        PyErr_SetString(PyExc_AttributeError, "The first argument was passed incorrectly, the first "
                                              "argument must be an object of type Crypto.Key.");
        return -1;
    }
    dap_sign_t *l_sign = NULL;
    //DATA = BYTES
    if (PyBytes_Check(obj_data)){
        void *l_bytes = PyBytes_AsString(obj_data);
        size_t l_bytes_size = PyBytes_Size(obj_data);
        l_sign = dap_sign_create(((PyCryptoKeyObject*)obj_key)->key, l_bytes, l_bytes_size,0 );
    }
    if (DapChainDatumToken_Check(obj_data)){
        l_sign = dap_sign_create(((PyCryptoKeyObject*)obj_key)->key,
                                 ((PyDapChainDatumTokenObject*)obj_data)->token,
                                 ((PyDapChainDatumTokenObject*)obj_data)->token_size, 0);
    }
    if (PyDapChainDatumTokenEmissionObject_check(obj_data)){
        l_sign = dap_sign_create(((PyCryptoKeyObject*)obj_key)->key,
                        ((PyDapChainDatumTokenEmissionObject*)obj_data)->token_emission,
                        ((PyDapChainDatumTokenEmissionObject*)obj_data)->token_size, 0);
    }
    if (PyDapChainDatum_Check(obj_data)){
        size_t l_datum_size = dap_chain_datum_size(((PyDapChainDatumObject*)obj_data)->datum);
        l_sign = dap_sign_create(
                ((PyCryptoKeyObject*)obj_key)->key,
                ((PyDapChainDatumObject*)obj_data)->datum, l_datum_size, 0);
    }
    if (DapChainDatumTx_Check(obj_data)){
        size_t l_datum_tx_size = dap_chain_datum_tx_get_size(((PyDapChainDatumTxObject*)obj_data)->datum_tx);
        l_sign = dap_sign_create(((PyCryptoKeyObject*)obj_key)->key,
                                 ((PyDapChainDatumTxObject*)obj_data)->datum_tx, l_datum_tx_size, 0);
    }
    if (!l_sign){
        PyErr_SetString(PyExc_AttributeError, "The signature could not be created, the second argument "
                                              "may have been passed incorrectly, or an unsupported argument may have "
                                              "been passed.");
        return -1;
    }
    ((PyDapSignObject*)self)->sign = l_sign;
    return 0;
}
