#include "cellframe.h"
#include "dap_chain_net.h"
#include "dap_chain_net_tx.h"
#include "dap_chain_common.h"
#include "dap_chain_datum.h"
#include "dap_common.h"
#include "dap_global_db_cluster.h"

#define LOG_TAG "python_cellframe_network"

/**
 * @brief Get network by name (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "s", &l_net_name)) {
        log_it(L_ERROR, "Invalid arguments for net_by_name");
        return NULL;
    }
    
    // Additional validation
    if (!l_net_name || strlen(l_net_name) == 0) {
        log_it(L_ERROR, "Network name is NULL or empty");
        PyErr_SetString(PyExc_ValueError, "Network name cannot be empty");
        return NULL;
    }
    
    if (strlen(l_net_name) > DAP_CHAIN_NET_NAME_MAX) {
        log_it(L_ERROR, "Network name too long: %zu", strlen(l_net_name));
        PyErr_Format(PyExc_ValueError, "Network name too long (max %d)", DAP_CHAIN_NET_NAME_MAX);
        return NULL;
    }
    
    // Call DAP SDK function
    dap_chain_net_t *l_net = dap_chain_net_by_name(l_net_name);
    if (!l_net) {
        log_it(L_WARNING, "Network '%s' not found", l_net_name);
        PyErr_Format(PyExc_ValueError, "Network '%s' not found", l_net_name);
        return NULL;
    }
    
    return PyCapsule_New(l_net, "dap_chain_net_t", NULL);
}

/**
 * @brief Get network by ID (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_by_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id_uint = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id_uint)) {
        log_it(L_ERROR, "Invalid arguments for net_by_id");
        return NULL;
    }
    
    // Validate network ID
    if (l_net_id_uint == 0) {
        log_it(L_ERROR, "Network ID cannot be zero");
        PyErr_SetString(PyExc_ValueError, "Network ID cannot be zero");
        return NULL;
    }
    
    // Call DAP SDK function
    dap_chain_net_id_t l_net_id = {.uint64 = l_net_id_uint};
    dap_chain_net_t *l_net = dap_chain_net_by_id(l_net_id);
    
    if (!l_net) {
        log_it(L_WARNING, "Network with ID %llu not found", l_net_id_uint);
        PyErr_Format(PyExc_ValueError, "Network with ID %llu not found", l_net_id_uint);
        return NULL;
    }
    
    return PyCapsule_New(l_net, "dap_chain_net_t", NULL);
}

/**
 * @brief Get network ID by name (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network ID as uint64 or NULL with Python exception set
 */
PyObject* dap_chain_net_id_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "s", &l_net_name)) {
        log_it(L_ERROR, "Invalid arguments for net_id_by_name");
        return NULL;
    }
    
    // Additional validation
    if (!l_net_name || strlen(l_net_name) == 0) {
        log_it(L_ERROR, "Network name is NULL or empty");
        PyErr_SetString(PyExc_ValueError, "Network name cannot be empty");
        return NULL;
    }
    
    // Call DAP SDK function
    dap_chain_net_id_t l_net_id = dap_chain_net_id_by_name(l_net_name);
    
    if (!l_net_id.uint64) {
        log_it(L_WARNING, "Network '%s' not found", l_net_name);
        PyErr_Format(PyExc_ValueError, "Network '%s' not found", l_net_name);
        return NULL;
    }
    
    return Py_BuildValue("K", l_net_id.uint64);
}

/**
 * @brief Get total count of networks (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return Number of networks
 */
PyObject* dap_chain_net_count_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    size_t l_count = dap_chain_net_count();
    log_it(L_DEBUG, "Total networks count: %zu", l_count);
    return Py_BuildValue("n", l_count);
}

/**
 * @brief Start iteration over networks (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return First network capsule or None if no networks
 */
PyObject* dap_chain_net_iter_start_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    dap_chain_net_t *l_net = dap_chain_net_iter_start();
    
    if (!l_net) {
        log_it(L_DEBUG, "No networks available for iteration");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Started network iteration, first net: %s", l_net->pub.name);
    return PyCapsule_New(l_net, "dap_chain_net_t", NULL);
}

/**
 * @brief Continue iteration over networks (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Next network capsule or None if end of iteration
 */
PyObject* dap_chain_net_iter_next_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_iter_next");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get next network
    dap_chain_net_t *l_net_next = dap_chain_net_iter_next(l_net);
    
    if (!l_net_next) {
        log_it(L_DEBUG, "End of network iteration");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Next network in iteration: %s", l_net_next->pub.name);
    return PyCapsule_New(l_net_next, "dap_chain_net_t", NULL);
}

/**
 * @brief Get network name from capsule (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network name string or NULL with Python exception set
 */
PyObject* dap_chain_net_get_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_name");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    return Py_BuildValue("s", l_net->pub.name);
}

/**
 * @brief Get network ID from capsule (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network ID as uint64 or NULL with Python exception set
 */
PyObject* dap_chain_net_get_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_id");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    return Py_BuildValue("K", l_net->pub.id.uint64);
}

/**
 * @brief Get network state (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network state as int or NULL with Python exception set
 */
PyObject* dap_chain_net_get_state_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_state");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    dap_chain_net_state_t l_state = dap_chain_net_get_state(l_net);
    log_it(L_DEBUG, "Network '%s' state: %d", l_net->pub.name, (int)l_state);
    return Py_BuildValue("i", (int)l_state);
}

/**
 * @brief Change network state (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return 0 on success, negative on error
 */
PyObject* dap_chain_net_state_go_to_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    int l_new_state = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_capsule, &l_new_state)) {
        log_it(L_ERROR, "Invalid arguments for net_state_go_to");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Validate state value
    if (l_new_state < NET_STATE_LOADING || l_new_state > NET_STATE_ONLINE) {
        log_it(L_ERROR, "Invalid network state: %d", l_new_state);
        PyErr_Format(PyExc_ValueError, "Invalid network state: %d", l_new_state);
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    log_it(L_INFO, "Changing network '%s' state to %d", l_net->pub.name, l_new_state);
    int l_ret = dap_chain_net_state_go_to(l_net, (dap_chain_net_state_t)l_new_state);
    
    if (l_ret != 0) {
        log_it(L_ERROR, "Failed to change network '%s' state: %d", l_net->pub.name, l_ret);
    }
    
    return Py_BuildValue("i", l_ret);
}

/**
 * @brief Start network (go online) (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return 0 on success, negative on error
 */
PyObject* dap_chain_net_start_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_start");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    log_it(L_INFO, "Starting network '%s'", l_net->pub.name);
    int l_ret = dap_chain_net_start(l_net);
    
    if (l_ret != 0) {
        log_it(L_ERROR, "Failed to start network '%s': %d", l_net->pub.name, l_ret);
    }
    
    return Py_BuildValue("i", l_ret);
}

/**
 * @brief Stop network (go offline) (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return True on success, False on error
 */
PyObject* dap_chain_net_stop_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_stop");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    log_it(L_INFO, "Stopping network '%s'", l_net->pub.name);
    bool l_ret = dap_chain_net_stop(l_net);
    
    if (!l_ret) {
        log_it(L_ERROR, "Failed to stop network '%s'", l_net->pub.name);
    }
    
    return Py_BuildValue("O", l_ret ? Py_True : Py_False);
}

/**
 * @brief Get network native ticker (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Native ticker string or None
 */
PyObject* dap_chain_net_get_native_ticker_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_native_ticker");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    const char *l_ticker = l_net->pub.native_ticker;
    if (!l_ticker) {
        log_it(L_DEBUG, "Network '%s' has no native ticker", l_net->pub.name);
        Py_RETURN_NONE;
    }
    
    return Py_BuildValue("s", l_ticker);
}

/**
 * @brief Load all networks from config (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return None
 */
PyObject* dap_chain_net_load_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    log_it(L_INFO, "Loading all networks from config");
    dap_chain_net_load_all();
    log_it(L_INFO, "All networks loaded");
    
    Py_RETURN_NONE;
}

/**
 * @brief Try to bring all networks online (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return None
 */
PyObject* dap_chain_net_try_online_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    log_it(L_INFO, "Trying to bring all networks online");
    dap_chain_net_try_online_all();
    log_it(L_INFO, "All networks processing complete");
    
    Py_RETURN_NONE;
}

// =========================================
// CHAIN ACCESS FUNCTIONS (Phase 3)
// =========================================

/**
 * @brief Get chain by name from network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, chain name)
 * @return Chain capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_get_chain_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    const char *l_chain_name = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "Os", &l_net_capsule, &l_chain_name)) {
        log_it(L_ERROR, "Invalid arguments for net_get_chain_by_name");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Validate chain name
    if (!l_chain_name || strlen(l_chain_name) == 0) {
        log_it(L_ERROR, "Chain name is NULL or empty");
        PyErr_SetString(PyExc_ValueError, "Chain name cannot be empty");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get chain by name
    dap_chain_t *l_chain = dap_chain_net_get_chain_by_name(l_net, l_chain_name);
    if (!l_chain) {
        log_it(L_WARNING, "Chain '%s' not found in network '%s'", l_chain_name, l_net->pub.name);
        PyErr_Format(PyExc_ValueError, "Chain '%s' not found in network '%s'", l_chain_name, l_net->pub.name);
        return NULL;
    }
    
    log_it(L_DEBUG, "Found chain '%s' in network '%s'", l_chain_name, l_net->pub.name);
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

/**
 * @brief Get chain by ID from network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, chain ID uint64)
 * @return Chain capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_get_chain_by_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    unsigned long long l_chain_id_uint = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "OK", &l_net_capsule, &l_chain_id_uint)) {
        log_it(L_ERROR, "Invalid arguments for net_get_chain_by_id");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Validate chain ID
    if (l_chain_id_uint == 0) {
        log_it(L_ERROR, "Chain ID cannot be zero");
        PyErr_SetString(PyExc_ValueError, "Chain ID cannot be zero");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get chain by ID
    dap_chain_id_t l_chain_id = {.uint64 = l_chain_id_uint};
    dap_chain_t *l_chain = dap_chain_net_get_chain_by_id(l_net, l_chain_id);
    
    if (!l_chain) {
        log_it(L_WARNING, "Chain with ID %llu not found in network '%s'", l_chain_id_uint, l_net->pub.name);
        PyErr_Format(PyExc_ValueError, "Chain with ID %llu not found in network '%s'", 
                     l_chain_id_uint, l_net->pub.name);
        return NULL;
    }
    
    log_it(L_DEBUG, "Found chain ID %llu in network '%s'", l_chain_id_uint, l_net->pub.name);
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

/**
 * @brief Get chain by type from network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, chain type int)
 * @return Chain capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_get_chain_by_chain_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    int l_chain_type = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_capsule, &l_chain_type)) {
        log_it(L_ERROR, "Invalid arguments for net_get_chain_by_chain_type");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get chain by type
    dap_chain_t *l_chain = dap_chain_net_get_chain_by_chain_type(l_net, (dap_chain_type_t)l_chain_type);
    
    if (!l_chain) {
        log_it(L_WARNING, "Chain with type %d not found in network '%s'", l_chain_type, l_net->pub.name);
        PyErr_Format(PyExc_ValueError, "Chain with type %d not found in network '%s'", 
                     l_chain_type, l_net->pub.name);
        return NULL;
    }
    
    log_it(L_DEBUG, "Found chain with type %d in network '%s'", l_chain_type, l_net->pub.name);
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

/**
 * @brief Get default chain by type from network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, chain type int)
 * @return Chain capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_get_default_chain_by_chain_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    int l_chain_type = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_capsule, &l_chain_type)) {
        log_it(L_ERROR, "Invalid arguments for net_get_default_chain_by_chain_type");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get default chain by type
    dap_chain_t *l_chain = dap_chain_net_get_default_chain_by_chain_type(l_net, (dap_chain_type_t)l_chain_type);
    
    if (!l_chain) {
        log_it(L_WARNING, "Default chain with type %d not found in network '%s'", l_chain_type, l_net->pub.name);
        PyErr_Format(PyExc_ValueError, "Default chain with type %d not found in network '%s'", 
                     l_chain_type, l_net->pub.name);
        return NULL;
    }
    
    log_it(L_DEBUG, "Found default chain with type %d in network '%s'", l_chain_type, l_net->pub.name);
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

/**
 * @brief Get chain name from capsule (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return Chain name string or NULL with Python exception set
 */
PyObject* dap_chain_get_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_get_name");
        return NULL;
    }
    
    // Validate capsule
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    // Extract chain pointer
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    return Py_BuildValue("s", l_chain->name);
}

/**
 * @brief Get chain ID from capsule (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return Chain ID as uint64 or NULL with Python exception set
 */
PyObject* dap_chain_get_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_get_id");
        return NULL;
    }
    
    // Validate capsule
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    // Extract chain pointer
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    return Py_BuildValue("K", l_chain->id.uint64);
}

// =========================================
// NODE INFO FUNCTIONS 
// =========================================

/**
 * @brief Get node role in network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Node role as int or NULL with Python exception set
 */
PyObject* dap_chain_net_get_role_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_role");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get node role
    dap_chain_node_role_t l_role = dap_chain_net_get_role(l_net);
    log_it(L_DEBUG, "Node role in network '%s': %u (%s)", l_net->pub.name, 
           l_role.enums, dap_chain_node_role_to_str(l_role));
    
    return Py_BuildValue("I", l_role.enums);  // Return as unsigned int
}

/**
 * @brief Check if node is authorized in network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return True/False or NULL with Python exception set
 */
PyObject* dap_chain_net_is_my_node_authorized_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_is_my_node_authorized");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Check authorization
    bool l_is_authorized = dap_chain_net_is_my_node_authorized(l_net);
    log_it(L_DEBUG, "Node authorized in network '%s': %s", l_net->pub.name, l_is_authorized ? "yes" : "no");
    
    return Py_BuildValue("O", l_is_authorized ? Py_True : Py_False);
}

/**
 * @brief Get current node address as integer (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Node address as uint64 or NULL with Python exception set
 */
PyObject* dap_chain_net_get_cur_addr_int_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_cur_addr_int");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get current node address
    uint64_t l_addr = dap_chain_net_get_cur_addr_int(l_net);
    log_it(L_DEBUG, "Current node address in network '%s': %lu", l_net->pub.name, l_addr);
    
    return Py_BuildValue("K", l_addr);
}

// =========================================
// NETWORK OPERATIONS FUNCTIONS
// =========================================

/**
 * @brief Dump network links to string (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Links dump string or NULL with Python exception set
 */
PyObject* dap_chain_net_links_dump_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_links_dump");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get links dump
    char *l_links_dump = dap_chain_net_links_dump(l_net);
    if (!l_links_dump) {
        log_it(L_WARNING, "Failed to dump links for network '%s'", l_net->pub.name);
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = Py_BuildValue("s", l_links_dump);
    DAP_DELETE(l_links_dump);  // Free the allocated string
    
    return l_result;
}

/**
 * @brief Add link to network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, addr uint64, host string, port int)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_net_link_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    unsigned long long l_addr_uint = 0;
    const char *l_host = NULL;
    int l_port = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "OKsi", &l_net_capsule, &l_addr_uint, &l_host, &l_port)) {
        log_it(L_ERROR, "Invalid arguments for net_link_add");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Validate host
    if (!l_host || strlen(l_host) == 0) {
        log_it(L_ERROR, "Host is NULL or empty");
        PyErr_SetString(PyExc_ValueError, "Host cannot be empty");
        return NULL;
    }
    
    // Validate port
    if (l_port <= 0 || l_port > 65535) {
        log_it(L_ERROR, "Invalid port: %d", l_port);
        PyErr_SetString(PyExc_ValueError, "Port must be between 1 and 65535");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Prepare address structure
    dap_stream_node_addr_t l_addr = {.uint64 = l_addr_uint};
    
    // Add link
    int l_result = dap_chain_net_link_add(l_net, &l_addr, l_host, (uint16_t)l_port);
    
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to add link to network '%s': host=%s port=%d (error=%d)", 
               l_net->pub.name, l_host, l_port, l_result);
    } else {
        log_it(L_INFO, "Link added to network '%s': host=%s port=%d", l_net->pub.name, l_host, l_port);
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Get target state of network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Target state as int or NULL with Python exception set
 */
PyObject* dap_chain_net_get_target_state_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_target_state");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    dap_chain_net_state_t l_state = dap_chain_net_get_target_state(l_net);
    return Py_BuildValue("i", (int)l_state);
}

/**
 * @brief Sync network chains (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_net_sync_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_sync");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    int l_result = dap_chain_net_sync(l_net);
    log_it(L_INFO, "Network '%s' sync initiated: %d", l_net->pub.name, l_result);
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Establish network links (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_net_links_establish_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_links_establish");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    int l_result = dap_chain_net_links_establish(l_net);
    log_it(L_INFO, "Network '%s' links establish initiated: %d", l_net->pub.name, l_result);
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Purge network data (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return None
 */
PyObject* dap_chain_net_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_purge");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    log_it(L_WARNING, "Purging network '%s' data", l_net->pub.name);
    dap_chain_net_purge(l_net);
    log_it(L_INFO, "Network '%s' purged", l_net->pub.name);
    
    Py_RETURN_NONE;
}

/**
 * @brief Announce network address (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return None
 */
PyObject* dap_chain_net_announce_addr_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_announce_addr");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    dap_chain_net_announce_addr(l_net);
    log_it(L_DEBUG, "Network '%s' address announced", l_net->pub.name);
    
    Py_RETURN_NONE;
}

/**
 * @brief Announce all network addresses (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return None
 */
PyObject* dap_chain_net_announce_addr_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    dap_chain_net_announce_addr_all(NULL);
    log_it(L_DEBUG, "All network addresses announced");
    
    Py_RETURN_NONE;
}

/**
 * @brief Get network transaction fee (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Dict with 'value' (256-bit int as string) and 'addr' or None if no fee
 */
PyObject* dap_chain_net_tx_get_fee_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_tx_get_fee");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    uint256_t l_fee = {};
    dap_chain_addr_t l_addr = {};
    
    bool l_has_fee = dap_chain_net_tx_get_fee(l_net->pub.id, &l_fee, &l_addr);
    
    if (!l_has_fee) {
        log_it(L_DEBUG, "Network '%s' has no transaction fee", l_net->pub.name);
        Py_RETURN_NONE;
    }
    
    // Convert uint256_t to string (coins and datoshi)
    const char *l_fee_coins_str = NULL;
    const char *l_fee_datoshi_str = dap_uint256_to_char(l_fee, &l_fee_coins_str);
    
    if (!l_fee_datoshi_str) {
        log_it(L_ERROR, "Failed to convert fee to string");
        Py_RETURN_NONE;
    }
    
    // Convert address to string (static buffer, no need to free)
    const char *l_addr_str = dap_chain_addr_to_str(&l_addr);
    if (!l_addr_str) {
        log_it(L_ERROR, "Failed to convert address to string");
        Py_RETURN_NONE;
    }
    
    // Build result dict with coins and datoshi
    PyObject *l_result = Py_BuildValue("{s:s,s:s,s:s}", 
                                       "datoshi", l_fee_datoshi_str,
                                       "coins", l_fee_coins_str ? l_fee_coins_str : "0",
                                       "addr", l_addr_str);
    
    return l_result;
}

/**
 * @brief Get current cell ID (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Dict with 'uint64' and 'raw' (bytes) or None if no cell
 */
PyObject* dap_chain_net_get_cur_cell_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_cur_cell");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t *l_cell_id = dap_chain_net_get_cur_cell(l_net);
    if (!l_cell_id) {
        log_it(L_DEBUG, "Network '%s' has no current cell", l_net->pub.name);
        Py_RETURN_NONE;
    }
    
    // Return dict with uint64 and raw bytes
    PyObject *l_raw_bytes = PyBytes_FromStringAndSize((const char*)l_cell_id->raw, DAP_CHAIN_SHARD_ID_SIZE);
    if (!l_raw_bytes) {
        log_it(L_ERROR, "Failed to create bytes object for cell ID");
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = Py_BuildValue("{s:K,s:O}", 
                                       "uint64", l_cell_id->uint64,
                                       "raw", l_raw_bytes);
    Py_DECREF(l_raw_bytes);
    
    return l_result;
}

/**
 * @brief Get current node info (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Dict with node info or None if no info available
 */
PyObject* dap_chain_net_get_my_node_info_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_my_node_info");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    dap_chain_node_info_t *l_node_info = dap_chain_net_get_my_node_info(l_net);
    if (!l_node_info) {
        log_it(L_DEBUG, "Network '%s' has no node info", l_net->pub.name);
        Py_RETURN_NONE;
    }
    
    // Convert address to uint64
    uint64_t l_addr_uint64 = l_node_info->address.uint64;
    
    // Convert cell_id
    uint64_t l_cell_id_uint64 = l_node_info->cell_id.uint64;
    
    // Get ext_host (variable length string at the end of structure)
    const char *l_ext_host = l_node_info->ext_host_len > 0 ? l_node_info->ext_host : "";
    
    // Build result dict
    PyObject *l_result = Py_BuildValue("{s:K,s:K,s:s,s:i,s:s}", 
                                       "address", l_addr_uint64,
                                       "cell_id", l_cell_id_uint64,
                                       "alias", l_node_info->alias,
                                       "ext_port", (int)l_node_info->ext_port,
                                       "ext_host", l_ext_host);
    
    return l_result;
}

/**
 * @brief Get authorized nodes (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return List of authorized node addresses (uint64) or empty list
 */
PyObject* dap_chain_net_get_authorized_nodes_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_authorized_nodes");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    size_t l_nodes_count = 0;
    dap_stream_node_addr_t *l_nodes = dap_chain_net_get_authorized_nodes(l_net, &l_nodes_count);
    
    // Create Python list
    PyObject *l_result = PyList_New(l_nodes_count);
    if (!l_result) {
        log_it(L_ERROR, "Failed to create list for authorized nodes");
        return NULL;
    }
    
    // Fill list with node addresses
    for (size_t i = 0; i < l_nodes_count; i++) {
        PyObject *l_addr = Py_BuildValue("K", l_nodes[i].uint64);
        if (!l_addr) {
            log_it(L_WARNING, "Failed to convert node address at index %zu", i);
            Py_DECREF(l_result);
            return NULL;
        }
        PyList_SET_ITEM(l_result, i, l_addr);  // Steals reference
    }
    
    log_it(L_DEBUG, "Network '%s' has %zu authorized nodes", l_net->pub.name, l_nodes_count);
    
    return l_result;
}

/**
 * @brief Verify datum for adding to chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, datum bytes, datum_hash bytes)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_net_verify_datum_for_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    const char *l_datum_bytes = NULL;
    Py_ssize_t l_datum_size = 0;
    const char *l_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "Oy#y#", &l_chain_capsule, &l_datum_bytes, &l_datum_size, &l_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for verify_datum_for_add");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    // Validate hash size
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        log_it(L_ERROR, "Invalid hash size: %zd, expected %zu", l_hash_size, sizeof(dap_hash_fast_t));
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    // Cast datum bytes to datum structure
    if (l_datum_size < (Py_ssize_t)sizeof(dap_chain_datum_t)) {
        log_it(L_ERROR, "Datum size too small: %zd (minimum %zu)", l_datum_size, sizeof(dap_chain_datum_t));
        PyErr_SetString(PyExc_ValueError, "Datum size too small");
        return NULL;
    }
    
    dap_chain_datum_t *l_datum = (dap_chain_datum_t*)l_datum_bytes;
    dap_hash_fast_t *l_datum_hash = (dap_hash_fast_t*)l_hash_bytes;
    
    // Verify datum
    int l_result = dap_chain_net_verify_datum_for_add(l_chain, l_datum, l_datum_hash);
    
    if (l_result != 0) {
        const char *l_err_str = dap_chain_net_verify_datum_err_code_to_str(l_datum, l_result);
        log_it(L_WARNING, "Datum verification failed: code=%d, msg=%s", l_result, l_err_str ? l_err_str : "unknown");
    } else {
        log_it(L_DEBUG, "Datum verified successfully for chain '%s'", l_chain->name);
    }
    
    return Py_BuildValue("i", l_result);
}

// =========================================
// CALLBACK FUNCTIONS
// =========================================

// Structure to hold Python callback information
typedef struct {
    PyObject *callback;      // Python callback function
    PyObject *user_data;     // User data to pass to callback
} python_callback_ctx_t;

/**
 * @brief C callback wrapper that calls Python callback
 * @param a_store_obj Store object
 * @param a_arg User data (python_callback_ctx_t*)
 */
static void s_python_callback_wrapper(dap_store_obj_t *a_store_obj, void *a_arg) {
    python_callback_ctx_t *l_ctx = (python_callback_ctx_t*)a_arg;
    if (!l_ctx || !l_ctx->callback) {
        log_it(L_ERROR, "Invalid callback context");
        return;
    }
    
    // Acquire GIL for Python callback
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    // Call Python callback with capsule and user data
    PyObject *l_arg_capsule = PyCapsule_New(a_store_obj, "dap_store_obj_t", NULL);
    if (!l_arg_capsule) {
        log_it(L_ERROR, "Failed to create capsule for callback argument");
        PyGILState_Release(l_gstate);
        return;
    }
    
    PyObject *l_result = PyObject_CallFunctionObjArgs(l_ctx->callback, l_arg_capsule, l_ctx->user_data, NULL);
    
    Py_DECREF(l_arg_capsule);
    
    if (!l_result) {
        log_it(L_ERROR, "Python callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }
    
    PyGILState_Release(l_gstate);
}

/**
 * @brief Add mempool notify callback for chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, callback function, user_data)
 * @return None
 */
PyObject* dap_chain_add_mempool_notify_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO|O", &l_chain_capsule, &l_callback, &l_user_data)) {
        log_it(L_ERROR, "Invalid arguments for add_mempool_notify_callback");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        log_it(L_ERROR, "Callback must be callable");
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    // Allocate callback context (will be freed by Python GC when callback is removed)
    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        log_it(L_ERROR, "Failed to allocate callback context");
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    
    // Increment ref counts to keep objects alive
    Py_INCREF(l_callback);
    Py_INCREF(l_user_data);
    
    // Register C callback
    dap_chain_add_mempool_notify_callback(l_chain, s_python_callback_wrapper, l_ctx);
    
    log_it(L_INFO, "Mempool notify callback registered for chain '%s'", l_chain->name);
    
    Py_RETURN_NONE;
}

/**
 * @brief Add nodelist notify callback for network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, callback function, user_data)
 * @return None
 */
PyObject* dap_chain_net_add_nodelist_notify_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO|O", &l_net_capsule, &l_callback, &l_user_data)) {
        log_it(L_ERROR, "Invalid arguments for add_nodelist_notify_callback");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        log_it(L_ERROR, "Callback must be callable");
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Allocate callback context
    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        log_it(L_ERROR, "Failed to allocate callback context");
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    
    Py_INCREF(l_callback);
    Py_INCREF(l_user_data);
    
    // Register C callback
    dap_chain_net_add_nodelist_notify_callback(l_net, s_python_callback_wrapper, l_ctx);
    
    log_it(L_INFO, "Nodelist notify callback registered for network '%s'", l_net->pub.name);
    
    Py_RETURN_NONE;
}

/**
 * @brief Add service order notify callback for network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, callback function, user_data)
 * @return None
 */
PyObject* dap_chain_net_srv_order_add_notify_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO|O", &l_net_capsule, &l_callback, &l_user_data)) {
        log_it(L_ERROR, "Invalid arguments for srv_order_add_notify_callback");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        log_it(L_ERROR, "Callback must be callable");
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Allocate callback context
    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        log_it(L_ERROR, "Failed to allocate callback context");
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    
    Py_INCREF(l_callback);
    Py_INCREF(l_user_data);
    
    // Register C callback
    dap_chain_net_srv_order_add_notify_callback(l_net, s_python_callback_wrapper, l_ctx);
    
    log_it(L_INFO, "Service order notify callback registered for network '%s'", l_net->pub.name);
    
    Py_RETURN_NONE;
}

// =========================================
// MODULE INITIALIZATION
// =========================================

/**
 * @brief Initialize network module and register all methods
 * @param a_module Parent Python module
 * @return 0 on success, -1 on failure
 */
int cellframe_network_init(PyObject *a_module) {
    if (!a_module) {
        log_it(L_ERROR, "Module is NULL");
        return -1;
    }
    
    // Define network module methods
    static PyMethodDef l_network_methods[] = {
        // Network lookup and iteration functions
        {"net_by_name", (PyCFunction)dap_chain_net_by_name_py, METH_VARARGS,
         "Get network by name"},
        {"net_by_id", (PyCFunction)dap_chain_net_by_id_py, METH_VARARGS,
         "Get network by ID"},
        {"net_id_by_name", (PyCFunction)dap_chain_net_id_by_name_py, METH_VARARGS,
         "Get network ID by name"},
        {"net_count", (PyCFunction)dap_chain_net_count_py, METH_NOARGS,
         "Get total count of networks"},
        {"net_iter_start", (PyCFunction)dap_chain_net_iter_start_py, METH_NOARGS,
         "Start iteration over networks"},
        {"net_iter_next", (PyCFunction)dap_chain_net_iter_next_py, METH_VARARGS,
         "Continue iteration over networks"},
        
        // Network state management functions
        {"net_get_name", (PyCFunction)dap_chain_net_get_name_py, METH_VARARGS,
         "Get network name from capsule"},
        {"net_get_id", (PyCFunction)dap_chain_net_get_id_py, METH_VARARGS,
         "Get network ID from capsule"},
        {"net_get_state", (PyCFunction)dap_chain_net_get_state_py, METH_VARARGS,
         "Get network state"},
        {"net_state_go_to", (PyCFunction)dap_chain_net_state_go_to_py, METH_VARARGS,
         "Change network state"},
        {"net_start", (PyCFunction)dap_chain_net_start_py, METH_VARARGS,
         "Start network (go online)"},
        {"net_stop", (PyCFunction)dap_chain_net_stop_py, METH_VARARGS,
         "Stop network (go offline)"},
        {"net_get_native_ticker", (PyCFunction)dap_chain_net_get_native_ticker_py, METH_VARARGS,
         "Get network native ticker"},
        {"net_load_all", (PyCFunction)dap_chain_net_load_all_py, METH_NOARGS,
         "Load all networks from config"},
        {"net_try_online_all", (PyCFunction)dap_chain_net_try_online_all_py, METH_NOARGS,
         "Try to bring all networks online"},
        
        // Chain access functions
        {"net_get_chain_by_name", (PyCFunction)dap_chain_net_get_chain_by_name_py, METH_VARARGS,
         "Get chain by name from network"},
        {"net_get_chain_by_id", (PyCFunction)dap_chain_net_get_chain_by_id_py, METH_VARARGS,
         "Get chain by ID from network"},
        {"net_get_chain_by_type", (PyCFunction)dap_chain_net_get_chain_by_chain_type_py, METH_VARARGS,
         "Get chain by type from network"},
        {"net_get_default_chain_by_type", (PyCFunction)dap_chain_net_get_default_chain_by_chain_type_py, METH_VARARGS,
         "Get default chain by type from network"},
        {"chain_get_name", (PyCFunction)dap_chain_get_name_py, METH_VARARGS,
         "Get chain name from capsule"},
        {"chain_get_id", (PyCFunction)dap_chain_get_id_py, METH_VARARGS,
         "Get chain ID from capsule"},
        
        // Node info functions
        {"net_get_role", (PyCFunction)dap_chain_net_get_role_py, METH_VARARGS,
         "Get node role in network"},
        {"net_is_my_node_authorized", (PyCFunction)dap_chain_net_is_my_node_authorized_py, METH_VARARGS,
         "Check if node is authorized in network"},
        {"net_get_cur_addr_int", (PyCFunction)dap_chain_net_get_cur_addr_int_py, METH_VARARGS,
         "Get current node address as integer"},
        
        // Network operations functions
        {"net_links_dump", (PyCFunction)dap_chain_net_links_dump_py, METH_VARARGS,
         "Dump network links to string"},
        {"net_link_add", (PyCFunction)dap_chain_net_link_add_py, METH_VARARGS,
         "Add link to network"},
        {"net_get_target_state", (PyCFunction)dap_chain_net_get_target_state_py, METH_VARARGS,
         "Get target state of network"},
        {"net_sync", (PyCFunction)dap_chain_net_sync_py, METH_VARARGS,
         "Sync network chains"},
        {"net_links_establish", (PyCFunction)dap_chain_net_links_establish_py, METH_VARARGS,
         "Establish network links"},
        {"net_purge", (PyCFunction)dap_chain_net_purge_py, METH_VARARGS,
         "Purge network data"},
        {"net_announce_addr", (PyCFunction)dap_chain_net_announce_addr_py, METH_VARARGS,
         "Announce network address"},
        {"net_announce_addr_all", (PyCFunction)dap_chain_net_announce_addr_all_py, METH_NOARGS,
         "Announce all network addresses"},
        {"net_tx_get_fee", (PyCFunction)dap_chain_net_tx_get_fee_py, METH_VARARGS,
         "Get network transaction fee"},
        
        // Advanced functions with complex structures
        {"net_get_cur_cell", (PyCFunction)dap_chain_net_get_cur_cell_py, METH_VARARGS,
         "Get current cell ID"},
        {"net_get_my_node_info", (PyCFunction)dap_chain_net_get_my_node_info_py, METH_VARARGS,
         "Get current node info"},
        {"net_get_authorized_nodes", (PyCFunction)dap_chain_net_get_authorized_nodes_py, METH_VARARGS,
         "Get authorized nodes list"},
        {"net_verify_datum_for_add", (PyCFunction)dap_chain_net_verify_datum_for_add_py, METH_VARARGS,
         "Verify datum for adding to chain"},
        
        // Callback functions
        {"chain_add_mempool_notify_callback", (PyCFunction)dap_chain_add_mempool_notify_callback_py, METH_VARARGS,
         "Add mempool notify callback for chain"},
        {"net_add_nodelist_notify_callback", (PyCFunction)dap_chain_net_add_nodelist_notify_callback_py, METH_VARARGS,
         "Add nodelist notify callback for network"},
        {"net_srv_order_add_notify_callback", (PyCFunction)dap_chain_net_srv_order_add_notify_callback_py, METH_VARARGS,
         "Add service order notify callback for network"},
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    
    // Register all methods
    for (PyMethodDef *l_method = l_network_methods; l_method->ml_name != NULL; l_method++) {
        PyObject *l_func = PyCFunction_New(l_method, NULL);
        if (!l_func) {
            log_it(L_ERROR, "Failed to create function %s", l_method->ml_name);
            return -1;
        }
        
        if (PyModule_AddObject(a_module, l_method->ml_name, l_func) < 0) {
            log_it(L_ERROR, "Failed to register function %s", l_method->ml_name);
            Py_DECREF(l_func);
            return -1;
        }
    }
    
    log_it(L_INFO, "Network module initialized successfully");
    return 0;
}

// =========================================
// LEGACY COMPATIBILITY
// =========================================

// Legacy stub type - for compatibility only
PyTypeObject PyCellframeNetworkType = {0};

PyObject* PyCellframeNetwork_new(PyTypeObject *a_type, PyObject *a_args, PyObject *a_kwds) {
    (void)a_type; (void)a_args; (void)a_kwds;
    log_it(L_WARNING, "Legacy PyCellframeNetwork_new called - use dap_chain_net_*_py functions instead");
    PyErr_SetString(PyExc_NotImplementedError, "Use dap_chain_net_*_py functions instead");
    return NULL;
}

int PyCellframeNetwork_init(PyCellframeNetwork *a_self, PyObject *a_args, PyObject *a_kwds) {
    (void)a_self; (void)a_args; (void)a_kwds;
    log_it(L_WARNING, "Legacy PyCellframeNetwork_init called");
    return -1;
}

void PyCellframeNetwork_dealloc(PyCellframeNetwork *a_self) {
    (void)a_self;
}

PyObject* PyCellframeNetwork_connect(PyCellframeNetwork *a_self, PyObject *a_args) {
    (void)a_self; (void)a_args;
    log_it(L_WARNING, "Legacy PyCellframeNetwork_connect called - use dap_chain_net_*_py functions instead");
    PyErr_SetString(PyExc_NotImplementedError, "Use dap_chain_net_*_py functions instead");
    return NULL;
} 
