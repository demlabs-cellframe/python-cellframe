/**
 * @file cf_compose.c
 * @brief Python bindings for TX Compose Registry API
 * @details Implements Python→C adapter for new registry-based TX compose system
 * @author Cellframe Development Team
 * @date 2025-01-11 (Phase 3.5 Refactoring)
 * 
 * NEW ARCHITECTURE (Post Phase 3.5):
 * - Registry-based plugin system (no direct dependencies)
 * - Dependency Inversion: modules register builders, not called directly
 * - Python callbacks registered via tx_type string identifier
 * - Proper GIL management and memory lifecycle
 * 
 * MIGRATION FROM OLD API:
 * OLD: dap_chain_tx_compose_callback_t(void *params, dap_chain_tx_compose_config_t *config)
 * NEW: dap_chain_tx_compose_callback_t(dap_ledger_t *ledger, dap_list_t *utxo, void *params)
 */

#include "cellframe.h"
#include "dap_chain_tx_compose_api.h"
#include "dap_chain_net.h"
#include "dap_list.h"
#include "../common/cf_callbacks_registry.h"  // CRITICAL: For memory leak prevention

#define LOG_TAG "python_cellframe_compose"

// =============================================================================
// PYTHON CALLBACK CONTEXT
// =============================================================================

/**
 * @brief Context for Python TX compose callback
 */
typedef struct {
    PyObject *callback;      // Python callable
    PyObject *user_data;     // Optional user data
    char *tx_type;           // TX type identifier (for cleanup)
} python_compose_ctx_t;

// Global registry for Python compose callbacks (for cleanup)
static python_compose_ctx_t **s_python_compose_callbacks = NULL;
static size_t s_python_compose_callbacks_count = 0;
static pthread_mutex_t s_compose_mutex = PTHREAD_MUTEX_INITIALIZER;

// =============================================================================
// C WRAPPER FOR PYTHON CALLBACKS
// =============================================================================

/**
 * @brief C wrapper that calls Python TX compose callback
 * @details Implements new SDK signature:
 *          dap_chain_datum_t* (*)(dap_ledger_t*, dap_list_t*, void*)
 * 
 * @param a_ledger Ledger context
 * @param a_list_used_outs List of UTXO (dap_chain_tx_used_out_t*)
 * @param a_params User parameters (passed from Python)
 * @return Datum or NULL on error
 */
static dap_chain_datum_t* s_python_compose_wrapper(dap_ledger_t *a_ledger,
                                                    dap_list_t *a_list_used_outs,
                                                    void *a_params) {
    python_compose_ctx_t *l_ctx = (python_compose_ctx_t*)a_params;
    if (!l_ctx || !l_ctx->callback) {
        log_it(L_ERROR, "Invalid compose callback context");
        return NULL;
    }
    
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    // Wrap arguments for Python
    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_utxo_list = PyCapsule_New(a_list_used_outs, "dap_list_t", NULL);
    
    // Call Python callback: callback(ledger, utxo_list, user_data)
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->callback, l_ledger, l_utxo_list, l_ctx->user_data, NULL
    );
    
    Py_DECREF(l_ledger);
    Py_DECREF(l_utxo_list);
    
    dap_chain_datum_t *l_datum = NULL;
    if (!l_result) {
        log_it(L_ERROR, "Python compose callback raised an exception");
        PyErr_Print();
    } else {
        if (PyCapsule_CheckExact(l_result)) {
            l_datum = (dap_chain_datum_t*)PyCapsule_GetPointer(l_result, "dap_chain_datum_t");
        } else if (l_result != Py_None) {
            log_it(L_WARNING, "Callback returned non-capsule object (expected dap_chain_datum_t capsule)");
        }
        Py_DECREF(l_result);
    }
    
    PyGILState_Release(l_gstate);
    return l_datum;
}

// =============================================================================
// PYTHON API FUNCTIONS
// =============================================================================

/**
 * @brief Register Python TX builder for specific type
 * @param a_self Python self object (unused)
 * @param a_args Arguments (tx_type, callback, [user_data])
 * @return Integer result code (0 on success)
 * 
 * USAGE:
 * ```python
 * def my_tx_builder(ledger, utxo_list, user_data):
 *     # Build TX datum
 *     return datum_capsule  # or None on error
 * 
 * dap_chain_tx_compose_register("my_custom_tx", my_tx_builder, user_data)
 * ```
 */
PyObject* dap_chain_tx_compose_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_tx_type;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "sO|O", &l_tx_type, &l_callback, &l_user_data)) {
        PyErr_SetString(PyExc_TypeError, "Expected (tx_type, callback, [user_data])");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be callable");
        return NULL;
    }
    
    // Allocate context
    python_compose_ctx_t *l_ctx = DAP_NEW_Z(python_compose_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->tx_type = dap_strdup(l_tx_type);
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    
    Py_INCREF(l_callback);
    Py_INCREF(l_user_data);
    
    // Register with SDK
    int l_result = dap_chain_tx_compose_register(l_tx_type, s_python_compose_wrapper, l_ctx);
    
    if (l_result != 0) {
        log_it(L_ERROR, "Failed to register TX builder for '%s': %d", l_tx_type, l_result);
        Py_DECREF(l_callback);
        Py_DECREF(l_user_data);
        DAP_DELETE(l_ctx->tx_type);
        DAP_DELETE(l_ctx);
        PyErr_SetString(PyExc_RuntimeError, "Failed to register TX builder");
        return NULL;
    }
    
    // Add to global cleanup list
    pthread_mutex_lock(&s_compose_mutex);
    s_python_compose_callbacks = DAP_REALLOC(s_python_compose_callbacks,
                                             (s_python_compose_callbacks_count + 1) * sizeof(python_compose_ctx_t*));
    s_python_compose_callbacks[s_python_compose_callbacks_count++] = l_ctx;
    pthread_mutex_unlock(&s_compose_mutex);
    
    log_it(L_DEBUG, "Registered Python TX builder for type '%s'", l_tx_type);
    
    return PyLong_FromLong(l_result);
}

/**
 * @brief Unregister TX builder
 * @param a_self Python self object (unused)
 * @param a_args Arguments (tx_type)
 * @return None
 */
PyObject* dap_chain_tx_compose_unregister_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_tx_type;
    
    if (!PyArg_ParseTuple(a_args, "s", &l_tx_type)) {
        return NULL;
    }
    
    // Find and remove from cleanup list
    pthread_mutex_lock(&s_compose_mutex);
    for (size_t i = 0; i < s_python_compose_callbacks_count; i++) {
        if (s_python_compose_callbacks[i] && 
            strcmp(s_python_compose_callbacks[i]->tx_type, l_tx_type) == 0) {
            
            python_compose_ctx_t *l_ctx = s_python_compose_callbacks[i];
            
            // Unregister from SDK
            dap_chain_tx_compose_unregister(l_tx_type);
            
            // Cleanup Python refs
            Py_DECREF(l_ctx->callback);
            Py_DECREF(l_ctx->user_data);
            DAP_DELETE(l_ctx->tx_type);
            DAP_DELETE(l_ctx);
            
            // Remove from array (shift remaining elements)
            for (size_t j = i; j < s_python_compose_callbacks_count - 1; j++) {
                s_python_compose_callbacks[j] = s_python_compose_callbacks[j + 1];
            }
            s_python_compose_callbacks_count--;
            
            log_it(L_DEBUG, "Unregistered Python TX builder for type '%s'", l_tx_type);
            break;
        }
    }
    pthread_mutex_unlock(&s_compose_mutex);
    
    Py_RETURN_NONE;
}

/**
 * @brief Check if TX builder is registered
 * @param a_self Python self object (unused)
 * @param a_args Arguments (tx_type)
 * @return Boolean
 */
PyObject* dap_chain_tx_compose_is_registered_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_tx_type;
    
    if (!PyArg_ParseTuple(a_args, "s", &l_tx_type)) {
        return NULL;
    }
    
    bool l_registered = dap_chain_tx_compose_is_registered(l_tx_type);
    
    return PyBool_FromLong(l_registered);
}

/**
 * @brief Create TX via registered builder (dispatcher)
 * @param a_self Python self object (unused)
 * @param a_args Arguments (tx_type, ledger, utxo_list, params)
 * @return Datum capsule or None
 * 
 * USAGE:
 * ```python
 * ledger = ...  # dap_ledger_t capsule
 * utxo_list = ...  # dap_list_t capsule
 * params = {"addr_to": ..., "value": ..., "fee": ...}
 * 
 * datum = dap_chain_tx_compose_create("transfer", ledger, utxo_list, params)
 * ```
 */
PyObject* dap_chain_tx_compose_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_tx_type;
    PyObject *l_ledger_cap, *l_utxo_cap, *l_params;
    
    if (!PyArg_ParseTuple(a_args, "sOOO", &l_tx_type, &l_ledger_cap, &l_utxo_cap, &l_params)) {
        PyErr_SetString(PyExc_TypeError, "Expected (tx_type, ledger, utxo_list, params)");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_cap) || !PyCapsule_CheckExact(l_utxo_cap)) {
        PyErr_SetString(PyExc_TypeError, "ledger and utxo_list must be capsules");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = PyCapsule_GetPointer(l_ledger_cap, "dap_ledger_t");
    dap_list_t *l_utxo_list = PyCapsule_GetPointer(l_utxo_cap, "dap_list_t");
    
    if (!l_ledger || !l_utxo_list) {
        PyErr_SetString(PyExc_ValueError, "Invalid capsule pointers");
        return NULL;
    }
    
    // Call SDK dispatcher
    dap_chain_datum_t *l_datum = dap_chain_tx_compose_create(l_tx_type, l_ledger, l_utxo_list, l_params);
    
    if (!l_datum) {
        log_it(L_WARNING, "TX compose create failed for type '%s'", l_tx_type);
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_datum, "dap_chain_datum_t", NULL);
}

/**
 * @brief Create compose config
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_name, [url_str], [port], [cert_path], [enc])
 * @return ComposeConfig capsule or None
 */
PyObject* dap_compose_config_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *net_name;
    const char *url_str = NULL;
    int port = 0;
    const char *cert_path = NULL;
    int enc = 0;
    
    if (!PyArg_ParseTuple(a_args, "s|sisi", &net_name, &url_str, &port, &cert_path, &enc)) {
        PyErr_SetString(PyExc_TypeError, "Expected (net_name, [url_str], [port], [cert_path], [enc])");
        return NULL;
    }
    
    // Create config dict (Python will handle this)
    PyObject *config_dict = PyDict_New();
    PyDict_SetItemString(config_dict, "net_name", PyUnicode_FromString(net_name));
    if (url_str) PyDict_SetItemString(config_dict, "url_str", PyUnicode_FromString(url_str));
    if (port > 0) PyDict_SetItemString(config_dict, "port", PyLong_FromLong(port));
    if (cert_path) PyDict_SetItemString(config_dict, "cert_path", PyUnicode_FromString(cert_path));
    PyDict_SetItemString(config_dict, "enc", PyBool_FromLong(enc));
    
    return config_dict;
}

/**
 * @brief Create transaction using compose system
 * @param a_self Python self object (unused)
 * @param a_args Arguments (config, tx_type, params_dict)
 * @return TX capsule or None
 */
PyObject* dap_compose_tx_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *config_dict;
    const char *tx_type;
    PyObject *params_dict;
    
    if (!PyArg_ParseTuple(a_args, "OsO", &config_dict, &tx_type, &params_dict)) {
        PyErr_SetString(PyExc_TypeError, "Expected (config_dict, tx_type, params_dict)");
        return NULL;
    }
    
    if (!PyDict_Check(config_dict) || !PyDict_Check(params_dict)) {
        PyErr_SetString(PyExc_TypeError, "config and params must be dictionaries");
        return NULL;
    }
    
    // Extract network name from config
    PyObject *net_name_obj = PyDict_GetItemString(config_dict, "net_name");
    if (!net_name_obj) {
        PyErr_SetString(PyExc_ValueError, "config must contain 'net_name'");
        return NULL;
    }
    
    const char *net_name = PyUnicode_AsUTF8(net_name_obj);
    if (!net_name) {
        return NULL;
    }
    
    // Get network and ledger
    dap_chain_net_t *net = dap_chain_net_by_name(net_name);
    if (!net) {
        PyErr_Format(PyExc_ValueError, "Network '%s' not found", net_name);
        return NULL;
    }
    
    dap_ledger_t *ledger = net->pub.ledger;
    if (!ledger) {
        PyErr_SetString(PyExc_RuntimeError, "Ledger not available for network");
        return NULL;
    }
    
    // For now, create empty UTXO list (Python layer will populate)
    dap_list_t *utxo_list = NULL;
    
    // Call compose dispatcher
    dap_chain_datum_t *datum = dap_chain_tx_compose_create(tx_type, ledger, utxo_list, params_dict);
    
    if (!datum) {
        log_it(L_WARNING, "Failed to compose TX of type '%s'", tx_type);
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(datum, "dap_chain_datum_t", NULL);
}

// =============================================================================
// CLEANUP
// =============================================================================

/**
 * @brief Cleanup all Python compose callbacks (CRITICAL: prevents memory leaks)
 * @details Must be called on module unload to Py_DECREF all registered callbacks
 */
void dap_chain_tx_compose_cleanup_callbacks_py(void) {
    log_it(L_INFO, "Cleaning up Python TX compose callbacks...");
    
    pthread_mutex_lock(&s_compose_mutex);
    
    for (size_t i = 0; i < s_python_compose_callbacks_count; i++) {
        if (s_python_compose_callbacks[i]) {
            python_compose_ctx_t *l_ctx = s_python_compose_callbacks[i];
            
            // Unregister from SDK
            if (l_ctx->tx_type) {
                dap_chain_tx_compose_unregister(l_ctx->tx_type);
                log_it(L_DEBUG, "Unregistered TX builder '%s'", l_ctx->tx_type);
            }
            
            // Cleanup Python refs
            Py_DECREF(l_ctx->callback);
            Py_DECREF(l_ctx->user_data);
            DAP_DELETE(l_ctx->tx_type);
            DAP_DELETE(l_ctx);
        }
    }
    
    DAP_DELETE(s_python_compose_callbacks);
    s_python_compose_callbacks = NULL;
    s_python_compose_callbacks_count = 0;
    
    pthread_mutex_unlock(&s_compose_mutex);
    
    log_it(L_INFO, "Python TX compose callbacks cleanup complete");
}
