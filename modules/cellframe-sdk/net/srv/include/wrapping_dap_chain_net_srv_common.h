
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