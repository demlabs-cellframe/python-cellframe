#include "wrapping_dap_enc_key.h"

#define LOG_TAG "wrapping-dap-enc-key"

PyObject* dap_enc_key_get_enc_size_py(PyObject *self, PyObject *args){
    PyObject *in_key;
    size_t buff_in_size;
    if (!PyArg_ParseTuple(args, "O|i", &in_key, &buff_in_size)){
        return NULL;
    }
//    dap_enc_key_t *key = key_list_get_key(keys, key_id);
//    if (key == NULL)
//        return NULL;
    size_t size_buff = dap_enc_key_get_enc_size(((PyCryptoKeyObject*)in_key)->key, buff_in_size);
    if (size_buff == 0)
        return NULL;
    return  PyLong_FromSize_t(size_buff);
}

PyObject* dap_enc_key_get_dec_size_py(PyObject *self, PyObject *args){
    PyObject *in_key;
    size_t buff_in_size;
    if (!PyArg_ParseTuple(args, "O|i", &in_key, &buff_in_size)){
        return NULL;
    }
//    dap_enc_key_t *key = key_list_get_key(keys, key_id);
//    if (key == NULL)
//        return NULL;
    size_t size_buff = dap_enc_key_get_dec_size(((PyCryptoKeyObject*)in_key)->key, buff_in_size);
    if (size_buff == 0)
        return NULL;
    return  PyLong_FromSize_t(size_buff);
}

// allocate memory for key struct
PyObject* dap_enc_key_new_py(PyObject *self, PyObject *args){
    uint8_t type_key;
    if(!PyArg_ParseTuple(args, "h", &type_key)){
        return NULL;
    }
    if (type_key > 16){
        return PyLong_FromLong(-1);
    }
    dap_enc_key_t *new_key = dap_enc_key_new(type_key);
    PyObject *obj = _PyObject_New(&PyCryptoKeyObject_PyCryptoKeyType);
    ((PyCryptoKeyObject*)obj)->key = new_key;
    return  Py_BuildValue("O", obj);
}

/// default gen key
PyObject *dap_enc_key_new_generate_py(PyObject *self, PyObject *args){
    uint8_t in_type_key;
    PyBytesObject *in_kex_buf;
    size_t in_kex_size;
    PyBytesObject *in_seed;
    size_t in_seed_size;
    size_t in_key_size;
    if (!PyArg_ParseTuple(args, "h|S|n|S|n|n", &in_type_key, &in_kex_buf, &in_kex_size, &in_seed,
                         &in_seed_size, &in_key_size)){
        return NULL;
    }
    if (in_type_key > 16){
        return PyLong_FromLong(-1);
    }
    void *kex_buf = NULL;
    void *seed = NULL;
    if (in_kex_size != 0)
        kex_buf = PyBytes_AsString((PyObject*)in_kex_buf);
    if (in_seed_size != 0)
        seed = PyBytes_AsString((PyObject*)in_seed);
    PyCryptoKeyObject *obj_key = (PyCryptoKeyObject*)_PyObject_New(&PyCryptoKeyObject_PyCryptoKeyType);
    obj_key->key = dap_enc_key_new_generate(in_type_key, kex_buf, in_kex_size, seed, in_seed_size, in_key_size);
    return  Py_BuildValue("O", (PyObject*)obj_key);
}

// update struct dap_enc_key_t after insert foreign keys
PyObject* dap_enc_key_update_py(PyObject *self, PyObject *args){
    PyObject *in_key;
    if (!PyArg_ParseTuple(args, "O", &in_key)){
        return NULL;
    }
//    dap_enc_key_t *key = key_list_get_key(keys, key_id);
//    if (key == NULL) {
//        return NULL;
//    }
    dap_enc_key_update(((PyCryptoKeyObject*)in_key)->key);
    return PyLong_FromLong(0);
}

// for asymmetric gen public key
PyObject *dap_enc_gen_pub_key_from_priv_py(PyObject *self, PyObject *args){ //NOTE libdap-crypto/src/libdap-crypto.c
    return PyLong_FromLong(0);
}


PyObject *dap_enc_gen_key_public_size_py(PyObject *self, PyObject *args){
    PyObject *in_key;
    if (PyArg_ParseTuple(args, "h", &in_key)){
        return NULL;
    }
//    dap_enc_key_t *key = key_list_get_key(keys, key_id);
//    if (key == NULL){
//        return NULL;
//    }
    size_t size = dap_enc_gen_key_public_size(((PyCryptoKeyObject*)in_key)->key);
    return PyLong_FromSize_t(size);
}

PyObject *dap_enc_gen_key_public_py(PyObject *self, PyObject *args){
    PyObject *in_key;
    PyObject *obj;
    if (PyArg_ParseTuple(args, "O|O", &in_key, &obj)){
        return NULL;
    }
//    dap_enc_key_t *key = key_list_get_key(keys, key_id);
//    if (key == NULL){
//        return NULL;
//    }
    int size = dap_enc_gen_key_public(((PyCryptoKeyObject*)in_key)->key, obj);
    return PyLong_FromLong(size);
}
