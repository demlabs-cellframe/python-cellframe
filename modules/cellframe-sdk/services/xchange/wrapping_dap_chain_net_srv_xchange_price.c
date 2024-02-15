#include "wrapping_dap_chain_net_srv_xchange_price.h"
#include "libdap-python.h"
#include "libdap_chain_net_python.h"
#include "libdap_crypto_key_python.h"
#include "dap_chain_wallet_python.h"

#define PRICE(a) ((PyDapChainNetSrvXchangeOrderObject*)a)->price

PyGetSetDef DapChainNetSrvXchangePriceGetSetDef[] = {
        {"walletStr", (getter)wrapping_dap_chain_net_srv_xchange_price_get_wallet_str, NULL, NULL, NULL},
        {"tokenSell", (getter)wrapping_dap_chain_net_srv_xchange_price_get_token_sell, NULL, NULL, NULL},
        {"datoshiSell", (getter)wrapping_dap_chain_net_srv_xchange_price_get_datoshi_sell, NULL, NULL, NULL},
        {"net", (getter)wrapping_dap_chain_net_srv_xchange_price_get_net, NULL, NULL, NULL},
        {"tokenBuy", (getter)wrapping_dap_chain_net_srv_xchange_price_get_token_buy, NULL, NULL, NULL},
        {"datoshiBuy", (getter) wrapping_dap_chain_net_srv_xchange_price_get_datoshi_buy, NULL, NULL, NULL},
        {"rate", (getter)wrapping_dap_chain_net_srv_xchange_price_get_rate, NULL, NULL, NULL},
        {"fee", (getter)wrapping_dap_chain_net_srv_xchange_price_get_fee, NULL, NULL, NULL},
        {"txHash", (getter)wrapping_dap_chain_net_srv_xchange_price_get_tx_hash, NULL, NULL, NULL},
        {"walletKey", (getter)wrapping_dap_chain_net_srv_xchange_price_get_wallet_key, NULL, NULL, NULL},
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
    PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hf->hash_fast = DAP_NEW(dap_hash_fast_t);
    dap_chain_hash_fast_t l_hf = PRICE(self)->tx_hash;
    memcpy(obj_hf->hash_fast, &l_hf, sizeof(dap_chain_hash_fast_t));
    obj_hf->origin = true;
    return (PyObject*)obj_hf;
}
PyObject *wrapping_dap_chain_net_srv_xchange_price_get_wallet_key(PyObject *self, void *closure){
    UNUSED(closure);
    PyCryptoKeyObject *obj_key = PyObject_New(PyCryptoKeyObject, &PyCryptoKeyObjectType);
    obj_key->key = PRICE(self)->wallet_key;
    return (PyObject*)obj_key;
}

PyTypeObject PyDapChainNetSrvXchangeOrderObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Service.XchangeOrder", sizeof(PyDapChainNetSrvXchangeOrderObject),
        "Price from service xchange",
        .tp_dealloc = (destructor)DapChainNetSrvXchangePrice_free,
        .tp_getset = DapChainNetSrvXchangePriceGetSetDef,
        .tp_methods = DapChainNetSrvXchangePriceMethods);

PyObject *wrapping_dap_chain_net_srv_xchange_price_create_object(dap_chain_net_srv_xchange_price_t *a_price) {
    PyDapChainNetSrvXchangeOrderObject *self = PyObject_New(PyDapChainNetSrvXchangeOrderObject, &PyDapChainNetSrvXchangeOrderObjectType);
    self->price = DAP_NEW(dap_chain_net_srv_xchange_price_t);
    memcpy(self->price, a_price, sizeof (dap_chain_net_srv_xchange_price_t ));
    return (PyObject*)self;
}

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
        case XCHANGE_PURCHASE_ERROR_INVALID_ARGUMENT: {
            PyErr_SetString(CellFrame_Xchange_Price_error, "An internal error occurred in the order "
                                                           "purchase function; not all arguments were passed.");
            return NULL;
        }
        case XCHANGE_PURCHASE_ERROR_SPECIFIED_ORDER_NOT_FOUND: {
            PyErr_SetString(CellFrame_Xchange_Price_error, "An error occurred: the order was not found "
                                                           "for the hash specified in the price list.");
            return NULL;
        }
        case XCHANGE_PURCHASE_ERROR_CAN_NOT_CREATE_PRICE: {
            PyErr_SetString(CellFrame_Xchange_Price_error, "An error occurred. It was not possible to "
                                                           "create a new price list to create an exchange transaction.");
            return NULL;
        }
        case XCHANGE_PURCHASE_ERROR_CAN_NOT_CREATE_EXCHANGE_TX: {
            PyErr_SetString(CellFrame_Xchange_Price_error, "An error occurred: it was not possible to "
                                                           "create an exchange transaction.");
            return NULL;
        }
        default: {
            char *l_ret = dap_strdup_printf("An error occurred with an unknown code: %d.", l_ret_code);
            PyErr_SetString(CellFrame_Xchange_Price_error, l_ret);
            DAP_DELETE(l_ret);
            return NULL;
        }
    }
}
