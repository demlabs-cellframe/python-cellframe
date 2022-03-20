#include "wrapping_dap_mempool.h"

PyObject *dap_chain_mempool_add_proc_py(PyObject *self, PyObject *args){
    PyObject *obj_server;
    const char *MEMPOOL_URL;
    if (!PyArg_ParseTuple(args, "O|s", &obj_server, &MEMPOOL_URL))
        return NULL;
    dap_chain_mempool_add_proc(DAP_HTTP(((PyDapServerObject*)obj_server)->t_server), MEMPOOL_URL);
    return PyLong_FromLong(0);
}

PyObject *dap_chain_mempool_tx_create_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_chain;
    PyObject *obj_key_from;
    PyObject *obj_addr_from;
    PyObject *obj_addr_to;
    PyObject *obj_addr_fee;
    char *l_token_ticker;
    uint64_t l_value;
    uint64_t l_value_fee;
    if (!PyArg_ParseTuple(args, "OOOOOskk", &obj_chain, &obj_key_from, &obj_addr_from, &obj_addr_to, &obj_addr_fee,
                          &l_token_ticker, &l_value, &l_value_fee)){
        return NULL;
    }
    dap_chain_t *l_chain = ((PyDapChainObject*)obj_chain)->chain_t;
    dap_enc_key_t *l_key_from = ((PyCryptoKeyObject*)obj_key_from)->key;
    dap_chain_addr_t *l_addr_from = ((PyDapChainAddrObject*)obj_addr_from)->addr;
    dap_chain_addr_t *l_addr_to = ((PyDapChainAddrObject*)obj_addr_to)->addr;
    dap_chain_addr_t *l_addr_fee = NULL;
    if (obj_addr_fee != Py_None)
        l_addr_fee = ((PyDapChainAddrObject*)obj_addr_fee)->addr;
    dap_chain_hash_fast_t  *l_hash_tx = dap_chain_mempool_tx_create(l_chain, l_key_from,
                                                                    l_addr_from, l_addr_to,
                                                                    l_addr_fee, l_token_ticker,
                                                                    l_value, l_value_fee);
    if (l_hash_tx == NULL){
        return Py_None;
    } else {
        PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
        PyObject_Dir((PyObject*)obj_hf);
        obj_hf->hash_fast = l_hash_tx;
        return (PyObject*)obj_hf;
    }
}

PyObject *dap_chain_mempool_tx_create_cond_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_net;
    PyObject* obj_key_from;
    PyObject* obj_key_cond;
    char *l_token_ticker;
    uint64_t l_value;
    uint64_t l_value_per_unit_max;
    PyObject *obj_unit;
    PyObject *obj_srv_uid;
    uint64_t l_fee;
    PyObject *obj_cond;
    if (!PyArg_ParseTuple(args, "OOOskkOOkO", &obj_net, &obj_key_from, &obj_key_cond, &l_token_ticker, &l_value,
                          &l_value_per_unit_max, &obj_unit, &obj_srv_uid, &l_fee, &obj_cond)){
        PyErr_SetString(PyExc_AttributeError, "Function takes exactly ten arguments.");
        return NULL;
    }
    if (!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_AttributeError, "Invalid first argument passed. The first argument must "
                                              "be an instance of an object of type ChainNet. ");
        return NULL;
    }
    void *l_bytes_cond = PyBytes_AsString(obj_cond);
    size_t l_bytes_cond_size = PyBytes_Size(obj_cond);
    dap_hash_fast_t *l_hf = dap_chain_mempool_tx_create_cond(
            ((PyDapChainNetObject*)obj_net)->chain_net,
            ((PyCryptoKeyObject*)obj_key_from)->key,
            ((PyDapPkeyObject *)obj_key_cond)->pkey,
            l_token_ticker,
            l_value,
            l_value_per_unit_max,
            ((PyDapChainNetSrvPriceUnitUIDObject*)obj_unit)->price_unit_uid,
            ((PyDapChainNetSrvUIDObject*)obj_srv_uid)->net_srv_uid,
            l_fee,
            l_bytes_cond,
            l_bytes_cond_size
            );
    if (!l_hf){
        return Py_None;
    }
    PyDapHashFastObject *l_obj_hf = PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
    l_obj_hf->hash_fast = l_hf;
    return (PyObject*)l_hf;
}
PyObject *dap_chain_mempool_tx_create_cond_input_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_net;
    PyObject *obj_tx_prev_hash;
    PyObject *obj_addr_to;
    PyObject *obj_key_tx_sign;
    PyObject *obj_receipt;
    if (!PyArg_ParseTuple(args, "OOOOO", &obj_net, &obj_tx_prev_hash, &obj_addr_to, &obj_key_tx_sign, &obj_receipt)){
        PyErr_SetString(PyExc_AttributeError, "Function takes exactly five arguments.");
        return NULL;
    }
    if (!PyDapChainNet_Check(obj_net)) {
        PyErr_SetString(PyExc_AttributeError, "Invalid first argument passed. The first argument must "
                                              "be an instance of an object of type ChainNet. ");
        return NULL;
    }
    if (!PyDapHashFast_Check(obj_tx_prev_hash)){
        PyErr_SetString(PyExc_AttributeError, "Invalid second argument passed. The first argument must "
                                              "be an instance of an object of type ChainNet. ");
        return NULL;
    }
    if (!PyDapSignObject_Check(obj_key_tx_sign)){
        PyErr_SetString(PyExc_AttributeError, "Invalid fourth argument passed. The first argument must "
                                              "be an instance of an object of type DapSign.");
        return NULL;
    }
    dap_chain_hash_fast_t *l_hf =dap_chain_mempool_tx_create_cond_input(
            ((PyDapChainNetObject*)obj_net)->chain_net,
            ((PyDapHashFastObject*)obj_tx_prev_hash)->hash_fast,
            ((PyDapChainAddrObject *)obj_addr_to)->addr,
            ((PyCryptoKeyObject*)obj_key_tx_sign)->key,
            ((PyDapChainTXReceiptObject*)obj_receipt)->tx_receipt);
    if (!l_hf){
        return Py_None;
    }
    PyDapHashFastObject *l_obj_hf = PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
    l_obj_hf->hash_fast = l_hf;
    return (PyObject*)l_hf;
}
