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
