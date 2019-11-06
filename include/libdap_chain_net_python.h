#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "dap_chain_net.h"
#include "wrapping_dap_chain_net_state.h"

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
    /*{"csAdd", (PyCFunction)dap_chain_cs_add_py, METH_VARARGS, ""},
    {"csCreate", (PyCFunction)dap_chain_cs_create_py, METH_VARARGS, ""},
    {"classAdd", (PyCFunction)dap_chain_class_add_py, METH_VARARGS, ""},
    {"classCreate", (PyCFunction)dap_chain_class_create_py, METH_VARARGS, ""},*/
    {NULL, NULL, 0, NULL}
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
