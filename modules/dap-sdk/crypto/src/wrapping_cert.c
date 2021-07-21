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
#include "wrapping_dap_pkey.h"
#include "wrapping_dap_sign.h"
#define LOG_TAG "wrapping_cert"


PyObject* dap_cert_new_py(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    const char *l_cert_name = NULL;
    if (!PyArg_ParseTuple(args, "s", &l_cert_name)) {
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    PyCryptoCertObject *obj = (PyCryptoCertObject*)PyType_GenericNew(type_object, args, kwds);
    obj->cert = dap_cert_new(l_cert_name);
    return (PyObject *)obj;
}

void dap_cert_delete_py(PyObject *self)
{
    PyCryptoCertObject *certObject = (PyCryptoCertObject *)self;
    dap_cert_delete( certObject->cert );
    Py_TYPE(certObject)->tp_free((PyObject*)certObject);
}

PyObject* dap_cert_generate_py(PyObject *self, PyObject *args)
{
    const char *l_cert_name = NULL;
    const char * l_seed = NULL;

    const char *l_arg_cert_name = NULL;
    dap_enc_key_type_t l_arg_cert_key_type = DAP_ENC_KEY_TYPE_SIG_DILITHIUM;
    const char *l_arg_seed_string = NULL;

    if (!PyArg_ParseTuple(args, "sis", &l_arg_cert_name, &l_arg_cert_key_type, &l_arg_seed_string) ){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }

    if (l_arg_cert_name != 0)
        l_cert_name = l_arg_cert_name;
    else {
        PyErr_SetString(PyExc_SyntaxError, "Certificate name is NULL");
        return NULL;
    }

    if (l_arg_seed_string != 0)
        l_seed = l_arg_seed_string;

    PyCryptoCertObject *obj_cert = (PyCryptoCertObject*)_PyObject_New(&g_crypto_cert_type_py);
    obj_cert->cert = l_seed ? dap_cert_generate_mem_with_seed( l_cert_name, l_arg_cert_key_type, l_seed, strlen(l_seed) )
              :dap_cert_generate_mem( l_cert_name,l_arg_cert_key_type );
    return  Py_BuildValue("O", (PyObject*)obj_cert);
}

PyObject* dap_cert_dump_py(PyObject *self, PyObject *args)
{
    (void) args;
    dap_cert_dump(((PyCryptoCertObject*)self)->cert);
    return Py_BuildValue("O", Py_None);
}

PyObject* dap_cert_to_pkey_py(PyObject *self, PyObject *args)
{
    (void) args;
    PyObject *l_obj = _PyObject_New(&DapPkeyObject_DapPkeyObjectType);
    ((PyDapPkeyObject*)l_obj)->pkey = dap_cert_to_pkey(((PyCryptoCertObject*)self)->cert);
    return Py_BuildValue("O", l_obj);
}

PyObject* dap_cert_find_by_name_py(PyObject *self, PyObject *args)
{
    (void) self;
    const char *l_name = NULL;
    if (!PyArg_ParseTuple(args, "s", l_name)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    dap_cert_t *l_tmp_cert = dap_cert_find_by_name(l_name);
    if (l_tmp_cert) {
        PyObject *l_obj_cert = _PyObject_New(&g_crypto_cert_type_py);
        ((PyCryptoCertObject*)l_obj_cert)->cert = l_tmp_cert;
        return Py_BuildValue("O", l_obj_cert);
    } else {
        return Py_None;
    }
}

PyObject* dap_cert_sign_py(PyObject *self, PyObject *args)
{
    PyObject *l_data_byte;
    size_t l_data_size;
    size_t l_output_size_wished;
    if (!PyArg_ParseTuple(args, "On", &l_data_byte, &l_output_size_wished)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyBytes_Check(l_data_byte)){
        PyErr_SetString(PyExc_BytesWarning, "The first agrument the is not a bytes object");
        return NULL;
    }
    l_data_size = (size_t)PyBytes_Size(l_data_byte);
    void *l_data = PyBytes_AsString(l_data_byte);
    dap_sign_t *l_sign = dap_cert_sign(((PyCryptoCertObject*)self)->cert, l_data, l_data_size, l_output_size_wished);
    if(l_sign) {
        PyObject *l_obj_sign = _PyObject_New(&DapSignObject_DapSignObjectType);
        ((PyDapSignObject*)l_obj_sign)->sign = l_sign;
        return Py_BuildValue("O", l_obj_sign);
    } else {
        return Py_None;
    }
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

PyObject* dap_cert_compare_with_sign_py(PyObject *self, PyObject *args)
{
    PyObject *l_obj_sign = NULL;
    if (!PyArg_ParseTuple(args, "O", &l_obj_sign)){
        PyErr_SetString(PyExc_BytesWarning, "The first agrument the is not a bytes object");
        return NULL;
    }
    int res = dap_cert_compare_with_sign(((PyCryptoCertObject*)self)->cert, ((PyDapSignObject*)l_obj_sign)->sign);
    return Py_BuildValue("i", res);
}

PyObject* dap_cert_save_py(PyObject *self, PyObject *args)
{
    (void) args;
    int res = dap_cert_save_to_folder(((PyCryptoCertObject*)self)->cert, dap_cert_get_folder(0) );
    return PyLong_FromLong(res);
}

PyObject* dap_cert_load_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject* dap_cert_close_py(PyObject *self, PyObject *args)
{
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}




PyObject* dap_cert_folder_add_py(PyObject *self, PyObject *args)
{
    (void) self;
    const char *l_path_folder = NULL;
    if (!PyArg_ParseTuple(args, "s", &l_path_folder)){
        PyErr_SetString(PyExc_BytesWarning, "The first agrument the is not a bytes object");
        return NULL;
    }
    dap_cert_add_folder(l_path_folder);
    return Py_None;
}

PyObject* dap_cert_folder_get_py(PyObject *self, PyObject *args)
{
    (void)self;
    int l_n_folder_path;
    if (!PyArg_ParseTuple(args, "i", &l_n_folder_path)){
        PyErr_SetString(PyExc_BytesWarning, "The first agrument the is not a bytes object");
        return NULL;
    }
    const char *l_path_folder = dap_cert_get_folder(l_n_folder_path);
    return Py_BuildValue("s", l_path_folder);
}

int dap_cert_init_py(void)
{
    return dap_cert_init();
}

void dap_cert_deinit_py(void)
{
    dap_cert_deinit();
}

