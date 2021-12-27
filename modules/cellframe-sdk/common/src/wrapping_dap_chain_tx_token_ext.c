#include "wrapping_dap_chain_tx_token_ext.h"

PyObject *wrapping_dap_chain_tx_token_ext_get_version(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("B", ((PyDapChainTxTokenExtObject*)self)->token_ext->header.version);
}
PyObject *wrapping_dap_chain_tx_token_ext_get_ticker(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainTxTokenExtObject*)self)->token_ext->header.ticker);
}
PyObject *wrapping_dap_chain_tx_token_get_chain_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainIDObject *obj_chain_id = PyObject_New(PyDapChainIDObject, &DapChainIDObject_DapChainIDType);
    PyObject_Dir((PyObject*)obj_chain_id);
    obj_chain_id->chain_id = &((PyDapChainTxTokenExtObject*)self)->token_ext->header.ext_chain_id;
    return (PyObject*)obj_chain_id;
}
PyObject *wrapping_dap_chain_tx_token_get_net_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetIdObject *obj_net_id = PyObject_New(PyDapChainNetIdObject, &DapChainNetIdObject_DapChainNetIdObjectType);
    PyObject_Dir((PyObject*)obj_net_id);
    obj_net_id->net_id = ((PyDapChainTxTokenExtObject*)self)->token_ext->header.ext_net_id;
    return (PyObject*)obj_net_id;
}
PyObject *wrapping_dap_chain_tx_token_get_tx_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
    obj_hash->hash_fast = &((PyDapChainTxTokenExtObject*)self)->token_ext->header.ext_tx_hash;
    PyObject_Dir((PyObject*)obj_hash);
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_chain_tx_token_get_tx_out_idx(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("H", ((PyDapChainTxTokenExtObject*)self)->token_ext->header.ext_tx_out_idx);
}