/*
 * Authors:
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://cellframe.net
 * Copyright  (c) 2017-2025
 * All rights reserved.

 This file is part of DAP (Distributed Applications Platform) the open source project

    DAP (Distributed Applications Platform) is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DAP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with any DAP based project.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "wrapping_dap_chain_net_srv_dex_order.h"
#include "dap_chain_net_srv_dex.h"
#include "libdap_chain_net_python.h"
#include "libdap-python.h"
#include "wrapping_dap_hash.h"
#include "wrapping_dap_chain_common.h"
#include "wrapped_dap_chain_wallet_python.h"
#include "dap_chain_mempool.h"
#include "dap_time.h"
#include "dap_chain_datum.h"
#include "dap_chain.h"
#include "datetime.h"

#define LOG_TAG "wrapping_dap_chain_net_srv_dex_order"

#define ORDER(o) ((PyDapChainNetSrvDexOrderObject *)(o))

/* ============================================================================
 * Property Getters
 * ============================================================================ */

PyObject *wrapping_dap_chain_net_srv_dex_order_get_root_hash(PyObject *self, void *closure)
{
    (void)closure;
    PyDapHashFastObject *l_obj = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    if (!l_obj) return PyErr_NoMemory();
    l_obj->hash_fast = DAP_NEW(dap_hash_fast_t);
    if (!l_obj->hash_fast) {
        Py_DECREF(l_obj);
        return PyErr_NoMemory();
    }
    *l_obj->hash_fast = ORDER(self)->root_hash;
    l_obj->origin = true;
    return (PyObject *)l_obj;
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_tail_hash(PyObject *self, void *closure)
{
    (void)closure;
    PyDapHashFastObject *l_obj = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    if (!l_obj) return PyErr_NoMemory();
    l_obj->hash_fast = DAP_NEW(dap_hash_fast_t);
    if (!l_obj->hash_fast) {
        Py_DECREF(l_obj);
        return PyErr_NoMemory();
    }
    *l_obj->hash_fast = ORDER(self)->tail_hash;
    l_obj->origin = true;
    return (PyObject *)l_obj;
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_pair(PyObject *self, void *closure)
{
    (void)closure;
    return Py_BuildValue("s", ORDER(self)->pair);
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_side(PyObject *self, void *closure)
{
    (void)closure;
    return Py_BuildValue("s", ORDER(self)->side == 0 ? "ASK" : "BID");
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_token_sell(PyObject *self, void *closure)
{
    (void)closure;
    return Py_BuildValue("s", ORDER(self)->token_sell);
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_token_buy(PyObject *self, void *closure)
{
    (void)closure;
    return Py_BuildValue("s", ORDER(self)->token_buy);
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_value_sell(PyObject *self, void *closure)
{
    (void)closure;
    DapMathObject *l_obj = PyObject_New(DapMathObject, &DapMathObjectType);
    if (!l_obj) return PyErr_NoMemory();
    l_obj->value = ORDER(self)->value_sell;
    return (PyObject *)l_obj;
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_rate(PyObject *self, void *closure)
{
    (void)closure;
    DapMathObject *l_obj = PyObject_New(DapMathObject, &DapMathObjectType);
    if (!l_obj) return PyErr_NoMemory();
    l_obj->value = ORDER(self)->rate;
    return (PyObject *)l_obj;
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_seller_addr(PyObject *self, void *closure)
{
    (void)closure;
    PyDapChainAddrObject *l_obj = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
    if (!l_obj) return PyErr_NoMemory();
    l_obj->addr = DAP_DUP(&ORDER(self)->seller_addr);
    if (!l_obj->addr) {
        Py_DECREF(l_obj);
        return PyErr_NoMemory();
    }
    return (PyObject *)l_obj;
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_min_fill_pct(PyObject *self, void *closure)
{
    (void)closure;
    return PyLong_FromLong(ORDER(self)->min_fill & 0x7F);
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_min_fill_from_origin(PyObject *self, void *closure)
{
    (void)closure;
    return PyBool_FromLong((ORDER(self)->min_fill & 0x80) != 0);
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_filled_pct(PyObject *self, void *closure)
{
    (void)closure;
    return PyLong_FromLong(ORDER(self)->filled_pct);
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_ts_created(PyObject *self, void *closure)
{
    (void)closure;
    if (ORDER(self)->ts_created == 0)
        Py_RETURN_NONE;
    
    PyObject *l_ts_float = PyLong_FromUnsignedLongLong(ORDER(self)->ts_created);
    PyObject *l_ts_tuple = Py_BuildValue("(O)", l_ts_float);
    Py_DECREF(l_ts_float);
    
    PyDateTime_IMPORT;
    PyObject *l_dt = PyDateTime_FromTimestamp(l_ts_tuple);
    Py_DECREF(l_ts_tuple);
    
    return l_dt;
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_ts_expires(PyObject *self, void *closure)
{
    (void)closure;
    if (ORDER(self)->ts_expires == 0)
        Py_RETURN_NONE;
    
    PyObject *l_ts_float = PyLong_FromUnsignedLongLong(ORDER(self)->ts_expires);
    PyObject *l_ts_tuple = Py_BuildValue("(O)", l_ts_float);
    Py_DECREF(l_ts_float);
    
    PyDateTime_IMPORT;
    PyObject *l_dt = PyDateTime_FromTimestamp(l_ts_tuple);
    Py_DECREF(l_ts_tuple);
    
    return l_dt;
}

PyObject *wrapping_dap_chain_net_srv_dex_order_get_net(PyObject *self, void *closure)
{
    (void)closure;
    if (!ORDER(self)->net)
        Py_RETURN_NONE;
    
    PyDapChainNetObject *l_obj = PyObject_New(PyDapChainNetObject, &DapChainNetObjectType);
    if (!l_obj) return PyErr_NoMemory();
    l_obj->chain_net = ORDER(self)->net;
    return (PyObject *)l_obj;
}

/* ============================================================================
 * Methods
 * ============================================================================ */

/**
 * @brief Invalidate (cancel) the order
 * 
 * Python signature: invalidate(fee, wallet) -> str (tx_hash)
 */
PyObject *wrapping_dap_chain_net_srv_dex_order_invalidate(PyObject *self, PyObject *args)
{
    PyObject *obj_fee = NULL;
    PyObject *obj_wallet = NULL;
    
    if (!PyArg_ParseTuple(args, "OO", &obj_fee, &obj_wallet)) {
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_fee)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a Math object (fee)");
        return NULL;
    }
    
    if (!PyDapChainWalletObject_Check(obj_wallet)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a Wallet object");
        return NULL;
    }
    
    dap_chain_net_t *l_net = ORDER(self)->net;
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Order has no associated network");
        return NULL;
    }
    
    uint256_t l_fee = ((DapMathObject *)obj_fee)->value;
    dap_chain_wallet_t *l_wallet = ((PyDapChainWalletObject *)obj_wallet)->wallet;
    
    dap_chain_datum_tx_t *l_tx = NULL;
    dap_hash_fast_t l_order_hash = ORDER(self)->tail_hash;
    const dap_chain_addr_t *l_owner_addr = dap_chain_wallet_get_addr(l_wallet, l_net->pub.id);
    
    int l_ret = dap_chain_net_srv_dex_remove(l_net, &l_order_hash, l_fee, l_wallet, l_owner_addr, &l_tx);
    
    switch (l_ret) {
        case DEX_REMOVE_ERROR_OK: {
            // Put transaction to mempool
            size_t l_tx_size = dap_chain_datum_tx_get_size(l_tx);
            dap_chain_datum_t *l_datum = dap_chain_datum_create(DAP_CHAIN_DATUM_TX, l_tx, l_tx_size);
            DAP_DELETE(l_tx);
            
            if (!l_datum) {
                PyErr_SetString(PyExc_RuntimeError, "Failed to create datum");
                return NULL;
            }
            
            dap_chain_t *l_chain = dap_chain_net_get_default_chain_by_chain_type(l_net, CHAIN_TYPE_TX);
            char *l_hash_str = NULL;
            if (l_chain) {
                l_hash_str = dap_chain_mempool_datum_add(l_datum, l_chain, "hex");
            }
            DAP_DELETE(l_datum);
            
            if (l_hash_str) {
                PyObject *l_result = Py_BuildValue("s", l_hash_str);
                DAP_DELETE(l_hash_str);
                return l_result;
            }
            PyErr_SetString(PyExc_RuntimeError, "Failed to add transaction to mempool");
            return NULL;
        }
        case DEX_REMOVE_ERROR_INVALID_ARGUMENT:
            PyErr_SetString(PyExc_ValueError, "Invalid argument");
            return NULL;
        case DEX_REMOVE_ERROR_FEE_IS_ZERO:
            PyErr_SetString(PyExc_ValueError, "Fee must be non-zero");
            return NULL;
        case DEX_REMOVE_ERROR_TX_NOT_FOUND:
            PyErr_SetString(PyExc_ValueError, "Order transaction not found");
            return NULL;
        case DEX_REMOVE_ERROR_INVALID_OUT:
            PyErr_SetString(PyExc_ValueError, "Invalid order output");
            return NULL;
        case DEX_REMOVE_ERROR_NOT_OWNER:
            PyErr_SetString(PyExc_PermissionError, "Wallet is not the order owner");
            return NULL;
        case DEX_REMOVE_ERROR_COMPOSE_TX:
            PyErr_SetString(PyExc_RuntimeError, "Failed to compose transaction");
            return NULL;
        default:
            PyErr_Format(PyExc_RuntimeError, "Unknown error code: %d", l_ret);
            return NULL;
    }
}

/**
 * @brief Update order value
 * 
 * Python signature: update(new_value, fee, wallet) -> str (tx_hash)
 */
PyObject *wrapping_dap_chain_net_srv_dex_order_update(PyObject *self, PyObject *args)
{
    PyObject *obj_value = NULL;
    PyObject *obj_fee = NULL;
    PyObject *obj_wallet = NULL;
    
    if (!PyArg_ParseTuple(args, "OOO", &obj_value, &obj_fee, &obj_wallet)) {
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_value)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a Math object (new_value)");
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_fee)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a Math object (fee)");
        return NULL;
    }
    
    if (!PyDapChainWalletObject_Check(obj_wallet)) {
        PyErr_SetString(PyExc_TypeError, "Third argument must be a Wallet object");
        return NULL;
    }
    
    dap_chain_net_t *l_net = ORDER(self)->net;
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Order has no associated network");
        return NULL;
    }
    
    uint256_t l_value = ((DapMathObject *)obj_value)->value;
    uint256_t l_fee = ((DapMathObject *)obj_fee)->value;
    dap_chain_wallet_t *l_wallet = ((PyDapChainWalletObject *)obj_wallet)->wallet;
    
    dap_chain_datum_tx_t *l_tx = NULL;
    dap_hash_fast_t l_order_root = ORDER(self)->root_hash;
    const dap_chain_addr_t *l_owner_addr = dap_chain_wallet_get_addr(l_wallet, l_net->pub.id);
    
    int l_ret = dap_chain_net_srv_dex_update(l_net, &l_order_root, true, l_value, l_fee, l_wallet, l_owner_addr, &l_tx);
    
    switch (l_ret) {
        case DEX_UPDATE_ERROR_OK: {
            // Put transaction to mempool
            size_t l_tx_size = dap_chain_datum_tx_get_size(l_tx);
            dap_chain_datum_t *l_datum = dap_chain_datum_create(DAP_CHAIN_DATUM_TX, l_tx, l_tx_size);
            DAP_DELETE(l_tx);
            
            if (!l_datum) {
                PyErr_SetString(PyExc_RuntimeError, "Failed to create datum");
                return NULL;
            }
            
            dap_chain_t *l_chain = dap_chain_net_get_default_chain_by_chain_type(l_net, CHAIN_TYPE_TX);
            char *l_hash_str = NULL;
            if (l_chain) {
                l_hash_str = dap_chain_mempool_datum_add(l_datum, l_chain, "hex");
            }
            DAP_DELETE(l_datum);
            
            if (l_hash_str) {
                PyObject *l_result = Py_BuildValue("s", l_hash_str);
                DAP_DELETE(l_hash_str);
                return l_result;
            }
            PyErr_SetString(PyExc_RuntimeError, "Failed to add transaction to mempool");
            return NULL;
        }
        case DEX_UPDATE_ERROR_INVALID_ARGUMENT:
            PyErr_SetString(PyExc_ValueError, "Invalid argument");
            return NULL;
        case DEX_UPDATE_ERROR_NOT_FOUND:
            PyErr_SetString(PyExc_ValueError, "Order not found");
            return NULL;
        case DEX_UPDATE_ERROR_NOT_OWNER:
            PyErr_SetString(PyExc_PermissionError, "Wallet is not the order owner");
            return NULL;
        case DEX_UPDATE_ERROR_COMPOSE_TX:
            PyErr_SetString(PyExc_RuntimeError, "Failed to compose transaction");
            return NULL;
        default:
            PyErr_Format(PyExc_RuntimeError, "Unknown error code: %d", l_ret);
            return NULL;
    }
}

/* ============================================================================
 * Type Definition
 * ============================================================================ */

static void PyDapChainNetSrvDexOrder_dealloc(PyDapChainNetSrvDexOrderObject *self)
{
    // net is not owned by us, don't free
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyGetSetDef DapChainNetSrvDexOrderGetSetDef[] = {
    {"rootHash", wrapping_dap_chain_net_srv_dex_order_get_root_hash, NULL, "Order root hash", NULL},
    {"tailHash", wrapping_dap_chain_net_srv_dex_order_get_tail_hash, NULL, "Order current tail hash", NULL},
    {"pair", wrapping_dap_chain_net_srv_dex_order_get_pair, NULL, "Trading pair string BASE/QUOTE", NULL},
    {"side", wrapping_dap_chain_net_srv_dex_order_get_side, NULL, "Order side: ASK or BID", NULL},
    {"tokenSell", wrapping_dap_chain_net_srv_dex_order_get_token_sell, NULL, "Token being sold", NULL},
    {"tokenBuy", wrapping_dap_chain_net_srv_dex_order_get_token_buy, NULL, "Token being bought", NULL},
    {"valueSell", wrapping_dap_chain_net_srv_dex_order_get_value_sell, NULL, "Current sell value (Math)", NULL},
    {"rate", wrapping_dap_chain_net_srv_dex_order_get_rate, NULL, "Exchange rate QUOTE/BASE (Math)", NULL},
    {"sellerAddr", wrapping_dap_chain_net_srv_dex_order_get_seller_addr, NULL, "Seller address", NULL},
    {"minFillPct", wrapping_dap_chain_net_srv_dex_order_get_min_fill_pct, NULL, "Minimum fill percentage (0-100)", NULL},
    {"minFillFromOrigin", wrapping_dap_chain_net_srv_dex_order_get_min_fill_from_origin, NULL, "Min fill from original value", NULL},
    {"filledPct", wrapping_dap_chain_net_srv_dex_order_get_filled_pct, NULL, "Current fill percentage", NULL},
    {"tsCreated", wrapping_dap_chain_net_srv_dex_order_get_ts_created, NULL, "Creation datetime", NULL},
    {"tsExpires", wrapping_dap_chain_net_srv_dex_order_get_ts_expires, NULL, "Expiration datetime (or None)", NULL},
    {"net", wrapping_dap_chain_net_srv_dex_order_get_net, NULL, "Network object", NULL},
    {0}
};

PyMethodDef DapChainNetSrvDexOrderMethods[] = {
    {
        "invalidate",
        wrapping_dap_chain_net_srv_dex_order_invalidate,
        METH_VARARGS,
        "Cancel the order.\n\n"
        "Args:\n"
        "    fee: Validator fee (Math)\n"
        "    wallet: Owner wallet\n\n"
        "Returns:\n"
        "    Transaction hash string"
    },
    {
        "update",
        wrapping_dap_chain_net_srv_dex_order_update,
        METH_VARARGS,
        "Update order value.\n\n"
        "Args:\n"
        "    new_value: New sell value (Math)\n"
        "    fee: Validator fee (Math)\n"
        "    wallet: Owner wallet\n\n"
        "Returns:\n"
        "    Transaction hash string"
    },
    {0}
};

PyTypeObject PyDapChainNetSrvDexOrderObjectType = DAP_PY_TYPE_OBJECT(
    "CellFrame.Services.DexOrder",
    sizeof(PyDapChainNetSrvDexOrderObject),
    "DEX v2 order object",
    .tp_dealloc = (destructor)PyDapChainNetSrvDexOrder_dealloc,
    .tp_getset = DapChainNetSrvDexOrderGetSetDef,
    .tp_methods = DapChainNetSrvDexOrderMethods
);
