#include "cellframe_ledger_internal.h"

/*
 * Cellframe ledger bindings for Python API
 * Provides full access to ledger operations: TX, tokens, emissions, balance calculations
 */

#define LOG_TAG "python_cellframe_ledger"


// =============================================================================
// LIFECYCLE OPERATIONS
// =============================================================================

/**
 * @brief Initialize ledger subsystem globally
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return Integer result code
 */
PyObject* dap_ledger_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    int l_result = dap_ledger_init();
    
    log_it(L_DEBUG, "Ledger subsystem initialized, result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize ledger subsystem globally
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_ledger_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    dap_ledger_deinit();
    
    log_it(L_DEBUG, "Ledger subsystem deinitialized");
    Py_RETURN_NONE;
}

/**
 * @brief Create new ledger for a network
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain_net capsule, flags integer)
 * @return PyCapsule wrapping dap_ledger_t* or None on error
 */
PyObject* dap_ledger_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj;
    uint16_t l_flags;
    
    if (!PyArg_ParseTuple(a_args, "OH", &l_net_obj, &l_flags)) {
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
    
    dap_ledger_t *l_ledger = dap_ledger_create(l_net, l_flags);
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to create ledger");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Created ledger for network with flags 0x%04x", l_flags);
    return PyCapsule_New(l_ledger, "dap_ledger_t", NULL);
}

/**
 * @brief Create new ledger by network name (convenience wrapper)
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_name string, flags integer optional)
 * @return PyCapsule wrapping dap_ledger_t* or None on error
 */
PyObject* dap_ledger_new_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name;
    uint16_t l_flags = 0;  // Default flags
    
    if (!PyArg_ParseTuple(a_args, "s|H", &l_net_name, &l_flags)) {
        return NULL;
    }
    
    // Find network by name
    dap_chain_net_t *l_net = dap_chain_net_by_name(l_net_name);
    if (!l_net) {
        log_it(L_ERROR, "Network '%s' not found", l_net_name);
        PyErr_Format(PyExc_ValueError, "Network '%s' not found", l_net_name);
        return NULL;
    }
    
    dap_ledger_t *l_ledger = dap_ledger_create(l_net, l_flags);
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to create ledger for network '%s'", l_net_name);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Created ledger for network '%s' with flags 0x%04x", l_net_name, l_flags);
    return PyCapsule_New(l_ledger, "dap_ledger_t", NULL);
}

/**
 * @brief Open existing ledger by network name (convenience wrapper)
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_name string)
 * @return PyCapsule wrapping dap_ledger_t* or None on error
 */
PyObject* dap_ledger_open_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name;
    
    if (!PyArg_ParseTuple(a_args, "s", &l_net_name)) {
        return NULL;
    }
    
    // Find network by name
    dap_chain_net_t *l_net = dap_chain_net_by_name(l_net_name);
    if (!l_net) {
        log_it(L_ERROR, "Network '%s' not found", l_net_name);
        PyErr_Format(PyExc_ValueError, "Network '%s' not found", l_net_name);
        return NULL;
    }
    
    // Get ledger from network (ledger is part of network structure)
    dap_ledger_t *l_ledger = l_net->pub.ledger;
    if (!l_ledger) {
        log_it(L_ERROR, "Ledger not found for network '%s'", l_net_name);
        PyErr_Format(PyExc_ValueError, "Ledger not found for network '%s'", l_net_name);
        return NULL;
    }
    
    log_it(L_DEBUG, "Opened ledger for network '%s'", l_net_name);
    return PyCapsule_New(l_ledger, "dap_ledger_t", NULL);
}

/**
 * @brief Close ledger (free handle)
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule)
 * @return None
 */
PyObject* dap_ledger_close_py(PyObject *a_self, PyObject *a_args) {
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
    
    // Free ledger handle (if owned)
    dap_ledger_handle_free(l_ledger);
    
    log_it(L_DEBUG, "Ledger closed");
    Py_RETURN_NONE;
}

/**
 * @brief Free ledger handle
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule)
 * @return None
 */
PyObject* dap_ledger_handle_free_py(PyObject *a_self, PyObject *a_args) {
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
    
    dap_ledger_handle_free(l_ledger);
    
    log_it(L_DEBUG, "Ledger handle freed");
    Py_RETURN_NONE;
}

// =============================================================================
// TOKEN TICKER OPERATIONS
// =============================================================================

/**
 * @brief Get all token tickers for an address
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, address bytes)
 * @return Python list of ticker strings or None
 */
PyObject* dap_ledger_addr_get_token_ticker_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_addr_bytes;
    Py_ssize_t l_addr_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_ledger_obj, &l_addr_bytes, &l_addr_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_addr_size != sizeof(dap_chain_addr_t)) {
        PyErr_Format(PyExc_ValueError, "Address must be exactly %zu bytes", sizeof(dap_chain_addr_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)l_addr_bytes;
    char **l_tickers = NULL;
    size_t l_tickers_size = 0;
    
    dap_ledger_addr_get_token_ticker_all(l_ledger, l_addr, &l_tickers, &l_tickers_size);
    
    if (!l_tickers || l_tickers_size == 0) {
        log_it(L_DEBUG, "No tickers found for address");
        Py_RETURN_NONE;
    }
    
    PyObject *l_list = PyList_New(l_tickers_size);
    for (size_t i = 0; i < l_tickers_size; i++) {
        PyList_SetItem(l_list, i, PyUnicode_FromString(l_tickers[i]));
    }
    
    // Free C array (strings are managed by ledger, only free the array itself if needed)
    
    log_it(L_DEBUG, "Retrieved %zu tickers for address", l_tickers_size);
    return l_list;
}

/**
 * @brief Get all token tickers for an address (deprecated version)
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, address bytes)
 * @return Python list of ticker strings or None
 */
PyObject* dap_ledger_addr_get_token_ticker_all_depricated_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_addr_bytes;
    Py_ssize_t l_addr_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_ledger_obj, &l_addr_bytes, &l_addr_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_addr_size != sizeof(dap_chain_addr_t)) {
        PyErr_Format(PyExc_ValueError, "Address must be exactly %zu bytes", sizeof(dap_chain_addr_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)l_addr_bytes;
    char **l_tickers = NULL;
    size_t l_tickers_size = 0;
    
    dap_ledger_addr_get_token_ticker_all_deprecated(l_ledger, l_addr, &l_tickers, &l_tickers_size);
    
    if (!l_tickers || l_tickers_size == 0) {
        log_it(L_DEBUG, "No tickers found for address (deprecated)");
        Py_RETURN_NONE;
    }
    
    PyObject *l_list = PyList_New(l_tickers_size);
    for (size_t i = 0; i < l_tickers_size; i++) {
        PyList_SetItem(l_list, i, PyUnicode_FromString(l_tickers[i]));
    }
    
    log_it(L_DEBUG, "Retrieved %zu tickers for address (deprecated)", l_tickers_size);
    return l_list;
}


// Get method definitions for lifecycle module
PyMethodDef* cellframe_ledger_lifecycle_get_methods(void) {
    static PyMethodDef lifecycle_methods[] = {
        {"ledger_purge", (PyCFunction)dap_ledger_purge_py, METH_VARARGS,
         "Purge ledger data"},
        {"ledger_count", (PyCFunction)dap_ledger_count_py, METH_VARARGS,
         "Get ledger transaction count"},
        {"ledger_init", (PyCFunction)dap_ledger_init_py, METH_VARARGS,
         "Initialize ledger subsystem globally"},
        {"ledger_deinit", (PyCFunction)dap_ledger_deinit_py, METH_VARARGS,
         "Deinitialize ledger subsystem globally"},
        {"ledger_create", (PyCFunction)dap_ledger_create_py, METH_VARARGS,
         "Create new ledger for a network"},
        {"ledger_handle_free", (PyCFunction)dap_ledger_handle_free_py, METH_VARARGS,
         "Free ledger handle"},
        {"ledger_addr_get_token_ticker_all", (PyCFunction)dap_ledger_addr_get_token_ticker_all_py, METH_VARARGS,
         "Get all token tickers for an address"},
        {"ledger_addr_get_token_ticker_all_depricated", (PyCFunction)dap_ledger_addr_get_token_ticker_all_depricated_py, METH_VARARGS,
         "Get all token tickers for an address (deprecated)"},
        {"ledger_count_from_to", (PyCFunction)dap_ledger_count_from_to_py, METH_VARARGS,
         "Count transactions from timestamp range"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return lifecycle_methods;
}
