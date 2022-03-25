#include "wrapping_dap_chain_tx_out_cond_subtype_srv_stake.h"

PyGetSetDef DapChainTxOutCondSubtypeSrvStakeGetsSetsDef[]={
        {"uid", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_uid,NULL,NULL,NULL},
        {"addr", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_addr,NULL,NULL,NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_value, NULL, NULL,NULL},
        {NULL}
};

PyTypeObject DapChainTxOutCondSubTypeSrvStakeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxOutCondSubTypeSrvStake",       /* tp_name */
        sizeof(PyDapChainTxOutCondObject),/* tp_basicsize */
        0,                               /* tp_itemsize */
        0,                               /* tp_dealloc */
        0,                               /* tp_print */
        0,                               /* tp_getattr */
        0,                               /* tp_setattr */
        0,                               /* tp_reserved */
        0,                               /* tp_repr */
        0,                               /* tp_as_number */
        0,                               /* tp_as_sequence */
        0,                               /* tp_as_mapping */
        0,                               /* tp_hash  */
        0,                               /* tp_call */
        0,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASE_EXC_SUBCLASS,         /* tp_flags */
        "Chain tx cond subtype srv stake object",             /* tp_doc */
        0,                                 /* tp_traverse */
        0,                                 /* tp_clear */
        0,                                 /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,                                 /* tp_iter */
        0,                                 /* tp_iternext */
        0,       /* tp_methods */
        0,                               /* tp_members */
        DapChainTxOutCondSubtypeSrvStakeGetsSetsDef,                               /* tp_getset */
        &DapChainTxOutCondObjectType,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        (initproc)DapChainTxOutCondSubTypeSrvStake_init,       /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

int DapChainTxOutCondSubTypeSrvStake_init(PyDapChainTxOutCondObject* self, PyObject* args, PyObject *kwds){
    const char* kwlist[] = {
            "srvUID",
            "value",
            "feeValue",
            "feeAddr",
            "hldrAddr",
            "signingAddr",
            "signerNodeAddr",
            NULL
    };
    PyObject *obj_srv_uid;
    uint64_t l_value;
    long double l_fee_value;
    PyObject *obj_fee_addr;
    PyObject *obj_hldr_addr;
    PyObject *obj_signing_addr;
    PyObject *obj_signer_node_addr;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OkdOOOO", (char**)kwlist, &obj_srv_uid, &l_value, &l_fee_value,
                                     &obj_fee_addr, &obj_hldr_addr, &obj_signing_addr, &obj_signer_node_addr)){
        return -1;
    }
    uint256_t l_value_256 = dap_chain_uint256_from(l_value);
    self->out_cond = dap_chain_datum_tx_item_out_cond_create_srv_stake(
            ((PyDapChainNetSrvUIDObject*)obj_srv_uid)->net_srv_uid,
            l_value_256,
            l_fee_value,
            ((PyDapChainAddrObject*)obj_fee_addr)->addr,
            ((PyDapChainAddrObject*)obj_hldr_addr)->addr,
            ((PyDapChainAddrObject*)obj_signing_addr)->addr,
            ((PyDapChainNodeAddrObject*)obj_signer_node_addr)->node_addr);
    return 0;
}

PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *obj_net_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject, &DapChainNetSrvUidObjectType);
    PyObject_Dir((PyObject*)obj_net_srv_uid);
    obj_net_srv_uid->net_srv_uid = ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_stake.srv_uid;
    return (PyObject*)obj_net_srv_uid;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_addr(PyObject *self, void *closure){
    (void)closure;
    PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
    PyObject_Dir((PyObject*)obj_addr);
    obj_addr->addr = &((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_stake.fee_addr;
    return (PyObject*)obj_addr;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_stake_get_value(PyObject *self, void *closure){
    (void)closure;
    return PyLong_FromDouble(((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_stake.fee_value);
}
