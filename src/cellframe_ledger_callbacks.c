#include "cellframe_ledger_internal.h"

/*
 * Cellframe ledger callbacks bindings
 * Callbacks and notifications: tx add, event, bridged tx notifications
 */


// =============================================================================
// CALLBACKS & NOTIFICATIONS
// =============================================================================

/**
 * @brief Convert dap_chain_tx_event_t to Python dict
 */
PyObject* py_chain_tx_event_to_dict(dap_chain_tx_event_t *a_event) {
    if (!a_event) {
        Py_RETURN_NONE;
    }
    
    PyObject *l_dict = PyDict_New();
    if (!l_dict) {
        return NULL;
    }
    
    // srv_uid
    PyObject *l_srv_uid = PyLong_FromUnsignedLongLong(a_event->srv_uid.uint64);
    if (l_srv_uid) {
        PyDict_SetItemString(l_dict, "srv_uid", l_srv_uid);
        Py_DECREF(l_srv_uid);
    }
    
    // timestamp
    PyObject *l_timestamp = PyLong_FromUnsignedLongLong(a_event->timestamp);
    if (l_timestamp) {
        PyDict_SetItemString(l_dict, "timestamp", l_timestamp);
        Py_DECREF(l_timestamp);
    }
    
    // group_name
    if (a_event->group_name) {
        PyObject *l_group_name = PyUnicode_FromString(a_event->group_name);
        if (l_group_name) {
            PyDict_SetItemString(l_dict, "group_name", l_group_name);
            Py_DECREF(l_group_name);
        }
    }
    
    // tx_hash
    PyObject *l_tx_hash = PyBytes_FromStringAndSize((const char*)a_event->tx_hash.raw, sizeof(dap_hash_fast_t));
    if (l_tx_hash) {
        PyDict_SetItemString(l_dict, "tx_hash", l_tx_hash);
        Py_DECREF(l_tx_hash);
    }
    
    // pkey_hash
    PyObject *l_pkey_hash = PyBytes_FromStringAndSize((const char*)a_event->pkey_hash.raw, sizeof(dap_hash_fast_t));
    if (l_pkey_hash) {
        PyDict_SetItemString(l_dict, "pkey_hash", l_pkey_hash);
        Py_DECREF(l_pkey_hash);
    }
    
    // event_type
    PyObject *l_event_type = PyLong_FromUnsignedLong(a_event->event_type);
    if (l_event_type) {
        PyDict_SetItemString(l_dict, "event_type", l_event_type);
        Py_DECREF(l_event_type);
    }
    
    // event_data
    if (a_event->event_data && a_event->event_data_size > 0) {
        PyObject *l_event_data = PyBytes_FromStringAndSize((const char*)a_event->event_data, a_event->event_data_size);
        if (l_event_data) {
            PyDict_SetItemString(l_dict, "event_data", l_event_data);
            Py_DECREF(l_event_data);
        }
    }
    
    return l_dict;
}

/**
 * @brief C callback wrapper for event notifications
 */
void py_ledger_event_notify_callback(void *a_arg, dap_ledger_t *a_ledger, 
                                            dap_chain_tx_event_t *a_event, 
                                            dap_hash_fast_t *a_tx_hash,
                                            dap_ledger_notify_opcodes_t a_opcode) {
    py_ledger_callback_data_t *l_data = (py_ledger_callback_data_t*)a_arg;
    if (!l_data || !l_data->callback) {
        return;
    }
    
    // Acquire GIL
    PyGILState_STATE l_gil_state = PyGILState_Ensure();
    
    // Convert event to Python dict
    PyObject *l_event_dict = py_chain_tx_event_to_dict(a_event);
    if (!l_event_dict) {
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Convert tx_hash to bytes
    PyObject *l_tx_hash_bytes = PyBytes_FromStringAndSize((const char*)a_tx_hash->raw, sizeof(dap_hash_fast_t));
    if (!l_tx_hash_bytes) {
        Py_DECREF(l_event_dict);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Convert opcode to int
    PyObject *l_opcode = PyLong_FromLong((long)a_opcode);
    if (!l_opcode) {
        Py_DECREF(l_event_dict);
        Py_DECREF(l_tx_hash_bytes);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Call Python callback: callback(ledger_capsule, event_dict, tx_hash_bytes, opcode)
    PyObject *l_result = PyObject_CallFunctionObjArgs(l_data->callback, 
                                                       l_data->ledger_capsule,
                                                       l_event_dict,
                                                       l_tx_hash_bytes,
                                                       l_opcode,
                                                       NULL);
    
    // Cleanup
    Py_XDECREF(l_result);
    Py_DECREF(l_event_dict);
    Py_DECREF(l_tx_hash_bytes);
    Py_DECREF(l_opcode);
    
    // Check for Python exceptions
    if (PyErr_Occurred()) {
        PyErr_Print();
        PyErr_Clear();
    }
    
    // Release GIL
    PyGILState_Release(l_gil_state);
}

/**
 * @brief C callback wrapper for transaction add notifications
 */
void py_ledger_tx_add_notify_callback(void *a_arg, dap_ledger_t *a_ledger, 
                                             dap_chain_datum_tx_t *a_tx,
                                             dap_ledger_notify_opcodes_t a_opcode) {
    py_ledger_callback_data_t *l_data = (py_ledger_callback_data_t*)a_arg;
    if (!l_data || !l_data->callback) {
        return;
    }
    
    // Acquire GIL
    PyGILState_STATE l_gil_state = PyGILState_Ensure();
    
    // Convert tx to capsule
    PyObject *l_tx_capsule = PyCapsule_New(a_tx, "dap_chain_datum_tx_t", NULL);
    if (!l_tx_capsule) {
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Convert opcode to int
    PyObject *l_opcode = PyLong_FromLong((long)a_opcode);
    if (!l_opcode) {
        Py_DECREF(l_tx_capsule);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Call Python callback: callback(ledger_capsule, tx_capsule, opcode)
    PyObject *l_result = PyObject_CallFunctionObjArgs(l_data->callback,
                                                       l_data->ledger_capsule,
                                                       l_tx_capsule,
                                                       l_opcode,
                                                       NULL);
    
    // Cleanup
    Py_XDECREF(l_result);
    Py_DECREF(l_tx_capsule);
    Py_DECREF(l_opcode);
    
    // Check for Python exceptions
    if (PyErr_Occurred()) {
        PyErr_Print();
        PyErr_Clear();
    }
    
    // Release GIL
    PyGILState_Release(l_gil_state);
}

/**
 * @brief C callback wrapper for bridged transaction notifications
 */
void py_ledger_bridged_tx_notify_callback(dap_ledger_t *a_ledger, 
                                                 dap_chain_datum_tx_t *a_tx,
                                                 dap_hash_fast_t *a_tx_hash,
                                                 void *a_arg,
                                                 dap_ledger_notify_opcodes_t a_opcode) {
    py_ledger_callback_data_t *l_data = (py_ledger_callback_data_t*)a_arg;
    if (!l_data || !l_data->callback) {
        return;
    }
    
    // Acquire GIL
    PyGILState_STATE l_gil_state = PyGILState_Ensure();
    
    // Convert tx to capsule
    PyObject *l_tx_capsule = PyCapsule_New(a_tx, "dap_chain_datum_tx_t", NULL);
    if (!l_tx_capsule) {
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Convert tx_hash to bytes
    PyObject *l_tx_hash_bytes = PyBytes_FromStringAndSize((const char*)a_tx_hash->raw, sizeof(dap_hash_fast_t));
    if (!l_tx_hash_bytes) {
        Py_DECREF(l_tx_capsule);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Convert opcode to int
    PyObject *l_opcode = PyLong_FromLong((long)a_opcode);
    if (!l_opcode) {
        Py_DECREF(l_tx_capsule);
        Py_DECREF(l_tx_hash_bytes);
        PyGILState_Release(l_gil_state);
        return;
    }
    
    // Call Python callback: callback(ledger_capsule, tx_capsule, tx_hash_bytes, opcode)
    PyObject *l_result = PyObject_CallFunctionObjArgs(l_data->callback,
                                                       l_data->ledger_capsule,
                                                       l_tx_capsule,
                                                       l_tx_hash_bytes,
                                                       l_opcode,
                                                       NULL);
    
    // Cleanup
    Py_XDECREF(l_result);
    Py_DECREF(l_tx_capsule);
    Py_DECREF(l_tx_hash_bytes);
    Py_DECREF(l_opcode);
    
    // Check for Python exceptions
    if (PyErr_Occurred()) {
        PyErr_Print();
        PyErr_Clear();
    }
    
    // Release GIL
    PyGILState_Release(l_gil_state);
}

/**
 * @brief Cleanup callback data when capsule is destroyed
 */
static void py_ledger_callback_data_cleanup(PyObject *a_capsule) {
    py_ledger_callback_data_t *l_data = (py_ledger_callback_data_t*)PyCapsule_GetPointer(a_capsule, "py_ledger_callback_data_t");
    if (l_data) {
        Py_XDECREF(l_data->callback);
        Py_XDECREF(l_data->ledger_capsule);
        DAP_DELETE(l_data);
    }
}

/**
 * @brief Add transaction add notification callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, Python callable)
 * @return None
 */
PyObject* dap_ledger_tx_add_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    PyObject *l_callback;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_callback)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a callable");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    // Allocate callback data structure
    py_ledger_callback_data_t *l_data = DAP_NEW_Z(py_ledger_callback_data_t);
    if (!l_data) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback data");
        return NULL;
    }
    
    // Store Python objects with incremented references
    Py_INCREF(l_callback);
    Py_INCREF(l_ledger_obj);
    l_data->callback = l_callback;
    l_data->ledger_capsule = l_ledger_obj;
    
    // Register C callback
    dap_ledger_tx_add_notify(l_ledger, py_ledger_tx_add_notify_callback, l_data);
    
    log_it(L_DEBUG, "Registered transaction add notification callback");
    
    Py_RETURN_NONE;
}

/**
 * @brief Add event notification callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, Python callable)
 * @return None
 */
PyObject* dap_ledger_event_notify_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    PyObject *l_callback;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_callback)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a callable");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    // Allocate callback data structure
    py_ledger_callback_data_t *l_data = DAP_NEW_Z(py_ledger_callback_data_t);
    if (!l_data) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback data");
        return NULL;
    }
    
    // Store Python objects with incremented references
    Py_INCREF(l_callback);
    Py_INCREF(l_ledger_obj);
    l_data->callback = l_callback;
    l_data->ledger_capsule = l_ledger_obj;
    
    // Register C callback
    dap_ledger_event_notify_add(l_ledger, py_ledger_event_notify_callback, l_data);
    
    log_it(L_DEBUG, "Registered event notification callback");
    
    Py_RETURN_NONE;
}

/**
 * @brief Add bridged transaction notification callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, Python callable)
 * @return None
 */
PyObject* dap_ledger_bridged_tx_notify_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    PyObject *l_callback;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_callback)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a ledger capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a callable");
        return NULL;
    }
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    // Allocate callback data structure
    py_ledger_callback_data_t *l_data = DAP_NEW_Z(py_ledger_callback_data_t);
    if (!l_data) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback data");
        return NULL;
    }
    
    // Store Python objects with incremented references
    Py_INCREF(l_callback);
    Py_INCREF(l_ledger_obj);
    l_data->callback = l_callback;
    l_data->ledger_capsule = l_ledger_obj;
    
    // Register C callback
    dap_ledger_bridged_tx_notify_add(l_ledger, py_ledger_bridged_tx_notify_callback, l_data);
    
    log_it(L_DEBUG, "Registered bridged transaction notification callback");
    
    Py_RETURN_NONE;
}

// =============================================================================
// CALLBACKS & SPECIAL OPERATIONS
// =============================================================================

/**
 * @brief Get token emission rate
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, token_ticker string)
 * @return Bytes representing uint256_t
 */
PyObject* dap_ledger_token_get_emission_rate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_token_ticker;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_ledger_obj, &l_token_ticker)) {
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
    
    uint256_t l_rate = dap_ledger_token_get_emission_rate(l_ledger, l_token_ticker);
    
    log_it(L_DEBUG, "Got emission rate for token '%s'", l_token_ticker);
    return PyBytes_FromStringAndSize((const char *)&l_rate, sizeof(uint256_t));
}

/**
 * @brief Mark token emissions hardfork
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, hardfork_time uint64)
 * @return Integer result code
 */
PyObject* dap_ledger_token_emissions_mark_hardfork_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    unsigned long long l_hardfork_time;
    
    if (!PyArg_ParseTuple(a_args, "OK", &l_ledger_obj, &l_hardfork_time)) {
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
    
    int l_result = dap_ledger_token_emissions_mark_hardfork(l_ledger, l_hardfork_time);
    
    log_it(L_DEBUG, "Mark emissions hardfork at time %llu, result: %d", l_hardfork_time, l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Add service with callback (stub - callbacks require GIL management)
 * @param a_self Python self object (unused)
 * @param a_args Arguments (service_uid uint64, tag_str string)
 * @return Integer result code (stub returns -1)
 */
PyObject* dap_ledger_service_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_uid;
    const char *l_tag_str;
    
    if (!PyArg_ParseTuple(a_args, "Ks", &l_uid, &l_tag_str)) {
        return NULL;
    }
    
    log_it(L_WARNING, "dap_ledger_service_add: stub implementation (callback registration requires GIL management)");
    log_it(L_INFO, "Service add requested: UID=%llu, tag='%s' - not implemented", l_uid, l_tag_str);
    
    // Stub: cannot register C callback from Python without proper GIL management
    // Full implementation would require Python callback wrapper
    return PyLong_FromLong(-1);  // Return error to indicate stub
}

/**
 * @brief Set tax callback (stub - callbacks require GIL management)
 * @param a_self Python self object (unused)
 * @param a_args No arguments (callback would be passed if implemented)
 * @return Integer result code (stub returns -1)
 */
PyObject* dap_ledger_tax_callback_set_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    log_it(L_WARNING, "dap_ledger_tax_callback_set: stub implementation (callback registration requires GIL management)");
    log_it(L_INFO, "Tax callback set requested - not implemented");
    
    // Stub: cannot register C callback from Python without proper GIL management
    return PyLong_FromLong(-1);  // Return error to indicate stub
}


// Get method definitions for callbacks module
PyMethodDef* cellframe_ledger_callbacks_get_methods(void) {
    static PyMethodDef callbacks_methods[] = {
        {"ledger_tx_add_notify", (PyCFunction)dap_ledger_tx_add_notify_py, METH_VARARGS,
         "Add transaction add notification callback"},
        {"ledger_event_notify_add", (PyCFunction)dap_ledger_event_notify_add_py, METH_VARARGS,
         "Add event notification callback"},
        {"ledger_bridged_tx_notify_add", (PyCFunction)dap_ledger_bridged_tx_notify_add_py, METH_VARARGS,
         "Add bridged transaction notification callback"},
        {"ledger_token_get_emission_rate", (PyCFunction)dap_ledger_token_get_emission_rate_py, METH_VARARGS,
         "Get token emission rate"},
        {"ledger_token_emissions_mark_hardfork", (PyCFunction)dap_ledger_token_emissions_mark_hardfork_py, METH_VARARGS,
         "Mark token emissions hardfork"},
        {"ledger_service_add", (PyCFunction)dap_ledger_service_add_py, METH_VARARGS,
         "Add service with callback (stub)"},
        {"ledger_tax_callback_set", (PyCFunction)dap_ledger_tax_callback_set_py, METH_VARARGS,
         "Set tax callback (stub)"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return callbacks_methods;
}
