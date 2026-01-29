#include "include/cf_ledger_internal.h"
#include "cf_ledger_callback_registry.h"

/*
 * Cellframe ledger anchor bindings
 * Anchor operations: verify, load, unload, find
 */

// =============================================================================
// ANCHOR CALLBACKS
// =============================================================================

static int s_ledger_anchor_unload_callback_wrapper(dap_ledger_t *a_ledger, dap_chain_datum_decree_t *a_decree,
                                                   dap_chain_id_t a_chain_id, dap_hash_fast_t *a_anchor_hash) {
    if (!a_decree) {
        log_it(L_WARNING, "Anchor unload callback without decree");
        return 0;
    }

    uint16_t l_type = a_decree->header.type;
    uint16_t l_subtype = a_decree->header.sub_type;
    python_ledger_simple_ctx_t *l_ctx = cf_ledger_anchor_unload_callback_get(l_type, l_subtype);
    if (!l_ctx || !l_ctx->callback) {
        return 0;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *l_decree = PyCapsule_New(a_decree, "dap_chain_datum_decree_t", NULL);
    PyObject *l_chain_id = PyLong_FromUnsignedLongLong(a_chain_id.uint64);
    PyObject *l_anchor_hash = a_anchor_hash ?
        PyBytes_FromStringAndSize((const char *)a_anchor_hash, sizeof(dap_hash_fast_t)) :
        Py_None;
    PyObject *l_user_data = l_ctx->user_data ? l_ctx->user_data : Py_None;

    if (l_anchor_hash == Py_None) {
        Py_INCREF(Py_None);
    }

    int l_ret = 0;
    if (!l_ledger || !l_decree || !l_chain_id || !l_anchor_hash) {
        log_it(L_ERROR, "Failed to create Python objects for anchor unload callback");
    } else {
        PyObject *l_result = PyObject_CallFunctionObjArgs(
            l_ctx->callback, l_ledger, l_decree, l_chain_id, l_anchor_hash, l_user_data, NULL
        );
        if (!l_result) {
            log_it(L_ERROR, "Python anchor unload callback raised an exception");
            PyErr_Print();
        } else {
            l_ret = PyLong_AsLong(l_result);
            Py_DECREF(l_result);
        }
    }

    Py_XDECREF(l_ledger);
    Py_XDECREF(l_decree);
    Py_XDECREF(l_chain_id);
    Py_XDECREF(l_anchor_hash);

    PyGILState_Release(l_gstate);
    return l_ret;
}

// =============================================================================
// ANCHOR OPERATIONS
// =============================================================================

/**
 * @brief Verify anchor
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule, anchor capsule, data_size)
 * @return Integer result code
 */
PyObject* dap_ledger_anchor_verify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj, *l_anchor_obj;
    Py_ssize_t l_data_size;
    
    if (!PyArg_ParseTuple(a_args, "OOn", &l_net_obj, &l_anchor_obj, &l_data_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain_net capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_anchor_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be an anchor capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    dap_chain_datum_anchor_t *l_anchor = (dap_chain_datum_anchor_t *)PyCapsule_GetPointer(l_anchor_obj, "dap_chain_datum_anchor_t");
    
    if (!l_net || !l_anchor) {
        PyErr_SetString(PyExc_ValueError, "Invalid net or anchor capsule");
        return NULL;
    }
    
    // API changed: now requires ledger instead of net
    if (!l_net->pub.ledger) {
        PyErr_SetString(PyExc_RuntimeError, "Network has no ledger");
        return NULL;
    }
    
    int l_result = dap_ledger_anchor_verify(l_net->pub.ledger, l_anchor, (size_t)l_data_size);
    
    log_it(L_DEBUG, "Anchor verify result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Load anchor to chain
 * @param a_self Python self object (unused)
 * @param a_args Arguments (anchor capsule, chain capsule, anchor_hash bytes)
 * @return Integer result code
 */
PyObject* dap_ledger_anchor_load_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_anchor_obj, *l_chain_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "OOs#", &l_anchor_obj, &l_chain_obj, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_anchor_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an anchor capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a chain capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_chain_datum_anchor_t *l_anchor = (dap_chain_datum_anchor_t *)PyCapsule_GetPointer(l_anchor_obj, "dap_chain_datum_anchor_t");
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_hash_bytes;
    
    if (!l_anchor || !l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid anchor or chain capsule");
        return NULL;
    }
    
    // API changed: get ledger from chain's network
    dap_chain_net_t *l_net = dap_chain_net_by_id(l_chain->net_id);
    if (!l_net || !l_net->pub.ledger) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get ledger from chain");
        return NULL;
    }
    
    // Get chain_id from chain
    dap_chain_id_t l_chain_id = l_chain->id;
    
    int l_result = dap_ledger_anchor_load(l_net->pub.ledger, l_anchor, l_chain_id, l_hash);
    
    log_it(L_DEBUG, "Anchor load result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Unload anchor from chain
 * @param a_self Python self object (unused)
 * @param a_args Arguments (anchor capsule, chain capsule, anchor_hash bytes)
 * @return Integer result code
 */
PyObject* dap_ledger_anchor_unload_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_anchor_obj, *l_chain_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "OOs#", &l_anchor_obj, &l_chain_obj, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_anchor_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be an anchor capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a chain capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_chain_datum_anchor_t *l_anchor = (dap_chain_datum_anchor_t *)PyCapsule_GetPointer(l_anchor_obj, "dap_chain_datum_anchor_t");
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_hash_bytes;
    
    if (!l_anchor || !l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid anchor or chain capsule");
        return NULL;
    }
    
    // API changed: get ledger from chain's network
    dap_chain_net_t *l_net = dap_chain_net_by_id(l_chain->net_id);
    if (!l_net || !l_net->pub.ledger) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get ledger from chain");
        return NULL;
    }
    
    // Get chain_id from chain
    dap_chain_id_t l_chain_id = l_chain->id;
    
    int l_result = dap_ledger_anchor_unload(l_net->pub.ledger, l_anchor, l_chain_id, l_hash);
    
    log_it(L_DEBUG, "Anchor unload result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Find anchor by hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger capsule, anchor_hash bytes)
 * @return PyCapsule wrapping dap_chain_datum_anchor_t* or None
 */
PyObject* dap_ledger_anchor_find_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_ledger_obj, &l_hash_bytes, &l_hash_size)) {
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
    
    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_hash_bytes;
    
    dap_chain_datum_anchor_t *l_anchor = dap_ledger_anchor_find(l_ledger, l_hash);
    if (!l_anchor) {
        log_it(L_DEBUG, "Anchor not found");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Anchor found");
    return PyCapsule_New(l_anchor, "dap_chain_datum_anchor_t", NULL);
}

/**
 * @brief Register anchor unload callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (type, subtype, callback, optional user_data)
 * @return None
 */
PyObject* dap_ledger_anchor_unload_set_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned short l_type = 0;
    unsigned short l_subtype = 0;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "HHO|O", &l_type, &l_subtype, &l_callback, &l_user_data)) {
        PyErr_SetString(PyExc_TypeError, "Expected (type, subtype, callback, [user_data])");
        return NULL;
    }

    if (l_callback == Py_None) {
        cf_ledger_anchor_unload_callback_unregister(l_type, l_subtype);
        Py_RETURN_NONE;
    }

    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "callback must be callable");
        return NULL;
    }

    PyObject *l_user_real = (l_user_data == Py_None) ? NULL : l_user_data;
    int l_reg = cf_ledger_anchor_unload_callback_register(l_type, l_subtype, l_callback, l_user_real);
    if (l_reg < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to register anchor unload callback");
        return NULL;
    }

    if (l_reg == 1) {
        dap_ledger_anchor_unload_set_callback(l_type, l_subtype, s_ledger_anchor_unload_callback_wrapper);
    }

    log_it(L_DEBUG, "Registered anchor unload callback for type=%hu subtype=%hu", l_type, l_subtype);
    Py_RETURN_NONE;
}

// Get method definitions for anchor module
PyMethodDef* cellframe_ledger_anchor_get_methods(void) {
    static PyMethodDef anchor_methods[] = {
        {"ledger_anchor_verify", (PyCFunction)dap_ledger_anchor_verify_py, METH_VARARGS,
         "Verify anchor"},
        {"ledger_anchor_load", (PyCFunction)dap_ledger_anchor_load_py, METH_VARARGS,
         "Load anchor to chain"},
        {"ledger_anchor_unload", (PyCFunction)dap_ledger_anchor_unload_py, METH_VARARGS,
         "Unload anchor from chain"},
        {"ledger_anchor_find", (PyCFunction)dap_ledger_anchor_find_py, METH_VARARGS,
         "Find anchor by hash"},
        {"ledger_anchor_unload_set_callback", (PyCFunction)dap_ledger_anchor_unload_set_callback_py, METH_VARARGS,
         "Register anchor unload callback"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return anchor_methods;
}
