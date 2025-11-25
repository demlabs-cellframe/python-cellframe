#include "cellframe_ledger_internal.h"

/*
 * Cellframe ledger bindings for Python API
 * Provides full access to ledger operations: TX, tokens, emissions, balance calculations
 */

#define LOG_TAG "python_cellframe_ledger"


// =============================================================================
// TOKEN AND BALANCE INFO OPERATIONS (JSON)
// =============================================================================

/**
 * @brief Get token info as JSON (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, limit, offset, version)
 * @return JSON object capsule or None
 */
PyObject* dap_ledger_token_info_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    size_t l_limit = 0;
    size_t l_offset = 0;
    int l_version = 0;
    
    if (!PyArg_ParseTuple(a_args, "Onni", &l_ledger_capsule, &l_limit, &l_offset, &l_version)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_info");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to extract ledger pointer");
        return NULL;
    }
    
    dap_json_t *l_json = dap_ledger_token_info(l_ledger, l_limit, l_offset, l_version);
    if (!l_json) {
        log_it(L_DEBUG, "No token info available");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved token info (limit=%zu, offset=%zu)", l_limit, l_offset);
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

/**
 * @brief Get token info by name as JSON (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, token_ticker, version)
 * @return JSON object capsule or None
 */
PyObject* dap_ledger_token_info_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_token_ticker = NULL;
    int l_version = 0;
    
    if (!PyArg_ParseTuple(a_args, "Osi", &l_ledger_capsule, &l_token_ticker, &l_version)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_info_by_name");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to extract ledger pointer");
        return NULL;
    }
    
    dap_json_t *l_json = dap_ledger_token_info_by_name(l_ledger, l_token_ticker, l_version);
    if (!l_json) {
        log_it(L_DEBUG, "Token '%s' info not found", l_token_ticker);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved token info for '%s'", l_token_ticker);
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

/**
 * @brief Get balance info as JSON (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, limit, offset, head, version)
 * @return JSON object capsule or None
 */
PyObject* dap_ledger_balance_info_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    size_t l_limit = 0;
    size_t l_offset = 0;
    int l_head = 0;
    int l_version = 0;
    
    if (!PyArg_ParseTuple(a_args, "Onnpi", &l_ledger_capsule, &l_limit, &l_offset, &l_head, &l_version)) {
        log_it(L_ERROR, "Invalid arguments for ledger_balance_info");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to extract ledger pointer");
        return NULL;
    }
    
    dap_json_t *l_json = dap_ledger_balance_info(l_ledger, l_limit, l_offset, (bool)l_head, l_version);
    if (!l_json) {
        log_it(L_DEBUG, "No balance info available");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved balance info");
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

/**
 * @brief Get threshold info as JSON (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, limit, offset, threshold_hash, head, version)
 * @return JSON object capsule or None
 */
PyObject* dap_ledger_threshold_info_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    size_t l_limit = 0;
    size_t l_offset = 0;
    PyObject *l_hash_capsule = Py_None;
    int l_head = 0;
    int l_version = 0;
    
    if (!PyArg_ParseTuple(a_args, "OnnOpi", &l_ledger_capsule, &l_limit, &l_offset, &l_hash_capsule, &l_head, &l_version)) {
        log_it(L_ERROR, "Invalid arguments for ledger_threshold_info");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to extract ledger pointer");
        return NULL;
    }
    
    dap_hash_fast_t *l_threshold_hash = NULL;
    if (l_hash_capsule && l_hash_capsule != Py_None) {
        l_threshold_hash = (dap_hash_fast_t*)PyCapsule_GetPointer(l_hash_capsule, "dap_hash_fast_t");
    }
    
    dap_json_t *l_json = dap_ledger_threshold_info(l_ledger, l_limit, l_offset, l_threshold_hash, (bool)l_head, l_version);
    if (!l_json) {
        log_it(L_DEBUG, "No threshold info available");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved threshold info");
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

// =============================================================================
// BALANCE & FIND OPERATIONS (CRITICAL!)
// =============================================================================

/**
 * @brief Calculate balance for address
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, addr capsule, token_ticker string)
 * @return Balance as bytes (uint256_t) or NULL on error
 */
PyObject* dap_ledger_calc_balance_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_addr_obj;
    const char *l_token_ticker;
    
    if (!PyArg_ParseTuple(a_args, "OOs", &l_ledger_obj, &l_addr_obj, &l_token_ticker)) {
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
    
    uint256_t l_balance = dap_ledger_calc_balance(l_ledger, l_addr, l_token_ticker);
    
    log_it(L_DEBUG, "Calculated balance for token %s", l_token_ticker);
    return PyBytes_FromStringAndSize((const char *)&l_balance, sizeof(uint256_t));
}

/**
 * @brief Calculate full balance for address (including all states)
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, addr capsule, token_ticker string)
 * @return Balance as bytes (uint256_t) or NULL on error
 */
PyObject* dap_ledger_calc_balance_full_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_addr_obj;
    const char *l_token_ticker;
    
    if (!PyArg_ParseTuple(a_args, "OOs", &l_ledger_obj, &l_addr_obj, &l_token_ticker)) {
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
    
    uint256_t l_balance = dap_ledger_calc_balance_full(l_ledger, l_addr, l_token_ticker);
    
    log_it(L_DEBUG, "Calculated full balance for token %s", l_token_ticker);
    return PyBytes_FromStringAndSize((const char *)&l_balance, sizeof(uint256_t));
}

/**
 * @brief Find transaction by hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, tx_hash bytes)
 * @return PyCapsule wrapping dap_chain_datum_tx_t* or None if not found
 */
PyObject* dap_ledger_tx_find_by_hash_py(PyObject *a_self, PyObject *a_args) {
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
    
    dap_chain_datum_tx_t *l_tx = dap_ledger_tx_find_by_hash(l_ledger, l_hash);
    if (!l_tx) {
        log_it(L_DEBUG, "Transaction not found by hash");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Transaction found by hash");
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

/**
 * @brief Find unspent transaction by hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, tx_hash bytes)
 * @return PyCapsule wrapping dap_chain_datum_tx_t* or None if not found
 */
PyObject* dap_ledger_tx_unspent_find_by_hash_py(PyObject *a_self, PyObject *a_args) {
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
    
    dap_chain_datum_tx_t *l_tx = dap_ledger_tx_unspent_find_by_hash(l_ledger, l_hash);
    if (!l_tx) {
        log_it(L_DEBUG, "Unspent transaction not found by hash");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Unspent transaction found by hash");
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

/**
 * @brief Find transaction by address
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, token, addr, tx_first_hash, unspent_only)
 * @return PyCapsule wrapping dap_chain_datum_tx_t* or None if not found
 */
PyObject* dap_ledger_tx_find_by_addr_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_addr_obj, *l_first_hash_obj = NULL;
    const char *l_token;
    int l_unspent_only = 0;
    
    if (!PyArg_ParseTuple(a_args, "OsO|Op", &l_ledger_obj, &l_token, &l_addr_obj, 
                          &l_first_hash_obj, &l_unspent_only)) {
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

    dap_chain_hash_fast_t *l_first_hash = NULL;
    if (l_first_hash_obj && l_first_hash_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_first_hash_obj)) {
            PyErr_SetString(PyExc_TypeError, "tx_first_hash must be a hash capsule");
            return NULL;
        }
        l_first_hash = (dap_chain_hash_fast_t *)PyCapsule_GetPointer(l_first_hash_obj, "dap_hash_fast_t");
    }
    
    dap_chain_datum_tx_t *l_tx = dap_ledger_tx_find_by_addr(l_ledger, l_token, l_addr, 
                                                             l_first_hash, (bool)l_unspent_only);
    if (!l_tx) {
        log_it(L_DEBUG, "Transaction not found by address for token %s", l_token);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Transaction found by address for token %s", l_token);
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}


// Get method definitions for balance module
PyMethodDef* cellframe_ledger_balance_get_methods(void) {
    static PyMethodDef balance_methods[] = {
        {"ledger_token_info", (PyCFunction)dap_ledger_token_info_py, METH_VARARGS,
         "Get token info as JSON"},
        {"ledger_token_info_by_name", (PyCFunction)dap_ledger_token_info_by_name_py, METH_VARARGS,
         "Get token info by name as JSON"},
        {"ledger_balance_info", (PyCFunction)dap_ledger_balance_info_py, METH_VARARGS,
         "Get balance info as JSON"},
        {"ledger_threshold_info", (PyCFunction)dap_ledger_threshold_info_py, METH_VARARGS,
         "Get threshold info as JSON"},
        {"ledger_calc_balance", (PyCFunction)dap_ledger_calc_balance_py, METH_VARARGS,
         "Calculate balance for address and token"},
        {"ledger_calc_balance_full", (PyCFunction)dap_ledger_calc_balance_full_py, METH_VARARGS,
         "Calculate full balance (including all states) for address and token"},
        {"ledger_tx_find_by_hash", (PyCFunction)dap_ledger_tx_find_by_hash_py, METH_VARARGS,
         "Find transaction by hash"},
        {"ledger_tx_unspent_find_by_hash", (PyCFunction)dap_ledger_tx_unspent_find_by_hash_py, METH_VARARGS,
         "Find unspent transaction by hash"},
        {"ledger_tx_find_by_addr", (PyCFunction)dap_ledger_tx_find_by_addr_py, METH_VARARGS,
         "Find transaction by address"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return balance_methods;
}
