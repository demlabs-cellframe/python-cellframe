#include "cellframe_ledger_internal.h"

/*
 * Cellframe ledger event bindings
 * Event operations: pkey management, event finding and listing
 */


// =============================================================================
// EVENT OPERATIONS
// =============================================================================

/**
 * @brief Check if event public key is registered
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, pkey_hash bytes)
 * @return Integer result (0=not found, 1=found)
 */
PyObject* dap_ledger_event_pkey_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_pkey_hash_bytes;
    Py_ssize_t l_pkey_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_ledger_obj, &l_pkey_hash_bytes, &l_pkey_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_pkey_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Pkey hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_pkey_hash = (dap_hash_fast_t *)l_pkey_hash_bytes;
    int l_result = dap_ledger_event_pkey_check(l_ledger, l_pkey_hash);
    
    log_it(L_DEBUG, "Event pkey check result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Add event public key
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, pkey_hash bytes)
 * @return Integer result code
 */
PyObject* dap_ledger_event_pkey_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_pkey_hash_bytes;
    Py_ssize_t l_pkey_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_ledger_obj, &l_pkey_hash_bytes, &l_pkey_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_pkey_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Pkey hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_pkey_hash = (dap_hash_fast_t *)l_pkey_hash_bytes;
    int l_result = dap_ledger_event_pkey_add(l_ledger, l_pkey_hash);
    
    log_it(L_DEBUG, "Event pkey add result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Remove event public key
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, pkey_hash bytes)
 * @return Integer result code
 */
PyObject* dap_ledger_event_pkey_rm_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_pkey_hash_bytes;
    Py_ssize_t l_pkey_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_ledger_obj, &l_pkey_hash_bytes, &l_pkey_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_pkey_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Pkey hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_pkey_hash = (dap_hash_fast_t *)l_pkey_hash_bytes;
    int l_result = dap_ledger_event_pkey_rm(l_ledger, l_pkey_hash);
    
    log_it(L_DEBUG, "Event pkey remove result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Get list of event public keys
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_ledger_event_pkey_list_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_ledger_obj)) {
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
    
    dap_list_t *l_list = dap_ledger_event_pkey_list(l_ledger);
    if (!l_list) {
        log_it(L_DEBUG, "No event pkeys in list");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Got event pkey list");
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

/**
 * @brief Find event by transaction hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, tx_hash bytes)
 * @return PyCapsule wrapping dap_chain_tx_event_t* or None
 */
PyObject* dap_ledger_event_find_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_tx_hash_bytes;
    Py_ssize_t l_tx_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_ledger_obj, &l_tx_hash_bytes, &l_tx_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if ((size_t)l_tx_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "TX hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_tx_hash = (dap_hash_fast_t *)l_tx_hash_bytes;
    dap_chain_tx_event_t *l_event = dap_ledger_event_find(l_ledger, l_tx_hash);
    
    if (!l_event) {
        log_it(L_DEBUG, "Event not found by TX hash");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Event found by TX hash");
    return PyCapsule_New(l_event, "dap_chain_tx_event_t", NULL);
}

/**
 * @brief Get list of events with extended options
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, group_name string, need_lock bool)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_ledger_event_get_list_ex_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_group_name;
    int l_need_lock;
    
    if (!PyArg_ParseTuple(a_args, "Osp", &l_ledger_obj, &l_group_name, &l_need_lock)) {
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
    
    dap_list_t *l_list = dap_ledger_event_get_list_ex(l_ledger, l_group_name, (bool)l_need_lock);
    if (!l_list) {
        log_it(L_DEBUG, "No events found for group '%s'", l_group_name);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Got events list for group '%s' (lock=%d)", l_group_name, l_need_lock);
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

// Get method definitions for event module
PyMethodDef* cellframe_ledger_event_get_methods(void) {
    static PyMethodDef event_methods[] = {
        {"ledger_event_pkey_check", (PyCFunction)dap_ledger_event_pkey_check_py, METH_VARARGS,
         "Check if event public key is registered"},
        {"ledger_event_pkey_add", (PyCFunction)dap_ledger_event_pkey_add_py, METH_VARARGS,
         "Add event public key"},
        {"ledger_event_pkey_rm", (PyCFunction)dap_ledger_event_pkey_rm_py, METH_VARARGS,
         "Remove event public key"},
        {"ledger_event_pkey_list", (PyCFunction)dap_ledger_event_pkey_list_py, METH_VARARGS,
         "Get list of event public keys"},
        {"ledger_event_find", (PyCFunction)dap_ledger_event_find_py, METH_VARARGS,
         "Find event by transaction hash"},
        {"ledger_event_get_list_ex", (PyCFunction)dap_ledger_event_get_list_ex_py, METH_VARARGS,
         "Get list of events with extended options"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return event_methods;
}
