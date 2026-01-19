#include "cf_chain_internal.h"
#include "dap_chain_block.h"
#include "dap_enc_key.h"

#define LOG_TAG "python_cellframe_chain"

// =========================================
// BLOCK OPERATIONS
// =========================================

/**
 * @brief Create new block (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (prev_block_hash bytes or None)
 * @return PyCapsule wrapping dap_chain_block_t* or None on error
 */
PyObject* py_dap_chain_block_new(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_prev_hash_obj = NULL;
    const char *l_prev_hash_bytes = NULL;
    Py_ssize_t l_prev_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "|O", &l_prev_hash_obj)) {
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_prev_hash = NULL;
    if (l_prev_hash_obj && l_prev_hash_obj != Py_None) {
        if (!PyArg_Parse(l_prev_hash_obj, "y#", &l_prev_hash_bytes, &l_prev_hash_size)) {
            PyErr_SetString(PyExc_TypeError, "prev_hash must be bytes or None");
            return NULL;
        }
        
        if (l_prev_hash_size != sizeof(dap_chain_hash_fast_t)) {
            PyErr_Format(PyExc_ValueError, "prev_hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
            return NULL;
        }
        
        l_prev_hash = (dap_chain_hash_fast_t*)l_prev_hash_bytes;
    }
    
    size_t l_block_size = 0;
    dap_chain_block_t *l_block = dap_chain_block_new(l_prev_hash, &l_block_size);
    if (!l_block) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create block");
        return NULL;
    }
    
    log_it(L_DEBUG, "Created new block (size=%zu)", l_block_size);
    return PyCapsule_New(l_block, "dap_chain_block_t", NULL);
}

/**
 * @brief Add datum to block (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (block capsule, datum capsule)
 * @return New block size or None on error
 */

PyObject* py_dap_chain_block_datum_add(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_block_obj = NULL;
    PyObject *l_datum_obj = NULL;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_block_obj, &l_datum_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_block_obj) || !PyCapsule_CheckExact(l_datum_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected block and datum capsules");
        return NULL;
    }
    
    dap_chain_block_t *l_block = (dap_chain_block_t*)PyCapsule_GetPointer(l_block_obj, "dap_chain_block_t");
    dap_chain_datum_t *l_datum = (dap_chain_datum_t*)PyCapsule_GetPointer(l_datum_obj, "dap_chain_datum_t");
    
    if (!l_block || !l_datum) {
        PyErr_SetString(PyExc_ValueError, "Invalid block or datum capsule");
        return NULL;
    }
    
    size_t l_block_size = dap_chain_block_get_size(l_block);
    uint64_t l_datum_size = dap_chain_datum_size(l_datum);
    
    size_t l_new_size = dap_chain_block_datum_add(&l_block, l_block_size, l_datum, (size_t)l_datum_size);
    if (l_new_size == 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add datum to block");
        return NULL;
    }
    
    log_it(L_DEBUG, "Added datum to block (new_size=%zu)", l_new_size);
    return PyLong_FromSize_t(l_new_size);
}

/**
 * @brief Get datums from block (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (block capsule)
 * @return List of datum capsules or None on error
 */

PyObject* py_dap_chain_block_get_datums(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_block_obj = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_block_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_block_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected block capsule");
        return NULL;
    }
    
    dap_chain_block_t *l_block = (dap_chain_block_t*)PyCapsule_GetPointer(l_block_obj, "dap_chain_block_t");
    if (!l_block) {
        PyErr_SetString(PyExc_ValueError, "Invalid block capsule");
        return NULL;
    }
    
    size_t l_block_size = dap_chain_block_get_size(l_block);
    size_t l_datums_count = 0;
    
    dap_chain_datum_t **l_datums = dap_chain_block_get_datums(l_block, l_block_size, &l_datums_count);
    if (!l_datums) {
        log_it(L_DEBUG, "No datums found in block");
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = PyList_New(l_datums_count);
    if (!l_result) {
        DAP_DELETE(l_datums);
        return NULL;
    }
    
    for (size_t i = 0; i < l_datums_count; i++) {
        PyObject *l_datum_capsule = PyCapsule_New(l_datums[i], "dap_chain_datum_t", NULL);
        if (!l_datum_capsule) {
            Py_DECREF(l_result);
            DAP_DELETE(l_datums);
            return NULL;
        }
        PyList_SET_ITEM(l_result, i, l_datum_capsule);
    }
    
    DAP_DELETE(l_datums);
    log_it(L_DEBUG, "Retrieved %zu datums from block", l_datums_count);
    return l_result;
}

/**
 * @brief Add metadata to block (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (block capsule, meta_type, data bytes)
 * @return New block size or None on error
 */

PyObject* py_dap_chain_block_meta_add(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_block_obj = NULL;
    unsigned int l_meta_type = 0;
    const char *l_data_bytes = NULL;
    Py_ssize_t l_data_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "OIy#", &l_block_obj, &l_meta_type, &l_data_bytes, &l_data_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_block_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected block capsule");
        return NULL;
    }
    
    dap_chain_block_t *l_block = (dap_chain_block_t*)PyCapsule_GetPointer(l_block_obj, "dap_chain_block_t");
    if (!l_block) {
        PyErr_SetString(PyExc_ValueError, "Invalid block capsule");
        return NULL;
    }
    
    size_t l_block_size = dap_chain_block_get_size(l_block);
    
    size_t l_new_size = dap_chain_block_meta_add(&l_block, l_block_size, (uint8_t)l_meta_type, l_data_bytes, l_data_size);
    if (l_new_size == 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add metadata to block");
        return NULL;
    }
    
    log_it(L_DEBUG, "Added metadata (type=%u) to block (new_size=%zu)", l_meta_type, l_new_size);
    return PyLong_FromSize_t(l_new_size);
}

/**
 * @brief Add signature to block (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (block capsule, key capsule)
 * @return New block size or None on error
 */

PyObject* py_dap_chain_block_sign_add(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_block_obj = NULL;
    PyObject *l_key_obj = NULL;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_block_obj, &l_key_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_block_obj) || !PyCapsule_CheckExact(l_key_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected block and key capsules");
        return NULL;
    }
    
    dap_chain_block_t *l_block = (dap_chain_block_t*)PyCapsule_GetPointer(l_block_obj, "dap_chain_block_t");
    dap_enc_key_t *l_key = (dap_enc_key_t*)PyCapsule_GetPointer(l_key_obj, "dap_enc_key_t");
    
    if (!l_block || !l_key) {
        PyErr_SetString(PyExc_ValueError, "Invalid block or key capsule");
        return NULL;
    }
    
    size_t l_block_size = dap_chain_block_get_size(l_block);
    
    size_t l_new_size = dap_chain_block_sign_add(&l_block, l_block_size, l_key);
    if (l_new_size == 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add signature to block");
        return NULL;
    }
    
    log_it(L_DEBUG, "Added signature to block (new_size=%zu)", l_new_size);
    return PyLong_FromSize_t(l_new_size);
}

// =========================================
// CHAIN CELL OPERATIONS
// =========================================
