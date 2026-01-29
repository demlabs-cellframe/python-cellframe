#include "include/cf_ledger_internal.h"

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
 * 
 * @note API changed in SDK: now uses dap_ledger_create_options_t instead of (net, flags)
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
    
    // Create ledger options
    dap_ledger_create_options_t l_options = {0};
    l_options.net_id = l_net->pub.id;
    l_options.flags = l_flags;
    l_options.native_ticker = l_net->pub.native_ticker;
    
    // Set default chain IDs
    dap_chain_id_t l_chain_id = {0};
    l_options.chain_ids = &l_chain_id;
    l_options.chain_ids_count = 1;
    
    // Generate name from net_id
    snprintf(l_options.name, sizeof(l_options.name), "%"DAP_UINT64_FORMAT_U, l_options.net_id.uint64);
    
    dap_ledger_t *l_ledger = dap_ledger_create(&l_options);
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
 * 
 * @note API changed in SDK: now uses dap_ledger_create_options_t instead of (net, flags)
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
    
    // Create ledger options
    dap_ledger_create_options_t l_options = {0};
    l_options.net_id = l_net->pub.id;
    l_options.flags = l_flags;
    l_options.native_ticker = l_net->pub.native_ticker;
    
    // Set default chain IDs
    dap_chain_id_t l_chain_id = {0};
    l_options.chain_ids = &l_chain_id;
    l_options.chain_ids_count = 1;
    
    // Generate name from net_id
    snprintf(l_options.name, sizeof(l_options.name), "%"DAP_UINT64_FORMAT_U, l_options.net_id.uint64);
    
    dap_ledger_t *l_ledger = dap_ledger_create(&l_options);
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to create ledger for network '%s'", l_net_name);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Created ledger '%s' for network '%s' with flags 0x%04x",
           l_options.name, l_net_name, l_flags);
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

static void cf_ledger_create_options_destructor(PyObject *a_capsule) {
    dap_ledger_create_options_t *l_opts = (dap_ledger_create_options_t *)PyCapsule_GetPointer(
        a_capsule, "dap_ledger_create_options_t"
    );
    if (!l_opts) {
        return;
    }
    DAP_DELETE(l_opts->chain_ids);
    DAP_DELETE(l_opts);
}

PyObject* dap_ledger_create_options_default_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    dap_ledger_create_options_t *l_opts = dap_ledger_create_options_default();
    if (!l_opts) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_opts, "dap_ledger_create_options_t", cf_ledger_create_options_destructor);
}

PyObject* dap_ledger_find_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name = NULL;

    if (!PyArg_ParseTuple(a_args, "s", &l_name)) {
        return NULL;
    }

    dap_ledger_t *l_ledger = dap_ledger_find_by_name(l_name);
    if (!l_ledger) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_ledger, "dap_ledger_t", NULL);
}

PyObject* dap_ledger_find_by_net_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_ledger_t *l_ledger = dap_ledger_find_by_net_id(l_net_id_struct);
    if (!l_ledger) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_ledger, "dap_ledger_t", NULL);
}

PyObject* dap_ledger_has_chain_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    unsigned long long l_chain_id = 0;

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

    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    bool l_result = dap_ledger_has_chain_id(l_ledger, l_chain_id_struct);
    return PyBool_FromLong(l_result ? 1 : 0);
}

PyObject* dap_ledger_get_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;

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

    const char *l_name = dap_ledger_get_name(l_ledger);
    if (!l_name) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(l_name);
}

PyObject* dap_ledger_get_net_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;

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

    dap_chain_net_id_t l_net_id = dap_ledger_get_net_id(l_ledger);
    return PyLong_FromUnsignedLongLong(l_net_id.uint64);
}

PyObject* dap_ledger_get_chain_info_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    unsigned long long l_chain_id = 0;

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

    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    dap_chain_info_t *l_info = dap_ledger_get_chain_info(l_ledger, l_chain_id_struct);
    if (!l_info) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_info, "dap_chain_info_t", NULL);
}

PyObject* dap_ledger_get_chain_info_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    const char *l_chain_name = NULL;

    if (!PyArg_ParseTuple(a_args, "Os", &l_ledger_obj, &l_chain_name)) {
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

    dap_chain_info_t *l_info = dap_ledger_get_chain_info_by_name(l_ledger, l_chain_name);
    if (!l_info) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_info, "dap_chain_info_t", NULL);
}

PyObject* dap_ledger_register_chain_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_chain_obj = NULL;
    unsigned long long l_chain_id = 0;
    unsigned int l_chain_type = 0;
    const char *l_chain_name = NULL;

    if (!PyArg_ParseTuple(a_args, "OKsIO", &l_ledger_obj, &l_chain_id, &l_chain_name,
                          &l_chain_type, &l_chain_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj) || !PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger and chain must be capsules");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_ledger || !l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or chain capsule");
        return NULL;
    }

    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    int l_result = dap_ledger_register_chain(l_ledger, l_chain_id_struct, l_chain_name,
                                             (uint16_t)l_chain_type, l_chain);
    return PyLong_FromLong(l_result);
}

PyObject* dap_ledger_unregister_chain_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    unsigned long long l_chain_id = 0;

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

    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    dap_ledger_unregister_chain(l_ledger, l_chain_id_struct);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_set_net_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_ledger_obj, &l_net_id)) {
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

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_ledger_set_net_id(l_ledger, l_net_id_struct);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_set_native_ticker_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    const char *l_ticker = NULL;

    if (!PyArg_ParseTuple(a_args, "Os", &l_ledger_obj, &l_ticker)) {
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

    dap_ledger_set_native_ticker(l_ledger, l_ticker);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_set_fee_params_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_addr_obj = NULL;
    const char *l_value_bytes = NULL;
    Py_ssize_t l_value_size = 0;

    if (!PyArg_ParseTuple(a_args, "Os#O", &l_ledger_obj, &l_value_bytes, &l_value_size, &l_addr_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj) || !PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger and addr must be capsules");
        return NULL;
    }

    if ((size_t)l_value_size != sizeof(uint256_t)) {
        PyErr_Format(PyExc_ValueError, "fee_value must be exactly %zu bytes", sizeof(uint256_t));
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_obj, "dap_chain_addr_t");
    if (!l_ledger || !l_addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or addr capsule");
        return NULL;
    }

    uint256_t l_value = *(uint256_t *)l_value_bytes;
    dap_ledger_set_fee_params(l_ledger, l_value, *l_addr);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_set_poa_keys_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_poa_keys_obj = NULL;
    unsigned int l_min_count = 0;

    if (!PyArg_ParseTuple(a_args, "OOI", &l_ledger_obj, &l_poa_keys_obj, &l_min_count)) {
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

    dap_list_t *l_keys = NULL;
    if (l_poa_keys_obj && l_poa_keys_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_poa_keys_obj)) {
            PyErr_SetString(PyExc_TypeError, "poa_keys must be a list capsule or None");
            return NULL;
        }
        l_keys = (dap_list_t *)PyCapsule_GetPointer(l_poa_keys_obj, "dap_list_t");
        if (!l_keys) {
            PyErr_SetString(PyExc_ValueError, "Invalid poa_keys capsule");
            return NULL;
        }
    }

    dap_ledger_set_poa_keys(l_ledger, l_keys, (uint16_t)l_min_count);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_set_syncing_state_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    int l_syncing = 0;

    if (!PyArg_ParseTuple(a_args, "Op", &l_ledger_obj, &l_syncing)) {
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

    dap_ledger_set_syncing_state(l_ledger, l_syncing != 0);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_set_blockchain_timer_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_chain_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_chain_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj) || !PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger and chain must be capsules");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_ledger || !l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or chain capsule");
        return NULL;
    }

    dap_ledger_set_blockchain_timer(l_ledger, l_chain);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_set_blockchain_time_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    unsigned long long l_time = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_ledger_obj, &l_time)) {
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

    dap_ledger_set_blockchain_time(l_ledger, (dap_time_t)l_time);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_load_cache_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;

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

    dap_ledger_load_cache(l_ledger);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_get_pkey_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    const char *l_wallet_name = NULL;
    unsigned int l_key_idx = 0;

    if (!PyArg_ParseTuple(a_args, "OsI", &l_ledger_obj, &l_wallet_name, &l_key_idx)) {
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

    dap_pkey_t *l_pkey = dap_ledger_get_pkey(l_ledger, l_wallet_name, l_key_idx);
    if (!l_pkey) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_pkey, "dap_pkey_t", NULL);
}

PyObject* dap_ledger_get_pkey_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    const char *l_wallet_name = NULL;
    unsigned int l_key_idx = 0;

    if (!PyArg_ParseTuple(a_args, "OsI", &l_ledger_obj, &l_wallet_name, &l_key_idx)) {
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

    dap_hash_fast_t *l_hash = dap_ledger_get_pkey_hash(l_ledger, l_wallet_name, l_key_idx);
    if (!l_hash) {
        Py_RETURN_NONE;
    }

    return PyBytes_FromStringAndSize((const char *)l_hash, sizeof(dap_hash_fast_t));
}

PyObject* dap_ledger_sign_data_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    const char *l_wallet_name = NULL;
    const char *l_data = NULL;
    Py_ssize_t l_data_size = 0;
    unsigned int l_key_idx = 0;

    if (!PyArg_ParseTuple(a_args, "Oss#I", &l_ledger_obj, &l_wallet_name, &l_data, &l_data_size, &l_key_idx)) {
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

    dap_sign_t *l_sign = dap_ledger_sign_data(l_ledger, l_wallet_name, l_data, (size_t)l_data_size, l_key_idx);
    if (!l_sign) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_sign, "dap_sign_t", NULL);
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
        {"ledger_create_options_default", (PyCFunction)dap_ledger_create_options_default_py, METH_VARARGS,
         "Create default ledger options"},
        {"ledger_handle_free", (PyCFunction)dap_ledger_handle_free_py, METH_VARARGS,
         "Free ledger handle"},
        {"ledger_find_by_name", (PyCFunction)dap_ledger_find_by_name_py, METH_VARARGS,
         "Find ledger by name"},
        {"ledger_find_by_net_id", (PyCFunction)dap_ledger_find_by_net_id_py, METH_VARARGS,
         "Find ledger by network ID"},
        {"ledger_has_chain_id", (PyCFunction)dap_ledger_has_chain_id_py, METH_VARARGS,
         "Check if ledger has chain ID"},
        {"ledger_get_name", (PyCFunction)dap_ledger_get_name_py, METH_VARARGS,
         "Get ledger name"},
        {"ledger_get_net_id", (PyCFunction)dap_ledger_get_net_id_py, METH_VARARGS,
         "Get ledger network ID"},
        {"ledger_get_chain_info", (PyCFunction)dap_ledger_get_chain_info_py, METH_VARARGS,
         "Get chain info by chain ID"},
        {"ledger_get_chain_info_by_name", (PyCFunction)dap_ledger_get_chain_info_by_name_py, METH_VARARGS,
         "Get chain info by name"},
        {"ledger_register_chain", (PyCFunction)dap_ledger_register_chain_py, METH_VARARGS,
         "Register chain in ledger"},
        {"ledger_unregister_chain", (PyCFunction)dap_ledger_unregister_chain_py, METH_VARARGS,
         "Unregister chain from ledger"},
        {"ledger_set_net_id", (PyCFunction)dap_ledger_set_net_id_py, METH_VARARGS,
         "Set ledger network ID"},
        {"ledger_set_native_ticker", (PyCFunction)dap_ledger_set_native_ticker_py, METH_VARARGS,
         "Set ledger native ticker"},
        {"ledger_set_fee_params", (PyCFunction)dap_ledger_set_fee_params_py, METH_VARARGS,
         "Set ledger fee params"},
        {"ledger_set_poa_keys", (PyCFunction)dap_ledger_set_poa_keys_py, METH_VARARGS,
         "Set ledger PoA keys"},
        {"ledger_set_syncing_state", (PyCFunction)dap_ledger_set_syncing_state_py, METH_VARARGS,
         "Set ledger syncing state"},
        {"ledger_set_blockchain_timer", (PyCFunction)dap_ledger_set_blockchain_timer_py, METH_VARARGS,
         "Set blockchain timer callbacks"},
        {"ledger_set_blockchain_time", (PyCFunction)dap_ledger_set_blockchain_time_py, METH_VARARGS,
         "Set blockchain time"},
        {"ledger_load_cache", (PyCFunction)dap_ledger_load_cache_py, METH_VARARGS,
         "Load ledger cache"},
        {"ledger_get_pkey", (PyCFunction)dap_ledger_get_pkey_py, METH_VARARGS,
         "Get public key via wallet callbacks"},
        {"ledger_get_pkey_hash", (PyCFunction)dap_ledger_get_pkey_hash_py, METH_VARARGS,
         "Get public key hash via wallet callbacks"},
        {"ledger_sign_data", (PyCFunction)dap_ledger_sign_data_py, METH_VARARGS,
         "Sign data via wallet callbacks"},
        {"ledger_count_from_to", (PyCFunction)dap_ledger_count_from_to_py, METH_VARARGS,
         "Count transactions from timestamp range"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return lifecycle_methods;
}
