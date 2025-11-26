#include "cellframe.h"
#include "dap_chain_cs.h"
#include "dap_chain.h"
#include "dap_chain_datum.h"
#include "dap_chain_mempool.h"
#include "dap_enc_key.h"
#include "dap_math_ops.h"

#define LOG_TAG "python_cellframe_mempool"

// =============================================================================
// MEMPOOL WRAPPERS
// =============================================================================

/**
 * @brief Add datum to mempool
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, datum capsule, hash_out_type string)
 * @return String hash or None
 */
PyObject* dap_chain_cs_mempool_datum_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_datum_obj;
    const char *l_hash_out_type;
    
    if (!PyArg_ParseTuple(a_args, "OOs", &l_chain_obj, &l_datum_obj, &l_hash_out_type)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_datum_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a datum capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    dap_chain_datum_t *l_datum = (dap_chain_datum_t *)PyCapsule_GetPointer(l_datum_obj, "dap_chain_datum_t");
    
    if (!l_chain || !l_datum) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain or datum capsule");
        return NULL;
    }
    
    char *l_hash = dap_chain_cs_mempool_datum_add(l_chain, l_datum, l_hash_out_type);
    if (!l_hash) {
        log_it(L_DEBUG, "Failed to add datum to mempool");
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = PyUnicode_FromString(l_hash);
    log_it(L_DEBUG, "Added datum to mempool, hash: %s", l_hash);
    return l_result;
}

/**
 * @brief Create transaction in mempool
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain, key_from, addr_from, addr_to_list, ticker, value, fee, hash_out_type)
 * @return String hash or None
 */
PyObject* dap_chain_mempool_tx_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_key_from_obj, *l_addr_from_obj, *l_addr_to_list, *l_value_obj;
    const char *l_ticker;
    unsigned long long l_fee_uint;
    const char *l_hash_out_type = "hex";
    
    if (!PyArg_ParseTuple(a_args, "OOOOssK|s", &l_chain_obj, &l_key_from_obj, &l_addr_from_obj, 
                          &l_addr_to_list, &l_ticker, &l_value_obj, &l_fee_uint, &l_hash_out_type)) {
        return NULL;
    }
    
    // Validate and extract capsules
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "chain must be a capsule");
        return NULL;
    }
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    
    if (!PyCapsule_CheckExact(l_key_from_obj)) {
        PyErr_SetString(PyExc_TypeError, "key_from must be a capsule");
        return NULL;
    }
    dap_enc_key_t *l_key_from = (dap_enc_key_t *)PyCapsule_GetPointer(l_key_from_obj, "dap_enc_key_t");
    
    if (!PyCapsule_CheckExact(l_addr_from_obj)) {
        PyErr_SetString(PyExc_TypeError, "addr_from must be a capsule");
        return NULL;
    }
    dap_chain_addr_t *l_addr_from = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_from_obj, "dap_chain_addr_t");
    
    if (!PyList_Check(l_addr_to_list)) {
        PyErr_SetString(PyExc_TypeError, "addr_to_list must be a list");
        return NULL;
    }
    
    // Process addr_to list
    size_t l_count = PyList_Size(l_addr_to_list);
    if (l_count == 0) {
        PyErr_SetString(PyExc_ValueError, "addr_to_list cannot be empty");
        return NULL;
    }

    dap_chain_addr_t **l_addr_to_arr = DAP_NEW_Z_COUNT(dap_chain_addr_t*, l_count);
    if (!l_addr_to_arr) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for addresses");
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
    
    // Process values
    uint256_t *l_value_arr = DAP_NEW_Z_COUNT(uint256_t, l_count);
    if (!l_value_arr) {
        DAP_DELETE(l_addr_to_arr);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for values");
        return NULL;
    }

    if (PyList_Check(l_value_obj)) {
        if (PyList_Size(l_value_obj) != (Py_ssize_t)l_count) {
            DAP_DELETE(l_addr_to_arr);
            DAP_DELETE(l_value_arr);
            PyErr_SetString(PyExc_ValueError, "value list size must match address list size");
            return NULL;
        }
        for (size_t i = 0; i < l_count; i++) {
            PyObject *l_item = PyList_GetItem(l_value_obj, i);
            if (!PyLong_Check(l_item)) {
                DAP_DELETE(l_addr_to_arr);
                DAP_DELETE(l_value_arr);
                PyErr_SetString(PyExc_TypeError, "value list items must be integers");
                return NULL;
            }
            l_value_arr[i] = GET_256_FROM_64(PyLong_AsUnsignedLongLong(l_item));
        }
    } else if (PyLong_Check(l_value_obj)) {
        uint256_t l_val = GET_256_FROM_64(PyLong_AsUnsignedLongLong(l_value_obj));
        for (size_t i = 0; i < l_count; i++) {
            l_value_arr[i] = l_val;
        }
    } else {
        DAP_DELETE(l_addr_to_arr);
        DAP_DELETE(l_value_arr);
        PyErr_SetString(PyExc_TypeError, "value must be an integer or a list of integers");
        return NULL;
    }

    uint256_t l_fee = GET_256_FROM_64(l_fee_uint);
    
    // Allocate time_unlock array (initialized to zero by NEW_Z)
    dap_time_t *l_time_unlock_arr = DAP_NEW_Z_COUNT(dap_time_t, l_count);
    if (!l_time_unlock_arr) {
        DAP_DELETE(l_addr_to_arr);
        DAP_DELETE(l_value_arr);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for timestamps");
        return NULL;
    }
    
    char *l_hash = dap_chain_mempool_tx_create(
        l_chain, l_key_from, l_addr_from, (const dap_chain_addr_t **)l_addr_to_arr,
        l_ticker, l_value_arr, l_fee, l_hash_out_type,
        l_count, l_time_unlock_arr
    );
    
    DAP_DELETE(l_addr_to_arr);
    DAP_DELETE(l_value_arr);
    DAP_DELETE(l_time_unlock_arr);
    
    if (!l_hash) {
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = PyUnicode_FromString(l_hash);
    return l_result;
}

/**
 * @brief Get datum from mempool
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, hash string)
 * @return Datum capsule or None
 */
PyObject* dap_chain_mempool_datum_get_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    const char *l_hash_str;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_chain_obj, &l_hash_str)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "chain must be a capsule");
        return NULL;
    }
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    
    dap_chain_datum_t *l_datum = dap_chain_mempool_datum_get(l_chain, l_hash_str);
    if (!l_datum) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_datum, "dap_chain_datum_t", NULL);
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

/**
 * @brief Get method definitions for mempool module
 */
PyMethodDef* cellframe_mempool_get_methods(void) {
    static PyMethodDef mempool_methods[] = {
        {"chain_cs_mempool_datum_add", (PyCFunction)dap_chain_cs_mempool_datum_add_py, METH_VARARGS,
         "Add datum to mempool (consensus)"},
        {"chain_mempool_tx_create", (PyCFunction)dap_chain_mempool_tx_create_py, METH_VARARGS,
         "Create transaction in mempool"},
        {"chain_mempool_datum_get", (PyCFunction)dap_chain_mempool_datum_get_py, METH_VARARGS,
         "Get datum from mempool"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return mempool_methods;
}

