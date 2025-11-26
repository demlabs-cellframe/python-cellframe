#ifndef CELLFRAME_LEDGER_INTERNAL_H
#define CELLFRAME_LEDGER_INTERNAL_H

#include "cellframe.h"
#include "dap_chain_ledger.h"
#include "dap_chain_datum_token.h"
#include "dap_chain_datum_tx_event.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

// Common log tag
#define LOG_TAG "python_cellframe_ledger"

// Structure to hold Python callback information
typedef struct {
    PyObject *callback;  // Python callable object
    PyObject *ledger_capsule;  // Ledger capsule for reference
} py_ledger_callback_data_t;

// Helper function to convert dap_chain_tx_event_t to Python dict
PyObject* py_chain_tx_event_to_dict(dap_chain_tx_event_t *a_event);

// C callback wrappers (implemented in callbacks module)
void py_ledger_event_notify_callback(void *a_arg, dap_ledger_t *a_ledger, 
                                    dap_chain_tx_event_t *a_event, 
                                    dap_hash_fast_t *a_tx_hash,
                                    dap_ledger_notify_opcodes_t a_opcode);

void py_ledger_tx_add_notify_callback(void *a_arg, dap_ledger_t *a_ledger, 
                                     dap_chain_datum_tx_t *a_tx,
                                     dap_ledger_notify_opcodes_t a_opcode);

void py_ledger_bridged_tx_notify_callback(dap_ledger_t *a_ledger, 
                                         dap_chain_datum_tx_t *a_tx,
                                         dap_hash_fast_t *a_tx_hash,
                                         void *a_arg,
                                         dap_ledger_notify_opcodes_t a_opcode);

// Forward declarations for Python binding functions from ledger modules
PyObject* dap_ledger_count_from_to_py(PyObject *a_self, PyObject *a_args);  // from utils
PyObject* dap_ledger_purge_py(PyObject *a_self, PyObject *a_args);  // from token
PyObject* dap_ledger_count_py(PyObject *a_self, PyObject *a_args);  // from token

// Forward declarations for get_methods functions from all modules
PyMethodDef* cellframe_ledger_lifecycle_get_methods(void);
PyMethodDef* cellframe_ledger_callbacks_get_methods(void);
PyMethodDef* cellframe_ledger_tx_get_methods(void);
PyMethodDef* cellframe_ledger_token_get_methods(void);
PyMethodDef* cellframe_ledger_balance_get_methods(void);
PyMethodDef* cellframe_ledger_tx_lists_get_methods(void);
PyMethodDef* cellframe_ledger_cond_get_methods(void);
PyMethodDef* cellframe_ledger_decree_get_methods(void);
PyMethodDef* cellframe_ledger_anchor_get_methods(void);
PyMethodDef* cellframe_ledger_event_get_methods(void);
PyMethodDef* cellframe_ledger_utils_get_methods(void);

#ifdef __cplusplus
}
#endif

#endif /* CELLFRAME_LEDGER_INTERNAL_H */

