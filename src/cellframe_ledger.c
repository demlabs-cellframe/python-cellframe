#include "cellframe.h"

/*
 * Cellframe ledger bindings for user API
 * Contains only user-facing functions for balance calculations and transaction queries
 */



// Balance calculation for address and token
PyObject* py_dap_ledger_calc_balance(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *ledger_capsule;
    const char *addr_str;
    const char *token_ticker;
    
    if (!PyArg_ParseTuple(args, "Oss", &ledger_capsule, &addr_str, &token_ticker)) {
        return NULL;
    }
    
    dap_ledger_t *ledger = (dap_ledger_t*)PyCapsule_GetPointer(ledger_capsule, "dap_ledger_t");
    if (!ledger) {
        PyErr_SetString(CellframeWalletError, "Invalid ledger handle");
        return NULL;
    }
    
    dap_chain_addr_t *addr = dap_chain_addr_from_str(addr_str);
    if (!addr) {
        PyErr_SetString(CellframeWalletError, "Invalid address format");
        return NULL;
    }
    
    DAP_DELETE(addr);
    PyErr_SetString(CellframeWalletError, 
        "FAIL-FAST: dap_ledger_calc_balance - function not found in Cellframe SDK. "
        "Use network-specific balance calculation functions instead.");
    return NULL;
}

// Full balance calculation with detailed info
PyObject* py_dap_ledger_calc_balance_full(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *ledger_capsule;
    const char *addr_str;
    const char *token_ticker;
    
    if (!PyArg_ParseTuple(args, "Oss", &ledger_capsule, &addr_str, &token_ticker)) {
        return NULL;
    }
    
    dap_ledger_t *ledger = (dap_ledger_t*)PyCapsule_GetPointer(ledger_capsule, "dap_ledger_t");
    if (!ledger) {
        PyErr_SetString(CellframeWalletError, "Invalid ledger handle");
        return NULL;
    }
    
    dap_chain_addr_t *addr = dap_chain_addr_from_str(addr_str);
    if (!addr) {
        PyErr_SetString(CellframeWalletError, "Invalid address format");
        return NULL;
    }
    
    DAP_DELETE(addr);
    PyErr_SetString(CellframeWalletError, 
        "FAIL-FAST: dap_ledger_calc_balance_full - function not found in Cellframe SDK. "
        "Use chain-specific balance calculation functions instead.");
    return NULL;
}



// Transaction search by hash
PyObject* py_dap_ledger_tx_find_by_hash(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *ledger_capsule;
    const char *tx_hash_str;
    
    if (!PyArg_ParseTuple(args, "Os", &ledger_capsule, &tx_hash_str)) {
        return NULL;
    }
    
    dap_ledger_t *ledger = (dap_ledger_t*)PyCapsule_GetPointer(ledger_capsule, "dap_ledger_t");
    if (!ledger) {
        PyErr_SetString(CellframeWalletError, "Invalid ledger handle");
        return NULL;
    }
    
    dap_chain_hash_fast_t tx_hash;
    if (dap_chain_hash_fast_from_str(tx_hash_str, &tx_hash) != 0) {
        PyErr_SetString(CellframeWalletError, "Invalid transaction hash format");
        return NULL;
    }
    
    PyErr_SetString(CellframeWalletError, 
        "FAIL-FAST: dap_ledger_tx_find_by_hash - function not found in Cellframe SDK. "
        "Use chain-specific transaction search functions instead.");
    return NULL;
}



// =========================================
// MODULE INITIALIZATION
// =========================================
// This function registers all ledger functions with the Python module
int cellframe_ledger_init(PyObject *module) {
    // Define ledger method definitions - only user-facing functions
    static PyMethodDef ledger_methods[] = {
        // Balance calculations 
        {"dap_ledger_calc_balance", py_dap_ledger_calc_balance, METH_VARARGS,
         "Calculate address balance for token"},
        {"dap_ledger_calc_balance_full", py_dap_ledger_calc_balance_full, METH_VARARGS,
         "Calculate full address balance info"},
        
        // Transaction search
        {"dap_ledger_tx_find_by_hash", py_dap_ledger_tx_find_by_hash, METH_VARARGS,
         "Find transaction by hash"},
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    
    // Add all methods to the module
    for (PyMethodDef *method = ledger_methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
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
