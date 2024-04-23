#include "wrapping_dap_chain_tx_out.h"

static PyGetSetDef DapChainTxOutGetsSetsDef[] = {
        {"addr", (getter)wrapping_dap_chain_tx_out_get_addr, NULL, "", NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_get_value, NULL, "", NULL},
        {"usedBy", (getter)wrapping_dap_chain_tx_out_get_used_by, NULL, "", NULL},
        {NULL}
};

static PyMethodDef PyDapChainTxOutObjectMethods[] ={
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainTxOutObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainTxOut", sizeof(PyDapChainTXOutObject),
        "Chain tx out object",
        .tp_methods = PyDapChainTxOutObjectMethods,
        .tp_getset = DapChainTxOutGetsSetsDef);

PyObject *wrapping_dap_chain_tx_out_get_addr(PyObject *self, void *closure){
    (void)closure;
    PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
    obj_addr->addr = &((PyDapChainTXOutObject*)self)->tx_out->addr;
    return (PyObject*)obj_addr;
}
PyObject *wrapping_dap_chain_tx_out_get_value(PyObject *self, void *closure){
    (void)closure;
    DapMathObject *l_math = PyObject_New(DapMathObject, &DapMathObjectType);
    l_math->value = ((PyDapChainTXOutObject*)self)->tx_out->header.value;
    return (PyObject*)l_math;
}

PyObject *wrapping_dap_chain_tx_out_get_used_by(PyObject *self, void *closure){ 
    dap_hash_fast_t l_spender_hash = {0};
    PyDapChainTXOutObject *l_obj = (PyDapChainTXOutObject*)self;
    if (dap_ledger_tx_hash_is_used_out_item(l_obj->ledger, l_obj->tx_hash, l_obj->idx, &l_spender_hash)) {
        PyDapHashFastObject *l_hf = PyObject_New(PyDapHashFastObject, &DapHashFastObjectType);
        l_hf->hash_fast = DAP_NEW(dap_hash_fast_t);
        memcpy(l_hf->hash_fast, &l_spender_hash, sizeof(dap_hash_fast_t));
        l_hf->origin = true;
        return (PyObject*)l_hf;
    }
    Py_RETURN_NONE;
}
