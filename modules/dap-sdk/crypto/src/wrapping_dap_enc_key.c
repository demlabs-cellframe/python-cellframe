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

int wrapping_dap_enc_key_init(PyCryptoKeyObject *self, PyObject *args, PyObject *kwds){
    PyObject *l_key_type;
    PyObject *in_kex_buf;
    PyObject *in_seed;
    size_t in_key_size;
    static char* kwlist[] = {"kexBuffer", "seed", "keySize",NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|OOi", kwlist, &l_key_type, &in_kex_buf, &in_seed, &in_key_size)){
        return -1;
    }
    if (in_kex_buf == NULL){
        self->key = dap_enc_key_new(((PyCryptoKeyTypeObject*)l_key_type)->key_type);
        self->key_filled = false;
        return 0;
    }else{
        if (!PyBytes_Check(in_kex_buf)){
            return -1;
        }
        void *l_in_kex = PyBytes_AsString(in_kex_buf);
        size_t l_in_kex_size = PyBytes_Size(in_kex_buf);

        if (!PyBytes_Check(in_seed)){
            void *l_in_seed = PyBytes_AsString(in_seed);
            size_t l_in_seed_size = PyBytes_Size(in_seed);
            self->key = dap_enc_key_new_generate(((PyCryptoKeyTypeObject*)l_key_type)->key_type, l_in_kex, l_in_kex_size,
                                                 l_in_seed, l_in_seed_size, in_key_size);
            self->key_filled = true;
        } else {
            self->key = dap_enc_key_new_generate(((PyCryptoKeyTypeObject*)l_key_type)->key_type, l_in_kex, l_in_kex_size,
                                                 NULL, 0, in_key_size);
            self->key_filled = true;
        }
    }
    return 0;
}

void dap_enc_key_delete_py(PyCryptoKeyObject *self){
    dap_enc_key_delete(self->key);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

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

// update struct dap_enc_key_t after insert foreign keys
PyObject* dap_enc_key_update_py(PyObject *self, PyObject *args){
    (void)args;
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

PyObject *wrapping_dap_enc_key_encrypt(PyObject *self, PyObject *args){
    PyObject *obj_in_data;
    if (!PyArg_ParseTuple(args, "O", &obj_in_data)){
        PyErr_SetString(PyExc_ValueError, "This function must accept one object type bytes");
        return NULL;
    }
    if (PyBytes_Check(obj_in_data)){
        void *l_in_data = PyBytes_AsString(obj_in_data);
        size_t l_in_data_size = PyBytes_Size(obj_in_data);
        void *l_out = NULL;
        dap_enc_key_t *tmp = ((PyCryptoKeyObject*)self)->key;
        size_t l_size_out = tmp->enc(tmp, l_in_data, l_in_data_size, l_out);
        if (l_size_out == 0){
            return Py_None;
        }
        PyObject *obj_out_data = PyBytes_FromStringAndSize(l_out, l_size_out);
        return obj_out_data;
    }
    if (PyUnicode_Check(obj_in_data)){
        const char *l_str = PyUnicode_AsUTF8(obj_in_data);
        void *l_out = NULL;
        dap_enc_key_t *tmp = ((PyCryptoKeyObject*)self)->key;
        size_t l_size_out = tmp->enc(tmp, l_str, strlen(l_str), l_out);
        if (l_size_out == 0){
            return Py_None;
        }
        PyObject *obj_out_data = PyBytes_FromStringAndSize(l_out, l_size_out);
        return obj_out_data;
    }
    PyErr_SetString(PyExc_ValueError, "This first argument is not a bytes or string object");
    return NULL;
}
PyObject *wrapping_dap_enc_key_decrypt(PyObject *self, PyObject *args){
    PyObject *obj_in_data;
    if (!PyArg_ParseTuple(args, "O", &obj_in_data)){
        PyErr_SetString(PyExc_ValueError, "This function must accept one object type bytes");
        return NULL;
    }
    if (!PyBytes_Check(obj_in_data)){
        PyErr_SetString(PyExc_ValueError, "This first argument is not a bytes object");
        return NULL;
    }
    void *l_in_data = PyBytes_AsString(obj_in_data);
    size_t l_in_data_size = PyBytes_Size(obj_in_data);
    void *l_out = NULL;
    dap_enc_key_t *tmp = ((PyCryptoKeyObject*)self)->key;
    size_t l_size_out = tmp->dec(tmp, l_in_data, l_in_data_size, l_out);
    if (l_size_out == 0){
        return Py_None;
    }
    PyObject *obj_out_data = PyBytes_FromStringAndSize(l_out, l_size_out);
    return obj_out_data;
}
