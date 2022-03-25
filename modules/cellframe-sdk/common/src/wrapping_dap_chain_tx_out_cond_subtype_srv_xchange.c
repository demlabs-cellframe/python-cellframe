#include "wrapping_dap_chain_tx_out_cond_subtype_srv_xchange.h"

PyGetSetDef DapChainTxOutCondSubtypeSrvXchangeGetsSetsDef[]={
        {"uid", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_uid,NULL,NULL,NULL},
        {"netId", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_net_id,NULL,NULL,NULL},
        {"token", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_token,NULL,NULL,NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_value,NULL,NULL,NULL},
        {NULL}
};

PyTypeObject DapChainTxOutCondSubTypeSrvXchangeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxOutCondSubTypeSrvXchange",       /* tp_name */
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
        "Chain tx cond subtype srv xchange object",             /* tp_doc */
        0,                                 /* tp_traverse */
        0,                                 /* tp_clear */
        0,                                 /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,                                 /* tp_iter */
        0,                                 /* tp_iternext */
        0,       /* tp_methods */
        0,                               /* tp_members */
        DapChainTxOutCondSubtypeSrvXchangeGetsSetsDef,                               /* tp_getset */
        &DapChainTxOutCondObjectType,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        (initproc)DapChainTxOutCondSubTypeSrvXchange,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

int DapChainTxOutCondSubTypeSrvXchange(PyDapChainTxOutCondObject *self, PyObject *arg, PyObject *kwds){
    const char *kwlist[] = {
            "srvUID",
            "netID",
            "token",
            "value",
            "params",
            NULL
    };
    PyObject *obj_srv_uid;
    PyObject *obj_net_id;
    char *l_token;
    uint64_t l_value;
    PyObject *obj_params;
    if (!PyArg_ParseTupleAndKeywords(arg, kwds, "OOskO", (char**)kwlist,
                                     &obj_srv_uid, &obj_net_id, &l_token, &l_value, &obj_params)){
        return -1;
    }
    uint256_t l_value_256 = dap_chain_uint256_from(l_value);
    void *l_params = PyBytes_AsString(obj_params);
    size_t l_params_size = PyBytes_Size(obj_params);
    self->out_cond = dap_chain_datum_tx_item_out_cond_create_srv_xchange(
            ((PyDapChainNetSrvUIDObject*)obj_srv_uid)->net_srv_uid,
            ((PyDapChainNetIdObject*)obj_net_id)->net_id,
            l_token,
            l_value_256,
            l_params, l_params_size);
    return 0;
}

PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *obj_net_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject, &DapChainNetSrvUidObjectType);
    PyObject_Dir((PyObject*)obj_net_srv_uid);
    obj_net_srv_uid->net_srv_uid = ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_stake.srv_uid;
    return (PyObject*)obj_net_srv_uid;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_net_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetIdObject *obj_net_id = PyObject_New(PyDapChainNetIdObject, &DapChainNetIdObjectType);
    PyObject_Dir((PyObject*)obj_net_id);
    obj_net_id->net_id = ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_xchange.net_id;
    return (PyObject*)obj_net_id;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_token(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_xchange.token);
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_xchange_get_value(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_xchange.value);
}
