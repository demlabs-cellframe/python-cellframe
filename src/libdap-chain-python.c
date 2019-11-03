#include "libdap-chain-python.h"


int init_chain_py(){
    return  dap_chain_init();
}

void deinit_chain_py(){
    dap_chain_deinit();
}

PyObject *dap_chain_find_by_id_py(PyObject *self, PyObject *args){
    PyObject *obj_net_id;
    PyObject *obj_chain_id;
    if (!PyArg_ParseTuple(args, "O|O", &obj_net_id, &obj_chain_id))
        return NULL;
    PyObject *new_obj = _PyObject_New(&dapChainObject_dapChainType);
    ((PyDapChainObject*)new_obj)->chain_t = dap_chain_find_by_id(((PyDapChainNetIdObject*)obj_net_id)->net_id, ((PyDapChainIdObject*)obj_chain_id)->id);
    return Py_BuildValue("O", &new_obj);
}

//PyObject *dap_chain_load_from_cfg_py(PyObject *self, PyObject *args){
//return NULL;
//}

PyObject *dap_chain_has_file_store_py(PyObject *self, PyObject *args){
    bool res = dap_chain_has_file_store(((PyDapChainObject*)self)->chain_t);
    if (res)
        return  Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);
}

PyObject *dap_chain_save_all_py(PyObject *self, PyObject *args){
    return PyLong_FromLong( dap_chain_save_all(((PyDapChainObject*)self)->chain_t) );
}

PyObject *dap_chain_load_all_py(PyObject *self, PyObject *args){
    return  PyLong_FromLong(dap_chain_load_all(((PyDapChainObject*)self)->chain_t));
}

PyObject *dap_chain_load_from_cfg_py(PyObject *self, PyObject *args){
    PyObject *obj_ledger;
    const char *chain_net_name;
    PyObject *obj_net_id;
    const char *cfg_name;
    if (!PyArg_ParseTuple(args, "O|s|O|s", &obj_ledger, &chain_net_name, &obj_net_id, &cfg_name))
        return NULL;
    PyObject *res_obj = _PyObject_New(&dapChainObject_dapChainType);
    ((PyDapChainObject*)res_obj)->chain_t = dap_chain_load_from_cfg(((PyDapChainLedgerObject*)obj_ledger)->ledger, chain_net_name, ((PyDapChainNetIdObject*)obj_net_id)->net_id, cfg_name);
    return Py_BuildValue("O", &res_obj);
}

//PyObject *dap_chain_init_net_cfg_name_py(PyObject *self, PyObject *args){
//return NULL;
//}

//PyObject *dap_chain_close_py(PyObject *self, PyObject *args){
//    dap_chain_close(((PyDapChainObject*)self)->chain_t);
//    return PyLong_FromLong(0);
//}

//PyObject *dap_chain_info_dump_log_py(PyObject *self, PyObject *args){
//return NULL;
//}


PyObject *PyDapChainObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapChainObject *obj = (PyDapChainObject*)PyType_GenericNew(type_object, args, kwds);
    return (PyObject *)obj;
}

void PyDapChainObject_dealloc(PyDapChainObject* chain){
    dap_chain_delete(chain->chain_t);
    Py_TYPE(chain)->tp_free((PyObject*)chain);
}
