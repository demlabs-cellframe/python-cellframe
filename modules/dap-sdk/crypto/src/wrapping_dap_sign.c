#include "wrapping_dap_sign.h"
#include "libdap_chain_net_python.h"

#define LOG_TAG "wrapping_sign"

/* Sign type*/
PyTypeObject DapCryproSignTypeObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.Crypto.SignType", sizeof(PyDapSignTypeObject),
        "Sign type object",
        .tp_str = PyDapSignType_to_str);

PyObject *PyDapSignType_to_str(PyObject *self){
    return Py_BuildValue("s", dap_sign_type_to_str(((PyDapSignTypeObject*)self)->sign_type));
}

/* Sign */
static PyGetSetDef DapSignObjectGetsSetsDef[] = {
        {"type", (getter)wrapping_dap_sign_get_type, NULL, NULL, NULL},
        {"pkey", (getter)wrapping_dap_sign_get_pkey, NULL, NULL, NULL},
        {"pkeyHash", (getter)wrapping_dap_sign_get_pkey_hash, NULL, NULL, NULL},
        {"size", (getter)wrapping_dap_sign_get_size, NULL, NULL, NULL},
        {}
};

static PyMethodDef DapSignObjectMethods[]= {
        {"verify", wrapping_dap_sign_verify, METH_VARARGS, ""},
        {"fromBytes", wrapping_dap_sign_from_bytes, METH_VARARGS | METH_STATIC, ""},
        {"toBytes", wrapping_dap_sign_get_bytes, METH_NOARGS, ""},
        {"fromBase64", wrapping_dap_sign_from_b64, METH_VARARGS | METH_STATIC, ""},
        {"toBase64", wrapping_dap_sign_to_b64, METH_NOARGS, ""},
        {"getAddr", wrapping_dap_sign_get_addr, METH_VARARGS, ""},
        {}
};

PyTypeObject DapCryptoSignObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.Crypto.Sign", sizeof(PyDapSignObject),
        "Sign object",
        .tp_dealloc = (destructor)PyDapSignObject_free,
        .tp_methods = DapSignObjectMethods,
        .tp_getset = DapSignObjectGetsSetsDef,
        .tp_init = wrapping_dap_sign_create);

void PyDapSignObject_free(PyDapSignObject *self) {
    if (self->sign) {
        DAP_DELETE(self->sign);
    }
    if (self->cached_pkey) {
        DAP_DELETE(self->cached_pkey);
    }
    PyTypeObject *tp = Py_TYPE(self);
    tp->tp_free(self);
}

PyObject *wrapping_dap_sign_get_type(PyObject *self, void *closure){
    (void)closure;
    PyDapSignTypeObject *obj_type_sign = PyObject_New(PyDapSignTypeObject, &DapCryproSignTypeObjectType);
    obj_type_sign->sign_type = ((PyDapSignObject*)self)->sign->header.type;
    return (PyObject*)obj_type_sign;
}
PyObject *wrapping_dap_sign_get_pkey(PyObject *self, void *closure){
    (void)closure;
    PyDapSignObject *l_sign_obj = (PyDapSignObject*)self;
    
    // Check if we already have a cached pkey
    if (l_sign_obj->cached_pkey) {
        // Create a new PyDapPkeyObject that references the cached pkey
        PyDapPkeyObject *obj_pkey = PyObject_New(PyDapPkeyObject, &DapPkeyObject_DapPkeyObjectType);
        if (!obj_pkey) {
            log_it(L_CRITICAL, "Failed to create PyDapPkeyObject");
            return NULL;
        }
        
        // Create a copy of the cached pkey for the Python object
        size_t l_pkey_size = sizeof(dap_pkey_t) + l_sign_obj->cached_pkey->header.size;
        obj_pkey->pkey = DAP_NEW_Z_SIZE(dap_pkey_t, l_pkey_size);
        if (!obj_pkey->pkey) {
            log_it(L_CRITICAL, "Memory allocation error for pkey copy");
            Py_DECREF(obj_pkey);
            return NULL;
        }
        memcpy(obj_pkey->pkey, l_sign_obj->cached_pkey, l_pkey_size);
        return (PyObject*)obj_pkey;
    }
    
    // First access - create and cache the pkey
    l_sign_obj->cached_pkey = dap_pkey_get_from_sign(l_sign_obj->sign);
    if (!l_sign_obj->cached_pkey) {
        log_it(L_ERROR, "Failed to get pkey from sign");
        return NULL;
    }
    
    // Create PyDapPkeyObject with a copy of the cached pkey
    PyDapPkeyObject *obj_pkey = PyObject_New(PyDapPkeyObject, &DapPkeyObject_DapPkeyObjectType);
    if (!obj_pkey) {
        log_it(L_CRITICAL, "Failed to create PyDapPkeyObject");
        return NULL;
    }
    
    size_t l_pkey_size = sizeof(dap_pkey_t) + l_sign_obj->cached_pkey->header.size;
    obj_pkey->pkey = DAP_NEW_Z_SIZE(dap_pkey_t, l_pkey_size);
    if (!obj_pkey->pkey) {
        log_it(L_CRITICAL, "Memory allocation error for pkey copy");
        Py_DECREF(obj_pkey);
        return NULL;
    }
    memcpy(obj_pkey->pkey, l_sign_obj->cached_pkey, l_pkey_size);
    
    return (PyObject*)obj_pkey;
}

PyObject *wrapping_dap_sign_get_pkey_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hash->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    dap_sign_get_pkey_hash(((PyDapSignObject*)self)->sign, obj_hash->hash_fast);
    obj_hash->origin = true;
    return (PyObject*)obj_hash;
}

PyObject *wrapping_dap_sign_get_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapSignObject*)self)->sign->header.sign_size);
}

PyObject *wrapping_dap_sign_get_addr(PyObject *self, PyObject *args){
    PyObject *obj_net;
    if (!PyArg_ParseTuple(args, "O", &obj_net)){
        return NULL;
    }
    if (!PyDapChainNet_Check((PyDapChainNetObject*)obj_net)){
        PyErr_SetString(PyExc_SyntaxError, "The getAddr function must take an object of type "
                                           "CellFrame.Network.Net as an argument.");
        return NULL;
    }

    PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
    
    obj_addr->addr = DAP_NEW(dap_chain_addr_t);
    
    dap_hash_fast_t l_hf = {0};
    dap_hash_fast(((PyDapSignObject *)self)->sign->pkey_n_sign, ((PyDapSignObject *)self)->sign->header.sign_pkey_size, &l_hf);
    dap_chain_addr_fill(obj_addr->addr,
                        ((PyDapSignObject *)self)->sign->header.type,
                        &l_hf,
                        ((PyDapChainNetObject*)obj_net)->chain_net->pub.id);
    return (PyObject*)obj_addr;
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
        l_sign = dap_sign_create(((PyCryptoKeyObject*)obj_key)->key, l_bytes, l_bytes_size);
    }
    if (DapChainDatumToken_Check(obj_data)){
        l_sign = dap_sign_create(((PyCryptoKeyObject*)obj_key)->key,
                                 ((PyDapChainDatumTokenObject*)obj_data)->token,
                                 ((PyDapChainDatumTokenObject*)obj_data)->token_size);
    }
    if (PyDapChainDatumTokenEmissionObject_check(obj_data)){
        l_sign = dap_sign_create(((PyCryptoKeyObject*)obj_key)->key,
                        ((PyDapChainDatumTokenEmissionObject*)obj_data)->token_emission,
                        ((PyDapChainDatumTokenEmissionObject*)obj_data)->token_size);
    }
    if (PyDapChainDatum_Check(obj_data)) {
        size_t l_datum_size = dap_chain_datum_size(((PyDapChainDatumObject*)obj_data)->datum);
        l_sign = dap_sign_create(
                ((PyCryptoKeyObject*)obj_key)->key,
                ((PyDapChainDatumObject*)obj_data)->datum, l_datum_size);
    }
    if (DapChainDatumTx_Check(obj_data)){
        size_t l_datum_tx_size = dap_chain_datum_tx_get_size(((PyDapChainDatumTxObject*)obj_data)->datum_tx);
        l_sign = dap_sign_create(((PyCryptoKeyObject*)obj_key)->key,
                                 ((PyDapChainDatumTxObject*)obj_data)->datum_tx, l_datum_tx_size);
    }
    if (!l_sign){
        PyErr_SetString(PyExc_AttributeError, "The signature could not be created, the second argument "
                                              "may have been passed incorrectly, or an unsupported argument may have "
                                              "been passed.");
        return -1;
    }
    ((PyDapSignObject*)self)->sign = l_sign;
    ((PyDapSignObject*)self)->cached_pkey = NULL;  // Initialize cached pkey to NULL
    return 0;
}

PyObject *wrapping_dap_sign_verify(PyObject *self, PyObject *args){
    PyObject *obj_data;
    if (!PyArg_ParseTuple(args, "O", &obj_data)){
        return NULL;
    }
    void *l_data = NULL;
    size_t l_data_size = 0;
    if (PyBytes_Check(obj_data)){
        l_data = PyBytes_AsString(obj_data);
        l_data_size = PyBytes_Size(obj_data);
    }
    if (DapChainDatumToken_Check(obj_data)){
        l_data = ((PyDapChainDatumTokenObject*)obj_data)->token;
        l_data_size = ((PyDapChainDatumTokenObject*)obj_data)->token_size;
    }
    if (PyDapChainDatumTokenEmissionObject_check(obj_data)){
        l_data = ((PyDapChainDatumTokenEmissionObject*)obj_data)->token_emission;
        l_data_size = ((PyDapChainDatumTokenEmissionObject*)obj_data)->token_size;
    }
    if (PyDapChainDatum_Check(obj_data)) {
        l_data = ((PyDapChainDatumObject*)obj_data)->datum;
        l_data_size = dap_chain_datum_size(((PyDapChainDatumObject*)obj_data)->datum);
    }
    if (DapChainDatumTx_Check(obj_data)){
        l_data = ((PyDapChainDatumTxObject*)obj_data)->datum_tx;
        l_data_size = dap_chain_datum_tx_get_size(((PyDapChainDatumTxObject*)obj_data)->datum_tx);
    }
    if (!l_data || l_data_size == 0){
        PyErr_SetString(PyExc_AttributeError, "The attribute of this function was passed incorrectly, "
                                              "the function accepts the attribute Datum, Datum Token, "
                                              "DatumTokenEmission, DatumTx, byte.");
        return NULL;
    }
    if(dap_sign_verify(((PyDapSignObject*)self)->sign, l_data, l_data_size)){
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

PyObject *wrapping_dap_sign_get_bytes(PyObject *self, PyObject *args){
    (void)args;
    dap_sign_t *l_sign = ((PyDapSignObject*)self)->sign;
    size_t l_sign_size = dap_sign_get_size(((PyDapSignObject*)self)->sign);
    PyObject *l_bytes = PyBytes_FromStringAndSize((char *)l_sign, l_sign_size);
    return l_bytes;
}

PyObject *wrapping_dap_sign_from_bytes(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_data;
    if (!PyArg_ParseTuple(args, "O", &obj_data)){
        return NULL;
    }
    if (!PyBytes_Check(obj_data)){
        PyErr_SetString(PyExc_AttributeError, "The first argument was not passed correctly, the first argument "
                                              "must be an object of type Bytes.");
        return NULL;
    }
    //size_t l_size = PyBytes_Size(obj_data);
    void *l_buff = PyBytes_AsString(obj_data);
    dap_sign_t *l_sign = (dap_sign_t*)l_buff;
    return PyDapSignObject_Create(l_sign);
}

PyObject *wrapping_dap_sign_to_b64(PyObject *self, PyObject *args){
    (void)args;
    dap_sign_t *l_sign = ((PyDapSignObject*)self)->sign;
    size_t l_sign_size = dap_sign_get_size(((PyDapSignObject*)self)->sign);
    char l_str_out[DAP_ENC_BASE64_ENCODE_SIZE(l_sign_size )];
    size_t l_str_out_size = dap_enc_base64_encode(l_sign, l_sign_size, l_str_out, DAP_ENC_DATA_TYPE_B64);
    if (l_str_out_size == 0){
        Py_RETURN_NONE;
    }
    return Py_BuildValue("s", l_str_out);
}

PyObject *wrapping_dap_sign_from_b64(PyObject *self, PyObject *args){
    (void)self;
    char *l_str;
    if (!PyArg_ParseTuple(args, "s", &l_str)){
        return NULL;
    }
    size_t l_str_size = dap_strlen(l_str);
    void *l_out = DAP_NEW_Z_SIZE(void*, DAP_ENC_BASE64_ENCODE_SIZE(l_str_size));
    size_t l_out_size = dap_enc_base64_decode(l_str, l_str_size, l_out, DAP_ENC_DATA_TYPE_B64);
    if (l_out_size == 0)
        Py_RETURN_NONE;
    PyDapSignObject *l_sign_obj = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
    l_sign_obj->sign = (dap_sign_t*)l_out;
    l_sign_obj->cached_pkey = NULL;  // Initialize cached pkey to NULL
    return (PyObject*)l_sign_obj;
}

/**
 * @brief Create a new PyDapSignObject from a dap_sign_t
 * @param a_sign The source sign data to copy
 * @return PyObject* or NULL on error
 */
PyObject *PyDapSignObject_Create(dap_sign_t *a_sign) {
    if (!a_sign) {
        log_it(L_ERROR, "Invalid sign parameter");
        return NULL;
    }
    
    PyDapSignObject *l_obj_sign = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
    if (!l_obj_sign) {
        log_it(L_CRITICAL, "Failed to create PyDapSignObject");
        return NULL;
    }
    
    size_t l_sign_size = dap_sign_get_size(a_sign);
    if (l_sign_size == 0) {
        log_it(L_ERROR, "Invalid sign size");
        Py_DECREF(l_obj_sign);
        return NULL;
    }
    
    l_obj_sign->sign = DAP_NEW_Z_SIZE(dap_sign_t, l_sign_size);
    if (!l_obj_sign->sign) {
        log_it(L_CRITICAL, "Memory allocation error for sign data");
        Py_DECREF(l_obj_sign);
        return NULL;
    }
    
    memcpy(l_obj_sign->sign, a_sign, l_sign_size);
    l_obj_sign->cached_pkey = NULL;  // Initialize cached pkey to NULL
    return (PyObject*)l_obj_sign;
}
