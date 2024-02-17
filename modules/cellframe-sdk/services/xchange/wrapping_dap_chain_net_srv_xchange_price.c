#include "wrapping_dap_chain_net_srv_xchange_price.h"
#include "libdap-python.h"
#include "libdap_chain_net_python.h"
#include "libdap_crypto_key_python.h"
#include "dap_chain_wallet_python.h"

#define PRICE(a) ((PyDapChainNetSrvXchangeOrderObject*)a)->price

PyGetSetDef DapChainNetSrvXchangePriceGetSetDef[] = {
        {"tokenSell", (getter)wrapping_dap_chain_net_srv_xchange_price_get_token_sell, NULL, NULL, NULL},
        {"datoshiSell", (getter)wrapping_dap_chain_net_srv_xchange_price_get_datoshi_sell, NULL, NULL, NULL},
        {"net", (getter)wrapping_dap_chain_net_srv_xchange_price_get_net, NULL, NULL, NULL},
        {"tokenBuy", (getter)wrapping_dap_chain_net_srv_xchange_price_get_token_buy, NULL, NULL, NULL},
        {"datoshiBuy", (getter) wrapping_dap_chain_net_srv_xchange_price_get_datoshi_buy, NULL, NULL, NULL},
        {"rate", (getter)wrapping_dap_chain_net_srv_xchange_price_get_rate, NULL, NULL, NULL},
        {"fee", (getter)wrapping_dap_chain_net_srv_xchange_price_get_fee, NULL, NULL, NULL},
        {"txHash", (getter)wrapping_dap_chain_net_srv_xchange_price_get_tx_hash, NULL, NULL, NULL},
        {"orderHash", (getter)wrapping_dap_chain_net_srv_xchange_price_get_order_hash, NULL, NULL, NULL},
        {"completionRate", (getter)wrapping_dap_chain_net_srv_xchange_price_get_completion_rate, NULL, NULL, NULL},
        {"status", (getter)wrapping_dap_chain_net_srv_xchange_price_get_status, NULL, NULL, NULL},
        {}
};

PyMethodDef DapChainNetSrvXchangePriceMethods[] = {
        {
            "purchase",
            wrapping_dap_chain_net_srv_xchange_price_purchase,
            METH_VARARGS,
            "Function for partial or full purchase of an order."
            },
        {NULL}
};

void DapChainNetSrvXchangePrice_free(PyDapChainDatumDecreeObject *self){
    DAP_DELETE(self->decree);
    Py_TYPE(self)->tp_free((PyObject*)self);
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
    DapMathObject *order_fee = PyObject_New(DapMathObject, &DapMathObjectType);
    DapMathObject *network_fee = PyObject_New(DapMathObject, &DapMathObjectType);
    
    uint256_t current_net_fee_val;
    dap_chain_addr_t comission_addr;
    uint16_t comission_type; 
    dap_chain_net_srv_xchange_get_fee(PRICE(self)->net->pub.id, &current_net_fee_val, &comission_addr,  &comission_type);
    
    order_fee->value = PRICE(self)->fee;
    network_fee->value = current_net_fee_val;

    PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
    obj_addr->addr = DAP_NEW(dap_chain_addr_t);
    mempcpy(obj_addr->addr, &comission_addr, sizeof(dap_chain_addr_t));

    PyObject *res = PyDict_New();
    PyDict_SetItemString(res, "order_fee", order_fee);
    PyDict_SetItemString(res, "network_fee", order_fee);
    PyDict_SetItemString(res, "address", obj_addr);
    PyDict_SetItemString(res, "type", Py_BuildValue("s", dap_chain_net_srv_fee_type_to_str(comission_type)));

    const char *l_native_ticker = PRICE(self)->net->pub.native_ticker;
    const char *l_service_ticker = (comission_type == SERVICE_FEE_OWN_FIXED || comission_type == SERVICE_FEE_OWN_PERCENT) ?
                PRICE(self)->token_buy : l_native_ticker;
    
    PyDict_SetItemString(res, "token", Py_BuildValue("s", l_service_ticker));
    return (PyObject*)res;
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_tx_hash(PyObject *self, void *closure){
    UNUSED(closure);
    PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hf->hash_fast = DAP_NEW(dap_hash_fast_t);
    dap_chain_hash_fast_t l_hf = PRICE(self)->tx_hash;
    memcpy(obj_hf->hash_fast, &l_hf, sizeof(dap_chain_hash_fast_t));
    obj_hf->origin = true;
    return (PyObject*)obj_hf;
}

PyObject *wrapping_dap_chain_net_srv_xchange_price_get_order_hash(PyObject *self, void *closure){
    UNUSED(closure);
    PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hf->hash_fast = DAP_NEW(dap_hash_fast_t);
    dap_chain_hash_fast_t l_hf = PRICE(self)->order_hash;
    memcpy(obj_hf->hash_fast, &l_hf, sizeof(dap_chain_hash_fast_t));
    obj_hf->origin = true;
    return (PyObject*)obj_hf;
}

PyObject *wrapping_dap_chain_net_srv_xchange_price_get_completion_rate(PyObject *self, void *closure){
    return Py_BuildValue("l",dap_chain_net_srv_xchange_get_order_completion_rate(PRICE(self)->net, PRICE(self)->order_hash));
}

PyObject *wrapping_dap_chain_net_srv_xchange_price_get_status(PyObject *self, void *closure){
    switch (dap_chain_net_srv_xchange_get_order_status(PRICE(self)->net, PRICE(self)->order_hash))
    {
        case XCHANGE_ORDER_STATUS_OPENED:
            return Py_BuildValue("s", "OPENED");
            
        case XCHANGE_ORDER_STATUS_CLOSED:
            return Py_BuildValue("s", "CLOSED");
    }
    return Py_BuildValue("s", "UNKNOWN");
}

PyTypeObject PyDapChainNetSrvXchangeOrderObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Service.XchangeOrder", sizeof(PyDapChainNetSrvXchangeOrderObject),
        "Price from service xchange",
        .tp_dealloc = (destructor)DapChainNetSrvXchangePrice_free,
        .tp_getset = DapChainNetSrvXchangePriceGetSetDef,
        .tp_methods = DapChainNetSrvXchangePriceMethods);


PyObject *wrapping_dap_chain_net_srv_xchange_price_purchase(PyObject *self, PyObject *argv){
    PyObject *obj_wallet, *obj_fee, *obj_value;
    if (!PyArg_ParseTuple(argv, "OOO", &obj_value, &obj_fee, &obj_wallet)) {
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_value)) {
        PyErr_SetString(PyExc_AttributeError, "The first argument was passed incorrectly. This must be "
                                              "an instance of an object of type Math.");
        return NULL;
    }
    if (!DapMathObject_Check(obj_fee)) {
        PyErr_SetString(PyExc_AttributeError, "The second argument was passed incorrectly. This must be "
                                              "an instance of an object of type Math.");
        return NULL;
    }
    if (!PyDapChainWalletObject_Check(obj_wallet)) {
        PyErr_SetString(PyExc_AttributeError, "The third parameter to the function passed an incorrect "
                                              "argument. This must be an instance of the Wallet class.");
        return NULL;
    }
    char *l_ret_tx_hash = NULL;
    dap_chain_net_srv_xchange_price_t *l_price = PRICE(self);
    int l_ret_code = dap_chain_net_srv_xchange_purchase(l_price->net, &l_price->order_hash,
                                                        ((DapMathObject*)obj_value)->value,
                                                        ((DapMathObject*)obj_fee)->value,
                                                        ((PyDapChainWalletObject*)obj_wallet)->wallet,
                                                        &l_ret_tx_hash);
    switch (l_ret_code) {
        case XCHANGE_PURCHASE_ERROR_OK: {
            return Py_BuildValue("s", l_ret_tx_hash);
        }
        default: {
            Py_RETURN_NONE;
        }
    }
}
