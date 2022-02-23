#include "wrapping_dap_chain_common.h"

PyObject *DapChainIdObject_str(PyObject *self){
    return Py_BuildValue("s", dap_strdup_printf("0x%016"DAP_UINT64_FORMAT_x, ((PyDapChainIDObject*)self)->chain_id->uint64));
}

PyObject *PyDapChainCellIdObject_str(PyObject *self){
    return Py_BuildValue("s", dap_strdup_printf("0x%016"DAP_UINT64_FORMAT_x, ((PyDapChainCellIDObject*)self)->cell_id.uint64));
}

PyObject *dap_chain_hash_slow_to_str_py(PyObject *self, PyObject *args){
    PyObject *obj_hash_slow;
    char *str;
    size_t str_max;
    if (!PyArg_ParseTuple(args, "O|s|n", &obj_hash_slow, &str, &str_max))
            return NULL;
    size_t res = dap_chain_hash_slow_to_str(((PyDapChainHashSlowObject*)obj_hash_slow)->hash_slow, str, str_max);
    return Py_BuildValue("ns", res, str_max);
}

PyObject *dap_chain_addr_to_str_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_addr;
    if (!PyArg_ParseTuple(args, "O", &obj_chain_addr))
        return NULL;
    const dap_chain_addr_t *addr = ((PyDapChainAddrObject*)obj_chain_addr)->addr;
    char *res = dap_chain_addr_to_str(addr);
    return Py_BuildValue("s", res);
}

PyObject *dap_chain_addr_from_str_py(PyObject *self, PyObject *args){
    const char *str;
    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
    PyObject *obj = _PyObject_New(&DapChainAddrObject_DapChainAddrObjectType);
    obj = PyObject_Init(obj, &DapChainAddrObject_DapChainAddrObjectType);
    ((PyDapChainAddrObject*)obj)->addr = dap_chain_addr_from_str(str);
    PyObject_Dir(obj);
    return Py_BuildValue("O", obj);
}

PyObject *dap_chain_addr_fill_py(PyObject *self, PyObject *args){
    PyObject *obj_sign_type;
    PyObject *obj_pkey_hash;
    PyObject *obj_chain_net_id;
    if (!PyArg_ParseTuple(args, "OOO", &obj_sign_type, &obj_pkey_hash, &obj_chain_net_id)){
        PyErr_SetString(PyExc_AttributeError, "This function takes three arguments, signature type, public key hash, chain network ID.");
        return NULL;
    }
    if (self == NULL){
        PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObject_DapChainAddrObjectType);
        PyObject_Dir((PyObject*)obj_addr);
        obj_addr->addr = DAP_NEW(dap_chain_addr_t);
        dap_chain_addr_fill(
                obj_addr->addr,
                *((PyDapSignTypeObject*)obj_sign_type)->sign_type,
                ((PyDapHashFastObject*)obj_pkey_hash)->hash_fast,
                ((PyDapChainNetIdObject*)obj_chain_net_id)->net_id);
        return (PyObject*)obj_addr;
    }else{
        dap_chain_addr_fill(
                ((PyDapChainAddrObject*)self)->addr,
                *((PyDapSignTypeObject*)obj_sign_type)->sign_type,
                ((PyDapHashFastObject*)obj_pkey_hash)->hash_fast,
                ((PyDapChainNetIdObject*)obj_chain_net_id)->net_id);
        return Py_None;
    }
}

PyObject *dap_chain_addr_fill_from_key_py(PyObject *self, PyObject *args){
    PyObject *key;
    PyObject *net_id;
    if (!PyArg_ParseTuple(args, "O|O", &key, &net_id))
        return NULL;
    dap_chain_addr_fill_from_key(((PyDapChainAddrObject*)self)->addr, ((PyCryptoKeyObject*)key)->key, (((PyDapChainNetIdObject*)net_id)->net_id));
    return self;
}

PyObject *dap_chain_addr_check_sum_py(PyObject *self, PyObject *args){
    int res = dap_chain_addr_check_sum(((PyDapChainAddrObject*)self)->addr);
    return PyLong_FromLong(res);
}

PyObject *obj_addr_str(PyObject *self){
    return Py_BuildValue("s", dap_chain_addr_to_str(((PyDapChainAddrObject*)self)->addr));
}

PyObject* dap_chain_addr_get_net_id_py(PyObject *self, PyObject *args){
    (void)args;
    PyObject *obj_net_id = _PyObject_New(&DapChainNetIdObject_DapChainNetIdObjectType);
    ((PyDapChainNetIdObject*)obj_net_id)->net_id = ((PyDapChainAddrObject*)self)->addr->net_id;
    return Py_BuildValue("O", obj_net_id);
}

PyObject *dap_chain_net_id_from_str_py(PyObject *self, PyObject *args){
    const char *str;
    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
    PyObject *obj_net_id = _PyObject_New(&DapChainNetIdObject_DapChainNetIdObjectType);
    ((PyDapChainNetIdObject*)obj_net_id)->net_id = dap_chain_net_id_from_str(str);
    return Py_BuildValue("O", obj_net_id);
}

PyObject *dap_chain_net_srv_uid_from_str_py(PyObject *self, PyObject *args){
    const char *str;
    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
    PyObject *obj = _PyObject_New(&DapChainNetSrvUIDObject_DapChainNetSrvUIDObjectType);
    ((PyDapChainNetSrvUIDObject*)obj)->net_srv_uid = dap_chain_net_srv_uid_from_str(str);
    return Py_BuildValue("O", obj);
}

PyObject * dap_chain_balance_to_coins_py(PyObject *self, PyObject *args){
    (void)self;
    uint64_t balance=0;
    if (!PyArg_ParseTuple(args, "k", &balance)){
        return NULL;
    }
    const char *str = dap_chain_balance_to_coins(dap_chain_uint128_from(balance));
    return Py_BuildValue("s", str);
}

PyObject *PyDapChainNetSrvPriceUnitUID_str(PyObject *self){
    return Py_BuildValue("s", serv_unit_enum_to_str(&((PyDapChainNetSrvPriceUnitUIDObject*)self)->price_unit_uid.enm));
}

PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_undefined(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType);
    PyObject_Dir((PyObject*)obj_srv_price_uid);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_UNDEFINED;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_UNDEFINED;
    return (PyObject*)obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_mb(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType);
    PyObject_Dir((PyObject*)obj_srv_price_uid);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_MB;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_MB;
    return (PyObject*)obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_sec(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType);
    PyObject_Dir((PyObject*)obj_srv_price_uid);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_SEC;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_SEC;
    return (PyObject*)obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_day(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType);
    PyObject_Dir((PyObject*)obj_srv_price_uid);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_DAY;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_DAY;
    return (PyObject*)obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_kb(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType);
    PyObject_Dir((PyObject*)obj_srv_price_uid);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_KB;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_KB;
    return (PyObject*)obj_srv_price_uid;
}
PyObject *wrapping_dap_chain_net_srv_price_unit_uid_get_b(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapChainNetSrvPriceUnitUIDObject *obj_srv_price_uid = PyObject_New(PyDapChainNetSrvPriceUnitUIDObject,
                                                                         &DapChainNetSrvPriceUnitUIDObject_DapChainNetSrvPriceUnitUIDObjectType);
    PyObject_Dir((PyObject*)obj_srv_price_uid);
    obj_srv_price_uid->price_unit_uid.uint32 = SERV_UNIT_B;
    obj_srv_price_uid->price_unit_uid.enm = SERV_UNIT_B;
    return (PyObject*)obj_srv_price_uid;
}

/* Chain node addr */
PyObject *PyDapChainNodeAddrObject_str(PyObject* self){
    char *ret = dap_strdup_printf(NODE_ADDR_FP_STR, NODE_ADDR_FP_ARGS(((PyDapChainNodeAddrObject *) self)->node_addr));
    return Py_BuildValue("s", ret);
}

PyObject *PyDapChainNetSrvUID_str(PyObject *self){
    char *res = dap_strdup_printf("0x%016"DAP_UINT64_FORMAT_X, ((PyDapChainNetSrvUIDObject*)self)->net_srv_uid.uint64);
    return Py_BuildValue("s", res);
//    dap_chain_net_srv_uid_from_str_py()
}
