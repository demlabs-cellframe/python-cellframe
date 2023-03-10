#include "wrapping_dap_chain_tx_out_cond_subtype_srv_stake.h"

static PyGetSetDef DapChainTxOutCondSubtypeSrvStakePosDelegateGetsSetsDef[]={
        {"uid", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_uid,NULL,NULL,NULL},
        {"addr", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_addr,NULL,NULL,NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_value, NULL, NULL,NULL},
        {NULL}
};

PyTypeObject DapChainTxOutCondSubTypeSrvStakePosDelegateObjectType = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "CellFrame.ChainTxOutCondSubTypeSrvStake",
        .tp_basicsize = sizeof(PyDapChainTxOutCondObject),
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASE_EXC_SUBCLASS,
        "Chain tx cond subtype srv stake object",
        .tp_getset = DapChainTxOutCondSubtypeSrvStakePosDelegateGetsSetsDef,
        .tp_base = &DapChainTxOutCondObjectType,
        .tp_new = PyType_GenericNew
};

PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *obj_net_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject, &DapChainNetSrvUidObjectType);
    obj_net_srv_uid->net_srv_uid = ((PyDapChainTxOutCondObject*)self)->out_cond->header.srv_uid;
    return (PyObject*)obj_net_srv_uid;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_addr(PyObject *self, void *closure){
    (void)closure;
    PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
    obj_addr->addr = &((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_stake_pos_delegate.signing_addr;
    return (PyObject*)obj_addr;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_value(PyObject *self, void *closure){
    (void)closure;
    Py_RETURN_NONE; // TODO return a string with 256 bit representation of (((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_stake.fee_value);
}
