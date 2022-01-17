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

#ifndef _WRAPPING_DAP_CHAIN_COMMON_
#define _WRAPPING_DAP_CHAIN_COMMON_
#include <Python.h>
#include "dap_chain_common.h"
#include "libdap_crypto_key_python.h"
#include "wrapping_dap_chain_ledger.h"
//#include "wrapping_dap_chain_ledger"

#ifdef __cplusplus
extern "C" {
#endif

/* Chain hash slow */
typedef struct PyDapChainHashSlow{
    PyObject_HEAD
    dap_chain_hash_slow_t *hash_slow;
}PyDapChainHashSlowObject;

PyObject *dap_chain_hash_slow_to_str_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainHashSlowMethod[] = {
    {"toStr", (PyCFunction)dap_chain_hash_slow_to_str_py, METH_VARARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainHashSlowObject_DapChainHashSlowObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.HashSlow",       /* tp_name */
    sizeof(PyDapChainHashSlowObject),/* tp_basicsize */
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
    "Chain hash slow object",        /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainHashSlowMethod,                               /* tp_methods */
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


/*=================*/

/* Chain addr */
typedef struct PyDapChainAddr{
    PyObject_HEAD
    dap_chain_addr_t *addr;
}PyDapChainAddrObject;

PyObject *dap_chain_addr_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_addr_from_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_addr_fill_py(PyObject *self, PyObject *args);
PyObject *dap_chain_addr_check_sum_py(PyObject *self, PyObject *args);

PyObject *dap_chain_addr_get_net_id_py(PyObject *self, PyObject *args);

PyObject *obj_addr_str(PyObject *self);

static PyMethodDef DapChainAddrMethods[] = {
    {"toStr", (PyCFunction)dap_chain_addr_to_str_py, METH_VARARGS, ""},
    {"fromStr", (PyCFunction)dap_chain_addr_from_str_py, METH_VARARGS | METH_STATIC, ""},
    {"fill", (PyCFunction)dap_chain_addr_fill_py, METH_VARARGS, ""},
    {"checkSum", (PyCFunction)dap_chain_addr_check_sum_py, METH_VARARGS, ""},
    {"getNetId", (PyCFunction)dap_chain_addr_get_net_id_py, METH_NOARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainAddrObject_DapChainAddrObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.Addr",       /* tp_name */
    sizeof(PyDapChainAddrObject),/* tp_basicsize */
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
    obj_addr_str,                               /* tp_str */
    0,                               /* tp_getattro */
    0,                               /* tp_setattro */
    0,                               /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
    "Chain addr object",             /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainAddrMethods,             /* tp_methods */
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


/*=================*/

/* Chain net id */

typedef struct PyDapChainNetId{
    PyObject_HEAD
    dap_chain_net_id_t net_id;
}PyDapChainNetIdObject;

PyObject *dap_chain_net_id_from_str_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainNetIdObjectMethods[] = {
    {"fromStr", (PyCFunction)dap_chain_net_id_from_str_py, METH_VARARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainNetIdObject_DapChainNetIdObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.NetID",          /* tp_name */
    sizeof(PyDapChainNetIdObject),   /* tp_basicsize */
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
    "Chain net id object",           /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainNetIdObjectMethods,      /* tp_methods */
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

/*=================*/

/* Chain net srv uid */

typedef struct PyDapChainNetSrvUID{
    PyObject_HEAD
    dap_chain_net_srv_uid_t net_srv_uid;
}PyDapChainNetSrvUIDObject;

PyObject *dap_chain_net_srv_uid_from_str_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainNetSrvUIDObject[] = {
    {"fromStr", (PyCFunction)dap_chain_net_srv_uid_from_str_py, METH_VARARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainNetSrvUIDObject_DapChainNetSrvUIDObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainNetSrvUID",          /* tp_name */
    sizeof(dap_chain_net_srv_uid_t),   /* tp_basicsize */
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
    "Chain net srv uid object",           /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainNetSrvUIDObject,         /* tp_methods */
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

/*=================*/

/* Chain net srv uid */
typedef struct PyDapChainNetSrvPriceUnitUID{
    PyObject_HEAD
    dap_chain_net_srv_price_unit_uid_t price_unit_uid;
}PyDapChainNetSrvPriceUnitUIDObject;

static PyTypeObject DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainNetSrvPriceUnitUID",             /* tp_name */
    sizeof(PyDapChainNetSrvPriceUnitUIDObject),      /* tp_basicsize */
    0,                                               /* tp_itemsize */
    0,                                               /* tp_dealloc */
    0,                                               /* tp_print */
    0,                                               /* tp_getattr */
    0,                                               /* tp_setattr */
    0,                                               /* tp_reserved */
    0,                                               /* tp_repr */
    0,                                               /* tp_as_number */
    0,                                               /* tp_as_sequence */
    0,                                               /* tp_as_mapping */
    0,                                               /* tp_hash  */
    0,                                               /* tp_call */
    0,                                               /* tp_str */
    0,                                               /* tp_getattro */
    0,                                               /* tp_setattro */
    0,                                               /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                         /* tp_flags */
    "Chain net srv price unit uid object",           /* tp_doc */
    0,		                                         /* tp_traverse */
    0,		                                         /* tp_clear */
    0,		                                         /* tp_richcompare */
    0,                                               /* tp_weaklistoffset */
    0,		                                         /* tp_iter */
    0,		                                         /* tp_iternext */
    0,                                               /* tp_methods */
    0,                                               /* tp_members */
    0,                                               /* tp_getset */
    0,                                               /* tp_base */
    0,                                               /* tp_dict */
    0,                                               /* tp_descr_get */
    0,                                               /* tp_descr_set */
    0,                                               /* tp_dictoffset */
    0,                                               /* tp_init */
    0,                                               /* tp_alloc */
    PyType_GenericNew,                               /* tp_new */
};
/*=================*/

/* Chain cell id */
typedef struct PyDapChainID{
    PyObject_HEAD
    dap_chain_id_t *chain_id;
}PyDapChainIDObject;

PyObject *DapChainIdObject_str(PyObject *self);

static PyTypeObject DapChainIDObject_DapChainIDType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainID"  ,       /* tp_name */
    sizeof(PyDapChainIDObject),  /* tp_basicsize */
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
    DapChainIdObject_str,                               /* tp_str */
    0,                               /* tp_getattro */
    0,                               /* tp_setattro */
    0,                               /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
    "Chain id object",          /* tp_doc */
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


/*=================*/

/* Chain cell id */
typedef struct PyDapChainCellID{
    PyObject_HEAD
    dap_chain_cell_id_t cell_id;
}PyDapChainCellIDObject;

static PyTypeObject DapChainCellIDObject_DapChainCellIDType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainCellID"  ,       /* tp_name */
    sizeof(PyDapChainCellIDObject),  /* tp_basicsize */
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
    "Chain cell id object",          /* tp_doc */
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


/*=================*/

/* Chain cell id */
typedef struct PyDapChainNodeAddr{
    PyObject_HEAD
    dap_chain_node_addr_t *node_addr;
}PyDapChainNodeAddrObject;

static PyTypeObject DapChainNodeAddrObject_DapChainNodeAddrObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNodeAddr"  ,       /* tp_name */
        sizeof(PyDapChainCellIDObject),  /* tp_basicsize */
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
        "Chain node addr object",          /* tp_doc */
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


/*=================*/

/* Chain cell id */
typedef struct PyDapChainHashSlowKind{
    PyObject_HEAD
    dap_chain_hash_slow_kind_t *slow_kind;
}PyDapChainHashSlowKindObject;

static PyTypeObject DapChainSlowKindObject_DapChainSlowKindType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainSlowKind"  ,       /* tp_name */
        sizeof(PyDapChainHashSlowKindObject),  /* tp_basicsize */
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
        "Chain slow kind object",          /* tp_doc */
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


/*=================*/

/**/
typedef struct PyDapChainCommon{
    PyObject_HEAD
}PyDapChainCommonObject;

PyObject * dap_chain_balance_to_coins_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainCommonMethodsDef[] = {
        {"balanceToCoins", (PyCFunction)dap_chain_balance_to_coins_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainCommonObject_DapChainCommonType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainCommon"  ,       /* tp_name */
        sizeof(PyDapChainCommonObject),  /* tp_basicsize */
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
        "Chain common object",          /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapChainCommonMethodsDef,         /* tp_methods */
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

/*=================*/


#ifdef __cplusplus
}
#endif
#endif //_WRAPPING_DAP_CHAIN_COMMON_
