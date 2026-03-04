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

#include "wrapping_dap_chain_net_srv_dex.h"
#include "wrapping_dap_chain_net_srv_dex_order.h"
#include "dap_chain_net_srv_dex.h"
#include "libdap_chain_net_python.h"
#include "libdap-python.h"
#include "dap_chain_ledger.h"
#include "dap_chain_datum_tx_out_cond.h"
#include "dap_common.h"
#include "uthash.h"
#include "datetime.h"

#define LOG_TAG "wrapping_dap_chain_net_srv_dex"

/**
 * @brief Safely copy a token ticker, bounded to DAP_CHAIN_TICKER_SIZE_MAX.
 *
 * Uses strnlen+memcpy instead of strncpy/snprintf so that GCC can prove
 * the copy length is within [0, DAP_CHAIN_TICKER_SIZE_MAX-1] and does not
 * emit -Wstringop-truncation or -Wformat-truncation regardless of the
 * static source-string size seen in the caller's translation unit.
 */
static inline void s_ticker_copy(char *a_dst, const char *a_src)
{
    size_t l_n = strnlen(a_src, DAP_CHAIN_TICKER_SIZE_MAX - 1);
    memcpy(a_dst, a_src, l_n);
    a_dst[l_n] = '\0';
}

/* Forward declarations for internal DEX functions we need */
// These are declared in dap_chain_net_srv_dex.c as static, so we need to access cache differently
// For now we'll use ledger iteration which is always available

/**
 * @brief Normalize pair to canonical BASE/QUOTE order
 */
static void s_pair_normalize(const char *a_sell_token, const char *a_buy_token,
                             char *a_base_out, char *a_quote_out, uint8_t *a_side_out)
{
    if (strcmp(a_sell_token, a_buy_token) < 0) {
        // sell < buy lexicographically: BASE=sell, QUOTE=buy → ASK
        s_ticker_copy(a_base_out,  a_sell_token);
        s_ticker_copy(a_quote_out, a_buy_token);
        if (a_side_out) *a_side_out = 0; // ASK
    } else {
        // sell >= buy: BASE=buy, QUOTE=sell → BID
        s_ticker_copy(a_base_out,  a_buy_token);
        s_ticker_copy(a_quote_out, a_sell_token);
        if (a_side_out) *a_side_out = 1; // BID
    }
}

/**
 * @brief Get list of DEX orders
 * 
 * Python signature: getOrders(net, pair=None, seller=None, limit=0, offset=0)
 */
PyObject *wrapping_dap_chain_net_srv_dex_get_orders(PyObject *self, PyObject *args, PyObject *kwargs)
{
    (void)self;
    
    PyObject *obj_net = NULL;
    const char *pair_str = NULL;
    const char *seller_str = NULL;
    int limit = 0;
    int offset = 0;
    
    static char *kwlist[] = {"net", "pair", "seller", "limit", "offset", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|zzii", kwlist,
                                      &obj_net, &pair_str, &seller_str, &limit, &offset)) {
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_net, &DapChainNetObjectType)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ChainNet object");
        return NULL;
    }
    
    dap_chain_net_t *l_net = ((PyDapChainNetObject *)obj_net)->chain_net;
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network object");
        return NULL;
    }
    
    // Parse pair if provided
    char l_ticker_base[DAP_CHAIN_TICKER_SIZE_MAX] = {0};
    char l_ticker_quote[DAP_CHAIN_TICKER_SIZE_MAX] = {0};
    bool l_has_pair = false;
    
    if (pair_str && pair_str[0]) {
        char l_pair_storage[DAP_CHAIN_TICKER_SIZE_MAX * 2 + 2];
        strncpy(l_pair_storage, pair_str, sizeof(l_pair_storage) - 1);
        
        char *l_slash = strchr(l_pair_storage, '/');
        if (!l_slash) {
            PyErr_SetString(PyExc_ValueError, "Invalid pair format, expected BASE/QUOTE");
            return NULL;
        }
        *l_slash++ = '\0';
        
        // Normalize to canonical order
        if (strcmp(l_pair_storage, l_slash) < 0) {
            s_ticker_copy(l_ticker_base,  l_pair_storage);
            s_ticker_copy(l_ticker_quote, l_slash);
        } else {
            s_ticker_copy(l_ticker_base,  l_slash);
            s_ticker_copy(l_ticker_quote, l_pair_storage);
        }
        l_has_pair = true;
    }
    
    // Parse seller address if provided
    dap_chain_addr_t l_seller_addr = {0};
    bool l_has_seller = false;
    
    if (seller_str && seller_str[0]) {
        dap_chain_addr_t *l_addr_tmp = dap_chain_addr_from_str(seller_str);
        if (!l_addr_tmp) {
            PyErr_SetString(PyExc_ValueError, "Invalid seller address");
            return NULL;
        }
        l_seller_addr = *l_addr_tmp;
        DAP_DELETE(l_addr_tmp);
        l_has_seller = true;
    }
    
    // Create result list
    PyObject *l_result = PyList_New(0);
    if (!l_result) {
        return PyErr_NoMemory();
    }
    
    // Iterate through ledger to find DEX orders
    dap_ledger_datum_iter_t *it = dap_ledger_datum_iter_create(l_net);
    if (!it) {
        Py_DECREF(l_result);
        PyErr_SetString(PyExc_RuntimeError, "Failed to create ledger iterator");
        return NULL;
    }
    
    dap_time_t l_now_ts = dap_ledger_get_blockchain_time(l_net->pub.ledger);
    int l_count = 0;
    int l_skipped = 0;
    
    for (dap_chain_datum_tx_t *l_tx = dap_ledger_datum_iter_get_first(it); 
         l_tx; 
         l_tx = dap_ledger_datum_iter_get_next(it)) {
        
        int l_out_idx = 0;
        dap_chain_tx_out_cond_t *l_out_cond = dap_chain_datum_tx_out_cond_get(
            l_tx, DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_DEX, &l_out_idx);
        
        if (!l_out_cond)
            continue;
        
        // Check if output is spent
        if (dap_ledger_tx_hash_is_used_out_item(l_net->pub.ledger, &it->cur_hash, l_out_idx, NULL))
            continue;
        
        // Check expiration
        if (l_out_cond->header.ts_expires && l_now_ts > l_out_cond->header.ts_expires)
            continue;
        
        // Check seller filter
        if (l_has_seller && !dap_chain_addr_compare(&l_out_cond->subtype.srv_dex.seller_addr, &l_seller_addr))
            continue;
        
        // Get sell token
        const char *l_sell_tok = dap_ledger_tx_get_token_ticker_by_hash(l_net->pub.ledger, &it->cur_hash);
        if (!l_sell_tok)
            continue;
        
        // Normalize pair
        char l_base[DAP_CHAIN_TICKER_SIZE_MAX] = {0};
        char l_quote[DAP_CHAIN_TICKER_SIZE_MAX] = {0};
        uint8_t l_side = 0;
        s_pair_normalize(l_sell_tok, l_out_cond->subtype.srv_dex.buy_token, l_base, l_quote, &l_side);
        
        // Check pair filter
        if (l_has_pair && (strcmp(l_base, l_ticker_base) != 0 || strcmp(l_quote, l_ticker_quote) != 0))
            continue;
        
        // Apply offset
        if (l_skipped < offset) {
            l_skipped++;
            continue;
        }
        
        // Create order object
        PyDapChainNetSrvDexOrderObject *l_order = PyObject_New(
            PyDapChainNetSrvDexOrderObject, &PyDapChainNetSrvDexOrderObjectType);
        
        if (!l_order) {
            dap_ledger_datum_iter_delete(it);
            Py_DECREF(l_result);
            return PyErr_NoMemory();
        }
        
        // Fill order data
        l_order->net = l_net;
        
        // Root hash: if order_root_hash is blank, this is the root
        if (dap_hash_fast_is_blank(&l_out_cond->subtype.srv_dex.order_root_hash)) {
            l_order->root_hash = it->cur_hash;
        } else {
            l_order->root_hash = l_out_cond->subtype.srv_dex.order_root_hash;
        }
        l_order->tail_hash = it->cur_hash;
        
        // Pair string
        snprintf(l_order->pair, sizeof(l_order->pair), "%s/%s", l_base, l_quote);
        
        // Tokens
        s_ticker_copy(l_order->token_sell, l_sell_tok);
        s_ticker_copy(l_order->token_buy,  l_out_cond->subtype.srv_dex.buy_token);
        
        // Values
        l_order->value_sell = l_out_cond->header.value;
        l_order->rate = l_out_cond->subtype.srv_dex.rate;
        l_order->seller_addr = l_out_cond->subtype.srv_dex.seller_addr;
        l_order->side = l_side;
        l_order->min_fill = l_out_cond->subtype.srv_dex.min_fill;
        l_order->filled_pct = 0; // TODO: calculate from order chain
        l_order->ts_created = l_tx->header.ts_created;
        l_order->ts_expires = l_out_cond->header.ts_expires;
        
        // Add to list
        if (PyList_Append(l_result, (PyObject *)l_order) < 0) {
            Py_DECREF(l_order);
            dap_ledger_datum_iter_delete(it);
            Py_DECREF(l_result);
            return NULL;
        }
        Py_DECREF(l_order);
        
        l_count++;
        
        // Check limit
        if (limit > 0 && l_count >= limit)
            break;
    }
    
    dap_ledger_datum_iter_delete(it);
    
    return l_result;
}

/**
 * @brief Get list of whitelisted trading pairs
 * 
 * Python signature: getPairs(net)
 */
PyObject *wrapping_dap_chain_net_srv_dex_get_pairs(PyObject *self, PyObject *args)
{
    (void)self;
    
    PyObject *obj_net = NULL;
    
    if (!PyArg_ParseTuple(args, "O", &obj_net)) {
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_net, &DapChainNetObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a ChainNet object");
        return NULL;
    }
    
    dap_chain_net_t *l_net = ((PyDapChainNetObject *)obj_net)->chain_net;
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network object");
        return NULL;
    }
    
    // Create result list
    PyObject *l_result = PyList_New(0);
    if (!l_result) {
        return PyErr_NoMemory();
    }
    
    // We need to find pairs by scanning ledger for decree transactions
    // or by looking at existing DEX orders to discover pairs
    // For now, scan existing orders to build pair list
    
    // Use a simple hash set to track unique pairs
    typedef struct pair_entry {
        char key[DAP_CHAIN_TICKER_SIZE_MAX * 2 + 2];
        char base[DAP_CHAIN_TICKER_SIZE_MAX];
        char quote[DAP_CHAIN_TICKER_SIZE_MAX];
        UT_hash_handle hh;
    } pair_entry_t;
    
    pair_entry_t *l_pairs_set = NULL;
    
    dap_ledger_datum_iter_t *it = dap_ledger_datum_iter_create(l_net);
    if (!it) {
        Py_DECREF(l_result);
        PyErr_SetString(PyExc_RuntimeError, "Failed to create ledger iterator");
        return NULL;
    }
    
    dap_time_t l_now_ts = dap_ledger_get_blockchain_time(l_net->pub.ledger);
    
    for (dap_chain_datum_tx_t *l_tx = dap_ledger_datum_iter_get_first(it); 
         l_tx; 
         l_tx = dap_ledger_datum_iter_get_next(it)) {
        
        int l_out_idx = 0;
        dap_chain_tx_out_cond_t *l_out_cond = dap_chain_datum_tx_out_cond_get(
            l_tx, DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_DEX, &l_out_idx);
        
        if (!l_out_cond)
            continue;
        
        // Get sell token
        const char *l_sell_tok = dap_ledger_tx_get_token_ticker_by_hash(l_net->pub.ledger, &it->cur_hash);
        if (!l_sell_tok)
            continue;
        
        // Normalize pair
        char l_base[DAP_CHAIN_TICKER_SIZE_MAX] = {0};
        char l_quote[DAP_CHAIN_TICKER_SIZE_MAX] = {0};
        s_pair_normalize(l_sell_tok, l_out_cond->subtype.srv_dex.buy_token, l_base, l_quote, NULL);
        
        // Create key
        char l_key[DAP_CHAIN_TICKER_SIZE_MAX * 2 + 2];
        snprintf(l_key, sizeof(l_key), "%s/%s", l_base, l_quote);
        
        // Check if already in set
        pair_entry_t *l_existing = NULL;
        HASH_FIND_STR(l_pairs_set, l_key, l_existing);
        
        if (!l_existing) {
            pair_entry_t *l_new = DAP_NEW_Z(pair_entry_t);
            if (l_new) {
                dap_strncpy(l_new->key, l_key, sizeof(l_new->key) - 1);
                dap_strncpy(l_new->base, l_base, DAP_CHAIN_TICKER_SIZE_MAX - 1);
                dap_strncpy(l_new->quote, l_quote, DAP_CHAIN_TICKER_SIZE_MAX - 1);
                HASH_ADD_STR(l_pairs_set, key, l_new);
            }
        }
    }
    
    dap_ledger_datum_iter_delete(it);
    
    // Convert hash set to Python list of dicts
    pair_entry_t *l_entry, *l_tmp;
    HASH_ITER(hh, l_pairs_set, l_entry, l_tmp) {
        PyObject *l_pair_dict = PyDict_New();
        if (!l_pair_dict) {
            // Cleanup
            HASH_ITER(hh, l_pairs_set, l_entry, l_tmp) {
                HASH_DEL(l_pairs_set, l_entry);
                DAP_DELETE(l_entry);
            }
            Py_DECREF(l_result);
            return PyErr_NoMemory();
        }
        
        PyDict_SetItemString(l_pair_dict, "pair", PyUnicode_FromString(l_entry->key));
        PyDict_SetItemString(l_pair_dict, "base_token", PyUnicode_FromString(l_entry->base));
        PyDict_SetItemString(l_pair_dict, "quote_token", PyUnicode_FromString(l_entry->quote));
        
        if (PyList_Append(l_result, l_pair_dict) < 0) {
            Py_DECREF(l_pair_dict);
            HASH_ITER(hh, l_pairs_set, l_entry, l_tmp) {
                HASH_DEL(l_pairs_set, l_entry);
                DAP_DELETE(l_entry);
            }
            Py_DECREF(l_result);
            return NULL;
        }
        Py_DECREF(l_pair_dict);
        
        HASH_DEL(l_pairs_set, l_entry);
        DAP_DELETE(l_entry);
    }
    
    return l_result;
}

/* Method definitions */
PyMethodDef DapChainNetSrvDexMethods[] = {
    {
        "getOrders",
        (PyCFunction)(void(*)(void))wrapping_dap_chain_net_srv_dex_get_orders,
        METH_VARARGS | METH_KEYWORDS | METH_STATIC,
        "Get list of DEX orders.\n\n"
        "Args:\n"
        "    net: ChainNet object\n"
        "    pair: Optional pair string 'BASE/QUOTE'\n"
        "    seller: Optional seller address string\n"
        "    limit: Maximum orders to return (0 = unlimited)\n"
        "    offset: Number of orders to skip\n\n"
        "Returns:\n"
        "    List of DexOrder objects"
    },
    {
        "getPairs",
        wrapping_dap_chain_net_srv_dex_get_pairs,
        METH_VARARGS | METH_STATIC,
        "Get list of trading pairs discovered from ledger.\n\n"
        "Args:\n"
        "    net: ChainNet object\n\n"
        "Returns:\n"
        "    List of dicts with pair, base_token, quote_token"
    },
    {NULL, NULL, 0, NULL}
};

/* Type definition */
PyTypeObject DapChainNetSrvDexObjectType = DAP_PY_TYPE_OBJECT(
    "CellFrame.Services.Dex",
    sizeof(PyDapChainNetSrvDexObject),
    "DEX v2 service for decentralized exchange",
    .tp_methods = DapChainNetSrvDexMethods
);
