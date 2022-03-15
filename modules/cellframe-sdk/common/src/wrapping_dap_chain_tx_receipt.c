#include "wrapping_dap_chain_tx_receipt.h"

PyGetSetDef DapChainTxReceiptGetSetDefs[] = {
        {"size", (getter)wrapping_dap_chain_tx_receipt_get_size, NULL, NULL, NULL},
        {"extSize", (getter)wrapping_dap_chain_tx_receipt_get_ext_size, NULL, NULL, NULL},
        {"units", (getter)wrapping_dap_chain_tx_receipt_get_units, NULL, NULL, NULL},
        {"uid", (getter)wrapping_dap_chain_tx_receipt_get_uid, NULL, NULL, NULL},
        {"unitsType", (getter)wrapping_dap_chain_tx_receipt_get_units_type, NULL, NULL, NULL},
        {"value", (getter)wrapping_dap_chain_tx_receipt_get_value, NULL, NULL, NULL},
        {"provider", (getter)wrapping_dap_chain_tx_receipt_get_sig_provider, NULL, NULL, NULL},
        {"client", (getter)wrapping_dap_chain_tx_receipt_get_sig_client, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainTxReceiptObject_DapChainTxReceiptTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxReceipt",        /* tp_name */
        sizeof(PyDapChainTXReceiptObject), /* tp_basicsize */
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
        "Chain tx item receipt object",        /* tp_doc */
        0,		                            /* tp_traverse */
        0,		                            /* tp_clear */
        0,		                            /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,		                            /* tp_iter */
        0,		                            /* tp_iternext */
        0,  /* tp_methods */
        0,                                  /* tp_members */
        DapChainTxReceiptGetSetDefs,                                  /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        0,                                  /* tp_init */
        0,                                  /* tp_alloc */
        PyType_GenericNew,                  /* tp_new */
};

PyObject *wrapping_dap_chain_tx_receipt_get_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("H", ((PyDapChainTXReceiptObject*)self)->tx_receipt->size);
}
PyObject *wrapping_dap_chain_tx_receipt_get_ext_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("H", ((PyDapChainTXReceiptObject*)self)->tx_receipt->exts_size);
}
PyObject *wrapping_dap_chain_tx_receipt_get_units(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTXReceiptObject*)self)->tx_receipt->receipt_info.units);
}
PyObject *wrapping_dap_chain_tx_receipt_get_uid(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTXReceiptObject*)self)->tx_receipt->receipt_info.srv_uid.uint64);
}
PyObject *wrapping_dap_chain_tx_receipt_get_units_type(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", serv_unit_enum_to_str(&
        ((PyDapChainTXReceiptObject*)self)->tx_receipt->receipt_info.units_type.enm));
}
PyObject *wrapping_dap_chain_tx_receipt_get_value(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTXReceiptObject*)self)->tx_receipt->receipt_info.value_datoshi);
}
PyObject *wrapping_dap_chain_tx_receipt_get_sig_provider(PyObject *self, void *closure){
    (void)closure;
    uint16_t l_exts_size = ((PyDapChainTXReceiptObject*)self)->tx_receipt->exts_size;
    if (l_exts_size == sizeof(dap_sign_t) + sizeof(dap_sign_t) || l_exts_size == sizeof(dap_sign_t)){
        PyObject *obj_sign_provider = _PyObject_New(&DapSignObject_DapSignObjectType);
        obj_sign_provider = PyObject_Init(obj_sign_provider, &DapSignObject_DapSignObjectType);
        PyObject_Dir(obj_sign_provider);
        ((PyDapSignObject*)obj_sign_provider)->sign = DAP_NEW(dap_sign_t);
        memcpy(
                ((PyDapSignObject*)obj_sign_provider)->sign,
                ((PyDapChainTXReceiptObject*)self)->tx_receipt->exts_n_signs,
                sizeof(dap_sign_t));
        return obj_sign_provider;
    }
    return Py_None;
}
PyObject *wrapping_dap_chain_tx_receipt_get_sig_client(PyObject *self, void *closure){
    (void)closure;
    uint16_t l_exts_size = ((PyDapChainTXReceiptObject*)self)->tx_receipt->exts_size;
    if (l_exts_size == sizeof(dap_sign_t) + sizeof(dap_sign_t)){
        PyObject *obj_sign_client = _PyObject_New(&DapSignObject_DapSignObjectType);
        obj_sign_client = PyObject_Init(obj_sign_client, &DapSignObject_DapSignObjectType);
        PyObject_Dir(obj_sign_client);
        ((PyDapSignObject*)obj_sign_client)->sign = DAP_NEW(dap_sign_t);
        memcpy(
                ((PyDapSignObject*)obj_sign_client)->sign,
               ((PyDapChainTXReceiptObject*)self)->tx_receipt->exts_n_signs + sizeof(dap_sign_t),
               sizeof(dap_sign_t));
        return obj_sign_client;
    }
    return Py_None;
}
