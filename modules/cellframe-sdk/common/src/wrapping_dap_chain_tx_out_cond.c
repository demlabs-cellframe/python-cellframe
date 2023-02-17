#include "libdap-python.h"
#include "wrapping_dap_chain_tx_out_cond.h"

static PyGetSetDef PyDapChainTxOutCondGetsSetsDef[] = {
        {"tsExpires", (getter)wrapping_dap_chain_tx_out_cond_get_ts_expires, NULL, NULL, NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_cond_get_value, NULL, NULL, NULL},
        {"typeSubtype", (getter)wrapping_dap_chain_tx_out_cond_get_type_subtype, NULL, NULL, NULL},
        {"subtype", (getter)wrapping_dap_chain_tx_out_cond_get_subtype, NULL, NULL, NULL},
        {NULL}
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

// DapChaTxOutCondSubtype
PyTypeObject DapChainTxOutCondSubType_DapChainTxOutCondSubTypeType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainTxOutCondSubType", sizeof(PyDapChainTxOutCondSubTypeObject),
        "Chain tx out cond object",
        .tp_str = PyDapChainTxOutCondSubType_str);

PyObject *wrapping_dap_chain_tx_out_cond_get_type_subtype(PyObject *self, void *closure){
    (void)closure;
    PyDapChainTxOutCondSubTypeObject *obj_type_subtype = PyObject_New(PyDapChainTxOutCondSubTypeObject,
                                              &DapChainTxOutCondSubType_DapChainTxOutCondSubTypeType);
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
