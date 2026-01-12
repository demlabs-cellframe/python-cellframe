#include "include/cf_ledger_internal.h"
#include "../common/cf_ledger_callback_registry.h"

/*
 * Cellframe ledger callbacks bindings
 * Callbacks and notifications: tx add, event, bridged tx notifications
 */

// =========================================
// SERVICE TAG CHECK CALLBACK WRAPPER
// =========================================

/**
 * @brief C wrapper for service tag check callback - calls Python callback
 * Signature: bool (*dap_ledger_tag_check_callback_t)(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx, 
 *                                                      dap_chain_datum_tx_item_groups_t *a_items_grp, 
 *                                                      dap_chain_tx_tag_action_type_t *a_action)
 */
static bool s_ledger_service_tag_check_wrapper(dap_ledger_t *a_ledger, dap_chain_datum_tx_t *a_tx,
                                                dap_chain_datum_tx_item_groups_t *a_items_grp,
                                                dap_chain_tx_tag_action_type_t *a_action) {
    // Note: SDK doesn't pass UID, we need to find it from tag in items_grp
    // For now, we'll iterate through all registered services to find matching callback
    // This is not optimal but SDK API limitation
    (void)a_ledger;
    (void)a_tx;
    (void)a_items_grp;
    (void)a_action;
    
    // TODO: Implement service UID lookup from items_grp
    log_it(L_WARNING, "Service tag check wrapper called - UID lookup not implemented");
    return true; // Default: allow
}

/**
 * @brief C wrapper for tax callback - calls Python callback
 * Signature: bool (*dap_ledger_tax_callback_t)(dap_chain_net_id_t a_net_id, dap_hash_fast_t *a_signer_pkey_hash, 
 *                                                dap_chain_addr_t *a_tax_addr, uint256_t *a_tax_value)
 */
static bool s_ledger_tax_callback_wrapper(dap_chain_net_id_t a_net_id, dap_hash_fast_t *a_signer_pkey_hash,
                                          dap_chain_addr_t *a_tax_addr, uint256_t *a_tax_value) {
    python_tax_ctx_t *l_ctx = cf_ledger_tax_callback_get();
    if (!l_ctx || !l_ctx->callback) {
        return true; // Default: allow
    }
    
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    PyObject *l_net_id = PyLong_FromUnsignedLongLong(a_net_id.uint64);
    PyObject *l_pkey_hash = a_signer_pkey_hash ? PyBytes_FromStringAndSize((const char*)a_signer_pkey_hash, sizeof(dap_hash_fast_t)) : Py_None;
    PyObject *l_tax_addr = a_tax_addr ? PyCapsule_New(a_tax_addr, "dap_chain_addr_t", NULL) : Py_None;
    PyObject *l_tax_value = a_tax_value ? PyCapsule_New(a_tax_value, "uint256_t", NULL) : Py_None;
    
    if (!l_net_id || !l_pkey_hash || !l_tax_addr || !l_tax_value) {
        log_it(L_ERROR, "Failed to create Python objects for tax callback");
        Py_XDECREF(l_net_id);
        Py_XDECREF(l_pkey_hash);
        Py_XDECREF(l_tax_addr);
        Py_XDECREF(l_tax_value);
        PyGILState_Release(l_gstate);
        return true;
    }
    
    PyObject *l_result = PyObject_CallFunctionObjArgs(l_ctx->callback, l_net_id, l_pkey_hash, l_tax_addr, l_tax_value, l_ctx->user_data, NULL);
    
    bool l_ret = true;
    if (!l_result) {
        log_it(L_ERROR, "Python tax callback raised an exception");
        PyErr_Print();
    } else {
        l_ret = PyObject_IsTrue(l_result);
        Py_DECREF(l_result);
    }
    
    Py_DECREF(l_net_id);
    Py_XDECREF(l_pkey_hash);
    Py_XDECREF(l_tax_addr);
    Py_XDECREF(l_tax_value);
    
    PyGILState_Release(l_gstate);
    return l_ret;
}


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
    
    l_data->callback = l_callback;
    l_data->ledger_capsule = l_ledger_obj;
    
    // Register C callback
    dap_ledger_tx_add_notify(l_ledger, py_ledger_tx_add_notify_callback, l_data);
    
    // Register in global registry for proper cleanup (prevents memory leak)
    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_LEDGER_TX, l_callback, l_ledger_obj, 
                                   l_data, NULL) != 0) {
        log_it(L_WARNING, "Failed to register callback in global registry (potential leak!)");
    }
    
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
    
    l_data->callback = l_callback;
    l_data->ledger_capsule = l_ledger_obj;
    
    // Register C callback
    dap_ledger_event_notify_add(l_ledger, py_ledger_event_notify_callback, l_data);
    
    // Register in global registry for proper cleanup (prevents memory leak)
    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_LEDGER_EVENT, l_callback, l_ledger_obj, 
                                   l_data, NULL) != 0) {
        log_it(L_WARNING, "Failed to register callback in global registry (potential leak!)");
    }
    
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
    
    l_data->callback = l_callback;
    l_data->ledger_capsule = l_ledger_obj;
    
    // Register C callback
    dap_ledger_bridged_tx_notify_add(l_ledger, py_ledger_bridged_tx_notify_callback, l_data);
    
    // Register in global registry for proper cleanup (prevents memory leak)
    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_LEDGER_BRIDGED_TX, l_callback, l_ledger_obj, 
                                   l_data, NULL) != 0) {
        log_it(L_WARNING, "Failed to register callback in global registry (potential leak!)");
    }
    
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
 * @brief Add service with callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (service_uid uint64, tag_str string, callback, optional user_data)
 * @return Integer result code
 */
PyObject* dap_ledger_service_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_uid;
    const char *l_tag_str;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "KsO|O", &l_uid, &l_tag_str, &l_callback, &l_user_data)) {
        PyErr_SetString(PyExc_TypeError, "Expected (uid, tag_str, callback, [user_data])");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Third argument must be callable");
        return NULL;
    }
    
    dap_chain_srv_uid_t l_srv_uid = { .uint64 = l_uid };
    
    // Register in global registry
    if (cf_ledger_service_callback_register(l_srv_uid, l_tag_str, l_callback, l_user_data) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register service callback");
        return NULL;
    }
    
    // Register with SDK
    int l_result = dap_ledger_service_add(l_srv_uid, (char*)l_tag_str, s_ledger_service_tag_check_wrapper);
    
    log_it(L_DEBUG, "Registered service callback: UID=0x%llx, tag='%s', result=%d", l_uid, l_tag_str, l_result);
    
    return PyLong_FromLong(l_result);
}

/**
 * @brief Set global tax callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (callback, optional user_data)
 * @return Integer result code
 */
PyObject* dap_ledger_tax_callback_set_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "O|O", &l_callback, &l_user_data)) {
        PyErr_SetString(PyExc_TypeError, "Expected (callback, [user_data])");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be callable");
        return NULL;
    }
    
    // Register in global registry
    if (cf_ledger_tax_callback_register(l_callback, l_user_data) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register tax callback");
        return NULL;
    }
    
    // Register with SDK
    int l_result = dap_ledger_tax_callback_set(s_ledger_tax_callback_wrapper);
    
    log_it(L_DEBUG, "Registered global tax callback, result=%d", l_result);
    
    return PyLong_FromLong(l_result);
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
