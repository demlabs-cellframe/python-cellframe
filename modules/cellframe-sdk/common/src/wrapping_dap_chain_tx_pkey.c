#include "wrapping_dap_chain_tx_pkey.h"

PyObject *wrapping_dap_chain_tx_pkey_sig_type(PyObject *self, void *closure){
    (void)closure;
    PyObject *obj_sign_type = _PyObject_New(&DapSignTypeObject_DapSignTypeObjectType);
    obj_sign_type = PyObject_Init(obj_sign_type, &DapSignTypeObject_DapSignTypeObjectType);
    PyObject_Dir(obj_sign_type);
    ((PyDapSignTypeObject*)obj_sign_type)->sign_type= &((PyDapChainTXPkeyObject*)self)->tx_pkey->header.sig_type;
    return obj_sign_type;
}
PyObject *wrapping_dap_chain_tx_pkey_sig_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainTXPkeyObject*)self)->tx_pkey->header.sig_size);
}
PyObject *wrapping_dap_chain_tx_pkey_seq_no(PyObject *self, void *closure){
    return Py_BuildValue("I", ((PyDapChainTXPkeyObject*)self)->tx_pkey->seq_no);
}
PyObject *wrapping_dap_chain_tx_pkey_get_pkey(PyObject *self, void *closure){
    (void)closure;
    PyDapPkeyObject *obj_pkey = PyObject_NEW(PyDapPkeyObject, &DapPkeyTypeObject_DapPkeyTypeObjectType);
    obj_pkey->pkey = ((PyDapChainTXPkeyObject*)self)->tx_pkey->pkey;
    return (PyObject*)obj_pkey;
}