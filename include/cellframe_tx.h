/*
 * Authors:
 * Cellframe      https://cellframe.net
 * DeM Labs Inc.  https://demlabs.net
 * Sources:       https://gitlab.demlabs.net/cellframe/python-cellframe
 */

#pragma once

#include <Python.h>

// ===========================================
// CELLFRAME TX FUNCTION DECLARATIONS
// Functions for transaction creation and management
// ===========================================

// Transaction creation and management
PyObject* py_dap_chain_datum_tx_create(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_delete(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_get_size(PyObject *self, PyObject *args);

// Transaction items manipulation
PyObject* py_dap_chain_datum_tx_add_in_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_out_ext_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_sign_item(PyObject *self, PyObject *args);

// Transaction verification
PyObject* py_dap_chain_datum_tx_verify_sign(PyObject *self, PyObject *args);

// Mempool operations
PyObject* py_dap_chain_mempool_datum_add(PyObject *self, PyObject *args);

// Module initialization function
int cellframe_tx_init(PyObject *module);


