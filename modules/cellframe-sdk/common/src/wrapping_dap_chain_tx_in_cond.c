#include "wrapping_dap_chain_tx_in_cond.h"

PyObject *wrapping_dap_chain_tx_in_cond_get_receipt_prev_idx(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainTXInCondObject*)self)->tx_in_cond->header.receipt_idx);
}
PyObject *wrapping_dap_chain_tx_in_cond_get_prev_hash(PyObject *self, void *closure){
    (void)closure;
    PyObject *obj_hash = _PyObject_New(&DapHashFastObject_DapHashFastObjectType);
    obj_hash = PyObject_Init(obj_hash, &DapHashFastObject_DapHashFastObjectType);
    PyObject_Dir(obj_hash);
    ((PyDapHashFastObject*)obj_hash)->hash_fast = &((PyDapChainTXInCondObject*)self)->tx_in_cond->header.tx_prev_hash;
    return obj_hash;
}
PyObject *wrapping_dap_chain_tx_in_cond_get_out_prev_idx(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainTXInCondObject*)self)->tx_in_cond->header.tx_out_prev_idx);
}