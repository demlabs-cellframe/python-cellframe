#include "wrapping_dap_chain_tx_out_cond_subtype_srv_stake.h"

PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *obj_net_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject, &DapChainNetSrvUIDObject_DapChainNetSrvUIDObjectType);
    PyObject_Dir((PyObject*)obj_net_srv_uid);
    obj_net_srv_uid->net_srv_uid = ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_stake.srv_uid;
    return (PyObject*)obj_net_srv_uid;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_addr(PyObject *self, void *closure){
    (void)closure;
    PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObject_DapChainAddrObjectType);
    PyObject_Dir((PyObject*)obj_addr);
    obj_addr->addr = &((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_stake.fee_addr;
    return (PyObject*)obj_addr;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_value(PyObject *self, void *closure){
    (void)closure;
    return PyLong_FromDouble(((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_stake.fee_value);
}
