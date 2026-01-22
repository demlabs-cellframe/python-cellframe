/*
 * Authors:
 * Dmitriy Gerasimov <naeper@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * Copyright  (c) 2025
 * All rights reserved.

Token Manager - Python Bindings

Thin wrappers around ledger token API:
- dap_ledger_token_ticker_check() - find token by ticker
- dap_ledger_token_info() - get all tokens
- dap_ledger_token_info_by_name() - get token info
- dap_ledger_token_emission_find() - find emission
*/

#include "include/cf_ledger_internal.h"
#include "dap_chain_datum_token.h"
#include "dap_chain_ledger.h"
#include "dap_json.h"

#define LOG_TAG "cf_token_manager"

// ============================================================================
// Token Manager Functions
// ============================================================================

/*
 * Get token by ticker
 * 
 * Args:
 *   ledger (capsule): dap_ledger_t capsule
 *   ticker (str): Token ticker
 * 
 * Returns:
 *   Token datum capsule or None
 */
static PyObject* py_ledger_token_get(PyObject *self, PyObject *args) {
    PyObject *ledger_capsule;
    const char *ticker;
    
    if (!PyArg_ParseTuple(args, "Os:ledger_token_get", &ledger_capsule, &ticker)) {
        return NULL;
    }
    
    dap_ledger_t *ledger = PyCapsule_GetPointer(ledger_capsule, "dap_ledger_t");
    if (!ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_datum_token_t *token = dap_ledger_token_ticker_check(ledger, ticker);
    if (!token) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(token, "dap_chain_datum_token_t", NULL);
}

/*
 * Get all tokens info as JSON
 * 
 * Args:
 *   ledger (capsule): dap_ledger_t capsule
 *   limit (int): Max tokens to return (0 = all)
 *   offset (int): Offset for pagination
 *   version (int): API version
 * 
 * Returns:
 *   dap_json_t capsule or None
 */
static PyObject* py_ledger_token_get_all(PyObject *self, PyObject *args) {
    PyObject *ledger_capsule;
    size_t limit = 0, offset = 0;
    int version = 0;
    
    if (!PyArg_ParseTuple(args, "O|nni:ledger_token_get_all", 
                          &ledger_capsule, &limit, &offset, &version)) {
        return NULL;
    }
    
    dap_ledger_t *ledger = PyCapsule_GetPointer(ledger_capsule, "dap_ledger_t");
    if (!ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_json_t *json = dap_ledger_token_info(ledger, limit, offset, version);
    if (!json) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(json, "dap_json_t", NULL);
}

/*
 * Get token info by name as JSON
 * 
 * Args:
 *   ledger (capsule): dap_ledger_t capsule
 *   ticker (str): Token ticker
 *   version (int): API version
 * 
 * Returns:
 *   dap_json_t capsule or None
 */
static PyObject* py_ledger_token_get_info(PyObject *self, PyObject *args) {
    PyObject *ledger_capsule;
    const char *ticker;
    int version = 0;
    
    if (!PyArg_ParseTuple(args, "Os|i:ledger_token_get_info", 
                          &ledger_capsule, &ticker, &version)) {
        return NULL;
    }
    
    dap_ledger_t *ledger = PyCapsule_GetPointer(ledger_capsule, "dap_ledger_t");
    if (!ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_json_t *json = dap_ledger_token_info_by_name(ledger, ticker, version);
    if (!json) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(json, "dap_json_t", NULL);
}

/*
 * Get token emission by hash
 * 
 * Args:
 *   ledger (capsule): dap_ledger_t capsule
 *   emission_hash (bytes): 32-byte emission hash
 * 
 * Returns:
 *   Token emission capsule or None
 */
static PyObject* py_ledger_token_emission_get(PyObject *self, PyObject *args) {
    PyObject *ledger_capsule;
    const char *hash_bytes;
    Py_ssize_t hash_size;
    
    if (!PyArg_ParseTuple(args, "Os#:ledger_token_emission_get", 
                          &ledger_capsule, &hash_bytes, &hash_size)) {
        return NULL;
    }
    
    if (hash_size != sizeof(dap_chain_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Invalid hash size: %zd (expected %zu)", 
                     hash_size, sizeof(dap_chain_hash_fast_t));
        return NULL;
    }
    
    dap_ledger_t *ledger = PyCapsule_GetPointer(ledger_capsule, "dap_ledger_t");
    if (!ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }
    
    dap_chain_hash_fast_t *hash = (dap_chain_hash_fast_t *)hash_bytes;
    dap_chain_datum_token_emission_t *emission = dap_ledger_token_emission_find(ledger, hash);
    
    if (!emission) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(emission, "dap_chain_datum_token_emission_t", NULL);
}

/*
 * Get token ticker from token datum
 * 
 * Args:
 *   token (capsule): dap_chain_datum_token_t capsule
 * 
 * Returns:
 *   str: Token ticker
 */
static PyObject* py_token_get_ticker(PyObject *self, PyObject *args) {
    PyObject *token_capsule;
    
    if (!PyArg_ParseTuple(args, "O:token_get_ticker", &token_capsule)) {
        return NULL;
    }
    
    dap_chain_datum_token_t *token = PyCapsule_GetPointer(token_capsule, "dap_chain_datum_token_t");
    if (!token) {
        PyErr_SetString(PyExc_ValueError, "Invalid token capsule");
        return NULL;
    }
    
    return PyUnicode_FromString(token->ticker);
}

/*
 * Get token type from token datum
 * 
 * Args:
 *   token (capsule): dap_chain_datum_token_t capsule
 * 
 * Returns:
 *   int: Token type
 */
static PyObject* py_token_get_type(PyObject *self, PyObject *args) {
    PyObject *token_capsule;
    
    if (!PyArg_ParseTuple(args, "O:token_get_type", &token_capsule)) {
        return NULL;
    }
    
    dap_chain_datum_token_t *token = PyCapsule_GetPointer(token_capsule, "dap_chain_datum_token_t");
    if (!token) {
        PyErr_SetString(PyExc_ValueError, "Invalid token capsule");
        return NULL;
    }
    
    return PyLong_FromLong((long)token->type);
}

// ============================================================================
// Module Methods
// ============================================================================

static PyMethodDef token_manager_methods[] = {
    {"ledger_token_get", py_ledger_token_get, METH_VARARGS,
     "Get token by ticker: ledger_token_get(ledger, ticker)"},
    {"ledger_token_get_all", py_ledger_token_get_all, METH_VARARGS,
     "Get all tokens as JSON: ledger_token_get_all(ledger, limit=0, offset=0, version=0)"},
    {"ledger_token_get_info", py_ledger_token_get_info, METH_VARARGS,
     "Get token info as JSON: ledger_token_get_info(ledger, ticker, version=0)"},
    {"ledger_token_emission_get", py_ledger_token_emission_get, METH_VARARGS,
     "Get token emission by hash: ledger_token_emission_get(ledger, emission_hash)"},
    {"token_get_ticker", py_token_get_ticker, METH_VARARGS,
     "Get token ticker: token_get_ticker(token)"},
    {"token_get_type", py_token_get_type, METH_VARARGS,
     "Get token type: token_get_type(token)"},
    {NULL, NULL, 0, NULL}
};

// ============================================================================
// Module Initialization
// ============================================================================

int cellframe_token_manager_init(PyObject *module) {
    // Add token manager functions to module
    for (PyMethodDef *method = token_manager_methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
        if (!func) {
            return -1;
        }
        if (PyModule_AddObject(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            return -1;
        }
    }
    
    // Add token type constants (v6.0 unified API)
    PyModule_AddIntConstant(module, "TOKEN_TYPE_DECL", DAP_CHAIN_DATUM_TOKEN_TYPE_DECL);
    PyModule_AddIntConstant(module, "TOKEN_TYPE_UPDATE", DAP_CHAIN_DATUM_TOKEN_TYPE_UPDATE);
    
    // Token subtypes
    PyModule_AddIntConstant(module, "TOKEN_SUBTYPE_SIMPLE", DAP_CHAIN_DATUM_TOKEN_SUBTYPE_SIMPLE);
    PyModule_AddIntConstant(module, "TOKEN_SUBTYPE_PRIVATE", DAP_CHAIN_DATUM_TOKEN_SUBTYPE_PRIVATE);
    PyModule_AddIntConstant(module, "TOKEN_SUBTYPE_NATIVE", DAP_CHAIN_DATUM_TOKEN_SUBTYPE_NATIVE);
    PyModule_AddIntConstant(module, "TOKEN_SUBTYPE_PUBLIC", DAP_CHAIN_DATUM_TOKEN_SUBTYPE_PUBLIC);
    
    return 0;
}
