#pragma once

#include "Python.h"
#include "dap_chain_net_srv_order.h"
#include "wrapping_dap_chain_common.h"
#include "libdap_chain_net_python.h"
#include "wrapping_dap_chain_net_srv_common.h"
#include "datetime.h"
#include "libdap_crypto_key_python.h"

typedef struct PyDapChainNetSrvOrder{
    PyObject_HEAD
    dap_chain_net_srv_order_t *order;
    //dap_chain_net_srv_order_old_t *order_old;
}PyDapChainNetSrvOrderObject;

//constructor
int PyDapChainNetSrvOrder_init(PyDapChainNetSrvOrderObject *self, PyObject *args, PyObject *kwds);

//Attributes
PyObject *wrapping_dap_chain_net_srv_order_get_version(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_order_get_srv_uid(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_order_get_srv_direction(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_order_get_srv_node_addr(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_order_get_srv_tx_cond_hash(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price_unit(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ts_created(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ts_expires(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price_ticker(PyObject *self, void *closure);
//PyObject *wrapping_dap_chain_net_srv_order_get_srv_free_space(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ext_size(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ext(PyObject *self, void *closure);
//Functions
PyObject *wrapping_dap_chain_net_srv_order_get_size(PyObject *self, PyObject *args);
//PyObject *wrapping_dap_chain_net_srv_order_set_continent_region(PyObject *self, PyObject *args);
//PyObject *wrapping_dap_chain_net_srv_order_get_continent_region(PyObject *self, PyObject *args);

//PyObject *wrapping_dap_chain_net_srv_order_get_country_code(PyObject *self, PyObject *args);
//PyObject *wrapping_dap_chain_net_srv_order_continents_count(PyObject *self, PyObject *args);
//PyObject *wrapping_dap_chain_net_srv_order_continent_to_str(PyObject *self, PyObject *args);
//PyObject *wrapping_dap_chain_net_srv_order_continent_to_num(PyObject *self, PyObject *args);

PyObject *wrapping_dap_chain_net_srv_order_find(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_order_delete(PyObject *self, PyObject *args);

PyObject *wrapping_dap_chain_net_srv_order_find_all_by(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_order_save(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_order_get_gdb_group(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_order_get_nodelist_group(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_order_add_notify_callback(PyObject *self, PyObject *args);

static PyMethodDef DapChainNetSrvOrderMethods[]={
        {"size", (PyCFunction)wrapping_dap_chain_net_srv_order_get_size, METH_VARARGS, ""},
        {"find", (PyCFunction)wrapping_dap_chain_net_srv_order_find, METH_VARARGS | METH_STATIC, ""},
        {"delete", (PyCFunction)wrapping_dap_chain_net_srv_order_delete, METH_VARARGS | METH_STATIC, ""},
        {"save", (PyCFunction)wrapping_dap_chain_net_srv_order_save, METH_VARARGS, ""},
        {"getGdbGroup", (PyCFunction)wrapping_dap_chain_net_srv_order_get_gdb_group, METH_VARARGS | METH_STATIC, ""},
        {"getNodelistGroup", (PyCFunction)wrapping_dap_chain_net_srv_order_get_nodelist_group, METH_VARARGS | METH_STATIC, ""},
        {"addNotify", (PyCFunction)wrapping_dap_chain_net_srv_order_add_notify_callback, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

static PyGetSetDef DapChaiNetSrvOrderGetsSets[] = {
        {"version", (getter)wrapping_dap_chain_net_srv_order_get_version, NULL, NULL, NULL},
        {"uid", (getter)wrapping_dap_chain_net_srv_order_get_srv_uid, NULL, NULL, NULL},
        {"direction", (getter)wrapping_dap_chain_net_srv_order_get_srv_direction, NULL, NULL, NULL},
        {"nodeAddr", (getter)wrapping_dap_chain_net_srv_order_get_srv_node_addr, NULL, NULL, NULL},
        {"condHash", (getter)wrapping_dap_chain_net_srv_order_get_srv_tx_cond_hash, NULL, NULL, NULL},
        {"priceUnit", (getter)wrapping_dap_chain_net_srv_order_get_srv_price_unit, NULL, NULL, NULL},
        {"tsCreated", (getter)wrapping_dap_chain_net_srv_order_get_srv_ts_created, NULL, NULL, NULL},
        {"tsExpires", (getter)wrapping_dap_chain_net_srv_order_get_srv_ts_expires, NULL, NULL, NULL},
        {"srvPrice", (getter)wrapping_dap_chain_net_srv_order_get_srv_price, NULL, NULL, NULL},
        {"srvPriceTicker", (getter)wrapping_dap_chain_net_srv_order_get_srv_price_ticker, NULL, NULL, NULL},
        {"extSize", (getter)wrapping_dap_chain_net_srv_order_get_srv_ext_size, NULL, NULL, NULL},
        {"ext", (getter)wrapping_dap_chain_net_srv_order_get_srv_ext, NULL, NULL, NULL},
        {NULL}
};

static PyTypeObject DapChainNetSrvOrderObject_DapChainNetSrvOrderObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetSrvOrder",        /* tp_name */
        sizeof(PyDapChainNetSrvOrderObject), /* tp_basicsize */
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
        "Chain net srv client object",               /* tp_doc */
        0,		                          /* tp_traverse */
        0,		                          /* tp_clear */
        0,		                          /* tp_richcompare */
        0,                                /* tp_weaklistoffset */
        0,		                          /* tp_iter */
        0,		                          /* tp_iternext */
        DapChainNetSrvOrderMethods,        /* tp_methods */
        0,                                /* tp_members */
        DapChaiNetSrvOrderGetsSets,        /* tp_getset */
        0,                                /* tp_base */
        0,                                /* tp_dict */
        0,                                /* tp_descr_get */
        0,                                /* tp_descr_set */
        0,                                /* tp_dictoffset */
        (initproc)PyDapChainNetSrvOrder_init, /* tp_init */
        0,                                /* tp_alloc */
        PyType_GenericNew,                /* tp_new */
};
