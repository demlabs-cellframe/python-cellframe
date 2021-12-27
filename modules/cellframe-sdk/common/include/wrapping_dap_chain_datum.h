/*
 * Authors:
 * Alexey V. Stratulat <alexey.stratulat@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://cellframe.net
 * Sources         https://gitlab.demlabs.net/cellframe
 * Copyright  (c) 2017-2021
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

#ifndef _WRAPPING_DAP_CHAIN_DATUM_
#define _WRAPPING_DAP_CHAIN_DATUM_
#include "Python.h"
#include "dap_chain_datum.h"
#include "datetime.h"
#include "wrapping_dap_chain_datum_tx.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DAP Chain datum type id */
typedef struct PyDapChainDatumTypeId{
    PyObject_HEAD
    dap_chain_datum_typeid_t *type_id;
}PyDapChainDatumTypeIdObject;

static PyTypeObject DapChainDatumTypeIdObject_DapChainDatumTypeIdObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.DatumTypeId",          /* tp_name */
    sizeof(PyDapChainDatumTypeIdObject),   /* tp_basicsize */
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
    "Chain datum type id object",           /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    0,                               /* tp_methods */
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
/* -------------------------------- */

typedef struct PyDapChainDatum{
    PyObject_HEAD
    dap_chain_datum_t *datum;
}PyDapChainDatumObject;

//void PyDapChainDatumObject_dealloc(PyDapChainDatumObject* object);
PyObject *PyDapChainDatumObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds);
PyObject *dap_chain_datum_size_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_get_ts_created(PyObject *self, void* closure);
PyObject *dap_chain_datum_is_type_tx(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_datum_get_datum_tx(PyObject *self, PyObject *args);

static PyMethodDef DapChainDatumMethods[] = {
    {"size", dap_chain_datum_size_py, METH_VARARGS, ""},
    {"isDatumTX", dap_chain_datum_is_type_tx, METH_NOARGS, ""},
    {"getDatumTX", wrapping_dap_chain_datum_get_datum_tx, METH_NOARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyGetSetDef  DapChainDatumGetSet[] = {
        {"tsCreated", (getter)dap_chain_datum_get_ts_created, NULL, NULL, NULL},
        {NULL}
};

static PyTypeObject DapChainDatumObject_DapChainDatumObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.Datum",          /* tp_name */
    sizeof(PyDapChainDatumObject),   /* tp_basicsize */
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
    "Chain datum object",            /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainDatumMethods,                               /* tp_methods */
    0,                               /* tp_members */
    DapChainDatumGetSet,                               /* tp_getset */
    0,                               /* tp_base */
    0,                               /* tp_dict */
    0,                               /* tp_descr_get */
    0,                               /* tp_descr_set */
    0,                               /* tp_dictoffset */
    0,                               /* tp_init */
    0,                               /* tp_alloc */
    PyDapChainDatumObject_new,       /* tp_new */
};
/* -------------------------------- */

/* DAP Chain datum iter*/
typedef struct PyDapChainDatumIter{
    PyObject_HEAD
    dap_chain_datum_iter_t *datum_iter;
}PyDapChainDatumIterObject;

static PyTypeObject DapChainDatumIterObject_DapChainDatumIterObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.DatumIter",          /* tp_name */
    sizeof(PyDapChainDatumIterObject),   /* tp_basicsize */
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
    "Chain datum iter object",           /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    0,                               /* tp_methods */
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
/* -------------------------------- */

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_DATUM_
