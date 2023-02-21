#include "wrapping_dap_chain_tx_receipt.h"
#include "wrapping_cert.h"

#define LOG_TAG "wrapping_dap_chain_tx_receipt"

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

PyMethodDef DapChainTxReceiptMethods[] = {
        {"sign", (PyCFunction)wrapping_dap_chain_tx_receipt_sign, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainTxReceiptObjectType = {
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
        Py_TPFLAGS_BASETYPE,                /* tp_flags */
        "Chain tx item receipt object",     /* tp_doc */
        0,                                  /* tp_traverse */
        0,                                  /* tp_clear */
        0,                                  /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,                                  /* tp_iter */
        0,                                  /* tp_iternext */
        DapChainTxReceiptMethods,           /* tp_methods */
        0,                                  /* tp_members */
        DapChainTxReceiptGetSetDefs,        /* tp_getset */
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
    serv_unit_enum_t l_unit = ((PyDapChainTXReceiptObject*)self)->tx_receipt->receipt_info.units_type.enm;
    return Py_BuildValue("s", serv_unit_enum_to_str(&l_unit));
}
PyObject *wrapping_dap_chain_tx_receipt_get_value(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTXReceiptObject*)self)->tx_receipt->receipt_info.value_datoshi);
}

PyObject *wrapping_dap_chain_tx_receipt_get_sig_provider(PyObject *self, void *closure){
    UNUSED(closure);
    dap_chain_datum_tx_receipt_t *l_receipt = ((PyDapChainTXReceiptObject*)self)->tx_receipt;
    uint64_t l_signs_size = l_receipt->size - l_receipt->exts_size;
    if (l_signs_size) {
        dap_sign_t *l_sign = (dap_sign_t *)&l_receipt->exts_n_signs[l_receipt->exts_size];
        if (!dap_sign_verify_size(l_sign, l_signs_size))
            Py_RETURN_NONE;
        size_t l_sign_size = dap_sign_get_size(l_sign);
        PyDapSignObject *obj_sign_provider = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
        obj_sign_provider->sign = DAP_DUP_SIZE(l_sign, l_sign_size);
        return (PyObject *)obj_sign_provider;
    }
    Py_RETURN_NONE;
}

PyObject *wrapping_dap_chain_tx_receipt_get_sig_client(PyObject *self, void *closure){
    UNUSED(closure);
    dap_chain_datum_tx_receipt_t *l_receipt = ((PyDapChainTXReceiptObject*)self)->tx_receipt;
    uint64_t l_signs_size = l_receipt->size - l_receipt->exts_size;
    if (l_signs_size) {
        dap_sign_t *l_sign = (dap_sign_t *)&l_receipt->exts_n_signs[l_receipt->exts_size];
        if (!dap_sign_verify_size(l_sign, l_signs_size))
            Py_RETURN_NONE;
        size_t l_sign_size = dap_sign_get_size(l_sign);
        if (l_receipt->exts_size + l_sign_size >= l_receipt->size)
            Py_RETURN_NONE;
        l_sign = (dap_sign_t *)&l_receipt->exts_n_signs[l_receipt->exts_size + l_sign_size];
        if (!dap_sign_verify_size(l_sign, l_signs_size - l_sign_size))
            Py_RETURN_NONE;
        l_sign_size = dap_sign_get_size(l_sign);
        PyDapSignObject *obj_sign_client = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
        obj_sign_client->sign = DAP_DUP_SIZE(l_sign, l_sign_size);
        return (PyObject *)obj_sign_client;
    }
    Py_RETURN_NONE;
}

PyObject *wrapping_dap_chain_tx_receipt_sign(PyObject *self, PyObject *sign) {
    PyCryptoCertObject *obj_cert;
    if (!PyArg_ParseTuple(sign, "O", &obj_cert)) {
        log_it(L_ERROR, "Certificate for receipt signing not provided");
        Py_RETURN_NONE;
    }
    if (!PyObject_TypeCheck(obj_cert, &DapCryptoCertObjectType)) {
        log_it(L_ERROR, "Certificate for receipt signing has invalid object type");
        Py_RETURN_NONE;
    }
    if (!obj_cert->cert || !obj_cert->cert->enc_key) {
        log_it(L_ERROR, "Certificate for receipt signing has no cert object or private key");
        Py_RETURN_NONE;
    }
    dap_chain_datum_tx_receipt_t *l_receipt = ((PyDapChainTXReceiptObject*)self)->tx_receipt;
    ((PyDapChainTXReceiptObject*)self)->tx_receipt = dap_chain_datum_tx_receipt_sign_add(l_receipt, obj_cert->cert->enc_key);
    return self;
}
