#include "wrapping_dap_chain_tx_token.h"

PyObject *wrapping_dap_chain_tx_token_get_ticker(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainTxTokenObject*)self)->token->header.ticker);
}
PyObject *wrapping_dap_chain_tx_token_get_token_emission_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
    PyObject_Dir((PyObject*)obj_hash);
    obj_hash->hash_fast = &((PyDapChainTxTokenObject*)self)->token->header.token_emission_hash;
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_chain_tx_token_get_token_emission_chain_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainIDObject *obj_chain_id = PyObject_New(PyDapChainIDObject, &DapChainIDObject_DapChainIDType);
    PyObject_Dir((PyObject*)obj_chain_id);
    obj_chain_id->chain_id = &((PyDapChainTxTokenObject*)self)->token->header.token_emission_chain_id;
    return (PyObject*)obj_chain_id;
}
