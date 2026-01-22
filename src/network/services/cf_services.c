#include "cellframe.h"
#include "dap_chain_net_srv.h"
#include "dap_chain_net_srv_order.h"
#include "dap_chain_net.h"
#include "dap_chain_common.h"
#include "dap_enc_key.h"

#define LOG_TAG "python_cellframe_services"

// =============================================================================
// SERVICE BASE OPERATIONS
// =============================================================================

/**
 * @brief Initialize network services subsystem
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_net_srv_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    int l_result = dap_chain_net_srv_init();
    
    log_it(L_DEBUG, "Network services subsystem initialized, result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize network services subsystem
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_chain_net_srv_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    dap_chain_net_srv_deinit();
    
    log_it(L_DEBUG, "Network services subsystem deinitialized");
    Py_RETURN_NONE;
}

/**
 * @brief Create service instance
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id uint64, srv_uid uint64, config capsule, callbacks capsule or None)
 * @return PyCapsule wrapping dap_chain_net_srv_t* or None
 */
PyObject* dap_chain_net_srv_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id_uint = 0;
    unsigned long long l_srv_uid_uint = 0;
    PyObject *l_config_obj = NULL;
    PyObject *l_callbacks_obj = NULL;
    
    if (!PyArg_ParseTuple(a_args, "KKO|O", &l_net_id_uint, &l_srv_uid_uint, &l_config_obj, &l_callbacks_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_config_obj)) {
        PyErr_SetString(PyExc_TypeError, "Third argument must be a config capsule");
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id = {.uint64 = l_net_id_uint};
    dap_chain_srv_uid_t l_srv_uid = {.uint64 = l_srv_uid_uint};
    dap_config_t *l_config = (dap_config_t *)PyCapsule_GetPointer(l_config_obj, "dap_config_t");
    
    if (!l_config) {
        PyErr_SetString(PyExc_ValueError, "Invalid config capsule");
        return NULL;
    }
    
    dap_chain_net_srv_callbacks_t *l_callbacks = NULL;
    if (l_callbacks_obj && l_callbacks_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_callbacks_obj)) {
            PyErr_SetString(PyExc_TypeError, "Fourth argument must be a callbacks capsule or None");
            return NULL;
        }
        l_callbacks = (dap_chain_net_srv_callbacks_t *)PyCapsule_GetPointer(l_callbacks_obj, "dap_chain_net_srv_callbacks_t");
        if (!l_callbacks) {
            PyErr_SetString(PyExc_ValueError, "Invalid callbacks capsule");
            return NULL;
        }
    }
    
    dap_chain_net_srv_t *l_srv = dap_chain_net_srv_create(l_net_id, l_srv_uid, l_config, l_callbacks);
    if (!l_srv) {
        log_it(L_ERROR, "Failed to create service");
        PyErr_SetString(PyExc_RuntimeError, "Failed to create service");
        return NULL;
    }
    
    log_it(L_DEBUG, "Service created successfully");
    return PyCapsule_New(l_srv, "dap_chain_net_srv_t", NULL);
}

/**
 * @brief Delete service instance
 * @param a_self Python self object (unused)
 * @param a_args Arguments (service capsule)
 * @return None
 */
PyObject* dap_chain_net_srv_del_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_srv_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_srv_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_srv_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a service capsule");
        return NULL;
    }
    
    dap_chain_net_srv_t *l_srv = (dap_chain_net_srv_t *)PyCapsule_GetPointer(l_srv_obj, "dap_chain_net_srv_t");
    if (!l_srv) {
        PyErr_SetString(PyExc_ValueError, "Invalid service capsule");
        return NULL;
    }
    
    dap_chain_net_srv_del(l_srv);
    
    log_it(L_DEBUG, "Service deleted");
    Py_RETURN_NONE;
}

// =============================================================================
// ORDER SERVICE OPERATIONS
// =============================================================================

/**
 * @brief Initialize order service subsystem
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_net_srv_order_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    int l_result = dap_chain_net_srv_order_init();
    
    log_it(L_DEBUG, "Order service subsystem initialized, result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize order service subsystem
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_chain_net_srv_order_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    dap_chain_net_srv_order_deinit();
    
    log_it(L_DEBUG, "Order service subsystem deinitialized");
    Py_RETURN_NONE;
}

/**
 * @brief Find order by hash string
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule, hash_str string)
 * @return PyCapsule wrapping dap_chain_net_srv_order_t* or None
 */
PyObject* dap_chain_net_srv_order_find_by_hash_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj;
    const char *l_hash_str;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_net_obj, &l_hash_str)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    
    dap_chain_net_srv_order_t *l_order = dap_chain_net_srv_order_find_by_hash_str(l_net, l_hash_str);
    if (!l_order) {
        log_it(L_DEBUG, "Order not found: %s", l_hash_str);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Order found: %s", l_hash_str);
    return PyCapsule_New(l_order, "dap_chain_net_srv_order_t", NULL);
}

/**
 * @brief Delete order by hash string
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule, hash_str string)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_net_srv_order_delete_by_hash_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj;
    const char *l_hash_str;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_net_obj, &l_hash_str)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    
    int l_result = dap_chain_net_srv_order_delete_by_hash_str_sync(l_net, l_hash_str);
    
    log_it(L_DEBUG, "Order delete result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Get order size
 * @param a_self Python self object (unused)
 * @param a_args Arguments (order capsule)
 * @return Order size as uint64
 */
PyObject* dap_chain_net_srv_order_get_size_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_order_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_order_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_order_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an order capsule");
        return NULL;
    }
    
    dap_chain_net_srv_order_t *l_order = (dap_chain_net_srv_order_t *)PyCapsule_GetPointer(l_order_obj, "dap_chain_net_srv_order_t");
    if (!l_order) {
        PyErr_SetString(PyExc_ValueError, "Invalid order capsule");
        return NULL;
    }
    
    uint64_t l_size = dap_chain_net_srv_order_get_size(l_order);
    
    log_it(L_DEBUG, "Order size: %llu", (unsigned long long)l_size);
    return PyLong_FromUnsignedLongLong(l_size);
}

/**
 * @brief Find all orders by criteria
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule, direction int, srv_uid uint64, price_unit int, price_ticker string, price_min bytes, price_max bytes)
 * @return List of order capsules or empty list
 */
PyObject* dap_chain_net_srv_order_find_all_by_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj;
    int l_direction;
    unsigned long long l_srv_uid_uint = 0;
    int l_price_unit_enm = 0;
    const char *l_price_ticker = NULL;
    const char *l_price_min_bytes = NULL;
    Py_ssize_t l_price_min_size = 0;
    const char *l_price_max_bytes = NULL;
    Py_ssize_t l_price_max_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "OiK|isYs#Ys#", &l_net_obj, &l_direction, &l_srv_uid_uint,
                          &l_price_unit_enm, &l_price_ticker, 
                          &l_price_min_bytes, &l_price_min_size,
                          &l_price_max_bytes, &l_price_max_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    
    dap_chain_net_srv_order_direction_t l_dir = (dap_chain_net_srv_order_direction_t)l_direction;
    dap_chain_srv_uid_t l_srv_uid = {.uint64 = l_srv_uid_uint};
    dap_chain_net_srv_price_unit_uid_t l_price_unit = {.enm = (int)l_price_unit_enm};
    
    uint256_t l_price_min = GET_256_FROM_64(0);
    uint256_t l_price_max = GET_256_FROM_64(0);
    
    if (l_price_min_bytes && l_price_min_size == sizeof(uint256_t)) {
        memcpy(&l_price_min, l_price_min_bytes, sizeof(uint256_t));
    }
    if (l_price_max_bytes && l_price_max_size == sizeof(uint256_t)) {
        memcpy(&l_price_max, l_price_max_bytes, sizeof(uint256_t));
    }
    
    char l_ticker[DAP_CHAIN_TICKER_SIZE_MAX] = {0};
    if (l_price_ticker) {
        strncpy(l_ticker, l_price_ticker, DAP_CHAIN_TICKER_SIZE_MAX - 1);
    }
    
    dap_list_t *l_orders = NULL;
    size_t l_orders_count = 0;
    
    int l_result = dap_chain_net_srv_order_find_all_by(l_net, l_dir, l_srv_uid, l_price_unit,
                                                        l_ticker, l_price_min, l_price_max,
                                                        &l_orders, &l_orders_count);
    
    if (l_result != 0) {
        log_it(L_ERROR, "Failed to find orders, result: %d", l_result);
        PyErr_SetString(PyExc_RuntimeError, "Failed to find orders");
        return NULL;
    }
    
    PyObject *l_list = PyList_New(0);
    if (!l_list) {
        return NULL;
    }
    
    dap_list_t *l_item = l_orders;
    while (l_item) {
        dap_chain_net_srv_order_t *l_order = (dap_chain_net_srv_order_t *)l_item->data;
        if (l_order) {
            PyObject *l_order_capsule = PyCapsule_New(l_order, "dap_chain_net_srv_order_t", NULL);
            if (l_order_capsule) {
                PyList_Append(l_list, l_order_capsule);
                Py_DECREF(l_order_capsule);
            }
        }
        l_item = l_item->next;
    }
    
    log_it(L_DEBUG, "Found %zu orders", l_orders_count);
    return l_list;
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

/**
 * @brief Get method definitions for services module
 */
PyMethodDef* cellframe_services_get_methods(void) {
    static PyMethodDef services_methods[] = {
        // Service base operations
        {"net_srv_init", (PyCFunction)dap_chain_net_srv_init_py, METH_VARARGS,
         "Initialize network services subsystem"},
        {"net_srv_deinit", (PyCFunction)dap_chain_net_srv_deinit_py, METH_VARARGS,
         "Deinitialize network services subsystem"},
        {"net_srv_create", (PyCFunction)dap_chain_net_srv_create_py, METH_VARARGS,
         "Create service instance"},
        {"net_srv_del", (PyCFunction)dap_chain_net_srv_del_py, METH_VARARGS,
         "Delete service instance"},
        
        // Order service operations
        {"net_srv_order_init", (PyCFunction)dap_chain_net_srv_order_init_py, METH_VARARGS,
         "Initialize order service subsystem"},
        {"net_srv_order_deinit", (PyCFunction)dap_chain_net_srv_order_deinit_py, METH_VARARGS,
         "Deinitialize order service subsystem"},
        {"net_srv_order_find_by_hash_str", (PyCFunction)dap_chain_net_srv_order_find_by_hash_str_py, METH_VARARGS,
         "Find order by hash string"},
        {"net_srv_order_delete_by_hash_str", (PyCFunction)dap_chain_net_srv_order_delete_by_hash_str_py, METH_VARARGS,
         "Delete order by hash string"},
        {"net_srv_order_get_size", (PyCFunction)dap_chain_net_srv_order_get_size_py, METH_VARARGS,
         "Get order size"},
        {"net_srv_order_find_all_by", (PyCFunction)dap_chain_net_srv_order_find_all_by_py, METH_VARARGS,
         "Find all orders by criteria"},
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return services_methods;
}

