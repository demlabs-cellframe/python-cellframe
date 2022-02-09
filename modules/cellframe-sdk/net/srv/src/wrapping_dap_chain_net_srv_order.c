#include "wrapping_dap_chain_net_srv_order.h"

#define WRAPPING_DAP_CHAIN_NET_SRV_ORDER(a) ((PyDapChainNetSrvOrderObject*)a)

int PyDapChainNetSrvOrder_init(PyDapChainNetSrvOrderObject *self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "net",
            "direction",
            "srvUID",
            "nodeAddr",
            "txCondHash",
            "price",
            "priceUnit",
            "priceTicker",
            "expires",
            "ext",
//            "extSize",
//            "region",
//            "continentNum",
            "key",
            NULL
    };
    PyObject *obj_net, *obj_direction, *obj_srv_uid, *obj_node_addr, *obj_tx_cond_hash, *obj_price_unit;
    uint64_t price;
    char *price_ticker;
    unsigned long expires;
    PyObject *obj_ext, *obj_key;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOOOOkOOO", kwlist, &obj_net, &obj_direction, &obj_srv_uid,
                                     &obj_node_addr, &obj_tx_cond_hash, &price, &obj_price_unit, &price_ticker,
                                     &expires, &obj_ext, &obj_key)){
        return -1;
    }
    void* l_ext = (void*)PyBytes_AsString(obj_ext);
    size_t l_ext_size = PyBytes_Size(obj_ext);
    dap_chain_net_srv_order_create(
            ((PyDapChainNetObject*)obj_net)->chain_net,
            ((PyDapChainNetSrvOrderDirectionObject*)obj_direction)->direction,
            ((PyDapChainNetSrvUIDObject*)obj_srv_uid)->net_srv_uid,
            *((PyDapChainNodeAddrObject*)obj_node_addr)->node_addr,
            *((PyDapHashFastObject*)obj_tx_cond_hash)->hash_fast,
            price,
            ((PyDapChainNetSrvPriceUnitUIDObject*)obj_price_unit)->price_unit_uid,
            price_ticker,
            (time_t)expires,
            l_ext,
            l_ext_size,
            "",
            0,
            ((PyCryptoKeyObject*)obj_key)->key
            );
    return 0;
}
PyObject *wrapping_dap_chain_net_srv_order_get_version(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("H", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->version);
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *obj_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject, &DapChainNetSrvUIDObject_DapChainNetSrvUIDObjectType);
    PyObject_Dir((PyObject*)obj_srv_uid);
    obj_srv_uid->net_srv_uid = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->srv_uid;
    return (PyObject*)obj_srv_uid;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_direction(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvOrderDirectionObject *srv_direction = PyObject_New(PyDapChainNetSrvOrderDirectionObject, &DapChainNetSrvOrderDirectionObject_DapChainNetSrvOrderDirectionObjectType);
    PyObject_Dir((PyObject*)srv_direction);
    srv_direction->direction = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->direction;
    return (PyObject*)srv_direction;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_node_addr(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNodeAddrObject *l_obj_node_addr = PyObject_New(PyDapChainNodeAddrObject, &DapChainNodeAddrObject_DapChainNodeAddrObjectType);
    PyObject_Dir((PyObject*)l_obj_node_addr);
    l_obj_node_addr->node_addr = &WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->node_addr;
    return (PyObject*)l_obj_node_addr;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_tx_cond_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *l_obj_hf = PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
    PyObject_Dir((PyObject*)l_obj_hf);
    l_obj_hf->hash_fast = &WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->tx_cond_hash;
    return (PyObject*)l_obj_hf;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price_unit(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvPriceUnitUIDObject *l_obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject, &DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType);
    PyObject_Dir((PyObject*)l_obj_srv_price_uid);
    l_obj_srv_price_uid->price_unit_uid = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->price_unit;
    return (PyObject*)l_obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ts_created(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ts_created);
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ts_expires(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ts_expires);
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->price);
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price_ticker(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->price_ticker);
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
        dap_sign_t *l_sign = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ext[WRAPPING_DAP_CHAIN_NET_SRV_ORDER(
                self)->order->ext_size];
        if (dap_sign_verify_size(l_sign, WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ext_size)) {
            PyDapSignObject *obj_sign = PyObject_New(PyDapSignObject, &DapSignObject_DapSignObjectType);
            PyObject_Dir((PyObject *) obj_sign);
            obj_sign->sign = l_sign;
            return (PyObject *) obj_sign;
        }
        return Py_None;
    }
    return Py_None;
}

//Functions
PyObject *wrapping_dap_chain_net_srv_order_get_size(PyObject *self, PyObject *args){
    (void)args;
    return Py_BuildValue("n", dap_chain_net_srv_order_get_size(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order));
}
//PyObject *wrapping_dap_chain_net_srv_order_set_continent_region(PyObject *self, PyObject *args){}
//PyObject *wrapping_dap_chain_net_srv_order_get_continent_region(PyObject *self, PyObject *args){}

//PyObject *wrapping_dap_chain_net_srv_order_get_country_code(PyObject *self, PyObject *args){}
//PyObject *wrapping_dap_chain_net_srv_order_continents_count(PyObject *self, PyObject *args){}
//PyObject *wrapping_dap_chain_net_srv_order_continent_to_str(PyObject *self, PyObject *args){}
//PyObject *wrapping_dap_chain_net_srv_order_continent_to_num(PyObject *self, PyObject *args){}

PyObject *wrapping_dap_chain_net_srv_order_find(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_net;
    PyObject *obj_order_hash;
    if (!PyArg_ParseTuple(args, "OO", &obj_net, &obj_order_hash)){
        PyErr_SetString(PyExc_ValueError, "This function must take two arguments. ");
        return NULL;
    }
    dap_chain_net_srv_order_t *l_order = NULL;
    if(!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_ValueError, "As the first argument, this function takes "
                                          "an instance of an object of type ChainNet.");
        return NULL;
    }
    if (PyUnicode_Check(obj_order_hash)){
        const char *l_str = PyUnicode_AsUTF8(obj_order_hash);
        l_order = dap_chain_net_srv_order_find_by_hash_str(((PyDapChainNetObject*)obj_net)->chain_net, l_str);
        if (l_order == NULL){
            return Py_None;
        }
        PyDapChainNetSrvOrderObject *l_obj_order = PyObject_New(PyDapChainNetSrvOrderObject, &DapChainNetSrvOrderObject_DapChainNetSrvOrderObjectType);
        PyObject_Dir((PyObject *)l_obj_order);
        l_obj_order->order = l_order;
        return (PyObject*)l_obj_order;
    }
    if (PyDapHashFast_Check(obj_order_hash)){
        l_order = dap_chain_net_srv_order_find_by_hash(((PyDapChainNetObject*)obj_net)->chain_net,
                                                       ((PyDapHashFastObject*)obj_order_hash)->hash_fast);
        if (l_order == NULL){
            return Py_None;
        }
        PyDapChainNetSrvOrderObject *l_obj_order = PyObject_New(PyDapChainNetSrvOrderObject, &DapChainNetSrvOrderObject_DapChainNetSrvOrderObjectType);
        PyObject_Dir((PyObject *)l_obj_order);
        l_obj_order->order = l_order;
        return (PyObject*)l_obj_order;
    }
    PyErr_SetString(PyExc_ValueError, "An invalid argument type was passed to this function. "
                                      "This function can take a string or an object of the HashFast type "
                                      "as the second argument. ");
    return NULL;
}

PyObject *wrapping_dap_chain_net_srv_order_delete(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_net;
    PyObject *obj_order_hash;
    if (!PyArg_ParseTuple(args, "OO", &obj_net, &obj_order_hash)){
        PyErr_SetString(PyExc_ValueError, "This function must take two arguments. ");
        return NULL;
    }
    if(!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_ValueError, "As the first argument, this function takes "
                                          "an instance of an object of type ChainNet.");
        return NULL;
    }
    int res = -1;
    if (PyUnicode_Check(obj_order_hash)){
        const char *l_str = PyUnicode_AsUTF8(obj_order_hash);
        res = dap_chain_net_srv_order_delete_by_hash_str(((PyDapChainNetObject*)obj_net)->chain_net,
                                                             l_str);
        return Py_BuildValue("i", res);
    }
    if (PyDapHashFast_Check(obj_order_hash)) {
        res =dap_chain_net_srv_order_delete_by_hash(((PyDapChainNetObject*)obj_net)->chain_net,
                                               ((PyDapHashFastObject*)obj_order_hash)->hash_fast);
        return Py_BuildValue("i", res);
    }
    PyErr_SetString(PyExc_ValueError, "An invalid argument type was passed to this function. "
                                      "This function can take a string or an object of the HashFast type "
                                      "as the second argument. ");
    return NULL;
}

PyObject *wrapping_dap_chain_net_srv_order_find_all_by(PyObject *self, PyObject *args){return Py_None;}
PyObject *wrapping_dap_chain_net_srv_order_save(PyObject *self, PyObject *args){
    PyObject *obj_net;
    if(!PyArg_ParseTuple(args, "O", &obj_net)){
        PyErr_SetString(PyExc_ValueError, "This function must take one arguments. ");
        return NULL;
    }
    if(!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_ValueError, "As the first argument, this function takes "
                                          "an instance of an object of type ChainNet.");
        return NULL;
    }
    int res = -1;
    res = dap_chain_net_srv_order_save(((PyDapChainNetObject *) self)->chain_net,
                                           WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order);
    return Py_BuildValue("i", res);
}
PyObject *wrapping_dap_chain_net_srv_order_get_gdb_group(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_net;
    if(!PyArg_ParseTuple(args, "O", &obj_net)){
        PyErr_SetString(PyExc_ValueError, "This function must take one arguments. ");
        return NULL;
    }
    if(!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_ValueError, "As the first argument, this function takes "
                                          "an instance of an object of type ChainNet.");
        return NULL;
    }
    return Py_BuildValue("s", dap_chain_net_srv_order_get_gdb_group(((PyDapChainNetObject*)obj_net)->chain_net));
}
PyObject *wrapping_dap_chain_net_srv_order_get_nodelist_group(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_net;
    if(!PyArg_ParseTuple(args, "O", &obj_net)){
        PyErr_SetString(PyExc_ValueError, "This function must take one arguments. ");
        return NULL;
    }
    if(!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_ValueError, "As the first argument, this function takes "
                                          "an instance of an object of type ChainNet.");
        return NULL;
    }
    return Py_BuildValue("s",
                         dap_chain_net_srv_order_get_nodelist_group(((PyDapChainNetObject*)obj_net)->chain_net));
}
