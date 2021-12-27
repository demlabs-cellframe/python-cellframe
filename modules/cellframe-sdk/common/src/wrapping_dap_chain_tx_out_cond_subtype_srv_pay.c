#include "wrapping_dap_chain_tx_out_cond_subtype_srv_pay.h"

PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_unit(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_pay.unit.uint32);
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *obj_net_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject, &DapChainNetSrvUIDObject_DapChainNetSrvUIDObjectType);
    PyObject_Dir((PyObject*)obj_net_srv_uid);
    obj_net_srv_uid->net_srv_uid = ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_pay.srv_uid;
    return (PyObject*)obj_net_srv_uid;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_pkey(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
    PyObject_Dir((PyObject*)obj_hash);
    obj_hash->hash_fast = &((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_pay.pkey_hash;
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_map_price(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_pay.unit_price_max_datoshi);
}