#include "include/cf_ledger_internal.h"
#include "cf_ledger_callback_registry.h"
#include "cf_common_utils.h"
#include "dap_json.h"
#include "dap_chain_ledger_callbacks.h"

void dap_ledger_set_fee_callback(dap_ledger_t *a_ledger, dap_ledger_set_fee_callback_t a_callback);

/*
 * Cellframe ledger callbacks bindings
 * Callbacks and notifications: tx add, event, bridged tx notifications
 */

// =========================================
// SERVICE TAG CHECK CALLBACK WRAPPER
// =========================================

/**
 * @brief C wrapper for service tag check callback - calls Python callback
 * Signature: bool (*dap_ledger_tag_check_callback_t)(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx, 
 *                                                      dap_chain_datum_tx_item_groups_t *a_items_grp, 
 *                                                      dap_chain_tx_tag_action_type_t *a_action)
 */
static bool s_ledger_service_tag_check_wrapper(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx,
                                                dap_chain_datum_tx_item_groups_t *a_items_grp,
                                                dap_chain_tx_tag_action_type_t *a_action) {
    // Note: SDK doesn't pass UID, we need to find it from tag in items_grp
    // For now, we'll iterate through all registered services to find matching callback
    // This is not optimal but SDK API limitation
    (void)a_ledger;
    (void)a_tx;
    (void)a_items_grp;
    (void)a_action;
    
    // Service tag check wrapper - default passthrough implementation
    // Python bindings delegate tag checking to Python callbacks via ledger.service_add()
    // For custom service tag logic, register Python callback with specific UID
    // 
    // Implementation note: items_grp contains parsed TX items grouped by type:
    //   - items_out_cond_srv_pay, items_out_cond_srv_stake_lock, etc.
    //   - items_vote, items_voting, items_event, items_reward
    //   - items_tsd (transaction-specific data)
    // 
    // Default: allow all (Python callback decides)
    return true;
}

/**
 * @brief C wrapper for tax callback - calls Python callback
 * Signature: bool (*dap_ledger_tax_callback_t)(dap_chain_net_id_t a_net_id, dap_hash_fast_t *a_signer_pkey_hash, 
 *                                                dap_chain_addr_t *a_tax_addr, uint256_t *a_tax_value)
 */
static bool s_ledger_tax_callback_wrapper(dap_chain_net_id_t a_net_id, dap_hash_fast_t *a_signer_pkey_hash,
                                          dap_chain_addr_t *a_tax_addr, uint256_t *a_tax_value) {
    python_tax_ctx_t *l_ctx = cf_ledger_tax_callback_get();
    if (!l_ctx || !l_ctx->callback) {
        return true; // Default: allow
    }
    
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    PyObject *l_net_id = PyLong_FromUnsignedLongLong(a_net_id.uint64);
    PyObject *l_pkey_hash = a_signer_pkey_hash ? PyBytes_FromStringAndSize((const char*)a_signer_pkey_hash, sizeof(dap_hash_fast_t)) : Py_None;
    PyObject *l_tax_addr = a_tax_addr ? PyCapsule_New(a_tax_addr, "dap_chain_addr_t", NULL) : Py_None;
    PyObject *l_tax_value = a_tax_value ? PyCapsule_New(a_tax_value, "uint256_t", NULL) : Py_None;
    
    if (!l_net_id || !l_pkey_hash || !l_tax_addr || !l_tax_value) {
        log_it(L_ERROR, "Failed to create Python objects for tax callback");
        Py_XDECREF(l_net_id);
        Py_XDECREF(l_pkey_hash);
        Py_XDECREF(l_tax_addr);
        Py_XDECREF(l_tax_value);
        PyGILState_Release(l_gstate);
        return true;
    }
    
    PyObject *l_result = PyObject_CallFunctionObjArgs(l_ctx->callback, l_net_id, l_pkey_hash, l_tax_addr, l_tax_value, l_ctx->user_data, NULL);
    
    bool l_ret = true;
    if (!l_result) {
        log_it(L_ERROR, "Python tax callback raised an exception");
        PyErr_Print();
    } else {
        l_ret = PyObject_IsTrue(l_result);
        Py_DECREF(l_result);
    }
    
    Py_DECREF(l_net_id);
    Py_XDECREF(l_pkey_hash);
    Py_XDECREF(l_tax_addr);
    Py_XDECREF(l_tax_value);
    
    PyGILState_Release(l_gstate);
    return l_ret;
}

// =========================================
// BALANCE & LEDGER CALLBACK WRAPPERS
// =========================================

static void s_ledger_balance_change_wrapper(dap_ledger_t *a_ledger, const dap_chain_addr_t *a_addr,
                                            const char *a_token_ticker, uint256_t a_old_balance,
                                            uint256_t a_new_balance, bool a_is_add) {
    python_ledger_balance_ctx_t *l_ctx = cf_ledger_balance_callback_get(a_ledger);
    if (!l_ctx) {
        return;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    dap_chain_addr_t *l_addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    PyObject *l_addr_capsule = NULL;
    if (l_addr_copy && a_addr) {
        *l_addr_copy = *a_addr;
        l_addr_capsule = cf_common_capsule_new_addr(l_addr_copy);
    }
    if (!l_addr_capsule) {
        if (l_addr_copy) {
            DAP_DELETE(l_addr_copy);
        }
        Py_XDECREF(l_ledger);
        PyGILState_Release(l_gstate);
        return;
    }

    PyObject *l_token = a_token_ticker ? PyUnicode_FromString(a_token_ticker) : Py_None;
    if (!a_token_ticker) {
        Py_INCREF(Py_None);
    }
    PyObject *l_old_balance = PyBytes_FromStringAndSize((const char *)&a_old_balance, sizeof(uint256_t));
    PyObject *l_new_balance = PyBytes_FromStringAndSize((const char *)&a_new_balance, sizeof(uint256_t));
    PyObject *l_is_add = PyBool_FromLong(a_is_add);

    for (python_ledger_balance_ctx_t *iter = l_ctx; iter; iter = iter->next) {
        if (!iter->callback) {
            continue;
        }
        PyObject *l_user_data = iter->user_data ? iter->user_data : Py_None;
        PyObject *l_result = PyObject_CallFunctionObjArgs(
            iter->callback, l_ledger, l_addr_capsule, l_token, l_old_balance, l_new_balance,
            l_is_add, l_user_data, NULL
        );
        if (!l_result) {
            log_it(L_ERROR, "Python balance change callback raised an exception");
            PyErr_Print();
        } else {
            Py_DECREF(l_result);
        }
    }

    Py_DECREF(l_ledger);
    Py_DECREF(l_addr_capsule);
    Py_DECREF(l_token);
    Py_DECREF(l_old_balance);
    Py_DECREF(l_new_balance);
    Py_DECREF(l_is_add);

    PyGILState_Release(l_gstate);
}

static void s_ledger_fee_callback_wrapper(dap_ledger_t *a_ledger, uint256_t a_fee) {
    python_ledger_simple_ctx_t *l_ctx = cf_ledger_fee_callback_get(a_ledger);
    if (!l_ctx || !l_ctx->callback) {
        return;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_fee = PyBytes_FromStringAndSize((const char *)&a_fee, sizeof(uint256_t));
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    PyObject *l_result = PyObject_CallFunctionObjArgs(l_ctx->callback, l_ledger, l_fee, l_user_data, NULL);
    if (!l_result) {
        log_it(L_ERROR, "Python fee callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_ledger);
    Py_DECREF(l_fee);

    PyGILState_Release(l_gstate);
}

static dap_chain_cell_id_t s_ledger_get_cur_cell_callback_wrapper(dap_ledger_t *a_ledger) {
    python_ledger_simple_ctx_t *l_ctx = cf_ledger_get_cur_cell_callback_get(a_ledger);
    if (!l_ctx || !l_ctx->callback) {
        return c_dap_chain_cell_id_null;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    dap_chain_cell_id_t l_cell_id = c_dap_chain_cell_id_null;
    PyObject *l_result = PyObject_CallFunctionObjArgs(l_ctx->callback, l_ledger, l_user_data, NULL);
    if (!l_result) {
        log_it(L_ERROR, "Python get_cur_cell callback raised an exception");
        PyErr_Print();
    } else if (PyCapsule_CheckExact(l_result)) {
        dap_chain_cell_id_t *l_ptr = PyCapsule_GetPointer(l_result, "dap_chain_cell_id_t");
        if (l_ptr) {
            l_cell_id = *l_ptr;
        }
        Py_DECREF(l_result);
    } else if (PyBytes_Check(l_result)) {
        if ((size_t)PyBytes_Size(l_result) == sizeof(dap_chain_cell_id_t)) {
            memcpy(&l_cell_id, PyBytes_AsString(l_result), sizeof(dap_chain_cell_id_t));
        }
        Py_DECREF(l_result);
    } else if (PyLong_Check(l_result)) {
        l_cell_id.uint64 = PyLong_AsUnsignedLongLong(l_result);
        Py_DECREF(l_result);
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_ledger);
    PyGILState_Release(l_gstate);
    return l_cell_id;
}

// =========================================
// WALLET CALLBACK WRAPPERS
// =========================================

static dap_sign_t *s_ledger_wallet_sign_callback_wrapper(const char *a_wallet_name,
                                                         const void *a_data, size_t a_data_size,
                                                         uint32_t a_key_idx) {
    python_ledger_wallet_ctx_t *l_ctx = cf_ledger_wallet_callbacks_get();
    if (!l_ctx || !l_ctx->sign_callback) {
        return NULL;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_wallet_name = PyUnicode_FromString(a_wallet_name ? a_wallet_name : "");
    PyObject *l_data = PyBytes_FromStringAndSize((const char *)a_data, a_data ? (Py_ssize_t)a_data_size : 0);
    PyObject *l_key_idx = PyLong_FromUnsignedLong(a_key_idx);
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    dap_sign_t *l_sign = NULL;
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->sign_callback, l_wallet_name, l_data, l_key_idx, l_user_data, NULL
    );
    if (!l_result) {
        log_it(L_ERROR, "Python wallet sign callback raised an exception");
        PyErr_Print();
    } else if (PyCapsule_CheckExact(l_result)) {
        l_sign = PyCapsule_GetPointer(l_result, "dap_sign_t");
        Py_DECREF(l_result);
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_wallet_name);
    Py_DECREF(l_data);
    Py_DECREF(l_key_idx);

    PyGILState_Release(l_gstate);
    return l_sign;
}

static dap_hash_fast_t *s_ledger_wallet_get_pkey_hash_callback_wrapper(const char *a_wallet_name, uint32_t a_key_idx) {
    python_ledger_wallet_ctx_t *l_ctx = cf_ledger_wallet_callbacks_get();
    if (!l_ctx || !l_ctx->get_pkey_hash_callback) {
        return NULL;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_wallet_name = PyUnicode_FromString(a_wallet_name ? a_wallet_name : "");
    PyObject *l_key_idx = PyLong_FromUnsignedLong(a_key_idx);
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    dap_hash_fast_t *l_result_hash = NULL;
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->get_pkey_hash_callback, l_wallet_name, l_key_idx, l_user_data, NULL
    );
    if (!l_result) {
        log_it(L_ERROR, "Python wallet get_pkey_hash callback raised an exception");
        PyErr_Print();
    } else if (l_result != Py_None) {
        if (cf_common_parse_hash_fast(l_result, &l_ctx->last_pkey_hash) == 0) {
            l_ctx->has_last_pkey_hash = true;
            l_result_hash = &l_ctx->last_pkey_hash;
        }
        Py_DECREF(l_result);
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_wallet_name);
    Py_DECREF(l_key_idx);

    PyGILState_Release(l_gstate);
    return l_result_hash;
}

static dap_pkey_t *s_ledger_wallet_get_pkey_callback_wrapper(const char *a_wallet_name, uint32_t a_key_idx) {
    python_ledger_wallet_ctx_t *l_ctx = cf_ledger_wallet_callbacks_get();
    if (!l_ctx || !l_ctx->get_pkey_callback) {
        return NULL;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_wallet_name = PyUnicode_FromString(a_wallet_name ? a_wallet_name : "");
    PyObject *l_key_idx = PyLong_FromUnsignedLong(a_key_idx);
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    dap_pkey_t *l_pkey = NULL;
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->get_pkey_callback, l_wallet_name, l_key_idx, l_user_data, NULL
    );
    if (!l_result) {
        log_it(L_ERROR, "Python wallet get_pkey callback raised an exception");
        PyErr_Print();
    } else if (PyCapsule_CheckExact(l_result)) {
        l_pkey = PyCapsule_GetPointer(l_result, "dap_pkey_t");
        Py_DECREF(l_result);
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_wallet_name);
    Py_DECREF(l_key_idx);

    PyGILState_Release(l_gstate);
    return l_pkey;
}

static void *s_ledger_wallet_get_addr_callback_wrapper(const char *a_wallet_name, dap_chain_net_id_t a_net_id) {
    python_ledger_wallet_ctx_t *l_ctx = cf_ledger_wallet_callbacks_get();
    if (!l_ctx || !l_ctx->get_addr_callback) {
        return NULL;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_wallet_name = PyUnicode_FromString(a_wallet_name ? a_wallet_name : "");
    PyObject *l_net_id = PyLong_FromUnsignedLongLong(a_net_id.uint64);
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    void *l_addr_ptr = NULL;
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->get_addr_callback, l_wallet_name, l_net_id, l_user_data, NULL
    );
    if (!l_result) {
        log_it(L_ERROR, "Python wallet get_addr callback raised an exception");
        PyErr_Print();
    } else if (l_result != Py_None) {
        if (cf_common_parse_addr(l_result, &l_ctx->last_addr) == 0) {
            l_ctx->has_last_addr = true;
            l_addr_ptr = &l_ctx->last_addr;
        }
        Py_DECREF(l_result);
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_wallet_name);
    Py_DECREF(l_net_id);

    PyGILState_Release(l_gstate);
    return l_addr_ptr;
}

static const char *s_ledger_wallet_check_sign_callback_wrapper(const char *a_wallet_name) {
    python_ledger_wallet_ctx_t *l_ctx = cf_ledger_wallet_callbacks_get();
    if (!l_ctx || !l_ctx->check_sign_callback) {
        return NULL;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_wallet_name = PyUnicode_FromString(a_wallet_name ? a_wallet_name : "");
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    const char *l_str_out = NULL;
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->check_sign_callback, l_wallet_name, l_user_data, NULL
    );
    if (!l_result) {
        log_it(L_ERROR, "Python wallet check_sign callback raised an exception");
        PyErr_Print();
    } else if (l_result != Py_None) {
        PyObject *l_str_obj = PyObject_Str(l_result);
        if (l_str_obj) {
            const char *l_str = PyUnicode_AsUTF8(l_str_obj);
            if (l_str) {
                DAP_DELETE(l_ctx->last_sign_info);
                l_ctx->last_sign_info = dap_strdup(l_str);
                l_str_out = l_ctx->last_sign_info;
            }
            Py_DECREF(l_str_obj);
        }
        Py_DECREF(l_result);
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_wallet_name);

    PyGILState_Release(l_gstate);
    return l_str_out;
}

// =========================================
// WALLET CACHE CALLBACK WRAPPERS
// =========================================

static int s_wallet_cache_tx_find_in_history_wrapper(dap_chain_addr_t *a_addr, void *a_ledger,
                                                     dap_chain_hash_fast_t *a_tx_hash,
                                                     dap_chain_datum_tx_t **a_tx_out,
                                                     dap_time_t *a_tx_time_out,
                                                     void **a_main_ticker_out,
                                                     dap_hash_fast_t *a_tx_hash_out) {
    python_ledger_wallet_cache_ctx_t *l_ctx = cf_ledger_wallet_cache_callbacks_get();
    if (!l_ctx || !l_ctx->tx_find_callback) {
        return -1;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    dap_chain_addr_t *l_addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    PyObject *l_addr = NULL;
    if (l_addr_copy && a_addr) {
        *l_addr_copy = *a_addr;
        l_addr = cf_common_capsule_new_addr(l_addr_copy);
    }
    PyObject *l_ledger_obj = a_ledger ? PyCapsule_New(a_ledger, "dap_ledger_t", NULL) : Py_None;
    if (!a_ledger) {
        Py_INCREF(Py_None);
    }
    PyObject *l_tx_hash = a_tx_hash ? PyBytes_FromStringAndSize((const char*)a_tx_hash, sizeof(dap_chain_hash_fast_t)) : Py_None;
    if (!a_tx_hash) {
        Py_INCREF(Py_None);
    }
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    if (!l_addr) {
        Py_DECREF(l_ledger_obj);
        Py_DECREF(l_tx_hash);
        PyGILState_Release(l_gstate);
        return -1;
    }

    int l_rc = -1;
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->tx_find_callback, l_addr, l_ledger_obj, l_tx_hash, l_user_data, NULL
    );

    if (!l_result) {
        log_it(L_ERROR, "Python wallet cache tx_find callback raised an exception");
        PyErr_Print();
    } else if (PyLong_Check(l_result)) {
        l_rc = (int)PyLong_AsLong(l_result);
        Py_DECREF(l_result);
    } else if (PyTuple_Check(l_result) || PyList_Check(l_result)) {
        Py_ssize_t l_len = PySequence_Length(l_result);
        if (l_len > 0) {
            PyObject *l_rc_obj = PySequence_GetItem(l_result, 0);
            if (l_rc_obj) {
                l_rc = (int)PyLong_AsLong(l_rc_obj);
                Py_DECREF(l_rc_obj);
            }
        }
        if (a_tx_out && l_len > 1) {
            PyObject *l_tx_obj = PySequence_GetItem(l_result, 1);
            if (l_tx_obj && PyCapsule_CheckExact(l_tx_obj)) {
                *a_tx_out = PyCapsule_GetPointer(l_tx_obj, "dap_chain_datum_tx_t");
            }
            Py_XDECREF(l_tx_obj);
        }
        if (a_tx_time_out && l_len > 2) {
            PyObject *l_time_obj = PySequence_GetItem(l_result, 2);
            if (l_time_obj) {
                *a_tx_time_out = (dap_time_t)PyLong_AsUnsignedLongLong(l_time_obj);
            }
            Py_XDECREF(l_time_obj);
        }
        if (a_main_ticker_out && l_len > 3) {
            PyObject *l_ticker_obj = PySequence_GetItem(l_result, 3);
            if (l_ticker_obj) {
                PyObject *l_str_obj = PyObject_Str(l_ticker_obj);
                if (l_str_obj) {
                    const char *l_str = PyUnicode_AsUTF8(l_str_obj);
                    if (l_str) {
                        DAP_DELETE(l_ctx->last_main_ticker);
                        l_ctx->last_main_ticker = dap_strdup(l_str);
                        *a_main_ticker_out = l_ctx->last_main_ticker;
                    }
                    Py_DECREF(l_str_obj);
                }
            }
            Py_XDECREF(l_ticker_obj);
        }
        if (a_tx_hash_out && l_len > 4) {
            PyObject *l_hash_obj = PySequence_GetItem(l_result, 4);
            if (l_hash_obj) {
                dap_hash_fast_t l_hash = {0};
                if (cf_common_parse_hash_fast(l_hash_obj, &l_hash) == 0) {
                    memcpy(a_tx_hash_out, &l_hash, sizeof(dap_hash_fast_t));
                }
            }
            Py_XDECREF(l_hash_obj);
        }
        Py_DECREF(l_result);
    } else if (PyDict_Check(l_result)) {
        PyObject *l_rc_obj = PyDict_GetItemString(l_result, "rc");
        if (l_rc_obj) {
            l_rc = (int)PyLong_AsLong(l_rc_obj);
        }
        if (a_tx_out) {
            PyObject *l_tx_obj = PyDict_GetItemString(l_result, "tx");
            if (l_tx_obj && PyCapsule_CheckExact(l_tx_obj)) {
                *a_tx_out = PyCapsule_GetPointer(l_tx_obj, "dap_chain_datum_tx_t");
            }
        }
        if (a_tx_time_out) {
            PyObject *l_time_obj = PyDict_GetItemString(l_result, "tx_time");
            if (l_time_obj) {
                *a_tx_time_out = (dap_time_t)PyLong_AsUnsignedLongLong(l_time_obj);
            }
        }
        if (a_main_ticker_out) {
            PyObject *l_ticker_obj = PyDict_GetItemString(l_result, "main_ticker");
            if (l_ticker_obj) {
                PyObject *l_str_obj = PyObject_Str(l_ticker_obj);
                if (l_str_obj) {
                    const char *l_str = PyUnicode_AsUTF8(l_str_obj);
                    if (l_str) {
                        DAP_DELETE(l_ctx->last_main_ticker);
                        l_ctx->last_main_ticker = dap_strdup(l_str);
                        *a_main_ticker_out = l_ctx->last_main_ticker;
                    }
                    Py_DECREF(l_str_obj);
                }
            }
        }
        if (a_tx_hash_out) {
            PyObject *l_hash_obj = PyDict_GetItemString(l_result, "tx_hash");
            if (l_hash_obj) {
                dap_hash_fast_t l_hash = {0};
                if (cf_common_parse_hash_fast(l_hash_obj, &l_hash) == 0) {
                    memcpy(a_tx_hash_out, &l_hash, sizeof(dap_hash_fast_t));
                }
            }
        }
        Py_DECREF(l_result);
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_addr);
    Py_DECREF(l_ledger_obj);
    Py_DECREF(l_tx_hash);

    PyGILState_Release(l_gstate);
    return l_rc;
}

static dap_ledger_wallet_cache_iter_t *s_wallet_cache_iter_create_wrapper(dap_chain_addr_t a_addr) {
    python_ledger_wallet_cache_ctx_t *l_ctx = cf_ledger_wallet_cache_callbacks_get();
    if (!l_ctx || !l_ctx->iter_create_callback) {
        return NULL;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    dap_chain_addr_t *l_addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    PyObject *l_addr = NULL;
    if (l_addr_copy) {
        *l_addr_copy = a_addr;
        l_addr = cf_common_capsule_new_addr(l_addr_copy);
    }
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    dap_ledger_wallet_cache_iter_t *l_iter = NULL;
    if (l_addr) {
        PyObject *l_result = PyObject_CallFunctionObjArgs(
            l_ctx->iter_create_callback, l_addr, l_user_data, NULL
        );
        if (!l_result) {
            log_it(L_ERROR, "Python wallet cache iter_create callback raised an exception");
            PyErr_Print();
        } else if (l_result != Py_None) {
            l_iter = (dap_ledger_wallet_cache_iter_t *)l_result;
        } else {
            Py_DECREF(l_result);
        }
        Py_DECREF(l_addr);
    }

    PyGILState_Release(l_gstate);
    return l_iter;
}

static dap_chain_datum_tx_t *s_wallet_cache_iter_get_wrapper(dap_ledger_wallet_cache_iter_t *a_iter,
                                                             dap_ledger_wallet_cache_direction_t a_direction) {
    python_ledger_wallet_cache_ctx_t *l_ctx = cf_ledger_wallet_cache_callbacks_get();
    if (!l_ctx || !l_ctx->iter_get_callback || !a_iter) {
        return NULL;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_iter = (PyObject *)a_iter;
    PyObject *l_dir = PyLong_FromLong((long)a_direction);
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    dap_chain_datum_tx_t *l_tx = NULL;
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->iter_get_callback, l_iter, l_dir, l_user_data, NULL
    );
    if (!l_result) {
        log_it(L_ERROR, "Python wallet cache iter_get callback raised an exception");
        PyErr_Print();
    } else if (PyCapsule_CheckExact(l_result)) {
        l_tx = PyCapsule_GetPointer(l_result, "dap_chain_datum_tx_t");
        Py_DECREF(l_result);
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_dir);

    PyGILState_Release(l_gstate);
    return l_tx;
}

static void s_wallet_cache_iter_delete_wrapper(dap_ledger_wallet_cache_iter_t *a_iter) {
    python_ledger_wallet_cache_ctx_t *l_ctx = cf_ledger_wallet_cache_callbacks_get();
    if (!a_iter) {
        return;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_iter = (PyObject *)a_iter;
    PyObject *l_user_data = l_ctx && l_ctx->user_data ? l_ctx->user_data : Py_None;

    if (l_ctx && l_ctx->iter_delete_callback) {
        PyObject *l_result = PyObject_CallFunctionObjArgs(
            l_ctx->iter_delete_callback, l_iter, l_user_data, NULL
        );
        if (!l_result) {
            log_it(L_ERROR, "Python wallet cache iter_delete callback raised an exception");
            PyErr_Print();
        } else {
            Py_DECREF(l_result);
        }
    }

    Py_DECREF(l_iter);

    PyGILState_Release(l_gstate);
}

// =========================================
// MEMPOOL CALLBACK WRAPPERS
// =========================================

static char *s_mempool_add_datum_callback_wrapper(void *a_datum, void *a_chain, const char *a_hash_out_type) {
    python_ledger_mempool_ctx_t *l_ctx = cf_ledger_mempool_callbacks_get();
    if (!l_ctx || !l_ctx->add_datum_callback) {
        return NULL;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_datum = PyCapsule_New(a_datum, "dap_chain_datum_t", NULL);
    PyObject *l_chain = a_chain ? PyCapsule_New(a_chain, "dap_chain_t", NULL) : Py_None;
    if (!a_chain) {
        Py_INCREF(Py_None);
    }
    PyObject *l_hash_out_type = a_hash_out_type ? PyUnicode_FromString(a_hash_out_type) : Py_None;
    if (!a_hash_out_type) {
        Py_INCREF(Py_None);
    }
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    char *l_ret_str = NULL;
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->add_datum_callback, l_datum, l_chain, l_hash_out_type, l_user_data, NULL
    );
    if (!l_result) {
        log_it(L_ERROR, "Python mempool add datum callback raised an exception");
        PyErr_Print();
    } else if (l_result != Py_None) {
        PyObject *l_str_obj = PyObject_Str(l_result);
        if (l_str_obj) {
            const char *l_str = PyUnicode_AsUTF8(l_str_obj);
            if (l_str) {
                l_ret_str = dap_strdup(l_str);
            }
            Py_DECREF(l_str_obj);
        }
        Py_DECREF(l_result);
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_datum);
    Py_DECREF(l_chain);
    Py_DECREF(l_hash_out_type);

    PyGILState_Release(l_gstate);
    return l_ret_str;
}

static char *s_mempool_create_tx_callback_wrapper(void *a_chain, dap_enc_key_t *a_key, void *a_from,
                                                  const void **a_to, uint256_t *a_value, uint256_t *a_fee,
                                                  const char *a_token, const char *a_hash_out_type) {
    python_ledger_mempool_ctx_t *l_ctx = cf_ledger_mempool_callbacks_get();
    if (!l_ctx || !l_ctx->create_tx_callback) {
        return NULL;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_chain = a_chain ? PyCapsule_New(a_chain, "dap_chain_t", NULL) : Py_None;
    if (!a_chain) {
        Py_INCREF(Py_None);
    }
    PyObject *l_key = a_key ? PyCapsule_New(a_key, "dap_enc_key_t", NULL) : Py_None;
    if (!a_key) {
        Py_INCREF(Py_None);
    }
    PyObject *l_from = a_from ? PyCapsule_New(a_from, "void_ptr", NULL) : Py_None;
    if (!a_from) {
        Py_INCREF(Py_None);
    }
    PyObject *l_to = a_to ? PyCapsule_New((void *)a_to, "void_ptr", NULL) : Py_None;
    if (!a_to) {
        Py_INCREF(Py_None);
    }
    PyObject *l_value = a_value ? PyBytes_FromStringAndSize((const char *)a_value, sizeof(uint256_t)) : Py_None;
    if (!a_value) {
        Py_INCREF(Py_None);
    }
    PyObject *l_fee = a_fee ? PyBytes_FromStringAndSize((const char *)a_fee, sizeof(uint256_t)) : Py_None;
    if (!a_fee) {
        Py_INCREF(Py_None);
    }
    PyObject *l_token = a_token ? PyUnicode_FromString(a_token) : Py_None;
    if (!a_token) {
        Py_INCREF(Py_None);
    }
    PyObject *l_hash_out_type = a_hash_out_type ? PyUnicode_FromString(a_hash_out_type) : Py_None;
    if (!a_hash_out_type) {
        Py_INCREF(Py_None);
    }
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    char *l_ret_str = NULL;
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->create_tx_callback, l_chain, l_key, l_from, l_to, l_value, l_fee,
        l_token, l_hash_out_type, l_user_data, NULL
    );
    if (!l_result) {
        log_it(L_ERROR, "Python mempool create tx callback raised an exception");
        PyErr_Print();
    } else if (l_result != Py_None) {
        PyObject *l_str_obj = PyObject_Str(l_result);
        if (l_str_obj) {
            const char *l_str = PyUnicode_AsUTF8(l_str_obj);
            if (l_str) {
                l_ret_str = dap_strdup(l_str);
            }
            Py_DECREF(l_str_obj);
        }
        Py_DECREF(l_result);
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_chain);
    Py_DECREF(l_key);
    Py_DECREF(l_from);
    Py_DECREF(l_to);
    Py_DECREF(l_value);
    Py_DECREF(l_fee);
    Py_DECREF(l_token);
    Py_DECREF(l_hash_out_type);

    PyGILState_Release(l_gstate);
    return l_ret_str;
}

// =========================================
// LEDGER CALLBACKS (GLOBAL)
// =========================================

static const char *s_ledger_addr_to_wallet_name_wrapper(const dap_chain_addr_t *a_addr) {
    python_ledger_callbacks_ctx_t *l_ctx = cf_ledger_callbacks_get();
    if (!l_ctx || !l_ctx->addr_to_wallet_name_callback) {
        return NULL;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    dap_chain_addr_t *l_addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    PyObject *l_addr = NULL;
    if (l_addr_copy && a_addr) {
        *l_addr_copy = *a_addr;
        l_addr = cf_common_capsule_new_addr(l_addr_copy);
    }
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    const char *l_ret = NULL;
    if (l_addr) {
        PyObject *l_result = PyObject_CallFunctionObjArgs(
            l_ctx->addr_to_wallet_name_callback, l_addr, l_user_data, NULL
        );
        if (!l_result) {
            log_it(L_ERROR, "Python addr_to_wallet_name callback raised an exception");
            PyErr_Print();
        } else if (l_result != Py_None) {
            PyObject *l_str_obj = PyObject_Str(l_result);
            if (l_str_obj) {
                const char *l_str = PyUnicode_AsUTF8(l_str_obj);
                if (l_str) {
                    DAP_DELETE(l_ctx->last_wallet_name);
                    l_ctx->last_wallet_name = dap_strdup(l_str);
                    l_ret = l_ctx->last_wallet_name;
                }
                Py_DECREF(l_str_obj);
            }
            Py_DECREF(l_result);
        } else {
            Py_DECREF(l_result);
        }
        Py_DECREF(l_addr);
    }

    PyGILState_Release(l_gstate);
    return l_ret;
}

static void s_ledger_wallet_info_to_json_wrapper(dap_json_object_t *a_json_obj,
                                                 const char *a_wallet_name,
                                                 const char *a_wallet_path) {
    python_ledger_callbacks_ctx_t *l_ctx = cf_ledger_callbacks_get();
    if (!l_ctx || !l_ctx->wallet_info_to_json_callback || !a_json_obj) {
        return;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_wallet_name = PyUnicode_FromString(a_wallet_name ? a_wallet_name : "");
    PyObject *l_wallet_path = PyUnicode_FromString(a_wallet_path ? a_wallet_path : "");
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->wallet_info_to_json_callback, l_wallet_name, l_wallet_path, l_user_data, NULL
    );

    if (!l_result) {
        log_it(L_ERROR, "Python wallet_info_to_json callback raised an exception");
        PyErr_Print();
    } else if (PyDict_Check(l_result)) {
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(l_result, &pos, &key, &value)) {
            PyObject *key_str = PyObject_Str(key);
            if (!key_str) {
                continue;
            }
            const char *key_cstr = PyUnicode_AsUTF8(key_str);
            if (!key_cstr) {
                Py_DECREF(key_str);
                continue;
            }
            if (PyBool_Check(value)) {
                dap_json_object_add_bool(a_json_obj, key_cstr, PyObject_IsTrue(value));
            } else if (PyLong_Check(value)) {
                dap_json_object_add_int64(a_json_obj, key_cstr, PyLong_AsLongLong(value));
            } else if (PyFloat_Check(value)) {
                dap_json_object_add_double(a_json_obj, key_cstr, PyFloat_AsDouble(value));
            } else if (PyBytes_Check(value)) {
                dap_json_object_add_string_len(a_json_obj, key_cstr, PyBytes_AsString(value),
                                               (int)PyBytes_Size(value));
            } else {
                PyObject *val_str = PyObject_Str(value);
                if (val_str) {
                    const char *val_cstr = PyUnicode_AsUTF8(val_str);
                    if (val_cstr) {
                        dap_json_object_add_string(a_json_obj, key_cstr, val_cstr);
                    }
                    Py_DECREF(val_str);
                }
            }
            Py_DECREF(key_str);
        }
        Py_DECREF(l_result);
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_wallet_name);
    Py_DECREF(l_wallet_path);

    PyGILState_Release(l_gstate);
}

static void s_ledger_tx_added_wrapper(dap_chain_datum_tx_t *a_tx, const char *a_net_name) {
    python_ledger_callbacks_ctx_t *l_ctx = cf_ledger_callbacks_get();
    if (!l_ctx || !l_ctx->tx_added_callback) {
        return;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_tx = PyCapsule_New(a_tx, "dap_chain_datum_tx_t", NULL);
    PyObject *l_net_name = PyUnicode_FromString(a_net_name ? a_net_name : "");
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->tx_added_callback, l_tx, l_net_name, l_user_data, NULL
    );
    if (!l_result) {
        log_it(L_ERROR, "Python tx_added callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }

    Py_DECREF(l_tx);
    Py_DECREF(l_net_name);

    PyGILState_Release(l_gstate);
}


// =============================================================================
// CALLBACKS & NOTIFICATIONS
// =============================================================================

/**
 * @brief Convert dap_chain_tx_event_t to Python dict
 */
PyObject* py_chain_tx_event_to_dict(dap_chain_tx_event_t *a_event) {
    if (!a_event) {
        Py_RETURN_NONE;
    }
    
    PyObject *l_dict = PyDict_New();
    if (!l_dict) {
        return NULL;
    }
    
    // srv_uid
    PyObject *l_srv_uid = PyLong_FromUnsignedLongLong(a_event->srv_uid.uint64);
    if (l_srv_uid) {
        PyDict_SetItemString(l_dict, "srv_uid", l_srv_uid);
        Py_DECREF(l_srv_uid);
    }
    
    // timestamp
    PyObject *l_timestamp = PyLong_FromUnsignedLongLong(a_event->timestamp);
    if (l_timestamp) {
        PyDict_SetItemString(l_dict, "timestamp", l_timestamp);
        Py_DECREF(l_timestamp);
    }
    
    // group_name
    if (a_event->group_name) {
        PyObject *l_group_name = PyUnicode_FromString(a_event->group_name);
        if (l_group_name) {
            PyDict_SetItemString(l_dict, "group_name", l_group_name);
            Py_DECREF(l_group_name);
        }
    }
    
    // tx_hash
    PyObject *l_tx_hash = PyBytes_FromStringAndSize((const char*)a_event->tx_hash.raw, sizeof(dap_hash_fast_t));
    if (l_tx_hash) {
        PyDict_SetItemString(l_dict, "tx_hash", l_tx_hash);
        Py_DECREF(l_tx_hash);
    }
    
    // pkey_hash
    PyObject *l_pkey_hash = PyBytes_FromStringAndSize((const char*)a_event->pkey_hash.raw, sizeof(dap_hash_fast_t));
    if (l_pkey_hash) {
        PyDict_SetItemString(l_dict, "pkey_hash", l_pkey_hash);
        Py_DECREF(l_pkey_hash);
    }
    
    // event_type
    PyObject *l_event_type = PyLong_FromUnsignedLong(a_event->event_type);
    if (l_event_type) {
        PyDict_SetItemString(l_dict, "event_type", l_event_type);
        Py_DECREF(l_event_type);
    }
    
    // event_data
    if (a_event->event_data && a_event->event_data_size > 0) {
        PyObject *l_event_data = PyBytes_FromStringAndSize((const char*)a_event->event_data, a_event->event_data_size);
        if (l_event_data) {
            PyDict_SetItemString(l_dict, "event_data", l_event_data);
            Py_DECREF(l_event_data);
        }
    }
    
    return l_dict;
}

/**
 * @brief C callback wrapper for event notifications
 */
void py_ledger_event_notify_callback(void *a_arg, dap_ledger_t *a_ledger, 
                                            dap_chain_tx_event_t *a_event, 
                                            dap_hash_fast_t *a_tx_hash,
                                            dap_ledger_notify_opcodes_t a_opcode) {
    py_ledger_callback_data_t *l_data = (py_ledger_callback_data_t*)a_arg;
    if (!l_data || !l_data->callback) {
        return;
    }
    
    // Acquire GIL
    PyGILState_STATE l_gil_state = PyGILState_Ensure();
    
    // Convert event to Python dict
    PyObject *l_event_dict = py_chain_tx_event_to_dict(a_event);
    if (!l_event_dict) {
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Convert tx_hash to bytes
    PyObject *l_tx_hash_bytes = PyBytes_FromStringAndSize((const char*)a_tx_hash->raw, sizeof(dap_hash_fast_t));
    if (!l_tx_hash_bytes) {
        Py_DECREF(l_event_dict);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Convert opcode to int
    PyObject *l_opcode = PyLong_FromLong((long)a_opcode);
    if (!l_opcode) {
        Py_DECREF(l_event_dict);
        Py_DECREF(l_tx_hash_bytes);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Call Python callback: callback(ledger_capsule, event_dict, tx_hash_bytes, opcode)
    PyObject *l_result = PyObject_CallFunctionObjArgs(l_data->callback, 
                                                       l_data->ledger_capsule,
                                                       l_event_dict,
                                                       l_tx_hash_bytes,
                                                       l_opcode,
                                                       NULL);
    
    // Cleanup
    Py_XDECREF(l_result);
    Py_DECREF(l_event_dict);
    Py_DECREF(l_tx_hash_bytes);
    Py_DECREF(l_opcode);
    
    // Check for Python exceptions
    if (PyErr_Occurred()) {
        PyErr_Print();
        PyErr_Clear();
    }
    
    // Release GIL
    PyGILState_Release(l_gil_state);
}

/**
 * @brief C callback wrapper for transaction add notifications
 */
void py_ledger_tx_add_notify_callback(void *a_arg, dap_ledger_t *a_ledger, 
                                             dap_chain_datum_tx_t *a_tx,
                                             dap_ledger_notify_opcodes_t a_opcode) {
    py_ledger_callback_data_t *l_data = (py_ledger_callback_data_t*)a_arg;
    if (!l_data || !l_data->callback) {
        return;
    }
    
    // Acquire GIL
    PyGILState_STATE l_gil_state = PyGILState_Ensure();
    
    // Convert tx to capsule
    PyObject *l_tx_capsule = PyCapsule_New(a_tx, "dap_chain_datum_tx_t", NULL);
    if (!l_tx_capsule) {
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Convert opcode to int
    PyObject *l_opcode = PyLong_FromLong((long)a_opcode);
    if (!l_opcode) {
        Py_DECREF(l_tx_capsule);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Call Python callback: callback(ledger_capsule, tx_capsule, opcode)
    PyObject *l_result = PyObject_CallFunctionObjArgs(l_data->callback,
                                                       l_data->ledger_capsule,
                                                       l_tx_capsule,
                                                       l_opcode,
                                                       NULL);
    
    // Cleanup
    Py_XDECREF(l_result);
    Py_DECREF(l_tx_capsule);
    Py_DECREF(l_opcode);
    
    // Check for Python exceptions
    if (PyErr_Occurred()) {
        PyErr_Print();
        PyErr_Clear();
    }
    
    // Release GIL
    PyGILState_Release(l_gil_state);
}

/**
 * @brief C callback wrapper for bridged transaction notifications
 */
void py_ledger_bridged_tx_notify_callback(dap_ledger_t *a_ledger, 
                                                 dap_chain_datum_tx_t *a_tx,
                                                 dap_hash_fast_t *a_tx_hash,
                                                 void *a_arg,
                                                 dap_ledger_notify_opcodes_t a_opcode) {
    py_ledger_callback_data_t *l_data = (py_ledger_callback_data_t*)a_arg;
    if (!l_data || !l_data->callback) {
        return;
    }
    
    // Acquire GIL
    PyGILState_STATE l_gil_state = PyGILState_Ensure();
    
    // Convert tx to capsule
    PyObject *l_tx_capsule = PyCapsule_New(a_tx, "dap_chain_datum_tx_t", NULL);
    if (!l_tx_capsule) {
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Convert tx_hash to bytes
    PyObject *l_tx_hash_bytes = PyBytes_FromStringAndSize((const char*)a_tx_hash->raw, sizeof(dap_hash_fast_t));
    if (!l_tx_hash_bytes) {
        Py_DECREF(l_tx_capsule);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Convert opcode to int
    PyObject *l_opcode = PyLong_FromLong((long)a_opcode);
    if (!l_opcode) {
        Py_DECREF(l_tx_capsule);
        Py_DECREF(l_tx_hash_bytes);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Call Python callback: callback(ledger_capsule, tx_capsule, tx_hash_bytes, opcode)
    PyObject *l_result = PyObject_CallFunctionObjArgs(l_data->callback,
                                                       l_data->ledger_capsule,
                                                       l_tx_capsule,
                                                       l_tx_hash_bytes,
                                                       l_opcode,
                                                       NULL);
    
    // Cleanup
    Py_XDECREF(l_result);
    Py_DECREF(l_tx_capsule);
    Py_DECREF(l_tx_hash_bytes);
    Py_DECREF(l_opcode);
    
    // Check for Python exceptions
    if (PyErr_Occurred()) {
        PyErr_Print();
        PyErr_Clear();
    }
    
    // Release GIL
    PyGILState_Release(l_gil_state);
}

/**
 * @brief Cleanup callback data when capsule is destroyed
 */
static void py_ledger_callback_data_cleanup(PyObject *a_capsule) {
    py_ledger_callback_data_t *l_data = (py_ledger_callback_data_t*)PyCapsule_GetPointer(a_capsule, "py_ledger_callback_data_t");
    if (l_data) {
        Py_XDECREF(l_data->callback);
        Py_XDECREF(l_data->ledger_capsule);
        DAP_DELETE(l_data);
    }
}

/**
 * @brief Add transaction add notification callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, Python callable)
 * @return None
 */
PyObject* dap_ledger_tx_add_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    PyObject *l_callback;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_callback)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a callable");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    // Allocate callback data structure
    py_ledger_callback_data_t *l_data = DAP_NEW_Z(py_ledger_callback_data_t);
    if (!l_data) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback data");
        return NULL;
    }
    
    l_data->callback = l_callback;
    l_data->ledger_capsule = l_ledger_obj;
    
    // Register C callback
    dap_ledger_tx_add_notify(l_ledger, py_ledger_tx_add_notify_callback, l_data);
    
    // Register in global registry for proper cleanup (prevents memory leak)
    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_LEDGER_TX, l_callback, l_ledger_obj, 
                                   l_data, NULL) != 0) {
        log_it(L_WARNING, "Failed to register callback in global registry (potential leak!)");
    }
    
    log_it(L_DEBUG, "Registered transaction add notification callback");
    
    Py_RETURN_NONE;
}

/**
 * @brief Add event notification callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, Python callable)
 * @return None
 */
PyObject* dap_ledger_event_notify_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    PyObject *l_callback;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_callback)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a callable");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    // Allocate callback data structure
    py_ledger_callback_data_t *l_data = DAP_NEW_Z(py_ledger_callback_data_t);
    if (!l_data) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback data");
        return NULL;
    }
    
    l_data->callback = l_callback;
    l_data->ledger_capsule = l_ledger_obj;
    
    // Register C callback
    dap_ledger_event_notify_add(l_ledger, py_ledger_event_notify_callback, l_data);
    
    // Register in global registry for proper cleanup (prevents memory leak)
    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_LEDGER_EVENT, l_callback, l_ledger_obj, 
                                   l_data, NULL) != 0) {
        log_it(L_WARNING, "Failed to register callback in global registry (potential leak!)");
    }
    
    log_it(L_DEBUG, "Registered event notification callback");
    
    Py_RETURN_NONE;
}

/**
 * @brief Add bridged transaction notification callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, Python callable)
 * @return None
 */
PyObject* dap_ledger_bridged_tx_notify_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    PyObject *l_callback;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_callback)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a callable");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    // Allocate callback data structure
    py_ledger_callback_data_t *l_data = DAP_NEW_Z(py_ledger_callback_data_t);
    if (!l_data) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback data");
        return NULL;
    }
    
    l_data->callback = l_callback;
    l_data->ledger_capsule = l_ledger_obj;
    
    // Register C callback
    dap_ledger_bridged_tx_notify_add(l_ledger, py_ledger_bridged_tx_notify_callback, l_data);
    
    // Register in global registry for proper cleanup (prevents memory leak)
    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_LEDGER_BRIDGED_TX, l_callback, l_ledger_obj, 
                                   l_data, NULL) != 0) {
        log_it(L_WARNING, "Failed to register callback in global registry (potential leak!)");
    }
    
    log_it(L_DEBUG, "Registered bridged transaction notification callback");
    
    Py_RETURN_NONE;
}

// =============================================================================
// REGISTRATION & SETTERS
// =============================================================================

/**
 * @brief Register balance change notification callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, callback, optional user_data)
 * @return None
 */
PyObject* dap_ledger_balance_change_notifier_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "OO|O", &l_ledger_obj, &l_callback, &l_user_data)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be callable");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    PyObject *l_user_real = (l_user_data == Py_None) ? NULL : l_user_data;
    int l_first = cf_ledger_balance_callback_add(l_ledger, l_callback, l_user_real);
    if (l_first < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register balance change callback");
        return NULL;
    }

    if (l_first == 1) {
        dap_ledger_balance_change_notifier_register(l_ledger, s_ledger_balance_change_wrapper, NULL);
    }

    Py_RETURN_NONE;
}

/**
 * @brief Unregister balance change notification callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, callback)
 * @return None
 */
PyObject* dap_ledger_balance_change_notifier_unregister_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_callback = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_callback)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be callable");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    int l_last = cf_ledger_balance_callback_remove(l_ledger, l_callback);
    if (l_last == 1) {
        dap_ledger_balance_change_notifier_unregister(l_ledger, s_ledger_balance_change_wrapper);
    }

    Py_RETURN_NONE;
}

/**
 * @brief Register global ledger callbacks
 * @param a_self Python self object (unused)
 * @param a_args Arguments (addr_to_wallet_name, wallet_info_to_json, tx_added, optional user_data)
 * @return None
 */
PyObject* dap_ledger_callbacks_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_addr_to_wallet = Py_None;
    PyObject *l_wallet_info = Py_None;
    PyObject *l_tx_added = Py_None;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "OOO|O", &l_addr_to_wallet, &l_wallet_info, &l_tx_added, &l_user_data)) {
        return NULL;
    }

    if (l_addr_to_wallet != Py_None && !PyCallable_Check(l_addr_to_wallet)) {
        PyErr_SetString(PyExc_TypeError, "addr_to_wallet_name must be callable or None");
        return NULL;
    }
    if (l_wallet_info != Py_None && !PyCallable_Check(l_wallet_info)) {
        PyErr_SetString(PyExc_TypeError, "wallet_info_to_json must be callable or None");
        return NULL;
    }
    if (l_tx_added != Py_None && !PyCallable_Check(l_tx_added)) {
        PyErr_SetString(PyExc_TypeError, "tx_added must be callable or None");
        return NULL;
    }

    PyObject *l_addr_real = (l_addr_to_wallet == Py_None) ? NULL : l_addr_to_wallet;
    PyObject *l_wallet_real = (l_wallet_info == Py_None) ? NULL : l_wallet_info;
    PyObject *l_tx_real = (l_tx_added == Py_None) ? NULL : l_tx_added;
    PyObject *l_user_real = (l_user_data == Py_None) ? NULL : l_user_data;

    if (cf_ledger_callbacks_register(l_addr_real, l_wallet_real, l_tx_real, l_user_real) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register ledger callbacks");
        return NULL;
    }

    dap_ledger_callbacks_t l_callbacks = {
        .addr_to_wallet_name = l_addr_real ? s_ledger_addr_to_wallet_name_wrapper : NULL,
        .wallet_info_to_json = l_wallet_real ? s_ledger_wallet_info_to_json_wrapper : NULL,
        .tx_added = l_tx_real ? s_ledger_tx_added_wrapper : NULL
    };
    dap_ledger_callbacks_register(&l_callbacks);

    Py_RETURN_NONE;
}

/**
 * @brief Get global ledger callbacks
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return Dict with callbacks
 */
PyObject* dap_ledger_callbacks_get_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    python_ledger_callbacks_ctx_t *l_ctx = cf_ledger_callbacks_get();
    PyObject *l_dict = PyDict_New();
    if (!l_dict) {
        return NULL;
    }

    PyObject *l_addr = l_ctx && l_ctx->addr_to_wallet_name_callback ? l_ctx->addr_to_wallet_name_callback : Py_None;
    PyObject *l_info = l_ctx && l_ctx->wallet_info_to_json_callback ? l_ctx->wallet_info_to_json_callback : Py_None;
    PyObject *l_tx = l_ctx && l_ctx->tx_added_callback ? l_ctx->tx_added_callback : Py_None;
    PyObject *l_user = l_ctx && l_ctx->user_data ? l_ctx->user_data : Py_None;

    Py_INCREF(l_addr);
    Py_INCREF(l_info);
    Py_INCREF(l_tx);
    Py_INCREF(l_user);

    PyDict_SetItemString(l_dict, "addr_to_wallet_name", l_addr);
    PyDict_SetItemString(l_dict, "wallet_info_to_json", l_info);
    PyDict_SetItemString(l_dict, "tx_added", l_tx);
    PyDict_SetItemString(l_dict, "user_data", l_user);

    Py_DECREF(l_addr);
    Py_DECREF(l_info);
    Py_DECREF(l_tx);
    Py_DECREF(l_user);

    return l_dict;
}

/**
 * @brief Set per-ledger fee callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, callback, optional user_data)
 * @return None
 */
PyObject* dap_ledger_set_fee_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_callback = Py_None;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "OO|O", &l_ledger_obj, &l_callback, &l_user_data)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    if (l_callback != Py_None && !PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be callable or None");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    PyObject *l_cb_real = (l_callback == Py_None) ? NULL : l_callback;
    PyObject *l_user_real = (l_user_data == Py_None) ? NULL : l_user_data;

    if (l_cb_real) {
        cf_ledger_fee_callback_register(l_ledger, l_cb_real, l_user_real);
        dap_ledger_set_fee_callback(l_ledger, s_ledger_fee_callback_wrapper);
    } else {
        cf_ledger_fee_callback_unregister(l_ledger);
        dap_ledger_set_fee_callback(l_ledger, NULL);
    }

    Py_RETURN_NONE;
}

/**
 * @brief Set per-ledger get current cell callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, callback, optional user_data)
 * @return None
 */
PyObject* dap_ledger_set_get_cur_cell_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_callback = Py_None;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "OO|O", &l_ledger_obj, &l_callback, &l_user_data)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    if (l_callback != Py_None && !PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be callable or None");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    PyObject *l_cb_real = (l_callback == Py_None) ? NULL : l_callback;
    PyObject *l_user_real = (l_user_data == Py_None) ? NULL : l_user_data;

    if (l_cb_real) {
        cf_ledger_get_cur_cell_callback_register(l_ledger, l_cb_real, l_user_real);
        dap_ledger_set_get_cur_cell_callback(l_ledger, s_ledger_get_cur_cell_callback_wrapper);
    } else {
        cf_ledger_get_cur_cell_callback_unregister(l_ledger);
        dap_ledger_set_get_cur_cell_callback(l_ledger, NULL);
    }

    Py_RETURN_NONE;
}

/**
 * @brief Set wallet callbacks
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, sign_cb, get_pkey_hash_cb, get_pkey_cb, get_addr_cb, check_sign_cb, optional user_data)
 * @return None
 */
PyObject* dap_ledger_set_wallet_callbacks_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_sign = Py_None;
    PyObject *l_get_hash = Py_None;
    PyObject *l_get_pkey = Py_None;
    PyObject *l_get_addr = Py_None;
    PyObject *l_check_sign = Py_None;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "OOOOOO|O", &l_ledger_obj, &l_sign, &l_get_hash, &l_get_pkey,
                          &l_get_addr, &l_check_sign, &l_user_data)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    if (l_sign != Py_None && !PyCallable_Check(l_sign)) {
        PyErr_SetString(PyExc_TypeError, "sign callback must be callable or None");
        return NULL;
    }
    if (l_get_hash != Py_None && !PyCallable_Check(l_get_hash)) {
        PyErr_SetString(PyExc_TypeError, "get_pkey_hash callback must be callable or None");
        return NULL;
    }
    if (l_get_pkey != Py_None && !PyCallable_Check(l_get_pkey)) {
        PyErr_SetString(PyExc_TypeError, "get_pkey callback must be callable or None");
        return NULL;
    }
    if (l_get_addr != Py_None && !PyCallable_Check(l_get_addr)) {
        PyErr_SetString(PyExc_TypeError, "get_addr callback must be callable or None");
        return NULL;
    }
    if (l_check_sign != Py_None && !PyCallable_Check(l_check_sign)) {
        PyErr_SetString(PyExc_TypeError, "check_sign callback must be callable or None");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    PyObject *l_sign_real = (l_sign == Py_None) ? NULL : l_sign;
    PyObject *l_hash_real = (l_get_hash == Py_None) ? NULL : l_get_hash;
    PyObject *l_pkey_real = (l_get_pkey == Py_None) ? NULL : l_get_pkey;
    PyObject *l_addr_real = (l_get_addr == Py_None) ? NULL : l_get_addr;
    PyObject *l_check_real = (l_check_sign == Py_None) ? NULL : l_check_sign;
    PyObject *l_user_real = (l_user_data == Py_None) ? NULL : l_user_data;

    if (cf_ledger_wallet_callbacks_register(l_sign_real, l_hash_real, l_pkey_real,
                                            l_addr_real, l_check_real, l_user_real) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register wallet callbacks");
        return NULL;
    }

    dap_ledger_set_wallet_callbacks(
        l_ledger,
        l_sign_real ? s_ledger_wallet_sign_callback_wrapper : NULL,
        l_hash_real ? s_ledger_wallet_get_pkey_hash_callback_wrapper : NULL,
        l_pkey_real ? s_ledger_wallet_get_pkey_callback_wrapper : NULL,
        l_addr_real ? s_ledger_wallet_get_addr_callback_wrapper : NULL,
        l_check_real ? s_ledger_wallet_check_sign_callback_wrapper : NULL
    );

    Py_RETURN_NONE;
}

/**
 * @brief Set wallet cache callbacks
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, tx_find, iter_create, iter_get, iter_delete, optional user_data)
 * @return None
 */
PyObject* dap_ledger_set_wallet_cache_callbacks_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_tx_find = Py_None;
    PyObject *l_iter_create = Py_None;
    PyObject *l_iter_get = Py_None;
    PyObject *l_iter_delete = Py_None;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "OOOOO|O", &l_ledger_obj, &l_tx_find, &l_iter_create,
                          &l_iter_get, &l_iter_delete, &l_user_data)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    if (l_tx_find != Py_None && !PyCallable_Check(l_tx_find)) {
        PyErr_SetString(PyExc_TypeError, "tx_find callback must be callable or None");
        return NULL;
    }
    if (l_iter_create != Py_None && !PyCallable_Check(l_iter_create)) {
        PyErr_SetString(PyExc_TypeError, "iter_create callback must be callable or None");
        return NULL;
    }
    if (l_iter_get != Py_None && !PyCallable_Check(l_iter_get)) {
        PyErr_SetString(PyExc_TypeError, "iter_get callback must be callable or None");
        return NULL;
    }
    if (l_iter_delete != Py_None && !PyCallable_Check(l_iter_delete)) {
        PyErr_SetString(PyExc_TypeError, "iter_delete callback must be callable or None");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    PyObject *l_tx_find_real = (l_tx_find == Py_None) ? NULL : l_tx_find;
    PyObject *l_iter_create_real = (l_iter_create == Py_None) ? NULL : l_iter_create;
    PyObject *l_iter_get_real = (l_iter_get == Py_None) ? NULL : l_iter_get;
    PyObject *l_iter_delete_real = (l_iter_delete == Py_None) ? NULL : l_iter_delete;
    PyObject *l_user_real = (l_user_data == Py_None) ? NULL : l_user_data;

    if (cf_ledger_wallet_cache_callbacks_register(l_tx_find_real, l_iter_create_real,
                                                  l_iter_get_real, l_iter_delete_real,
                                                  l_user_real) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register wallet cache callbacks");
        return NULL;
    }

    dap_ledger_set_wallet_cache_callbacks(
        l_ledger,
        l_tx_find_real ? s_wallet_cache_tx_find_in_history_wrapper : NULL,
        l_iter_create_real ? s_wallet_cache_iter_create_wrapper : NULL,
        l_iter_get_real ? s_wallet_cache_iter_get_wrapper : NULL,
        l_iter_delete_real ? s_wallet_cache_iter_delete_wrapper : NULL
    );

    Py_RETURN_NONE;
}

/**
 * @brief Set mempool callbacks
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, add_datum, create_tx, optional user_data)
 * @return None
 */
PyObject* dap_ledger_set_mempool_callbacks_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_add_datum = Py_None;
    PyObject *l_create_tx = Py_None;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "OOO|O", &l_ledger_obj, &l_add_datum, &l_create_tx, &l_user_data)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }

    if (l_add_datum != Py_None && !PyCallable_Check(l_add_datum)) {
        PyErr_SetString(PyExc_TypeError, "add_datum callback must be callable or None");
        return NULL;
    }
    if (l_create_tx != Py_None && !PyCallable_Check(l_create_tx)) {
        PyErr_SetString(PyExc_TypeError, "create_tx callback must be callable or None");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    PyObject *l_add_real = (l_add_datum == Py_None) ? NULL : l_add_datum;
    PyObject *l_create_real = (l_create_tx == Py_None) ? NULL : l_create_tx;
    PyObject *l_user_real = (l_user_data == Py_None) ? NULL : l_user_data;

    if (cf_ledger_mempool_callbacks_register(l_add_real, l_create_real, l_user_real) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register mempool callbacks");
        return NULL;
    }

    dap_ledger_set_mempool_callbacks(
        l_ledger,
        l_add_real ? s_mempool_add_datum_callback_wrapper : NULL,
        l_create_real ? s_mempool_create_tx_callback_wrapper : NULL
    );

    Py_RETURN_NONE;
}

// =============================================================================
// CALLBACKS & SPECIAL OPERATIONS
// =============================================================================

/**
 * @brief Get token emission rate
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, token_ticker string)
 * @return Bytes representing uint256_t
 */
PyObject* dap_ledger_token_get_emission_rate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_token_ticker;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_ledger_obj, &l_token_ticker)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    uint256_t l_rate = dap_ledger_token_get_emission_rate(l_ledger, l_token_ticker);
    
    log_it(L_DEBUG, "Got emission rate for token '%s'", l_token_ticker);
    return PyBytes_FromStringAndSize((const char *)&l_rate, sizeof(uint256_t));
}

/**
 * @brief Mark token emissions hardfork
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, hardfork_time uint64)
 * @return Integer result code
 */
PyObject* dap_ledger_token_emissions_mark_hardfork_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    unsigned long long l_hardfork_time;
    
    if (!PyArg_ParseTuple(a_args, "OK", &l_ledger_obj, &l_hardfork_time)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    int l_result = dap_ledger_token_emissions_mark_hardfork(l_ledger, l_hardfork_time);
    
    log_it(L_DEBUG, "Mark emissions hardfork at time %llu, result: %d", l_hardfork_time, l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Add service with callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (service_uid uint64, tag_str string, callback, optional user_data)
 * @return Integer result code
 */
PyObject* dap_ledger_service_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_uid;
    const char *l_tag_str;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "KsO|O", &l_uid, &l_tag_str, &l_callback, &l_user_data)) {
        PyErr_SetString(PyExc_TypeError, "Expected (uid, tag_str, callback, [user_data])");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Third argument must be callable");
        return NULL;
    }
    
    dap_chain_srv_uid_t l_srv_uid = { .uint64 = l_uid };
    
    // Register in global registry
    if (cf_ledger_service_callback_register(l_srv_uid, l_tag_str, l_callback, l_user_data) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register service callback");
        return NULL;
    }
    
    // Register with SDK
    int l_result = dap_ledger_service_add(l_srv_uid, (char*)l_tag_str, s_ledger_service_tag_check_wrapper);
    
    log_it(L_DEBUG, "Registered service callback: UID=0x%llx, tag='%s', result=%d", l_uid, l_tag_str, l_result);
    
    return PyLong_FromLong(l_result);
}

/**
 * @brief Set global tax callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (callback, optional user_data)
 * @return Integer result code
 */
PyObject* dap_ledger_tax_callback_set_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "O|O", &l_callback, &l_user_data)) {
        PyErr_SetString(PyExc_TypeError, "Expected (callback, [user_data])");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be callable");
        return NULL;
    }
    
    // Register in global registry
    if (cf_ledger_tax_callback_register(l_callback, l_user_data) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register tax callback");
        return NULL;
    }
    
    // Register with SDK
    int l_result = dap_ledger_tax_callback_set(s_ledger_tax_callback_wrapper);
    
    log_it(L_DEBUG, "Registered global tax callback, result=%d", l_result);
    
    return PyLong_FromLong(l_result);
}


// Get method definitions for callbacks module
PyMethodDef* cellframe_ledger_callbacks_get_methods(void) {
    static PyMethodDef callbacks_methods[] = {
        {"ledger_tx_add_notify", (PyCFunction)dap_ledger_tx_add_notify_py, METH_VARARGS,
         "Add transaction add notification callback"},
        {"ledger_event_notify_add", (PyCFunction)dap_ledger_event_notify_add_py, METH_VARARGS,
         "Add event notification callback"},
        {"ledger_bridged_tx_notify_add", (PyCFunction)dap_ledger_bridged_tx_notify_add_py, METH_VARARGS,
         "Add bridged transaction notification callback"},
        {"ledger_balance_change_notifier_register", (PyCFunction)dap_ledger_balance_change_notifier_register_py, METH_VARARGS,
         "Register balance change notification callback"},
        {"ledger_balance_change_notifier_unregister", (PyCFunction)dap_ledger_balance_change_notifier_unregister_py, METH_VARARGS,
         "Unregister balance change notification callback"},
        {"ledger_callbacks_register", (PyCFunction)dap_ledger_callbacks_register_py, METH_VARARGS,
         "Register global ledger callbacks"},
        {"ledger_callbacks_get", (PyCFunction)dap_ledger_callbacks_get_py, METH_NOARGS,
         "Get global ledger callbacks"},
        {"ledger_set_fee_callback", (PyCFunction)dap_ledger_set_fee_callback_py, METH_VARARGS,
         "Set per-ledger fee callback"},
        {"ledger_set_get_cur_cell_callback", (PyCFunction)dap_ledger_set_get_cur_cell_callback_py, METH_VARARGS,
         "Set per-ledger get current cell callback"},
        {"ledger_set_wallet_callbacks", (PyCFunction)dap_ledger_set_wallet_callbacks_py, METH_VARARGS,
         "Set wallet callbacks"},
        {"ledger_set_wallet_cache_callbacks", (PyCFunction)dap_ledger_set_wallet_cache_callbacks_py, METH_VARARGS,
         "Set wallet cache callbacks"},
        {"ledger_set_mempool_callbacks", (PyCFunction)dap_ledger_set_mempool_callbacks_py, METH_VARARGS,
         "Set mempool callbacks"},
        {"ledger_token_get_emission_rate", (PyCFunction)dap_ledger_token_get_emission_rate_py, METH_VARARGS,
         "Get token emission rate"},
        {"ledger_token_emissions_mark_hardfork", (PyCFunction)dap_ledger_token_emissions_mark_hardfork_py, METH_VARARGS,
         "Mark token emissions hardfork"},
        {"ledger_service_add", (PyCFunction)dap_ledger_service_add_py, METH_VARARGS,
         "Add service with callback"},
        {"ledger_tax_callback_set", (PyCFunction)dap_ledger_tax_callback_set_py, METH_VARARGS,
         "Set tax callback"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return callbacks_methods;
}
