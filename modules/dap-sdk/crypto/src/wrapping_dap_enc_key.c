/*
 * Authors:
 * Alexey Stratulat <alexey.stratulat@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://cellframe.net
 * Sources         https://gitlab.demlabs.net/cellframe
 * Copyright  (c) 2017-2020
 * All rights reserved.

 This file is part of CellFrame SDK the open source project

    CellFrame SDK is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CellFrame SDK is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with any CellFrame SDK based project.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "wrapping_dap_enc_key.h"

#define LOG_TAG "wrapping-dap-enc-key"

PyObject* dap_enc_key_get_enc_size_py(PyObject *self, PyObject *args){
    size_t buff_in_size;
    if (!PyArg_ParseTuple(args, "i", &buff_in_size)){
        return NULL;
    }
    size_t size_buff = dap_enc_key_get_enc_size(((PyCryptoKeyObject*)self)->key, buff_in_size);
    return  PyLong_FromSize_t(size_buff);
}

PyObject* dap_enc_key_get_dec_size_py(PyObject *self, PyObject *args){
    size_t buff_in_size;
    if (!PyArg_ParseTuple(args, "i", &buff_in_size)){
        return NULL;
    }
    size_t size_buff = dap_enc_key_get_dec_size(((PyCryptoKeyObject*)self)->key, buff_in_size);
    return  PyLong_FromSize_t(size_buff);
}

PyObject *dap_enc_key_serealize_priv_key_py(PyObject *self, PyObject *args){
    (void)args;
    size_t bufflen_out = 0;
    void *data = dap_enc_key_serealize_priv_key(((PyCryptoKeyObject*)self)->key, &bufflen_out);
    PyObject *ret = PyBytes_FromStringAndSize(data, bufflen_out);
    return ret;
}

PyObject *dap_enc_key_serealize_pub_key_py(PyObject *self, PyObject *args){
    (void)args;
    size_t bufflen_out = 0;
    void *data = dap_enc_key_serealize_pub_key(((PyCryptoKeyObject*)self)->key, &bufflen_out);
    PyObject *ret = PyBytes_FromStringAndSize(data, bufflen_out);
    return ret;
}

PyObject *dap_enc_key_deserealize_priv_key_py(PyObject *self, PyObject *args){
    PyObject *obj_priv_key;
    if (!PyArg_ParseTuple(args, "O", &obj_priv_key)){
        PyErr_SetString(PyExc_ValueError, "This function must accept one object type bytes");
        return NULL;
    }
    if (!PyBytes_Check(obj_priv_key)){
        PyErr_SetString(PyExc_ValueError, "This first argument is not a bytes object");
        return NULL;
    }
    void *l_in_data = PyBytes_AsString(obj_priv_key);
    size_t l_in_data_size = PyBytes_Size(obj_priv_key);
    int ret_code = dap_enc_key_deserealize_priv_key(((PyCryptoKeyObject*)self)->key, l_in_data, l_in_data_size);
    if (ret_code == 0)
        return Py_None;
    return Py_BuildValue("O", Py_False);
}

PyObject *dap_enc_key_deserealize_pub_key_py(PyObject *self, PyObject *args){
    PyObject *obj_pub_key;
    if (!PyArg_ParseTuple(args, "O", &obj_pub_key)){
        PyErr_SetString(PyExc_ValueError, "This function must accept one object type bytes");
        return NULL;
    }
    if (!PyBytes_Check(obj_pub_key)){
        PyErr_SetString(PyExc_ValueError, "This first argument is not a bytes object");
        return NULL;
    }
    void *l_in_data = PyBytes_AsString(obj_pub_key);
    size_t l_in_data_size = PyBytes_Size(obj_pub_key);
    int ret_code = dap_enc_key_deserealize_pub_key(((PyCryptoKeyObject*)self)->key, l_in_data, l_in_data_size);
    if (ret_code == 0)
        return Py_None;
    return Py_BuildValue("O", Py_False);
}

PyObject *dap_enc_key_dup_py(PyObject *self, PyObject *args){
    (void)args;
    PyObject *new_obj = _PyObject_New(&CryptoKeyObjecy_CryptoKeyObjecyType);
    ((PyCryptoKeyObject*)new_obj)->key = dap_enc_key_dup(((PyCryptoKeyObject*)self)->key);
    return new_obj;
}

// allocate memory for key struct
PyObject* dap_enc_key_new_py(PyObject *self, PyObject *args){
/*    uint8_t type_key;
    if(!PyArg_ParseTuple(args, "h", &type_key)){
        return NULL;
    }
    if (type_key > 16){
        return PyLong_FromLong(-1);
    }
    dap_enc_key_t *new_key = dap_enc_key_new(type_key);
    PyObject *obj = _PyObject_New(&PyCryptoKeyObject_PyCryptoKeyType);
    ((PyCryptoKeyObject*)obj)->key = new_key;
    return  Py_BuildValue("O", obj)*/return NULL;
}

/// default gen key
PyObject *dap_enc_key_new_generate_py(PyObject *self, PyObject *args){
    return NULL;
//    uint8_t in_type_key;
//    PyBytesObject *in_kex_buf;
//    size_t in_kex_size;
//    PyBytesObject *in_seed;
//    size_t in_seed_size;
//    size_t in_key_size;
//    if (!PyArg_ParseTuple(args, "h|S|n|S|n|n", &in_type_key, &in_kex_buf, &in_kex_size, &in_seed,
//                         &in_seed_size, &in_key_size)){
//        return NULL;
//    }
//    if (in_type_key > 16){
//        return PyLong_FromLong(-1);
//    }
//    void *kex_buf = NULL;
//    void *seed = NULL;
//    if (in_kex_size != 0)
//        kex_buf = PyBytes_AsString((PyObject*)in_kex_buf);
//    if (in_seed_size != 0)
//        seed = PyBytes_AsString((PyObject*)in_seed);
//    PyCryptoKeyObject *obj_key = (PyCryptoKeyObject*)_PyObject_New(&PyCryptoKeyObject_PyCryptoKeyType);
//    obj_key->key = dap_enc_key_new_generate(in_type_key, kex_buf, in_kex_size, seed, in_seed_size, in_key_size);
//    return  Py_BuildValue("O", (PyObject*)obj_key);
}

// update struct dap_enc_key_t after insert foreign keys
PyObject* dap_enc_key_update_py(PyObject *self, PyObject *args){
    (void)args;
//    dap_enc_key_t *key = key_list_get_key(keys, key_id);
//    if (key == NULL) {
//        return NULL;
//    }
    dap_enc_key_update(((PyCryptoKeyObject*)self)->key);
    return Py_None;
}

// for asymmetric gen public key
PyObject *dap_enc_gen_pub_key_from_priv_py(PyObject *self, PyObject *args){ //NOTE libdap-crypto/src/libdap-crypto.c
    return PyLong_FromLong(0);
}


PyObject *dap_enc_gen_key_public_size_py(PyObject *self, PyObject *args){
    (void)args;
    size_t size = dap_enc_gen_key_public_size(((PyCryptoKeyObject*)self)->key);
    return PyLong_FromSize_t(size);
}

PyObject *dap_enc_gen_key_public_py(PyObject *self, PyObject *args){
    (void)args;
    void *out_data = NULL;
    int ret_code = dap_enc_gen_key_public(((PyCryptoKeyObject*)self)->key, out_data);
    if (ret_code == -1)
        return Py_None;
    //TODO
    return NULL;
    return PyLong_FromLong(ret_code);
}
