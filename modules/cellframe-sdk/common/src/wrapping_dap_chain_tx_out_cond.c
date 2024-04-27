#include "libdap-python.h"
#include "wrapping_dap_chain_tx_out_cond.h"
#include "libdap-chain-python.h"

static PyGetSetDef PyDapChainTxOutCondGetsSetsDef[] = {
        {"tsExpires", (getter)wrapping_dap_chain_tx_out_cond_get_ts_expires, NULL, "", NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_cond_get_value, NULL, "", NULL},
        {"typeSubtype", (getter)wrapping_dap_chain_tx_out_cond_get_type_subtype, NULL, "", NULL},
        {"subtype", (getter)wrapping_dap_chain_tx_out_cond_get_subtype, NULL, "", NULL},
        {"usedBy", (getter)wrapping_dap_chain_tx_out_cound_used_by, NULL, "", NULL},
        {}
};

PyTypeObject DapChainTxOutCondObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainTxOutCond", sizeof(PyDapChainTxOutCondObject),
        "Chain tx out cond object",
        .tp_getset = PyDapChainTxOutCondGetsSetsDef);

PyObject *wrapping_dap_chain_tx_out_cond_get_ts_expires(PyObject *self, void *closure){
    (void)closure;
    PyDateTime_IMPORT;
    PyObject *timestamp = Py_BuildValue("(k)", ((PyDapChainTxOutCondObject*)self)->out_cond->header.ts_expires);
    return PyDateTime_FromTimestamp(timestamp);
}
PyObject *wrapping_dap_chain_tx_out_cond_get_value(PyObject *self, void *closure){
    (void)closure;
    DapMathObject *obj_math = PyObject_New(DapMathObject, &DapMathObjectType);
    obj_math->value = ((PyDapChainTxOutCondObject*)self)->out_cond->header.value;
    return (PyObject*)obj_math;
}


PyObject *wrapping_dap_chain_tx_out_cound_used_by(PyObject *self, void *closure){
    dap_hash_fast_t l_spender_hash = {0};
    PyDapChainTxOutCondObject *obj =((PyDapChainTxOutCondObject*)self);
    if (dap_ledger_tx_hash_is_used_out_item(obj->ledger, obj->tx_hash, obj->idx, &l_spender_hash)) {
        PyDapHashFastObject *l_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
        l_hf->hash_fast = DAP_NEW(dap_hash_fast_t);
        memcpy(l_hf->hash_fast, &l_spender_hash, sizeof(dap_hash_fast_t));
        l_hf->origin = true;
        return (PyObject*)l_hf;
    }
    Py_RETURN_NONE;
}

// DapChaTxOutCondSubtype
PyTypeObject DapChainTxOutCondSubTypeObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainTxOutCondSubType", sizeof(PyDapChainTxOutCondSubTypeObject),
        "Chain tx out cond object",
        .tp_str = PyDapChainTxOutCondSubType_str);

PyObject *wrapping_dap_chain_tx_out_cond_get_type_subtype(PyObject *self, void *closure){
    (void)closure;
    PyDapChainTxOutCondSubTypeObject *obj_type_subtype = PyObject_New(PyDapChainTxOutCondSubTypeObject,
                                              &DapChainTxOutCondSubTypeObjectType);
    obj_type_subtype->out_cond_subtype = &((PyDapChainTxOutCondObject*)self)->out_cond->header.subtype;
    return (PyObject*)obj_type_subtype;
}
PyObject *wrapping_dap_chain_tx_out_cond_get_subtype(PyObject *self, void *closure){
    (void)closure;
    Py_RETURN_NONE;
}

PyObject *PyDapChainTxOutCondSubType_str(PyObject *self){
    return Py_BuildValue("s",
                         dap_chain_tx_out_cond_subtype_to_str(*((PyDapChainTxOutCondSubTypeObject*)self)->out_cond_subtype)
                         );
}
