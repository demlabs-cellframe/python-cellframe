#ifndef CELLFRAME_LEDGER_H
#define CELLFRAME_LEDGER_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

// =========================================
// CELLFRAME LEDGER FUNCTION DECLARATIONS
// User-facing functions for balance calculations and transaction queries
// =========================================

// Lifecycle operations
PyObject* dap_ledger_new_py(PyObject *self, PyObject *args);
PyObject* dap_ledger_open_py(PyObject *self, PyObject *args);
PyObject* dap_ledger_close_py(PyObject *self, PyObject *args);

// Balance calculations 
PyObject* py_dap_ledger_calc_balance(PyObject *self, PyObject *args);
PyObject* py_dap_ledger_calc_balance_full(PyObject *self, PyObject *args);

// Transaction search
PyObject* py_dap_ledger_tx_find_by_hash(PyObject *self, PyObject *args);

// NOTE: Only user-facing functions for balance and transaction queries
// Functions will FAIL-FAST with clear error messages if not available

// Module initialization function
int cellframe_ledger_init(PyObject *module);

#ifdef __cplusplus
}
#endif

#endif /* CELLFRAME_LEDGER_H */
