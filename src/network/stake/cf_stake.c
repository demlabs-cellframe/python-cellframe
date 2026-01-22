#include "cellframe.h"
#include "dap_chain_cs.h"
#include "dap_chain.h"

#define LOG_TAG "python_cellframe_stake"

// =============================================================================
// STAKE SERVICE WRAPPERS
// =============================================================================

/**
 * @brief Check stake pkey hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, pkey_hash bytes)
 * @return Tuple (result_code, sovereign_tax bytes, sovereign_addr capsule) or None
 */
PyObject* dap_chain_cs_stake_check_pkey_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    const char *l_pkey_hash_bytes;
    Py_ssize_t l_pkey_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_chain_obj, &l_pkey_hash_bytes, &l_pkey_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    if ((size_t)l_pkey_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Pkey hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_pkey_hash = (dap_hash_fast_t *)l_pkey_hash_bytes;
    uint256_t l_sovereign_tax = uint256_0;
    dap_chain_addr_t l_sovereign_addr = {0};
    
    int l_result = dap_chain_cs_stake_check_pkey_hash(l_chain, l_pkey_hash, &l_sovereign_tax, &l_sovereign_addr);
    
    PyObject *l_tuple = PyTuple_New(3);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_result));
    PyTuple_SetItem(l_tuple, 1, PyBytes_FromStringAndSize((const char *)&l_sovereign_tax, sizeof(uint256_t)));
    
    // Create address capsule
    dap_chain_addr_t *l_addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    if (!l_addr_copy) {
        Py_DECREF(l_tuple);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }
    *l_addr_copy = l_sovereign_addr;
    PyTuple_SetItem(l_tuple, 2, PyCapsule_New(l_addr_copy, "dap_chain_addr_t", NULL));
    
    log_it(L_DEBUG, "Stake check pkey hash result: %d", l_result);
    return l_tuple;
}

/**
 * @brief Import stake hardfork data
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, decree_hash bytes)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_stake_hardfork_data_import_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    const char *l_decree_hash_bytes;
    Py_ssize_t l_decree_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_chain_obj, &l_decree_hash_bytes, &l_decree_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    if ((size_t)l_decree_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Decree hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_decree_hash = (dap_hash_fast_t *)l_decree_hash_bytes;
    
    int l_result = dap_chain_cs_stake_hardfork_data_import(l_chain, l_decree_hash);
    
    log_it(L_DEBUG, "Stake hardfork data import result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Switch stake table
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, to_sandbox bool)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_stake_switch_table_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    int l_to_sandbox;
    
    if (!PyArg_ParseTuple(a_args, "Op", &l_chain_obj, &l_to_sandbox)) {
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
    
    int l_result = dap_chain_cs_stake_switch_table(l_chain, (bool)l_to_sandbox);
    
    log_it(L_DEBUG, "Stake switch table result: %d", l_result);
    return PyLong_FromLong(l_result);
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

/**
 * @brief Get method definitions for stake module
 */
PyMethodDef* cellframe_stake_get_methods(void) {
    static PyMethodDef stake_methods[] = {
        {"chain_cs_stake_check_pkey_hash", (PyCFunction)dap_chain_cs_stake_check_pkey_hash_py, METH_VARARGS,
         "Check stake pkey hash"},
        {"chain_cs_stake_hardfork_data_import", (PyCFunction)dap_chain_cs_stake_hardfork_data_import_py, METH_VARARGS,
         "Import stake hardfork data"},
        {"chain_cs_stake_switch_table", (PyCFunction)dap_chain_cs_stake_switch_table_py, METH_VARARGS,
         "Switch stake table"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return stake_methods;
}

