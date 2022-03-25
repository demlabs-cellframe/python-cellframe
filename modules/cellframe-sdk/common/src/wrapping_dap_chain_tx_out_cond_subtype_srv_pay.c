#include "wrapping_dap_chain_tx_out_cond_subtype_srv_pay.h"

PyGetSetDef DapChainTxOutCondSubtypeStvPayGetsSetsDef[]={
        {"unit", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_unit,NULL,NULL,NULL},
        {"uid", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_uid,NULL,NULL,NULL},
        {"pkeyHash", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_pkey,NULL,NULL,NULL},
        {"maxPrice", (getter)wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_map_price,NULL,NULL,NULL},
        {NULL}
};

PyTypeObject DapChainTxOutCondSubTypeSrvPayObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxOutCondSubTypeSrvPay",       /* tp_name */
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
        "Chain tx cond subtype srv pay object",             /* tp_doc */
        0,                                 /* tp_traverse */
        0,                                 /* tp_clear */
        0,                                 /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,                                 /* tp_iter */
        0,                                 /* tp_iternext */
        0,       /* tp_methods */
        0,                               /* tp_members */
        DapChainTxOutCondSubtypeStvPayGetsSetsDef,                               /* tp_getset */
        &DapChainTxOutCondObjectType,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        (initproc)DapChainTxOutCondSubTypeSrvPay_init,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

int DapChainTxOutCondSubTypeSrvPay_init(PyDapChainTxOutCondObject* self, PyObject *arg, PyObject *kwds){
    const char *kwlist[] = {
            "pkey",
            "srvUID",
            "value",
            "valueMaxPerUnit",
            "unit",
            "params",
            NULL
    };
    PyObject *obj_pkey;
    PyObject *obj_srv_uid;
    uint64_t l_value;
    uint64_t l_value_max_per_unit;
    PyObject *obj_unit;
    PyObject *obj_params;
    if (!PyArg_ParseTupleAndKeywords(arg, kwds, "OOkkOO", (char**)kwlist, &obj_pkey, &obj_srv_uid, &l_value, &l_value_max_per_unit, &obj_unit, &obj_params)){
        return -1;
    }
    uint256_t l_value_256 = dap_chain_uint256_from(l_value);
    uint256_t l_value_max_per_unit_256 = dap_chain_uint256_from(l_value_max_per_unit);
    void *l_params = PyBytes_AsString(obj_params);
    size_t l_params_size = PyBytes_Size(obj_params);
    self->out_cond = dap_chain_datum_tx_item_out_cond_create_srv_pay(
            ((PyDapPkeyObject*)obj_pkey)->pkey,
            ((PyDapChainNetSrvUIDObject*)obj_srv_uid)->net_srv_uid,
            l_value_256, l_value_max_per_unit_256,
            ((PyDapChainNetSrvPriceUnitUIDObject*)obj_unit)->price_unit_uid, l_params, l_params_size);
    return 0;
}

PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_unit(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_pay.unit.uint32);
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *obj_net_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject, &DapChainNetSrvUidObjectType);
    PyObject_Dir((PyObject*)obj_net_srv_uid);
    obj_net_srv_uid->net_srv_uid = ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_pay.srv_uid;
    return (PyObject*)obj_net_srv_uid;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_pkey(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    PyObject_Dir((PyObject*)obj_hash);
    obj_hash->hash_fast = &((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_pay.pkey_hash;
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_chain_tx_out_cond_subtype_srv_pay_get_map_price(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTxOutCondObject*)self)->out_cond->subtype.srv_pay.unit_price_max_datoshi);
}