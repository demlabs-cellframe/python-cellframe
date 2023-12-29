#include "wrapping_dap_chain_net_srv_xchange_price.h"
#include "libdap-python.h"
#include "libdap_chain_net_python.h"
#include "libdap_crypto_key_python.h"

#define PRICE(a) ((PyDapChainNetSrvXchangePriceObject*)a)->price

PyGetSetDef DapChainNetSrvXchangeGetSetDef[] = {
        {"walletStr", (getter)wrapping_dap_chain_net_srv_xchange_price_get_wallet_str, NULL, NULL, NULL},
        {"tokenSell", (getter)wrapping_dap_chain_net_srv_xchange_price_get_token_sell, NULL, NULL, NULL},
        {"datoshiSell", (getter)wrapping_dap_chain_net_srv_xchange_price_get_datoshi_sell, NULL, NULL, NULL},
        {"net", (getter)wrapping_dap_chain_net_srv_xchange_price_get_net, NULL, NULL, NULL},
        {"tokenBuy", (getter)wrapping_dap_chain_net_srv_xchange_price_get_token_buy, NULL, NULL, NULL},
        {"datoshiBuy", (getter) wrapping_dap_chain_net_srv_xchange_price_get_datoshi_buy, NULL, NULL, NULL},
        {"rate", (getter)wrapping_dap_chain_net_srv_xchange_price_get_rate, NULL, NULL, NULL},
        {"fee", (getter)wrapping_dap_chain_net_srv_xchange_price_get_fee, NULL, NULL, NULL},
        {"txHash", (getter)wrapping_dap_chain_net_srv_xchange_price_get_tx_hash, NULL, NULL, NULL},
        {"orderHash", (getter)wrapping_dap_chain_net_srv_xchange_price_get_order_hash, NULL, NULL, NULL},
        {"walletKey", (getter)wrapping_dap_chain_net_srv_xchange_price_get_wallet_key, NULL, NULL, NULL},
        {}
};

PyObject *wrapping_dap_chain_net_srv_xchange_price_get_wallet_str(PyObject *self, void *closure){
    UNUSED(closure);
    return Py_BuildValue("s", PRICE(self)->wallet_str);
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_token_sell(PyObject *self, void *closure){
    UNUSED(closure);
    return Py_BuildValue("s", PRICE(self)->token_sell);
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_datoshi_sell(PyObject *self, void *closure){
    UNUSED(closure);
    DapMathObject *obj_math = PyObject_NEW(DapMathObject, &DapMathObjectType);
    obj_math->value = PRICE(self)->datoshi_sell;
    return (PyObject*)obj_math;
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_net(PyObject *self, void *closure){
    UNUSED(closure);
    PyDapChainNetObject *obj_net = PyObject_NEW(PyDapChainNetObject, &DapChainNetObjectType);
    obj_net->chain_net = PRICE(self)->net;
    return (PyObject*)obj_net;
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_token_buy(PyObject *self, void *closure){
    UNUSED(closure);
    return Py_BuildValue("s", PRICE(self)->token_buy);
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_datoshi_buy(PyObject *self, void *closure){
    UNUSED(closure);
    DapMathObject *obj_math = PyObject_New(DapMathObject, &DapMathObjectType);
    obj_math->value = PRICE(self)->datoshi_buy;
    return (PyObject*)obj_math;
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_rate(PyObject *self, void *closure){
    UNUSED(closure);
    DapMathObject *obj_math = PyObject_New(DapMathObject, &DapMathObjectType);
    obj_math->value = PRICE(self)->rate;
    return (PyObject*)obj_math;
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_fee(PyObject *self, void *closure){
    UNUSED(closure);
    DapMathObject *obj_math = PyObject_New(DapMathObject, &DapMathObjectType);
    obj_math->value = PRICE(self)->fee;
    return (PyObject*)obj_math;
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_tx_hash(PyObject *self, void *closure){
    UNUSED(closure);
    PyDapHashFastObject *obj_hash = PyObject_NEW(PyDapHashFastObject, &DapHashFastObjectType);
    obj_hash->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    dap_chain_hash_fast_t l_hf = PRICE(self)->tx_hash;
    memcpy(obj_hash->hash_fast, &l_hf, sizeof(dap_chain_hash_fast_t));
    obj_hash->origin = true;
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_order_hash(PyObject *self, void *closure){
    UNUSED(closure);
    PyDapHashFastObject *obj_hash = PyObject_NEW(PyDapHashFastObject, &DapHashFastObjectType);
    obj_hash->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    dap_chain_hash_fast_t l_hf = PRICE(self)->order_hash;
    memcpy(obj_hash->hash_fast, &l_hf, sizeof(dap_chain_hash_fast_t));
    obj_hash->origin = true;
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_wallet_key(PyObject *self, void *closure){
    UNUSED(closure);
    PyCryptoKeyObject *obj_key = PyObject_New(PyCryptoKeyObject, &PyCryptoKeyObjectType);
    obj_key->key = PRICE(self)->wallet_key;
    return (PyObject*)obj_key;
}

PyTypeObject PyDapChainNetSrvXchangePriceObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Service.Xchange.Price", sizeof(PyDapChainNetSrvXchangePriceObject),
        "Price from service xchange",
        .tp_getset = DapChainNetSrvXchangeGetSetDef);

PyObject *wrapping_dap_chain_net_srv_xchange_price_create_object(dap_chain_net_srv_xchange_price_t *a_price) {
    PyDapChainNetSrvXchangePriceObject *self = PyObject_New(PyDapChainNetSrvXchangePriceObject, &PyDapChainNetSrvXchangePriceObjectType);
    self->price = a_price;
    return (PyObject*)self;
}
