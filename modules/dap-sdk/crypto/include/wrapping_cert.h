/*
 * Authors:
 * Dmitriy A. Gearasimov <gerasimov.dmitriy@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://dap.net
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
#pragma once
#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include "dap_common.h"
#include "dap_cert.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyCryptoCert{
    PyObject_HEAD;
    dap_cert_t * cert;
}PyCryptoCertObject;

int dap_cert_init_py();
void dap_cert_deinit_py();

PyObject* dap_cert_new_py(PyTypeObject *type_obj, PyObject *args, PyObject *kwds);
void dap_cert_delete_py(PyObject *self);
PyObject* dap_cert_delete_by_name_py(PyObject *self, PyObject *args);

PyObject* dap_cert_generate_py(PyObject *self, PyObject *args);
PyObject* dap_cert_generate_mem_py(PyObject *self, PyObject *args);
PyObject* dap_cert_generate_mem_with_seed_py(PyObject *self, PyObject *args);
PyObject* dap_cert_add_file_py(PyObject *self, PyObject *args);
PyObject* dap_cert_save_to_folder_py(PyObject *self, PyObject *args);
PyObject* dap_cert_dump_py(PyObject *self, PyObject *args);
PyObject* dap_cert_to_pkey_py(PyObject *self, PyObject *args);
PyObject* dap_cert_find_by_name_py(PyObject *self, PyObject *args);
PyObject* dap_cert_sign_py(PyObject *self, PyObject *args);
//PyObject* dap_cert_cert_sign_add_py(PyObject *self, PyObject *args);
//PyObject* dap_cert_cert_signs_py(PyObject *self, PyObject *args);
PyObject* dap_cert_compare_with_sign_py(PyObject *self, PyObject *args);
PyObject* dap_cert_sign_output_size_py(PyObject *self, PyObject *args);
PyObject* dap_cert_save_py(PyObject *self, PyObject *args);
//PyObject* dap_cert_load_py(PyObject *self, PyObject *args);
PyObject* dap_cert_folder_add_py(PyObject *self, PyObject *args);
PyObject* dap_cert_folder_get_py(PyObject *self, PyObject *args);

PyObject* dap_cert_add_cert_sign_py(PyObject *self, PyObject *args);
PyObject* dap_cert_count_cert_sign_py(PyObject *self, PyObject *args);

PyObject* dap_cert_encrypt_py(PyObject *self, PyObject *args);
PyObject* dap_cert_decrypt_py(PyObject *self, PyObject *args);

static PyMethodDef g_crypto_cert_methods_py[] = {
        {"generate",dap_cert_generate_py , METH_VARARGS | METH_STATIC, "Generate from seed or randomly the new certificate"},
        {"generateMEM", dap_cert_generate_mem_py, METH_VARARGS | METH_STATIC, ""},
        {"generateMEMWithSeed", dap_cert_generate_mem_with_seed_py, METH_VARARGS | METH_STATIC, ""},
        {"addFile", dap_cert_add_file_py, METH_VARARGS | METH_STATIC, ""},
        {"saveToFolder", dap_cert_save_to_folder_py, METH_VARARGS, ""},
        {"findByName", dap_cert_find_by_name_py, METH_VARARGS | METH_STATIC, ""},
        {"folderAdd", dap_cert_folder_add_py, METH_VARARGS | METH_STATIC, "Add folders with .dcert files in it"},
        {"folderGet", dap_cert_folder_get_py, METH_VARARGS | METH_STATIC, "Get folder by number or the default one"},
//        {"load", dap_cert_load_py, METH_VARARGS | METH_STATIC ,""},
        {"dump", dap_cert_dump_py, METH_NOARGS , ""},
        {"toPkey", dap_cert_to_pkey_py, METH_NOARGS , ""},
        {"sign", dap_cert_sign_py, METH_VARARGS , ""},
//        {"certSignAdd", dap_cert_cert_sign_add_py, METH_VARARGS,  ""},
//        {"certSigns", dap_cert_cert_signs_py, METH_VARARGS , ""},
        {"compareWithSign", dap_cert_compare_with_sign_py, METH_VARARGS, ""},
        {"signOutputSize", dap_cert_sign_output_size_py, METH_VARARGS, ""},
        {"save", dap_cert_save_py, METH_VARARGS , "Save to the first directory in cert folders list"},
        {"addCertSign", dap_cert_add_cert_sign_py, METH_VARARGS, ""},
        {"countCertSign", dap_cert_count_cert_sign_py, METH_NOARGS, ""},
        {"deleteByName", dap_cert_delete_by_name_py, METH_VARARGS | METH_STATIC, ""},
        {"encrypt", dap_cert_encrypt_py, METH_VARARGS, ""},
        {"decrypt", dap_cert_decrypt_py, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject g_crypto_cert_type_py = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "DAP.Cert",             /* tp_name */
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
        0,                         /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        0,                         /* tp_init */
        0,                         /* tp_alloc */
        (initproc)dap_cert_new_py,           /* tp_new */
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

static bool PyCryptoCertObject_Check(const PyObject *self){
    return PyObject_TypeCheck(self, &g_crypto_cert_type_py);
}

#ifdef __cplusplus
}
#endif
