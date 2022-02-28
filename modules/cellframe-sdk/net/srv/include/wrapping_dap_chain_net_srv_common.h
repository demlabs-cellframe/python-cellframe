#pragma once


#include "Python.h"
#include "dap_chain_net_srv_common.h"
#include "wrapping_dap_chain_common.h"
#include "libdap_chain_net_python.h"

/*wrapping dap_chain_net_srv_price*/
typedef struct PyDapChainNetSrvPrice{
    PyObject_HEAD
    dap_chain_net_srv_price_t price;
}PyDapChainNetSrvPriceObject;

PyObject *wrapping_dap_chain_net_srv_get_wallet(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_get_net_name(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_get_net(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_get_value_datoshi(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_get_token(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_get_units(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_get_units_uid(PyObject *self, void *closure);

static PyGetSetDef DapChainNetSrvPrice_GetsSetsDef[] = {
        {"wallet", (getter)wrapping_dap_chain_net_srv_get_wallet, NULL, NULL, NULL},
        {"netName", (getter)wrapping_dap_chain_net_srv_get_net_name, NULL, NULL, NULL},
        {"net", (getter)wrapping_dap_chain_net_srv_get_net, NULL, NULL, NULL},
        {"valueDatoshi", (getter)wrapping_dap_chain_net_srv_get_value_datoshi, NULL, NULL, NULL},
        {"token", (getter)wrapping_dap_chain_net_srv_get_token, NULL, NULL, NULL},
        {"units", (getter)wrapping_dap_chain_net_srv_get_units, NULL, NULL, NULL},
        {"unitsUid", (getter)wrapping_dap_chain_net_srv_get_units_uid, NULL, NULL, NULL},
        {NULL}
};

static PyTypeObject DapChainNetSrvPriceObject_DapChainNetSrvPriceObjectType = {
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

/*wrapping dap_chain_net_srv_order_direction*/
typedef struct PyDapChainNetSrvOrderDirection{
    PyObject_HEAD
    dap_chain_net_srv_order_direction_t direction;
}PyDapChainNetSrvOrderDirectionObject;

PyObject *DapChainNetSrvOrderDirection_str(PyObject *self);

PyObject *wrapping_dap_chain_net_srv_order_direction_get_serv_dir_buy(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_order_direction_get_serv_dir_sell(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_order_direction_get_serv_dir_undefined(PyObject *self, PyObject *args);

static PyMethodDef DapChainNetSrvOrderDirection_Methods[] = {
        {"getDirBuy", (PyCFunction)wrapping_dap_chain_net_srv_order_direction_get_serv_dir_buy, METH_NOARGS | METH_STATIC, ""},
        {"getDirSell", (PyCFunction)wrapping_dap_chain_net_srv_order_direction_get_serv_dir_sell, METH_NOARGS | METH_STATIC, ""},
        {"getDirUndefined", (PyCFunction)wrapping_dap_chain_net_srv_order_direction_get_serv_dir_undefined, METH_NOARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainNetSrvOrderDirectionObject_DapChainNetSrvOrderDirectionObjectType = {
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
        "Chain net service order direction object",               /* tp_doc */
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

/* Chain net srv pkt type */
//typedef struct PyDapChainNetSrvPKTType{
//    PyObject_HEAD
//    uint8_t type;
//}PyDapChainNetSrvPKTTypeObject;
//
//PyObject *PyDapChainNetSrvPKTTypeObject_str(PyObject *self);
//
//
//
//static PyMethodDef DapChainNetSrvPKTType_Methods[] = {
//        {NULL, NULL, 0, NULL}
//};
//
//static PyTypeObject PyDapChainNetSrvPKTTypeObject_PyDapChainNetSrvPKTTypeObjectType = {
//        PyVarObject_HEAD_INIT(NULL, 0)
//        "CellFrame.ChainNetSrvPKTType",        /* tp_name */
//        sizeof(PyDapChainNetSrvPKTTypeObject), /* tp_basicsize */
//        0,                                /* tp_itemsize */
//        0,                                /* tp_dealloc */
//        0,                                /* tp_print */
//        0,                                /* tp_getattr */
//        0,                                /* tp_setattr */
//        0,                                /* tp_reserved */
//        0,                                /* tp_repr */
//        0,                                /* tp_as_number */
//        0,                                /* tp_as_sequence */
//        0,                                /* tp_as_mapping */
//        0,                                /* tp_hash  */
//        0,                                /* tp_call */
//        PyDapChainNetSrvPKTTypeObject_str, /* tp_str */
//        0,                                /* tp_getattro */
//        0,                                /* tp_setattro */
//        0,                                /* tp_as_buffer */
//        Py_TPFLAGS_DEFAULT |
//        Py_TPFLAGS_BASETYPE,          /* tp_flags */
//        "Chain net srv PKT type",               /* tp_doc */
//        0,		                          /* tp_traverse */
//        0,		                          /* tp_clear */
//        0,		                          /* tp_richcompare */
//        0,                                /* tp_weaklistoffset */
//        0,		                          /* tp_iter */
//        0,		                          /* tp_iternext */
//        DapChainNetSrvPKTType_Methods,        /* tp_methods */
//        0,                                /* tp_members */
//        0,        /* tp_getset */
//        0,                                /* tp_base */
//        0,                                /* tp_dict */
//        0,                                /* tp_descr_get */
//        0,                                /* tp_descr_set */
//        0,                                /* tp_dictoffset */
//        0,                                /* tp_init */
//        0,                                /* tp_alloc */
//        PyType_GenericNew,                /* tp_new */
//};