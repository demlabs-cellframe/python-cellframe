#include "wrapping_dap_chain_common.h"

PyObject *dap_chain_hash_slow_to_str_py(PyObject *self, PyObject *args){
    PyObject *hashSlow;
    char *str;
    size_t strMax;
    if (!PyArg_ParseTuple(args, "O|s|n", &hashSlow, &str, &strMax))
        return NULL;
    size_t result = dap_chain_hash_slow_to_str(((PyDapChainHashSlowObject*)hashSlow)->hash_slow, str, strMax);
    return PyLong_FromSize_t(result);
}

PyObject *dap_chain_str_to_hash_fast_py(PyObject *self, PyObject *args){
    const char *hashStr;
    PyObject *hashFast;
    if (!PyArg_ParseTuple(args, "s|O", &hashStr, &hashFast))
        return NULL;
    int result = dap_chain_str_to_hash_fast(hashStr, ((PyDapChainHashFastObject*)hashFast)->hash_fast);
    return PyLong_FromLong(result);
}

PyObject *dap_chain_addr_to_str_py(PyObject *self, PyObject *args){
    PyObject *addr;
    if (!PyArg_ParseTuple(args, "O", &addr))
        return NULL;
    char *result = dap_chain_addr_to_str(((PyDapChainAddrObject*)addr)->addr);
    return Py_BuildValue("s", result);
}

PyObject *dap_chain_addr_from_str_py(PyObject *self, PyObject *args){
    const char *str;
    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
    PyObject *addrObject = _PyObject_New(&DapChainAddrObject_DapChainAddrObjectType);
    ((PyDapChainAddrObject*)addrObject)->addr = dap_chain_addr_from_str(str);
    return Py_BuildValue("O", addrObject);
}

PyObject *dap_chain_addr_fill_py(PyObject *self, PyObject *args){
    PyObject *addr;
    PyObject *key;
    PyObject *net_id;
    if (!PyArg_ParseTuple(args, "O|O|O", &addr, &key, &net_id))
        return NULL;
    dap_chain_addr_fill(((PyDapChainAddrObject*)addr)->addr, ((PyCryptoKeyObject*)key)->key,
                        ((PyDapChainNetIdObject*)net_id)->net_id);
    return PyLong_FromLong(0);
}

PyObject *dap_chain_addr_check_sum_py(PyObject *self, PyObject *args){
    PyObject *addrObject;
    if (!PyArg_ParseTuple(args, "O", &addrObject))
        return NULL;
    int result = dap_chain_addr_check_sum(((PyDapChainAddrObject*)addrObject)->addr);
    return PyLong_FromLong(result);
}
