#include "libdap_chain_net_python.h"

int dap_chain_net_init_py(void){
    int res = dap_chain_net_init();
    return res;
}
void dap_chain_net_deinit_py(void){
    dap_chain_net_deinit();
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
    if (!PyArg_ParseTuple(args, "s", &a_name))
        return NULL;
    PyObject *obj_chain_net = _PyObject_New(&DapChainNetObject_DapChainNetObjectType);
    ((PyDapChainNetObject*)obj_chain_net)->chain_net = dap_chain_net_by_name(a_name);
    return Py_BuildValue("O", obj_chain_net);
}
PyObject *dap_chain_net_by_id_py(PyObject *self, PyObject *args){
    PyObject *obj_net_id;
    if (!PyArg_ParseTuple(args, "O", &obj_net_id))
        return NULL;
    PyObject *obj_net = _PyObject_New(&DapChainNetObject_DapChainNetObjectType);
    ((PyDapChainNetObject*)obj_net)->chain_net = dap_chain_net_by_id(((PyDapChainNetIdObject*)obj_net_id)->net_id);
    return Py_BuildValue("O", obj_net);
}
PyObject *dap_chain_net_id_by_name_py(PyObject *self, PyObject *args){
    const char *name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    PyObject *obj_net_id = _PyObject_New(&DapChainNetIdObject_DapChainNetIdObjectType);
    ((PyDapChainNetIdObject*)obj_net_id)->net_id = dap_chain_net_id_by_name(name);
    return Py_BuildValue("O", obj_net_id);
}
PyObject *dap_chain_ledger_by_net_name_py(PyObject *self, PyObject *args){
    const char *net_name;
    if (!PyArg_ParseTuple(args, "s", &net_name))
        return NULL;
    PyObject *obj_ledger = _PyObject_New(&DapChainLedger_DapChainLedgerType);
    ((PyDapChainLedgerObject*)obj_ledger)->ledger = dap_chain_ledger_by_net_name(net_name);
    return Py_BuildValue("O", obj_ledger);
}

PyObject *dap_chain_net_get_chain_by_name_py(PyObject *self, PyObject *args){
    const char* chain_name;
    if (!PyArg_ParseTuple(args, "s", &chain_name))
        return NULL;
      PyObject *obj_chain = _PyObject_New(&dapChainObject_dapChainType);
      ((PyDapChainObject*)obj_chain)->chain_t = dap_chain_net_get_chain_by_name(((PyDapChainNetObject*)self)->chain_net, chain_name);
      return Py_BuildValue("O", obj_chain);
}

PyObject *dap_chain_net_get_cur_addr_py(PyObject *self, PyObject *args){
    PyObject *obj_node_addr = _PyObject_New(&DapChainNodeAddrObject_DapChainNodeAddrObjectType);
    ((PyDapChainNodeAddrObject*)obj_node_addr)->node_addr = dap_chain_net_get_cur_addr(((PyDapChainNetObject*)self)->chain_net);
    return Py_BuildValue("O", obj_node_addr);
}
PyObject *dap_chain_net_get_cur_cell_py(PyObject *self, PyObject *args){
    PyObject *obj_cell_id = _PyObject_New(&DapChainNodeAddrObject_DapChainNodeAddrObjectType);
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
    return Py_BuildValue("s", res);
}
PyObject *dap_chain_net_get_gdb_group_mempool_by_chain_type_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_type;
    if (!PyArg_ParseTuple(args, "O", &obj_chain_type))
        return NULL;
    char *res = dap_chain_net_get_gdb_group_mempool_by_chain_type(((PyDapChainNetObject*)self)->chain_net,
                                                                  ((PyChainTypeObject*)obj_chain_type)->chain_type);
    return Py_BuildValue("s", res);

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
    PyObject *obj_chain = _PyObject_New(&dapChainObject_dapChainType);
    ((PyDapChainObject*)obj_chain)->chain_t = dap_chain_net_get_chain_by_chain_type(
                ((PyDapChainNetObject*)self)->chain_net,
                ((PyChainTypeObject*)obj_chain_type)->chain_type);
    return Py_BuildValue("O", obj_chain);
}
