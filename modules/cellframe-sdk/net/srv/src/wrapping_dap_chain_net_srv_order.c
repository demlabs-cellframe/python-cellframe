#include "wrapping_dap_chain_net_srv_order.h"

#define WRAPPING_DAP_CHAIN_NET_SRV_ORDER(a) ((PyDapChainNetSrvOrderObject*)a)

PyObject *wrapping_dap_chain_net_srv_order_get_version(PyObject *self, void *closure){
    (void)closure;
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("H", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order_old->version);
    }else{
        return Py_BuildValue("H", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->version);
    }
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *obj_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject, &DapChainNetSrvUIDObject_DapChainNetSrvUIDObjectType);
    PyObject_Dir((PyObject*)obj_srv_uid);
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        obj_srv_uid->net_srv_uid = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order_old->srv_uid;
    }else{
        obj_srv_uid->net_srv_uid = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->srv_uid;
    }
    return (PyObject*)obj_srv_uid;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_direction(PyObject *self, void *closure){
    (void)closure;
    //TODO
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){}else{}
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_node_addr(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNodeAddrObject *l_obj_node_addr = PyObject_New(PyDapChainNodeAddrObject, &DapChainNodeAddrObject_DapChainNodeAddrObjectType);
    PyObject_Dir((PyObject*)l_obj_node_addr);
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        l_obj_node_addr->node_addr = &WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order_old->node_addr;
    }else{
        l_obj_node_addr->node_addr = &WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->node_addr;
    }
    return (PyObject*)l_obj_node_addr;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_tx_cond_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *l_obj_hf = PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
    PyObject_Dir((PyObject*)l_obj_hf);
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        l_obj_hf->hash_fast = &WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order_old->tx_cond_hash;
    }else{
        l_obj_hf->hash_fast = &WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->tx_cond_hash;
    }
    return (PyObject*)l_obj_hf;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price_unit(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvPriceUnitUIDObject *l_obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject, &DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType);
    PyObject_Dir((PyObject*)l_obj_srv_price_uid);
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        l_obj_srv_price_uid->price_unit_uid = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order_old->price_unit;
    }else{
        l_obj_srv_price_uid->price_unit_uid = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->price_unit;
    }
    return (PyObject*)l_obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ts_created(PyObject *self, void *closure){
    (void)closure;
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order_old->ts_created);
    }else{
        return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ts_created);
    }
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ts_expires(PyObject *self, void *closure){
    (void)closure;
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order_old->ts_expires);
    }else{
        return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ts_expires);
    }
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price(PyObject *self, void *closure){
    (void)closure;
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order_old->price);
    }else{
        return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->price);
    }
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price_ticker(PyObject *self, void *closure){
    (void)closure;
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("s", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order_old->price_ticker);
    }else{
        return Py_BuildValue("s", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->price_ticker);
    }
}
//PyObject *wrapping_dap_chain_net_srv_order_get_srv_free_space(PyObject *self, void *closure){}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ext_size(PyObject *self, void *closure){
    (void)closure;
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("I", 0);
    }else{
        return Py_BuildValue("I", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ext_size);
    }
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ext_n_sign(PyObject *self, void *closure) {
    (void) closure;
    if (WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order != NULL) {
        dap_sign_t *l_sign = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ext_n_sign[WRAPPING_DAP_CHAIN_NET_SRV_ORDER(
                self)->order->ext_size];
        if (dap_sign_verify_size(l_sign, WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ext_size)) {
            PyDapSignObject *obj_sign = PyObject_New(PyDapSignObject, &DapSignObject_DapSignObjectType);
            PyObject_Dir((PyObject *) obj_sign);
            obj_sign->sign = l_sign;
            return (PyObject *) obj_sign;
        }
        return Py_None;
    }
}

//Functions
PyObject *wrapping_dap_chain_net_srv_order_get_size(PyObject *self, PyObject *args){
    (void)args;
    if (WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("n", dap_chain_net_srv_order_get_size(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order_old));
    }else {
        return Py_BuildValue("n", dap_chain_net_srv_order_get_size(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order));
    }
}
//PyObject *wrapping_dap_chain_net_srv_order_set_continent_region(PyObject *self, PyObject *args){}
//PyObject *wrapping_dap_chain_net_srv_order_get_continent_region(PyObject *self, PyObject *args){}

//PyObject *wrapping_dap_chain_net_srv_order_get_country_code(PyObject *self, PyObject *args){}
//PyObject *wrapping_dap_chain_net_srv_order_continents_count(PyObject *self, PyObject *args){}
//PyObject *wrapping_dap_chain_net_srv_order_continent_to_str(PyObject *self, PyObject *args){}
//PyObject *wrapping_dap_chain_net_srv_order_continent_to_num(PyObject *self, PyObject *args){}

PyObject *wrapping_dap_chain_net_srv_order_find_by_hash_str(PyObject *self, PyObject *args){}

PyObject *wrapping_dap_chain_net_srv_order_find_by_hash(PyObject *self, PyObject *args){}

PyObject *wrapping_dap_chain_net_srv_order_find_all_by(PyObject *self, PyObject *args){}
PyObject *wrapping_dap_chain_net_srv_order_delete_by_hash_str(PyObject *self, PyObject *args){}
PyObject *wrapping_dap_chain_net_srv_order_delete_by_hash(PyObject *self, PyObject *args){}
PyObject *wrapping_dap_chain_net_srv_order_create(PyObject *self, PyObject *args){}
PyObject *wrapping_dap_chain_net_srv_order_save(PyObject *self, PyObject *args){}
PyObject *wrapping_dap_chain_net_srv_order_get_gdb_group(PyObject *self, PyObject *args){}
PyObject *wrapping_dap_chain_net_srv_order_get_nodelist_group(PyObject *self, PyObject *args){}
