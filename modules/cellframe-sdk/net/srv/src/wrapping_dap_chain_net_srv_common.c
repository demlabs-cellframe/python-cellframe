#include "wrapping_dap_chain_net_srv_common.h"

/* Attribute for ChainNetSrvPrice */
#define WRAPPING_DAP_CHAIN_NET_SRV_PRICE(a) ((PyDapChainNetSrvPriceObject*) a)

PyObject *wrapping_dap_chain_net_srv_get_wallet(PyObject *self, void *closure){
    (void)closure;
}
PyObject *wrapping_dap_chain_net_srv_get_net_name(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", WRAPPING_DAP_CHAIN_NET_SRV_PRICE(self)->price.net_name);
}
PyObject *wrapping_dap_chain_net_srv_get_net(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetObject *l_obj_net = PyObject_New(PyDapChainNetObject, &DapChainNetObject_DapChainNetObjectType);
    PyObject_Dir((PyObject*)l_obj_net);
    l_obj_net->chain_net = WRAPPING_DAP_CHAIN_NET_SRV_PRICE(self)->price.net;
    return l_obj_net;
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
    PyDapChainNetSrvPriceUnitUIDObject *l_price_unit_uid = PyObject_New(PyDapChainNetSrvPriceObject, &DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType);
    PyObject_Dir((PyObject*)l_price_unit_uid);
    l_price_unit_uid->price_unit_uid = WRAPPING_DAP_CHAIN_NET_SRV_PRICE(self)->price.units_uid;
    return (PyObject*)l_price_unit_uid;
}
