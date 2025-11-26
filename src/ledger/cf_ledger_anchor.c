#include "include/cf_ledger_internal.h"

/*
 * Cellframe ledger anchor bindings
 * Anchor operations: verify, load, unload, find
 */


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
    
    int l_result = dap_ledger_anchor_verify(l_net, l_anchor, (size_t)l_data_size);
    
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
    
    int l_result = dap_ledger_anchor_load(l_anchor, l_chain, l_hash);
    
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
    
    int l_result = dap_ledger_anchor_unload(l_anchor, l_chain, l_hash);
    
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
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return anchor_methods;
}
