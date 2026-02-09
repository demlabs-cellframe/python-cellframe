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
#pragma once

#include <Python.h>
#include "dap_chain_net.h"
#include "dap_hash.h"
#include "dap_chain_common.h"
#include "dap_math_ops.h"
#include "dap_time.h"

/**
 * @brief DEX Order object for Python
 * Contains all order information from cache or ledger
 */
typedef struct PyDapChainNetSrvDexOrder {
    PyObject_HEAD
    dap_chain_net_t *net;           // Network reference
    dap_hash_fast_t root_hash;      // Order root hash
    dap_hash_fast_t tail_hash;      // Current tail hash
    char pair[DAP_CHAIN_TICKER_SIZE_MAX * 2 + 2];  // "BASE/QUOTE"
    char token_sell[DAP_CHAIN_TICKER_SIZE_MAX];    // Sell token ticker
    char token_buy[DAP_CHAIN_TICKER_SIZE_MAX];     // Buy token ticker
    uint256_t value_sell;           // Current sell value
    uint256_t rate;                 // Rate (QUOTE/BASE)
    dap_chain_addr_t seller_addr;   // Seller address
    uint8_t side;                   // 0 = ASK, 1 = BID
    uint8_t min_fill;               // Min fill policy (bit7=from_origin, bits0-6=percent)
    uint8_t filled_pct;             // Current fill percentage
    dap_time_t ts_created;          // Creation timestamp
    dap_time_t ts_expires;          // Expiration timestamp (0 = never)
} PyDapChainNetSrvDexOrderObject;

// Property getters
PyObject *wrapping_dap_chain_net_srv_dex_order_get_root_hash(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_tail_hash(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_pair(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_side(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_token_sell(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_token_buy(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_value_sell(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_rate(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_seller_addr(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_min_fill_pct(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_min_fill_from_origin(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_filled_pct(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_ts_created(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_ts_expires(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_net_srv_dex_order_get_net(PyObject *self, void *closure);

// Methods
PyObject *wrapping_dap_chain_net_srv_dex_order_invalidate(PyObject *self, PyObject *args);
PyObject *wrapping_dap_chain_net_srv_dex_order_update(PyObject *self, PyObject *args);

extern PyTypeObject PyDapChainNetSrvDexOrderObjectType;
