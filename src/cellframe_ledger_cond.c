#include "cellframe_ledger_internal.h"

/*
 * Cellframe ledger conditional outputs bindings
 * Conditional outputs: get, find, list, verificator management
 */


// =============================================================================
// CONDITIONAL OUTPUTS OPERATIONS
// =============================================================================

/**
 * @brief Get first transaction conditional output
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, addr_from, subtype)
 * @return PyCapsule wrapping dap_chain_tx_used_out_item_t* or None
 */
PyObject* dap_ledger_get_tx_cond_out_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_addr_obj;
    int l_subtype;
    
    if (!PyArg_ParseTuple(a_args, "OOi", &l_ledger_obj, &l_addr_obj, &l_subtype)) {
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
    
    dap_chain_tx_used_out_item_t *l_out = dap_ledger_get_tx_cond_out(l_ledger, l_addr, 
                                                                       (dap_chain_tx_out_cond_subtype_t)l_subtype);
    if (!l_out) {
        log_it(L_DEBUG, "No conditional output found for subtype %d", l_subtype);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved conditional output for subtype %d", l_subtype);
    return PyCapsule_New(l_out, "dap_chain_tx_used_out_item_t", NULL);
}

/**
 * @brief Find unspent conditional output by address
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, token, subtype, addr, tx_first_hash)
 * @return Dict with out_cond (capsule) and out_idx (int) or None
 */
PyObject* dap_ledger_out_cond_unspent_find_by_addr_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_addr_obj, *l_hash_obj = NULL;
    const char *l_token;
    int l_subtype;
    
    if (!PyArg_ParseTuple(a_args, "OsiO|O", &l_ledger_obj, &l_token, &l_subtype, &l_addr_obj, &l_hash_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "Fourth argument must be an address capsule");
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
    
    dap_chain_hash_fast_t *l_hash = NULL;
    if (l_hash_obj && l_hash_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_hash_obj)) {
            PyErr_SetString(PyExc_TypeError, "tx_first_hash must be a hash capsule");
            return NULL;
        }
        l_hash = (dap_chain_hash_fast_t *)PyCapsule_GetPointer(l_hash_obj, "dap_hash_fast_t");
    }
    
    int l_out_idx = 0;
    dap_chain_tx_out_cond_t *l_out_cond = dap_ledger_out_cond_unspent_find_by_addr(
        l_ledger, l_token, (dap_chain_tx_out_cond_subtype_t)l_subtype, l_addr, l_hash, &l_out_idx
    );
    
    if (!l_out_cond) {
        log_it(L_DEBUG, "No unspent conditional output found");
        Py_RETURN_NONE;
    }
    
    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "out_cond", PyCapsule_New(l_out_cond, "dap_chain_tx_out_cond_t", NULL));
    PyDict_SetItemString(l_dict, "out_idx", PyLong_FromLong(l_out_idx));
    
    log_it(L_DEBUG, "Found unspent conditional output at index %d", l_out_idx);
    return l_dict;
}

/**
 * @brief Get list of conditional outputs
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, subtype, token_ticker, addr_from)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_ledger_get_list_tx_cond_outs_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_addr_obj;
    int l_subtype;
    const char *l_token_ticker;
    
    if (!PyArg_ParseTuple(a_args, "OisO", &l_ledger_obj, &l_subtype, &l_token_ticker, &l_addr_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "Fourth argument must be an address capsule");
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
    
    dap_list_t *l_list = dap_ledger_get_list_tx_cond_outs(l_ledger, (dap_chain_tx_out_cond_subtype_t)l_subtype,
                                                           l_token_ticker, l_addr);
    if (!l_list) {
        log_it(L_DEBUG, "No conditional outputs found for token %s", l_token_ticker);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved list of conditional outputs for token %s", l_token_ticker);
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

/**
 * @brief Find all conditional outputs from TX cache by service UID
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, srv_uid)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_ledger_tx_cache_find_out_cond_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    unsigned long long l_srv_uid;
    
    if (!PyArg_ParseTuple(a_args, "OK", &l_ledger_obj, &l_srv_uid)) {
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
    
    dap_chain_srv_uid_t l_uid = {.uint64 = l_srv_uid};
    
    dap_list_t *l_list = dap_ledger_tx_cache_find_out_cond_all(l_ledger, l_uid);
    if (!l_list) {
        log_it(L_DEBUG, "No conditional outputs found in cache for service UID %llu", l_srv_uid);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved conditional outputs from cache for service UID %llu", l_srv_uid);
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

/**
 * @brief Check condition owner
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, tx_hash bytes, cond_subtype, out_idx, owner_sign)
 * @return True if owner is valid, False otherwise
 */
PyObject* dap_ledger_check_condition_owner_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_sign_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    int l_cond_subtype, l_out_idx;
    
    if (!PyArg_ParseTuple(a_args, "Os#iiO", &l_ledger_obj, &l_hash_bytes, &l_hash_size, 
                          &l_cond_subtype, &l_out_idx, &l_sign_obj)) {
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
    
    if (!PyCapsule_CheckExact(l_sign_obj)) {
        PyErr_SetString(PyExc_TypeError, "Fifth argument must be a signature capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_hash_bytes;
    dap_sign_t *l_sign = (dap_sign_t *)PyCapsule_GetPointer(l_sign_obj, "dap_sign_t");
    if (!l_sign) {
        PyErr_SetString(PyExc_ValueError, "Invalid signature capsule");
        return NULL;
    }
    
    bool l_result = dap_ledger_check_condition_owner(l_ledger, l_hash, 
                                                      (dap_chain_tx_out_cond_subtype_t)l_cond_subtype,
                                                      l_out_idx, l_sign);
    
    log_it(L_DEBUG, "Condition owner check result: %s", l_result ? "valid" : "invalid");
    return PyBool_FromLong(l_result);
}

/**
 * @brief Add verificator callbacks for conditional outputs
 * @note Callback functionality requires complex implementation - stub
 * @param a_self Python self object (unused)
 * @param a_args Arguments (subtype)
 * @return Integer result code
 */
PyObject* dap_ledger_verificator_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_subtype;
    
    if (!PyArg_ParseTuple(a_args, "i", &l_subtype)) {
        return NULL;
    }
    
    // TODO: Implement full Python callback wrapper with GIL management
    // This requires 6 callback functions for verify/add/delete operations
    log_it(L_INFO, "Add verificator for subtype %d (stub - callbacks not yet implemented)", l_subtype);
    
    return PyLong_FromLong(0);  // Success
}

/**
 * @brief Add voting verificator callbacks
 * @note Callback functionality requires complex implementation - stub
 * @param a_self Python self object (unused)
 * @param a_args Arguments ()
 * @return Integer result code
 */
PyObject* dap_ledger_voting_verificator_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    
    if (!PyArg_ParseTuple(a_args, "")) {
        return NULL;
    }
    
    // TODO: Implement full Python callback wrapper with GIL management
    // This requires 4 callback functions for voting operations
    log_it(L_INFO, "Add voting verificator (stub - callbacks not yet implemented)");
    
    return PyLong_FromLong(0);  // Success
}


// Get method definitions for cond module
PyMethodDef* cellframe_ledger_cond_get_methods(void) {
    static PyMethodDef cond_methods[] = {
        {"ledger_get_tx_cond_out", (PyCFunction)dap_ledger_get_tx_cond_out_py, METH_VARARGS,
         "Get first transaction conditional output"},
        {"ledger_out_cond_unspent_find_by_addr", (PyCFunction)dap_ledger_out_cond_unspent_find_by_addr_py, METH_VARARGS,
         "Find unspent conditional output by address"},
        {"ledger_get_list_tx_cond_outs", (PyCFunction)dap_ledger_get_list_tx_cond_outs_py, METH_VARARGS,
         "Get list of conditional outputs"},
        {"ledger_tx_cache_find_out_cond_all", (PyCFunction)dap_ledger_tx_cache_find_out_cond_all_py, METH_VARARGS,
         "Find all conditional outputs from TX cache by service UID"},
        {"ledger_check_condition_owner", (PyCFunction)dap_ledger_check_condition_owner_py, METH_VARARGS,
         "Check condition owner"},
        {"ledger_verificator_add", (PyCFunction)dap_ledger_verificator_add_py, METH_VARARGS,
         "Add verificator callbacks for conditional outputs (stub)"},
        {"ledger_voting_verificator_add", (PyCFunction)dap_ledger_voting_verificator_add_py, METH_VARARGS,
         "Add voting verificator callbacks (stub)"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return cond_methods;
}
