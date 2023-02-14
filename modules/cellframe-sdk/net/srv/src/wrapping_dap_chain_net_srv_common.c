#include "wrapping_dap_chain_net_srv_common.h"

/* Attribute for ChainNetSrvPrice */
#define WRAPPING_DAP_CHAIN_NET_SRV_PRICE(a) ((PyDapChainNetSrvPriceObject*) a)

PyGetSetDef DapChainNetSrvPrice_GetsSetsDef[] = {
        {"wallet", (getter)wrapping_dap_chain_net_srv_get_wallet, NULL, NULL, NULL},
        {"netName", (getter)wrapping_dap_chain_net_srv_get_net_name, NULL, NULL, NULL},
        {"net", (getter)wrapping_dap_chain_net_srv_get_net, NULL, NULL, NULL},
        {"valueDatoshi", (getter)wrapping_dap_chain_net_srv_get_value_datoshi, NULL, NULL, NULL},
        {"token", (getter)wrapping_dap_chain_net_srv_get_token, NULL, NULL, NULL},
        {"units", (getter)wrapping_dap_chain_net_srv_get_units, NULL, NULL, NULL},
        {"unitsUid", (getter)wrapping_dap_chain_net_srv_get_units_uid, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainNetSrvPriceObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetSrvPrice",        /* tp_name */
        sizeof(PyDapChainNetSrvPriceObject), /* tp_basicsize */
        0,                                /* tp_itemsize */
        0,                                /* tp_dealloc */
        0,                                /* tp_print */
        0,                                /* tp_getattr */
        0,                                /* tp_setattr */
        0,                                /* tp_reserved */
        0,                                /* tp_repr */
        0,                                /* tp_as_number */
        0,                                /* tp_as_sequence */
        0,                                /* tp_as_mapping */
        0,                                /* tp_hash  */
        0,                                /* tp_call */
        0,                                /* tp_str */
        0,                                /* tp_getattro */
        0,                                /* tp_setattro */
        0,                                /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,          /* tp_flags */
        "Chain net srv price object",               /* tp_doc */
        0,		                          /* tp_traverse */
        0,		                          /* tp_clear */
        0,		                          /* tp_richcompare */
        0,                                /* tp_weaklistoffset */
        0,		                          /* tp_iter */
        0,		                          /* tp_iternext */
        0,        /* tp_methods */
        0,                                /* tp_members */
        DapChainNetSrvPrice_GetsSetsDef,        /* tp_getset */
        0,                                /* tp_base */
        0,                                /* tp_dict */
        0,                                /* tp_descr_get */
        0,                                /* tp_descr_set */
        0,                                /* tp_dictoffset */
        0,                                /* tp_init */
        0,                                /* tp_alloc */
        PyType_GenericNew,                /* tp_new */
};

PyObject *wrapping_dap_chain_net_srv_get_wallet(PyObject *self, void *closure){
    (void)closure;
    return NULL;
}
PyObject *wrapping_dap_chain_net_srv_get_net_name(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", WRAPPING_DAP_CHAIN_NET_SRV_PRICE(self)->price.net_name);
}
PyObject *wrapping_dap_chain_net_srv_get_net(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetObject *l_obj_net = PyObject_New(PyDapChainNetObject, &DapChainNetObjectType);
    l_obj_net->chain_net = WRAPPING_DAP_CHAIN_NET_SRV_PRICE(self)->price.net;
    return (PyObject*)l_obj_net;
}
PyObject *wrapping_dap_chain_net_srv_get_value_datoshi(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_PRICE(self)->price.value_datoshi);
}
PyObject *wrapping_dap_chain_net_srv_get_token(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", WRAPPING_DAP_CHAIN_NET_SRV_PRICE(self)->price.token);
}
PyObject *wrapping_dap_chain_net_srv_get_units(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_PRICE(self)->price.units);
}
PyObject *wrapping_dap_chain_net_srv_get_units_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvPriceUnitUIDObject *l_price_unit_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject, &DapChainNetSrvPriceUnitUidObjectType);
    l_price_unit_uid->price_unit_uid = WRAPPING_DAP_CHAIN_NET_SRV_PRICE(self)->price.units_uid;
    return (PyObject*)l_price_unit_uid;
}

/* wrapping dap_chain_net_srv_order_direction */
PyMethodDef DapChainNetSrvOrderDirection_Methods[] = {
        {"getDirBuy", (PyCFunction)wrapping_dap_chain_net_srv_order_direction_get_serv_dir_buy, METH_NOARGS | METH_STATIC, ""},
        {"getDirSell", (PyCFunction)wrapping_dap_chain_net_srv_order_direction_get_serv_dir_sell, METH_NOARGS | METH_STATIC, ""},
        {"getDirUndefined", (PyCFunction)wrapping_dap_chain_net_srv_order_direction_get_serv_dir_undefined, METH_NOARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainNetSrvOrderDirectionObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetSrvOrderDirection",        /* tp_name */
        sizeof(PyDapChainNetSrvOrderDirectionObject), /* tp_basicsize */
        0,                                /* tp_itemsize */
        0,                                /* tp_dealloc */
        0,                                /* tp_print */
        0,                                /* tp_getattr */
        0,                                /* tp_setattr */
        0,                                /* tp_reserved */
        0,                                /* tp_repr */
        0,                                /* tp_as_number */
        0,                                /* tp_as_sequence */
        0,                                /* tp_as_mapping */
        0,                                /* tp_hash  */
        0,                                /* tp_call */
        DapChainNetSrvOrderDirection_str, /* tp_str */
        0,                                /* tp_getattro */
        0,                                /* tp_setattro */
        0,                                /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,          /* tp_flags */
        "Chain net srv order direction object",               /* tp_doc */
        0,		                          /* tp_traverse */
        0,		                          /* tp_clear */
        0,		                          /* tp_richcompare */
        0,                                /* tp_weaklistoffset */
        0,		                          /* tp_iter */
        0,		                          /* tp_iternext */
        DapChainNetSrvOrderDirection_Methods,        /* tp_methods */
        0,                                /* tp_members */
        0,        /* tp_getset */
        0,                                /* tp_base */
        0,                                /* tp_dict */
        0,                                /* tp_descr_get */
        0,                                /* tp_descr_set */
        0,                                /* tp_dictoffset */
        0,                                /* tp_init */
        0,                                /* tp_alloc */
        PyType_GenericNew,                /* tp_new */
};

PyObject *DapChainNetSrvOrderDirection_str(PyObject *self){
    char *ret;
    switch (((PyDapChainNetSrvOrderDirectionObject*)self)->direction) {
        case 1:
            ret = "SERV_DIR_BUY";
            break;
        case 2:
            ret = "SERV_DIR_SELL";
            break;
        default:
            ret = "SERV_DIR_UNDEFINED";
            break;
    }
    return Py_BuildValue("s", ret);
}

PyObject *wrapping_dap_chain_net_srv_order_direction_get_serv_dir_buy(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvOrderDirectionObject *l_obj = PyObject_New(
            PyDapChainNetSrvOrderDirectionObject,
            &DapChainNetSrvOrderDirectionObjectType);
    l_obj->direction = SERV_DIR_BUY;
    return (PyObject*)l_obj;
}
PyObject *wrapping_dap_chain_net_srv_order_direction_get_serv_dir_sell(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvOrderDirectionObject *l_obj = PyObject_New(
            PyDapChainNetSrvOrderDirectionObject,
            &DapChainNetSrvOrderDirectionObjectType);
    l_obj->direction = SERV_DIR_SELL;
    return (PyObject*)l_obj;
}
PyObject *wrapping_dap_chain_net_srv_order_direction_get_serv_dir_undefined(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvOrderDirectionObject *l_obj = PyObject_New(
            PyDapChainNetSrvOrderDirectionObject,
            &DapChainNetSrvOrderDirectionObjectType);
    l_obj->direction = SERV_DIR_UNDEFINED;
    return (PyObject*)l_obj;
}
