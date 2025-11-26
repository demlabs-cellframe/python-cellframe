#include "include/cf_ledger_internal.h"

/*
 * Cellframe ledger bindings for Python API
 * Provides full access to ledger operations: TX, tokens, emissions, balance calculations
 */

#define LOG_TAG "python_cellframe_ledger"


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
 * @brief Get token ticker from token datum (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (token capsule)
 * @return Token ticker string
 */
PyObject* dap_chain_datum_token_get_ticker_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_token_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_token_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_datum_token_get_ticker");
        return NULL;
    }
    
    if (!l_token_capsule || !PyCapsule_CheckExact(l_token_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected token capsule");
        return NULL;
    }
    
    dap_chain_datum_token_t *l_token = (dap_chain_datum_token_t*)PyCapsule_GetPointer(l_token_capsule, "dap_chain_datum_token_t");
    if (!l_token) {
        log_it(L_ERROR, "Failed to extract token pointer");
        PyErr_SetString(PyExc_ValueError, "Invalid token capsule");
        return NULL;
    }
    
    return PyUnicode_FromString(l_token->ticker);
}

/**
 * @brief Get token total supply from token datum (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (token capsule)
 * @return Token total supply as uint256 capsule
 */
PyObject* dap_chain_datum_token_get_total_supply_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_token_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_token_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_datum_token_get_total_supply");
        return NULL;
    }
    
    if (!l_token_capsule || !PyCapsule_CheckExact(l_token_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected token capsule");
        return NULL;
    }
    
    dap_chain_datum_token_t *l_token = (dap_chain_datum_token_t*)PyCapsule_GetPointer(l_token_capsule, "dap_chain_datum_token_t");
    if (!l_token) {
        log_it(L_ERROR, "Failed to extract token pointer");
        PyErr_SetString(PyExc_ValueError, "Invalid token capsule");
        return NULL;
    }
    
    // Create uint256 capsule for total_supply
    uint256_t *l_supply = DAP_NEW_Z(uint256_t);
    if (!l_supply) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate uint256_t");
        return NULL;
    }
    *l_supply = l_token->total_supply;
    
    return PyCapsule_New(l_supply, "uint256_t", NULL);
}

/**
 * @brief Get token signs_valid from token datum (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (token capsule)
 * @return Signs valid count
 */
PyObject* dap_chain_datum_token_get_signs_valid_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_token_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_token_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_datum_token_get_signs_valid");
        return NULL;
    }
    
    if (!l_token_capsule || !PyCapsule_CheckExact(l_token_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected token capsule");
        return NULL;
    }
    
    dap_chain_datum_token_t *l_token = (dap_chain_datum_token_t*)PyCapsule_GetPointer(l_token_capsule, "dap_chain_datum_token_t");
    if (!l_token) {
        log_it(L_ERROR, "Failed to extract token pointer");
        PyErr_SetString(PyExc_ValueError, "Invalid token capsule");
        return NULL;
    }
    
    return PyLong_FromUnsignedLong(l_token->signs_valid);
}

/**
 * @brief Get token signs_total from token datum (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (token capsule)
 * @return Signs total count
 */
PyObject* dap_chain_datum_token_get_signs_total_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_token_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_token_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_datum_token_get_signs_total");
        return NULL;
    }
    
    if (!l_token_capsule || !PyCapsule_CheckExact(l_token_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected token capsule");
        return NULL;
    }
    
    dap_chain_datum_token_t *l_token = (dap_chain_datum_token_t*)PyCapsule_GetPointer(l_token_capsule, "dap_chain_datum_token_t");
    if (!l_token) {
        log_it(L_ERROR, "Failed to extract token pointer");
        PyErr_SetString(PyExc_ValueError, "Invalid token capsule");
        return NULL;
    }
    
    return PyLong_FromUnsignedLong(l_token->signs_total);
}

/**
 * @brief Get token type from token datum (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (token capsule)
 * @return Token type
 */
PyObject* dap_chain_datum_token_get_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_token_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_token_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_datum_token_get_type");
        return NULL;
    }
    
    if (!l_token_capsule || !PyCapsule_CheckExact(l_token_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected token capsule");
        return NULL;
    }
    
    dap_chain_datum_token_t *l_token = (dap_chain_datum_token_t*)PyCapsule_GetPointer(l_token_capsule, "dap_chain_datum_token_t");
    if (!l_token) {
        log_it(L_ERROR, "Failed to extract token pointer");
        PyErr_SetString(PyExc_ValueError, "Invalid token capsule");
        return NULL;
    }
    
    return PyLong_FromUnsignedLong(l_token->type);
}

/**
 * @brief Get token version from token datum (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (token capsule)
 * @return Token version
 */
PyObject* dap_chain_datum_token_get_version_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_token_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_token_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_datum_token_get_version");
        return NULL;
    }
    
    if (!l_token_capsule || !PyCapsule_CheckExact(l_token_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected token capsule");
        return NULL;
    }
    
    dap_chain_datum_token_t *l_token = (dap_chain_datum_token_t*)PyCapsule_GetPointer(l_token_capsule, "dap_chain_datum_token_t");
    if (!l_token) {
        log_it(L_ERROR, "Failed to extract token pointer");
        PyErr_SetString(PyExc_ValueError, "Invalid token capsule");
        return NULL;
    }
    
    return PyLong_FromUnsignedLong(l_token->version);
}

/**
 * @brief Get token subtype from token datum (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (token capsule)
 * @return Token subtype
 */
PyObject* dap_chain_datum_token_get_subtype_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_token_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_token_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_datum_token_get_subtype");
        return NULL;
    }
    
    if (!l_token_capsule || !PyCapsule_CheckExact(l_token_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected token capsule");
        return NULL;
    }
    
    dap_chain_datum_token_t *l_token = (dap_chain_datum_token_t*)PyCapsule_GetPointer(l_token_capsule, "dap_chain_datum_token_t");
    if (!l_token) {
        log_it(L_ERROR, "Failed to extract token pointer");
        PyErr_SetString(PyExc_ValueError, "Invalid token capsule");
        return NULL;
    }
    
    return PyLong_FromUnsignedLong(l_token->subtype);
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


// Get method definitions for token module
PyMethodDef* cellframe_ledger_token_get_methods(void) {
    static PyMethodDef token_methods[] = {
        {"ledger_token_add", (PyCFunction)dap_ledger_token_add_py, METH_VARARGS,
         "Add token to ledger"},
        {"ledger_token_load", (PyCFunction)dap_ledger_token_load_py, METH_VARARGS,
         "Load token to ledger"},
        {"ledger_token_add_check", (PyCFunction)dap_ledger_token_add_check_py, METH_VARARGS,
         "Check/validate token before adding"},
        {"ledger_token_emission_add", (PyCFunction)dap_ledger_token_emission_add_py, METH_VARARGS,
         "Add token emission to ledger"},
        {"ledger_token_emission_load", (PyCFunction)dap_ledger_token_emission_load_py, METH_VARARGS,
         "Load token emission to ledger"},
        {"ledger_token_emission_add_check", (PyCFunction)dap_ledger_token_emission_add_check_py, METH_VARARGS,
         "Check/validate token emission before adding"},
        {"ledger_token_purge", (PyCFunction)dap_ledger_token_purge_py, METH_VARARGS,
         "Purge token data from ledger"},
        {"ledger_token_ticker_check", (PyCFunction)dap_ledger_token_ticker_check_py, METH_VARARGS,
         "Check token ticker and get token datum"},
        {"ledger_token_get_auth_signs_valid", (PyCFunction)dap_ledger_token_get_auth_signs_valid_py, METH_VARARGS,
         "Get count of valid authorization signatures for token"},
        {"ledger_token_get_auth_signs_total", (PyCFunction)dap_ledger_token_get_auth_signs_total_py, METH_VARARGS,
         "Get total count of authorization signatures for token"},
        {"chain_datum_token_get_ticker", (PyCFunction)dap_chain_datum_token_get_ticker_py, METH_VARARGS,
         "Get token ticker from token datum"},
        {"chain_datum_token_get_total_supply", (PyCFunction)dap_chain_datum_token_get_total_supply_py, METH_VARARGS,
         "Get token total supply from token datum (returns uint256 capsule)"},
        {"chain_datum_token_get_signs_valid", (PyCFunction)dap_chain_datum_token_get_signs_valid_py, METH_VARARGS,
         "Get token signs_valid from token datum"},
        {"chain_datum_token_get_signs_total", (PyCFunction)dap_chain_datum_token_get_signs_total_py, METH_VARARGS,
         "Get token signs_total from token datum"},
        {"chain_datum_token_get_type", (PyCFunction)dap_chain_datum_token_get_type_py, METH_VARARGS,
         "Get token type from token datum"},
        {"chain_datum_token_get_version", (PyCFunction)dap_chain_datum_token_get_version_py, METH_VARARGS,
         "Get token version from token datum"},
        {"chain_datum_token_get_subtype", (PyCFunction)dap_chain_datum_token_get_subtype_py, METH_VARARGS,
         "Get token subtype from token datum"},
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
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return token_methods;
}
