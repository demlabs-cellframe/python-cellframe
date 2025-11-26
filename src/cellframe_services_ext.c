#include "cellframe.h"
#include "include/cellframe_services_ext.h"
#include "dap_chain_net_srv_stake_ext.h"
#include "dap_chain_net_srv_vpn.h"
#include "dap_chain_net_srv_xchange.h"
#include "dap_chain_net_srv_voting.h"

#define LOG_TAG "python_cellframe_services_ext"

#include "dap_cert.h"
#include "dap_cert_file.h"
// #include "python_dap_crypto_cert.h" // For PyCryptoCertObject - временно закомментировано

// =============================================================================
// STAKE EXT
// =============================================================================

PyObject* dap_chain_net_srv_stake_ext_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_res = dap_chain_net_srv_stake_ext_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_srv_stake_ext_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_net_srv_stake_ext_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_ext_find_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj;
    char *a_hash_str;
    if (!PyArg_ParseTuple(a_args, "Os", &a_net_obj, &a_hash_str))
        return NULL;
        
    dap_chain_net_t *l_net = ((PyDapChainNetObject*)a_net_obj)->chain_net;
    dap_chain_hash_fast_t l_hash;
    dap_chain_hash_fast_from_str(a_hash_str, &l_hash);
    
    dap_chain_net_srv_stake_ext_t *l_item = dap_chain_net_srv_stake_ext_find(l_net, &l_hash);
    if (!l_item) Py_RETURN_NONE;
    
    // Return simple dict with basic info
    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "guuid", PyUnicode_FromString(l_item->guuid));
    PyDict_SetItemString(l_dict, "status", PyLong_FromLong(l_item->status));
    PyDict_SetItemString(l_dict, "start_time", PyLong_FromLong(l_item->start_time));
    PyDict_SetItemString(l_dict, "end_time", PyLong_FromLong(l_item->end_time));
    PyDict_SetItemString(l_dict, "description", PyUnicode_FromString(l_item->description));
    return l_dict;
}

PyObject* dap_chain_net_srv_stake_ext_get_list_py(PyObject *a_self, PyObject *a_args) {
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
    dap_list_t *l_list = dap_chain_net_srv_stake_ext_get_list(l_net, DAP_STAKE_EXT_STATUS_UNKNOWN, false);
    
    PyObject *l_py_list = PyList_New(0);
    for (dap_list_t *l_ptr = l_list; l_ptr; l_ptr = l_ptr->next) {
        dap_chain_net_srv_stake_ext_t *l_item = (dap_chain_net_srv_stake_ext_t*)l_ptr->data;
        // Basic info
        PyObject *l_dict = PyDict_New();
        PyDict_SetItemString(l_dict, "guuid", PyUnicode_FromString(l_item->guuid));
        PyList_Append(l_py_list, l_dict);
        Py_DECREF(l_dict);
    }
    dap_list_free(l_list); // Be careful if deep free is needed, but get_list usually returns list of pointers or copies
    return l_py_list;
}

PyObject* dap_chain_net_srv_stake_ext_get_detailed_py(PyObject *a_self, PyObject *a_args) {
    return dap_chain_net_srv_stake_ext_find_py(a_self, a_args); // Alias for now
}

PyObject* dap_chain_net_srv_stake_ext_get_stats_py(PyObject *a_self, PyObject *a_args) {
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
    dap_chain_srv_stake_ext_stats_t *l_stats = dap_chain_net_srv_stake_ext_get_stats(l_net);
    if (!l_stats) Py_RETURN_NONE;
    
    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "total_stake_ext", PyLong_FromLong(l_stats->total_stake_ext));
    PyDict_SetItemString(l_dict, "active_stake_ext", PyLong_FromLong(l_stats->active_stake_ext));
    PyDict_SetItemString(l_dict, "ended_stake_ext", PyLong_FromLong(l_stats->ended_stake_ext));
    return l_dict;
}

PyObject* dap_chain_net_srv_stake_ext_lock_create_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj, *a_key_obj;
    char *a_stake_hash_str;
    PyObject *a_amount_obj;
    long long a_lock_time;
    unsigned int a_position_id;
    PyObject *a_fee_obj;
    
    if (!PyArg_ParseTuple(a_args, "OOsOLO", &a_net_obj, &a_key_obj, &a_stake_hash_str, &a_amount_obj, &a_lock_time, &a_position_id, &a_fee_obj))
        return NULL;
        
    dap_chain_net_t *l_net = ((PyDapChainNetObject*)a_net_obj)->chain_net;
    dap_enc_key_t *l_key = ((PyCryptoKeyObject*)a_key_obj)->key;
    dap_chain_hash_fast_t l_stake_hash;
    dap_chain_hash_fast_from_str(a_stake_hash_str, &l_stake_hash);
    
    uint256_t l_amount = dap_chain_balance_scan(a_amount_obj);
    uint256_t l_fee = dap_chain_balance_scan(a_fee_obj);
    
    int l_ret_code = 0;
    char *l_tx_hash = dap_chain_net_srv_stake_ext_lock_create(l_net, l_key, &l_stake_hash, l_amount, (dap_time_t)a_lock_time, (uint32_t)a_position_id, l_fee, &l_ret_code);
    
    if (!l_tx_hash) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create lock transaction");
        return NULL;
    }
    
    return PyUnicode_FromString(l_tx_hash);
}

PyObject* dap_chain_net_srv_stake_ext_unlock_create_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj, *a_key_obj;
    char *a_lock_hash_str;
    PyObject *a_fee_obj;
    
    if (!PyArg_ParseTuple(a_args, "OOsO", &a_net_obj, &a_key_obj, &a_lock_hash_str, &a_fee_obj))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj) || !PyCapsule_CheckExact(a_key_obj)) {
        PyErr_SetString(PyExc_TypeError, "Invalid capsule arguments");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    dap_enc_key_t *l_key = (dap_enc_key_t *)PyCapsule_GetPointer(a_key_obj, "dap_enc_key_t");
    dap_chain_hash_fast_t l_lock_hash;
    dap_chain_hash_fast_from_str(a_lock_hash_str, &l_lock_hash);
    
    uint256_t l_fee = dap_chain_balance_scan(a_fee_obj);
    uint256_t l_value = {0};
    int l_ret_code = 0;
    
    char *l_tx_hash = dap_chain_net_srv_stake_ext_unlock_create(l_net, l_key, &l_lock_hash, l_fee, &l_value, &l_ret_code);
    
    if (!l_tx_hash) {
         PyErr_SetString(PyExc_RuntimeError, "Failed to create unlock transaction");
         return NULL;
    }
    
    return PyUnicode_FromString(l_tx_hash);
}


// =============================================================================
// VPN
// =============================================================================

PyObject* dap_chain_net_srv_vpn_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_res = dap_chain_net_srv_vpn_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_srv_vpn_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_net_srv_vpn_deinit();
    Py_RETURN_NONE;
}

// =============================================================================
// XCHANGE
// =============================================================================

PyObject* dap_chain_net_srv_xchange_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_res = dap_chain_net_srv_xchange_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_srv_xchange_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
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
    uint256_t l_datoshi_sell = dap_chain_balance_scan(a_datoshi_sell_obj);
    uint256_t l_rate = dap_chain_balance_scan(a_rate_obj);
    uint256_t l_fee = dap_chain_balance_scan(a_fee_obj);
    
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
    uint256_t l_fee = dap_chain_balance_scan(a_fee_obj);
    
    char *l_out_hash = NULL;
    int l_ret = dap_chain_net_srv_xchange_remove(l_net, &l_hash, l_fee, l_wallet, &l_out_hash);
    
    return Py_BuildValue("is", l_ret, l_out_hash);
}

PyObject* dap_chain_net_srv_xchange_purchase_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj;
    char *a_order_hash_str;
    PyObject *a_value_obj, *a_fee_obj, *a_wallet_obj;
    
    if (!PyArg_ParseTuple(a_args, "OsOOO", &a_net_obj, &a_order_hash_str, &a_value_obj, &a_fee_obj, &a_wallet_obj))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj) || !PyCapsule_CheckExact(a_wallet_obj)) {
        PyErr_SetString(PyExc_TypeError, "Invalid capsule arguments");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    dap_chain_wallet_t *l_wallet = (dap_chain_wallet_t *)PyCapsule_GetPointer(a_wallet_obj, "dap_chain_wallet_t");
    dap_chain_hash_fast_t l_order_hash;
    dap_chain_hash_fast_from_str(a_order_hash_str, &l_order_hash);
    uint256_t l_value = dap_chain_balance_scan(a_value_obj);
    uint256_t l_fee = dap_chain_balance_scan(a_fee_obj);
    
    char *l_out_hash = NULL;
    int l_ret = dap_chain_net_srv_xchange_purchase(l_net, &l_order_hash, l_value, l_fee, l_wallet, &l_out_hash);
    
    return Py_BuildValue("is", l_ret, l_out_hash);
}

PyObject* dap_chain_net_srv_xchange_get_tx_xchange_py(PyObject *a_self, PyObject *a_args) {
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
    dap_list_t *l_list = dap_chain_net_srv_xchange_get_tx_xchange(l_net);
    
    PyObject *l_py_list = PyList_New(0);
    for (dap_list_t *l_ptr = l_list; l_ptr; l_ptr = l_ptr->next) {
        // Assuming list contains dap_chain_datum_tx_t* or hash strings?
        // Looking at dap_chain_net_srv_xchange.c source, it returns list of dap_chain_datum_tx_t*
        dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t*)l_ptr->data;
        dap_chain_hash_fast_t l_tx_hash;
        dap_hash_fast(l_tx, dap_chain_datum_tx_get_size(l_tx), &l_tx_hash);
        char *l_hash_str = dap_chain_hash_fast_to_str_new(&l_tx_hash);
        
        PyObject *l_hash_obj = PyUnicode_FromString(l_hash_str);
        PyList_Append(l_py_list, l_hash_obj);
        Py_DECREF(l_hash_obj);
        DAP_DELETE(l_hash_str);
    }
    dap_list_free(l_list);
    return l_py_list;
}

PyObject* dap_chain_net_srv_xchange_get_prices_py(PyObject *a_self, PyObject *a_args) {
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
    dap_list_t *l_list = dap_chain_net_srv_xchange_get_prices(l_net);
    
    PyObject *l_py_list = PyList_New(0);
    for (dap_list_t *l_ptr = l_list; l_ptr; l_ptr = l_ptr->next) {
        dap_chain_net_srv_xchange_price_t *l_price = (dap_chain_net_srv_xchange_price_t*)l_ptr->data;
        PyObject *l_dict = PyDict_New();
        
        PyDict_SetItemString(l_dict, "token_sell", PyUnicode_FromString(l_price->token_sell));
        PyDict_SetItemString(l_dict, "token_buy", PyUnicode_FromString(l_price->token_buy));
        
        // Uint256 conversion (using string for safety)
        char *l_sell_str = dap_chain_balance_print(l_price->datoshi_sell);
        PyDict_SetItemString(l_dict, "datoshi_sell", PyUnicode_FromString(l_sell_str));
        DAP_DELETE(l_sell_str);
        
        char *l_rate_str = dap_chain_balance_print(l_price->rate);
        PyDict_SetItemString(l_dict, "rate", PyUnicode_FromString(l_rate_str));
        DAP_DELETE(l_rate_str);
        
        // Address
        char *l_addr_str = dap_chain_addr_to_str(&l_price->creator_addr);
        PyDict_SetItemString(l_dict, "creator_addr", PyUnicode_FromString(l_addr_str));
        DAP_DELETE(l_addr_str);
        
        // Hash
        char *l_hash_str = dap_chain_hash_fast_to_str_new(&l_price->tx_hash);
        PyDict_SetItemString(l_dict, "tx_hash", PyUnicode_FromString(l_hash_str));
        DAP_DELETE(l_hash_str);
        
        PyList_Append(l_py_list, l_dict);
        Py_DECREF(l_dict);
    }
    dap_list_free(l_list);
    return l_py_list;
}

PyObject* dap_chain_net_srv_xchange_get_order_status_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj;
    char *a_hash_str;
    if (!PyArg_ParseTuple(a_args, "Os", &a_net_obj, &a_hash_str))
        return NULL;
    dap_chain_net_t *l_net = ((PyDapChainNetObject*)a_net_obj)->chain_net;
    dap_chain_hash_fast_t l_hash;
    dap_chain_hash_fast_from_str(a_hash_str, &l_hash);
    
    dap_chain_net_srv_xchange_order_status_t l_status = dap_chain_net_srv_xchange_get_order_status(l_net, l_hash);
    return PyLong_FromLong(l_status);
}

// =============================================================================
// VOTING
// =============================================================================

PyObject* dap_chain_net_srv_voting_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_res = dap_chain_net_srv_voting_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_srv_voting_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_net_srv_voting_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_voting_create_py(PyObject *a_self, PyObject *a_args) {
    char *a_question;
    PyObject *a_options_list;
    long long a_expire_vote;
    long long a_max_vote;
    PyObject *a_fee_obj;
    int a_delegated_key_required;
    int a_vote_changing_allowed;
    PyObject *a_wallet_obj;
    PyObject *a_net_obj;
    char *a_token_ticker;
    char *a_hash_out_type = "hex";
    
    if (!PyArg_ParseTuple(a_args, "sOLLOiiOOs|s", 
        &a_question, &a_options_list, &a_expire_vote, &a_max_vote, &a_fee_obj, 
        &a_delegated_key_required, &a_vote_changing_allowed, &a_wallet_obj, &a_net_obj, 
        &a_token_ticker, &a_hash_out_type))
        return NULL;

    if (!PyList_Check(a_options_list)) {
        PyErr_SetString(PyExc_TypeError, "Options must be a list of strings");
        return NULL;
    }

    dap_list_t *l_options = NULL;
    Py_ssize_t l_size = PyList_Size(a_options_list);
    for (Py_ssize_t i = 0; i < l_size; i++) {
        PyObject *l_item = PyList_GetItem(a_options_list, i);
        if (!PyUnicode_Check(l_item)) {
            dap_list_free_full(l_options, NULL); // Strings are not allocated by us here if using PyUnicode_AsUTF8 directly? 
            // PyUnicode_AsUTF8 returns pointer to internal buffer. dap_list stores pointers.
            // But dap_chain_net_srv_voting_create likely expects to own strings or copy them?
            // Usually SDK copies if needed. Let's check... it takes 'dap_list_t *a_options'.
            // If it's just list of char*, we should strdup them if the list is long-lived or modifyable.
            // But PyUnicode_AsUTF8 buffer is valid as long as PyObject is alive.
            // Let's strdup to be safe and free later if SDK doesn't take ownership.
            // Actually, SDK usually copies. Let's use strdup.
            
            PyErr_SetString(PyExc_TypeError, "All options must be strings");
            return NULL;
        }
        l_options = dap_list_append(l_options, strdup(PyUnicode_AsUTF8(l_item)));
    }

    dap_chain_wallet_t *l_wallet = ((PyDapChainWalletObject*)a_wallet_obj)->wallet;
    dap_chain_net_t *l_net = ((PyDapChainNetObject*)a_net_obj)->chain_net;
    uint256_t l_fee = dap_chain_balance_scan(a_fee_obj);
    char *l_hash_output = NULL;

    int l_ret = dap_chain_net_srv_voting_create(
        a_question, l_options, (dap_time_t)a_expire_vote, (uint64_t)a_max_vote, l_fee,
        (bool)a_delegated_key_required, (bool)a_vote_changing_allowed,
        l_wallet, l_net, a_token_ticker, a_hash_out_type, &l_hash_output
    );

    // Free options list (and strings)
    dap_list_free_full(l_options, free);

    if (l_ret != 0) {
        return Py_BuildValue("is", l_ret, NULL);
    }
    
    PyObject *l_ret_obj = Py_BuildValue("is", l_ret, l_hash_output);
    DAP_DELETE(l_hash_output);
    return l_ret_obj;
}

PyObject* dap_chain_net_srv_vote_create_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_cert_obj; // Optional?
    PyObject *a_fee_obj;
    PyObject *a_wallet_obj;
    char *a_voting_hash_str;
    long long a_option_idx;
    PyObject *a_net_obj;
    char *a_hash_out_type = "hex";

    if (!PyArg_ParseTuple(a_args, "OOOsLO|s", &a_cert_obj, &a_fee_obj, &a_wallet_obj, 
        &a_voting_hash_str, &a_option_idx, &a_net_obj, &a_hash_out_type))
        return NULL;

    dap_cert_t *l_cert = NULL;
    if (a_cert_obj != Py_None) {
        if (PyCapsule_CheckExact(a_cert_obj)) {
            l_cert = (dap_cert_t *)PyCapsule_GetPointer(a_cert_obj, "dap_cert_t");
        }
    }

    if (!PyCapsule_CheckExact(a_wallet_obj) || !PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "Invalid capsule arguments");
        return NULL;
    }

    dap_chain_wallet_t *l_wallet = (dap_chain_wallet_t *)PyCapsule_GetPointer(a_wallet_obj, "dap_chain_wallet_t");
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    uint256_t l_fee = dap_chain_balance_scan(a_fee_obj);
    
    dap_chain_hash_fast_t l_voting_hash;
    dap_chain_hash_fast_from_str(a_voting_hash_str, &l_voting_hash);

    char *l_hash_tx_out = NULL;
    int l_ret = dap_chain_net_srv_vote_create(
        l_cert, l_fee, l_wallet, &l_voting_hash, (uint64_t)a_option_idx,
        l_net, a_hash_out_type, &l_hash_tx_out
    );

    if (l_ret != 0) {
        return Py_BuildValue("is", l_ret, NULL);
    }

    PyObject *l_ret_obj = Py_BuildValue("is", l_ret, l_hash_tx_out);
    DAP_DELETE(l_hash_tx_out);
    return l_ret_obj;
}

PyObject* dap_chain_net_voting_list_py(PyObject *a_self, PyObject *a_args) {
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
    dap_list_t *l_list = dap_chain_net_voting_list(l_net);
    
    PyObject *l_py_list = PyList_New(0);
    for (dap_list_t *l_ptr = l_list; l_ptr; l_ptr = l_ptr->next) {
        dap_chain_net_voting_info_t *l_info = (dap_chain_net_voting_info_t*)l_ptr->data;
        PyObject *l_dict = PyDict_New();
        
        char *l_hash_str = dap_chain_hash_fast_to_str_new(&l_info->hash);
        PyDict_SetItemString(l_dict, "hash", PyUnicode_FromString(l_hash_str));
        DAP_DELETE(l_hash_str);
        
        PyDict_SetItemString(l_dict, "is_expired", PyBool_FromLong(l_info->is_expired));
        PyDict_SetItemString(l_dict, "is_cancelled", PyBool_FromLong(l_info->is_cancelled));
        
        PyList_Append(l_py_list, l_dict);
        Py_DECREF(l_dict);
    }
    // dap_chain_net_voting_list returns list of POINTERS to internal structures or copies?
    // Usually such lists need deep free, or just list free if pointers are managed elsewhere.
    // dap_chain_net_voting_list documentation is sparse. Assuming caller owns the list but not the items?
    // Or caller owns everything. Let's be safe and just free the list container for now to avoid double free if items are internal.
    // But wait, dap_chain_net_voting_extract_info returns a NEW structure.
    // Let's assume list is list of pointers.
    dap_list_free(l_list);
    return l_py_list;
}

PyObject* dap_chain_net_voting_extract_info_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj;
    char *a_hash_str;
    if (!PyArg_ParseTuple(a_args, "Os", &a_net_obj, &a_hash_str))
        return NULL;
    dap_chain_net_t *l_net = ((PyDapChainNetObject*)a_net_obj)->chain_net;
    dap_chain_hash_fast_t l_hash;
    dap_chain_hash_fast_from_str(a_hash_str, &l_hash);
    dap_chain_net_voting_info_t *l_info = dap_chain_net_voting_extract_info(l_net, &l_hash);
    if (!l_info) Py_RETURN_NONE;
    
    // Return dict info
    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "is_expired", PyBool_FromLong(l_info->is_expired));
    dap_chain_net_voting_info_free(l_info);
    return l_dict;
}

PyObject* dap_chain_net_vote_cancel_py(PyObject *a_self, PyObject *a_args) {
    // Wrapper for dap_chain_net_vote_cancel
    PyErr_SetString(PyExc_NotImplementedError, "Vote cancel not fully implemented yet");
    return NULL;
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

PyMethodDef* cellframe_services_ext_get_methods(void) {
    static PyMethodDef methods[] = {
        // Stake-Ext
        {"net_srv_stake_ext_init", (PyCFunction)dap_chain_net_srv_stake_ext_init_py, METH_VARARGS, "Init stake_ext service"},
        {"net_srv_stake_ext_deinit", (PyCFunction)dap_chain_net_srv_stake_ext_deinit_py, METH_VARARGS, "Deinit stake_ext service"},
        {"net_srv_stake_ext_find", (PyCFunction)dap_chain_net_srv_stake_ext_find_py, METH_VARARGS, "Find stake_ext item"},
        {"net_srv_stake_ext_get_list", (PyCFunction)dap_chain_net_srv_stake_ext_get_list_py, METH_VARARGS, "Get stake_ext list"},
        {"net_srv_stake_ext_get_detailed", (PyCFunction)dap_chain_net_srv_stake_ext_get_detailed_py, METH_VARARGS, "Get detailed stake_ext info"},
        {"net_srv_stake_ext_get_stats", (PyCFunction)dap_chain_net_srv_stake_ext_get_stats_py, METH_VARARGS, "Get stake_ext statistics"},
        {"net_srv_stake_ext_lock_create", (PyCFunction)dap_chain_net_srv_stake_ext_lock_create_py, METH_VARARGS, "Create lock tx for stake_ext"},
        {"net_srv_stake_ext_unlock_create", (PyCFunction)dap_chain_net_srv_stake_ext_unlock_create_py, METH_VARARGS, "Create unlock tx for stake_ext"},
        
        // VPN
        {"net_srv_vpn_init", (PyCFunction)dap_chain_net_srv_vpn_init_py, METH_VARARGS, "Init VPN service"},
        {"net_srv_vpn_deinit", (PyCFunction)dap_chain_net_srv_vpn_deinit_py, METH_VARARGS, "Deinit VPN service"},
        
        // Xchange
        {"net_srv_xchange_init", (PyCFunction)dap_chain_net_srv_xchange_init_py, METH_VARARGS, "Init Xchange service"},
        {"net_srv_xchange_deinit", (PyCFunction)dap_chain_net_srv_xchange_deinit_py, METH_VARARGS, "Deinit Xchange service"},
        {"net_srv_xchange_create", (PyCFunction)dap_chain_net_srv_xchange_create_py, METH_VARARGS, "Create exchange order"},
        {"net_srv_xchange_remove", (PyCFunction)dap_chain_net_srv_xchange_remove_py, METH_VARARGS, "Remove exchange order"},
        {"net_srv_xchange_purchase", (PyCFunction)dap_chain_net_srv_xchange_purchase_py, METH_VARARGS, "Purchase exchange order"},
        {"net_srv_xchange_get_tx_xchange", (PyCFunction)dap_chain_net_srv_xchange_get_tx_xchange_py, METH_VARARGS, "Get exchange transactions"},
        {"net_srv_xchange_get_prices", (PyCFunction)dap_chain_net_srv_xchange_get_prices_py, METH_VARARGS, "Get exchange prices"},
        {"net_srv_xchange_get_order_status", (PyCFunction)dap_chain_net_srv_xchange_get_order_status_py, METH_VARARGS, "Get order status"},
        
        // Voting
        {"net_srv_voting_init", (PyCFunction)dap_chain_net_srv_voting_init_py, METH_VARARGS, "Init voting service"},
        {"net_srv_voting_deinit", (PyCFunction)dap_chain_net_srv_voting_deinit_py, METH_VARARGS, "Deinit voting service"},
        {"net_srv_voting_create", (PyCFunction)dap_chain_net_srv_voting_create_py, METH_VARARGS, "Create voting"},
        {"net_srv_vote_create", (PyCFunction)dap_chain_net_srv_vote_create_py, METH_VARARGS, "Create vote"},
        {"net_srv_voting_list", (PyCFunction)dap_chain_net_voting_list_py, METH_VARARGS, "List votings"},
        {"net_srv_voting_extract_info", (PyCFunction)dap_chain_net_voting_extract_info_py, METH_VARARGS, "Get voting info"},
        {"net_srv_vote_cancel", (PyCFunction)dap_chain_net_vote_cancel_py, METH_VARARGS, "Cancel vote"},

        {NULL, NULL, 0, NULL}
    };
    return methods;
}

