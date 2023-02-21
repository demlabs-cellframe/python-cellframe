#include "libdap_chain_net_python.h"
#include "dap_chain_net_tx.h"

PyMethodDef DapChainNetMethods[] = {
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
        {"getNets", dap_chain_get_nets_py, METH_NOARGS | METH_STATIC, ""},
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
        {"getTxByHash", dap_chain_net_get_tx_by_hash_py, METH_VARARGS, ""},
        {"addNotify", (PyCFunction)dap_chain_net_add_notify_py, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyGetSetDef DapChainNetGetsSetsDef[] = {
        {"id", (getter)dap_chain_net_python_get_id, NULL, NULL, NULL},
        {"chains", (getter)dap_chain_net_python_get_chains, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainNetObjectType = {
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
        PyDapChainNet_str,               /* tp_str */
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

int dap_chain_net_init_py(void){
    int res = dap_chain_net_init();
    return res;
}
void dap_chain_net_deinit_py(void){
    dap_chain_net_deinit();
}

PyObject* PyDapChainNet_str(PyObject *self){
    return Py_BuildValue("s", ((PyDapChainNetObject*)self)->chain_net->pub.name);
}

PyObject *dap_chain_net_load_all_py(PyObject *self, PyObject *args){
    dap_chain_net_load_all();
    return PyLong_FromLong(0);
}
PyObject *dap_chain_net_state_go_to_py(PyObject *self, PyObject *args){
    PyObject *obj_net_state;
    if (!PyArg_ParseTuple(args, "O", &obj_net_state))
        return NULL;
    int res = dap_chain_net_state_go_to(((PyDapChainNetObject*)self)->chain_net, ((PyDapChainNetStateObject*)obj_net_state)->state);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_net_start_py(PyObject *self, PyObject *args){
    int res = dap_chain_net_start(((PyDapChainNetObject*)self)->chain_net);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_net_stop_py(PyObject *self, PyObject *args){
    int res = dap_chain_net_stop(((PyDapChainNetObject*)self)->chain_net);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_net_links_establish_py(PyObject *self, PyObject *args){
    int res = dap_chain_net_links_establish(((PyDapChainNetObject*)self)->chain_net);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_net_sync_chains_py(PyObject *self, PyObject *args){
    int res = dap_chain_net_sync_chains(((PyDapChainNetObject*)self)->chain_net);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_net_sync_gdb_py(PyObject *self, PyObject *args){
    int res = dap_chain_net_sync_gdb(((PyDapChainNetObject*)self)->chain_net);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_net_sync_all_py(PyObject *self, PyObject *args){
    int res = dap_chain_net_sync_all(((PyDapChainNetObject*)self)->chain_net);
    return PyLong_FromLong(res);
}

PyObject *dap_chain_net_proc_datapool_py(PyObject *self, PyObject *args){
    //dap_chain_net_proc_datapool(((PyDapChainNetObject*)self)->chain_net);
    //return PyLong_FromLong(0);
    return NULL;
}

PyObject *dap_chain_net_by_name_py(PyObject *self, PyObject *args){
    const char *a_name;
    if (!PyArg_ParseTuple(args, "s", &a_name)) {
        PyErr_SetString(PyExc_AttributeError,
                        "Invalid argument specified. The first argument for this function must be a string. ");
        return NULL;
    }
    PyDapChainNetObject *obj_chain_net = PyObject_New(PyDapChainNetObject, &DapChainNetObjectType);
    ((PyDapChainNetObject*)obj_chain_net)->chain_net = dap_chain_net_by_name(a_name);
    if (((PyDapChainNetObject*)obj_chain_net)->chain_net == NULL){
        PyObject_Del(obj_chain_net);
        Py_RETURN_NONE;
    }
    return Py_BuildValue("O", obj_chain_net);
}
PyObject *dap_chain_get_nets_py(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    uint16_t l_net_count = 0;
    dap_chain_net_t **l_nets = dap_chain_net_list(&l_net_count);
    PyObject *obj_nets = PyList_New(l_net_count);
    for (uint16_t i = 0; i < l_net_count; i++) {
        PyDapChainNetObject *l_obj_net = PyObject_New(PyDapChainNetObject, &DapChainNetObjectType);
        l_obj_net->chain_net = l_nets[i];
        PyList_SetItem(obj_nets, i, (PyObject*)l_obj_net);
    }
    return obj_nets;
}
PyObject *dap_chain_net_by_id_py(PyObject *self, PyObject *args){
    PyObject *obj_net_id;
    if (!PyArg_ParseTuple(args, "O", &obj_net_id))
        return NULL;
    PyDapChainNetObject *obj_net = PyObject_New(PyDapChainNetObject, &DapChainNetObjectType);
    ((PyDapChainNetObject*)obj_net)->chain_net = dap_chain_net_by_id(((PyDapChainNetIdObject*)obj_net_id)->net_id);
    return (PyObject*)obj_net;
}
PyObject *dap_chain_net_id_by_name_py(PyObject *self, PyObject *args){
    const char *name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    PyObject *obj_net_id = _PyObject_New(&DapChainNetIdObjectType);
    ((PyDapChainNetIdObject*)obj_net_id)->net_id = dap_chain_net_id_by_name(name);
    return Py_BuildValue("O", obj_net_id);
}
PyObject *dap_chain_ledger_by_net_name_py(PyObject *self, PyObject *args){
    const char *net_name;
    if (!PyArg_ParseTuple(args, "s", &net_name))
        return NULL;
    PyObject *obj_ledger = _PyObject_New(&DapChainLedgerObjectType);
    ((PyDapChainLedgerObject*)obj_ledger)->ledger = dap_chain_ledger_by_net_name(net_name);
    return Py_BuildValue("O", obj_ledger);
}

PyObject *dap_chain_net_get_chain_by_name_py(PyObject *self, PyObject *args){
    const char* chain_name;
    if (!PyArg_ParseTuple(args, "s", &chain_name)) {
        PyErr_SetString(PyExc_AttributeError, "Function takes exactly one argument, which must be a string.");
        return NULL;
    }
    PyDapChainObject *obj_chain = PyObject_New(PyDapChainObject, &DapChainObjectType);
    ((PyDapChainObject*)obj_chain)->chain_t = dap_chain_net_get_chain_by_name(((PyDapChainNetObject*)self)->chain_net, chain_name);
    if (!((PyDapChainObject*)obj_chain)->chain_t){
        Py_XDECREF(obj_chain);
        Py_RETURN_NONE;
    }
    return Py_BuildValue("O", obj_chain);
}

PyObject *dap_chain_net_python_get_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetIdObject *obj_net_id = PyObject_New(PyDapChainNetIdObject, &DapChainNetIdObjectType);
    obj_net_id->net_id = ((PyDapChainNetObject*)self)->chain_net->pub.id;
    return (PyObject*)obj_net_id;
}

PyObject *dap_chain_net_python_get_chains(PyObject *self, void *closure){
    (void)closure;
    dap_chain_t *l_chain = NULL;
    PyObject *obj_list = PyList_New(0);
    DL_FOREACH(((PyDapChainNetObject*)self)->chain_net->pub.chains, l_chain) {
        PyDapChainObject *obj_chain = PyObject_New(PyDapChainObject, &DapChainObjectType);
        obj_chain->chain_t = l_chain;
        PyList_Append(obj_list, (PyObject*)obj_chain);
    }
    return obj_list;
}

PyObject *dap_chain_net_get_cur_addr_py(PyObject *self, PyObject *args){
    PyObject *obj_node_addr = _PyObject_New(&DapChainNodeAddrObjectType);
    ((PyDapChainNodeAddrObject*)obj_node_addr)->node_addr = dap_chain_net_get_cur_addr(((PyDapChainNetObject*)self)->chain_net);
    return Py_BuildValue("O", obj_node_addr);
}
PyObject *dap_chain_net_get_cur_cell_py(PyObject *self, PyObject *args){
    PyObject *obj_cell_id = _PyObject_New(&DapChainNodeAddrObjectType);
    ((PyDapChainCellIDObject*)obj_cell_id)->cell_id = *(dap_chain_net_get_cur_cell(((PyDapChainNetObject*)self)->chain_net));
    return Py_BuildValue("O", obj_cell_id);
}
PyObject *dap_chain_net_get_cur_addr_int_py(PyObject *self, PyObject *args){
    uint64_t res = dap_chain_net_get_cur_addr_int(((PyDapChainNetObject*)self)->chain_net);
    return PyLong_FromUnsignedLongLong(res);
}

PyObject *dap_chain_net_get_gdb_group_mempool_py(PyObject *self, PyObject *args){
    PyObject *obj_chain;
    if (!PyArg_ParseTuple(args, "O", &obj_chain))
        return NULL;
    char *res = dap_chain_net_get_gdb_group_mempool(((PyDapChainObject*)obj_chain)->chain_t);
    if (!res)
        Py_RETURN_NONE;
    PyObject *l_obj_res = Py_BuildValue("s", res);
    DAP_DELETE(res);
    return l_obj_res;
}
PyObject *dap_chain_net_get_gdb_group_mempool_by_chain_type_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_type;
    if (!PyArg_ParseTuple(args, "O", &obj_chain_type))
        return NULL;
    char *res = dap_chain_net_get_gdb_group_mempool_by_chain_type(((PyDapChainNetObject*)self)->chain_net,
                                                                  ((PyChainTypeObject*)obj_chain_type)->chain_type);
    if (!res)
        Py_RETURN_NONE;
    PyObject *l_obj_res = Py_BuildValue("s", res);
    DAP_DELETE(res);
    return l_obj_res;
}
PyObject *dap_chain_net_links_connect_py(PyObject *self, PyObject *args){
//    dap_chain_net_links_connect(((PyDapChainNetObject*)self)->chain_net);
//    PyLong_FromLong(0);
    return NULL;
}
PyObject *dap_chain_net_get_chain_by_chain_type_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_type;
    if(!PyArg_ParseTuple(args, "O", &obj_chain_type))
        return NULL;
    PyObject *obj_chain = _PyObject_New(&DapChainObjectType);
    ((PyDapChainObject*)obj_chain)->chain_t = dap_chain_net_get_chain_by_chain_type(
                ((PyDapChainNetObject*)self)->chain_net,
                ((PyChainTypeObject*)obj_chain_type)->chain_type);
    return Py_BuildValue("O", obj_chain);
}

PyObject *dap_chain_net_get_ledger_py(PyObject *self, PyObject *args){
    (void)args;
    PyDapChainLedgerObject *obj_ledger = PyObject_New(PyDapChainLedgerObject, &DapChainLedgerObjectType);
    obj_ledger->ledger = ((PyDapChainNetObject*)self)->chain_net->pub.ledger;
    return (PyObject*)obj_ledger;
}

PyObject *dap_chain_net_get_name_py(PyObject *self, PyObject *args){
    (void)args;
    PyObject *obj_name = PyUnicode_FromString(((PyDapChainNetObject*)self)->chain_net->pub.name);
    return obj_name;
}

PyObject *dap_chain_net_get_tx_by_hash_py(PyObject *self, PyObject *args){
    PyDapHashFastObject *obj_hash;
    if (!PyArg_ParseTuple(args, "O", &obj_hash)){
        return NULL;
    }
    if (!PyDapHashFast_Check(obj_hash)){
        return NULL;
    }
    PyDapChainDatumTxObject *l_tx = PyObject_New(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
    l_tx->datum_tx = dap_chain_net_get_tx_by_hash(
            ((PyDapChainNetObject*)self)->chain_net,
            obj_hash->hash_fast,
            TX_SEARCH_TYPE_NET);
    if (l_tx->datum_tx == NULL){
        Py_TYPE(l_tx)->tp_free((PyObject*)l_tx);
        Py_RETURN_NONE;
    }
    l_tx->original = false;
    return (PyObject*)l_tx;
}

typedef struct _wrapping_dap_chain_net_notify_callback{
    PyObject *arg;
    PyObject *func;
}_wrapping_dap_chain_net_notify_callback_t;

void pvt_dap_chain_net_py_notify_handler(void * a_arg, const char a_op_code, const char * a_group,
                                         const char * a_key, const void * a_value, const size_t a_value_len){
    if (!a_arg)
        return;
    _wrapping_dap_chain_net_notify_callback_t *l_callback = (_wrapping_dap_chain_net_notify_callback_t *)a_arg;
    PyObject *l_obj_value = NULL;
    char l_op_code[2];
    l_op_code[0] = a_op_code;
    l_op_code[1] = '\0';
    PyGILState_STATE state = PyGILState_Ensure();
    if (a_value == NULL || a_value_len == 0){
        l_obj_value = Py_None;
    } else {
        l_obj_value = PyBytes_FromStringAndSize(a_value, (Py_ssize_t)a_value_len);
    }
    PyObject *argv = Py_BuildValue("sssOO", l_op_code, a_group, a_key, l_obj_value, l_callback->arg);
    PyEval_CallObject(l_callback->func, argv);
    Py_DECREF(argv);
    PyGILState_Release(state);
}

PyObject *dap_chain_net_add_notify_py(PyObject *self, PyObject *args){
    PyObject *obj_func = NULL, *obj_arg;
    if (!PyArg_ParseTuple(args, "OO", &obj_func, &obj_arg)){
        return NULL;
    }
    if (!PyCallable_Check(obj_func)){
        PyErr_SetString(PyExc_AttributeError, "Argument must be callable");
        return NULL;
    }
    _wrapping_dap_chain_net_notify_callback_t *l_callback = DAP_NEW(_wrapping_dap_chain_net_notify_callback_t);
    l_callback->func = obj_func;
    l_callback->arg = obj_arg;
    Py_INCREF(obj_func);
    Py_INCREF(obj_arg);
    dap_chain_net_add_gdb_notify_callback(((PyDapChainNetObject*)self)->chain_net, pvt_dap_chain_net_py_notify_handler, l_callback);
    Py_RETURN_NONE;
}
