#include "wrapping_dap_enc_key.h"

PyObject* dap_enc_key_get_enc_size_py(PyObject *self, PyObject *args){
    return PyLong_FromLong(0);
}
PyObject* dap_enc_key_get_dec_size_py(PyObject *self, PyObject *args){
    return PyLong_FromLong(0);
}

// allocate memory for key struct
PyObject* dap_enc_key_new_py(PyObject *self, PyObject *args){
    int type_key;
    if(!PyArg_ParseTuple(args, "i", &type_key)){
        return NULL;
    }
    if (type_key < 0 || type_key > 16){
        return PyLong_FromLong(-1);
    }
    dap_enc_key_t *new_key = dap_enc_key_new(type_key);
    uint8_t res = key_list_add_element(keys, new_key);
    return  PyLong_FromLong(res);
}

/// default gen key
PyObject *dap_enc_key_new_generate_py(PyObject *self, PyObject *args){
    return PyLong_FromLong(0);
}

// update struct dap_enc_key_t after insert foreign keys
PyObject* dap_enc_key_update_py(PyObject *self, PyObject *args){
    return PyLong_FromLong(0);
}

// for asymmetric gen public key
PyObject *dap_enc_gen_pub_key_from_priv_py(PyObject *self, PyObject *args){
    return PyLong_FromLong(0);
}


PyObject *dap_enc_gen_key_public_size_py(PyObject *self, PyObject *args){
    return PyLong_FromLong(0);
}
PyObject *dap_enc_gen_key_public_py(PyObject *self, PyObject *args){
    return PyLong_FromLong(0);
}

PyObject *dap_enc_key_signature_delete_py(PyObject *self, PyObject *args){
    return PyLong_FromLong(0);
}

PyObject *dap_enc_key_delete_py(PyObject *self, PyObject *args){
    //PyObject *obj;
    uint8_t key_id;
    if (!PyArg_ParseTuple(args, "h", &key_id)){
        return NULL;
    }
    dap_enc_key_t *key = key_list_get_key(keys, key_id);
    if (key == NULL) {
        return NULL;
    }
    key_list_del_element(keys, key_id);
    dap_enc_key_delete(key);
    return PyLong_FromLong(0);
}