#include "include/cf_ledger_internal.h"

/*
 * Cellframe ledger transaction lists bindings
 * Transaction lists and queries: get txs, check recipient, service info, tag operations
 */


// =============================================================================
// TX LISTS & OPERATIONS
// =============================================================================

/**
 * @brief Get list of transaction items for address as JSON
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, addr, hash_out_type, unspent_only)
 * @return PyCapsule wrapping dap_json_t* or NULL on error
 */
PyObject* dap_ledger_token_tx_item_list_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_addr_obj;
    const char *l_hash_out_type;
    int l_unspent_only = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOsp", &l_ledger_obj, &l_addr_obj, &l_hash_out_type, &l_unspent_only)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be an address capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_obj, "dap_chain_addr_t");
    if (!l_addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid address capsule");
        return NULL;
    }
    
    dap_json_t *l_json = dap_ledger_token_tx_item_list(l_ledger, l_addr, l_hash_out_type, (bool)l_unspent_only);
    if (!l_json) {
        log_it(L_DEBUG, "No transaction items found");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved transaction item list");
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

/**
 * @brief Get list of transactions
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, count, page, reverse, unspent_only)
 * @return PyCapsule wrapping dap_list_t* or NULL on error
 */
PyObject* dap_ledger_get_txs_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    Py_ssize_t l_count = 0, l_page = 0;
    int l_reverse = 0, l_unspent_only = 0;
    
    if (!PyArg_ParseTuple(a_args, "Onn|pp", &l_ledger_obj, &l_count, &l_page, &l_reverse, &l_unspent_only)) {
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
    
    dap_list_t *l_list = dap_ledger_get_txs(l_ledger, (size_t)l_count, (size_t)l_page, 
                                             (bool)l_reverse, (bool)l_unspent_only);
    if (!l_list) {
        log_it(L_DEBUG, "No transactions found");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved transaction list (count=%zu, page=%zu)", (size_t)l_count, (size_t)l_page);
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

/**
 * @brief Check if TX output is used
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, tx_hash bytes, idx_out, out_spender_hash bytes)
 * @return True if used, False otherwise
 */
PyObject* dap_ledger_tx_hash_is_used_out_item_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    int l_idx_out;
    const char *l_spender_bytes = NULL;
    Py_ssize_t l_spender_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "Os#i|s#", &l_ledger_obj, &l_hash_bytes, &l_hash_size, 
                          &l_idx_out, &l_spender_bytes, &l_spender_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_chain_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_hash = (dap_chain_hash_fast_t *)l_hash_bytes;
    dap_hash_fast_t *l_out_spender = l_spender_bytes ? (dap_hash_fast_t *)l_spender_bytes : NULL;
    
    bool l_result = dap_ledger_tx_hash_is_used_out_item(l_ledger, l_hash, l_idx_out, l_out_spender);
    
    log_it(L_DEBUG, "TX output used check: %s", l_result ? "true" : "false");
    return PyBool_FromLong(l_result);
}

/**
 * @brief Get final chain transaction hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, cond_type, tx_hash bytes, unspent_only)
 * @return Hash as bytes or None
 */
PyObject* dap_ledger_get_final_chain_tx_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    int l_cond_type;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    int l_unspent_only = 0;
    
    if (!PyArg_ParseTuple(a_args, "Ois#|p", &l_ledger_obj, &l_cond_type, &l_hash_bytes, &l_hash_size, &l_unspent_only)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_chain_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_tx_hash = (dap_chain_hash_fast_t *)l_hash_bytes;
    
    dap_hash_fast_t l_result = dap_ledger_get_final_chain_tx_hash(l_ledger, 
                                                                    (dap_chain_tx_out_cond_subtype_t)l_cond_type,
                                                                    l_tx_hash, (bool)l_unspent_only);
    
    log_it(L_DEBUG, "Retrieved final chain TX hash");
    return PyBytes_FromStringAndSize((const char *)&l_result, sizeof(dap_hash_fast_t));
}

/**
 * @brief Get first chain transaction hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, cond_type, tx_hash bytes)
 * @return Hash as bytes or None
 */
PyObject* dap_ledger_get_first_chain_tx_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    int l_cond_type;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Ois#", &l_ledger_obj, &l_cond_type, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_chain_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_tx_hash = (dap_chain_hash_fast_t *)l_hash_bytes;
    
    dap_hash_fast_t l_result = dap_ledger_get_first_chain_tx_hash(l_ledger, 
                                                                    (dap_chain_tx_out_cond_subtype_t)l_cond_type,
                                                                    l_tx_hash);
    
    log_it(L_DEBUG, "Retrieved first chain TX hash");
    return PyBytes_FromStringAndSize((const char *)&l_result, sizeof(dap_hash_fast_t));
}

/**
 * @brief Check if address is transaction recipient
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, tx_prev_hash bytes, addr)
 * @return True if recipient, False otherwise
 */
PyObject* dap_ledger_tx_check_recipient_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_addr_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#O", &l_ledger_obj, &l_hash_bytes, &l_hash_size, &l_addr_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "Third argument must be an address capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_chain_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_obj, "dap_chain_addr_t");
    if (!l_addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid address capsule");
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_hash = (dap_chain_hash_fast_t *)l_hash_bytes;
    
    bool l_result = dap_ledger_tx_check_recipient(l_ledger, l_hash, l_addr);
    
    log_it(L_DEBUG, "TX recipient check: %s", l_result ? "true" : "false");
    return PyBool_FromLong(l_result);
}

/**
 * @brief Check if transaction is PoA signed
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, tx capsule)
 * @return True if PoA signed, False otherwise
 */
PyObject* dap_ledger_tx_poa_signed_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_tx_obj;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_tx_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_tx_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a transaction capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t *)PyCapsule_GetPointer(l_tx_obj, "dap_chain_datum_tx_t");
    if (!l_tx) {
        PyErr_SetString(PyExc_ValueError, "Invalid transaction capsule");
        return NULL;
    }
    
    bool l_result = dap_ledger_tx_poa_signed(l_ledger, l_tx);
    
    log_it(L_DEBUG, "TX PoA signed check: %s", l_result ? "true" : "false");
    return PyBool_FromLong(l_result);
}

/**
 * @brief Get transaction service info
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, tx_hash bytes)
 * @return Dict with uid, service_name, action or None
 */
PyObject* dap_ledger_tx_service_info_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_ledger_obj, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_hash_bytes;
    dap_chain_srv_uid_t l_uid = {0};
    char *l_service_name = NULL;
    dap_chain_tx_tag_action_type_t l_action = 0;
    
    bool l_result = dap_ledger_tx_service_info(l_ledger, l_hash, &l_uid, &l_service_name, &l_action);
    if (!l_result) {
        log_it(L_DEBUG, "TX service info not found");
        Py_RETURN_NONE;
    }
    
    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "uid", PyLong_FromUnsignedLongLong(l_uid.uint64));
    PyDict_SetItemString(l_dict, "service_name", l_service_name ? PyUnicode_FromString(l_service_name) : Py_None);
    PyDict_SetItemString(l_dict, "action", PyLong_FromLong(l_action));
    
    log_it(L_DEBUG, "Retrieved TX service info");
    return l_dict;
}

/**
 * @brief Deduct transaction tag info
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, tx capsule)
 * @return Dict with service_name, uid, action or None
 */
PyObject* dap_ledger_deduct_tx_tag_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_tx_obj;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_tx_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_tx_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a transaction capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t *)PyCapsule_GetPointer(l_tx_obj, "dap_chain_datum_tx_t");
    if (!l_tx) {
        PyErr_SetString(PyExc_ValueError, "Invalid transaction capsule");
        return NULL;
    }
    
    char *l_service_name = NULL;
    dap_chain_srv_uid_t l_uid = {0};
    dap_chain_tx_tag_action_type_t l_action = 0;
    
    bool l_result = dap_ledger_deduct_tx_tag(l_ledger, l_tx, &l_service_name, &l_uid, &l_action);
    if (!l_result) {
        log_it(L_DEBUG, "TX tag info not found");
        Py_RETURN_NONE;
    }
    
    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "service_name", l_service_name ? PyUnicode_FromString(l_service_name) : Py_None);
    PyDict_SetItemString(l_dict, "uid", PyLong_FromUnsignedLongLong(l_uid.uint64));
    PyDict_SetItemString(l_dict, "action", PyLong_FromLong(l_action));
    
    log_it(L_DEBUG, "Deducted TX tag info");
    return l_dict;
}

/**
 * @brief Get token ticker by transaction hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, tx_hash bytes)
 * @return Token ticker string or None
 */
PyObject* dap_ledger_tx_get_token_ticker_by_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_ledger_obj, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_chain_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_hash = (dap_chain_hash_fast_t *)l_hash_bytes;
    
    const char *l_ticker = dap_ledger_tx_get_token_ticker_by_hash(l_ledger, l_hash);
    if (!l_ticker) {
        log_it(L_DEBUG, "Token ticker not found for TX");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved token ticker: %s", l_ticker);
    return PyUnicode_FromString(l_ticker);
}


// Get method definitions for tx_lists module
PyMethodDef* cellframe_ledger_tx_lists_get_methods(void) {
    static PyMethodDef tx_lists_methods[] = {
        {"ledger_token_tx_item_list", (PyCFunction)dap_ledger_token_tx_item_list_py, METH_VARARGS,
         "Get list of transaction items for address as JSON"},
        {"ledger_get_txs", (PyCFunction)dap_ledger_get_txs_py, METH_VARARGS,
         "Get list of transactions"},
        {"ledger_tx_hash_is_used_out_item", (PyCFunction)dap_ledger_tx_hash_is_used_out_item_py, METH_VARARGS,
         "Check if TX output is used"},
        {"ledger_get_final_chain_tx_hash", (PyCFunction)dap_ledger_get_final_chain_tx_hash_py, METH_VARARGS,
         "Get final chain transaction hash"},
        {"ledger_get_first_chain_tx_hash", (PyCFunction)dap_ledger_get_first_chain_tx_hash_py, METH_VARARGS,
         "Get first chain transaction hash"},
        {"ledger_tx_check_recipient", (PyCFunction)dap_ledger_tx_check_recipient_py, METH_VARARGS,
         "Check if address is transaction recipient"},
        {"ledger_tx_poa_signed", (PyCFunction)dap_ledger_tx_poa_signed_py, METH_VARARGS,
         "Check if transaction is PoA signed"},
        {"ledger_tx_service_info", (PyCFunction)dap_ledger_tx_service_info_py, METH_VARARGS,
         "Get transaction service info"},
        {"ledger_deduct_tx_tag", (PyCFunction)dap_ledger_deduct_tx_tag_py, METH_VARARGS,
         "Deduct transaction tag info"},
        {"ledger_tx_get_token_ticker_by_hash", (PyCFunction)dap_ledger_tx_get_token_ticker_by_hash_py, METH_VARARGS,
         "Get token ticker by transaction hash"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return tx_lists_methods;
}
