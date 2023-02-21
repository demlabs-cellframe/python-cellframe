#include "wrapping_dap_chain_tx_pkey.h"

PyGetSetDef DapChainTxPkeyGetsSetsDef[] = {
        {"sigType", (getter)wrapping_dap_chain_tx_pkey_sig_type, NULL, NULL, NULL},
        {"sigSize", (getter)wrapping_dap_chain_tx_pkey_sig_size, NULL, NULL, NULL},
        {"sequenceNumber", (getter)wrapping_dap_chain_tx_pkey_seq_no, NULL, NULL, NULL},
        {"pkey", (getter)wrapping_dap_chain_tx_pkey_get_pkey, NULL, NULL, NULL},
        {NULL}
};

PyMethodDef PyDapChainTxPkeyObjectMethods[] ={
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainTxPkeyObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxPkey",        /* tp_name */
        sizeof(PyDapChainTXPkeyObject), /* tp_basicsize */
        0,                                  /* tp_itemsize */
        0,                                  /* tp_dealloc */
        0,                                  /* tp_print */
        0,                                  /* tp_getattr */
        0,                                  /* tp_setattr */
        0,                                  /* tp_reserved */
        0,                                  /* tp_repr */
        0,                                  /* tp_as_number */
        0,                                  /* tp_as_sequence */
        0,                                  /* tp_as_mapping */
        0,                                  /* tp_hash  */
        0,                                  /* tp_call */
        0,                                  /* tp_str */
        0,                                  /* tp_getattro */
        0,                                  /* tp_setattro */
        0,                                  /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
        "Chain tx pkey object",        /* tp_doc */
        0,		                            /* tp_traverse */
        0,		                            /* tp_clear */
        0,		                            /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,		                            /* tp_iter */
        0,		                            /* tp_iternext */
        PyDapChainTxPkeyObjectMethods,  /* tp_methods */
        0,                                  /* tp_members */
        DapChainTxPkeyGetsSetsDef,                                  /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        0,                                  /* tp_init */
        0,                                  /* tp_alloc */
        PyType_GenericNew,                  /* tp_new */
};

PyObject *wrapping_dap_chain_tx_pkey_sig_type(PyObject *self, void *closure){
    (void)closure;
    PyDapSignTypeObject *obj_sign_type = PyObject_New(PyDapSignTypeObject, &DapCryproSignTypeObjectType);
    obj_sign_type->sign_type= &((PyDapChainTXPkeyObject*)self)->tx_pkey->header.sig_type;
    return (PyObject*)obj_sign_type;
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
    obj_pkey->pkey = (dap_pkey_t *)((PyDapChainTXPkeyObject*)self)->tx_pkey->pkey;
    return (PyObject*)obj_pkey;
}
