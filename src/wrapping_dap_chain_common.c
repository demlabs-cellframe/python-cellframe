#include "wrapping_dap_chain_common.h"

PyObject *dap_chain_hash_slow_to_str_py(PyObject *self, PyObject *args){
    PyObject *obj_hash_slow;
    char *str;
    size_t str_max;
    if (!PyArg_ParseTuple(args, "O|s|n", &obj_hash_slow, &str, &str_max))
            return NULL;
    size_t res = dap_chain_hash_slow_to_str(((PyDapChainHashSlowObject*)obj_hash_slow)->hash_slow, str, str_max);
    return Py_BuildValue("ns", res, str_max);
}

PyObject *dap_chain_addr_to_str_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_addr;
    if (!PyArg_ParseTuple(args, "O", &obj_chain_addr))
        return NULL;
    const dap_chain_addr_t *addr = ((PyDapChainAddrObject*)obj_chain_addr)->addr;
    char *res = dap_chain_addr_to_str(addr);
    return Py_BuildValue("s", res);
}

PyObject *dap_chain_addr_from_str_py(PyObject *self, PyObject *args){
    const char *str;
    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
    PyObject *obj = _PyObject_New(&DapChainAddrObject_DapChainAddrObjectType);
    ((PyDapChainAddrObject*)obj)->addr = dap_chain_addr_from_str(str);
    return Py_BuildValue("O", obj);
}

PyObject *dap_chain_addr_fill_py(PyObject *self, PyObject *args){
    PyObject *key;
    PyObject *net_id;
    if (!PyArg_ParseTuple(args, "O|O", &key, &net_id))
        return NULL;
    dap_chain_addr_fill(((PyDapChainAddrObject*)self)->addr, ((PyCryptoKeyObject*)key)->key, &(((PyDapChainNetIdObject*)net_id)->net_id));
}

PyObject *dap_chain_addr_check_sum_py(PyObject *self, PyObject *args){
    int res = dap_chain_addr_check_sum(((PyDapChainAddrObject*)self)->addr);
    return PyLong_FromLong(res);
}

PyObject *dap_chain_net_id_from_str_py(PyObject *self, PyObject *args){
    const char *str;
    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
    PyObject *obj_net_id = _PyObject_New(&DapChainNetIdObject_DapChainNetIdObjectType);
    ((PyDapChainNetIdObject*)obj_net_id)->net_id = dap_chain_net_id_from_str(str);
    return Py_BuildValue("O", obj_net_id);
}

PyObject *dap_chain_net_srv_uid_from_str_py(PyObject *self, PyObject *args){
    const char *str;
    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
    PyObject *obj = _PyObject_New(&DapChainNetSrvUIDObject_DapChainNetSrvUIDObjectType);
    ((PyDapChainNetSrvUIDObject*)obj)->net_srv_uid = dap_chain_net_srv_uid_from_str(str);
    return Py_BuildValue("O", obj);
}
