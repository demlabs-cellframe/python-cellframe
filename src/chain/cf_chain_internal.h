#ifndef CF_CHAIN_INTERNAL_H
#define CF_CHAIN_INTERNAL_H

#include "cellframe.h"
#include "../common/cf_callbacks_registry.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Python callback context structure
typedef struct {
    PyObject *callback;      // Python callable
    PyObject *user_data;     // Optional user data
} python_chain_callback_ctx_t;

typedef struct {
    dap_chain_callback_datum_notify_t callback;
    dap_proc_thread_t *proc_thread;
    void *arg;
} cf_chain_datum_notifier_t;

typedef struct {
    dap_chain_callback_datum_removed_notify_t callback;
    dap_proc_thread_t *proc_thread;
    void *arg;
} cf_chain_datum_removed_notifier_t;

int cf_chain_parse_cell_id(PyObject *obj, dap_chain_cell_id_t *out);
bool cf_chain_cell_is_configured(dap_chain_t *a_chain);
int cf_chain_parse_hash(PyObject *obj, dap_hash_fast_t **out_hash);

#endif  // CF_CHAIN_INTERNAL_H
