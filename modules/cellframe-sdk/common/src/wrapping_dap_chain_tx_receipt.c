#include "wrapping_dap_chain_tx_receipt.h"
#include "wrapping_cert.h"

PyObject *wrapping_dap_chain_tx_receipt_get_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("L", ((PyDapChainTXReceiptObject*)self)->tx_receipt->size);
}
PyObject *wrapping_dap_chain_tx_receipt_get_ext_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("L", ((PyDapChainTXReceiptObject*)self)->tx_receipt->exts_size);
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
    UNUSED(closure);
    dap_chain_datum_tx_receipt_t *l_receipt = ((PyDapChainTXReceiptObject*)self)->tx_receipt;
    uint64_t l_signs_size = l_receipt->size - l_receipt->exts_size;
    if (l_signs_size) {
        dap_sign_t *l_sign = (dap_sign_t *)&l_receipt->exts_n_signs[l_receipt->exts_size];
        if (!dap_sign_verify_size(l_sign, l_signs_size))
            return Py_None;
        size_t l_sign_size = dap_sign_get_size(l_sign);
        PyDapSignObject *obj_sign_provider = PyObject_New(PyDapSignObject, &DapSignObject_DapSignObjectType);
        obj_sign_provider = (PyDapSignObject *)PyObject_Init((PyObject *)obj_sign_provider,
                                                             &DapSignObject_DapSignObjectType);
        PyObject_Dir((PyObject *)obj_sign_provider);
        obj_sign_provider->sign = DAP_DUP_SIZE(l_sign, l_sign_size);
        return (PyObject *)obj_sign_provider;
    }
    return Py_None;
}
PyObject *wrapping_dap_chain_tx_receipt_get_sig_client(PyObject *self, void *closure){
    UNUSED(closure);
    dap_chain_datum_tx_receipt_t *l_receipt = ((PyDapChainTXReceiptObject*)self)->tx_receipt;
    uint64_t l_signs_size = l_receipt->size - l_receipt->exts_size;
    if (l_signs_size) {
        dap_sign_t *l_sign = (dap_sign_t *)&l_receipt->exts_n_signs[l_receipt->exts_size];
        if (!dap_sign_verify_size(l_sign, l_signs_size))
            return Py_None;
        size_t l_sign_size = dap_sign_get_size(l_sign);
        if (l_receipt->exts_size + l_sign_size >= l_receipt->size)
            return Py_None;
        l_sign = (dap_sign_t *)&l_receipt->exts_n_signs[l_receipt->exts_size + l_sign_size];
        if (!dap_sign_verify_size(l_sign, l_signs_size - l_sign_size))
            return Py_None;
        l_sign_size = dap_sign_get_size(l_sign);
        PyDapSignObject *obj_sign_client = PyObject_New(PyDapSignObject, &DapSignObject_DapSignObjectType);
        obj_sign_client = (PyDapSignObject *)PyObject_Init((PyObject *)obj_sign_client,
                                                             &DapSignObject_DapSignObjectType);
        PyObject_Dir((PyObject *)obj_sign_client);
        obj_sign_client->sign = DAP_DUP_SIZE(l_sign, l_sign_size);
        return (PyObject *)obj_sign_client;
    }
    return Py_None;
}

PyObject *wrapping_dap_chain_tx_receipt_sign(PyObject *self, PyObject *args) {
    PyCryptoCertObject *obj_cert;
    if (!PyArg_ParseTuple(args, "O", &obj_cert))
        return Py_None;
    if (!PyObject_TypeCheck(obj_cert, &g_crypto_cert_type_py))
       return Py_None;
    if (!obj_cert->cert || !obj_cert->cert->enc_key)
        return Py_None;
    dap_chain_datum_tx_receipt_t *l_receipt = ((PyDapChainTXReceiptObject*)self)->tx_receipt;
    ((PyDapChainTXReceiptObject*)self)->tx_receipt = dap_chain_datum_tx_receipt_sign_add(l_receipt, obj_cert->cert->enc_key);
    return self;
}
