#include "wrapping_dap_chain_tx_receipt.h"
#include "wrapping_cert.h"

#define LOG_TAG "wrapping_dap_chain_tx_receipt"

static PyGetSetDef DapChainTxReceiptGetSetDefs[] = {
        {"size", (getter)wrapping_dap_chain_tx_receipt_get_size, NULL, NULL, NULL},
        {"extSize", (getter)wrapping_dap_chain_tx_receipt_get_ext_size, NULL, NULL, NULL},
        {"units", (getter)wrapping_dap_chain_tx_receipt_get_units, NULL, NULL, NULL},
        {"uid", (getter)wrapping_dap_chain_tx_receipt_get_uid, NULL, NULL, NULL},
        {"unitsType", (getter)wrapping_dap_chain_tx_receipt_get_units_type, NULL, NULL, NULL},
        {"value", (getter)wrapping_dap_chain_tx_receipt_get_value, NULL, NULL, NULL},
        {"provider", (getter)wrapping_dap_chain_tx_receipt_get_sig_provider, NULL, NULL, NULL},
        {"client", (getter)wrapping_dap_chain_tx_receipt_get_sig_client, NULL, NULL, NULL},
        {}
};

static PyMethodDef DapChainTxReceiptMethods[] = {
        {"sign", (PyCFunction)wrapping_dap_chain_tx_receipt_sign, METH_VARARGS, ""},
        {}
};

PyTypeObject DapChainTxReceiptObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainTxReceipt", sizeof(PyDapChainTXReceiptObject),
        "Chain tx item receipt object",
        .tp_methods = DapChainTxReceiptMethods,
        .tp_getset = DapChainTxReceiptGetSetDefs);

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
    dap_chain_srv_unit_enum_t l_unit = ((PyDapChainTXReceiptObject*)self)->tx_receipt->receipt_info.units_type.enm;
    return Py_BuildValue("s", dap_chain_srv_unit_enum_to_str(l_unit));
}
PyObject *wrapping_dap_chain_tx_receipt_get_value(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTXReceiptObject*)self)->tx_receipt->receipt_info.value_datoshi);
}

PyObject *wrapping_dap_chain_tx_receipt_get_sig_provider(PyObject *self, void *closure){
    UNUSED(closure);
    dap_chain_datum_tx_receipt_t *l_receipt = ((PyDapChainTXReceiptObject*)self)->tx_receipt;
    if (l_receipt->receipt_info.version < 2){
        dap_chain_datum_tx_receipt_old_t *l_receipt_old = (dap_chain_datum_tx_receipt_old_t*)l_receipt;
        uint64_t l_signs_size = l_receipt_old->size - l_receipt_old->exts_size;
        if (l_signs_size) {
            dap_sign_t *l_sign = (dap_sign_t *)&l_receipt_old->exts_n_signs[l_receipt_old->exts_size];
            if ( dap_sign_verify_size(l_sign, l_signs_size) )
                Py_RETURN_NONE;
            PyObject  *obj_sign_provider = PyDapSignObject_Create(l_sign);
            return (PyObject *)obj_sign_provider;
        }
    } else {
        /*
        uint64_t l_signs_size = l_receipt->size - l_receipt->exts_size;
        if (l_signs_size) {
            dap_sign_t *l_sign = (dap_sign_t *)&l_receipt->exts_n_signs[l_receipt->exts_size];
            if ( dap_sign_verify_size(l_sign, l_signs_size) )
                Py_RETURN_NONE;
            PyObject  *obj_sign_provider = PyDapSignObject_Create(l_sign);
            return (PyObject *)obj_sign_provider;
        }
        */
       Py_RETURN_NONE;
    }
    Py_RETURN_NONE;
}

PyObject *wrapping_dap_chain_tx_receipt_get_sig_client(PyObject *self, void *closure){
    UNUSED(closure);
    dap_chain_datum_tx_receipt_t *l_receipt = ((PyDapChainTXReceiptObject*)self)->tx_receipt;
    if (l_receipt->receipt_info.version < 2){
        dap_chain_datum_tx_receipt_old_t *l_receipt_old = (dap_chain_datum_tx_receipt_old_t*)l_receipt;
        uint64_t l_signs_size = l_receipt_old->size - l_receipt_old->exts_size;
        if (l_signs_size) {
            dap_sign_t *l_sign = (dap_sign_t *)&l_receipt_old->exts_n_signs[l_receipt_old->exts_size];
            if ( dap_sign_verify_size(l_sign, l_signs_size) )
                Py_RETURN_NONE;
            size_t l_sign_size = dap_sign_get_size(l_sign);
            if (l_receipt_old->exts_size + l_sign_size >= l_receipt_old->size)
                Py_RETURN_NONE;
            l_sign = (dap_sign_t *)&l_receipt_old->exts_n_signs[l_receipt_old->exts_size + l_sign_size];
            if ( dap_sign_verify_size(l_sign, l_signs_size - l_sign_size) )
                Py_RETURN_NONE;
            PyObject *obj_sign_client = PyDapSignObject_Create(l_sign);
            return obj_sign_client;
        }
    } else {
        Py_RETURN_NONE;
        /*
        uint64_t l_signs_size = l_receipt->size - l_receipt->exts_size;
        if (l_signs_size) {
            dap_sign_t *l_sign = (dap_sign_t *)&l_receipt->exts_n_signs[l_receipt->exts_size];
            if ( dap_sign_verify_size(l_sign, l_signs_size) )
                Py_RETURN_NONE;
            size_t l_sign_size = dap_sign_get_size(l_sign);
            if (l_receipt->exts_size + l_sign_size >= l_receipt->size)
                Py_RETURN_NONE;
            l_sign = (dap_sign_t *)&l_receipt->exts_n_signs[l_receipt->exts_size + l_sign_size];
            if ( dap_sign_verify_size(l_sign, l_signs_size - l_sign_size) )
                Py_RETURN_NONE;
            PyObject *obj_sign_client = PyDapSignObject_Create(l_sign);
            return obj_sign_client;
        }
        */
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
    if (l_receipt->receipt_info.version < 2){
        log_it(L_ERROR, "Receipt version < 2 is deprecated.");
        Py_RETURN_NONE;
    }
    ((PyDapChainTXReceiptObject*)self)->tx_receipt = dap_chain_datum_tx_receipt_sign_add(l_receipt, obj_cert->cert->enc_key);
    return self;
}
