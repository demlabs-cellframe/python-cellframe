#include "wrapping_dap_chain_tx_receipt.h"

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
