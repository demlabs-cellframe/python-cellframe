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
    return NULL;
}
PyObject *dap_chain_net_sync_chains_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_net_sync_gdb_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_net_sync_all_py(PyObject *self, PyObject *args){
    return NULL;
}

PyObject *dap_chain_net_proc_datapool_py(PyObject *self, PyObject *args){
    return NULL;
}

PyObject *dap_chain_net_by_name_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_net_by_id_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_net_id_by_name_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_ledger_by_net_name_py(PyObject *self, PyObject *args){
    return NULL;
}

PyObject *dap_chain_net_get_chain_by_name_py(PyObject *self, PyObject *args){
    return NULL;
}

PyObject *dap_chain_net_get_cur_addr_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_net_get_cur_cell_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_net_get_cur_addr_int_py(PyObject *self, PyObject *args){
    return NULL;
}

PyObject *dap_chain_net_get_gdb_group_mempool_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_net_get_gdb_group_mempool_by_chain_type_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_net_links_connect_py(PyObject *self, PyObject *args){
    return NULL;
}
PyObject *dap_chain_net_get_chain_by_chain_type_py(PyObject *self, PyObject *args){
    return NULL;
}
