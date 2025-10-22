#include "cellframe.h"
#include "dap_chain_ledger.h"

/*
 * Cellframe ledger bindings for Python API
 * Provides full access to ledger operations: TX, tokens, emissions, balance calculations
 */

#define LOG_TAG "python_cellframe_ledger"

// =============================================================================
// TRANSACTION OPERATIONS
// =============================================================================

/**
 * @brief Add transaction to ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, tx capsule, tx_hash bytes, from_threshold bool)
 * @return 0 on success, error code on failure
 */
PyObject* dap_ledger_tx_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    PyObject *l_tx_capsule = NULL;
    const char *l_tx_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    int l_from_threshold = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOy#p", &l_ledger_capsule, &l_tx_capsule, &l_tx_hash_bytes, &l_hash_size, &l_from_threshold)) {
        log_it(L_ERROR, "Invalid arguments for ledger_tx_add");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (!l_tx_capsule || !PyCapsule_CheckExact(l_tx_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected transaction capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t*)PyCapsule_GetPointer(l_tx_capsule, "dap_chain_datum_tx_t");
    
    if (!l_ledger || !l_tx) {
        log_it(L_ERROR, "Failed to extract pointers from capsules");
        return NULL;
    }
    
    dap_hash_fast_t *l_tx_hash = (dap_hash_fast_t*)l_tx_hash_bytes;
    
    int l_result = dap_ledger_tx_add(l_ledger, l_tx, l_tx_hash, (bool)l_from_threshold, NULL);
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to add transaction to ledger: %d", l_result);
    } else {
        log_it(L_INFO, "Transaction added to ledger successfully");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Load transaction to ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, tx capsule, tx_hash bytes)
 * @return 0 on success, error code on failure
 */
PyObject* dap_ledger_tx_load_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    PyObject *l_tx_capsule = NULL;
    const char *l_tx_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOy#", &l_ledger_capsule, &l_tx_capsule, &l_tx_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for ledger_tx_load");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (!l_tx_capsule || !PyCapsule_CheckExact(l_tx_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected transaction capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t*)PyCapsule_GetPointer(l_tx_capsule, "dap_chain_datum_tx_t");
    
    if (!l_ledger || !l_tx) {
        log_it(L_ERROR, "Failed to extract pointers from capsules");
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_tx_hash = (dap_chain_hash_fast_t*)l_tx_hash_bytes;
    
    int l_result = dap_ledger_tx_load(l_ledger, l_tx, l_tx_hash, NULL);
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to load transaction to ledger: %d", l_result);
    } else {
        log_it(L_DEBUG, "Transaction loaded to ledger successfully");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Remove transaction from ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, tx capsule, tx_hash bytes)
 * @return 0 on success, error code on failure
 */
PyObject* dap_ledger_tx_remove_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    PyObject *l_tx_capsule = NULL;
    const char *l_tx_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOy#", &l_ledger_capsule, &l_tx_capsule, &l_tx_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for ledger_tx_remove");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (!l_tx_capsule || !PyCapsule_CheckExact(l_tx_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected transaction capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t*)PyCapsule_GetPointer(l_tx_capsule, "dap_chain_datum_tx_t");
    
    if (!l_ledger || !l_tx) {
        log_it(L_ERROR, "Failed to extract pointers from capsules");
        return NULL;
    }
    
    dap_hash_fast_t *l_tx_hash = (dap_hash_fast_t*)l_tx_hash_bytes;
    
    int l_result = dap_ledger_tx_remove(l_ledger, l_tx, l_tx_hash);
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to remove transaction from ledger: %d", l_result);
    } else {
        log_it(L_INFO, "Transaction removed from ledger successfully");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Check/validate transaction before adding (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, tx bytes, tx_hash bytes)
 * @return 0 if valid, error code otherwise
 */
PyObject* dap_ledger_tx_add_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_tx_bytes = NULL;
    Py_ssize_t l_tx_size = 0;
    const char *l_tx_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "Oy#y#", &l_ledger_capsule, &l_tx_bytes, &l_tx_size, &l_tx_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for ledger_tx_add_check");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to extract ledger pointer");
        return NULL;
    }
    
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t*)l_tx_bytes;
    dap_hash_fast_t *l_tx_hash = (dap_hash_fast_t*)l_tx_hash_bytes;
    
    int l_result = dap_ledger_tx_add_check(l_ledger, l_tx, (size_t)l_tx_size, l_tx_hash);
    
    if (l_result != 0) {
        log_it(L_DEBUG, "Transaction validation failed: %d", l_result);
    } else {
        log_it(L_DEBUG, "Transaction validation passed");
    }
    
    return Py_BuildValue("i", l_result);
}

// =============================================================================
// TOKEN OPERATIONS
// =============================================================================

/**
 * @brief Add token to ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, token bytes, creation_time)
 * @return 0 on success, error code on failure
 */
PyObject* dap_ledger_token_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_token_bytes = NULL;
    Py_ssize_t l_token_size = 0;
    unsigned long long l_creation_time = 0;
    
    if (!PyArg_ParseTuple(a_args, "Oy#K", &l_ledger_capsule, &l_token_bytes, &l_token_size, &l_creation_time)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_add");
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
    
    int l_result = dap_ledger_token_add(l_ledger, (byte_t*)l_token_bytes, (size_t)l_token_size, (dap_time_t)l_creation_time);
    
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to add token to ledger: %d", l_result);
    } else {
        log_it(L_INFO, "Token added to ledger successfully");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Load token to ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, token bytes, creation_time)
 * @return 0 on success, error code on failure
 */
PyObject* dap_ledger_token_load_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_token_bytes = NULL;
    Py_ssize_t l_token_size = 0;
    unsigned long long l_creation_time = 0;
    
    if (!PyArg_ParseTuple(a_args, "Oy#K", &l_ledger_capsule, &l_token_bytes, &l_token_size, &l_creation_time)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_load");
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
    
    int l_result = dap_ledger_token_load(l_ledger, (byte_t*)l_token_bytes, (size_t)l_token_size, (dap_time_t)l_creation_time);
    
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to load token to ledger: %d", l_result);
    } else {
        log_it(L_DEBUG, "Token loaded to ledger successfully");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Check/validate token before adding (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, token bytes)
 * @return 0 if valid, error code otherwise
 */
PyObject* dap_ledger_token_add_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_token_bytes = NULL;
    Py_ssize_t l_token_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "Oy#", &l_ledger_capsule, &l_token_bytes, &l_token_size)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_add_check");
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
    
    int l_result = dap_ledger_token_add_check(l_ledger, (byte_t*)l_token_bytes, (size_t)l_token_size);
    
    if (l_result != 0) {
        log_it(L_DEBUG, "Token validation failed: %d", l_result);
    } else {
        log_it(L_DEBUG, "Token validation passed");
    }
    
    return Py_BuildValue("i", l_result);
}



// =============================================================================
// TOKEN EMISSION OPERATIONS
// =============================================================================

/**
 * @brief Add token emission to ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, emission bytes, emission_hash bytes)
 * @return 0 on success, error code on failure
 */
PyObject* dap_ledger_token_emission_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_emission_bytes = NULL;
    Py_ssize_t l_emission_size = 0;
    const char *l_emission_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "Oy#y#", &l_ledger_capsule, &l_emission_bytes, &l_emission_size, &l_emission_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_emission_add");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to extract ledger pointer");
        return NULL;
    }
    
    dap_hash_fast_t *l_emission_hash = (dap_hash_fast_t*)l_emission_hash_bytes;
    
    int l_result = dap_ledger_token_emission_add(l_ledger, (byte_t*)l_emission_bytes, (size_t)l_emission_size, l_emission_hash);
    
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to add token emission to ledger: %d", l_result);
    } else {
        log_it(L_INFO, "Token emission added to ledger successfully");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Load token emission to ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, emission bytes, emission_hash bytes)
 * @return 0 on success, error code on failure
 */
PyObject* dap_ledger_token_emission_load_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_emission_bytes = NULL;
    Py_ssize_t l_emission_size = 0;
    const char *l_emission_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "Oy#y#", &l_ledger_capsule, &l_emission_bytes, &l_emission_size, &l_emission_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_emission_load");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to extract ledger pointer");
        return NULL;
    }
    
    dap_hash_fast_t *l_emission_hash = (dap_hash_fast_t*)l_emission_hash_bytes;
    
    int l_result = dap_ledger_token_emission_load(l_ledger, (byte_t*)l_emission_bytes, (size_t)l_emission_size, l_emission_hash);
    
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to load token emission to ledger: %d", l_result);
    } else {
        log_it(L_DEBUG, "Token emission loaded to ledger successfully");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Check/validate token emission before adding (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, emission bytes, emission_hash bytes)
 * @return 0 if valid, error code otherwise
 */
PyObject* dap_ledger_token_emission_add_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_emission_bytes = NULL;
    Py_ssize_t l_emission_size = 0;
    const char *l_emission_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "Oy#y#", &l_ledger_capsule, &l_emission_bytes, &l_emission_size, &l_emission_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_emission_add_check");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to extract ledger pointer");
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_emission_hash = (dap_chain_hash_fast_t*)l_emission_hash_bytes;
    
    int l_result = dap_ledger_token_emission_add_check(l_ledger, (byte_t*)l_emission_bytes, (size_t)l_emission_size, l_emission_hash);
    
    if (l_result != 0) {
        log_it(L_DEBUG, "Token emission validation failed: %d", l_result);
    } else {
        log_it(L_DEBUG, "Token emission validation passed");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Purge ledger data (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, preserve_db bool)
 * @return None
 */
PyObject* dap_ledger_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    int l_preserve_db = 0;
    
    if (!PyArg_ParseTuple(a_args, "Op", &l_ledger_capsule, &l_preserve_db)) {
        log_it(L_ERROR, "Invalid arguments for ledger_purge");
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
    
    log_it(L_INFO, "Purging ledger (preserve_db=%d)", l_preserve_db);
    dap_ledger_purge(l_ledger, (bool)l_preserve_db);
    
    Py_RETURN_NONE;
}

/**
 * @brief Get ledger transaction count (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule)
 * @return Transaction count as unsigned int
 */
PyObject* dap_ledger_count_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_ledger_capsule)) {
        log_it(L_ERROR, "Invalid arguments for ledger_count");
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
    
    unsigned l_count = dap_ledger_count(l_ledger);
    log_it(L_DEBUG, "Ledger contains %u transactions", l_count);
    
    return Py_BuildValue("I", l_count);
}

// =============================================================================
// TOKEN INFO AND QUERY OPERATIONS
// =============================================================================

/**
 * @brief Check token ticker and get token datum (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, token_ticker string)
 * @return Token datum capsule or None if not found
 */
PyObject* dap_ledger_token_ticker_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_token_ticker = NULL;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_ledger_capsule, &l_token_ticker)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_ticker_check");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (!l_token_ticker) {
        PyErr_SetString(PyExc_ValueError, "Token ticker is required");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to extract ledger pointer");
        return NULL;
    }
    
    dap_chain_datum_token_t *l_token = dap_ledger_token_ticker_check(l_ledger, l_token_ticker);
    if (!l_token) {
        log_it(L_DEBUG, "Token '%s' not found in ledger", l_token_ticker);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Token '%s' found in ledger", l_token_ticker);
    return PyCapsule_New(l_token, "dap_chain_datum_token_t", NULL);
}

/**
 * @brief Get token authorization signs valid count (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, token_ticker string)
 * @return Count of valid authorization signatures
 */
PyObject* dap_ledger_token_get_auth_signs_valid_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_token_ticker = NULL;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_ledger_capsule, &l_token_ticker)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_get_auth_signs_valid");
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
    
    size_t l_count = dap_ledger_token_get_auth_signs_valid(l_ledger, l_token_ticker);
    log_it(L_DEBUG, "Token '%s' has %zu valid auth signatures", l_token_ticker, l_count);
    
    return Py_BuildValue("n", (Py_ssize_t)l_count);
}

/**
 * @brief Get token authorization signs total count (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, token_ticker string)
 * @return Total count of authorization signatures
 */
PyObject* dap_ledger_token_get_auth_signs_total_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_token_ticker = NULL;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_ledger_capsule, &l_token_ticker)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_get_auth_signs_total");
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
    
    size_t l_count = dap_ledger_token_get_auth_signs_total(l_ledger, l_token_ticker);
    log_it(L_DEBUG, "Token '%s' has %zu total auth signatures", l_token_ticker, l_count);
    
    return Py_BuildValue("n", (Py_ssize_t)l_count);
}

/**
 * @brief Purge token data from ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, preserve_db bool)
 * @return None
 */
PyObject* dap_ledger_token_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    int l_preserve_db = 0;
    
    if (!PyArg_ParseTuple(a_args, "Op", &l_ledger_capsule, &l_preserve_db)) {
        log_it(L_ERROR, "Invalid arguments for ledger_token_purge");
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
    
    log_it(L_INFO, "Purging token data from ledger (preserve_db=%d)", l_preserve_db);
    dap_ledger_token_purge(l_ledger, (bool)l_preserve_db);
    
    Py_RETURN_NONE;
}

/**
 * @brief Purge transaction data from ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, preserve_db bool)
 * @return None
 */
PyObject* dap_ledger_tx_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    int l_preserve_db = 0;
    
    if (!PyArg_ParseTuple(a_args, "Op", &l_ledger_capsule, &l_preserve_db)) {
        log_it(L_ERROR, "Invalid arguments for ledger_tx_purge");
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
    
    log_it(L_INFO, "Purging transaction data from ledger (preserve_db=%d)", l_preserve_db);
    dap_ledger_tx_purge(l_ledger, (bool)l_preserve_db);
    
    Py_RETURN_NONE;
}

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

// =============================================================================
// TOKEN & EMISSION OPERATIONS
// =============================================================================

/**
 * @brief Get all token declarations
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return PyCapsule wrapping dap_list_t* or NULL on error
 */
PyObject* dap_ledger_token_decl_all_py(PyObject *a_self, PyObject *a_args) {
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
    
    dap_list_t *l_list = dap_ledger_token_decl_all(l_ledger);
    if (!l_list) {
        log_it(L_DEBUG, "No token declarations found");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved all token declarations");
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

/**
 * @brief Get authorization public key hashes for token
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, token_ticker)
 * @return PyCapsule wrapping dap_list_t* or NULL on error
 */
PyObject* dap_ledger_token_get_auth_pkeys_hashes_py(PyObject *a_self, PyObject *a_args) {
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
    
    dap_list_t *l_list = dap_ledger_token_get_auth_pkeys_hashes(l_ledger, l_token_ticker);
    if (!l_list) {
        log_it(L_DEBUG, "No auth pkey hashes found for token %s", l_token_ticker);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved auth pkey hashes for token %s", l_token_ticker);
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

/**
 * @brief Find token emission by hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, emission_hash bytes)
 * @return PyCapsule wrapping dap_chain_datum_token_emission_t* or None
 */
PyObject* dap_ledger_token_emission_find_py(PyObject *a_self, PyObject *a_args) {
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
    
    dap_chain_datum_token_emission_t *l_emission = dap_ledger_token_emission_find(l_ledger, l_hash);
    if (!l_emission) {
        log_it(L_DEBUG, "Token emission not found");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Token emission found");
    return PyCapsule_New(l_emission, "dap_chain_datum_token_emission_t", NULL);
}

/**
 * @brief Add emission for stake lock item
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, tx_hash bytes)
 * @return Integer result code
 */
PyObject* dap_ledger_emission_for_stake_lock_item_add_py(PyObject *a_self, PyObject *a_args) {
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
    
    int l_result = dap_ledger_emission_for_stake_lock_item_add(l_ledger, l_hash);
    
    log_it(L_DEBUG, "Emission for stake lock item add result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Get token description by ticker
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, token_ticker)
 * @return Description string or None
 */
PyObject* dap_ledger_get_description_by_ticker_py(PyObject *a_self, PyObject *a_args) {
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
    
    const char *l_description = dap_ledger_get_description_by_ticker(l_ledger, l_token_ticker);
    if (!l_description) {
        log_it(L_DEBUG, "No description found for token %s", l_token_ticker);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved description for token %s", l_token_ticker);
    return PyUnicode_FromString(l_description);
}

/**
 * @brief Get locked values for address
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, addr)
 * @return PyCapsule wrapping dap_ledger_locked_out_t* or None
 */
PyObject* dap_ledger_get_locked_values_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_addr_obj;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_addr_obj)) {
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
    
    dap_ledger_locked_out_t *l_locked = dap_ledger_get_locked_values(l_ledger, l_addr);
    if (!l_locked) {
        log_it(L_DEBUG, "No locked values found for address");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved locked values for address");
    return PyCapsule_New(l_locked, "dap_ledger_locked_out_t", NULL);
}

/**
 * @brief Check if reward is used
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, block_hash bytes, sign_pkey_hash bytes)
 * @return True if used, False otherwise
 */
PyObject* dap_ledger_is_used_reward_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_block_hash_bytes, *l_pkey_hash_bytes;
    Py_ssize_t l_block_hash_size, l_pkey_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#s#", &l_ledger_obj, &l_block_hash_bytes, &l_block_hash_size,
                          &l_pkey_hash_bytes, &l_pkey_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_block_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Block hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    if ((size_t)l_pkey_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Pkey hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_block_hash = (dap_hash_fast_t *)l_block_hash_bytes;
    dap_hash_fast_t *l_pkey_hash = (dap_hash_fast_t *)l_pkey_hash_bytes;
    
    bool l_result = dap_ledger_is_used_reward(l_ledger, l_block_hash, l_pkey_hash);
    
    log_it(L_DEBUG, "Reward used check: %s", l_result ? "true" : "false");
    return PyBool_FromLong(l_result);
}

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

// =============================================================================
// CALLBACKS & NOTIFICATIONS
// =============================================================================

/**
 * @brief Add transaction add notification callback
 * @note Callback functionality requires complex implementation - stub
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return None
 */
PyObject* dap_ledger_tx_add_notify_py(PyObject *a_self, PyObject *a_args) {
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
    log_it(L_INFO, "Add TX add notification callback (stub - callback not yet implemented)");
    
    Py_RETURN_NONE;
}

/**
 * @brief Add event notification callback
 * @note Callback functionality requires complex implementation - stub
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return None
 */
PyObject* dap_ledger_event_notify_add_py(PyObject *a_self, PyObject *a_args) {
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
    log_it(L_INFO, "Add event notification callback (stub - callback not yet implemented)");
    
    Py_RETURN_NONE;
}

/**
 * @brief Add bridged transaction notification callback
 * @note Callback functionality requires complex implementation - stub
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return None
 */
PyObject* dap_ledger_bridged_tx_notify_add_py(PyObject *a_self, PyObject *a_args) {
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
    log_it(L_INFO, "Add bridged TX notification callback (stub - callback not yet implemented)");
    
    Py_RETURN_NONE;
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

// =========================================
// MODULE INITIALIZATION
// =========================================
// This function registers all ledger functions with the Python module
int cellframe_ledger_init(PyObject *module) {
    // Define ledger method definitions - full API
    static PyMethodDef ledger_methods[] = {
        // Transaction operations
        {"ledger_tx_add", (PyCFunction)dap_ledger_tx_add_py, METH_VARARGS,
         "Add transaction to ledger"},
        {"ledger_tx_load", (PyCFunction)dap_ledger_tx_load_py, METH_VARARGS,
         "Load transaction to ledger"},
        {"ledger_tx_remove", (PyCFunction)dap_ledger_tx_remove_py, METH_VARARGS,
         "Remove transaction from ledger"},
        {"ledger_tx_add_check", (PyCFunction)dap_ledger_tx_add_check_py, METH_VARARGS,
         "Check/validate transaction before adding"},
        
        // Token operations
        {"ledger_token_add", (PyCFunction)dap_ledger_token_add_py, METH_VARARGS,
         "Add token to ledger"},
        {"ledger_token_load", (PyCFunction)dap_ledger_token_load_py, METH_VARARGS,
         "Load token to ledger"},
        {"ledger_token_add_check", (PyCFunction)dap_ledger_token_add_check_py, METH_VARARGS,
         "Check/validate token before adding"},
        
        // Token emission operations
        {"ledger_token_emission_add", (PyCFunction)dap_ledger_token_emission_add_py, METH_VARARGS,
         "Add token emission to ledger"},
        {"ledger_token_emission_load", (PyCFunction)dap_ledger_token_emission_load_py, METH_VARARGS,
         "Load token emission to ledger"},
        {"ledger_token_emission_add_check", (PyCFunction)dap_ledger_token_emission_add_check_py, METH_VARARGS,
         "Check/validate token emission before adding"},
        
        // Ledger management
        {"ledger_purge", (PyCFunction)dap_ledger_purge_py, METH_VARARGS,
         "Purge ledger data"},
        {"ledger_count", (PyCFunction)dap_ledger_count_py, METH_VARARGS,
         "Get ledger transaction count"},
        {"ledger_token_purge", (PyCFunction)dap_ledger_token_purge_py, METH_VARARGS,
         "Purge token data from ledger"},
        {"ledger_tx_purge", (PyCFunction)dap_ledger_tx_purge_py, METH_VARARGS,
         "Purge transaction data from ledger"},
        
        // Token info and query
        {"ledger_token_ticker_check", (PyCFunction)dap_ledger_token_ticker_check_py, METH_VARARGS,
         "Check token ticker and get token datum"},
        {"ledger_token_get_auth_signs_valid", (PyCFunction)dap_ledger_token_get_auth_signs_valid_py, METH_VARARGS,
         "Get count of valid authorization signatures for token"},
        {"ledger_token_get_auth_signs_total", (PyCFunction)dap_ledger_token_get_auth_signs_total_py, METH_VARARGS,
         "Get total count of authorization signatures for token"},
        
        // JSON info operations
        {"ledger_token_info", (PyCFunction)dap_ledger_token_info_py, METH_VARARGS,
         "Get token info as JSON"},
        {"ledger_token_info_by_name", (PyCFunction)dap_ledger_token_info_by_name_py, METH_VARARGS,
         "Get token info by name as JSON"},
        {"ledger_balance_info", (PyCFunction)dap_ledger_balance_info_py, METH_VARARGS,
         "Get balance info as JSON"},
        {"ledger_threshold_info", (PyCFunction)dap_ledger_threshold_info_py, METH_VARARGS,
         "Get threshold info as JSON"},
        
        // Balance & Find operations (CRITICAL!)
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
        
        // TX Lists & Operations
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
        
        // Token & Emission operations
        {"ledger_token_decl_all", (PyCFunction)dap_ledger_token_decl_all_py, METH_VARARGS,
         "Get all token declarations"},
        {"ledger_token_get_auth_pkeys_hashes", (PyCFunction)dap_ledger_token_get_auth_pkeys_hashes_py, METH_VARARGS,
         "Get authorization public key hashes for token"},
        {"ledger_token_emission_find", (PyCFunction)dap_ledger_token_emission_find_py, METH_VARARGS,
         "Find token emission by hash"},
        {"ledger_emission_for_stake_lock_item_add", (PyCFunction)dap_ledger_emission_for_stake_lock_item_add_py, METH_VARARGS,
         "Add emission for stake lock item"},
        {"ledger_get_description_by_ticker", (PyCFunction)dap_ledger_get_description_by_ticker_py, METH_VARARGS,
         "Get token description by ticker"},
        {"ledger_get_locked_values", (PyCFunction)dap_ledger_get_locked_values_py, METH_VARARGS,
         "Get locked values for address"},
        {"ledger_is_used_reward", (PyCFunction)dap_ledger_is_used_reward_py, METH_VARARGS,
         "Check if reward is used"},
        
        // Datum Iterator operations
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
        
        // Conditional Outputs operations
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
        
        // Callbacks & Notifications
        {"ledger_tx_add_notify", (PyCFunction)dap_ledger_tx_add_notify_py, METH_VARARGS,
         "Add transaction add notification callback (stub)"},
        {"ledger_event_notify_add", (PyCFunction)dap_ledger_event_notify_add_py, METH_VARARGS,
         "Add event notification callback (stub)"},
        {"ledger_bridged_tx_notify_add", (PyCFunction)dap_ledger_bridged_tx_notify_add_py, METH_VARARGS,
         "Add bridged transaction notification callback (stub)"},
        
        // Chain Purge & Misc operations
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
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    
    // Add all methods to the module
    for (PyMethodDef *method = ledger_methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
        if (!func) {
            return -1;
        }
        
        if (PyModule_AddObject(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            return -1;
        }
    }
    
    return 0;
}
