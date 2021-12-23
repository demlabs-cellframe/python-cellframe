#include "wrapping_dap_chain_tx_out_cond.h"

PyObject *wrapping_dap_chain_tx_out_cond_get_ts_expires(PyObject *self, void *closure){
    (void)closure;
    PyDateTime_IMPORT;
    return PyDateTime_FromTimestamp((time_t)((PyDapChainTxOutCondObject*)self)->out_cond->header.ts_expires);
}
PyObject *wrapping_dap_chain_tx_out_cond_get_value(PyObject *self, void *closure){
    (void)closure;
    Py_BuildValue("k", ((PyDapChainTxOutCondObject*)self)->out_cond->header.value);
}
PyObject *wrapping_dap_chain_tx_out_cond_get_type_subtype(PyObject *self, void *closure){
    (void)closure;
    PyDapChainTxOutCondSubTypeObject *obj_type_subtype = PyObject_New(PyDapChainTxOutCondSubTypeObject,
                                              &DapChainTxOutCondSubType_DapChainTxOutCondSubTypeType);
    PyObject_Dir((PyObject*)obj_type_subtype);
    obj_type_subtype->out_cond_subtype = ((PyDapChainTxOutCondObject*)self)->out_cond->header.subtype;
    return (PyObject*)obj_type_subtype;
}
PyObject *wrapping_dap_chain_tx_out_cond_get_subtype(PyObject *self, void *closure){
    (void)closure;
    return Py_None;
}

PyObject *PyDapChainTxOutCondSubType_str(PyObject *self){
    return Py_BuildValue("s",
                         dap_chain_tx_out_cond_subtype_to_str(*((PyDapChainTxOutCondSubTypeObject*)self)->out_cond_subtype)
                         );
}
