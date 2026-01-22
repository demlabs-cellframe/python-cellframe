#include "cellframe.h"
#include "dap_chain_cs.h"
#include "dap_chain.h"
#include "dap_config.h"

#define LOG_TAG "python_cellframe_consensus"

// =============================================================================
// CONSENSUS SYSTEM INITIALIZATION
// =============================================================================

/**
 * @brief Initialize consensus subsystem globally
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    int l_result = dap_chain_cs_init();
    
    log_it(L_DEBUG, "Consensus subsystem initialized, result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize consensus subsystem globally
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_chain_cs_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    dap_chain_cs_deinit();
    
    log_it(L_DEBUG, "Consensus subsystem deinitialized");
    Py_RETURN_NONE;
}

// =============================================================================
// CONSENSUS LIFECYCLE OPERATIONS
// =============================================================================

/**
 * @brief Create consensus from config
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, config capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_config_obj;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_obj, &l_config_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_config_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a config capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    dap_config_t *l_config = (dap_config_t *)PyCapsule_GetPointer(l_config_obj, "dap_config_t");
    
    if (!l_chain || !l_config) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain or config capsule");
        return NULL;
    }
    
    int l_result = dap_chain_cs_create(l_chain, l_config);
    
    log_it(L_DEBUG, "Consensus create result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Load consensus from config
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, config capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_load_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_config_obj;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_obj, &l_config_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_config_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a config capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    dap_config_t *l_config = (dap_config_t *)PyCapsule_GetPointer(l_config_obj, "dap_config_t");
    
    if (!l_chain || !l_config) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain or config capsule");
        return NULL;
    }
    
    int l_result = dap_chain_cs_load(l_chain, l_config);
    
    log_it(L_DEBUG, "Consensus load result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Start consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_start_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    int l_result = dap_chain_cs_start(l_chain);
    
    log_it(L_DEBUG, "Consensus start result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Stop consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_stop_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    int l_result = dap_chain_cs_stop(l_chain);
    
    log_it(L_DEBUG, "Consensus stop result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Purge consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    int l_result = dap_chain_cs_purge(l_chain);
    
    log_it(L_DEBUG, "Consensus purge result: %d", l_result);
    return PyLong_FromLong(l_result);
}

// =============================================================================
// CONSENSUS CALLBACKS MANAGEMENT
// =============================================================================

/**
 * @brief Set callbacks for chain
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, callbacks capsule)
 * @return None
 */
PyObject* dap_chain_cs_set_callbacks_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_callbacks_obj;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_obj, &l_callbacks_obj)) {
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
    
    dap_chain_cs_callbacks_t *l_callbacks = NULL;
    if (l_callbacks_obj && l_callbacks_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_callbacks_obj)) {
            PyErr_SetString(PyExc_TypeError, "Second argument must be a callbacks capsule or None");
            return NULL;
        }
        l_callbacks = (dap_chain_cs_callbacks_t *)PyCapsule_GetPointer(l_callbacks_obj, "dap_chain_cs_callbacks_t");
        if (!l_callbacks) {
            PyErr_SetString(PyExc_ValueError, "Invalid callbacks capsule");
            return NULL;
        }
    }
    
    dap_chain_cs_set_callbacks(l_chain, l_callbacks);
    
    log_it(L_DEBUG, "Consensus callbacks %s for chain", l_callbacks ? "set" : "cleared");
    Py_RETURN_NONE;
}

/**
 * @brief Get registered callbacks for chain
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return PyCapsule wrapping dap_chain_cs_callbacks_t* or None
 */
PyObject* dap_chain_cs_get_callbacks_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    dap_chain_cs_callbacks_t *l_callbacks = dap_chain_cs_get_callbacks(l_chain);
    if (!l_callbacks) {
        log_it(L_DEBUG, "No callbacks registered for chain");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved consensus callbacks for chain");
    return PyCapsule_New(l_callbacks, "dap_chain_cs_callbacks_t", NULL);
}

// =============================================================================
// CONSENSUS WRAPPER FUNCTIONS
// =============================================================================

/**
 * @brief Get fee group for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, net_name string)
 * @return String or None
 */
PyObject* dap_chain_cs_get_fee_group_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    const char *l_net_name;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_chain_obj, &l_net_name)) {
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
    
    char *l_fee_group = dap_chain_cs_get_fee_group(l_chain, l_net_name);
    if (!l_fee_group) {
        log_it(L_DEBUG, "No fee group found");
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = PyUnicode_FromString(l_fee_group);
    log_it(L_DEBUG, "Retrieved fee group: %s", l_fee_group);
    return l_result;
}

/**
 * @brief Get reward group for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, net_name string)
 * @return String or None
 */
PyObject* dap_chain_cs_get_reward_group_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    const char *l_net_name;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_chain_obj, &l_net_name)) {
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
    
    char *l_reward_group = dap_chain_cs_get_reward_group(l_chain, l_net_name);
    if (!l_reward_group) {
        log_it(L_DEBUG, "No reward group found");
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = PyUnicode_FromString(l_reward_group);
    log_it(L_DEBUG, "Retrieved reward group: %s", l_reward_group);
    return l_result;
}

/**
 * @brief Get fee for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Bytes representing uint256_t
 */
PyObject* dap_chain_cs_get_fee_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    uint256_t l_fee = dap_chain_cs_get_fee(l_chain);
    
    log_it(L_DEBUG, "Retrieved consensus fee");
    return PyBytes_FromStringAndSize((const char *)&l_fee, sizeof(uint256_t));
}

/**
 * @brief Get sign public key for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return PyCapsule wrapping dap_pkey_t* or None
 */
PyObject* dap_chain_cs_get_sign_pkey_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    dap_pkey_t *l_pkey = dap_chain_cs_get_sign_pkey(l_chain);
    if (!l_pkey) {
        log_it(L_DEBUG, "No sign pkey found");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved consensus sign pkey");
    return PyCapsule_New(l_pkey, "dap_pkey_t", NULL);
}

/**
 * @brief Get collecting level for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Bytes representing uint256_t
 */
PyObject* dap_chain_cs_get_collecting_level_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    uint256_t l_level = dap_chain_cs_get_collecting_level(l_chain);
    
    log_it(L_DEBUG, "Retrieved consensus collecting level");
    return PyBytes_FromStringAndSize((const char *)&l_level, sizeof(uint256_t));
}

/**
 * @brief Get autocollect status for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Boolean
 */
PyObject* dap_chain_cs_get_autocollect_status_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    bool l_status = dap_chain_cs_get_autocollect_status(l_chain);
    
    log_it(L_DEBUG, "Consensus autocollect status: %s", l_status ? "true" : "false");
    return PyBool_FromLong(l_status);
}

/**
 * @brief Add block collect for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, block_cache capsule, params capsule, type int)
 * @return None
 */
PyObject* dap_chain_cs_add_block_collect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_block_cache_obj, *l_params_obj;
    int l_type;
    
    if (!PyArg_ParseTuple(a_args, "OOOi", &l_chain_obj, &l_block_cache_obj, &l_params_obj, &l_type)) {
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
    
    void *l_block_cache = NULL;
    if (l_block_cache_obj && l_block_cache_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_block_cache_obj)) {
            PyErr_SetString(PyExc_TypeError, "Second argument must be a capsule or None");
            return NULL;
        }
        l_block_cache = PyCapsule_GetPointer(l_block_cache_obj, NULL);
    }
    
    void *l_params = NULL;
    if (l_params_obj && l_params_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_params_obj)) {
            PyErr_SetString(PyExc_TypeError, "Third argument must be a capsule or None");
            return NULL;
        }
        l_params = PyCapsule_GetPointer(l_params_obj, NULL);
    }
    
    dap_chain_cs_add_block_collect(l_chain, l_block_cache, l_params, l_type);
    
    log_it(L_DEBUG, "Added block collect for consensus (type=%d)", l_type);
    Py_RETURN_NONE;
}

/**
 * @brief Set hardfork state for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, state bool)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_set_hardfork_state_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    int l_state;
    
    if (!PyArg_ParseTuple(a_args, "Op", &l_chain_obj, &l_state)) {
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
    
    int l_result = dap_chain_cs_set_hardfork_state(l_chain, (bool)l_state);
    
    log_it(L_DEBUG, "Consensus hardfork state set to %s, result: %d", l_state ? "true" : "false", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Check if hardfork is engaged
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Boolean
 */
PyObject* dap_chain_cs_hardfork_engaged_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    bool l_engaged = dap_chain_cs_hardfork_engaged(l_chain);
    
    log_it(L_DEBUG, "Consensus hardfork engaged: %s", l_engaged ? "true" : "false");
    return PyBool_FromLong(l_engaged);
}


// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

/**
 * @brief Get method definitions for consensus module
 */
PyMethodDef* cellframe_consensus_get_methods(void) {
    static PyMethodDef consensus_methods[] = {
        // System initialization
        {"chain_cs_init", (PyCFunction)dap_chain_cs_init_py, METH_VARARGS,
         "Initialize consensus subsystem globally"},
        {"chain_cs_deinit", (PyCFunction)dap_chain_cs_deinit_py, METH_VARARGS,
         "Deinitialize consensus subsystem globally"},
        
        // Lifecycle operations
        {"chain_cs_create", (PyCFunction)dap_chain_cs_create_py, METH_VARARGS,
         "Create consensus from config"},
        {"chain_cs_load", (PyCFunction)dap_chain_cs_load_py, METH_VARARGS,
         "Load consensus from config"},
        {"chain_cs_start", (PyCFunction)dap_chain_cs_start_py, METH_VARARGS,
         "Start consensus"},
        {"chain_cs_stop", (PyCFunction)dap_chain_cs_stop_py, METH_VARARGS,
         "Stop consensus"},
        {"chain_cs_purge", (PyCFunction)dap_chain_cs_purge_py, METH_VARARGS,
         "Purge consensus"},
        
        // Callbacks management
        {"chain_cs_set_callbacks", (PyCFunction)dap_chain_cs_set_callbacks_py, METH_VARARGS,
         "Set callbacks for chain"},
        {"chain_cs_get_callbacks", (PyCFunction)dap_chain_cs_get_callbacks_py, METH_VARARGS,
         "Get registered callbacks for chain"},
        
        // Consensus wrapper functions
        {"chain_cs_get_fee_group", (PyCFunction)dap_chain_cs_get_fee_group_py, METH_VARARGS,
         "Get fee group for consensus"},
        {"chain_cs_get_reward_group", (PyCFunction)dap_chain_cs_get_reward_group_py, METH_VARARGS,
         "Get reward group for consensus"},
        {"chain_cs_get_fee", (PyCFunction)dap_chain_cs_get_fee_py, METH_VARARGS,
         "Get fee for consensus"},
        {"chain_cs_get_sign_pkey", (PyCFunction)dap_chain_cs_get_sign_pkey_py, METH_VARARGS,
         "Get sign public key for consensus"},
        {"chain_cs_get_collecting_level", (PyCFunction)dap_chain_cs_get_collecting_level_py, METH_VARARGS,
         "Get collecting level for consensus"},
        {"chain_cs_get_autocollect_status", (PyCFunction)dap_chain_cs_get_autocollect_status_py, METH_VARARGS,
         "Get autocollect status for consensus"},
        {"chain_cs_set_hardfork_state", (PyCFunction)dap_chain_cs_set_hardfork_state_py, METH_VARARGS,
         "Set hardfork state for consensus"},
        {"chain_cs_hardfork_engaged", (PyCFunction)dap_chain_cs_hardfork_engaged_py, METH_VARARGS,
         "Check if hardfork is engaged"},
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return consensus_methods;
}

