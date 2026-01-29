#include "include/cf_ledger_internal.h"
#include "dap_chain_ledger_tx.h"

/*
 * Cellframe ledger transaction bindings
 * Transaction add, load, remove, validation operations
 */

extern dap_chain_datum_tx_t *dap_ledger_tx_create_event(
    dap_list_t *a_list_used_outs,
    dap_pkey_t *a_pkey_service,
    dap_chain_srv_uid_t a_srv_uid,
    const char *a_group_name,
    uint16_t a_event_type,
    const void *a_event_data,
    size_t a_event_data_size,
    uint256_t a_value_fee) __attribute__((weak));

extern dap_chain_datum_tx_t *dap_ledger_tx_create_from_emission(
    dap_chain_hash_fast_t *a_emission_hash,
    dap_chain_id_t a_emission_chain_id,
    uint256_t a_emission_value,
    const char *a_ticker,
    dap_chain_addr_t *a_addr_to,
    uint256_t a_value_fee) __attribute__((weak));


// =============================================================================
// TRANSACTION OPERATIONS
// =============================================================================

/**
 * @brief Add transaction to ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, tx capsule, tx_hash bytes, from_threshold bool)
 * @return 0 on success, error code on failure
 */
PyObject* dap_ledger_tx_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    PyObject *l_tx_capsule = NULL;
    const char *l_tx_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    int l_from_threshold = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOy#p", &l_ledger_capsule, &l_tx_capsule, &l_tx_hash_bytes, &l_hash_size, &l_from_threshold)) {
        log_it(L_ERROR, "Invalid arguments for ledger_tx_add");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (!l_tx_capsule || !PyCapsule_CheckExact(l_tx_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected transaction capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t*)PyCapsule_GetPointer(l_tx_capsule, "dap_chain_datum_tx_t");
    
    if (!l_ledger || !l_tx) {
        log_it(L_ERROR, "Failed to extract pointers from capsules");
        return NULL;
    }
    
    dap_hash_fast_t *l_tx_hash = (dap_hash_fast_t*)l_tx_hash_bytes;
    
    int l_result = dap_ledger_tx_add(l_ledger, l_tx, l_tx_hash, (bool)l_from_threshold, NULL);
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to add transaction to ledger: %d", l_result);
    } else {
        log_it(L_INFO, "Transaction added to ledger successfully");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Load transaction to ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, tx capsule, tx_hash bytes)
 * @return 0 on success, error code on failure
 */
PyObject* dap_ledger_tx_load_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    PyObject *l_tx_capsule = NULL;
    const char *l_tx_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOy#", &l_ledger_capsule, &l_tx_capsule, &l_tx_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for ledger_tx_load");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (!l_tx_capsule || !PyCapsule_CheckExact(l_tx_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected transaction capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t*)PyCapsule_GetPointer(l_tx_capsule, "dap_chain_datum_tx_t");
    
    if (!l_ledger || !l_tx) {
        log_it(L_ERROR, "Failed to extract pointers from capsules");
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_tx_hash = (dap_chain_hash_fast_t*)l_tx_hash_bytes;
    
    int l_result = dap_ledger_tx_load(l_ledger, l_tx, l_tx_hash, NULL);
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to load transaction to ledger: %d", l_result);
    } else {
        log_it(L_DEBUG, "Transaction loaded to ledger successfully");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Remove transaction from ledger (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, tx capsule, tx_hash bytes)
 * @return 0 on success, error code on failure
 */
PyObject* dap_ledger_tx_remove_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    PyObject *l_tx_capsule = NULL;
    const char *l_tx_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOy#", &l_ledger_capsule, &l_tx_capsule, &l_tx_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for ledger_tx_remove");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (!l_tx_capsule || !PyCapsule_CheckExact(l_tx_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected transaction capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t*)PyCapsule_GetPointer(l_tx_capsule, "dap_chain_datum_tx_t");
    
    if (!l_ledger || !l_tx) {
        log_it(L_ERROR, "Failed to extract pointers from capsules");
        return NULL;
    }
    
    dap_hash_fast_t *l_tx_hash = (dap_hash_fast_t*)l_tx_hash_bytes;
    
    int l_result = dap_ledger_tx_remove(l_ledger, l_tx, l_tx_hash);
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to remove transaction from ledger: %d", l_result);
    } else {
        log_it(L_INFO, "Transaction removed from ledger successfully");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Check/validate transaction before adding (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (ledger capsule, tx bytes, tx_hash bytes)
 * @return 0 if valid, error code otherwise
 */
PyObject* dap_ledger_tx_add_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_capsule = NULL;
    const char *l_tx_bytes = NULL;
    Py_ssize_t l_tx_size = 0;
    const char *l_tx_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "Oy#y#", &l_ledger_capsule, &l_tx_bytes, &l_tx_size, &l_tx_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for ledger_tx_add_check");
        return NULL;
    }
    
    if (!l_ledger_capsule || !PyCapsule_CheckExact(l_ledger_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected ledger capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t*)PyCapsule_GetPointer(l_ledger_capsule, "dap_ledger_t");
    if (!l_ledger) {
        log_it(L_ERROR, "Failed to extract ledger pointer");
        return NULL;
    }
    
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t*)l_tx_bytes;
    dap_hash_fast_t *l_tx_hash = (dap_hash_fast_t*)l_tx_hash_bytes;
    
    int l_result = dap_ledger_tx_add_check(l_ledger, l_tx, (size_t)l_tx_size, l_tx_hash);
    
    if (l_result != 0) {
        log_it(L_DEBUG, "Transaction validation failed: %d", l_result);
    } else {
        log_it(L_DEBUG, "Transaction validation passed");
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Create event transaction (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (used_outs capsule or None, pkey capsule, srv_uid, group_name, event_type, event_data bytes, fee bytes)
 * @return PyCapsule wrapping dap_chain_datum_tx_t* or None
 */
PyObject* dap_ledger_tx_create_event_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_used_outs_obj = Py_None;
    PyObject *l_pkey_obj = NULL;
    unsigned long long l_srv_uid = 0;
    const char *l_group_name = NULL;
    unsigned short l_event_type = 0;
    const char *l_event_data = NULL;
    Py_ssize_t l_event_data_size = 0;
    const char *l_fee_bytes = NULL;
    Py_ssize_t l_fee_size = 0;

    if (!PyArg_ParseTuple(a_args, "OOKsHy#y#", &l_used_outs_obj, &l_pkey_obj, &l_srv_uid,
                          &l_group_name, &l_event_type, &l_event_data, &l_event_data_size,
                          &l_fee_bytes, &l_fee_size)) {
        return NULL;
    }

    if (!dap_ledger_tx_create_event) {
        PyErr_SetString(PyExc_RuntimeError, "dap_ledger_tx_create_event is not available in this SDK build");
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

    if (!PyCapsule_CheckExact(l_pkey_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a dap_pkey_t capsule");
        return NULL;
    }

    dap_pkey_t *l_pkey = (dap_pkey_t *)PyCapsule_GetPointer(l_pkey_obj, "dap_pkey_t");
    if (!l_pkey) {
        PyErr_SetString(PyExc_ValueError, "Invalid dap_pkey_t capsule");
        return NULL;
    }

    if ((size_t)l_fee_size != sizeof(uint256_t)) {
        PyErr_Format(PyExc_ValueError, "Fee value must be exactly %zu bytes", sizeof(uint256_t));
        return NULL;
    }

    uint256_t l_fee_value = *(uint256_t *)l_fee_bytes;
    dap_chain_srv_uid_t l_srv_uid_struct = {.uint64 = l_srv_uid};

    dap_chain_datum_tx_t *l_tx = dap_ledger_tx_create_event(
        l_used_outs,
        l_pkey,
        l_srv_uid_struct,
        l_group_name,
        (uint16_t)l_event_type,
        l_event_data,
        (size_t)l_event_data_size,
        l_fee_value);

    if (!l_tx) {
        log_it(L_WARNING, "Failed to create event transaction");
        Py_RETURN_NONE;
    }

    log_it(L_DEBUG, "Event transaction created");
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

/**
 * @brief Create transaction from emission (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (emission_hash bytes, chain_id, emission_value bytes, ticker, addr capsule/bytes, fee bytes)
 * @return PyCapsule wrapping dap_chain_datum_tx_t* or None
 */
PyObject* dap_ledger_tx_create_from_emission_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_emission_hash_bytes = NULL;
    Py_ssize_t l_emission_hash_size = 0;
    unsigned long long l_chain_id = 0;
    const char *l_emission_value_bytes = NULL;
    Py_ssize_t l_emission_value_size = 0;
    const char *l_ticker = NULL;
    PyObject *l_addr_obj = NULL;
    const char *l_fee_bytes = NULL;
    Py_ssize_t l_fee_size = 0;

    if (!PyArg_ParseTuple(a_args, "y#Ky#sOy#", &l_emission_hash_bytes, &l_emission_hash_size, &l_chain_id,
                          &l_emission_value_bytes, &l_emission_value_size, &l_ticker, &l_addr_obj,
                          &l_fee_bytes, &l_fee_size)) {
        return NULL;
    }

    if (!dap_ledger_tx_create_from_emission) {
        PyErr_SetString(PyExc_RuntimeError, "dap_ledger_tx_create_from_emission is not available in this SDK build");
        return NULL;
    }

    if ((size_t)l_emission_hash_size != sizeof(dap_chain_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Emission hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
        return NULL;
    }

    if ((size_t)l_emission_value_size != sizeof(uint256_t)) {
        PyErr_Format(PyExc_ValueError, "Emission value must be exactly %zu bytes", sizeof(uint256_t));
        return NULL;
    }

    if ((size_t)l_fee_size != sizeof(uint256_t)) {
        PyErr_Format(PyExc_ValueError, "Fee value must be exactly %zu bytes", sizeof(uint256_t));
        return NULL;
    }

    dap_chain_addr_t *l_addr = NULL;
    dap_chain_addr_t l_addr_tmp;
    if (PyCapsule_CheckExact(l_addr_obj)) {
        l_addr = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_obj, "dap_chain_addr_t");
        if (!l_addr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_chain_addr_t capsule");
            return NULL;
        }
    } else if (PyBytes_Check(l_addr_obj)) {
        if ((size_t)PyBytes_Size(l_addr_obj) != sizeof(dap_chain_addr_t)) {
            PyErr_Format(PyExc_ValueError, "Address must be exactly %zu bytes", sizeof(dap_chain_addr_t));
            return NULL;
        }
        memcpy(&l_addr_tmp, PyBytes_AsString(l_addr_obj), sizeof(dap_chain_addr_t));
        l_addr = &l_addr_tmp;
    } else {
        PyErr_SetString(PyExc_TypeError, "Address must be a dap_chain_addr_t capsule or bytes");
        return NULL;
    }

    dap_chain_hash_fast_t l_emission_hash;
    memcpy(&l_emission_hash, l_emission_hash_bytes, sizeof(dap_chain_hash_fast_t));

    uint256_t l_emission_value = *(uint256_t *)l_emission_value_bytes;
    uint256_t l_fee_value = *(uint256_t *)l_fee_bytes;

    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    dap_chain_datum_tx_t *l_tx = dap_ledger_tx_create_from_emission(
        &l_emission_hash,
        l_chain_id_struct,
        l_emission_value,
        l_ticker,
        l_addr,
        l_fee_value);

    if (!l_tx) {
        log_it(L_WARNING, "Failed to create transaction from emission");
        Py_RETURN_NONE;
    }

    log_it(L_DEBUG, "Emission transaction created");
    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

/**
 * @brief Register ledger TX builders (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return 0 on success, negative on error
 */
PyObject* dap_ledger_tx_builders_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    int l_result = dap_ledger_tx_builders_register();
    return PyLong_FromLong(l_result);
}

/**
 * @brief Unregister ledger TX builders (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_ledger_tx_builders_unregister_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    dap_ledger_tx_builders_unregister();
    Py_RETURN_NONE;
}


// Get method definitions for tx module
PyMethodDef* cellframe_ledger_tx_get_methods(void) {
    static PyMethodDef tx_methods[] = {
        {"ledger_tx_add", (PyCFunction)dap_ledger_tx_add_py, METH_VARARGS,
         "Add transaction to ledger"},
        {"ledger_tx_load", (PyCFunction)dap_ledger_tx_load_py, METH_VARARGS,
         "Load transaction to ledger"},
        {"ledger_tx_remove", (PyCFunction)dap_ledger_tx_remove_py, METH_VARARGS,
         "Remove transaction from ledger"},
        {"ledger_tx_add_check", (PyCFunction)dap_ledger_tx_add_check_py, METH_VARARGS,
         "Check/validate transaction before adding"},
        {"ledger_tx_create_event", (PyCFunction)dap_ledger_tx_create_event_py, METH_VARARGS,
         "Create event transaction"},
        {"ledger_tx_create_from_emission", (PyCFunction)dap_ledger_tx_create_from_emission_py, METH_VARARGS,
         "Create transaction from emission"},
        {"ledger_tx_builders_register", (PyCFunction)dap_ledger_tx_builders_register_py, METH_NOARGS,
         "Register ledger TX builders"},
        {"ledger_tx_builders_unregister", (PyCFunction)dap_ledger_tx_builders_unregister_py, METH_NOARGS,
         "Unregister ledger TX builders"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return tx_methods;
}
