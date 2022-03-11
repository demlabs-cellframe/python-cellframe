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

#ifndef _WRAPPING_DAP_NET_PYTHON_
#define _WRAPPING_DAP_NET_PYTHON_

#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "dap_chain_net.h"
#include "wrapping_dap_chain_net_state.h"
//#include "wrapping_dap_chain_net_state.h"
#include "wrapping_dap_chain_common.h"
#include "wrapping_dap_chain_ledger.h"
#include "libdap-chain-python.h"
#include "libdap_chain_type_python.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapChainNet{
    PyObject_HEAD
    dap_chain_net_t *chain_net;
}PyDapChainNetObject;

int dap_chain_net_init_py(void);
void dap_chain_net_deinit_py(void);

PyObject *dap_chain_net_load_all_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_state_go_to_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_start_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_stop_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_links_establish_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_sync_chains_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_sync_gdb_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_sync_all_py(PyObject *self, PyObject *args);

PyObject *dap_chain_net_proc_datapool_py(PyObject *self, PyObject *args);

PyObject *dap_chain_net_by_name_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_by_id_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_id_by_name_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_by_net_name_py(PyObject *self, PyObject *args);

PyObject *dap_chain_net_get_chain_by_name_py(PyObject *self, PyObject *args);

PyObject *dap_chain_net_get_cur_addr_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_get_cur_cell_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_get_cur_addr_int_py(PyObject *self, PyObject *args);

PyObject *dap_chain_net_get_gdb_group_mempool_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_get_gdb_group_mempool_by_chain_type_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_links_connect_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_get_chain_by_chain_type_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_get_ledger_py(PyObject *self, PyObject *args);
PyObject *dap_chain_net_get_name_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainNetMethods[] = {
    {"loadAll", dap_chain_net_load_all_py, METH_NOARGS | METH_STATIC, ""},
    {"stateGoTo", dap_chain_net_state_go_to_py, METH_VARARGS, ""},
    {"start", dap_chain_net_start_py, METH_VARARGS, ""},
    {"stop", dap_chain_net_stop_py, METH_VARARGS, ""},
    {"linksEstablish", dap_chain_net_links_establish_py, METH_VARARGS, ""},
    {"syncChains", dap_chain_net_sync_all_py, METH_VARARGS, ""},
    {"syncGdb", dap_chain_net_sync_gdb_py, METH_VARARGS, ""},
    {"syncAll", dap_chain_net_sync_all_py, METH_VARARGS, ""},
    {"procDatapool", dap_chain_net_proc_datapool_py, METH_VARARGS, ""},
    {"byName", dap_chain_net_by_name_py, METH_VARARGS | METH_STATIC, ""},
    {"byId", dap_chain_net_by_id_py, METH_VARARGS | METH_STATIC, ""},
    {"idByName", dap_chain_net_id_by_name_py, METH_VARARGS | METH_STATIC, ""},
    {"ledgerByNetName", dap_chain_ledger_by_net_name_py, METH_VARARGS | METH_STATIC, ""},
    {"getChainByName", dap_chain_net_get_chain_by_name_py, METH_VARARGS, ""},
    {"getCurAddr", dap_chain_net_get_cur_addr_py, METH_VARARGS, ""},
    {"getCurCell", dap_chain_net_get_cur_cell_py, METH_VARARGS, ""},
    {"getGdbGroupMempool", dap_chain_net_get_gdb_group_mempool_py, METH_VARARGS | METH_STATIC, ""},
    {"getGdbGroupMempoolByChainType", dap_chain_net_get_gdb_group_mempool_by_chain_type_py, METH_VARARGS, ""},
    {"linksConnect", dap_chain_net_links_connect_py, METH_VARARGS, ""},
    {"getChainByChainType", dap_chain_net_get_chain_by_chain_type_py, METH_VARARGS, ""},
    {"getLedger", dap_chain_net_get_ledger_py, METH_NOARGS, ""},
    {"getName", dap_chain_net_get_name_py, METH_NOARGS, ""},
    {NULL, NULL, 0, NULL}
};

PyObject *dap_chain_net_python_get_id(PyObject *self, void *closure);

static PyGetSetDef DapChainNetGetsSetsDef[] = {
        {"id", (getter)dap_chain_net_python_get_id, NULL, NULL, NULL},
    {NULL}
};

static PyTypeObject DapChainNetObject_DapChainNetObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainNet",            /* tp_name */
    sizeof(PyDapChainNetObject),     /* tp_basicsize */
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
    "Chain net object",              /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainNetMethods,              /* tp_methods */
    0,                               /* tp_members */
    DapChainNetGetsSetsDef,                               /* tp_getset */
    0,                               /* tp_base */
    0,                               /* tp_dict */
    0,                               /* tp_descr_get */
    0,                               /* tp_descr_set */
    0,                               /* tp_dictoffset */
    0,                               /* tp_init */
    0,                               /* tp_alloc */
    PyType_GenericNew,               /* tp_new */
};

static bool PyDapChainNet_Check(PyDapChainNetObject *pyNet)
{
    return !PyObject_TypeCheck(pyNet, &DapChainNetObject_DapChainNetObjectType);
}

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_NET_PYTHON_
