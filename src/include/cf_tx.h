/*
 * Authors:
 * Cellframe      https://cellframe.net
 * DeM Labs Inc.  https://demlabs.net
 * Sources:       https://gitlab.demlabs.net/cellframe/python-cellframe
 */

#pragma once

#include <Python.h>

// Transaction core
PyObject* py_dap_chain_datum_tx_create(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_delete(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_get_size(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_in_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_in_item_list(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_in_cond_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_in_cond_item_list(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_in_reward_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_out_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_fee_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_out_ext_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_out_std_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_out_cond_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_event_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_sign_item(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_add_sign(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_get_sign(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_get_sign_data(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_get_fee_value(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_group_items(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_group_items_free(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_item_get(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_item_get_nth(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_items_get(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_out_cond_get(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_verify_sign(PyObject *self, PyObject *args);
PyObject* py_dap_chain_datum_tx_verify_sign_all(PyObject *self, PyObject *args);
PyObject* py_dap_chain_node_datum_tx_calc_hash(PyObject *self, PyObject *args);

// Transaction creation helpers

// Tx wrapper helpers
PyObject* py_dap_chain_tx_wrap_packed(PyObject *self, PyObject *args);
PyObject* py_dap_chain_tx_dup(PyObject *self, PyObject *args);
PyObject* py_dap_chain_tx_delete(PyObject *self, PyObject *args);
PyObject* py_dap_chain_tx_hh_add(PyObject *self, PyObject *args);
PyObject* py_dap_chain_tx_hh_find(PyObject *self, PyObject *args);
PyObject* py_dap_chain_tx_hh_free(PyObject *self, PyObject *args);
PyObject* py_dap_chain_tx_get_signing_data(PyObject *self, PyObject *args);
PyObject* py_dap_chain_tx_sign_add(PyObject *self, PyObject *args);

// Mempool operations
PyObject* py_dap_chain_mempool_datum_add(PyObject *self, PyObject *args);

// Module initialization function
int cellframe_tx_init(PyObject *module);
