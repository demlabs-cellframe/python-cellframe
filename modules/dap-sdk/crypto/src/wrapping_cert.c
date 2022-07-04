/*
 * Authors:
 * Dmitriy A. Gearasimov <gerasimov.dmitriy@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://cellframe.net
 * Sources         https://gitlab.demlabs.net/cellframe
 * Copyright  (c) 2017-2020
 * All rights reserved.

 This file is part of DAP (Deus Applications Prototypes) the open source project

    DAP (Deus Applicaions Prototypes) is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DAP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with any DAP based project.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "dap_common.h"
#include "dap_cert.h"
#include "dap_strfuncs.h"

#include "wrapping_cert.h"
#include "libdap_crypto_key_type_python.h"
#define LOG_TAG "wrapping_cert"

PyMethodDef g_crypto_cert_methods_py[] = {
        {"generate",dap_cert_generate_py , METH_VARARGS | METH_STATIC, "Generate from seed or randomly the new certificate"},
        {"find", dap_cert_find_py, METH_VARARGS | METH_STATIC, ""},
        {"folderAdd", dap_cert_folder_add_py, METH_VARARGS | METH_STATIC, "Add folders with .dcert files in it"},
        {"folderGet", dap_cert_folder_get_py, METH_VARARGS | METH_STATIC, "Get folder by number or the default one"},
        {"load", dap_cert_load_py, METH_VARARGS | METH_STATIC ,""},
        {"dump", dap_cert_dump_py, METH_VARARGS , ""},
        {"pkey", dap_cert_pkey_py, METH_VARARGS , ""},
        {"sign", dap_cert_sign_py, METH_VARARGS , ""},
        {"certSignAdd", dap_cert_cert_sign_add_py, METH_VARARGS,  ""},
        {"certSigns", dap_cert_cert_signs_py, METH_VARARGS , ""},
        {"compare", dap_cert_compare_py, METH_VARARGS, ""},
        {"save", dap_cert_save_py, METH_VARARGS , "Save to the first directory in cert folders list"},
        {NULL, NULL, 0, NULL}
};

PyGetSetDef g_crypto_cert_getssets_py[] = {
        {"key", (getter)wrapping_cert_get_enc_key, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapCryptoCertObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.Crypto.Cert",             /* tp_name */
        sizeof(PyCryptoCertObject),         /* tp_basicsize */
        0,                         /* tp_itemsize */
        dap_cert_delete_py,                         /* tp_dealloc */
        0,                         /* tp_print */
        0,                         /* tp_getattr */
        0,                         /* tp_setattr */
        0,                         /* tp_reserved */
        0,                         /* tp_repr */
        0,                         /* tp_as_number */
        0,                         /* tp_as_sequence */
        0,                         /* tp_as_mapping */
        0,                         /* tp_hash  */
        0,                         /* tp_call */
        0,                         /* tp_str */
        0,                         /* tp_getattro */
        0,                         /* tp_setattro */
        0,                         /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
        "Crypto cert object",           /* tp_doc */
        0,		               /* tp_traverse */
        0,		               /* tp_clear */
        0,		               /* tp_richcompare */
        0,		               /* tp_weaklistoffset */
        0,		               /* tp_iter */
        0,		               /* tp_iternext */
        g_crypto_cert_methods_py,             /* tp_methods */
        0,                         /* tp_members */
        g_crypto_cert_getssets_py, /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        0,                         /* tp_init */
        0,                         /* tp_alloc */
        PyType_GenericNew,         /* tp_new */
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
};

PyObject* dap_cert_generate_py(PyObject *self, PyObject *args)
{
    const char *l_cert_name = NULL;
    const char * l_seed = NULL;

    const char *l_arg_cert_name = NULL;
    dap_enc_key_type_t l_arg_cert_key_type = DAP_ENC_KEY_TYPE_SIG_DILITHIUM;
    const char *l_arg_seed_string = NULL;

    if (!PyArg_ParseTuple(args, "s|is", &l_arg_cert_name, &l_arg_cert_key_type, &l_arg_seed_string) ){
        PyErr_SetString(PyExc_SyntaxError, "Wrong argument list");
        return NULL;
    }

    if (l_arg_cert_name != 0)
        l_cert_name = l_arg_cert_name;
    else {
        PyErr_SetString(PyExc_SyntaxError, "Certificate name is None");
        return NULL;
    }

    if (l_arg_seed_string != 0)
        l_seed = l_arg_seed_string;

    PyCryptoCertObject *obj_cert = PyObject_New(PyCryptoCertObject, &DapCryptoCertObjectType);
    obj_cert->cert = l_seed ? dap_cert_generate_mem_with_seed( l_cert_name, l_arg_cert_key_type, l_seed, strlen(l_seed) )
              :dap_cert_generate_mem( l_cert_name,l_arg_cert_key_type );
    if (!obj_cert->cert){
        Py_XDECREF(obj_cert);
        Py_RETURN_NONE;
    }
    return (PyObject*)obj_cert;
}

PyObject* dap_cert_dump_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject* dap_cert_pkey_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject* dap_cert_find_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject* dap_cert_sign_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject* dap_cert_cert_sign_add_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject* dap_cert_cert_signs_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject* dap_cert_compare_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject* dap_cert_save_py(PyObject *self, PyObject *args)
{
    (void) args;
    int res = dap_cert_save_to_folder(((PyCryptoCertObject*)self)->cert, dap_cert_get_folder(0) );
    return PyLong_FromLong(res);
}

PyObject* dap_cert_load_py(PyObject *self, PyObject *args)
{
    const char *l_cert_name;
    if (!PyArg_ParseTuple(args, "s", &l_cert_name)) {
        Py_RETURN_NONE;
    }
    dap_cert_t *l_ret = dap_cert_find_by_name(l_cert_name);
    if (!l_ret)
        return self;
    if (!self) {
        self = _PyObject_New(&DapCryptoCertObjectType);
    }
    ((PyCryptoCertObject *)self)->cert = l_ret;
    return self;
}

PyObject* dap_cert_close_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}


void dap_cert_delete_py(PyObject *self)
{
    PyCryptoCertObject *certObject = (PyCryptoCertObject *)self;
    dap_cert_delete( certObject->cert );
    Py_TYPE(certObject)->tp_free((PyObject*)certObject);
}


PyObject* dap_cert_folder_add_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject* dap_cert_folder_get_py(PyObject *self, PyObject *args)
{
    (void)self;
    const char *a_folder_path;
    if(!PyArg_ParseTuple(args, "s", &a_folder_path))
        return NULL;
    dap_cert_add_folder(a_folder_path);
    return PyLong_FromLong(0);
}

PyObject *wrapping_cert_get_enc_key(PyObject *self, void *closure){
    (void)closure;
    PyCryptoKeyObject *obj_key = PyObject_New(PyCryptoKeyObject, &PyCryptoKeyObjectType);
    obj_key->key = ((PyCryptoCertObject*)self)->cert->enc_key;
    return (PyObject*)obj_key;
}

int dap_cert_init_py(void)
{
    return dap_cert_init();
}

void dap_cert_deinit_py(void)
{
    dap_cert_deinit();
}

