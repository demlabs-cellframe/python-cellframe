/*
 * Authors:
 * Alexey Stratulat <alexey.stratulat@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://dap.net
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
#pragma once

#include "dap_enc_key.h"
#include "Python.h"
#include "dap_common.h"
#include "libdap_crypto_key_python.h"
#include "libdap_crypto_key_type_python.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyCryptoKey{
    PyObject_HEAD
    dap_enc_key_t *key;
    bool key_filled;
}PyCryptoKeyObject;

int wrapping_dap_enc_key_init(PyCryptoKeyObject *self, PyObject *args, PyObject *kwds);
void dap_enc_key_delete_py(PyCryptoKeyObject *self);

PyObject* dap_enc_key_get_enc_size_py(PyObject *self, PyObject *args);
PyObject* dap_enc_key_get_dec_size_py(PyObject *self, PyObject *args);

//PyObject *dap_enc_key_serealize_sign_py(PyObject *self, PyObject *args);

PyObject *dap_enc_key_serealize_priv_key_py(PyObject *self, PyObject *args);
PyObject *dap_enc_key_serealize_pub_key_py(PyObject *self, PyObject *args);
PyObject *dap_enc_key_deserealize_priv_key_py(PyObject *self, PyObject *args);
PyObject *dap_enc_key_deserealize_pub_key_py(PyObject *self, PyObject *args);

PyObject *dap_enc_key_dup_py(PyObject *self, PyObject *args);




// update struct dap_enc_key_t after insert foreign keys
PyObject* dap_enc_key_update_py(PyObject *self, PyObject *args);

// for asymmetric gen public key
PyObject *dap_enc_gen_pub_key_from_priv_py(PyObject *self, PyObject *args);


PyObject *dap_enc_gen_key_public_size_py(PyObject *self, PyObject *args);
PyObject *dap_enc_gen_key_public_py(PyObject *self, PyObject *args);

PyObject *wrapping_dap_enc_key_encrypt(PyObject *self, PyObject *args);
PyObject *wrapping_dap_enc_key_decrypt(PyObject *self, PyObject *args);

static PyMethodDef g_crypto_key_methods[]={
    {"getEncSize", dap_enc_key_get_enc_size_py, METH_VARARGS, ""},
    {"getDecSize", dap_enc_key_get_dec_size_py, METH_VARARGS, ""},
    {"serealizePrivKey", dap_enc_key_serealize_priv_key_py, METH_NOARGS, ""},
    {"serealizePubKey", dap_enc_key_serealize_pub_key_py, METH_NOARGS, ""},
    {"deserealizePrivKey", dap_enc_key_deserealize_priv_key_py, METH_VARARGS, ""},
    {"deserealizePubKey", dap_enc_key_deserealize_pub_key_py, METH_VARARGS, ""},
    {"clone", dap_enc_key_dup_py, METH_NOARGS, "This function clones the key"},
    {"update", dap_enc_key_update_py, METH_NOARGS, ""},
    {"genKeyPublicSize", dap_enc_gen_key_public_size_py, METH_VARARGS, ""},
    {"genPublic", dap_enc_gen_key_public_py, METH_VARARGS, ""},
    {"encrypt", wrapping_dap_enc_key_encrypt, METH_VARARGS, ""},
    {"decrypt", wrapping_dap_enc_key_decrypt, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

//static PyGetSetDef g_crypto_key_getset[]={
//    {NULL}
//};

static PyTypeObject CryptoKeyObjecy_CryptoKeyObjecyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "DAP.EncKey",           /* tp_name */
    sizeof(PyCryptoKeyObject),       /* tp_basicsize */
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
    "Crypto key objects",          /* tp_doc */
    0,                                   /* tp_traverse */
    0,		                         /* tp_clear */
    0,		               		 /* tp_richcompare */
    0,                                   /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    g_crypto_key_methods,        /* tp_methods */
    0,                                   /* tp_members */
    0,                                   /* tp_getset */
    0,                                   /* tp_base */
    0,                                   /* tp_dict */
    0,                                   /* tp_descr_get */
    0,                                   /* tp_descr_set */
    0,                                   /* tp_dictoffset */
    (initproc)wrapping_dap_enc_key_init, /* tp_init */
    0,                                   /* tp_alloc */
    PyType_GenericNew,                   /* tp_new */
};

#ifdef __cplusplus
}
#endif
