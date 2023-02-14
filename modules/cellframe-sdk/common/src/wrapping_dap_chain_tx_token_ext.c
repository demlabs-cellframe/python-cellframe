#include "wrapping_dap_chain_tx_token_ext.h"

PyGetSetDef PyDapChainTxTokenExtGetsSetsDef[] = {
        {"version", (getter)wrapping_dap_chain_tx_token_ext_get_version, NULL, NULL, NULL},
        {"ticker", (getter)wrapping_dap_chain_tx_token_ext_get_ticker, NULL, NULL, NULL},
        {"chainId", (getter)wrapping_dap_chain_tx_token_get_chain_id, NULL, NULL, NULL},
        {"netId", (getter)wrapping_dap_chain_tx_token_get_net_id, NULL,NULL, NULL},
        {"txHash", (getter)wrapping_dap_chain_tx_token_get_tx_hash, NULL, NULL, NULL},
        {"txOutIdx", (getter)wrapping_dap_chain_tx_token_get_tx_out_idx, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainTxTokenExtType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxTokenExt",       /* tp_name */
        sizeof(PyDapChainTxTokenExtObject),      /* tp_basicsize */
        0,                                       /* tp_itemsize */
        0,                                       /* tp_dealloc */
        0,                                       /* tp_print */
        0,                                       /* tp_getattr */
        0,                                       /* tp_setattr */
        0,                                       /* tp_reserved */
        0,                                       /* tp_repr */
        0,                                       /* tp_as_number */
        0,                                       /* tp_as_sequence */
        0,                                       /* tp_as_mapping */
        0,                                       /* tp_hash  */
        0,                                       /* tp_call */
        0,                                       /* tp_str */
        0,                                       /* tp_getattro */
        0,                                       /* tp_setattro */
        0,                                       /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                 /* tp_flags */
        "Chain tx token ext object",              /* tp_doc */
        0,		                                 /* tp_traverse */
        0,		                                 /* tp_clear */
        0,		                                 /* tp_richcompare */
        0,                                       /* tp_weaklistoffset */
        0,		                                 /* tp_iter */
        0,		                                 /* tp_iternext */
        0,                                       /* tp_methods */
        0,                                       /* tp_members */
        PyDapChainTxTokenExtGetsSetsDef,                                       /* tp_getset */
        0,                                       /* tp_base */
        0,                                       /* tp_dict */
        0,                                       /* tp_descr_get */
        0,                                       /* tp_descr_set */
        0,                                       /* tp_dictoffset */
        0,                                       /* tp_init */
        0,                                       /* tp_alloc */
        PyType_GenericNew,                       /* tp_new */
};

PyObject *wrapping_dap_chain_tx_token_ext_get_version(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("B", ((PyDapChainTxTokenExtObject*)self)->token_ext->header.version);
}
PyObject *wrapping_dap_chain_tx_token_ext_get_ticker(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainTxTokenExtObject*)self)->token_ext->header.ticker);
}
PyObject *wrapping_dap_chain_tx_token_get_chain_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainIDObject *obj_chain_id = PyObject_New(PyDapChainIDObject, &DapChainIdObjectType);
    obj_chain_id->chain_id = &((PyDapChainTxTokenExtObject*)self)->token_ext->header.ext_chain_id;
    return (PyObject*)obj_chain_id;
}
PyObject *wrapping_dap_chain_tx_token_get_net_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetIdObject *obj_net_id = PyObject_New(PyDapChainNetIdObject, &DapChainNetIdObjectType);
    obj_net_id->net_id = ((PyDapChainTxTokenExtObject*)self)->token_ext->header.ext_net_id;
    return (PyObject*)obj_net_id;
}
PyObject *wrapping_dap_chain_tx_token_get_tx_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hash->hash_fast = &((PyDapChainTxTokenExtObject*)self)->token_ext->header.ext_tx_hash;
    obj_hash->origin = false;
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_chain_tx_token_get_tx_out_idx(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("H", ((PyDapChainTxTokenExtObject*)self)->token_ext->header.ext_tx_out_idx);
}