#include "wrapping_dap_chain_net_srv_order.h"

#define WRAPPING_DAP_CHAIN_NET_SRV_ORDER(a) ((PyDapChainNetSrvOrderObject*)a)

PyMethodDef DapChainNetSrvOrderMethods[]={
        {"size", (PyCFunction)wrapping_dap_chain_net_srv_order_get_size, METH_VARARGS, ""},
        {"find", (PyCFunction)wrapping_dap_chain_net_srv_order_find, METH_VARARGS | METH_STATIC, ""},
        {"delete", (PyCFunction)wrapping_dap_chain_net_srv_order_delete, METH_VARARGS | METH_STATIC, ""},
        {"save", (PyCFunction)wrapping_dap_chain_net_srv_order_save, METH_VARARGS, ""},
        {"getGdbGroup", (PyCFunction)wrapping_dap_chain_net_srv_order_get_gdb_group, METH_VARARGS | METH_STATIC, ""},
        {"getNodelistGroup", (PyCFunction)wrapping_dap_chain_net_srv_order_get_nodelist_group, METH_VARARGS | METH_STATIC, ""},
        {"addNotify", (PyCFunction)wrapping_dap_chain_net_srv_order_add_notify_callback, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyGetSetDef DapChaiNetSrvOrderGetsSets[] = {
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
        {"extSign", (getter)wrapping_dap_chain_net_srv_order_get_srv_ext_n_sign, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainNetSrvOrderObjectType = {
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


typedef struct _wrapping_order_callable{
    PyObject *func;
    PyObject *arg;
}_wrapping_order_callable_t;

void _wrapping_handler_add_order_notify(void * a_arg, const char a_op_code, const char * a_group,
                                        const char * a_key, const void * a_value, const size_t a_value_len){
    if (!a_arg)
        return;
    _wrapping_order_callable_t *l_callback = (_wrapping_order_callable_t *)a_arg;
    PyGILState_STATE state = PyGILState_Ensure();
    PyDapChainNetSrvOrderObject *l_obj_order = (PyDapChainNetSrvOrderObject *)Py_None;
    if (a_value_len != 0 && a_op_code != DAP_DB$K_OPTYPE_DEL) {
        l_obj_order = PyObject_New(PyDapChainNetSrvOrderObject, &DapChainNetSrvOrderObjectType);
        l_obj_order->order = DAP_NEW_Z_SIZE(void, a_value_len);
        memcpy(l_obj_order->order, a_value, a_value_len);
    }
    char l_op_code[2];
    l_op_code[0] = a_op_code;
    l_op_code[1] = '\0';
    PyObject *l_args = Py_BuildValue("sssOO", l_op_code, a_group, a_key, l_obj_order, l_callback->arg);
    PyEval_CallObject(l_callback->func, l_args);
    Py_DECREF(l_args);
    PyGILState_Release(state);
}

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
    PyObject *obj_direction, *obj_srv_uid, *obj_node_addr, *obj_tx_cond_hash, *obj_price_unit;
    PyDapChainNetObject *obj_net;
    uint64_t price;
    char *price_ticker;
    unsigned long expires;
    PyObject *obj_ext, *obj_key;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOOOOkOsOOO", (char **)kwlist, &obj_net, &obj_direction, &obj_srv_uid,
                                     &obj_node_addr, &obj_tx_cond_hash, &price, &obj_price_unit, &price_ticker,
                                     &expires, &obj_ext, &obj_key)){
        return -1;
    }
    if (!PyDapChainNet_Check(obj_net)){
        return -1;
    }
    void* l_ext = (void*)PyBytes_AsString(obj_ext);
    size_t l_ext_size = PyBytes_Size(obj_ext);
    uint256_t l_price = dap_chain_uint256_from(price);
    dap_chain_hash_fast_t l_hf = {};
    if (obj_tx_cond_hash != Py_None)
        l_hf = *((PyDapHashFastObject *)obj_tx_cond_hash)->hash_fast;
    self->order = dap_chain_net_srv_order_compose(
            ((PyDapChainNetObject *) obj_net)->chain_net,
            ((PyDapChainNetSrvOrderDirectionObject *) obj_direction)->direction,
            ((PyDapChainNetSrvUIDObject *) obj_srv_uid)->net_srv_uid,
            *((PyDapChainNodeAddrObject *) obj_node_addr)->node_addr,
            l_hf,
            &l_price,
            ((PyDapChainNetSrvPriceUnitUIDObject *) obj_price_unit)->price_unit_uid,
            price_ticker,
            (time_t) expires,
            l_ext,
            l_ext_size,
            "",
            0,
            ((PyCryptoKeyObject *) obj_key)->key
    );
    return 0;
}

PyObject *wrapping_dap_chain_net_srv_order_get_version(PyObject *self, void *closure){
    (void)closure;
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("H", 0);
    }else{
        return Py_BuildValue("H", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->version);
    }
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_uid(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvUIDObject *obj_srv_uid = PyObject_New(PyDapChainNetSrvUIDObject,
                                                          &DapChainNetSrvUidObjectType);
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        obj_srv_uid->net_srv_uid.uint64 = 0;
    }else{
        obj_srv_uid->net_srv_uid = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->srv_uid;
    }
    return (PyObject*)obj_srv_uid;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_direction(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvOrderDirectionObject *srv_direction =
            PyObject_New(PyDapChainNetSrvOrderDirectionObject,
                         &DapChainNetSrvOrderDirectionObjectType);
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        srv_direction->direction = 0;
    }else{
        srv_direction->direction = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->direction;
    }
    return (PyObject*)srv_direction;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_node_addr(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNodeAddrObject *l_obj_node_addr = PyObject_New(PyDapChainNodeAddrObject, &DapChainNodeAddrObjectType);
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        l_obj_node_addr->node_addr = NULL;
    }else{
        l_obj_node_addr->node_addr = &WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->node_addr;
    }
    return (PyObject*)l_obj_node_addr;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_tx_cond_hash(PyObject *self, void *closure){
    (void)closure;

    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        Py_RETURN_NONE;
    }else{
        PyDapHashFastObject *l_obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
        l_obj_hf->hash_fast = &WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->tx_cond_hash;
        l_obj_hf->origin = false;
        return (PyObject*)l_obj_hf;
    }
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price_unit(PyObject *self, void *closure){
    (void)closure;
    PyDapChainNetSrvPriceUnitUIDObject *l_obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                           &DapChainNetSrvPriceUnitUidObjectType);
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        l_obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_UNDEFINED;
    }else{
        l_obj_srv_price_uid->price_unit_uid = WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->price_unit;
    }
    return (PyObject*)l_obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ts_created(PyObject *self, void *closure){
    (void)closure;
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("k", 0);
    }else{
        return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ts_created);
    }
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_ts_expires(PyObject *self, void *closure){
    (void)closure;
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("k", 0);
    }else{
        return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ts_expires);
    }
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price(PyObject *self, void *closure){
    (void)closure;
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("k", 0);
    }else{
        return Py_BuildValue("k", WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->price);
    }
}
PyObject *wrapping_dap_chain_net_srv_order_get_srv_price_ticker(PyObject *self, void *closure){
    (void)closure;
    if(WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("s", "");
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
        dap_sign_t *l_sign = (dap_sign_t*)&WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ext_n_sign[WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order->ext_size];
        PyDapSignObject *obj_sign = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
        obj_sign->sign = l_sign;
        return (PyObject *) obj_sign;
    }
    Py_RETURN_NONE;
}

//Functions
PyObject *wrapping_dap_chain_net_srv_order_get_size(PyObject *self, PyObject *args){
    (void)args;
    if (WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order == NULL){
        return Py_BuildValue("n", 0);
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

PyObject *wrapping_dap_chain_net_srv_order_find(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainNetObject *obj_net;
    PyDapHashFastObject *obj_order_hash;
    if (!PyArg_ParseTuple(args, "OO", &obj_net, &obj_order_hash)){
        PyErr_SetString(PyExc_ValueError, "Function takes exactly two arguments");
        return NULL;
    }
    dap_chain_net_srv_order_t *l_order = NULL;
    if(!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_ValueError, "The first argument must be ChainNet object");
        return NULL;
    }
    if (PyUnicode_Check(obj_order_hash)){
        const char *l_str = PyUnicode_AsUTF8((PyObject *)obj_order_hash);
        l_order = dap_chain_net_srv_order_find_by_hash_str(obj_net->chain_net, l_str);
        if (l_order == NULL){
            Py_RETURN_NONE;
        }
        PyDapChainNetSrvOrderObject *l_obj_order = PyObject_New(PyDapChainNetSrvOrderObject,
                                                                &DapChainNetSrvOrderObjectType);
        l_obj_order->order = l_order;
        return (PyObject*)l_obj_order;
    }
    if (PyDapHashFast_Check(obj_order_hash)){
        l_order = dap_chain_net_srv_order_find_by_hash(obj_net->chain_net, obj_order_hash->hash_fast);
        if (l_order == NULL){
            Py_RETURN_NONE;
        }
        PyDapChainNetSrvOrderObject *l_obj_order = PyObject_New(PyDapChainNetSrvOrderObject,
                                                                &DapChainNetSrvOrderObjectType);
        l_obj_order->order = l_order;
        return (PyObject*)l_obj_order;
    }
    PyErr_SetString(PyExc_ValueError, "The second argument must be a string or HashFast object");
    return NULL;
}

PyObject *wrapping_dap_chain_net_srv_order_delete(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainNetObject *obj_net;
    PyDapHashFastObject *obj_order_hash;
    if (!PyArg_ParseTuple(args, "OO", &obj_net, &obj_order_hash)){
        PyErr_SetString(PyExc_ValueError, "Function takes exactly two arguments");
        return NULL;
    }
    if(!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_ValueError, "The first argument must be ChainNet object");
        return NULL;
    }
    int res = -1;
    if (PyUnicode_Check(obj_order_hash)){
        const char *l_str = PyUnicode_AsUTF8((PyObject *)obj_order_hash);
        res = dap_chain_net_srv_order_delete_by_hash_str(obj_net->chain_net, l_str);
        return Py_BuildValue("i", res);
    }
    if (PyDapHashFast_Check(obj_order_hash)) {
        res =dap_chain_net_srv_order_delete_by_hash(obj_net->chain_net,
                                               ((PyDapHashFastObject*)obj_order_hash)->hash_fast);
        return Py_BuildValue("i", res);
    }
    PyErr_SetString(PyExc_ValueError, "The second argument must be a string or HashFast object");
    return NULL;
}

PyObject *wrapping_dap_chain_net_srv_order_find_all_by(PyObject *self, PyObject *args){
    return NULL;
}

PyObject *wrapping_dap_chain_net_srv_order_save(PyObject *self, PyObject *args){
    PyDapChainNetObject *obj_net;
    if(!PyArg_ParseTuple(args, "O", &obj_net)){
        PyErr_SetString(PyExc_ValueError, "Function takes exactly one argument");
        return NULL;
    }
    if(!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_ValueError, "The first argument must be ChainNet object");
        return NULL;
    }
    char* res = NULL;
    dap_chain_net_t *l_net = obj_net->chain_net;
    res = dap_chain_net_srv_order_save(l_net,
                                           WRAPPING_DAP_CHAIN_NET_SRV_ORDER(self)->order);
    if (res == NULL)
        Py_RETURN_NONE;
    PyObject *l_obj_ret = Py_BuildValue("s", res);
    DAP_DELETE(res);
    return l_obj_ret;
}
PyObject *wrapping_dap_chain_net_srv_order_get_gdb_group(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainNetObject *obj_net;
    if(!PyArg_ParseTuple(args, "O", &obj_net)){
        PyErr_SetString(PyExc_ValueError, "Function takes exactly one argument");
        return NULL;
    }
    if(!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_ValueError, "The first argument must be ChainNet object");
        return NULL;
    }
    char *l_str_srv_order_gdb_group = dap_chain_net_srv_order_get_gdb_group(obj_net->chain_net);
    PyObject *l_str_ret_obj = Py_BuildValue("s", l_str_srv_order_gdb_group);
    DAP_DELETE(l_str_srv_order_gdb_group);
    return l_str_ret_obj;
}
PyObject *wrapping_dap_chain_net_srv_order_get_nodelist_group(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainNetObject *obj_net;
    if(!PyArg_ParseTuple(args, "O", &obj_net)){
        PyErr_SetString(PyExc_ValueError, "This function must take one argument");
        return NULL;
    }
    if(!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_ValueError, "The first argument must be ChainNet object");
        return NULL;
    }
    char *l_node_list_group = dap_chain_net_srv_order_get_nodelist_group(obj_net->chain_net);
    if (!l_node_list_group)
        Py_RETURN_NONE;
    PyObject *l_obj_res = Py_BuildValue("s", l_node_list_group);
    DAP_DELETE(l_node_list_group);
    return l_obj_res;
}

PyObject *wrapping_dap_chain_net_srv_order_add_notify_callback(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainNetObject *obj_net;
    PyObject *func_call, *call_arg;
    if (!PyArg_ParseTuple(args, "OOO", &obj_net, &func_call, &call_arg)){
        return NULL;
    }
    if (!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_AttributeError, "The first argument must be ChainNet object");
        return NULL;
    }
    if (!PyCallable_Check(func_call)){
        PyErr_SetString(PyExc_AttributeError, "The second argument must be a callable");
        return NULL;
    }
    _wrapping_order_callable_t *l_callback = DAP_NEW(_wrapping_order_callable_t);
    l_callback->func = func_call;
    l_callback->arg = call_arg;
    Py_INCREF(func_call);
    Py_INCREF(call_arg);
    dap_chain_net_srv_order_add_notify_callback(obj_net->chain_net,
                                                _wrapping_handler_add_order_notify, l_callback);
    Py_RETURN_NONE;
}
