#ifndef CELLFRAME_WALLET_H
#define CELLFRAME_WALLET_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

// =========================================
// WALLET TYPE DEFINITION
// =========================================

typedef struct {
    PyObject_HEAD
    void *wallet_ptr;
    char *wallet_name;
} PyCellframeWallet;

// =========================================
// WALLET FUNCTION DECLARATIONS
// =========================================

extern PyTypeObject PyCellframeWalletType;
PyObject* PyCellframeWallet_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int PyCellframeWallet_init(PyCellframeWallet *self, PyObject *args, PyObject *kwds);
void PyCellframeWallet_dealloc(PyCellframeWallet *self);
PyObject* PyCellframeWallet_create(PyObject *self, PyObject *args);
PyObject* PyCellframeWallet_get_balance_method(PyCellframeWallet *self, PyObject *args);

// Wallet module-level functions (for import)
PyObject* py_dap_chain_wallet_create(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_create_with_seed(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_create_with_seed_multi(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_open(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_open_ext(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_close(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_save(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_get_addr(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_get_key(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_get_pkey(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_get_balance(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_activate(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_deactivate(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_get_pkey_hash(PyObject *self, PyObject *args);

// Shared wallet functions
PyObject* py_dap_chain_wallet_shared_get_tx_hashes_json(PyObject *self, PyObject *args);
PyObject* py_dap_chain_wallet_shared_hold_tx_add(PyObject *self, PyObject *args);

// Module initialization function
int cellframe_wallet_init(PyObject *module);

#ifdef __cplusplus
}
#endif

#endif // CELLFRAME_WALLET_H 