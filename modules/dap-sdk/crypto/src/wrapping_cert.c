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


PyObject* dap_cert_generate_py(PyObject *self, PyObject *args)
{
    const char *l_cert_name = NULL;
    const char * l_seed = NULL;

    const char *l_arg_cert_name = NULL;
    dap_enc_key_type_t l_arg_cert_key_type = DAP_ENC_KEY_TYPE_SIG_DILITHIUM;
    const char *l_arg_seed_string = NULL;

    if (!PyArg_ParseTuple(args, "sis", &l_arg_cert_name, &l_arg_cert_key_type, &l_arg_seed_string) ){
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

    PyCryptoCertObject *obj_cert = (PyCryptoCertObject*)_PyObject_New(&g_crypto_cert_type_py);
    obj_cert->cert = l_seed ? dap_cert_generate_mem_with_seed( l_cert_name, l_arg_cert_key_type, l_seed, strlen(l_seed) )
              :dap_cert_generate_mem( l_cert_name,l_arg_cert_key_type );
    return  Py_BuildValue("O", (PyObject*)obj_cert);
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

int dap_cert_init_py(void)
{
    return dap_cert_init();
}

void dap_cert_deinit_py(void)
{
    dap_cert_deinit();
}

