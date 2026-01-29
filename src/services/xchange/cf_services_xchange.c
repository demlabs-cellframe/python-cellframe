#include "cellframe.h"
#include "cf_services_ext.h"
#include "cf_services_ext_helpers.h"
#include "dap_chain_net_srv_order.h"
#include "dap_chain_net_srv_xchange.h"
#include "dap_chain_net_srv_xchange_compose.h"
#include "../../datum/cf_datum_internal.h"

// =============================================================================
// XCHANGE
// =============================================================================

PyObject* dap_chain_net_srv_xchange_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    int l_res = dap_chain_net_srv_xchange_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_srv_xchange_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    dap_chain_net_srv_xchange_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_xchange_create_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj;
    char *a_token_buy, *a_token_sell;
    PyObject *a_datoshi_sell_obj, *a_rate_obj, *a_fee_obj;
    PyObject *a_wallet_obj;
    
    if (!PyArg_ParseTuple(a_args, "OssOOOO", &a_net_obj, &a_token_buy, &a_token_sell, &a_datoshi_sell_obj, &a_rate_obj, &a_fee_obj, &a_wallet_obj))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj) || !PyCapsule_CheckExact(a_wallet_obj)) {
        PyErr_SetString(PyExc_TypeError, "Invalid capsule arguments");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    dap_chain_wallet_t *l_wallet = (dap_chain_wallet_t *)PyCapsule_GetPointer(a_wallet_obj, "dap_chain_wallet_t");
    uint256_t l_datoshi_sell = py_obj_to_uint256(a_datoshi_sell_obj);
    uint256_t l_rate = py_obj_to_uint256(a_rate_obj);
    uint256_t l_fee = py_obj_to_uint256(a_fee_obj);
    
    char *l_tx_hash = NULL;
    int l_ret = dap_chain_net_srv_xchange_create(l_net, a_token_buy, a_token_sell, l_datoshi_sell, l_rate, l_fee, l_wallet, &l_tx_hash);
    
    if (l_ret != 0) {
        return Py_BuildValue("is", l_ret, NULL);
    }
    return Py_BuildValue("is", l_ret, l_tx_hash);
}

PyObject* dap_chain_net_srv_xchange_remove_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj;
    char *a_hash_str;
    PyObject *a_fee_obj, *a_wallet_obj;
    
    if (!PyArg_ParseTuple(a_args, "OsOO", &a_net_obj, &a_hash_str, &a_fee_obj, &a_wallet_obj))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj) || !PyCapsule_CheckExact(a_wallet_obj)) {
        PyErr_SetString(PyExc_TypeError, "Invalid capsule arguments");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    dap_chain_wallet_t *l_wallet = (dap_chain_wallet_t *)PyCapsule_GetPointer(a_wallet_obj, "dap_chain_wallet_t");
    dap_chain_hash_fast_t l_hash;
    dap_chain_hash_fast_from_str(a_hash_str, &l_hash);
    uint256_t l_fee = py_obj_to_uint256(a_fee_obj);
    
    char *l_out_hash = NULL;
    int l_ret = dap_chain_net_srv_xchange_remove(l_net, &l_hash, l_fee, l_wallet, &l_out_hash);
    
    return Py_BuildValue("is", l_ret, l_out_hash);
}

PyObject* dap_chain_net_srv_xchange_get_fee_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj;

    if (!PyArg_ParseTuple(a_args, "O", &a_net_obj))
        return NULL;

    if (!PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    uint256_t l_fee = {};
    dap_chain_addr_t l_addr = {};
    uint16_t l_type = 0;
    bool l_has_fee = dap_chain_net_srv_xchange_get_fee(l_net->pub.id, &l_fee, &l_addr, &l_type);
    if (!l_has_fee) {
        Py_RETURN_NONE;
    }

    const char *l_fee_coins_str = NULL;
    const char *l_fee_datoshi_str = dap_uint256_to_char(l_fee, &l_fee_coins_str);
    if (!l_fee_datoshi_str) {
        Py_RETURN_NONE;
    }

    const char *l_addr_str = dap_chain_addr_to_str(&l_addr);
    if (!l_addr_str) {
        Py_RETURN_NONE;
    }

    return Py_BuildValue("{s:s,s:s,s:s,s:H}",
        "datoshi", l_fee_datoshi_str,
        "coins", l_fee_coins_str ? l_fee_coins_str : "0",
        "addr", l_addr_str,
        "type", l_type);
}

PyObject* dap_chain_net_srv_xchange_get_order_completion_rate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_net_obj;
    char *a_hash_str;

    if (!PyArg_ParseTuple(a_args, "Os", &a_net_obj, &a_hash_str))
        return NULL;

    if (!PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_hash_fast_t l_hash;
    dap_chain_hash_fast_from_str(a_hash_str, &l_hash);

    uint64_t l_rate = dap_chain_net_srv_xchange_get_order_completion_rate(l_net, l_hash);
    return PyLong_FromUnsignedLongLong((unsigned long long)l_rate);
}

PyObject* dap_chain_net_srv_xchange_purchase_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    PyErr_SetString(PyExc_NotImplementedError,
        "dap_chain_net_srv_xchange_purchase is not available in Cellframe SDK 6.0");
    return NULL;
}

PyObject* dap_chain_net_srv_xchange_get_tx_xchange_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    PyErr_SetString(PyExc_NotImplementedError,
        "dap_chain_net_srv_xchange_get_tx_xchange is not available in Cellframe SDK 6.0");
    return NULL;
}

PyObject* dap_chain_net_srv_xchange_get_prices_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    PyErr_SetString(PyExc_NotImplementedError,
        "dap_chain_net_srv_xchange_get_prices is not available in Cellframe SDK 6.0");
    return NULL;
}

PyObject* dap_chain_net_srv_xchange_print_fee_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    PyErr_SetString(PyExc_NotImplementedError,
        "dap_chain_net_srv_xchange_print_fee is not available in Cellframe SDK 6.0");
    return NULL;
}

PyObject* dap_chain_net_srv_xchange_print_fee_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    PyErr_SetString(PyExc_NotImplementedError,
        "dap_chain_net_srv_xchange_print_fee_json is not available in Cellframe SDK 6.0");
    return NULL;
}

PyObject* dap_chain_net_srv_xchange_tx_get_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_ledger_obj = NULL;
    PyObject *a_tx_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &a_ledger_obj, &a_tx_obj))
        return NULL;

    if (!PyCapsule_CheckExact(a_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    if (!PyCapsule_CheckExact(a_tx_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a tx capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(a_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t *)PyCapsule_GetPointer(a_tx_obj, "dap_chain_datum_tx_t");
    if (!l_tx) {
        PyErr_SetString(PyExc_ValueError, "Invalid tx capsule");
        return NULL;
    }

    dap_chain_tx_out_cond_t *l_out_cond = NULL;
    dap_chain_tx_out_cond_t *l_prev_cond = NULL;
    int l_item_idx = -1;
    xchange_tx_type_t l_type = dap_chain_net_srv_xchange_tx_get_type(
        l_ledger, l_tx, &l_out_cond, &l_item_idx, &l_prev_cond);

    PyObject *l_tuple = PyTuple_New(4);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_type));
    if (l_out_cond) {
        PyTuple_SetItem(l_tuple, 1, PyCapsule_New(l_out_cond, "dap_chain_tx_out_cond_t", NULL));
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 1, Py_None);
    }
    PyTuple_SetItem(l_tuple, 2, PyLong_FromLong(l_item_idx));
    if (l_prev_cond) {
        PyTuple_SetItem(l_tuple, 3, PyCapsule_New(l_prev_cond, "dap_chain_tx_out_cond_t", NULL));
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 3, Py_None);
    }
    return l_tuple;
}

PyObject* dap_chain_net_srv_xchange_get_order_status_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj;
    char *a_hash_str;
    if (!PyArg_ParseTuple(a_args, "Os", &a_net_obj, &a_hash_str))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    dap_chain_hash_fast_t l_hash;
    dap_chain_hash_fast_from_str(a_hash_str, &l_hash);
    
    dap_chain_net_srv_xchange_order_status_t l_status = dap_chain_net_srv_xchange_get_order_status(l_net, l_hash);
    return PyLong_FromLong(l_status);
}

// =============================================================================
// XCHANGE COMPOSE
// =============================================================================

PyObject* dap_chain_net_srv_xchange_compose_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    int l_res = dap_chain_net_srv_xchange_compose_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_srv_xchange_compose_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    dap_chain_net_srv_xchange_compose_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_tx_compose_xchange_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    PyErr_SetString(PyExc_NotImplementedError,
        "dap_chain_tx_compose_xchange_create is not available in Cellframe SDK 6.0");
    return NULL;
}

PyObject* dap_chain_tx_compose_xchange_order_remove_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    PyErr_SetString(PyExc_NotImplementedError,
        "dap_chain_tx_compose_xchange_order_remove is not available in Cellframe SDK 6.0");
    return NULL;
}

PyObject* dap_chain_tx_compose_xchange_purchase_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    PyErr_SetString(PyExc_NotImplementedError,
        "dap_chain_tx_compose_xchange_purchase is not available in Cellframe SDK 6.0");
    return NULL;
}

PyObject* dap_xchange_tx_create_order_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_ledger_obj = NULL;
    char *a_token_buy = NULL;
    char *a_token_sell = NULL;
    PyObject *a_datoshi_sell_obj = NULL;
    PyObject *a_rate_obj = NULL;
    PyObject *a_fee_obj = NULL;
    PyObject *a_wallet_addr_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OssOOOO", &a_ledger_obj, &a_token_buy, &a_token_sell,
        &a_datoshi_sell_obj, &a_rate_obj, &a_fee_obj, &a_wallet_addr_obj))
        return NULL;

    if (!PyCapsule_CheckExact(a_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(a_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    uint256_t l_datoshi_sell = {};
    if (cf_datum_parse_uint256(a_datoshi_sell_obj, &l_datoshi_sell) != 0) {
        return NULL;
    }

    uint256_t l_rate = {};
    if (cf_datum_parse_uint256(a_rate_obj, &l_rate) != 0) {
        return NULL;
    }

    uint256_t l_fee = {};
    if (cf_datum_parse_uint256(a_fee_obj, &l_fee) != 0) {
        return NULL;
    }

    dap_chain_addr_t l_wallet_addr = {};
    if (cf_datum_parse_addr(a_wallet_addr_obj, &l_wallet_addr) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *l_tx = dap_xchange_tx_create_order(
        l_ledger, a_token_buy, a_token_sell, l_datoshi_sell, l_rate, l_fee, &l_wallet_addr);
    if (!l_tx) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

PyObject* dap_xchange_tx_create_invalidate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_ledger_obj = NULL;
    PyObject *a_order_hash_obj = NULL;
    PyObject *a_fee_obj = NULL;
    PyObject *a_wallet_addr_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OOOO", &a_ledger_obj, &a_order_hash_obj, &a_fee_obj, &a_wallet_addr_obj))
        return NULL;

    if (!PyCapsule_CheckExact(a_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(a_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_chain_hash_fast_t l_order_hash;
    if (cf_datum_parse_hash_fast(a_order_hash_obj, &l_order_hash) != 0) {
        return NULL;
    }

    uint256_t l_fee = {};
    if (cf_datum_parse_uint256(a_fee_obj, &l_fee) != 0) {
        return NULL;
    }

    dap_chain_addr_t l_wallet_addr = {};
    if (cf_datum_parse_addr(a_wallet_addr_obj, &l_wallet_addr) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *l_tx = dap_xchange_tx_create_invalidate(
        l_ledger, &l_order_hash, l_fee, &l_wallet_addr);
    if (!l_tx) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

PyObject* dap_xchange_tx_create_purchase_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_ledger_obj = NULL;
    PyObject *a_order_hash_obj = NULL;
    PyObject *a_value_obj = NULL;
    PyObject *a_fee_obj = NULL;
    PyObject *a_wallet_addr_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OOOOO", &a_ledger_obj, &a_order_hash_obj, &a_value_obj,
        &a_fee_obj, &a_wallet_addr_obj))
        return NULL;

    if (!PyCapsule_CheckExact(a_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(a_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_chain_hash_fast_t l_order_hash;
    if (cf_datum_parse_hash_fast(a_order_hash_obj, &l_order_hash) != 0) {
        return NULL;
    }

    uint256_t l_value = {};
    if (cf_datum_parse_uint256(a_value_obj, &l_value) != 0) {
        return NULL;
    }

    uint256_t l_fee = {};
    if (cf_datum_parse_uint256(a_fee_obj, &l_fee) != 0) {
        return NULL;
    }

    dap_chain_addr_t l_wallet_addr = {};
    if (cf_datum_parse_addr(a_wallet_addr_obj, &l_wallet_addr) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *l_tx = dap_xchange_tx_create_purchase(
        l_ledger, &l_order_hash, l_value, l_fee, &l_wallet_addr);
    if (!l_tx) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

PyObject* dap_xchange_find_last_tx_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_ledger_obj = NULL;
    PyObject *a_order_hash_obj = NULL;
    PyObject *a_seller_addr_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OOO", &a_ledger_obj, &a_order_hash_obj, &a_seller_addr_obj))
        return NULL;

    if (!PyCapsule_CheckExact(a_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(a_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_chain_hash_fast_t l_order_hash;
    if (cf_datum_parse_hash_fast(a_order_hash_obj, &l_order_hash) != 0) {
        return NULL;
    }

    dap_chain_addr_t l_seller_addr = {};
    if (cf_datum_parse_addr(a_seller_addr_obj, &l_seller_addr) != 0) {
        return NULL;
    }

    dap_time_t l_ts_created = 0;
    char *l_token_ticker = NULL;
    int32_t l_prev_cond_idx = -1;
    dap_hash_fast_t l_hash_out = {};

    dap_chain_tx_out_cond_t *l_out_cond = dap_xchange_find_last_tx(
        l_ledger, &l_order_hash, &l_seller_addr, &l_ts_created, &l_token_ticker,
        &l_prev_cond_idx, &l_hash_out);

    if (!l_out_cond) {
        Py_RETURN_NONE;
    }

    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "out_cond", PyCapsule_New(l_out_cond, "dap_chain_tx_out_cond_t", NULL));
    PyDict_SetItemString(l_dict, "ts_created", PyLong_FromUnsignedLongLong((unsigned long long)l_ts_created));
    if (l_token_ticker) {
        PyDict_SetItemString(l_dict, "token_ticker", PyUnicode_FromString(l_token_ticker));
        DAP_DELETE(l_token_ticker);
    } else {
        PyDict_SetItemString(l_dict, "token_ticker", Py_None);
    }
    PyDict_SetItemString(l_dict, "prev_cond_idx", PyLong_FromLong((long)l_prev_cond_idx));
    char *l_hash_str = dap_chain_hash_fast_to_str_new(&l_hash_out);
    if (l_hash_str) {
        PyDict_SetItemString(l_dict, "hash", PyUnicode_FromString(l_hash_str));
        DAP_DELETE(l_hash_str);
    } else {
        PyDict_SetItemString(l_dict, "hash", Py_None);
    }
    return l_dict;
}

PyObject* dap_xchange_price_from_order_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_ledger_obj = NULL;
    PyObject *a_cond_obj = NULL;
    unsigned long long a_ts_created = 0;
    PyObject *a_order_hash_obj = NULL;
    PyObject *a_hash_out_obj = NULL;
    const char *a_token_ticker = NULL;
    PyObject *a_fee_obj = Py_None;
    int a_ret_is_invalid = 0;

    if (!PyArg_ParseTuple(a_args, "OOKOOsO|p", &a_ledger_obj, &a_cond_obj, &a_ts_created,
        &a_order_hash_obj, &a_hash_out_obj, &a_token_ticker, &a_fee_obj, &a_ret_is_invalid))
        return NULL;

    if (!PyCapsule_CheckExact(a_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    if (!PyCapsule_CheckExact(a_cond_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a tx out cond capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(a_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_chain_tx_out_cond_t *l_cond = (dap_chain_tx_out_cond_t *)PyCapsule_GetPointer(
        a_cond_obj, "dap_chain_tx_out_cond_t");
    if (!l_cond) {
        PyErr_SetString(PyExc_ValueError, "Invalid tx out cond capsule");
        return NULL;
    }

    dap_chain_hash_fast_t l_order_hash;
    if (cf_datum_parse_hash_fast(a_order_hash_obj, &l_order_hash) != 0) {
        return NULL;
    }

    dap_chain_hash_fast_t l_hash_out;
    if (cf_datum_parse_hash_fast(a_hash_out_obj, &l_hash_out) != 0) {
        return NULL;
    }

    uint256_t l_fee = {};
    uint256_t *l_fee_ptr = NULL;
    if (a_fee_obj && a_fee_obj != Py_None) {
        if (cf_datum_parse_uint256(a_fee_obj, &l_fee) != 0) {
            return NULL;
        }
        l_fee_ptr = &l_fee;
    }

    dap_chain_net_srv_xchange_price_t *l_price = dap_xchange_price_from_order(
        l_ledger, l_cond, (dap_time_t)a_ts_created, &l_order_hash, &l_hash_out,
        a_token_ticker, l_fee_ptr, (bool)a_ret_is_invalid);

    if (!l_price) {
        Py_RETURN_NONE;
    }

    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "token_sell", PyUnicode_FromString(l_price->token_sell));
    PyDict_SetItemString(l_dict, "token_buy", PyUnicode_FromString(l_price->token_buy));
    const char *l_rate_str = dap_uint256_to_char(l_price->rate, NULL);
    PyDict_SetItemString(l_dict, "rate", PyUnicode_FromString(l_rate_str ? l_rate_str : "0"));
    const char *l_value_str = dap_uint256_to_char(l_price->datoshi_sell, NULL);
    PyDict_SetItemString(l_dict, "datoshi_sell", PyUnicode_FromString(l_value_str ? l_value_str : "0"));
    const char *l_fee_str = dap_uint256_to_char(l_price->fee, NULL);
    PyDict_SetItemString(l_dict, "fee", PyUnicode_FromString(l_fee_str ? l_fee_str : "0"));
    PyDict_SetItemString(l_dict, "creation_date", PyLong_FromUnsignedLongLong((unsigned long long)l_price->creation_date));

    char *l_tx_hash_str = dap_chain_hash_fast_to_str_new(&l_price->tx_hash);
    if (l_tx_hash_str) {
        PyDict_SetItemString(l_dict, "tx_hash", PyUnicode_FromString(l_tx_hash_str));
        DAP_DELETE(l_tx_hash_str);
    } else {
        PyDict_SetItemString(l_dict, "tx_hash", Py_None);
    }

    char *l_order_hash_str = dap_chain_hash_fast_to_str_new(&l_price->order_hash);
    if (l_order_hash_str) {
        PyDict_SetItemString(l_dict, "order_hash", PyUnicode_FromString(l_order_hash_str));
        DAP_DELETE(l_order_hash_str);
    } else {
        PyDict_SetItemString(l_dict, "order_hash", Py_None);
    }

    const char *l_creator_addr = dap_chain_addr_to_str(&l_price->creator_addr);
    if (l_creator_addr) {
        PyDict_SetItemString(l_dict, "creator_addr", PyUnicode_FromString(l_creator_addr));
    } else {
        PyDict_SetItemString(l_dict, "creator_addr", Py_None);
    }

    DAP_DELETE(l_price);
    return l_dict;
}

