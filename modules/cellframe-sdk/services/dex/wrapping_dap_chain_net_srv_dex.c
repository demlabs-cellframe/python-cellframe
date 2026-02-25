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
#include "wrapping_dap_hash.h"
#include "wrapped_dap_chain_wallet_python.h"
#include "wrapping_dap_chain_common.h"
#include "dap_chain_ledger.h"
#include "dap_chain_datum_tx_out_cond.h"
#include "dap_chain_mempool.h"
#include "dap_chain_datum.h"
#include "dap_chain.h"
#include "dap_common.h"
#include "uthash.h"
#include "datetime.h"

#define LOG_TAG "wrapping_dap_chain_net_srv_dex"

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
        strncpy(a_base_out, a_sell_token, DAP_CHAIN_TICKER_SIZE_MAX - 1);
        strncpy(a_quote_out, a_buy_token, DAP_CHAIN_TICKER_SIZE_MAX - 1);
        if (a_side_out) *a_side_out = 0; // ASK
    } else {
        // sell >= buy: BASE=buy, QUOTE=sell → BID
        strncpy(a_base_out, a_buy_token, DAP_CHAIN_TICKER_SIZE_MAX - 1);
        strncpy(a_quote_out, a_sell_token, DAP_CHAIN_TICKER_SIZE_MAX - 1);
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
            strncpy(l_ticker_base, l_pair_storage, DAP_CHAIN_TICKER_SIZE_MAX - 1);
            strncpy(l_ticker_quote, l_slash, DAP_CHAIN_TICKER_SIZE_MAX - 1);
        } else {
            strncpy(l_ticker_base, l_slash, DAP_CHAIN_TICKER_SIZE_MAX - 1);
            strncpy(l_ticker_quote, l_pair_storage, DAP_CHAIN_TICKER_SIZE_MAX - 1);
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
        strncpy(l_order->token_sell, l_sell_tok, DAP_CHAIN_TICKER_SIZE_MAX - 1);
        strncpy(l_order->token_buy, l_out_cond->subtype.srv_dex.buy_token, DAP_CHAIN_TICKER_SIZE_MAX - 1);
        
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

/**
 * @brief Create new DEX order
 * 
 * Python signature: create(net, token_sell, token_buy, value_sell, rate, fee, wallet, 
 *                          min_fill_pct=0, min_fill_from_origin=False) -> str (tx_hash)
 */
PyObject *wrapping_dap_chain_net_srv_dex_create(PyObject *self, PyObject *args, PyObject *kwargs)
{
    (void)self;
    
    PyObject *obj_net = NULL;
    const char *token_sell = NULL;
    const char *token_buy = NULL;
    PyObject *obj_value_sell = NULL;
    PyObject *obj_rate = NULL;
    PyObject *obj_fee = NULL;
    PyObject *obj_wallet = NULL;
    int min_fill_pct = 0;
    int min_fill_from_origin = 0;
    
    static char *kwlist[] = {"net", "token_sell", "token_buy", "value_sell", "rate", 
                             "fee", "wallet", "min_fill_pct", "min_fill_from_origin", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OssOOOO|ip", kwlist,
                                      &obj_net, &token_sell, &token_buy,
                                      &obj_value_sell, &obj_rate, &obj_fee, &obj_wallet,
                                      &min_fill_pct, &min_fill_from_origin)) {
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_net, &DapChainNetObjectType)) {
        PyErr_SetString(PyExc_TypeError, "First argument 'net' must be a ChainNet object");
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_value_sell)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'value_sell' must be a Math object");
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_rate)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'rate' must be a Math object");
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_fee)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'fee' must be a Math object");
        return NULL;
    }
    
    if (!PyDapChainWalletObject_Check(obj_wallet)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'wallet' must be a Wallet object");
        return NULL;
    }
    
    if (min_fill_pct < 0 || min_fill_pct > 100) {
        PyErr_SetString(PyExc_ValueError, "min_fill_pct must be between 0 and 100");
        return NULL;
    }
    
    dap_chain_net_t *l_net = ((PyDapChainNetObject *)obj_net)->chain_net;
    uint256_t l_value_sell = ((DapMathObject *)obj_value_sell)->value;
    uint256_t l_rate = ((DapMathObject *)obj_rate)->value;
    uint256_t l_fee = ((DapMathObject *)obj_fee)->value;
    dap_chain_wallet_t *l_wallet = ((PyDapChainWalletObject *)obj_wallet)->wallet;
    
    // Combine min_fill_pct and min_fill_from_origin into single byte
    uint8_t l_min_fill_combined = (uint8_t)(min_fill_pct & 0x7F);
    if (min_fill_from_origin)
        l_min_fill_combined |= 0x80;
    
    const dap_chain_addr_t *l_owner_addr = dap_chain_wallet_get_addr(l_wallet, l_net->pub.id);
    
    dap_chain_datum_tx_t *l_tx = NULL;
    int l_ret = dap_chain_net_srv_dex_create(l_net, token_buy, token_sell, l_value_sell, 
                                              l_rate, l_min_fill_combined, l_fee, l_wallet, 
                                              l_owner_addr, &l_tx);
    
    switch (l_ret) {
        case DEX_CREATE_ERROR_OK: {
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
        case DEX_CREATE_ERROR_INVALID_ARGUMENT:
            PyErr_SetString(PyExc_ValueError, "Invalid argument");
            return NULL;
        case DEX_CREATE_ERROR_TOKEN_TICKER_SELL_NOT_FOUND:
            PyErr_SetString(PyExc_ValueError, "Sell token ticker not found");
            return NULL;
        case DEX_CREATE_ERROR_TOKEN_TICKER_BUY_NOT_FOUND:
            PyErr_SetString(PyExc_ValueError, "Buy token ticker not found");
            return NULL;
        case DEX_CREATE_ERROR_PAIR_NOT_ALLOWED:
            PyErr_SetString(PyExc_ValueError, "Trading pair not allowed (not whitelisted)");
            return NULL;
        case DEX_CREATE_ERROR_RATE_IS_ZERO:
            PyErr_SetString(PyExc_ValueError, "Rate must be non-zero");
            return NULL;
        case DEX_CREATE_ERROR_FEE_IS_ZERO:
            PyErr_SetString(PyExc_ValueError, "Fee must be non-zero");
            return NULL;
        case DEX_CREATE_ERROR_VALUE_SELL_IS_ZERO:
            PyErr_SetString(PyExc_ValueError, "Sell value must be non-zero");
            return NULL;
        case DEX_CREATE_ERROR_INTEGER_OVERFLOW:
            PyErr_SetString(PyExc_OverflowError, "Integer overflow in calculation");
            return NULL;
        case DEX_CREATE_ERROR_NOT_ENOUGH_CASH_FOR_FEE:
            PyErr_SetString(PyExc_ValueError, "Not enough funds for fee");
            return NULL;
        case DEX_CREATE_ERROR_NOT_ENOUGH_CASH:
            PyErr_SetString(PyExc_ValueError, "Not enough funds");
            return NULL;
        case DEX_CREATE_ERROR_COMPOSE_TX:
            PyErr_SetString(PyExc_RuntimeError, "Failed to compose transaction");
            return NULL;
        default:
            PyErr_Format(PyExc_RuntimeError, "Create error code: %d", l_ret);
            return NULL;
    }
}

/**
 * @brief Execute trade against order by hash
 * 
 * Python signature: purchase(net, order_hash, value, fee, wallet, is_budget_buy=False,
 *                            create_leftover_order=False, leftover_rate=None, leftover_min_fill=0,
 *                            recipient_addr=None) -> str
 */
PyObject *wrapping_dap_chain_net_srv_dex_purchase(PyObject *self, PyObject *args, PyObject *kwargs)
{
    (void)self;
    
    PyObject *obj_net = NULL;
    PyObject *obj_order_hash = NULL;
    PyObject *obj_value = NULL;
    PyObject *obj_fee = NULL;
    PyObject *obj_wallet = NULL;
    int is_budget_buy = 0;
    int create_leftover_order = 0;
    PyObject *obj_leftover_rate = NULL;
    int leftover_min_fill = 0;
    PyObject *obj_recipient_addr = NULL;
    
    static char *kwlist[] = {"net", "order_hash", "value", "fee", "wallet", 
                             "is_budget_buy", "create_leftover_order", 
                             "leftover_rate", "leftover_min_fill", "recipient_addr", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOOOO|ppOiO", kwlist,
                                      &obj_net, &obj_order_hash, &obj_value, 
                                      &obj_fee, &obj_wallet,
                                      &is_budget_buy, &create_leftover_order,
                                      &obj_leftover_rate, &leftover_min_fill, &obj_recipient_addr)) {
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_net, &DapChainNetObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'net' must be a ChainNet object");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_order_hash, &DapChainHashFastObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'order_hash' must be a HashFast object");
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_value)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'value' must be a Math object");
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_fee)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'fee' must be a Math object");
        return NULL;
    }
    
    if (!PyDapChainWalletObject_Check(obj_wallet)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'wallet' must be a Wallet object");
        return NULL;
    }
    
    dap_chain_net_t *l_net = ((PyDapChainNetObject *)obj_net)->chain_net;
    dap_hash_fast_t l_order_hash = *((PyDapHashFastObject *)obj_order_hash)->hash_fast;
    uint256_t l_value = ((DapMathObject *)obj_value)->value;
    uint256_t l_fee = ((DapMathObject *)obj_fee)->value;
    dap_chain_wallet_t *l_wallet = ((PyDapChainWalletObject *)obj_wallet)->wallet;
    
    uint256_t l_leftover_rate = {0};
    if (obj_leftover_rate && obj_leftover_rate != Py_None) {
        if (!DapMathObject_Check(obj_leftover_rate)) {
            PyErr_SetString(PyExc_TypeError, "Argument 'leftover_rate' must be a Math object or None");
            return NULL;
        }
        l_leftover_rate = ((DapMathObject *)obj_leftover_rate)->value;
    }
    
    // Use recipient_addr if provided, otherwise use wallet address
    const dap_chain_addr_t *l_buyer_addr = NULL;
    if (obj_recipient_addr && obj_recipient_addr != Py_None) {
        if (!PyDapChainAddrObject_Check((PyDapChainAddrObject *)obj_recipient_addr)) {
            PyErr_SetString(PyExc_TypeError, "Argument 'recipient_addr' must be a ChainAddr object or None");
            return NULL;
        }
        l_buyer_addr = ((PyDapChainAddrObject *)obj_recipient_addr)->addr;
    } else {
        l_buyer_addr = dap_chain_wallet_get_addr(l_wallet, l_net->pub.id);
    }
    
    dap_chain_datum_tx_t *l_tx = NULL;
    int l_ret = dap_chain_net_srv_dex_purchase(l_net, &l_order_hash, l_value, 
                                                (bool)is_budget_buy, l_fee, l_wallet, l_buyer_addr,
                                                (bool)create_leftover_order, l_leftover_rate, 
                                                (uint8_t)leftover_min_fill, &l_tx);
    
    switch (l_ret) {
        case DEX_PURCHASE_ERROR_OK: {
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
        case DEX_PURCHASE_ERROR_INVALID_ARGUMENT:
            PyErr_SetString(PyExc_ValueError, "Invalid argument");
            return NULL;
        case DEX_PURCHASE_ERROR_ORDER_NOT_FOUND:
            PyErr_SetString(PyExc_ValueError, "Order not found");
            return NULL;
        case DEX_PURCHASE_ERROR_ORDER_SPENT:
            PyErr_SetString(PyExc_ValueError, "Order already spent or expired");
            return NULL;
        case DEX_PURCHASE_ERROR_COMPOSE_TX:
            PyErr_SetString(PyExc_RuntimeError, "Failed to compose transaction");
            return NULL;
        default:
            PyErr_Format(PyExc_RuntimeError, "Purchase error code: %d", l_ret);
            return NULL;
    }
}

/**
 * @brief Auto-match purchase: find best orders and execute trade
 * 
 * Python signature: purchaseAuto(net, token_sell, token_buy, value, fee, wallet, 
 *                                is_budget_buy=False, rate_cap=None, create_leftover_order=False,
 *                                leftover_rate=None, leftover_min_fill=0, recipient_addr=None) -> str
 */
PyObject *wrapping_dap_chain_net_srv_dex_purchase_auto(PyObject *self, PyObject *args, PyObject *kwargs)
{
    (void)self;
    
    PyObject *obj_net = NULL;
    const char *token_sell = NULL;
    const char *token_buy = NULL;
    PyObject *obj_value = NULL;
    PyObject *obj_fee = NULL;
    PyObject *obj_wallet = NULL;
    int is_budget_buy = 0;
    PyObject *obj_rate_cap = NULL;
    int create_leftover_order = 0;
    PyObject *obj_leftover_rate = NULL;
    int leftover_min_fill = 0;
    PyObject *obj_recipient_addr = NULL;
    
    static char *kwlist[] = {"net", "token_sell", "token_buy", "value", "fee", "wallet",
                             "is_budget_buy", "rate_cap", "create_leftover_order",
                             "leftover_rate", "leftover_min_fill", "recipient_addr", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OssOOO|pOpOiO", kwlist,
                                      &obj_net, &token_sell, &token_buy,
                                      &obj_value, &obj_fee, &obj_wallet,
                                      &is_budget_buy, &obj_rate_cap, &create_leftover_order,
                                      &obj_leftover_rate, &leftover_min_fill, &obj_recipient_addr)) {
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_net, &DapChainNetObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'net' must be a ChainNet object");
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_value)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'value' must be a Math object");
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_fee)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'fee' must be a Math object");
        return NULL;
    }
    
    if (!PyDapChainWalletObject_Check(obj_wallet)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'wallet' must be a Wallet object");
        return NULL;
    }
    
    dap_chain_net_t *l_net = ((PyDapChainNetObject *)obj_net)->chain_net;
    uint256_t l_value = ((DapMathObject *)obj_value)->value;
    uint256_t l_fee = ((DapMathObject *)obj_fee)->value;
    dap_chain_wallet_t *l_wallet = ((PyDapChainWalletObject *)obj_wallet)->wallet;
    
    uint256_t l_rate_cap = {0};
    if (obj_rate_cap && obj_rate_cap != Py_None) {
        if (!DapMathObject_Check(obj_rate_cap)) {
            PyErr_SetString(PyExc_TypeError, "Argument 'rate_cap' must be a Math object or None");
            return NULL;
        }
        l_rate_cap = ((DapMathObject *)obj_rate_cap)->value;
    }
    
    uint256_t l_leftover_rate = {0};
    if (obj_leftover_rate && obj_leftover_rate != Py_None) {
        if (!DapMathObject_Check(obj_leftover_rate)) {
            PyErr_SetString(PyExc_TypeError, "Argument 'leftover_rate' must be a Math object or None");
            return NULL;
        }
        l_leftover_rate = ((DapMathObject *)obj_leftover_rate)->value;
    }
    
    // Use recipient_addr if provided, otherwise use wallet address
    const dap_chain_addr_t *l_buyer_addr = NULL;
    if (obj_recipient_addr && obj_recipient_addr != Py_None) {
        if (!PyDapChainAddrObject_Check((PyDapChainAddrObject *)obj_recipient_addr)) {
            PyErr_SetString(PyExc_TypeError, "Argument 'recipient_addr' must be a ChainAddr object or None");
            return NULL;
        }
        l_buyer_addr = ((PyDapChainAddrObject *)obj_recipient_addr)->addr;
    } else {
        l_buyer_addr = dap_chain_wallet_get_addr(l_wallet, l_net->pub.id);
    }
    
    dap_chain_datum_tx_t *l_tx = NULL;
    int l_ret = dap_chain_net_srv_dex_purchase_auto(l_net, token_sell, token_buy, l_value,
                                                     (bool)is_budget_buy, l_fee, l_rate_cap, 
                                                     l_wallet, l_buyer_addr,
                                                     (bool)create_leftover_order, l_leftover_rate,
                                                     (uint8_t)leftover_min_fill, &l_tx);
    
    switch (l_ret) {
        case DEX_PURCHASE_ERROR_OK: {
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
        case DEX_PURCHASE_ERROR_INVALID_ARGUMENT:
            PyErr_SetString(PyExc_ValueError, "Invalid argument");
            return NULL;
        case DEX_PURCHASE_AUTO_ERROR_NO_MATCHES:
            PyErr_SetString(PyExc_ValueError, "No matching orders found");
            return NULL;
        case DEX_PURCHASE_ERROR_COMPOSE_TX:
            PyErr_SetString(PyExc_RuntimeError, "Failed to compose transaction");
            return NULL;
        default:
            PyErr_Format(PyExc_RuntimeError, "Purchase auto error code: %d", l_ret);
            return NULL;
    }
}

/**
 * @brief Execute DEX purchase using specific UTXO as input
 * 
 * This function is designed for bridge-dex scenarios where a specific UTXO
 * must be consumed to prevent double-spending. The sell token is taken from
 * the forced UTXO, while fee is collected automatically from the wallet.
 * 
 * @param net Network object
 * @param token_sell Token ticker to sell (must match UTXO token)
 * @param token_buy Token ticker to buy
 * @param fee Validator fee amount (Math object)
 * @param wallet Wallet for signing and fee source
 * @param utxo_tx_hash TX hash containing the UTXO (string)
 * @param utxo_out_idx Output index in the TX
 * @param utxo_value Value of the UTXO (Math object)
 * @param rate_cap Optional rate limit (Math object or None)
 * @param recipient_addr Optional recipient address (ChainAddr or None)
 * @return TX hash string on success, raises exception on error
 */
PyObject *wrapping_dap_chain_net_srv_dex_purchase_auto_with_utxo(PyObject *self, PyObject *args, PyObject *kwargs)
{
    (void)self;
    
    PyObject *obj_net = NULL;
    const char *token_sell = NULL;
    const char *token_buy = NULL;
    PyObject *obj_fee = NULL;
    PyObject *obj_wallet = NULL;
    const char *utxo_tx_hash_str = NULL;
    unsigned int utxo_out_idx = 0;
    PyObject *obj_utxo_value = NULL;
    PyObject *obj_rate_cap = NULL;
    PyObject *obj_recipient_addr = NULL;
    
    static char *kwlist[] = {"net", "token_sell", "token_buy", "fee", "wallet",
                             "utxo_tx_hash", "utxo_out_idx", "utxo_value",
                             "rate_cap", "recipient_addr", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OssOOsIO|OO", kwlist,
                                      &obj_net, &token_sell, &token_buy,
                                      &obj_fee, &obj_wallet,
                                      &utxo_tx_hash_str, &utxo_out_idx, &obj_utxo_value,
                                      &obj_rate_cap, &obj_recipient_addr)) {
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_net, &DapChainNetObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'net' must be a ChainNet object");
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_fee)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'fee' must be a Math object");
        return NULL;
    }
    
    if (!PyDapChainWalletObject_Check(obj_wallet)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'wallet' must be a Wallet object");
        return NULL;
    }
    
    if (!DapMathObject_Check(obj_utxo_value)) {
        PyErr_SetString(PyExc_TypeError, "Argument 'utxo_value' must be a Math object");
        return NULL;
    }
    
    // Parse UTXO TX hash
    dap_hash_fast_t l_utxo_tx_hash = {0};
    if (dap_chain_hash_fast_from_str(utxo_tx_hash_str, &l_utxo_tx_hash) != 0) {
        PyErr_SetString(PyExc_ValueError, "Invalid utxo_tx_hash format");
        return NULL;
    }
    
    dap_chain_net_t *l_net = ((PyDapChainNetObject *)obj_net)->chain_net;
    uint256_t l_fee = ((DapMathObject *)obj_fee)->value;
    dap_chain_wallet_t *l_wallet = ((PyDapChainWalletObject *)obj_wallet)->wallet;
    uint256_t l_utxo_value = ((DapMathObject *)obj_utxo_value)->value;
    
    uint256_t l_rate_cap = {0};
    if (obj_rate_cap && obj_rate_cap != Py_None) {
        if (!DapMathObject_Check(obj_rate_cap)) {
            PyErr_SetString(PyExc_TypeError, "Argument 'rate_cap' must be a Math object or None");
            return NULL;
        }
        l_rate_cap = ((DapMathObject *)obj_rate_cap)->value;
    }
    
    // Use recipient_addr if provided, otherwise NULL (will use wallet address)
    const dap_chain_addr_t *l_recipient_addr = NULL;
    if (obj_recipient_addr && obj_recipient_addr != Py_None) {
        if (!PyDapChainAddrObject_Check((PyDapChainAddrObject *)obj_recipient_addr)) {
            PyErr_SetString(PyExc_TypeError, "Argument 'recipient_addr' must be a ChainAddr object or None");
            return NULL;
        }
        l_recipient_addr = ((PyDapChainAddrObject *)obj_recipient_addr)->addr;
    }
    
    dap_chain_datum_tx_t *l_tx = NULL;
    int l_ret = dap_chain_net_srv_dex_purchase_auto_with_utxo(l_net, token_sell, token_buy, l_fee, l_rate_cap,
                                                              l_wallet, l_recipient_addr,
                                                              &l_utxo_tx_hash, utxo_out_idx, l_utxo_value, &l_tx);
    
    switch (l_ret) {
        case DEX_PURCHASE_ERROR_OK: {
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
        case DEX_PURCHASE_ERROR_INVALID_ARGUMENT:
            PyErr_SetString(PyExc_ValueError, "Invalid argument");
            return NULL;
        case DEX_PURCHASE_AUTO_ERROR_NO_MATCHES:
            PyErr_SetString(PyExc_ValueError, "No matching orders found");
            return NULL;
        case DEX_PURCHASE_ERROR_COMPOSE_TX:
            PyErr_SetString(PyExc_RuntimeError, "Failed to compose transaction");
            return NULL;
        default:
            PyErr_Format(PyExc_RuntimeError, "Purchase auto with UTXO error code: %d", l_ret);
            return NULL;
    }
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
    {
        "create",
        (PyCFunction)(void(*)(void))wrapping_dap_chain_net_srv_dex_create,
        METH_VARARGS | METH_KEYWORDS | METH_STATIC,
        "Create new DEX order.\n\n"
        "Args:\n"
        "    net: ChainNet object\n"
        "    token_sell: Token to sell (string)\n"
        "    token_buy: Token to buy (string)\n"
        "    value_sell: Amount to sell (Math)\n"
        "    rate: Exchange rate in QUOTE/BASE format (Math)\n"
        "    fee: Validator fee in native tokens (Math)\n"
        "    wallet: Wallet for signing\n"
        "    min_fill_pct: Minimum fill percentage 0-100 (default: 0)\n"
        "    min_fill_from_origin: Calculate min fill from original value (default: False)\n\n"
        "Returns:\n"
        "    Transaction hash string"
    },
    {
        "purchase",
        (PyCFunction)(void(*)(void))wrapping_dap_chain_net_srv_dex_purchase,
        METH_VARARGS | METH_KEYWORDS | METH_STATIC,
        "Execute trade against specific order by hash.\n\n"
        "Args:\n"
        "    net: ChainNet object\n"
        "    order_hash: Order hash (HashFast)\n"
        "    value: Trade amount (Math)\n"
        "    fee: Validator fee (Math)\n"
        "    wallet: Buyer wallet (source of funds)\n"
        "    is_budget_buy: If True, value is max budget in BUY tokens (default: False)\n"
        "    create_leftover_order: Create order from unspent funds (default: False)\n"
        "    leftover_rate: Rate for leftover order (Math, optional)\n"
        "    leftover_min_fill: Min fill policy for leftover order (default: 0)\n"
        "    recipient_addr: Address to receive purchased tokens (ChainAddr, optional).\n"
        "                    If None, tokens go to wallet address.\n\n"
        "Returns:\n"
        "    Transaction hash string"
    },
    {
        "purchaseAuto",
        (PyCFunction)(void(*)(void))wrapping_dap_chain_net_srv_dex_purchase_auto,
        METH_VARARGS | METH_KEYWORDS | METH_STATIC,
        "Auto-match purchase: find best orders and execute trade.\n\n"
        "Args:\n"
        "    net: ChainNet object\n"
        "    token_sell: Token to sell (string)\n"
        "    token_buy: Token to buy (string)\n"
        "    value: Trade amount (Math)\n"
        "    fee: Validator fee (Math)\n"
        "    wallet: Buyer wallet (source of funds)\n"
        "    is_budget_buy: If True, value is max budget in BUY tokens (default: False)\n"
        "    rate_cap: Rate limit for matching (Math, optional)\n"
        "    create_leftover_order: Create order from unspent funds (default: False)\n"
        "    leftover_rate: Rate for leftover order (Math, optional)\n"
        "    leftover_min_fill: Min fill policy for leftover order (default: 0)\n"
        "    recipient_addr: Address to receive purchased tokens (ChainAddr, optional).\n"
        "                    If None, tokens go to wallet address.\n\n"
        "Returns:\n"
        "    Transaction hash string"
    },
    {
        "purchaseAutoWithUtxo",
        (PyCFunction)(void(*)(void))wrapping_dap_chain_net_srv_dex_purchase_auto_with_utxo,
        METH_VARARGS | METH_KEYWORDS | METH_STATIC,
        "Auto-match purchase using specific UTXO as input for sell token.\n\n"
        "Designed for bridge-dex scenarios where a specific UTXO must be consumed\n"
        "to prevent double-spending. The sell token is taken from the forced UTXO,\n"
        "while fee is collected automatically from the wallet.\n\n"
        "Args:\n"
        "    net: ChainNet object\n"
        "    token_sell: Token to sell (string, must match UTXO token)\n"
        "    token_buy: Token to buy (string)\n"
        "    fee: Validator fee (Math)\n"
        "    wallet: Wallet for signing and fee source\n"
        "    utxo_tx_hash: TX hash containing the UTXO (string)\n"
        "    utxo_out_idx: Output index in the TX (int)\n"
        "    utxo_value: Value of the UTXO (Math)\n"
        "    rate_cap: Rate limit for matching (Math, optional)\n"
        "    recipient_addr: Address to receive purchased tokens (ChainAddr, optional).\n"
        "                    If None, tokens go to wallet address.\n\n"
        "Returns:\n"
        "    Transaction hash string\n\n"
        "Example:\n"
        "    # Use specific UTXO from bridge transaction\n"
        "    tx_hash = Dex.purchaseAutoWithUtxo(\n"
        "        net=net,\n"
        "        token_sell='wETH',\n"
        "        token_buy='CELL',\n"
        "        fee=fee_amount,\n"
        "        wallet=dex_wallet,\n"
        "        utxo_tx_hash='0x...',\n"
        "        utxo_out_idx=2,\n"
        "        utxo_value=amount_to_exchange,\n"
        "        recipient_addr=user_addr\n"
        "    )"
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
