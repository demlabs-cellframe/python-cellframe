#include "wrapping_dap_chain_tx_receipt.h"

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
        dap_sign_t *l_sign = &l_receipt->exts_n_signs[l_receipt->exts_size];
        if (!dap_sign_verify_size(l_sign, l_signs_size))
            return Py_None;
        size_t l_sign_size = dap_sign_get_size(l_sign);
        PyDapSignObject *obj_sign_provider = PyObject_New(PyDapSignObject, &DapSignObject_DapSignObjectType);
        obj_sign_provider = PyObject_Init(obj_sign_provider, &DapSignObject_DapSignObjectType);
        PyObject_Dir(obj_sign_provider);
        obj_sign_provider->sign = DAP_DUP_SIZE(l_sign, l_sign_size);
        return obj_sign_provider;
    }
    return Py_None;
}
PyObject *wrapping_dap_chain_tx_receipt_get_sig_client(PyObject *self, void *closure){
    UNUSED(closure);
    dap_chain_datum_tx_receipt_t *l_receipt = ((PyDapChainTXReceiptObject*)self)->tx_receipt;
    uint64_t l_signs_size = l_receipt->size - l_receipt->exts_size;
    if (l_signs_size) {
        dap_sign_t *l_sign = &l_receipt->exts_n_signs[l_receipt->exts_size];
        if (!dap_sign_verify_size(l_sign, l_signs_size))
            return Py_None;
        size_t l_sign_size = dap_sign_get_size(l_sign);
        if (l_receipt->exts_size + l_sign_size >= l_receipt->size)
            return Py_None;
        l_sign = &l_receipt->exts_n_signs[l_receipt->exts_size + l_sign_size];
        if (!dap_sign_verify_size(l_sign, l_signs_size - l_sign_size))
            return Py_None;
        l_sign_size = dap_sign_get_size(l_sign);
        PyDapSignObject *obj_sign_client = PyObject_New(PyDapSignObject, &DapSignObject_DapSignObjectType);
        obj_sign_client = PyObject_Init(obj_sign_provider, &DapSignObject_DapSignObjectType);
        PyObject_Dir(obj_sign_client);
        obj_sign_client->sign = DAP_DUP_SIZE(l_sign, l_sign_size);
        return obj_sign_client;
    }
    return Py_None;
}

PyObject *wrapping_dap_chain_tx_receipt_sign(PyObject *self, PyObject *args) {
    PyDapSignObject *obj_sign_ret;
    PyDapChainNetSrvUIDObject *obj_srv_uid;
    PyObject *obj_bytes;
    if (!PyArg_ParseTuple(args, "OOO", &obj_net_id, &obj_srv_uid, &obj_bytes)) {
        return Py_None;
    }
    if (!PyDapChainNetSrvUid_Check(obj_srv_uid))
        return Py_None;
    if (!PyObject_TypeCheck(obj_net_id, &DapChainNetIdObject_DapChainNetIdObjectType))
       return Py_None;
    if (!PyBytes_Check(obj_bytes)) {
        return Py_None;
    }
    //Generate packet
    size_t l_bytes_size = PyBytes_Size(obj_bytes);
    void *l_bytes = PyBytes_AsString(obj_bytes);
    size_t l_request_size = sizeof(dap_stream_ch_chain_net_srv_pkt_test_t) + l_bytes_size;
}
