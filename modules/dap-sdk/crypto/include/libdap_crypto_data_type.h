/*
 * Authors:
 * Alexey Stratulat <alexey.stratulat@demlabs.net>
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
#include "Python.h"
#include "dap_enc_key.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyCryptoDataType{
    PyObject_HEAD
}PyCryptoDataTypeObjecy;

static PyObject *get_ENC_DATA_TYPE_RAW(){
    return PyLong_FromLong(DAP_ENC_DATA_TYPE_RAW);
}
static PyObject *get_ENC_DATA_TYPE_B64(){
    return PyLong_FromLong(DAP_ENC_DATA_TYPE_B64);
}
static PyObject *get_ENC_DATA_TYPE_B64_URLSAFE(){
    return PyLong_FromLong(DAP_ENC_DATA_TYPE_B64_URLSAFE);
}

static PyMethodDef PyCryptoDataTypeObjecyMethods[] = {
        {"DAP_ENC_DATA_TYPE_RAW", (PyCFunction)get_ENC_DATA_TYPE_RAW, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_DATA_TYPE_B64", (PyCFunction)get_ENC_DATA_TYPE_B64, METH_NOARGS | METH_STATIC, ""},
        {"DAP_ENC_DATA_TYPE_B64_URLSAFE", (PyCFunction)get_ENC_DATA_TYPE_B64_URLSAFE, METH_NOARGS | METH_STATIC, ""},

        {NULL, NULL, 0, NULL}
};

static PyTypeObject CryptoDataTypeObjecy_CryptoDataTypeObjecyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "DAP.CryptoDataType",      /* tp_name */
    sizeof(PyCryptoDataTypeObjecy),  /* tp_basicsize */
    0,                               /* tp_itemsize */
    0,                               /* tp_dealloc */
    0,                               /* tp_print */
    0,                               /* tp_getattr */
    0,                               /* tp_setattr */
    0,                               /* tp_reserved */
    0,                               /* tp_repr */
    0,                               /* tp_as_number */
    0,                               /* tp_as_sequence */
    0,                               /* tp_as_mapping */
    0,                               /* tp_hash  */
    0,                               /* tp_call */
    0,                               /* tp_str */
    0,                               /* tp_getattro */
    0,                               /* tp_setattro */
    0,                               /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
    "Crypto data type objects",      /* tp_doc */
    0,		                     /* tp_traverse */
    0,		                     /* tp_clear */
    0,		                     /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                     /* tp_iter */
    0,		                     /* tp_iternext */
    PyCryptoDataTypeObjecyMethods,   /* tp_methods */
    0,                               /* tp_members */
    0,                               /* tp_getset */
    0,                               /* tp_base */
    0,                               /* tp_dict */
    0,                               /* tp_descr_get */
    0,                               /* tp_descr_set */
    0,                               /* tp_dictoffset */
    0,                               /* tp_init */
    0,                               /* tp_alloc */
    PyType_GenericNew,               /* tp_new */
};

#ifdef __cplusplus
}
#endif
