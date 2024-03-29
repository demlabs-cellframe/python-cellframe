#include "wrapping_dap_chain_tx_token.h"

static PyGetSetDef PyDapChainTxTokenGetsSetsDef[] = {
        {"ticker", (getter)wrapping_dap_chain_tx_token_get_ticker, NULL, NULL, NULL},
        {"tokenEmissionHash", (getter)wrapping_dap_chain_tx_token_get_token_emission_hash, NULL, NULL, NULL},
        {"tokenEmissionChainId", (getter)wrapping_dap_chain_tx_token_get_token_emission_chain_id, NULL, NULL, NULL},
        {}
};

PyTypeObject DapChainTxTokenObjectType  = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainTxToken", sizeof(PyDapChainTxTokenObject),
        "Chain tx token object",
        .tp_getset = PyDapChainTxTokenGetsSetsDef);

PyObject *wrapping_dap_chain_tx_token_get_ticker(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainTxTokenObject*)self)->token->header.ticker);
}
PyObject *wrapping_dap_chain_tx_token_get_token_emission_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hash->hash_fast = &((PyDapChainTxTokenObject*)self)->token->header.token_emission_hash;
    obj_hash->origin = false;
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_chain_tx_token_get_token_emission_chain_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainIDObject *obj_chain_id = PyObject_New(PyDapChainIDObject, &DapChainIdObjectType);
    obj_chain_id->chain_id = &((PyDapChainTxTokenObject*)self)->token->header.token_emission_chain_id;
    return (PyObject*)obj_chain_id;
}
