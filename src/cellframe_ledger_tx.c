#include "cellframe_ledger_internal.h"

/*
 * Cellframe ledger transaction bindings
 * Transaction add, load, remove, validation operations
 */


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


// Get method definitions for tx module
PyMethodDef* cellframe_ledger_tx_get_methods(void) {
    static PyMethodDef tx_methods[] = {
        {"ledger_tx_add", (PyCFunction)dap_ledger_tx_add_py, METH_VARARGS,
         "Add transaction to ledger"},
        {"ledger_tx_load", (PyCFunction)dap_ledger_tx_load_py, METH_VARARGS,
         "Load transaction to ledger"},
        {"ledger_tx_remove", (PyCFunction)dap_ledger_tx_remove_py, METH_VARARGS,
         "Remove transaction from ledger"},
        {"ledger_tx_add_check", (PyCFunction)dap_ledger_tx_add_check_py, METH_VARARGS,
         "Check/validate transaction before adding"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return tx_methods;
}
