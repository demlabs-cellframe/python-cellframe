#include "wrapping_dap_chain_tx_out_cond_subtype_srv_xchange.h"

static PyGetSetDef DapChainTxOutCondSubtypeSrvXchangeGetsSetsDef[]={
        {"uid", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_uid,NULL,NULL,NULL},
        {"netId", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_net_id,NULL,NULL,NULL},
        {"token", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_token,NULL,NULL,NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_value,NULL,NULL,NULL},
        {}
};

PyTypeObject DapChainTxOutCondSubTypeSrvXchangeObjectType = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "CellFrame.ChainTxOutCondSubTypeSrvXchange",
        .tp_basicsize = sizeof(PyDapChainTxOutCondObject),
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASE_EXC_SUBCLASS,
        "Chain tx cond subtype srv xchange object",
        .tp_getset = DapChainTxOutCondSubtypeSrvXchangeGetsSetsDef,
        .tp_base = &DapChainTxOutCondObjectType,
        .tp_new = PyType_GenericNew
};

PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *obj_net_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject, &DapChainNetSrvUidObjectType);
    obj_net_srv_uid->net_srv_uid = ((PyDapChainTxOutCondObject*)self)->out_cond->header.srv_uid;
    return (PyObject*)obj_net_srv_uid;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_net_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetIdObject *obj_net_id = PyObject_New(PyDapChainNetIdObject, &DapChainNetIdObjectType);
    obj_net_id->net_id = ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_xchange.buy_net_id;
    return (PyObject*)obj_net_id;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_token(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_xchange.buy_token);
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_value(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTxOutCondObject*)self)->out_cond->header.value);
}
