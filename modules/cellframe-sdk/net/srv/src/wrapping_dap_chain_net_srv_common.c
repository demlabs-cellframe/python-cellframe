#include "wrapping_dap_chain_net_srv_common.h"

/* Attribute for ChainNetSrvPrice */
#define WRAPPING_DAP_CHAIN_NET_SRV_PRICE(a) ((PyDapChainNetSrvPriceObject*) a)

PyObject *wrapping_dap_chain_net_srv_get_wallet(PyObject *self, void *closure){
    (void)closure;
    return Py_None;
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
    PyDapChainNetSrvPriceUnitUIDObject *l_price_unit_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject, &DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType);
    PyObject_Dir((PyObject*)l_price_unit_uid);
    l_price_unit_uid->price_unit_uid = WRAPPING_DAP_CHAIN_NET_SRV_PRICE(self)->price.units_uid;
    return (PyObject*)l_price_unit_uid;
}

/* wrapping dap_chain_net_srv_order_direction */

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
            &DapChainNetSrvOrderDirectionObject_DapChainNetSrvOrderDirectionObjectType);
    PyObject_Dir((PyObject*)l_obj);
    l_obj->direction = SERV_DIR_BUY;
    return (PyObject*)l_obj;
}
PyObject *wrapping_dap_chain_net_srv_order_direction_get_serv_dir_sell(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvOrderDirectionObject *l_obj = PyObject_New(
            PyDapChainNetSrvOrderDirectionObject,
            &DapChainNetSrvOrderDirectionObject_DapChainNetSrvOrderDirectionObjectType);
    PyObject_Dir((PyObject*)l_obj);
    l_obj->direction = SERV_DIR_SELL;
    return (PyObject*)l_obj;
}
PyObject *wrapping_dap_chain_net_srv_order_direction_get_serv_dir_undefined(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvOrderDirectionObject *l_obj = PyObject_New(
            PyDapChainNetSrvOrderDirectionObject,
            &DapChainNetSrvOrderDirectionObject_DapChainNetSrvOrderDirectionObjectType);
    PyObject_Dir((PyObject*)l_obj);
    l_obj->direction = SERV_DIR_UNDEFINED;
    return (PyObject*)l_obj;
}
