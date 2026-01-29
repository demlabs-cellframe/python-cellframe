#include "cellframe.h"
#include <string.h>
#include "dap_chain_net_srv.h"
#include "dap_chain_net_srv_order.h"
#include "dap_chain_net_srv_client.h"
#include "dap_chain_net_srv_ch.h"
#include "dap_chain_net_srv_ch_pkt.h"
#include "dap_chain_net_srv_stream_session.h"
#include "dap_chain_net_srv_tx.h"
#include "dap_chain_net_srv_common.h"
#include "dap_chain_net_srv_geoip.h"
#include "dap_chain_net.h"
#include "dap_chain_common.h"
#include "dap_chain_datum_tx_receipt.h"
#include "dap_enc_key.h"
#include "dap_json.h"
#include "dap_string.h"
#include "dap_sign.h"
#include "../datum/cf_datum_internal.h"

#define LOG_TAG "python_cellframe_services"

typedef struct cf_services_order_args {
    dap_chain_net_t *net;
    dap_chain_net_srv_order_direction_t direction;
    dap_chain_srv_uid_t srv_uid;
    dap_chain_node_addr_t node_addr;
    dap_hash_fast_t tx_cond_hash;
    uint256_t price;
    dap_chain_net_srv_price_unit_uid_t price_unit;
    char price_ticker[DAP_CHAIN_TICKER_SIZE_MAX];
    dap_time_t expires;
    const uint8_t *ext;
    uint32_t ext_size;
    uint64_t units;
    const char *region;
    int8_t continent_num;
    dap_enc_key_t *key;
} cf_services_order_args_t;

static void cf_services_order_capsule_destructor(PyObject *capsule) {
    dap_chain_net_srv_order_t *order = (dap_chain_net_srv_order_t *)PyCapsule_GetPointer(
        capsule, "dap_chain_net_srv_order_t");
    if (order) {
        DAP_DELETE(order);
    }
}

static void cf_services_price_capsule_destructor(PyObject *capsule) {
    dap_chain_net_srv_price_t *price = (dap_chain_net_srv_price_t *)PyCapsule_GetPointer(
        capsule, "dap_chain_net_srv_price_t");
    if (price) {
        DAP_DELETE(price);
    }
}

static int cf_services_parse_order_args(PyObject *a_args, cf_services_order_args_t *out) {
    PyObject *l_net_obj = NULL;
    int l_direction = 0;
    unsigned long long l_srv_uid = 0;
    PyObject *l_node_addr_obj = NULL;
    PyObject *l_tx_cond_hash_obj = NULL;
    PyObject *l_price_obj = NULL;
    int l_price_unit = 0;
    const char *l_price_ticker = NULL;
    unsigned long long l_expires = 0;
    PyObject *l_ext_obj = NULL;
    unsigned long long l_units = 0;
    PyObject *l_region_obj = NULL;
    int l_continent_num = 0;
    PyObject *l_key_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OiKOOOisKOKOiO",
                          &l_net_obj, &l_direction, &l_srv_uid, &l_node_addr_obj,
                          &l_tx_cond_hash_obj, &l_price_obj, &l_price_unit, &l_price_ticker,
                          &l_expires, &l_ext_obj, &l_units, &l_region_obj, &l_continent_num, &l_key_obj)) {
        return -1;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return -1;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return -1;
    }

    if (!PyCapsule_CheckExact(l_key_obj)) {
        PyErr_SetString(PyExc_TypeError, "Key must be a dap_enc_key_t capsule");
        return -1;
    }

    dap_enc_key_t *l_key = (dap_enc_key_t *)PyCapsule_GetPointer(l_key_obj, "dap_enc_key_t");
    if (!l_key) {
        PyErr_SetString(PyExc_ValueError, "Invalid key capsule");
        return -1;
    }

    dap_chain_node_addr_t l_node_addr = {};
    if (cf_datum_parse_node_addr(l_node_addr_obj, &l_node_addr) != 0) {
        return -1;
    }

    dap_hash_fast_t l_tx_cond_hash = {};
    if (cf_datum_parse_hash_fast(l_tx_cond_hash_obj, &l_tx_cond_hash) != 0) {
        return -1;
    }

    uint256_t l_price = {};
    if (cf_datum_parse_uint256(l_price_obj, &l_price) != 0) {
        return -1;
    }

    const uint8_t *l_ext = NULL;
    uint32_t l_ext_size = 0;
    if (l_ext_obj && l_ext_obj != Py_None) {
        if (!PyBytes_Check(l_ext_obj)) {
            PyErr_SetString(PyExc_TypeError, "ext must be bytes or None");
            return -1;
        }
        Py_ssize_t l_ext_len = 0;
        const char *l_ext_bytes = NULL;
        if (PyBytes_AsStringAndSize(l_ext_obj, (char **)&l_ext_bytes, &l_ext_len) < 0) {
            return -1;
        }
        l_ext = (const uint8_t *)l_ext_bytes;
        l_ext_size = (uint32_t)l_ext_len;
    }

    const char *l_region = NULL;
    if (l_region_obj && l_region_obj != Py_None) {
        if (PyUnicode_Check(l_region_obj)) {
            l_region = PyUnicode_AsUTF8(l_region_obj);
        } else if (PyBytes_Check(l_region_obj)) {
            l_region = PyBytes_AsString(l_region_obj);
        } else {
            PyErr_SetString(PyExc_TypeError, "region must be a string, bytes, or None");
            return -1;
        }
        if (!l_region) {
            return -1;
        }
    }

    memset(out, 0, sizeof(*out));
    out->net = l_net;
    out->direction = (dap_chain_net_srv_order_direction_t)l_direction;
    out->srv_uid.uint64 = l_srv_uid;
    out->node_addr = l_node_addr;
    out->tx_cond_hash = l_tx_cond_hash;
    out->price = l_price;
    out->price_unit.enm = l_price_unit;
    if (l_price_ticker) {
        strncpy(out->price_ticker, l_price_ticker, DAP_CHAIN_TICKER_SIZE_MAX - 1);
    }
    out->expires = (dap_time_t)l_expires;
    out->ext = l_ext;
    out->ext_size = l_ext_size;
    out->units = l_units;
    out->region = l_region;
    out->continent_num = (int8_t)l_continent_num;
    out->key = l_key;
    return 0;
}

static dap_chain_id_t cf_services_get_chain_id_by_name_cb(dap_chain_net_id_t a_net_id, const char *a_chain_name) {
    if (!a_chain_name) {
        return (dap_chain_id_t){.uint64 = 0};
    }
    dap_chain_net_t *net = dap_chain_net_by_id(a_net_id);
    if (!net) {
        return (dap_chain_id_t){.uint64 = 0};
    }
    dap_chain_t *chain = dap_chain_net_get_chain_by_name(net, a_chain_name);
    return chain ? chain->id : (dap_chain_id_t){.uint64 = 0};
}

static dap_chain_id_t cf_services_get_chain_id_by_type_cb(dap_chain_net_id_t a_net_id, dap_chain_type_t a_chain_type) {
    dap_chain_net_t *net = dap_chain_net_by_id(a_net_id);
    if (!net) {
        return (dap_chain_id_t){.uint64 = 0};
    }
    dap_chain_t *chain = dap_chain_net_get_default_chain_by_chain_type(net, a_chain_type);
    return chain ? chain->id : (dap_chain_id_t){.uint64 = 0};
}

static dap_chain_net_id_t cf_services_get_net_id_by_name_cb(const char *a_net_name) {
    return dap_chain_net_id_by_name(a_net_name);
}

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
// SERVICE PRICING / RECEIPT
// =============================================================================

PyObject* dap_chain_net_srv_issue_receipt_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_srv_obj = NULL;
    PyObject *l_price_obj = NULL;
    PyObject *l_ext_obj = Py_None;
    PyObject *l_prev_hash_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "OO|OO", &l_srv_obj, &l_price_obj, &l_ext_obj, &l_prev_hash_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_srv_obj) || !PyCapsule_CheckExact(l_price_obj)) {
        PyErr_SetString(PyExc_TypeError, "First two arguments must be service and price capsules");
        return NULL;
    }

    dap_chain_net_srv_t *l_srv = (dap_chain_net_srv_t *)PyCapsule_GetPointer(l_srv_obj, "dap_chain_net_srv_t");
    dap_chain_net_srv_price_t *l_price = (dap_chain_net_srv_price_t *)PyCapsule_GetPointer(
        l_price_obj, "dap_chain_net_srv_price_t");
    if (!l_srv || !l_price) {
        PyErr_SetString(PyExc_ValueError, "Invalid service or price capsule");
        return NULL;
    }

    const void *l_ext = NULL;
    size_t l_ext_size = 0;
    if (l_ext_obj && l_ext_obj != Py_None) {
        if (!PyBytes_Check(l_ext_obj)) {
            PyErr_SetString(PyExc_TypeError, "ext must be bytes or None");
            return NULL;
        }
        Py_ssize_t l_size = 0;
        if (PyBytes_AsStringAndSize(l_ext_obj, (char **)&l_ext, &l_size) < 0) {
            return NULL;
        }
        l_ext_size = (size_t)l_size;
    }

    dap_hash_fast_t l_prev_hash = {};
    dap_hash_fast_t *l_prev_hash_ptr = NULL;
    if (l_prev_hash_obj && l_prev_hash_obj != Py_None) {
        if (cf_datum_parse_hash_fast(l_prev_hash_obj, &l_prev_hash) != 0) {
            return NULL;
        }
        l_prev_hash_ptr = &l_prev_hash;
    }

    dap_chain_datum_tx_receipt_t *l_receipt = dap_chain_net_srv_issue_receipt(
        l_srv, l_price, l_ext, l_ext_size, l_prev_hash_ptr);
    if (!l_receipt) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_receipt, "dap_chain_datum_tx_receipt_t", NULL);
}

PyObject* dap_chain_net_srv_get_price_from_order_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_srv_obj = NULL;
    PyObject *l_order_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_srv_obj, &l_order_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_srv_obj) || !PyCapsule_CheckExact(l_order_obj)) {
        PyErr_SetString(PyExc_TypeError, "Arguments must be service and order capsules");
        return NULL;
    }

    dap_chain_net_srv_t *l_srv = (dap_chain_net_srv_t *)PyCapsule_GetPointer(l_srv_obj, "dap_chain_net_srv_t");
    dap_chain_net_srv_order_t *l_order = (dap_chain_net_srv_order_t *)PyCapsule_GetPointer(
        l_order_obj, "dap_chain_net_srv_order_t");
    if (!l_srv || !l_order) {
        PyErr_SetString(PyExc_ValueError, "Invalid service or order capsule");
        return NULL;
    }

    dap_chain_net_srv_price_t *l_price = dap_chain_net_srv_get_price_from_order(l_srv, l_order);
    if (!l_price) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_price, "dap_chain_net_srv_price_t", cf_services_price_capsule_destructor);
}

PyObject* dap_chain_net_srv_price_unit_uid_from_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_unit_str = NULL;

    if (!PyArg_ParseTuple(a_args, "s", &l_unit_str)) {
        return NULL;
    }

    dap_chain_net_srv_price_unit_uid_t l_unit = dap_chain_net_srv_price_unit_uid_from_str(l_unit_str);
    return PyLong_FromLong(l_unit.enm);
}

PyObject* dap_chain_net_srv_price_unit_uid_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_unit = 0;

    if (!PyArg_ParseTuple(a_args, "i", &l_unit)) {
        return NULL;
    }

    dap_chain_net_srv_price_unit_uid_t l_uid = {.enm = l_unit};
    const char *l_str = dap_chain_net_srv_price_unit_uid_to_str(l_uid);
    return PyUnicode_FromString(l_str ? l_str : "UNKNOWN");
}

PyObject* dap_chain_net_srv_uid_compare_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_uid_a = 0;
    unsigned long long l_uid_b = 0;

    if (!PyArg_ParseTuple(a_args, "KK", &l_uid_a, &l_uid_b)) {
        return NULL;
    }

    dap_chain_srv_uid_t l_a = {.uint64 = l_uid_a};
    dap_chain_srv_uid_t l_b = {.uint64 = l_uid_b};
    return PyBool_FromLong(dap_chain_net_srv_uid_compare(l_a, l_b));
}

PyObject* dap_chain_net_srv_uid_compare_scalar_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_uid = 0;
    unsigned long long l_id = 0;

    if (!PyArg_ParseTuple(a_args, "KK", &l_uid, &l_id)) {
        return NULL;
    }

    dap_chain_srv_uid_t l_a = {.uint64 = l_uid};
    return PyBool_FromLong(dap_chain_net_srv_uid_compare_scalar(l_a, l_id));
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
    return PyCapsule_New(l_order, "dap_chain_net_srv_order_t", cf_services_order_capsule_destructor);
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
            PyObject *l_order_capsule = PyCapsule_New(l_order, "dap_chain_net_srv_order_t",
                                                     cf_services_order_capsule_destructor);
            if (l_order_capsule) {
                PyList_Append(l_list, l_order_capsule);
                Py_DECREF(l_order_capsule);
            }
        }
        l_item = l_item->next;
    }
    
    dap_list_free(l_orders);
    log_it(L_DEBUG, "Found %zu orders", l_orders_count);
    return l_list;
}

// =============================================================================
// ORDER EXTENDED OPERATIONS
// =============================================================================

PyObject* dap_chain_net_srv_order_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_hash_str = NULL;
    const char *l_order_bytes = NULL;
    Py_ssize_t l_order_size = 0;

    if (!PyArg_ParseTuple(a_args, "sy#", &l_hash_str, &l_order_bytes, &l_order_size)) {
        return NULL;
    }

    const dap_chain_net_srv_order_t *l_order = dap_chain_net_srv_order_check(
        l_hash_str, (const byte_t *)l_order_bytes, (size_t)l_order_size);
    return PyBool_FromLong(l_order != NULL);
}

PyObject* dap_chain_net_srv_order_compose_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    cf_services_order_args_t l_args = {};
    if (cf_services_parse_order_args(a_args, &l_args) != 0) {
        return NULL;
    }

    dap_chain_net_srv_order_t *l_order = dap_chain_net_srv_order_compose(
        l_args.net, l_args.direction, l_args.srv_uid, l_args.node_addr, l_args.tx_cond_hash,
        &l_args.price, l_args.price_unit, l_args.price_ticker, l_args.expires,
        l_args.ext, l_args.ext_size, l_args.units, l_args.region, l_args.continent_num, l_args.key);

    if (!l_order) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_order, "dap_chain_net_srv_order_t", cf_services_order_capsule_destructor);
}

PyObject* dap_chain_net_srv_order_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    cf_services_order_args_t l_args = {};
    if (cf_services_parse_order_args(a_args, &l_args) != 0) {
        return NULL;
    }

    char *l_hash_str = dap_chain_net_srv_order_create(
        l_args.net, l_args.direction, l_args.srv_uid, l_args.node_addr, l_args.tx_cond_hash,
        &l_args.price, l_args.price_unit, l_args.price_ticker, l_args.expires,
        l_args.ext, l_args.ext_size, l_args.units, l_args.region, l_args.continent_num, l_args.key);

    if (!l_hash_str) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = PyUnicode_FromString(l_hash_str);
    DAP_DELETE(l_hash_str);
    return l_result;
}

PyObject* dap_chain_net_srv_order_save_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_order_obj = NULL;
    int l_common = 0;

    if (!PyArg_ParseTuple(a_args, "OO|p", &l_net_obj, &l_order_obj, &l_common)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj) || !PyCapsule_CheckExact(l_order_obj)) {
        PyErr_SetString(PyExc_TypeError, "Arguments must be network and order capsules");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    dap_chain_net_srv_order_t *l_order = (dap_chain_net_srv_order_t *)PyCapsule_GetPointer(
        l_order_obj, "dap_chain_net_srv_order_t");
    if (!l_net || !l_order) {
        PyErr_SetString(PyExc_ValueError, "Invalid network or order capsule");
        return NULL;
    }

    char *l_hash_str = dap_chain_net_srv_order_save(l_net, l_order, (bool)l_common);
    if (!l_hash_str) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = PyUnicode_FromString(l_hash_str);
    DAP_DELETE(l_hash_str);
    return l_result;
}

PyObject* dap_chain_net_srv_order_dump_to_string_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_order_obj = NULL;
    const char *l_hash_out_type = NULL;
    PyObject *l_native_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "Os|O", &l_order_obj, &l_hash_out_type, &l_native_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_order_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an order capsule");
        return NULL;
    }

    dap_chain_net_srv_order_t *l_order = (dap_chain_net_srv_order_t *)PyCapsule_GetPointer(
        l_order_obj, "dap_chain_net_srv_order_t");
    if (!l_order) {
        PyErr_SetString(PyExc_ValueError, "Invalid order capsule");
        return NULL;
    }

    const char *l_native_ticker = NULL;
    if (l_native_obj && l_native_obj != Py_None) {
        if (PyUnicode_Check(l_native_obj)) {
            l_native_ticker = PyUnicode_AsUTF8(l_native_obj);
        } else if (PyBytes_Check(l_native_obj)) {
            l_native_ticker = PyBytes_AsString(l_native_obj);
        } else {
            PyErr_SetString(PyExc_TypeError, "native_ticker must be a string, bytes, or None");
            return NULL;
        }
        if (!l_native_ticker) {
            return NULL;
        }
    }

    dap_string_t *l_str = dap_string_new("");
    if (!l_str) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate output string");
        return NULL;
    }

    dap_chain_net_srv_order_dump_to_string(l_order, l_str, l_hash_out_type, l_native_ticker);
    char *l_out = dap_string_free(l_str, false);
    if (!l_out) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = PyUnicode_FromString(l_out);
    DAP_DELETE(l_out);
    return l_result;
}

PyObject* dap_chain_net_srv_order_dump_to_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_order_obj = NULL;
    const char *l_hash_out_type = NULL;
    PyObject *l_native_obj = Py_None;
    int l_version = 1;

    if (!PyArg_ParseTuple(a_args, "Os|Oi", &l_order_obj, &l_hash_out_type, &l_native_obj, &l_version)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_order_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an order capsule");
        return NULL;
    }

    dap_chain_net_srv_order_t *l_order = (dap_chain_net_srv_order_t *)PyCapsule_GetPointer(
        l_order_obj, "dap_chain_net_srv_order_t");
    if (!l_order) {
        PyErr_SetString(PyExc_ValueError, "Invalid order capsule");
        return NULL;
    }

    const char *l_native_ticker = NULL;
    if (l_native_obj && l_native_obj != Py_None) {
        if (PyUnicode_Check(l_native_obj)) {
            l_native_ticker = PyUnicode_AsUTF8(l_native_obj);
        } else if (PyBytes_Check(l_native_obj)) {
            l_native_ticker = PyBytes_AsString(l_native_obj);
        } else {
            PyErr_SetString(PyExc_TypeError, "native_ticker must be a string, bytes, or None");
            return NULL;
        }
        if (!l_native_ticker) {
            return NULL;
        }
    }

    dap_json_t *l_json = dap_json_object_new();
    if (!l_json) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate JSON object");
        return NULL;
    }

    dap_chain_net_srv_order_dump_to_json(l_order, l_json, l_hash_out_type, l_native_ticker, l_version);
    char *l_json_str = dap_json_to_string(l_json);
    dap_json_object_free(l_json);
    if (!l_json_str) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = PyUnicode_FromString(l_json_str);
    DAP_DELETE(l_json_str);
    return l_result;
}

PyObject* dap_chain_net_srv_order_get_sign_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_order_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_order_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_order_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an order capsule");
        return NULL;
    }

    dap_chain_net_srv_order_t *l_order = (dap_chain_net_srv_order_t *)PyCapsule_GetPointer(
        l_order_obj, "dap_chain_net_srv_order_t");
    if (!l_order) {
        PyErr_SetString(PyExc_ValueError, "Invalid order capsule");
        return NULL;
    }

    dap_sign_t *l_sign = dap_chain_net_srv_order_get_sign(l_order);
    if (!l_sign) {
        Py_RETURN_NONE;
    }

    size_t l_size = dap_sign_get_size(l_sign);
    if (!l_size) {
        Py_RETURN_NONE;
    }

    return PyBytes_FromStringAndSize((const char *)l_sign, (Py_ssize_t)l_size);
}

PyObject* dap_chain_net_srv_order_get_gdb_group_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
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

    char *l_group = dap_chain_net_srv_order_get_gdb_group(l_net);
    if (!l_group) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = PyUnicode_FromString(l_group);
    DAP_DELETE(l_group);
    return l_result;
}

PyObject* dap_chain_net_srv_order_get_common_group_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
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

    char *l_group = dap_chain_net_srv_order_get_common_group(l_net);
    if (!l_group) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = PyUnicode_FromString(l_group);
    DAP_DELETE(l_group);
    return l_result;
}

PyObject* dap_chain_net_srv_order_set_continent_region_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_order_obj = NULL;
    int l_continent = 0;
    PyObject *l_region_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "Oi|O", &l_order_obj, &l_continent, &l_region_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_order_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an order capsule");
        return NULL;
    }

    dap_chain_net_srv_order_t *l_order = (dap_chain_net_srv_order_t *)PyCapsule_GetPointer(
        l_order_obj, "dap_chain_net_srv_order_t");
    if (!l_order) {
        PyErr_SetString(PyExc_ValueError, "Invalid order capsule");
        return NULL;
    }

    const char *l_region = NULL;
    if (l_region_obj && l_region_obj != Py_None) {
        if (PyUnicode_Check(l_region_obj)) {
            l_region = PyUnicode_AsUTF8(l_region_obj);
        } else if (PyBytes_Check(l_region_obj)) {
            l_region = PyBytes_AsString(l_region_obj);
        } else {
            PyErr_SetString(PyExc_TypeError, "region must be a string, bytes, or None");
            return NULL;
        }
        if (!l_region) {
            return NULL;
        }
    }

    dap_chain_net_srv_order_t *l_order_ptr = l_order;
    bool l_rc = dap_chain_net_srv_order_set_continent_region(&l_order_ptr, (uint8_t)l_continent, l_region);
    if (!l_rc) {
        return PyBool_FromLong(0);
    }

    if (l_order_ptr != l_order) {
        if (PyCapsule_SetPointer(l_order_obj, l_order_ptr) != 0) {
            return NULL;
        }
    }
    return PyBool_FromLong(1);
}

PyObject* dap_chain_net_srv_order_get_continent_region_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_order_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_order_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_order_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an order capsule");
        return NULL;
    }

    dap_chain_net_srv_order_t *l_order = (dap_chain_net_srv_order_t *)PyCapsule_GetPointer(
        l_order_obj, "dap_chain_net_srv_order_t");
    if (!l_order) {
        PyErr_SetString(PyExc_ValueError, "Invalid order capsule");
        return NULL;
    }

    uint8_t l_continent = 0;
    char *l_region = NULL;
    if (!dap_chain_net_srv_order_get_continent_region(l_order, &l_continent, &l_region)) {
        Py_RETURN_NONE;
    }

    PyObject *l_region_obj = l_region ? PyUnicode_FromString(l_region) : Py_None;
    if (!l_region_obj) {
        DAP_DELETE(l_region);
        return NULL;
    }

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromUnsignedLong(l_continent));
    if (l_region) {
        PyTuple_SetItem(l_tuple, 1, l_region_obj);
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 1, Py_None);
    }
    DAP_DELETE(l_region);
    return l_tuple;
}

PyObject* dap_chain_net_srv_order_get_country_code_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_order_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_order_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_order_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an order capsule");
        return NULL;
    }

    dap_chain_net_srv_order_t *l_order = (dap_chain_net_srv_order_t *)PyCapsule_GetPointer(
        l_order_obj, "dap_chain_net_srv_order_t");
    if (!l_order) {
        PyErr_SetString(PyExc_ValueError, "Invalid order capsule");
        return NULL;
    }

    const char *l_code = dap_chain_net_srv_order_get_country_code(l_order);
    if (!l_code) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(l_code);
}

PyObject* dap_chain_net_srv_order_continents_count_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    size_t l_count = dap_chain_net_srv_order_continents_count();
    return PyLong_FromSize_t(l_count);
}

PyObject* dap_chain_net_srv_order_continent_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_num = 0;

    if (!PyArg_ParseTuple(a_args, "i", &l_num)) {
        return NULL;
    }

    const char *l_str = dap_chain_net_srv_order_continent_to_str((int8_t)l_num);
    if (!l_str) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(l_str);
}

PyObject* dap_chain_net_srv_order_continent_to_num_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_continent_str = NULL;

    if (!PyArg_ParseTuple(a_args, "s", &l_continent_str)) {
        return NULL;
    }

    int8_t l_num = dap_chain_net_srv_order_continent_to_num(l_continent_str);
    return PyLong_FromLong(l_num);
}

PyObject* dap_chain_srv_order_pin_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_srv_order_pin_init();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_order_find_by_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_hash_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_net_obj, &l_hash_obj)) {
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

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_chain_net_srv_order_t *l_order = dap_chain_net_srv_order_find_by_hash(l_net, &l_hash);
    if (!l_order) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_order, "dap_chain_net_srv_order_t", cf_services_order_capsule_destructor);
}

PyObject* dap_chain_net_srv_order_delete_by_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_hash_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_net_obj, &l_hash_obj)) {
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

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    int l_rc = dap_chain_net_srv_order_delete_by_hash(l_net, &l_hash);
    return PyLong_FromLong(l_rc);
}

// =============================================================================
// SERVICE CLIENT OPERATIONS
// =============================================================================

PyObject* dap_chain_net_srv_client_connect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    const char *l_addr = NULL;
    unsigned int l_port = 0;
    int l_timeout_ms = 0;

    if (!PyArg_ParseTuple(a_args, "OsIi", &l_net_obj, &l_addr, &l_port, &l_timeout_ms)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    if (l_port > UINT16_MAX) {
        PyErr_SetString(PyExc_ValueError, "port must be between 0 and 65535");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_net_srv_client_t *l_client = dap_chain_net_srv_client_connect(
        l_net, l_addr, (uint16_t)l_port, l_timeout_ms);
    if (!l_client) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_client, "dap_chain_net_srv_client_t", NULL);
}

PyObject* dap_chain_net_srv_client_close_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_client_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_client_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_client_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a service client capsule");
        return NULL;
    }

    dap_chain_net_srv_client_t *l_client = (dap_chain_net_srv_client_t *)PyCapsule_GetPointer(
        l_client_obj, "dap_chain_net_srv_client_t");
    if (!l_client) {
        PyErr_SetString(PyExc_ValueError, "Invalid service client capsule");
        return NULL;
    }

    dap_chain_net_srv_client_close(l_client);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_client_is_connected_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_client_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_client_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_client_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a service client capsule");
        return NULL;
    }

    dap_chain_net_srv_client_t *l_client = (dap_chain_net_srv_client_t *)PyCapsule_GetPointer(
        l_client_obj, "dap_chain_net_srv_client_t");
    if (!l_client) {
        PyErr_SetString(PyExc_ValueError, "Invalid service client capsule");
        return NULL;
    }

    return PyBool_FromLong(dap_chain_net_srv_client_is_connected(l_client));
}

PyObject* dap_chain_net_srv_client_error_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_err = 0;

    if (!PyArg_ParseTuple(a_args, "i", &l_err)) {
        return NULL;
    }

    const char *l_str = dap_chain_net_srv_client_error_str((dap_chain_net_srv_client_error_t)l_err);
    return PyUnicode_FromString(l_str ? l_str : "Unknown error");
}

PyObject* dap_chain_net_srv_client_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_client_obj = NULL;
    unsigned long long l_net_id = 0;
    unsigned long long l_srv_uid = 0;
    PyObject *l_data_obj = Py_None;
    int l_timeout_ms = 0;

    if (!PyArg_ParseTuple(a_args, "OKKOi", &l_client_obj, &l_net_id, &l_srv_uid,
                          &l_data_obj, &l_timeout_ms)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_client_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a service client capsule");
        return NULL;
    }

    dap_chain_net_srv_client_t *l_client = (dap_chain_net_srv_client_t *)PyCapsule_GetPointer(
        l_client_obj, "dap_chain_net_srv_client_t");
    if (!l_client) {
        PyErr_SetString(PyExc_ValueError, "Invalid service client capsule");
        return NULL;
    }

    const void *l_data = NULL;
    size_t l_data_size = 0;
    if (l_data_obj && l_data_obj != Py_None) {
        if (!PyBytes_Check(l_data_obj)) {
            PyErr_SetString(PyExc_TypeError, "data must be bytes or None");
            return NULL;
        }
        Py_ssize_t l_size = 0;
        if (PyBytes_AsStringAndSize(l_data_obj, (char **)&l_data, &l_size) < 0) {
            return NULL;
        }
        l_data_size = (size_t)l_size;
    }

    dap_chain_net_srv_ch_pkt_test_t *l_response = NULL;
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_srv_uid_struct = {.uint64 = l_srv_uid};
    int l_rc = dap_chain_net_srv_client_check(
        l_client, l_net_id_struct, l_srv_uid_struct, l_data, l_data_size, &l_response, l_timeout_ms);

    PyObject *l_resp_obj = Py_None;
    if (l_response) {
        size_t l_resp_size = sizeof(dap_chain_net_srv_ch_pkt_test_t) + (size_t)l_response->data_size;
        l_resp_obj = PyBytes_FromStringAndSize((const char *)l_response, (Py_ssize_t)l_resp_size);
        if (!l_resp_obj) {
            DAP_DELETE(l_response);
            return NULL;
        }
    } else {
        Py_INCREF(Py_None);
        l_resp_obj = Py_None;
    }
    DAP_DELETE(l_response);

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    PyTuple_SetItem(l_tuple, 1, l_resp_obj);
    return l_tuple;
}

PyObject* dap_chain_net_srv_client_request_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_client_obj = NULL;
    unsigned long long l_net_id = 0;
    unsigned long long l_srv_uid = 0;
    PyObject *l_tx_cond_obj = Py_None;
    int l_timeout_ms = 0;

    if (!PyArg_ParseTuple(a_args, "OKKOi", &l_client_obj, &l_net_id, &l_srv_uid,
                          &l_tx_cond_obj, &l_timeout_ms)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_client_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a service client capsule");
        return NULL;
    }

    dap_chain_net_srv_client_t *l_client = (dap_chain_net_srv_client_t *)PyCapsule_GetPointer(
        l_client_obj, "dap_chain_net_srv_client_t");
    if (!l_client) {
        PyErr_SetString(PyExc_ValueError, "Invalid service client capsule");
        return NULL;
    }

    dap_hash_fast_t l_tx_cond = {};
    dap_hash_fast_t *l_tx_cond_ptr = NULL;
    if (l_tx_cond_obj && l_tx_cond_obj != Py_None) {
        if (cf_datum_parse_hash_fast(l_tx_cond_obj, &l_tx_cond) != 0) {
            return NULL;
        }
        l_tx_cond_ptr = &l_tx_cond;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_srv_uid_struct = {.uint64 = l_srv_uid};
    dap_chain_net_srv_ch_pkt_success_t *l_success = NULL;
    size_t l_success_size = 0;
    int l_rc = dap_chain_net_srv_client_request(
        l_client, l_net_id_struct, l_srv_uid_struct, l_tx_cond_ptr,
        &l_success, &l_success_size, l_timeout_ms);

    PyObject *l_resp_obj = Py_None;
    if (l_success && l_success_size) {
        l_resp_obj = PyBytes_FromStringAndSize((const char *)l_success, (Py_ssize_t)l_success_size);
        if (!l_resp_obj) {
            DAP_DELETE(l_success);
            return NULL;
        }
    } else {
        Py_INCREF(Py_None);
        l_resp_obj = Py_None;
    }
    DAP_DELETE(l_success);

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    PyTuple_SetItem(l_tuple, 1, l_resp_obj);
    return l_tuple;
}

PyObject* dap_chain_net_srv_client_write_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_client_obj = NULL;
    int l_type = 0;
    PyObject *l_data_obj = Py_None;
    int l_expected_response = 0;
    int l_timeout_ms = 0;

    if (!PyArg_ParseTuple(a_args, "OiOii", &l_client_obj, &l_type, &l_data_obj,
                          &l_expected_response, &l_timeout_ms)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_client_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a service client capsule");
        return NULL;
    }

    if (l_type < 0 || l_type > 255 || l_expected_response < 0 || l_expected_response > 255) {
        PyErr_SetString(PyExc_ValueError, "packet types must be between 0 and 255");
        return NULL;
    }

    dap_chain_net_srv_client_t *l_client = (dap_chain_net_srv_client_t *)PyCapsule_GetPointer(
        l_client_obj, "dap_chain_net_srv_client_t");
    if (!l_client) {
        PyErr_SetString(PyExc_ValueError, "Invalid service client capsule");
        return NULL;
    }

    const void *l_data = NULL;
    size_t l_data_size = 0;
    if (l_data_obj && l_data_obj != Py_None) {
        if (!PyBytes_Check(l_data_obj)) {
            PyErr_SetString(PyExc_TypeError, "data must be bytes or None");
            return NULL;
        }
        Py_ssize_t l_size = 0;
        if (PyBytes_AsStringAndSize(l_data_obj, (char **)&l_data, &l_size) < 0) {
            return NULL;
        }
        l_data_size = (size_t)l_size;
    }

    void *l_out_data = NULL;
    size_t l_out_size = 0;
    int l_rc = dap_chain_net_srv_client_write(
        l_client, (uint8_t)l_type, l_data, l_data_size,
        (uint8_t)l_expected_response, &l_out_data, &l_out_size, l_timeout_ms);

    PyObject *l_resp_obj = Py_None;
    if (l_out_data && l_out_size) {
        l_resp_obj = PyBytes_FromStringAndSize((const char *)l_out_data, (Py_ssize_t)l_out_size);
        if (!l_resp_obj) {
            DAP_DELETE(l_out_data);
            return NULL;
        }
    } else {
        Py_INCREF(Py_None);
        l_resp_obj = Py_None;
    }
    DAP_DELETE(l_out_data);

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    PyTuple_SetItem(l_tuple, 1, l_resp_obj);
    return l_tuple;
}

// =============================================================================
// SERVICE CHANNEL OPERATIONS
// =============================================================================

PyObject* dap_chain_net_srv_ch_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_net_srv_ch_init();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_ch_grace_control_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_srv_obj = NULL;
    PyObject *l_hash_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_srv_obj, &l_hash_obj)) {
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

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    int l_rc = dap_chain_net_srv_ch_grace_control(l_srv, &l_hash);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_srv_ch_create_statistic_report_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    char *l_report = dap_chain_net_srv_ch_create_statistic_report();
    if (!l_report) {
        Py_RETURN_NONE;
    }

    PyObject *l_str = PyUnicode_FromString(l_report);
    DAP_DELETE(l_report);
    return l_str;
}

// =============================================================================
// SERVICE CHANNEL PACKET OPERATIONS
// =============================================================================

PyObject* dap_chain_net_srv_ch_pkt_data_write_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ch_obj = NULL;
    unsigned long long l_srv_uid = 0;
    unsigned int l_usage_id = 0;
    PyObject *l_data_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "OKIO", &l_ch_obj, &l_srv_uid, &l_usage_id, &l_data_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ch_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a stream channel capsule");
        return NULL;
    }

    dap_stream_ch_t *l_ch = (dap_stream_ch_t *)PyCapsule_GetPointer(l_ch_obj, "dap_stream_ch_t");
    if (!l_ch) {
        PyErr_SetString(PyExc_ValueError, "Invalid stream channel capsule");
        return NULL;
    }

    const void *l_data = NULL;
    size_t l_data_size = 0;
    if (l_data_obj && l_data_obj != Py_None) {
        if (!PyBytes_Check(l_data_obj)) {
            PyErr_SetString(PyExc_TypeError, "data must be bytes or None");
            return NULL;
        }
        Py_ssize_t l_size = 0;
        if (PyBytes_AsStringAndSize(l_data_obj, (char **)&l_data, &l_size) < 0) {
            return NULL;
        }
        l_data_size = (size_t)l_size;
    }

    dap_chain_srv_uid_t l_srv_uid_struct = {.uint64 = l_srv_uid};
    size_t l_written = dap_chain_net_srv_ch_pkt_data_write(
        l_ch, l_srv_uid_struct, (uint32_t)l_usage_id, l_data, l_data_size);
    return PyLong_FromSize_t(l_written);
}

PyObject* dap_chain_net_srv_ch_pkt_data_write_f_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ch_obj = NULL;
    unsigned long long l_srv_uid = 0;
    unsigned int l_usage_id = 0;
    const char *l_fmt = NULL;
    PyObject *l_fmt_args = Py_None;

    if (!PyArg_ParseTuple(a_args, "OKIs|O", &l_ch_obj, &l_srv_uid, &l_usage_id, &l_fmt, &l_fmt_args)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ch_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a stream channel capsule");
        return NULL;
    }

    dap_stream_ch_t *l_ch = (dap_stream_ch_t *)PyCapsule_GetPointer(l_ch_obj, "dap_stream_ch_t");
    if (!l_ch) {
        PyErr_SetString(PyExc_ValueError, "Invalid stream channel capsule");
        return NULL;
    }

    PyObject *l_fmt_obj = PyUnicode_FromString(l_fmt);
    if (!l_fmt_obj) {
        return NULL;
    }

    PyObject *l_args_tuple = NULL;
    if (!l_fmt_args || l_fmt_args == Py_None) {
        l_args_tuple = PyTuple_New(0);
    } else if (PyList_Check(l_fmt_args)) {
        l_args_tuple = PyList_AsTuple(l_fmt_args);
    } else if (PyTuple_Check(l_fmt_args) || PyDict_Check(l_fmt_args)) {
        Py_INCREF(l_fmt_args);
        l_args_tuple = l_fmt_args;
    } else {
        Py_DECREF(l_fmt_obj);
        PyErr_SetString(PyExc_TypeError, "format args must be a tuple, list, dict, or None");
        return NULL;
    }

    if (!l_args_tuple) {
        Py_DECREF(l_fmt_obj);
        return NULL;
    }

    PyObject *l_formatted = PyUnicode_Format(l_fmt_obj, l_args_tuple);
    Py_DECREF(l_fmt_obj);
    Py_DECREF(l_args_tuple);
    if (!l_formatted) {
        return NULL;
    }

    const char *l_data = PyUnicode_AsUTF8(l_formatted);
    if (!l_data) {
        Py_DECREF(l_formatted);
        return NULL;
    }

    dap_chain_srv_uid_t l_srv_uid_struct = {.uint64 = l_srv_uid};
    size_t l_written = dap_chain_net_srv_ch_pkt_data_write(
        l_ch, l_srv_uid_struct, (uint32_t)l_usage_id, l_data, strlen(l_data));
    Py_DECREF(l_formatted);
    return PyLong_FromSize_t(l_written);
}

// =============================================================================
// SERVICE STREAM SESSION OPERATIONS
// =============================================================================

PyObject* dap_chain_net_srv_stream_session_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_session_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_session_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_session_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a stream session capsule");
        return NULL;
    }

    dap_stream_session_t *l_session = (dap_stream_session_t *)PyCapsule_GetPointer(
        l_session_obj, "dap_stream_session_t");
    if (!l_session) {
        PyErr_SetString(PyExc_ValueError, "Invalid stream session capsule");
        return NULL;
    }

    dap_chain_net_srv_stream_session_t *l_srv_session = dap_chain_net_srv_stream_session_create(l_session);
    if (!l_srv_session) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_srv_session, "dap_chain_net_srv_stream_session_t", NULL);
}

PyObject* dap_chain_net_srv_stream_session_delete_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_session_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_session_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_session_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a stream session capsule");
        return NULL;
    }

    const char *l_name = PyCapsule_GetName(l_session_obj);
    if (!l_name) {
        PyErr_SetString(PyExc_ValueError, "Invalid stream session capsule");
        return NULL;
    }

    if (strcmp(l_name, "dap_stream_session_t") == 0) {
        dap_stream_session_t *l_session = (dap_stream_session_t *)PyCapsule_GetPointer(
            l_session_obj, "dap_stream_session_t");
        if (!l_session) {
            PyErr_SetString(PyExc_ValueError, "Invalid stream session capsule");
            return NULL;
        }
        dap_chain_net_srv_stream_session_delete(l_session);
    } else if (strcmp(l_name, "dap_chain_net_srv_stream_session_t") == 0) {
        dap_chain_net_srv_stream_session_t *l_srv_session = (dap_chain_net_srv_stream_session_t *)PyCapsule_GetPointer(
            l_session_obj, "dap_chain_net_srv_stream_session_t");
        if (!l_srv_session || !l_srv_session->parent) {
            PyErr_SetString(PyExc_ValueError, "Invalid service stream session capsule");
            return NULL;
        }
        dap_chain_net_srv_stream_session_delete(l_srv_session->parent);
    } else {
        PyErr_SetString(PyExc_TypeError, "Unsupported capsule type for stream session delete");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_usage_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_session_obj = NULL;
    PyObject *l_net_obj = NULL;
    PyObject *l_srv_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OOO", &l_session_obj, &l_net_obj, &l_srv_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_session_obj) || !PyCapsule_CheckExact(l_net_obj) || !PyCapsule_CheckExact(l_srv_obj)) {
        PyErr_SetString(PyExc_TypeError, "Arguments must be service session, network, and service capsules");
        return NULL;
    }

    dap_chain_net_srv_stream_session_t *l_session = (dap_chain_net_srv_stream_session_t *)PyCapsule_GetPointer(
        l_session_obj, "dap_chain_net_srv_stream_session_t");
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    dap_chain_net_srv_t *l_srv = (dap_chain_net_srv_t *)PyCapsule_GetPointer(l_srv_obj, "dap_chain_net_srv_t");
    if (!l_session || !l_net || !l_srv) {
        PyErr_SetString(PyExc_ValueError, "Invalid service session, network, or service capsule");
        return NULL;
    }

    dap_chain_net_srv_usage_t *l_usage = dap_chain_net_srv_usage_add(l_session, l_net, l_srv);
    if (!l_usage) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_usage, "dap_chain_net_srv_usage_t", NULL);
}

PyObject* dap_chain_net_srv_usage_delete_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_session_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_session_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_session_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a service session capsule");
        return NULL;
    }

    dap_chain_net_srv_stream_session_t *l_session = (dap_chain_net_srv_stream_session_t *)PyCapsule_GetPointer(
        l_session_obj, "dap_chain_net_srv_stream_session_t");
    if (!l_session) {
        PyErr_SetString(PyExc_ValueError, "Invalid service session capsule");
        return NULL;
    }

    dap_chain_net_srv_usage_delete(l_session);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_usage_find_unsafe_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_session_obj = NULL;
    unsigned int l_usage_id = 0;

    if (!PyArg_ParseTuple(a_args, "OI", &l_session_obj, &l_usage_id)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_session_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a service session capsule");
        return NULL;
    }

    dap_chain_net_srv_stream_session_t *l_session = (dap_chain_net_srv_stream_session_t *)PyCapsule_GetPointer(
        l_session_obj, "dap_chain_net_srv_stream_session_t");
    if (!l_session) {
        PyErr_SetString(PyExc_ValueError, "Invalid service session capsule");
        return NULL;
    }

    dap_chain_net_srv_usage_t *l_usage = dap_chain_net_srv_usage_find_unsafe(l_session, l_usage_id);
    if (!l_usage) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_usage, "dap_chain_net_srv_usage_t", NULL);
}

// =============================================================================
// SERVICE COMMON CALLBACKS
// =============================================================================

PyObject* dap_chain_net_srv_get_chain_id_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    const char *l_chain_name = NULL;

    if (!PyArg_ParseTuple(a_args, "Ks", &l_net_id, &l_chain_name)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_id_t l_chain_id = dap_chain_net_srv_get_chain_id_by_name(l_net_id_struct, l_chain_name);
    return PyLong_FromUnsignedLongLong(l_chain_id.uint64);
}

PyObject* dap_chain_net_srv_get_chain_id_by_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    int l_chain_type = 0;

    if (!PyArg_ParseTuple(a_args, "Ki", &l_net_id, &l_chain_type)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_id_t l_chain_id = dap_chain_net_srv_get_chain_id_by_type(
        l_net_id_struct, (dap_chain_type_t)l_chain_type);
    return PyLong_FromUnsignedLongLong(l_chain_id.uint64);
}

PyObject* dap_chain_net_srv_get_net_id_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name = NULL;

    if (!PyArg_ParseTuple(a_args, "s", &l_net_name)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id = dap_chain_net_srv_get_net_id_by_name(l_net_name);
    return PyLong_FromUnsignedLongLong(l_net_id.uint64);
}

PyObject* dap_chain_net_srv_set_net_callbacks_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_net_srv_set_net_callbacks(
        cf_services_get_chain_id_by_name_cb,
        cf_services_get_chain_id_by_type_cb,
        cf_services_get_net_id_by_name_cb);
    Py_RETURN_NONE;
}

// =============================================================================
// SERVICE TX OPERATIONS
// =============================================================================

PyObject* dap_net_srv_tx_builders_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_net_srv_tx_builders_register();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_net_srv_tx_builders_unregister_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_net_srv_tx_builders_unregister();
    Py_RETURN_NONE;
}

PyObject* dap_net_srv_tx_create_cond_output_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_used_outs_obj = Py_None;
    PyObject *l_pkey_hash_obj = NULL;
    const char *l_token = NULL;
    PyObject *l_value_obj = NULL;
    PyObject *l_value_per_unit_obj = NULL;
    int l_unit = 0;
    unsigned long long l_srv_uid = 0;
    PyObject *l_fee_obj = NULL;
    PyObject *l_cond_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "OOsOOiKO|O",
                          &l_used_outs_obj, &l_pkey_hash_obj, &l_token,
                          &l_value_obj, &l_value_per_unit_obj, &l_unit,
                          &l_srv_uid, &l_fee_obj, &l_cond_obj)) {
        return NULL;
    }

    dap_list_t *l_used_outs = NULL;
    if (l_used_outs_obj && l_used_outs_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_used_outs_obj)) {
            PyErr_SetString(PyExc_TypeError, "First argument must be a dap_list_t capsule or None");
            return NULL;
        }
        l_used_outs = (dap_list_t *)PyCapsule_GetPointer(l_used_outs_obj, "dap_list_t");
        if (!l_used_outs) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_list_t capsule");
            return NULL;
        }
    }

    dap_hash_fast_t l_pkey_hash = {};
    if (cf_datum_parse_hash_fast(l_pkey_hash_obj, &l_pkey_hash) != 0) {
        return NULL;
    }

    uint256_t l_value = {};
    if (cf_datum_parse_uint256(l_value_obj, &l_value) != 0) {
        return NULL;
    }

    uint256_t l_value_per_unit = {};
    if (cf_datum_parse_uint256(l_value_per_unit_obj, &l_value_per_unit) != 0) {
        return NULL;
    }

    uint256_t l_fee = {};
    if (cf_datum_parse_uint256(l_fee_obj, &l_fee) != 0) {
        return NULL;
    }

    const void *l_cond = NULL;
    size_t l_cond_size = 0;
    if (l_cond_obj && l_cond_obj != Py_None) {
        if (!PyBytes_Check(l_cond_obj)) {
            PyErr_SetString(PyExc_TypeError, "cond must be bytes or None");
            return NULL;
        }
        Py_ssize_t l_size = 0;
        if (PyBytes_AsStringAndSize(l_cond_obj, (char **)&l_cond, &l_size) < 0) {
            return NULL;
        }
        l_cond_size = (size_t)l_size;
    }

    dap_chain_net_srv_price_unit_uid_t l_unit_uid = {.enm = l_unit};
    dap_chain_srv_uid_t l_srv_uid_struct = {.uint64 = l_srv_uid};
    dap_chain_datum_tx_t *l_tx = dap_net_srv_tx_create_cond_output(
        l_used_outs, &l_pkey_hash, l_token, l_value, l_value_per_unit,
        l_unit_uid, l_srv_uid_struct, l_fee, l_cond, l_cond_size);

    if (!l_tx) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

PyObject* dap_net_srv_tx_create_cond_input_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_tx_hash_obj = NULL;
    unsigned int l_out_idx = 0;
    PyObject *l_receipt_obj = NULL;
    PyObject *l_addr_obj = NULL;
    PyObject *l_value_obj = NULL;
    const char *l_token = NULL;

    if (!PyArg_ParseTuple(a_args, "OIOOOs",
                          &l_tx_hash_obj, &l_out_idx, &l_receipt_obj,
                          &l_addr_obj, &l_value_obj, &l_token)) {
        return NULL;
    }

    dap_hash_fast_t l_tx_hash = {};
    if (cf_datum_parse_hash_fast(l_tx_hash_obj, &l_tx_hash) != 0) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_receipt_obj)) {
        PyErr_SetString(PyExc_TypeError, "receipt must be a dap_chain_datum_tx_receipt_t capsule");
        return NULL;
    }

    dap_chain_datum_tx_receipt_t *l_receipt = (dap_chain_datum_tx_receipt_t *)PyCapsule_GetPointer(
        l_receipt_obj, "dap_chain_datum_tx_receipt_t");
    if (!l_receipt) {
        PyErr_SetString(PyExc_ValueError, "Invalid receipt capsule");
        return NULL;
    }

    dap_chain_addr_t l_addr = {};
    if (cf_datum_parse_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    uint256_t l_value = {};
    if (cf_datum_parse_uint256(l_value_obj, &l_value) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *l_tx = dap_net_srv_tx_create_cond_input(
        &l_tx_hash, (uint32_t)l_out_idx, l_receipt, &l_addr, l_value, l_token);
    if (!l_tx) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

// =============================================================================
// SERVICE GEOIP OPERATIONS
// =============================================================================

PyObject* chain_net_geoip_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_config_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_config_obj)) {
        return NULL;
    }

    dap_config_t *l_config = NULL;
    if (l_config_obj && l_config_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_config_obj)) {
            PyErr_SetString(PyExc_TypeError, "Argument must be a config capsule or None");
            return NULL;
        }
        l_config = (dap_config_t *)PyCapsule_GetPointer(l_config_obj, "dap_config_t");
        if (!l_config) {
            PyErr_SetString(PyExc_ValueError, "Invalid config capsule");
            return NULL;
        }
    }

    int l_rc = chain_net_geoip_init(l_config);
    return PyLong_FromLong(l_rc);
}

PyObject* chain_net_geoip_get_ip_info_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_ip_str = NULL;

    if (!PyArg_ParseTuple(a_args, "s", &l_ip_str)) {
        return NULL;
    }

    geoip_info_t *l_info = chain_net_geoip_get_ip_info(l_ip_str);
    if (!l_info) {
        Py_RETURN_NONE;
    }

    PyObject *l_dict = PyDict_New();
    if (!l_dict) {
        DAP_DELETE(l_info);
        return NULL;
    }

    PyDict_SetItemString(l_dict, "ip_str", PyUnicode_FromString(l_info->ip_str));
    PyDict_SetItemString(l_dict, "continent", PyUnicode_FromString(l_info->continent));
    PyDict_SetItemString(l_dict, "country_name", PyUnicode_FromString(l_info->country_name));
    PyDict_SetItemString(l_dict, "country_code", PyUnicode_FromString(l_info->country_code));
    PyDict_SetItemString(l_dict, "city_name", PyUnicode_FromString(l_info->city_name));
    PyDict_SetItemString(l_dict, "latitude", PyFloat_FromDouble(l_info->latitude));
    PyDict_SetItemString(l_dict, "longitude", PyFloat_FromDouble(l_info->longitude));

    DAP_DELETE(l_info);
    return l_dict;
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
        {"net_srv_issue_receipt", (PyCFunction)dap_chain_net_srv_issue_receipt_py, METH_VARARGS,
         "Issue a service receipt"},
        {"net_srv_get_price_from_order", (PyCFunction)dap_chain_net_srv_get_price_from_order_py, METH_VARARGS,
         "Get price from order"},
        {"net_srv_price_unit_uid_from_str", (PyCFunction)dap_chain_net_srv_price_unit_uid_from_str_py, METH_VARARGS,
         "Parse price unit UID from string"},
        {"net_srv_price_unit_uid_to_str", (PyCFunction)dap_chain_net_srv_price_unit_uid_to_str_py, METH_VARARGS,
         "Format price unit UID to string"},
        {"net_srv_uid_compare", (PyCFunction)dap_chain_net_srv_uid_compare_py, METH_VARARGS,
         "Compare service UID values"},
        {"net_srv_uid_compare_scalar", (PyCFunction)dap_chain_net_srv_uid_compare_scalar_py, METH_VARARGS,
         "Compare service UID with scalar value"},
        
        // Order service operations
        {"net_srv_order_init", (PyCFunction)dap_chain_net_srv_order_init_py, METH_VARARGS,
         "Initialize order service subsystem"},
        {"net_srv_order_deinit", (PyCFunction)dap_chain_net_srv_order_deinit_py, METH_VARARGS,
         "Deinitialize order service subsystem"},
        {"net_srv_order_check", (PyCFunction)dap_chain_net_srv_order_check_py, METH_VARARGS,
         "Check order by hash and bytes"},
        {"net_srv_order_compose", (PyCFunction)dap_chain_net_srv_order_compose_py, METH_VARARGS,
         "Compose order in memory"},
        {"net_srv_order_create", (PyCFunction)dap_chain_net_srv_order_create_py, METH_VARARGS,
         "Create order in storage"},
        {"net_srv_order_save", (PyCFunction)dap_chain_net_srv_order_save_py, METH_VARARGS,
         "Save order to storage"},
        {"net_srv_order_find_by_hash", (PyCFunction)dap_chain_net_srv_order_find_by_hash_py, METH_VARARGS,
         "Find order by hash"},
        {"net_srv_order_find_by_hash_str", (PyCFunction)dap_chain_net_srv_order_find_by_hash_str_py, METH_VARARGS,
         "Find order by hash string"},
        {"net_srv_order_delete_by_hash_str", (PyCFunction)dap_chain_net_srv_order_delete_by_hash_str_py, METH_VARARGS,
         "Delete order by hash string"},
        {"net_srv_order_delete_by_hash", (PyCFunction)dap_chain_net_srv_order_delete_by_hash_py, METH_VARARGS,
         "Delete order by hash"},
        {"net_srv_order_get_size", (PyCFunction)dap_chain_net_srv_order_get_size_py, METH_VARARGS,
         "Get order size"},
        {"net_srv_order_find_all_by", (PyCFunction)dap_chain_net_srv_order_find_all_by_py, METH_VARARGS,
         "Find all orders by criteria"},
        {"net_srv_order_dump_to_string", (PyCFunction)dap_chain_net_srv_order_dump_to_string_py, METH_VARARGS,
         "Dump order to string"},
        {"net_srv_order_dump_to_json", (PyCFunction)dap_chain_net_srv_order_dump_to_json_py, METH_VARARGS,
         "Dump order to JSON"},
        {"net_srv_order_get_sign", (PyCFunction)dap_chain_net_srv_order_get_sign_py, METH_VARARGS,
         "Get order signature"},
        {"net_srv_order_get_gdb_group", (PyCFunction)dap_chain_net_srv_order_get_gdb_group_py, METH_VARARGS,
         "Get order GDB group"},
        {"net_srv_order_get_common_group", (PyCFunction)dap_chain_net_srv_order_get_common_group_py, METH_VARARGS,
         "Get order common group"},
        {"net_srv_order_set_continent_region", (PyCFunction)dap_chain_net_srv_order_set_continent_region_py, METH_VARARGS,
         "Set order continent and region"},
        {"net_srv_order_get_continent_region", (PyCFunction)dap_chain_net_srv_order_get_continent_region_py, METH_VARARGS,
         "Get order continent and region"},
        {"net_srv_order_get_country_code", (PyCFunction)dap_chain_net_srv_order_get_country_code_py, METH_VARARGS,
         "Get order country code"},
        {"net_srv_order_continents_count", (PyCFunction)dap_chain_net_srv_order_continents_count_py, METH_VARARGS,
         "Get continents count"},
        {"net_srv_order_continent_to_str", (PyCFunction)dap_chain_net_srv_order_continent_to_str_py, METH_VARARGS,
         "Convert continent num to string"},
        {"net_srv_order_continent_to_num", (PyCFunction)dap_chain_net_srv_order_continent_to_num_py, METH_VARARGS,
         "Convert continent string to num"},
        {"net_srv_order_pin_init", (PyCFunction)dap_chain_srv_order_pin_init_py, METH_VARARGS,
         "Initialize order pin subsystem"},

        // Service client operations
        {"net_srv_client_connect", (PyCFunction)dap_chain_net_srv_client_connect_py, METH_VARARGS,
         "Connect to service provider"},
        {"net_srv_client_close", (PyCFunction)dap_chain_net_srv_client_close_py, METH_VARARGS,
         "Close service client"},
        {"net_srv_client_is_connected", (PyCFunction)dap_chain_net_srv_client_is_connected_py, METH_VARARGS,
         "Check if service client is connected"},
        {"net_srv_client_error_str", (PyCFunction)dap_chain_net_srv_client_error_str_py, METH_VARARGS,
         "Get service client error string"},
        {"net_srv_client_check", (PyCFunction)dap_chain_net_srv_client_check_py, METH_VARARGS,
         "Check service availability"},
        {"net_srv_client_request", (PyCFunction)dap_chain_net_srv_client_request_py, METH_VARARGS,
         "Request service (conditional)"},
        {"net_srv_client_write", (PyCFunction)dap_chain_net_srv_client_write_py, METH_VARARGS,
         "Write raw packet to service channel"},

        // Service channel operations
        {"net_srv_ch_init", (PyCFunction)dap_chain_net_srv_ch_init_py, METH_VARARGS,
         "Initialize service channel"},
        {"net_srv_ch_grace_control", (PyCFunction)dap_chain_net_srv_ch_grace_control_py, METH_VARARGS,
         "Handle grace control"},
        {"net_srv_ch_create_statistic_report", (PyCFunction)dap_chain_net_srv_ch_create_statistic_report_py, METH_VARARGS,
         "Create service statistic report"},

        // Service channel packet operations
        {"net_srv_ch_pkt_data_write", (PyCFunction)dap_chain_net_srv_ch_pkt_data_write_py, METH_VARARGS,
         "Write service channel data packet"},
        {"net_srv_ch_pkt_data_write_f", (PyCFunction)dap_chain_net_srv_ch_pkt_data_write_f_py, METH_VARARGS,
         "Write formatted service channel data packet"},

        // Service stream session operations
        {"net_srv_stream_session_create", (PyCFunction)dap_chain_net_srv_stream_session_create_py, METH_VARARGS,
         "Create service stream session"},
        {"net_srv_stream_session_delete", (PyCFunction)dap_chain_net_srv_stream_session_delete_py, METH_VARARGS,
         "Delete service stream session"},
        {"net_srv_usage_add", (PyCFunction)dap_chain_net_srv_usage_add_py, METH_VARARGS,
         "Add service usage"},
        {"net_srv_usage_delete", (PyCFunction)dap_chain_net_srv_usage_delete_py, METH_VARARGS,
         "Delete service usage"},
        {"net_srv_usage_find_unsafe", (PyCFunction)dap_chain_net_srv_usage_find_unsafe_py, METH_VARARGS,
         "Find service usage (unsafe)"},

        // Service common callbacks
        {"net_srv_get_chain_id_by_name", (PyCFunction)dap_chain_net_srv_get_chain_id_by_name_py, METH_VARARGS,
         "Get chain ID by name"},
        {"net_srv_get_chain_id_by_type", (PyCFunction)dap_chain_net_srv_get_chain_id_by_type_py, METH_VARARGS,
         "Get chain ID by type"},
        {"net_srv_get_net_id_by_name", (PyCFunction)dap_chain_net_srv_get_net_id_by_name_py, METH_VARARGS,
         "Get net ID by name"},
        {"net_srv_set_net_callbacks", (PyCFunction)dap_chain_net_srv_set_net_callbacks_py, METH_VARARGS,
         "Set net callbacks for services"},

        // Service TX operations
        {"net_srv_tx_builders_register", (PyCFunction)dap_net_srv_tx_builders_register_py, METH_VARARGS,
         "Register net/srv TX builders"},
        {"net_srv_tx_builders_unregister", (PyCFunction)dap_net_srv_tx_builders_unregister_py, METH_VARARGS,
         "Unregister net/srv TX builders"},
        {"net_srv_tx_create_cond_input", (PyCFunction)dap_net_srv_tx_create_cond_input_py, METH_VARARGS,
         "Create conditional input transaction"},
        {"net_srv_tx_create_cond_output", (PyCFunction)dap_net_srv_tx_create_cond_output_py, METH_VARARGS,
         "Create conditional output transaction"},

        // GeoIP operations
        {"chain_net_geoip_init", (PyCFunction)chain_net_geoip_init_py, METH_VARARGS,
         "Initialize GeoIP subsystem"},
        {"chain_net_geoip_get_ip_info", (PyCFunction)chain_net_geoip_get_ip_info_py, METH_VARARGS,
         "Get GeoIP information by IP"},
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return services_methods;
}
