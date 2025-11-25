#include "cellframe_ledger_internal.h"

/*
 * Cellframe ledger utilities bindings
 * Utility functions: iterators, purges, cache, time, trackers, aggregation
 */


// =============================================================================
// DATUM ITERATOR OPERATIONS
// =============================================================================

/**
 * @brief Create datum iterator
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain_net capsule)
 * @return PyCapsule wrapping dap_ledger_datum_iter_t* or NULL on error
 */
PyObject* dap_ledger_datum_iter_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain_net capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain_net capsule");
        return NULL;
    }
    
    dap_ledger_datum_iter_t *l_iter = dap_ledger_datum_iter_create(l_net);
    if (!l_iter) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create datum iterator");
        return NULL;
    }
    
    log_it(L_DEBUG, "Created datum iterator");
    return PyCapsule_New(l_iter, "dap_ledger_datum_iter_t", NULL);
}

/**
 * @brief Delete datum iterator
 * @param a_self Python self object (unused)
 * @param a_args Arguments (iterator capsule)
 * @return None
 */
PyObject* dap_ledger_datum_iter_delete_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_iter_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_iter_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_iter_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an iterator capsule");
        return NULL;
    }
    
    dap_ledger_datum_iter_t *l_iter = (dap_ledger_datum_iter_t *)PyCapsule_GetPointer(l_iter_obj, "dap_ledger_datum_iter_t");
    if (!l_iter) {
        PyErr_SetString(PyExc_ValueError, "Invalid iterator capsule");
        return NULL;
    }
    
    dap_ledger_datum_iter_delete(l_iter);
    
    log_it(L_DEBUG, "Deleted datum iterator");
    Py_RETURN_NONE;
}

/**
 * @brief Get first datum from iterator
 * @param a_self Python self object (unused)
 * @param a_args Arguments (iterator capsule)
 * @return PyCapsule wrapping dap_chain_datum_tx_t* or None
 */
PyObject* dap_ledger_datum_iter_get_first_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_iter_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_iter_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_iter_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an iterator capsule");
        return NULL;
    }
    
    dap_ledger_datum_iter_t *l_iter = (dap_ledger_datum_iter_t *)PyCapsule_GetPointer(l_iter_obj, "dap_ledger_datum_iter_t");
    if (!l_iter) {
        PyErr_SetString(PyExc_ValueError, "Invalid iterator capsule");
        return NULL;
    }
    
    dap_chain_datum_tx_t *l_tx = dap_ledger_datum_iter_get_first(l_iter);
    if (!l_tx) {
        log_it(L_DEBUG, "No first datum in iterator");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved first datum from iterator");
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

/**
 * @brief Get next datum from iterator
 * @param a_self Python self object (unused)
 * @param a_args Arguments (iterator capsule)
 * @return PyCapsule wrapping dap_chain_datum_tx_t* or None
 */
PyObject* dap_ledger_datum_iter_get_next_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_iter_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_iter_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_iter_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an iterator capsule");
        return NULL;
    }
    
    dap_ledger_datum_iter_t *l_iter = (dap_ledger_datum_iter_t *)PyCapsule_GetPointer(l_iter_obj, "dap_ledger_datum_iter_t");
    if (!l_iter) {
        PyErr_SetString(PyExc_ValueError, "Invalid iterator capsule");
        return NULL;
    }
    
    dap_chain_datum_tx_t *l_tx = dap_ledger_datum_iter_get_next(l_iter);
    if (!l_tx) {
        log_it(L_DEBUG, "No next datum in iterator");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved next datum from iterator");
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

/**
 * @brief Get last datum from iterator
 * @param a_self Python self object (unused)
 * @param a_args Arguments (iterator capsule)
 * @return PyCapsule wrapping dap_chain_datum_tx_t* or None
 */
PyObject* dap_ledger_datum_iter_get_last_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_iter_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_iter_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_iter_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an iterator capsule");
        return NULL;
    }
    
    dap_ledger_datum_iter_t *l_iter = (dap_ledger_datum_iter_t *)PyCapsule_GetPointer(l_iter_obj, "dap_ledger_datum_iter_t");
    if (!l_iter) {
        PyErr_SetString(PyExc_ValueError, "Invalid iterator capsule");
        return NULL;
    }
    
    dap_chain_datum_tx_t *l_tx = dap_ledger_datum_iter_get_last(l_iter);
    if (!l_tx) {
        log_it(L_DEBUG, "No last datum in iterator");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved last datum from iterator");
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

// =============================================================================
// CHAIN PURGE & MISC OPERATIONS
// =============================================================================

/**
 * @brief Purge chain data from ledger
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, atom_size)
 * @return Integer result code
 */
PyObject* dap_ledger_chain_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    Py_ssize_t l_atom_size;
    
    if (!PyArg_ParseTuple(a_args, "On", &l_chain_obj, &l_atom_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    int l_result = dap_ledger_chain_purge(l_chain, (size_t)l_atom_size);
    
    log_it(L_DEBUG, "Chain purge result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Purge decree data from ledger
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return Integer result code
 */
PyObject* dap_ledger_decree_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_ledger_obj)) {
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
    
    int l_result = dap_ledger_decree_purge(l_ledger);
    
    log_it(L_DEBUG, "Decree purge result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Purge anchor data from ledger
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, chain_id)
 * @return Integer result code
 */
PyObject* dap_ledger_anchor_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    unsigned long long l_chain_id;
    
    if (!PyArg_ParseTuple(a_args, "OK", &l_ledger_obj, &l_chain_id)) {
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
    
    dap_chain_id_t l_chain_id_union = {.uint64 = l_chain_id};
    int l_result = dap_ledger_anchor_purge(l_ledger, l_chain_id_union);
    
    log_it(L_DEBUG, "Anchor purge result for chain_id %llu: %d", l_chain_id, l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Check if ledger cache is enabled
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return True if cache enabled, False otherwise
 */
PyObject* dap_ledger_cache_enabled_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_ledger_obj)) {
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
    
    bool l_result = dap_ledger_cache_enabled(l_ledger);
    
    log_it(L_DEBUG, "Ledger cache enabled: %s", l_result ? "true" : "false");
    return PyBool_FromLong(l_result);
}

/**
 * @brief Set cache TX check callback
 * @note Callback functionality requires complex implementation - stub
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return None
 */
PyObject* dap_ledger_set_cache_tx_check_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_ledger_obj)) {
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
    
    // TODO: Implement full Python callback wrapper with GIL management
    log_it(L_INFO, "Set cache TX check callback (stub - callback not yet implemented)");
    
    Py_RETURN_NONE;
}

/**
 * @brief Signal that ledger loading is complete
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return None
 */
PyObject* dap_ledger_load_end_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_ledger_obj)) {
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
    
    dap_ledger_load_end(l_ledger);
    
    log_it(L_DEBUG, "Ledger load end signaled");
    Py_RETURN_NONE;
}

// =============================================================================
// UTILITY & SERVICE FUNCTIONS
// =============================================================================

/**
 * @brief Count transactions from timestamp range
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, ts_from uint64, ts_to uint64)
 * @return Integer count of transactions
 */
PyObject* dap_ledger_count_from_to_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    unsigned long long l_ts_from, l_ts_to;
    
    if (!PyArg_ParseTuple(a_args, "OKK", &l_ledger_obj, &l_ts_from, &l_ts_to)) {
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
    
    uint64_t l_count = dap_ledger_count_from_to(l_ledger, l_ts_from, l_ts_to);
    
    log_it(L_DEBUG, "TX count from %lu to %lu: %lu", (unsigned long)l_ts_from, (unsigned long)l_ts_to, (unsigned long)l_count);
    return PyLong_FromUnsignedLongLong(l_count);
}

/**
 * @brief Check if datum is enforced
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, hash bytes, accept bool)
 * @return Boolean
 */
PyObject* dap_ledger_datum_is_enforced_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    int l_accept;
    
    if (!PyArg_ParseTuple(a_args, "Os#p", &l_ledger_obj, &l_hash_bytes, &l_hash_size, &l_accept)) {
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
    bool l_result = dap_ledger_datum_is_enforced(l_ledger, l_hash, (bool)l_accept);
    
    log_it(L_DEBUG, "Datum is_enforced: %s", l_result ? "true" : "false");
    return PyBool_FromLong(l_result);
}

/**
 * @brief Get blockchain time
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule)
 * @return Integer timestamp
 */
PyObject* dap_ledger_get_blockchain_time_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_ledger_obj)) {
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
    
    dap_time_t l_time = dap_ledger_get_blockchain_time(l_ledger);
    
    log_it(L_DEBUG, "Blockchain time: %llu", (unsigned long long)l_time);
    return PyLong_FromUnsignedLongLong(l_time);
}

/**
 * @brief Get transaction action string
 * @param a_self Python self object (unused)
 * @param a_args Arguments (action type integer)
 * @return String or None
 */
PyObject* dap_ledger_tx_action_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_action_type;
    
    if (!PyArg_ParseTuple(a_args, "i", &l_action_type)) {
        return NULL;
    }
    
    const char *l_str = dap_ledger_tx_action_str((dap_chain_tx_tag_action_type_t)l_action_type);
    if (!l_str) {
        log_it(L_DEBUG, "Unknown action type: %d", l_action_type);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Action type %d -> '%s'", l_action_type, l_str);
    return PyUnicode_FromString(l_str);
}

/**
 * @brief Convert action string to action type
 * @param a_self Python self object (unused)
 * @param a_args Arguments (action string)
 * @return Integer action type
 */
PyObject* dap_ledger_tx_action_str_to_action_t_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_str;
    
    if (!PyArg_ParseTuple(a_args, "s", &l_str)) {
        return NULL;
    }
    
    dap_chain_tx_tag_action_type_t l_action = dap_ledger_tx_action_str_to_action_t(l_str);
    
    log_it(L_DEBUG, "Action string '%s' -> type %d", l_str, l_action);
    return PyLong_FromLong(l_action);
}

/**
 * @brief Get tag string by service UID
 * @param a_self Python self object (unused)
 * @param a_args Arguments (service_uid uint64)
 * @return String or None
 */
PyObject* dap_ledger_tx_tag_str_by_uid_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_uid;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_uid)) {
        return NULL;
    }
    
    dap_chain_srv_uid_t l_service_uid = {.uint64 = l_uid};
    const char *l_str = dap_ledger_tx_tag_str_by_uid(l_service_uid);
    if (!l_str) {
        log_it(L_DEBUG, "No tag for service UID %llu", l_uid);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Service UID %llu -> tag '%s'", l_uid, l_str);
    return PyUnicode_FromString(l_str);
}

/**
 * @brief Calculate main ticker for transaction
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, tx capsule)
 * @return Tuple (ticker string, result_code) or None
 */
PyObject* dap_ledger_tx_calculate_main_ticker_py(PyObject *a_self, PyObject *a_args) {
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
        PyErr_SetString(PyExc_TypeError, "Second argument must be a tx capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t *)PyCapsule_GetPointer(l_tx_obj, "dap_chain_datum_tx_t");
    
    if (!l_ledger || !l_tx) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or tx capsule");
        return NULL;
    }
    
    int l_rc = 0;
    dap_chain_token_ticker_str_t l_ticker = dap_ledger_tx_calculate_main_ticker_(l_ledger, l_tx, &l_rc);
    
    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyUnicode_FromString(l_ticker.s));
    PyTuple_SetItem(l_tuple, 1, PyLong_FromLong(l_rc));
    
    log_it(L_DEBUG, "Calculated main ticker: '%s', rc: %d", l_ticker.s, l_rc);
    return l_tuple;
}

/**
 * @brief Get list of TX outputs from JSON
 * @param a_self Python self object (unused)
 * @param a_args Arguments (json capsule, outputs_count, value_need bytes, need_all_outputs bool)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_ledger_get_list_tx_outs_from_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_json_obj;
    int l_outputs_count;
    const char *l_value_need_bytes;
    Py_ssize_t l_value_need_size;
    int l_need_all_outputs;
    
    // Note: value_transfer is output parameter, for simplicity we skip it in Python binding
    if (!PyArg_ParseTuple(a_args, "Ois#p", &l_json_obj, &l_outputs_count, 
                          &l_value_need_bytes, &l_value_need_size, &l_need_all_outputs)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_json_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a JSON capsule");
        return NULL;
    }
    
    if ((size_t)l_value_need_size != sizeof(uint256_t)) {
        PyErr_Format(PyExc_ValueError, "value_need must be exactly %zu bytes", sizeof(uint256_t));
        return NULL;
    }
    
    dap_json_t *l_json = (dap_json_t *)PyCapsule_GetPointer(l_json_obj, "dap_json_t");
    if (!l_json) {
        PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
        return NULL;
    }
    
    uint256_t l_value_need = *(uint256_t *)l_value_need_bytes;
    uint256_t l_value_transfer = uint256_0;
    
    dap_list_t *l_list = dap_ledger_get_list_tx_outs_from_json(l_json, l_outputs_count,
                                                                 l_value_need, &l_value_transfer,
                                                                 (bool)l_need_all_outputs);
    if (!l_list) {
        log_it(L_DEBUG, "No TX outs from JSON");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Got TX outs list from JSON (need_all=%d)", l_need_all_outputs);
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

// =============================================================================
// TRACKER & COLOR OPERATIONS
// =============================================================================

/**
 * @brief Get uncoloured value of a coin
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, voting_hash, tx_hash, out_idx, pkey_hash)
 * @return Bytes representing uint256_t
 */
PyObject* dap_ledger_coin_get_uncoloured_value_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_voting_hash_bytes, *l_tx_hash_bytes, *l_pkey_hash_bytes;
    Py_ssize_t l_voting_hash_size, l_tx_hash_size, l_pkey_hash_size;
    int l_out_idx;
    
    if (!PyArg_ParseTuple(a_args, "Os#s#is#", &l_ledger_obj, 
                          &l_voting_hash_bytes, &l_voting_hash_size,
                          &l_tx_hash_bytes, &l_tx_hash_size,
                          &l_out_idx,
                          &l_pkey_hash_bytes, &l_pkey_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_voting_hash_size != sizeof(dap_hash_fast_t) ||
        (size_t)l_tx_hash_size != sizeof(dap_hash_fast_t) ||
        (size_t)l_pkey_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "All hashes must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_voting_hash = (dap_hash_fast_t *)l_voting_hash_bytes;
    dap_hash_fast_t *l_tx_hash = (dap_hash_fast_t *)l_tx_hash_bytes;
    dap_hash_fast_t *l_pkey_hash = (dap_hash_fast_t *)l_pkey_hash_bytes;
    
    uint256_t l_value = dap_ledger_coin_get_uncoloured_value(l_ledger, l_voting_hash, 
                                                              l_tx_hash, l_out_idx, l_pkey_hash);
    
    log_it(L_DEBUG, "Got uncoloured value for coin (out_idx=%d)", l_out_idx);
    return PyBytes_FromStringAndSize((const char *)&l_value, sizeof(uint256_t));
}

/**
 * @brief Get transaction trackers
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, tx_hash bytes, out_idx uint32)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_ledger_tx_get_trackers_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_tx_hash_bytes;
    Py_ssize_t l_tx_hash_size;
    unsigned int l_out_idx;
    
    if (!PyArg_ParseTuple(a_args, "Os#I", &l_ledger_obj, &l_tx_hash_bytes, &l_tx_hash_size, &l_out_idx)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_tx_hash_size != sizeof(dap_chain_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "TX hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_tx_hash = (dap_chain_hash_fast_t *)l_tx_hash_bytes;
    
    dap_list_t *l_list = dap_ledger_tx_get_trackers(l_ledger, l_tx_hash, (uint32_t)l_out_idx);
    if (!l_list) {
        log_it(L_DEBUG, "No trackers for TX out_idx=%u", l_out_idx);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Got trackers for TX out_idx=%u", l_out_idx);
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

/**
 * @brief Clear transaction colour
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, tx_hash bytes)
 * @return None
 */
PyObject* dap_ledger_tx_clear_colour_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_tx_hash_bytes;
    Py_ssize_t l_tx_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_ledger_obj, &l_tx_hash_bytes, &l_tx_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_tx_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "TX hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_tx_hash = (dap_hash_fast_t *)l_tx_hash_bytes;
    
    dap_ledger_tx_clear_colour(l_ledger, l_tx_hash);
    
    log_it(L_DEBUG, "Cleared TX colour");
    Py_RETURN_NONE;
}

/**
 * @brief Colour clear callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (list_data pointer as capsule)
 * @return None
 */
PyObject* dap_ledger_colour_clear_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_list_data_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_list_data_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_list_data_obj)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a capsule");
        return NULL;
    }
    
    void *l_list_data = PyCapsule_GetPointer(l_list_data_obj, NULL);
    if (!l_list_data) {
        PyErr_SetString(PyExc_ValueError, "Invalid capsule");
        return NULL;
    }
    
    dap_ledger_colour_clear_callback(l_list_data);
    
    log_it(L_DEBUG, "Colour clear callback executed");
    Py_RETURN_NONE;
}

// =============================================================================
// AGGREGATION & DECREE OPERATIONS
// =============================================================================

/**
 * @brief Aggregate ledger states for hardfork
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, hardfork_decree_creation_time uint64)
 * @return PyCapsule wrapping dap_ledger_hardfork_balances_t* or None
 * Note: cond_outs_list output parameter is skipped for Python binding simplicity
 */
PyObject* dap_ledger_states_aggregate_py(PyObject *a_self, PyObject *a_args) {
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
    
    dap_ledger_hardfork_condouts_t *l_cond_outs = NULL;
    dap_ledger_hardfork_fees_t l_fees_list = {0};
    dap_ledger_hardfork_balances_t *l_balances = dap_ledger_states_aggregate(l_ledger, l_hardfork_time, &l_cond_outs, NULL, &l_fees_list);
    
    if (!l_balances) {
        log_it(L_DEBUG, "States aggregation returned NULL");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "States aggregated for hardfork time %llu", l_hardfork_time);
    return PyCapsule_New(l_balances, "dap_ledger_hardfork_balances_t", NULL);
}

/**
 * @brief Aggregate anchors for chain
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, chain_id uint64)
 * @return PyCapsule wrapping dap_ledger_hardfork_anchors_t* or None
 */
PyObject* dap_ledger_anchors_aggregate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    unsigned long long l_chain_id;
    
    if (!PyArg_ParseTuple(a_args, "OK", &l_ledger_obj, &l_chain_id)) {
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
    
    dap_chain_id_t l_chain_id_union = {.uint64 = l_chain_id};
    dap_ledger_hardfork_anchors_t *l_anchors = dap_ledger_anchors_aggregate(l_ledger, l_chain_id_union);
    
    if (!l_anchors) {
        log_it(L_DEBUG, "Anchors aggregation returned NULL for chain_id %llu", l_chain_id);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Anchors aggregated for chain_id %llu", l_chain_id);
    return PyCapsule_New(l_anchors, "dap_ledger_hardfork_anchors_t", NULL);
}

/**
 * @brief Aggregate events for chain
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, chain_id uint64)
 * @return PyCapsule wrapping dap_ledger_hardfork_events_t* or None
 */
PyObject* dap_ledger_events_aggregate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    unsigned long long l_chain_id;
    
    if (!PyArg_ParseTuple(a_args, "OK", &l_ledger_obj, &l_chain_id)) {
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
    
    dap_chain_id_t l_chain_id_union = {.uint64 = l_chain_id};
    dap_ledger_hardfork_events_t *l_events = dap_ledger_events_aggregate(l_ledger, l_chain_id_union);
    
    if (!l_events) {
        log_it(L_DEBUG, "Events aggregation returned NULL for chain_id %llu", l_chain_id);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Events aggregated for chain_id %llu", l_chain_id);
    return PyCapsule_New(l_events, "dap_ledger_hardfork_events_t", NULL);
}

/**
 * @brief Get decrees by type
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, decree_type integer)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_ledger_decrees_get_by_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    int l_type;
    
    if (!PyArg_ParseTuple(a_args, "Oi", &l_ledger_obj, &l_type)) {
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
    
    dap_list_t *l_list = dap_ledger_decrees_get_by_type(l_ledger, l_type);
    
    if (!l_list) {
        log_it(L_DEBUG, "No decrees found for type %d", l_type);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Got decrees list for type %d", l_type);
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}


// Get method definitions for utils module
PyMethodDef* cellframe_ledger_utils_get_methods(void) {
    static PyMethodDef utils_methods[] = {
        {"ledger_datum_iter_create", (PyCFunction)dap_ledger_datum_iter_create_py, METH_VARARGS,
         "Create datum iterator"},
        {"ledger_datum_iter_delete", (PyCFunction)dap_ledger_datum_iter_delete_py, METH_VARARGS,
         "Delete datum iterator"},
        {"ledger_datum_iter_get_first", (PyCFunction)dap_ledger_datum_iter_get_first_py, METH_VARARGS,
         "Get first datum from iterator"},
        {"ledger_datum_iter_get_next", (PyCFunction)dap_ledger_datum_iter_get_next_py, METH_VARARGS,
         "Get next datum from iterator"},
        {"ledger_datum_iter_get_last", (PyCFunction)dap_ledger_datum_iter_get_last_py, METH_VARARGS,
         "Get last datum from iterator"},
        {"ledger_chain_purge", (PyCFunction)dap_ledger_chain_purge_py, METH_VARARGS,
         "Purge chain data from ledger"},
        {"ledger_decree_purge", (PyCFunction)dap_ledger_decree_purge_py, METH_VARARGS,
         "Purge decree data from ledger"},
        {"ledger_anchor_purge", (PyCFunction)dap_ledger_anchor_purge_py, METH_VARARGS,
         "Purge anchor data from ledger"},
        {"ledger_cache_enabled", (PyCFunction)dap_ledger_cache_enabled_py, METH_VARARGS,
         "Check if ledger cache is enabled"},
        {"ledger_set_cache_tx_check_callback", (PyCFunction)dap_ledger_set_cache_tx_check_callback_py, METH_VARARGS,
         "Set cache TX check callback (stub)"},
        {"ledger_load_end", (PyCFunction)dap_ledger_load_end_py, METH_VARARGS,
         "Signal that ledger loading is complete"},
        {"ledger_datum_is_enforced", (PyCFunction)dap_ledger_datum_is_enforced_py, METH_VARARGS,
         "Check if datum is enforced"},
        {"ledger_get_blockchain_time", (PyCFunction)dap_ledger_get_blockchain_time_py, METH_VARARGS,
         "Get blockchain time"},
        {"ledger_tx_action_str", (PyCFunction)dap_ledger_tx_action_str_py, METH_VARARGS,
         "Get transaction action string"},
        {"ledger_tx_action_str_to_action_t", (PyCFunction)dap_ledger_tx_action_str_to_action_t_py, METH_VARARGS,
         "Convert action string to action type"},
        {"ledger_tx_tag_str_by_uid", (PyCFunction)dap_ledger_tx_tag_str_by_uid_py, METH_VARARGS,
         "Get tag string by service UID"},
        {"ledger_tx_calculate_main_ticker", (PyCFunction)dap_ledger_tx_calculate_main_ticker_py, METH_VARARGS,
         "Calculate main ticker for transaction"},
        {"ledger_get_list_tx_outs_from_json", (PyCFunction)dap_ledger_get_list_tx_outs_from_json_py, METH_VARARGS,
         "Get list of TX outputs from JSON"},
        {"ledger_coin_get_uncoloured_value", (PyCFunction)dap_ledger_coin_get_uncoloured_value_py, METH_VARARGS,
         "Get uncoloured value of a coin"},
        {"ledger_tx_get_trackers", (PyCFunction)dap_ledger_tx_get_trackers_py, METH_VARARGS,
         "Get transaction trackers"},
        {"ledger_tx_clear_colour", (PyCFunction)dap_ledger_tx_clear_colour_py, METH_VARARGS,
         "Clear transaction colour"},
        {"ledger_colour_clear_callback", (PyCFunction)dap_ledger_colour_clear_callback_py, METH_VARARGS,
         "Colour clear callback"},
        {"ledger_states_aggregate", (PyCFunction)dap_ledger_states_aggregate_py, METH_VARARGS,
         "Aggregate ledger states for hardfork"},
        {"ledger_anchors_aggregate", (PyCFunction)dap_ledger_anchors_aggregate_py, METH_VARARGS,
         "Aggregate anchors for chain"},
        {"ledger_events_aggregate", (PyCFunction)dap_ledger_events_aggregate_py, METH_VARARGS,
         "Aggregate events for chain"},
        {"ledger_decrees_get_by_type", (PyCFunction)dap_ledger_decrees_get_by_type_py, METH_VARARGS,
         "Get decrees by type"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return utils_methods;
}
