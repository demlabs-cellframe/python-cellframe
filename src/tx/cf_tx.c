/*
 * Authors:
 * Cellframe      https://cellframe.net
 * DeM Labs Inc.  https://demlabs.net
 * Sources:       https://gitlab.demlabs.net/cellframe/python-cellframe
 */

#include "../include/cf_tx.h"
#include "dap_chain_datum_tx.h"
#include "dap_chain_mempool.h"
#include "cellframe.h"

#define LOG_TAG "python_cellframe_tx"

// Create new transaction
PyObject* py_dap_chain_datum_tx_create(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    
    dap_chain_datum_tx_t *tx = dap_chain_datum_tx_create();
    if (!tx) {
        PyErr_SetString(CellframeNetworkError, "Failed to create transaction");
        return NULL;
    }
    
    return PyCapsule_New(tx, "dap_chain_datum_tx_t", NULL);
}

// Add input item to transaction
PyObject* py_dap_chain_datum_tx_add_in_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule;
    PyObject *prev_hash_capsule;
    uint32_t prev_tx_out_idx;
    
    if (!PyArg_ParseTuple(args, "OOI", &tx_capsule, &prev_hash_capsule, &prev_tx_out_idx)) {
        return NULL;
    }
    
    dap_chain_datum_tx_t **tx = (dap_chain_datum_tx_t**)PyCapsule_GetPointer(tx_capsule, "dap_chain_datum_tx_t");
    dap_chain_hash_fast_t *prev_hash = (dap_chain_hash_fast_t*)PyCapsule_GetPointer(prev_hash_capsule, "dap_chain_hash_fast_t");
    
    if (!tx || !prev_hash) {
        PyErr_SetString(CellframeNetworkError, "Invalid transaction or hash object");
        return NULL;
    }
    
    int ret = dap_chain_datum_tx_add_in_item(tx, prev_hash, prev_tx_out_idx);
    
    return PyLong_FromLong(ret);
}

// Add extended output item to transaction  
PyObject* py_dap_chain_datum_tx_add_out_ext_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule;
    PyObject *addr_capsule;
    PyObject *value_obj;
    const char *token_ticker;
    
    if (!PyArg_ParseTuple(args, "OOOs", &tx_capsule, &addr_capsule, &value_obj, &token_ticker)) {
        return NULL;
    }
    
    dap_chain_datum_tx_t **tx = (dap_chain_datum_tx_t**)PyCapsule_GetPointer(tx_capsule, "dap_chain_datum_tx_t");
    dap_chain_addr_t *addr = (dap_chain_addr_t*)PyCapsule_GetPointer(addr_capsule, "dap_chain_addr_t");
    
    if (!tx || !addr) {
        PyErr_SetString(CellframeNetworkError, "Invalid transaction or address object");
        return NULL;
    }
    
    // Convert value from Python to uint256_t
    uint256_t value = {};
    if (PyLong_Check(value_obj)) {
        unsigned long long val = PyLong_AsUnsignedLongLong(value_obj);
        if (PyErr_Occurred()) {
            return NULL;
        }
        value = dap_chain_uint256_from(val);
    } else {
        PyErr_SetString(PyExc_TypeError, "Value must be an integer");
        return NULL;
    }
    
    int ret = dap_chain_datum_tx_add_out_ext_item(tx, addr, value, token_ticker);
    
    return PyLong_FromLong(ret);
}

// Add signature item to transaction
PyObject* py_dap_chain_datum_tx_add_sign_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule;
    PyObject *key_capsule;
    
    if (!PyArg_ParseTuple(args, "OO", &tx_capsule, &key_capsule)) {
        return NULL;
    }
    
    dap_chain_datum_tx_t **tx = (dap_chain_datum_tx_t**)PyCapsule_GetPointer(tx_capsule, "dap_chain_datum_tx_t");
    dap_enc_key_t *key = (dap_enc_key_t*)PyCapsule_GetPointer(key_capsule, "dap_enc_key_t");
    
    if (!tx || !key) {
        PyErr_SetString(CellframeNetworkError, "Invalid transaction or key object");
        return NULL;
    }
    
    int ret = dap_chain_datum_tx_add_sign_item(tx, key);
    
    return PyLong_FromLong(ret);
}

// Verify transaction signature
PyObject* py_dap_chain_datum_tx_verify_sign(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule;
    int sign_num = -1; // Default to verify all signatures
    
    if (!PyArg_ParseTuple(args, "O|i", &tx_capsule, &sign_num)) {
        return NULL;
    }
    
    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t*)PyCapsule_GetPointer(tx_capsule, "dap_chain_datum_tx_t");
    if (!tx) {
        PyErr_SetString(CellframeNetworkError, "Invalid transaction object");
        return NULL;
    }
    
    int ret;
    if (sign_num >= 0) {
        ret = dap_chain_datum_tx_verify_sign(tx, sign_num);
    } else {
        ret = dap_chain_datum_tx_verify_sign_all(tx);
    }
    
    return PyLong_FromLong(ret);
}

// Get transaction size
PyObject* py_dap_chain_datum_tx_get_size(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule;
    
    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }
    
    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t*)PyCapsule_GetPointer(tx_capsule, "dap_chain_datum_tx_t");
    if (!tx) {
        PyErr_SetString(CellframeNetworkError, "Invalid transaction object");
        return NULL;
    }
    
    size_t size = dap_chain_datum_tx_get_size(tx);
    
    return PyLong_FromSize_t(size);
}

// Add transaction to mempool
PyObject* py_dap_chain_mempool_datum_add(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *datum_capsule;
    PyObject *chain_capsule;
    const char *hash_out_type = NULL;
    
    if (!PyArg_ParseTuple(args, "OO|s", &datum_capsule, &chain_capsule, &hash_out_type)) {
        return NULL;
    }
    
    dap_chain_datum_t *datum = (dap_chain_datum_t*)PyCapsule_GetPointer(datum_capsule, "dap_chain_datum_t");
    dap_chain_t *chain = (dap_chain_t*)PyCapsule_GetPointer(chain_capsule, "dap_chain_t");
    
    if (!datum || !chain) {
        PyErr_SetString(CellframeNetworkError, "Invalid datum or chain object");
        return NULL;
    }
    
    char *result = dap_chain_mempool_datum_add(datum, chain, hash_out_type);
    if (!result) {
        PyErr_SetString(CellframeNetworkError, "Failed to add datum to mempool");
        return NULL;
    }
    
    PyObject *py_result = PyUnicode_FromString(result);
    DAP_DELETE(result);
    
    return py_result;
}

// Delete transaction
PyObject* py_dap_chain_datum_tx_delete(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule;
    
    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }
    
    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t*)PyCapsule_GetPointer(tx_capsule, "dap_chain_datum_tx_t");
    if (!tx) {
        PyErr_SetString(CellframeNetworkError, "Invalid transaction object");
        return NULL;
    }
    
    dap_chain_datum_tx_delete(tx);
    
    Py_RETURN_NONE;
}

// Module initialization function
int cellframe_tx_init(PyObject *module) {
    // Define TX method definitions
    static PyMethodDef tx_methods[] = {
        {"dap_chain_datum_tx_create", py_dap_chain_datum_tx_create, METH_NOARGS,
         "Create new transaction"},
        {"dap_chain_datum_tx_add_in_item", py_dap_chain_datum_tx_add_in_item, METH_VARARGS,
         "Add input item to transaction"},
        {"dap_chain_datum_tx_add_out_ext_item", py_dap_chain_datum_tx_add_out_ext_item, METH_VARARGS,
         "Add extended output item to transaction"},
        {"dap_chain_datum_tx_add_sign_item", py_dap_chain_datum_tx_add_sign_item, METH_VARARGS,
         "Add signature item to transaction"},
        {"dap_chain_datum_tx_verify_sign", py_dap_chain_datum_tx_verify_sign, METH_VARARGS,
         "Verify transaction signature(s)"},
        {"dap_chain_datum_tx_get_size", py_dap_chain_datum_tx_get_size, METH_VARARGS,
         "Get transaction size"},
        {"dap_chain_mempool_datum_add", py_dap_chain_mempool_datum_add, METH_VARARGS,
         "Add datum to mempool"},
        {"dap_chain_datum_tx_delete", py_dap_chain_datum_tx_delete, METH_VARARGS,
         "Delete transaction"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    
    // Add methods to module
    for (PyMethodDef *method = tx_methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_NewEx(method, NULL, NULL);
        if (!func) {
            return -1;
        }
        
        if (PyModule_AddObject(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            return -1;
        }
    }
    
    return 0;
}
