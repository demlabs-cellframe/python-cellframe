#include "include/cf_ledger_internal.h"

/*
 * Cellframe ledger decree bindings
 * Decree operations: init, apply, verify, load, get by hash
 */


// =============================================================================
// DECREE OPERATIONS
// =============================================================================

/**
 * @brief Initialize decree subsystem for ledger
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return None
 */
PyObject* dap_ledger_decree_init_py(PyObject *a_self, PyObject *a_args) {
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
    
    dap_ledger_decree_init(l_ledger);
    
    log_it(L_DEBUG, "Decree subsystem initialized");
    Py_RETURN_NONE;
}

/**
 * @brief Get minimum number of signers for decree
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return Integer - minimum number of signers
 */
PyObject* dap_ledger_decree_get_min_num_of_signers_py(PyObject *a_self, PyObject *a_args) {
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
    
    uint16_t l_result = dap_ledger_decree_get_min_num_of_signers(l_ledger);
    
    log_it(L_DEBUG, "Decree min signers: %u", l_result);
    return PyLong_FromUnsignedLong(l_result);
}

/**
 * @brief Get number of decree owners
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return Integer - number of owners
 */
PyObject* dap_ledger_decree_get_num_of_owners_py(PyObject *a_self, PyObject *a_args) {
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
    
    uint16_t l_result = dap_ledger_decree_get_num_of_owners(l_ledger);
    
    log_it(L_DEBUG, "Decree owners count: %u", l_result);
    return PyLong_FromUnsignedLong(l_result);
}

/**
 * @brief Get decree owners public keys
 * @param a_self Python self object (unused)
 * @param a_args Arguments (ledger)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_ledger_decree_get_owners_pkeys_py(PyObject *a_self, PyObject *a_args) {
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
    
    const dap_list_t *l_list = dap_ledger_decree_get_owners_pkeys(l_ledger);
    if (!l_list) {
        log_it(L_DEBUG, "No decree owners pkeys found");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved decree owners pkeys");
    // Note: const cast - Python will not modify, only read
    return PyCapsule_New((void *)l_list, "dap_list_t", NULL);
}

/**
 * @brief Apply decree to chain
 * @param a_self Python self object (unused)
 * @param a_args Arguments (decree_hash bytes, decree capsule, chain capsule, anchor_hash bytes)
 * @return Integer result code
 */
PyObject* dap_ledger_decree_apply_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_decree_hash_bytes, *l_anchor_hash_bytes;
    Py_ssize_t l_decree_hash_size, l_anchor_hash_size;
    PyObject *l_decree_obj, *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "s#OOs#", &l_decree_hash_bytes, &l_decree_hash_size,
                          &l_decree_obj, &l_chain_obj, &l_anchor_hash_bytes, &l_anchor_hash_size)) {
        return NULL;
    }
    
    if ((size_t)l_decree_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Decree hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    if ((size_t)l_anchor_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Anchor hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_decree_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a decree capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "Third argument must be a chain capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_decree_hash = (dap_hash_fast_t *)l_decree_hash_bytes;
    dap_hash_fast_t *l_anchor_hash = (dap_hash_fast_t *)l_anchor_hash_bytes;
    dap_chain_datum_decree_t *l_decree = (dap_chain_datum_decree_t *)PyCapsule_GetPointer(l_decree_obj, "dap_chain_datum_decree_t");
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    
    if (!l_decree || !l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid decree or chain capsule");
        return NULL;
    }
    
    int l_result = dap_ledger_decree_apply(l_decree_hash, l_decree, l_chain, l_anchor_hash);
    
    log_it(L_DEBUG, "Decree apply result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Verify decree
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule, decree capsule, data_size, decree_hash bytes)
 * @return Integer result code
 */
PyObject* dap_ledger_decree_verify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj, *l_decree_obj;
    Py_ssize_t l_data_size;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "OOns#", &l_net_obj, &l_decree_obj, &l_data_size, 
                          &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain_net capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_decree_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a decree capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_chain_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    dap_chain_datum_decree_t *l_decree = (dap_chain_datum_decree_t *)PyCapsule_GetPointer(l_decree_obj, "dap_chain_datum_decree_t");
    dap_chain_hash_fast_t *l_hash = (dap_chain_hash_fast_t *)l_hash_bytes;
    
    if (!l_net || !l_decree) {
        PyErr_SetString(PyExc_ValueError, "Invalid net or decree capsule");
        return NULL;
    }
    
    int l_result = dap_ledger_decree_verify(l_net, l_decree, (size_t)l_data_size, l_hash);
    
    log_it(L_DEBUG, "Decree verify result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Load decree to chain
 * @param a_self Python self object (unused)
 * @param a_args Arguments (decree capsule, chain capsule, decree_hash bytes)
 * @return Integer result code
 */
PyObject* dap_ledger_decree_load_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_decree_obj, *l_chain_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "OOs#", &l_decree_obj, &l_chain_obj, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_decree_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a decree capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a chain capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_chain_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
        return NULL;
    }
    
    dap_chain_datum_decree_t *l_decree = (dap_chain_datum_decree_t *)PyCapsule_GetPointer(l_decree_obj, "dap_chain_datum_decree_t");
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    dap_chain_hash_fast_t *l_hash = (dap_chain_hash_fast_t *)l_hash_bytes;
    
    if (!l_decree || !l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid decree or chain capsule");
        return NULL;
    }
    
    int l_result = dap_ledger_decree_load(l_decree, l_chain, l_hash);
    
    log_it(L_DEBUG, "Decree load result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Get decree by hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule, hash bytes)
 * @return Tuple (decree capsule, is_applied bool) or None
 */
PyObject* dap_ledger_decree_get_by_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_net_obj, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain_net capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain_net capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_hash_bytes;
    bool l_is_applied = false;
    
    dap_chain_datum_decree_t *l_decree = dap_ledger_decree_get_by_hash(l_net, l_hash, &l_is_applied);
    if (!l_decree) {
        log_it(L_DEBUG, "Decree not found");
        Py_RETURN_NONE;
    }
    
    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyCapsule_New(l_decree, "dap_chain_datum_decree_t", NULL));
    PyTuple_SetItem(l_tuple, 1, PyBool_FromLong(l_is_applied));
    
    log_it(L_DEBUG, "Retrieved decree (applied: %s)", l_is_applied ? "true" : "false");
    return l_tuple;
}

/**
 * @brief Reset applied status for decree
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule, decree_hash bytes)
 * @return Integer result code
 */
PyObject* dap_ledger_decree_reset_applied_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj;
    const char *l_hash_bytes;
    Py_ssize_t l_hash_size;
    
    if (!PyArg_ParseTuple(a_args, "Os#", &l_net_obj, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain_net capsule");
        return NULL;
    }
    
    if ((size_t)l_hash_size != sizeof(dap_chain_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain_net capsule");
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_hash = (dap_chain_hash_fast_t *)l_hash_bytes;
    
    int l_result = dap_ledger_decree_reset_applied(l_net, l_hash);
    
    log_it(L_DEBUG, "Decree reset applied result: %d", l_result);
    return PyLong_FromLong(l_result);
}


// Get method definitions for decree module
PyMethodDef* cellframe_ledger_decree_get_methods(void) {
    static PyMethodDef decree_methods[] = {
        {"ledger_decree_init", (PyCFunction)dap_ledger_decree_init_py, METH_VARARGS,
         "Initialize decree subsystem for ledger"},
        {"ledger_decree_get_min_num_of_signers", (PyCFunction)dap_ledger_decree_get_min_num_of_signers_py, METH_VARARGS,
         "Get minimum number of signers for decree"},
        {"ledger_decree_get_num_of_owners", (PyCFunction)dap_ledger_decree_get_num_of_owners_py, METH_VARARGS,
         "Get number of decree owners"},
        {"ledger_decree_get_owners_pkeys", (PyCFunction)dap_ledger_decree_get_owners_pkeys_py, METH_VARARGS,
         "Get decree owners public keys"},
        {"ledger_decree_apply", (PyCFunction)dap_ledger_decree_apply_py, METH_VARARGS,
         "Apply decree to chain"},
        {"ledger_decree_verify", (PyCFunction)dap_ledger_decree_verify_py, METH_VARARGS,
         "Verify decree"},
        {"ledger_decree_load", (PyCFunction)dap_ledger_decree_load_py, METH_VARARGS,
         "Load decree to chain"},
        {"ledger_decree_get_by_hash", (PyCFunction)dap_ledger_decree_get_by_hash_py, METH_VARARGS,
         "Get decree by hash"},
        {"ledger_decree_reset_applied", (PyCFunction)dap_ledger_decree_reset_applied_py, METH_VARARGS,
         "Reset applied status for decree"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return decree_methods;
}
