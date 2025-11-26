#include "cellframe.h"
#include "dap_chain_tx_compose.h"
#include "dap_chain_tx_compose_callbacks.h"
#include "dap_chain_net.h"
#include "dap_enc_key.h"
#include "dap_list.h"
#include "dap_math_ops.h"

#define LOG_TAG "python_cellframe_compose"

// =============================================================================
// COMPOSE CALLBACKS
// =============================================================================

#define MAX_COMPOSE_CALLBACKS 16

static PyObject *s_python_compose_callbacks[MAX_COMPOSE_CALLBACKS] = {0};
static bool s_compose_callback_occupied[MAX_COMPOSE_CALLBACKS] = {false};

// Generic C callback that calls Python callback
static dap_chain_datum_tx_t* python_compose_callback_generic(int slot_idx, void *a_service_params, dap_chain_tx_compose_config_t *a_config) {
    if (slot_idx < 0 || slot_idx >= MAX_COMPOSE_CALLBACKS || !s_python_compose_callbacks[slot_idx]) {
        log_it(L_ERROR, "Invalid callback slot or no callback registered: %d", slot_idx);
        return NULL;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();
    
    // Wrap arguments
    PyObject *l_params_cap = PyCapsule_New(a_service_params, NULL, NULL);
    PyObject *l_config_cap = PyCapsule_New(a_config, "dap_chain_tx_compose_config_t", NULL);
    
    PyObject *l_args = PyTuple_Pack(2, l_params_cap, l_config_cap);
    PyObject *l_result = PyObject_CallObject(s_python_compose_callbacks[slot_idx], l_args);
    
    Py_DECREF(l_args);
    Py_DECREF(l_params_cap);
    // So we need to decref caps after pack.
    Py_DECREF(l_config_cap);
    
    dap_chain_datum_tx_t *l_tx = NULL;
    if (l_result) {
        if (PyCapsule_CheckExact(l_result)) {
            l_tx = (dap_chain_datum_tx_t*)PyCapsule_GetPointer(l_result, "dap_chain_datum_tx_t");
        } else if (l_result != Py_None) {
            log_it(L_ERROR, "Callback returned invalid object (expected capsule or None)");
        }
        Py_DECREF(l_result);
    } else {
        PyErr_Print();
    }
    
    PyGILState_Release(gstate);
    return l_tx;
}

// Macros to generate wrapper functions
#define WRAPPER_FUNC(N) \
static dap_chain_datum_tx_t* compose_callback_wrapper_##N(void *a_service_params, dap_chain_tx_compose_config_t *a_config) { \
    return python_compose_callback_generic(N, a_service_params, a_config); \
}

WRAPPER_FUNC(0)
WRAPPER_FUNC(1)
WRAPPER_FUNC(2)
WRAPPER_FUNC(3)
WRAPPER_FUNC(4)
WRAPPER_FUNC(5)
WRAPPER_FUNC(6)
WRAPPER_FUNC(7)
WRAPPER_FUNC(8)
WRAPPER_FUNC(9)
WRAPPER_FUNC(10)
WRAPPER_FUNC(11)
WRAPPER_FUNC(12)
WRAPPER_FUNC(13)
WRAPPER_FUNC(14)
WRAPPER_FUNC(15)

static dap_chain_tx_compose_callback_t s_wrappers[MAX_COMPOSE_CALLBACKS] = {
    compose_callback_wrapper_0, compose_callback_wrapper_1, compose_callback_wrapper_2, compose_callback_wrapper_3,
    compose_callback_wrapper_4, compose_callback_wrapper_5, compose_callback_wrapper_6, compose_callback_wrapper_7,
    compose_callback_wrapper_8, compose_callback_wrapper_9, compose_callback_wrapper_10, compose_callback_wrapper_11,
    compose_callback_wrapper_12, compose_callback_wrapper_13, compose_callback_wrapper_14, compose_callback_wrapper_15
};

/**
 * @brief Register Python compose callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (srv_uid, callback)
 * @return None
 */
PyObject* dap_chain_tx_compose_service_callback_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_srv_uid;
    PyObject *l_callback;
    
    if (!PyArg_ParseTuple(a_args, "KO", &l_srv_uid, &l_callback)) {
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }
    
    // Find free slot
    int slot = -1;
    for (int i = 0; i < MAX_COMPOSE_CALLBACKS; i++) {
        if (!s_compose_callback_occupied[i]) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Max compose callbacks limit reached");
        return NULL;
    }
    
    // Store callback
    Py_INCREF(l_callback);
    s_python_compose_callbacks[slot] = l_callback;
    s_compose_callback_occupied[slot] = true;
    
    // Register C wrapper
    dap_chain_tx_compose_service_callback_register((dap_chain_srv_uid_t){.uint64 = l_srv_uid}.uint64, s_wrappers[slot]);
    
    log_it(L_DEBUG, "Registered compose callback for service UID %llu at slot %d", l_srv_uid, slot);
    
    Py_RETURN_NONE;
}

// =============================================================================
// COMPOSE FUNCTIONS
// =============================================================================

// Destructor for config capsule
static void dap_chain_tx_compose_config_delete_py(PyObject *capsule) {
    dap_chain_tx_compose_config_t *config = (dap_chain_tx_compose_config_t *)PyCapsule_GetPointer(capsule, "dap_chain_tx_compose_config_t");
    if (config) {
        // DAP_DELETE(config); // Assuming standard free, check SDK if specific deinit needed. 
        // Most SDK structs are just freed if no deinit function provided.
        // dap_chain_tx_compose_config_init allocates with DAP_NEW_Z.
        DAP_DELETE(config);
    }
}

/**
 * @brief Initialize compose configuration
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id, net_name, native_ticker, url_str, port, enc_cert_path)
 * @return PyCapsule wrapping dap_chain_tx_compose_config_t*
 */
PyObject* dap_chain_tx_compose_config_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id_uint;
    const char *l_net_name;
    const char *l_native_ticker;
    const char *l_url_str;
    int l_port;
    const char *l_enc_cert_path;
    
    if (!PyArg_ParseTuple(a_args, "Ksssih", &l_net_id_uint, &l_net_name, &l_native_ticker, 
                          &l_url_str, &l_port, &l_enc_cert_path)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id = {.uint64 = l_net_id_uint};
    
    dap_chain_tx_compose_config_t *l_config = dap_chain_tx_compose_config_init(
        l_net_id, l_net_name, l_native_ticker, l_url_str, (uint16_t)l_port, l_enc_cert_path
    );
    
    if (!l_config) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to initialize compose config");
        return NULL;
    }
    
    return PyCapsule_New(l_config, "dap_chain_tx_compose_config_t", dap_chain_tx_compose_config_delete_py);
}

/**
 * @brief Create transaction datum (compose)
 * @param a_self Python self object (unused)
 * @param a_args Arguments (addr_from, addr_to_list, ticker, value, fee, tx_num, config, time_unlock)
 * @return PyCapsule wrapping dap_chain_datum_tx_t*
 */
PyObject* dap_chain_tx_compose_datum_tx_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_addr_from_obj;
    PyObject *l_addr_to_list;
    const char *l_ticker;
    unsigned long long l_value_uint;
    unsigned long long l_fee_uint;
    unsigned long long l_tx_num;
    PyObject *l_config_obj;
    unsigned long long l_time_unlock_uint = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOsKKKO|K", &l_addr_from_obj, &l_addr_to_list, &l_ticker,
                          &l_value_uint, &l_fee_uint, &l_tx_num, &l_config_obj, &l_time_unlock_uint)) {
        return NULL;
    }
    
    // Validate inputs
    if (!PyCapsule_CheckExact(l_addr_from_obj)) {
        PyErr_SetString(PyExc_TypeError, "addr_from must be a capsule");
        return NULL;
    }
    dap_chain_addr_t *l_addr_from = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_from_obj, "dap_chain_addr_t");
    
    if (!PyList_Check(l_addr_to_list)) {
        PyErr_SetString(PyExc_TypeError, "addr_to_list must be a list");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_config_obj)) {
        PyErr_SetString(PyExc_TypeError, "config must be a capsule");
        return NULL;
    }
    dap_chain_tx_compose_config_t *l_config = (dap_chain_tx_compose_config_t *)PyCapsule_GetPointer(l_config_obj, "dap_chain_tx_compose_config_t");
    
    // Prepare inputs for C function
    size_t l_count = PyList_Size(l_addr_to_list);
    dap_chain_addr_t **l_addr_to_arr = DAP_NEW_Z_COUNT(dap_chain_addr_t*, l_count);
    if (!l_addr_to_arr) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory");
        return NULL;
    }
    
    for (size_t i = 0; i < l_count; i++) {
        PyObject *l_item = PyList_GetItem(l_addr_to_list, i);
        if (!PyCapsule_CheckExact(l_item)) {
            DAP_DELETE(l_addr_to_arr);
            PyErr_SetString(PyExc_TypeError, "addr_to_list items must be capsules");
            return NULL;
        }
        l_addr_to_arr[i] = (dap_chain_addr_t *)PyCapsule_GetPointer(l_item, "dap_chain_addr_t");
    }
    
    uint256_t l_value = GET_256_FROM_64(l_value_uint);
    uint256_t l_fee = GET_256_FROM_64(l_fee_uint);
    dap_time_t l_time_unlock = (dap_time_t)l_time_unlock_uint;
    
    dap_chain_datum_tx_t *l_tx = dap_chain_tx_compose_datum_tx_create(
        l_addr_from, l_addr_to_arr, l_ticker, &l_value, &l_time_unlock, l_fee, (size_t)l_tx_num, l_config
    );
    
    DAP_DELETE(l_addr_to_arr);
    
    if (!l_tx) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to compose transaction");
        return NULL;
    }
    
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

/**
 * @brief Get method definitions for compose module
 */
PyMethodDef* cellframe_compose_get_methods(void) {
    static PyMethodDef compose_methods[] = {
        {"tx_compose_config_init", (PyCFunction)dap_chain_tx_compose_config_init_py, METH_VARARGS,
         "Initialize compose configuration"},
        {"tx_compose_datum_tx_create", (PyCFunction)dap_chain_tx_compose_datum_tx_create_py, METH_VARARGS,
         "Create transaction datum"},
        {"tx_compose_service_callback_register", (PyCFunction)dap_chain_tx_compose_service_callback_register_py, METH_VARARGS,
         "Register service compose callback"},
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return compose_methods;
}

