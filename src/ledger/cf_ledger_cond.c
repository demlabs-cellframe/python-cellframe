#include "include/cf_ledger_internal.h"
#include "../common/cf_callbacks_registry.h"
#include "cf_verificator_registry.h"

extern dap_chain_tx_used_out_item_t *dap_ledger_get_tx_cond_out(dap_ledger_t *a_ledger,
                                                                const dap_chain_addr_t *a_addr_from,
                                                                dap_chain_tx_out_cond_subtype_t a_subtype) __attribute__((weak));

/*
 * Cellframe ledger conditional outputs bindings
 * Conditional outputs: get, find, list, verificator management
 */

// Verificator callbacks context - holds 6 Python callbacks
// NOTE: Defined in cf_verificator_registry.h, included here for reference
// typedef struct {
//     PyObject *in_verify_callback;
//     PyObject *out_verify_callback;
//     PyObject *in_add_callback;
//     PyObject *out_add_callback;
//     PyObject *in_delete_callback;
//     PyObject *out_delete_callback;
//     PyObject *user_data;
// } python_verificator_ctx_t;

// Voting verificator callbacks context - holds 4 Python callbacks
// typedef struct {
//     PyObject *voting_callback;
//     PyObject *vote_callback;
//     PyObject *delete_callback;
//     PyObject *expire_callback;
//     PyObject *user_data;
// } python_voting_ctx_t;


// =============================================================================
// CONDITIONAL OUTPUTS OPERATIONS
// =============================================================================

// REMOVED: dap_ledger_get_tx_cond_out_py
// Function dap_ledger_get_tx_cond_out() was removed from SDK
// Use dap_ledger_get_list_tx_cond_outs_py() instead

/**
 * @brief Find unspent conditional output by address
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, token, subtype, addr, tx_first_hash)
 * @return Dict with out_cond (capsule) and out_idx (int) or None
 */
PyObject* dap_ledger_out_cond_unspent_find_by_addr_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_addr_obj, *l_hash_obj = NULL;
    const char *l_token;
    int l_subtype;
    
    if (!PyArg_ParseTuple(a_args, "OsiO|O", &l_ledger_obj, &l_token, &l_subtype, &l_addr_obj, &l_hash_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "Fourth argument must be an address capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_obj, "dap_chain_addr_t");
    if (!l_addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid address capsule");
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_hash = NULL;
    if (l_hash_obj && l_hash_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_hash_obj)) {
            PyErr_SetString(PyExc_TypeError, "tx_first_hash must be a hash capsule");
            return NULL;
        }
        l_hash = (dap_chain_hash_fast_t *)PyCapsule_GetPointer(l_hash_obj, "dap_hash_fast_t");
    }
    
    int l_out_idx = 0;
    dap_chain_tx_out_cond_t *l_out_cond = dap_ledger_out_cond_unspent_find_by_addr(
        l_ledger, l_token, (dap_chain_tx_out_cond_subtype_t)l_subtype, l_addr, l_hash, &l_out_idx
    );
    
    if (!l_out_cond) {
        log_it(L_DEBUG, "No unspent conditional output found");
        Py_RETURN_NONE;
    }
    
    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "out_cond", PyCapsule_New(l_out_cond, "dap_chain_tx_out_cond_t", NULL));
    PyDict_SetItemString(l_dict, "out_idx", PyLong_FromLong(l_out_idx));
    
    log_it(L_DEBUG, "Found unspent conditional output at index %d", l_out_idx);
    return l_dict;
}

/**
 * @brief Get first conditional output by subtype
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, addr capsule, subtype)
 * @return PyCapsule wrapping dap_chain_tx_used_out_item_t* or None
 */
PyObject* dap_ledger_get_tx_cond_out_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_addr_obj = NULL;
    int l_subtype = 0;

    if (!PyArg_ParseTuple(a_args, "OOi", &l_ledger_obj, &l_addr_obj, &l_subtype)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj) || !PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger and addr must be capsules");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_obj, "dap_chain_addr_t");
    if (!l_ledger || !l_addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or addr capsule");
        return NULL;
    }

    if (!dap_ledger_get_tx_cond_out) {
        PyErr_SetString(PyExc_RuntimeError, "dap_ledger_get_tx_cond_out is not available in this SDK build");
        return NULL;
    }

    dap_chain_tx_used_out_item_t *l_item = dap_ledger_get_tx_cond_out(
        l_ledger, l_addr, (dap_chain_tx_out_cond_subtype_t)l_subtype
    );
    if (!l_item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_item, "dap_chain_tx_used_out_item_t", NULL);
}

/**
 * @brief Get linked output condition for input condition
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, tx_in_cond capsule)
 * @return PyCapsule wrapping dap_chain_tx_out_cond_t* or None
 */
PyObject* dap_chain_ledger_get_tx_out_cond_linked_to_tx_in_cond_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_in_cond_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_in_cond_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj) || !PyCapsule_CheckExact(l_in_cond_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger and tx_in_cond must be capsules");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_chain_tx_in_cond_t *l_in_cond = (dap_chain_tx_in_cond_t *)PyCapsule_GetPointer(
        l_in_cond_obj, "dap_chain_tx_in_cond_t"
    );
    if (!l_ledger || !l_in_cond) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or tx_in_cond capsule");
        return NULL;
    }

    dap_chain_tx_out_cond_t *l_out_cond = dap_chain_ledger_get_tx_out_cond_linked_to_tx_in_cond(l_ledger, l_in_cond);
    if (!l_out_cond) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_out_cond, "dap_chain_tx_out_cond_t", NULL);
}

/**
 * @brief Get list of conditional outputs
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, subtype, token_ticker, addr_from)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_ledger_get_list_tx_cond_outs_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_addr_obj;
    int l_subtype;
    const char *l_token_ticker;
    
    if (!PyArg_ParseTuple(a_args, "OisO", &l_ledger_obj, &l_subtype, &l_token_ticker, &l_addr_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "Fourth argument must be an address capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_obj, "dap_chain_addr_t");
    if (!l_addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid address capsule");
        return NULL;
    }
    
    dap_list_t *l_list = dap_ledger_get_list_tx_cond_outs(l_ledger, (dap_chain_tx_out_cond_subtype_t)l_subtype,
                                                           l_token_ticker, l_addr);
    if (!l_list) {
        log_it(L_DEBUG, "No conditional outputs found for token %s", l_token_ticker);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved list of conditional outputs for token %s", l_token_ticker);
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

/**
 * @brief Get UTXO list for value
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, token_ticker, addr, value_need bytes)
 * @return Tuple (list capsule or None, value_found bytes)
 */
PyObject* dap_ledger_get_utxo_for_value_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_addr_obj = NULL;
    const char *l_token = NULL;
    const char *l_value_bytes = NULL;
    Py_ssize_t l_value_size = 0;

    if (!PyArg_ParseTuple(a_args, "OsOs#", &l_ledger_obj, &l_token, &l_addr_obj, &l_value_bytes, &l_value_size)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj) || !PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger and addr must be capsules");
        return NULL;
    }

    if ((size_t)l_value_size != sizeof(uint256_t)) {
        PyErr_Format(PyExc_ValueError, "value_need must be exactly %zu bytes", sizeof(uint256_t));
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_obj, "dap_chain_addr_t");
    if (!l_ledger || !l_addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or addr capsule");
        return NULL;
    }

    uint256_t l_value_need = *(uint256_t *)l_value_bytes;
    uint256_t l_value_found = uint256_0;

    dap_list_t *l_list = dap_ledger_get_utxo_for_value(l_ledger, l_token, l_addr, l_value_need, &l_value_found);

    PyObject *l_list_obj = l_list ? PyCapsule_New(l_list, "dap_list_t", NULL) : Py_None;
    if (!l_list) {
        Py_INCREF(Py_None);
    }

    PyObject *l_value_obj = PyBytes_FromStringAndSize((const char *)&l_value_found, sizeof(uint256_t));
    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, l_list_obj);
    PyTuple_SetItem(l_tuple, 1, l_value_obj);
    return l_tuple;
}

/**
 * @brief Find all conditional outputs from TX cache by service UID
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, srv_uid)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_ledger_tx_cache_find_out_cond_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    unsigned long long l_srv_uid;
    
    if (!PyArg_ParseTuple(a_args, "OK", &l_ledger_obj, &l_srv_uid)) {
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
    
    dap_chain_srv_uid_t l_uid = {.uint64 = l_srv_uid};
    
    dap_list_t *l_list = dap_ledger_tx_cache_find_out_cond_all(l_ledger, l_uid);
    if (!l_list) {
        log_it(L_DEBUG, "No conditional outputs found in cache for service UID %llu", l_srv_uid);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved conditional outputs from cache for service UID %llu", l_srv_uid);
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

/**
 * @brief Check condition owner
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger, tx_hash bytes, cond_subtype, out_idx, owner_sign)
 * @return True if owner is valid, False otherwise
 */
PyObject* dap_ledger_check_condition_owner_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj, *l_sign_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    int l_cond_subtype, l_out_idx;
    
    if (!PyArg_ParseTuple(a_args, "Os#iiO", &l_ledger_obj, &l_hash_bytes, &l_hash_size, 
                          &l_cond_subtype, &l_out_idx, &l_sign_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_sign_obj)) {
        PyErr_SetString(PyExc_TypeError, "Fifth argument must be a signature capsule");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_hash_bytes;
    dap_sign_t *l_sign = (dap_sign_t *)PyCapsule_GetPointer(l_sign_obj, "dap_sign_t");
    if (!l_sign) {
        PyErr_SetString(PyExc_ValueError, "Invalid signature capsule");
        return NULL;
    }
    
    bool l_result = dap_ledger_check_condition_owner(l_ledger, l_hash, 
                                                      (dap_chain_tx_out_cond_subtype_t)l_cond_subtype,
                                                      l_out_idx, l_sign);
    
    log_it(L_DEBUG, "Condition owner check result: %s", l_result ? "valid" : "invalid");
    return PyBool_FromLong(l_result);
}

// =========================================
// VERIFICATOR CALLBACK WRAPPERS (6 callbacks)
// =========================================

/**
 * @brief C wrapper for cond_in_verify callback - calls Python callback
 * Signature: int (*dap_ledger_cond_in_verify_callback_t)(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_in,  
 *                                                          dap_hash_fast_t *a_tx_in_hash,  dap_chain_tx_out_cond_t *a_prev_cond, 
 *                                                          bool a_owner, bool a_from_mempool)
 */
static int s_verificator_in_verify_wrapper(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_in,  
                                            dap_hash_fast_t *a_tx_in_hash, dap_chain_tx_out_cond_t *a_prev_cond, 
                                            bool a_owner, bool a_from_mempool) {
    if (!a_prev_cond) {
        return 0;
    }
    
    dap_chain_tx_out_cond_subtype_t l_subtype = a_prev_cond->header.subtype;
    python_verificator_ctx_t *l_ctx = cf_verificator_get(l_subtype);
    if (!l_ctx || !l_ctx->in_verify_callback) {
        return 0; // No callback - allow
    }
    
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_tx = PyCapsule_New(a_tx_in, "dap_chain_datum_tx_t", NULL);
    PyObject *l_tx_hash = a_tx_in_hash ? PyBytes_FromStringAndSize((const char*)a_tx_in_hash, sizeof(dap_hash_fast_t)) : Py_None;
    PyObject *l_cond = PyCapsule_New(a_prev_cond, "dap_chain_tx_out_cond_t", NULL);
    PyObject *l_owner = PyBool_FromLong(a_owner);
    PyObject *l_from_mempool = PyBool_FromLong(a_from_mempool);
    
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->in_verify_callback, l_ledger, l_tx, l_tx_hash, l_cond, l_owner, l_from_mempool, l_ctx->user_data, NULL
    );
    
    int l_ret = 0;
    if (!l_result) {
        log_it(L_ERROR, "Python in_verify callback raised an exception");
        PyErr_Print();
    } else {
        l_ret = PyLong_AsLong(l_result);
        Py_DECREF(l_result);
    }
    
    Py_DECREF(l_ledger);
    Py_DECREF(l_tx);
    Py_XDECREF(l_tx_hash);
    Py_DECREF(l_cond);
    Py_DECREF(l_owner);
    Py_DECREF(l_from_mempool);
    
    PyGILState_Release(l_gstate);
    return l_ret;
}

/**
 * @brief C wrapper for cond_out_verify callback - calls Python callback
 * Signature: int (*dap_ledger_cond_out_verify_callback_t)(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_out, 
 *                                                           dap_hash_fast_t *a_tx_out_hash, dap_chain_tx_out_cond_t *a_cond)
 */
static int s_verificator_out_verify_wrapper(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_out, 
                                             dap_hash_fast_t *a_tx_out_hash, dap_chain_tx_out_cond_t *a_cond) {
    if (!a_cond) {
        return 0;
    }
    
    dap_chain_tx_out_cond_subtype_t l_subtype = a_cond->header.subtype;
    python_verificator_ctx_t *l_ctx = cf_verificator_get(l_subtype);
    if (!l_ctx || !l_ctx->out_verify_callback) {
        return 0; // No callback - allow
    }
    
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_tx = PyCapsule_New(a_tx_out, "dap_chain_datum_tx_t", NULL);
    PyObject *l_tx_hash = a_tx_out_hash ? PyBytes_FromStringAndSize((const char*)a_tx_out_hash, sizeof(dap_hash_fast_t)) : Py_None;
    PyObject *l_cond = PyCapsule_New(a_cond, "dap_chain_tx_out_cond_t", NULL);
    
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->out_verify_callback, l_ledger, l_tx, l_tx_hash, l_cond, l_ctx->user_data, NULL
    );
    
    int l_ret = 0;
    if (!l_result) {
        log_it(L_ERROR, "Python out_verify callback raised an exception");
        PyErr_Print();
    } else {
        l_ret = PyLong_AsLong(l_result);
        Py_DECREF(l_result);
    }
    
    Py_DECREF(l_ledger);
    Py_DECREF(l_tx);
    Py_XDECREF(l_tx_hash);
    Py_DECREF(l_cond);
    
    PyGILState_Release(l_gstate);
    return l_ret;
}

/**
 * @brief C wrapper for cond_in_add callback - calls Python callback
 * Signature: void (*dap_ledger_cond_in_add_callback_t)(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_in,  
 *                                                        dap_hash_fast_t *a_tx_in_hash, dap_chain_tx_out_cond_t *a_prev_cond)
 */
static void s_verificator_in_add_wrapper(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_in,  
                                         dap_hash_fast_t *a_tx_in_hash, dap_chain_tx_out_cond_t *a_prev_cond) {
    if (!a_prev_cond) {
        return;
    }
    
    dap_chain_tx_out_cond_subtype_t l_subtype = a_prev_cond->header.subtype;
    python_verificator_ctx_t *l_ctx = cf_verificator_get(l_subtype);
    if (!l_ctx || !l_ctx->in_add_callback) {
        return;
    }
    
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_tx = PyCapsule_New(a_tx_in, "dap_chain_datum_tx_t", NULL);
    PyObject *l_tx_hash = a_tx_in_hash ? PyBytes_FromStringAndSize((const char*)a_tx_in_hash, sizeof(dap_hash_fast_t)) : Py_None;
    PyObject *l_cond = PyCapsule_New(a_prev_cond, "dap_chain_tx_out_cond_t", NULL);
    
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->in_add_callback, l_ledger, l_tx, l_tx_hash, l_cond, l_ctx->user_data, NULL
    );
    
    if (!l_result) {
        log_it(L_ERROR, "Python in_add callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }
    
    Py_DECREF(l_ledger);
    Py_DECREF(l_tx);
    Py_XDECREF(l_tx_hash);
    Py_DECREF(l_cond);
    
    PyGILState_Release(l_gstate);
}

/**
 * @brief C wrapper for cond_out_add callback - calls Python callback
 * Signature: void (*dap_ledger_cond_out_add_callback_t)(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_out, 
 *                                                         dap_hash_fast_t *a_tx_out_hash, dap_chain_tx_out_cond_t *a_cond)
 */
static void s_verificator_out_add_wrapper(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_out, 
                                          dap_hash_fast_t *a_tx_out_hash, dap_chain_tx_out_cond_t *a_cond) {
    if (!a_cond) {
        return;
    }
    
    dap_chain_tx_out_cond_subtype_t l_subtype = a_cond->header.subtype;
    python_verificator_ctx_t *l_ctx = cf_verificator_get(l_subtype);
    if (!l_ctx || !l_ctx->out_add_callback) {
        return;
    }
    
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_tx = PyCapsule_New(a_tx_out, "dap_chain_datum_tx_t", NULL);
    PyObject *l_tx_hash = a_tx_out_hash ? PyBytes_FromStringAndSize((const char*)a_tx_out_hash, sizeof(dap_hash_fast_t)) : Py_None;
    PyObject *l_cond = PyCapsule_New(a_cond, "dap_chain_tx_out_cond_t", NULL);
    
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->out_add_callback, l_ledger, l_tx, l_tx_hash, l_cond, l_ctx->user_data, NULL
    );
    
    if (!l_result) {
        log_it(L_ERROR, "Python out_add callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }
    
    Py_DECREF(l_ledger);
    Py_DECREF(l_tx);
    Py_XDECREF(l_tx_hash);
    Py_DECREF(l_cond);
    
    PyGILState_Release(l_gstate);
}

/**
 * @brief C wrapper for cond_in_delete callback - calls Python callback
 * Signature: void (*dap_ledger_cond_in_delete_callback_t)(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_in,  
 *                                                           dap_hash_fast_t *a_tx_in_hash, dap_chain_tx_out_cond_t *a_prev_cond)
 */
static void s_verificator_in_delete_wrapper(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_in,  
                                            dap_hash_fast_t *a_tx_in_hash, dap_chain_tx_out_cond_t *a_prev_cond) {
    if (!a_prev_cond) {
        return;
    }
    
    dap_chain_tx_out_cond_subtype_t l_subtype = a_prev_cond->header.subtype;
    python_verificator_ctx_t *l_ctx = cf_verificator_get(l_subtype);
    if (!l_ctx || !l_ctx->in_delete_callback) {
        return;
    }
    
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_tx = PyCapsule_New(a_tx_in, "dap_chain_datum_tx_t", NULL);
    PyObject *l_tx_hash = a_tx_in_hash ? PyBytes_FromStringAndSize((const char*)a_tx_in_hash, sizeof(dap_hash_fast_t)) : Py_None;
    PyObject *l_cond = PyCapsule_New(a_prev_cond, "dap_chain_tx_out_cond_t", NULL);
    
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->in_delete_callback, l_ledger, l_tx, l_tx_hash, l_cond, l_ctx->user_data, NULL
    );
    
    if (!l_result) {
        log_it(L_ERROR, "Python in_delete callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }
    
    Py_DECREF(l_ledger);
    Py_DECREF(l_tx);
    Py_XDECREF(l_tx_hash);
    Py_DECREF(l_cond);
    
    PyGILState_Release(l_gstate);
}

/**
 * @brief C wrapper for cond_out_delete callback - calls Python callback
 * Signature: void (*dap_ledger_cond_out_delete_callback_t)(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_out, 
 *                                                            dap_hash_fast_t *a_tx_out_hash, dap_chain_tx_out_cond_t *a_cond)
 */
static void s_verificator_out_delete_wrapper(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx_out, 
                                             dap_hash_fast_t *a_tx_out_hash, dap_chain_tx_out_cond_t *a_cond) {
    if (!a_cond) {
        return;
    }
    
    dap_chain_tx_out_cond_subtype_t l_subtype = a_cond->header.subtype;
    python_verificator_ctx_t *l_ctx = cf_verificator_get(l_subtype);
    if (!l_ctx || !l_ctx->out_delete_callback) {
        return;
    }
    
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_tx = PyCapsule_New(a_tx_out, "dap_chain_datum_tx_t", NULL);
    PyObject *l_tx_hash = a_tx_out_hash ? PyBytes_FromStringAndSize((const char*)a_tx_out_hash, sizeof(dap_hash_fast_t)) : Py_None;
    PyObject *l_cond = PyCapsule_New(a_cond, "dap_chain_tx_out_cond_t", NULL);
    
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->out_delete_callback, l_ledger, l_tx, l_tx_hash, l_cond, l_ctx->user_data, NULL
    );
    
    if (!l_result) {
        log_it(L_ERROR, "Python out_delete callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }
    
    Py_DECREF(l_ledger);
    Py_DECREF(l_tx);
    Py_XDECREF(l_tx_hash);
    Py_DECREF(l_cond);
    
    PyGILState_Release(l_gstate);
}

// =========================================
// VOTING VERIFICATOR CALLBACK WRAPPERS (4 callbacks)
// =========================================

static int s_voting_callback_wrapper(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx,
                                     dap_hash_fast_t *a_tx_hash, bool a_apply) {
    python_voting_ctx_t *l_ctx = cf_voting_verificator_get();
    if (!l_ctx || !l_ctx->voting_callback) {
        return 0;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_tx = PyCapsule_New(a_tx, "dap_chain_datum_tx_t", NULL);
    PyObject *l_tx_hash = a_tx_hash ? PyBytes_FromStringAndSize((const char*)a_tx_hash, sizeof(dap_hash_fast_t)) : Py_None;
    PyObject *l_apply = PyBool_FromLong(a_apply);

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->voting_callback, l_ledger, l_tx, l_tx_hash, l_apply, l_ctx->user_data, NULL
    );

    int l_ret = 0;
    if (!l_result) {
        log_it(L_ERROR, "Python voting callback raised an exception");
        PyErr_Print();
    } else {
        l_ret = PyLong_AsLong(l_result);
        Py_DECREF(l_result);
    }

    Py_DECREF(l_ledger);
    Py_DECREF(l_tx);
    Py_XDECREF(l_tx_hash);
    Py_DECREF(l_apply);

    PyGILState_Release(l_gstate);
    return l_ret;
}

static int s_vote_callback_wrapper(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx,
                                   dap_hash_fast_t *a_tx_hash, dap_hash_fast_t *a_pkey_hash, bool a_apply) {
    python_voting_ctx_t *l_ctx = cf_voting_verificator_get();
    if (!l_ctx || !l_ctx->vote_callback) {
        return 0;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_tx = PyCapsule_New(a_tx, "dap_chain_datum_tx_t", NULL);
    PyObject *l_tx_hash = a_tx_hash ? PyBytes_FromStringAndSize((const char*)a_tx_hash, sizeof(dap_hash_fast_t)) : Py_None;
    PyObject *l_pkey_hash = a_pkey_hash ? PyBytes_FromStringAndSize((const char*)a_pkey_hash, sizeof(dap_hash_fast_t)) : Py_None;
    PyObject *l_apply = PyBool_FromLong(a_apply);

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->vote_callback, l_ledger, l_tx, l_tx_hash, l_pkey_hash, l_apply, l_ctx->user_data, NULL
    );

    int l_ret = 0;
    if (!l_result) {
        log_it(L_ERROR, "Python vote callback raised an exception");
        PyErr_Print();
    } else {
        l_ret = PyLong_AsLong(l_result);
        Py_DECREF(l_result);
    }

    Py_DECREF(l_ledger);
    Py_DECREF(l_tx);
    Py_XDECREF(l_tx_hash);
    Py_XDECREF(l_pkey_hash);
    Py_DECREF(l_apply);

    PyGILState_Release(l_gstate);
    return l_ret;
}

static bool s_voting_delete_wrapper(dap_ledger_t *a_ledger, dap_chain_tx_item_type_t a_type,
                                    dap_chain_datum_tx_t *a_tx, dap_hash_fast_t *a_tx_hash) {
    python_voting_ctx_t *l_ctx = cf_voting_verificator_get();
    if (!l_ctx || !l_ctx->delete_callback) {
        return false;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_type = PyLong_FromLong((long)a_type);
    PyObject *l_tx = PyCapsule_New(a_tx, "dap_chain_datum_tx_t", NULL);
    PyObject *l_tx_hash = a_tx_hash ? PyBytes_FromStringAndSize((const char*)a_tx_hash, sizeof(dap_hash_fast_t)) : Py_None;

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->delete_callback, l_ledger, l_type, l_tx, l_tx_hash, l_ctx->user_data, NULL
    );

    bool l_ret = false;
    if (!l_result) {
        log_it(L_ERROR, "Python voting delete callback raised an exception");
        PyErr_Print();
    } else {
        l_ret = PyObject_IsTrue(l_result);
        Py_DECREF(l_result);
    }

    Py_DECREF(l_ledger);
    Py_DECREF(l_type);
    Py_DECREF(l_tx);
    Py_XDECREF(l_tx_hash);

    PyGILState_Release(l_gstate);
    return l_ret;
}

static dap_time_t s_voting_expire_wrapper(dap_ledger_t *a_ledger, dap_hash_fast_t *a_voting_hash) {
    python_voting_ctx_t *l_ctx = cf_voting_verificator_get();
    if (!l_ctx || !l_ctx->expire_callback) {
        return 0;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_voting_hash = a_voting_hash ? PyBytes_FromStringAndSize((const char*)a_voting_hash, sizeof(dap_hash_fast_t)) : Py_None;

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->expire_callback, l_ledger, l_voting_hash, l_ctx->user_data, NULL
    );

    dap_time_t l_ret = 0;
    if (!l_result) {
        log_it(L_ERROR, "Python voting expire callback raised an exception");
        PyErr_Print();
    } else {
        l_ret = (dap_time_t)PyLong_AsUnsignedLongLong(l_result);
        Py_DECREF(l_result);
    }

    Py_DECREF(l_ledger);
    Py_XDECREF(l_voting_hash);

    PyGILState_Release(l_gstate);
    return l_ret;
}

/**
 * @brief Add verificator callbacks for conditional outputs
 * @param a_self Python self object (unused)
 * @param a_args Arguments (subtype, in_verify, out_verify, in_add, out_add, in_delete, out_delete, user_data)
 *              All callbacks except subtype are optional (can be None)
 * @return Integer result code
 */
PyObject* dap_ledger_verificator_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_subtype;
    PyObject *l_in_verify = Py_None;
    PyObject *l_out_verify = Py_None;
    PyObject *l_in_add = Py_None;
    PyObject *l_out_add = Py_None;
    PyObject *l_in_delete = Py_None;
    PyObject *l_out_delete = Py_None;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "i|OOOOOOO", &l_subtype, &l_in_verify, &l_out_verify, 
                          &l_in_add, &l_out_add, &l_in_delete, &l_out_delete, &l_user_data)) {
        PyErr_SetString(PyExc_TypeError, "Expected (subtype, [in_verify], [out_verify], [in_add], [out_add], [in_delete], [out_delete], [user_data])");
        return NULL;
    }
    
    // Validate callbacks are callable or None
    if (l_in_verify != Py_None && !PyCallable_Check(l_in_verify)) {
        PyErr_SetString(PyExc_TypeError, "in_verify must be callable or None");
        return NULL;
    }
    if (l_out_verify != Py_None && !PyCallable_Check(l_out_verify)) {
        PyErr_SetString(PyExc_TypeError, "out_verify must be callable or None");
        return NULL;
    }
    if (l_in_add != Py_None && !PyCallable_Check(l_in_add)) {
        PyErr_SetString(PyExc_TypeError, "in_add must be callable or None");
        return NULL;
    }
    if (l_out_add != Py_None && !PyCallable_Check(l_out_add)) {
        PyErr_SetString(PyExc_TypeError, "out_add must be callable or None");
        return NULL;
    }
    if (l_in_delete != Py_None && !PyCallable_Check(l_in_delete)) {
        PyErr_SetString(PyExc_TypeError, "in_delete must be callable or None");
        return NULL;
    }
    if (l_out_delete != Py_None && !PyCallable_Check(l_out_delete)) {
        PyErr_SetString(PyExc_TypeError, "out_delete must be callable or None");
        return NULL;
    }
    
    // Convert None to NULL for registry
    PyObject *l_in_verify_real = (l_in_verify == Py_None) ? NULL : l_in_verify;
    PyObject *l_out_verify_real = (l_out_verify == Py_None) ? NULL : l_out_verify;
    PyObject *l_in_add_real = (l_in_add == Py_None) ? NULL : l_in_add;
    PyObject *l_out_add_real = (l_out_add == Py_None) ? NULL : l_out_add;
    PyObject *l_in_delete_real = (l_in_delete == Py_None) ? NULL : l_in_delete;
    PyObject *l_out_delete_real = (l_out_delete == Py_None) ? NULL : l_out_delete;
    PyObject *l_user_data_real = (l_user_data == Py_None) ? NULL : l_user_data;
    
    // Register in global registry
    if (cf_verificator_register(l_subtype, l_in_verify_real, l_out_verify_real, 
                                l_in_add_real, l_out_add_real, l_in_delete_real, 
                                l_out_delete_real, l_user_data_real) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register verificator callbacks");
        return NULL;
    }
    
    // Register C wrappers with SDK
    int l_result = dap_ledger_verificator_add(
        l_subtype,
        l_in_verify_real ? s_verificator_in_verify_wrapper : NULL,
        l_out_verify_real ? s_verificator_out_verify_wrapper : NULL,
        l_in_add_real ? s_verificator_in_add_wrapper : NULL,
        l_out_add_real ? s_verificator_out_add_wrapper : NULL,
        l_in_delete_real ? s_verificator_in_delete_wrapper : NULL,
        l_out_delete_real ? s_verificator_out_delete_wrapper : NULL
    );
    
    log_it(L_DEBUG, "Registered verificator for subtype %d, result=%d", l_subtype, l_result);
    
    return PyLong_FromLong(l_result);
}

/**
 * @brief Add voting verificator callbacks
 * @param a_self Python self object (unused)
 * @param a_args Arguments (voting, vote, delete, expire, user_data)
 * @return Integer result code
 */
PyObject* dap_ledger_voting_verificator_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_voting = Py_None;
    PyObject *l_vote = Py_None;
    PyObject *l_delete = Py_None;
    PyObject *l_expire = Py_None;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "OOOO|O", &l_voting, &l_vote, &l_delete, &l_expire, &l_user_data)) {
        PyErr_SetString(PyExc_TypeError, "Expected (voting, vote, delete, expire, [user_data])");
        return NULL;
    }

    if (l_voting != Py_None && !PyCallable_Check(l_voting)) {
        PyErr_SetString(PyExc_TypeError, "voting must be callable or None");
        return NULL;
    }
    if (l_vote != Py_None && !PyCallable_Check(l_vote)) {
        PyErr_SetString(PyExc_TypeError, "vote must be callable or None");
        return NULL;
    }
    if (l_delete != Py_None && !PyCallable_Check(l_delete)) {
        PyErr_SetString(PyExc_TypeError, "delete must be callable or None");
        return NULL;
    }
    if (l_expire != Py_None && !PyCallable_Check(l_expire)) {
        PyErr_SetString(PyExc_TypeError, "expire must be callable or None");
        return NULL;
    }

    PyObject *l_voting_real = (l_voting == Py_None) ? NULL : l_voting;
    PyObject *l_vote_real = (l_vote == Py_None) ? NULL : l_vote;
    PyObject *l_delete_real = (l_delete == Py_None) ? NULL : l_delete;
    PyObject *l_expire_real = (l_expire == Py_None) ? NULL : l_expire;
    PyObject *l_user_data_real = (l_user_data == Py_None) ? NULL : l_user_data;

    if (cf_voting_verificator_register(l_voting_real, l_vote_real, l_delete_real, l_expire_real, l_user_data_real) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register voting verificator callbacks");
        return NULL;
    }

    int l_result = dap_ledger_voting_verificator_add(
        l_voting_real ? s_voting_callback_wrapper : NULL,
        l_vote_real ? s_vote_callback_wrapper : NULL,
        l_delete_real ? s_voting_delete_wrapper : NULL,
        l_expire_real ? s_voting_expire_wrapper : NULL
    );

    log_it(L_DEBUG, "Registered voting verificator callbacks, result=%d", l_result);
    return PyLong_FromLong(l_result);
}


// Get method definitions for cond module
PyMethodDef* cellframe_ledger_cond_get_methods(void) {
    static PyMethodDef cond_methods[] = {
        {"ledger_out_cond_unspent_find_by_addr", (PyCFunction)dap_ledger_out_cond_unspent_find_by_addr_py, METH_VARARGS,
         "Find unspent conditional output by address"},
        {"ledger_get_tx_cond_out", (PyCFunction)dap_ledger_get_tx_cond_out_py, METH_VARARGS,
         "Get first conditional output by subtype"},
        {"ledger_get_tx_out_cond_linked_to_tx_in_cond", (PyCFunction)dap_chain_ledger_get_tx_out_cond_linked_to_tx_in_cond_py, METH_VARARGS,
         "Get linked output condition for input condition"},
        {"ledger_get_list_tx_cond_outs", (PyCFunction)dap_ledger_get_list_tx_cond_outs_py, METH_VARARGS,
         "Get list of conditional outputs"},
        {"ledger_get_utxo_for_value", (PyCFunction)dap_ledger_get_utxo_for_value_py, METH_VARARGS,
         "Get UTXO list for value"},
        {"ledger_tx_cache_find_out_cond_all", (PyCFunction)dap_ledger_tx_cache_find_out_cond_all_py, METH_VARARGS,
         "Find all conditional outputs from TX cache by service UID"},
        {"ledger_check_condition_owner", (PyCFunction)dap_ledger_check_condition_owner_py, METH_VARARGS,
         "Check condition owner"},
        {"ledger_verificator_add", (PyCFunction)dap_ledger_verificator_add_py, METH_VARARGS,
         "Add verificator callbacks for conditional outputs"},
        {"ledger_voting_verificator_add", (PyCFunction)dap_ledger_voting_verificator_add_py, METH_VARARGS,
         "Add voting verificator callbacks"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return cond_methods;
}
