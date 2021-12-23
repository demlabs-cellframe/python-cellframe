#include "wrapping_dap_chain_tx_sig.h"

PyObject *wrapping_dap_chain_tx_sig_get_sign(PyObject *self, void *closure){
    (void)closure;
    PyDapSignObject *obj_sign = PyObject_New(PyDapSignObject, &DapSignObject_DapSignObjectType);
    PyObject_Dir((PyObject*)obj_sign);
    obj_sign->sign = (dap_sign_t*)((PyDapChainTXSigObject*)self)->tx_sig->sig;
    return (PyObject*)obj_sign;
}

PyObject *wrapping_dap_chain_tx_sig_get_sig_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainTXSigObject*)self)->tx_sig->header.sig_size);
}