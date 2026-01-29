#include "cf_chain_internal.h"
#include "dap_chain_datum.h"
#include "dap_chain_srv.h"
#include "dap_proc_thread.h"

#define LOG_TAG "python_cellframe_chain"

typedef struct cf_chain_callback_entry {
    dap_chain_t *chain;
    cf_callback_type_t type;
    python_chain_callback_ctx_t *ctx;
    struct cf_chain_callback_entry *next;
} cf_chain_callback_entry_t;

static pthread_mutex_t s_chain_cb_lock = PTHREAD_MUTEX_INITIALIZER;
static cf_chain_callback_entry_t *s_chain_cb_entries = NULL;

static void s_chain_cb_track(dap_chain_t *chain, cf_callback_type_t type, python_chain_callback_ctx_t *ctx) {
    if (!chain || !ctx) {
        return;
    }
    cf_chain_callback_entry_t *entry = DAP_NEW_Z(cf_chain_callback_entry_t);
    if (!entry) {
        log_it(L_WARNING, "Failed to track chain callback context for cleanup");
        return;
    }
    entry->chain = chain;
    entry->type = type;
    entry->ctx = ctx;
    pthread_mutex_lock(&s_chain_cb_lock);
    entry->next = s_chain_cb_entries;
    s_chain_cb_entries = entry;
    pthread_mutex_unlock(&s_chain_cb_lock);
}

static size_t s_chain_cb_snapshot(dap_chain_t *chain, cf_callback_type_t type, python_chain_callback_ctx_t ***out_items) {
    if (!out_items || !chain) {
        return 0;
    }
    *out_items = NULL;
    pthread_mutex_lock(&s_chain_cb_lock);
    size_t count = 0;
    for (cf_chain_callback_entry_t *cur = s_chain_cb_entries; cur; cur = cur->next) {
        if (cur->chain == chain && cur->type == type && cur->ctx) {
            count++;
        }
    }
    if (!count) {
        pthread_mutex_unlock(&s_chain_cb_lock);
        return 0;
    }
    python_chain_callback_ctx_t **items = DAP_NEW_Z_COUNT(python_chain_callback_ctx_t *, count);
    if (!items) {
        pthread_mutex_unlock(&s_chain_cb_lock);
        log_it(L_WARNING, "Failed to allocate callback snapshot");
        return 0;
    }
    size_t idx = 0;
    for (cf_chain_callback_entry_t *cur = s_chain_cb_entries; cur && idx < count; cur = cur->next) {
        if (cur->chain == chain && cur->type == type && cur->ctx) {
            items[idx++] = cur->ctx;
        }
    }
    pthread_mutex_unlock(&s_chain_cb_lock);
    *out_items = items;
    return idx;
}

size_t cf_chain_cleanup_callbacks(dap_chain_t *a_chain) {
    if (!a_chain) {
        return 0;
    }

    cf_chain_callback_entry_t *detached = NULL;
    pthread_mutex_lock(&s_chain_cb_lock);
    cf_chain_callback_entry_t *prev = NULL;
    cf_chain_callback_entry_t *cur = s_chain_cb_entries;
    while (cur) {
        cf_chain_callback_entry_t *next = cur->next;
        if (cur->chain == a_chain) {
            if (prev) {
                prev->next = next;
            } else {
                s_chain_cb_entries = next;
            }
            cur->next = detached;
            detached = cur;
        } else {
            prev = cur;
        }
        cur = next;
    }
    pthread_mutex_unlock(&s_chain_cb_lock);

    size_t removed = 0;
    while (detached) {
        cf_chain_callback_entry_t *next = detached->next;
        cf_callbacks_registry_remove(detached->ctx);
        DAP_DELETE(detached);
        removed++;
        detached = next;
    }
    return removed;
}

static void s_chain_atom_notify_callback_wrapper(void *a_arg, dap_chain_t *a_chain,
                                                  dap_chain_cell_id_t a_id,
                                                  dap_chain_hash_fast_t *a_atom_hash,
                                                  void *a_atom, size_t a_atom_size,
                                                  dap_time_t a_atom_time);
static void s_chain_datum_notify_callback_wrapper(void *a_arg, dap_chain_hash_fast_t *a_datum_hash,
                                                  dap_chain_hash_fast_t *a_atom_hash, void *a_datum,
                                                  size_t a_datum_size, int a_ret_code,
                                                  uint32_t a_action, dap_chain_srv_uid_t a_uid);
static void s_chain_datum_removed_notify_callback_wrapper(void *a_arg, dap_chain_hash_fast_t *a_datum_hash,
                                                          dap_chain_datum_t *a_datum);
static void s_chain_timer_callback_wrapper(dap_chain_t *a_chain, dap_time_t a_time,
                                           void *a_arg, bool a_reverse);

static void s_chain_callback_ctx_free(void *ctx) {
    if (!ctx) {
        return;
    }
    DAP_DELETE(ctx);
}

/**
 * @brief Add callback notify for chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, callback function, user_data)
 * @return None
 */
PyObject* dap_chain_add_callback_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO|O", &l_chain_capsule, &l_callback, &l_user_data)) {
        log_it(L_ERROR, "Invalid arguments for chain_add_callback_notify");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        log_it(L_ERROR, "Callback must be callable");
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }

    python_chain_callback_ctx_t *l_ctx = DAP_NEW_Z(python_chain_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }

    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;

    char l_callback_id[128];
    snprintf(l_callback_id, sizeof(l_callback_id), "chain_atom_notify_%s_%p", l_chain->name, l_callback);
    if (cf_callbacks_registry_add_ex(CF_CALLBACK_TYPE_CHAIN_ATOM_NOTIFY, l_callback, l_user_data,
                                     l_ctx, l_callback_id, s_chain_callback_ctx_free) != 0) {
        DAP_DELETE(l_ctx);
        PyErr_SetString(PyExc_RuntimeError, "Failed to register chain atom notify callback");
        return NULL;
    }

    dap_chain_add_callback_notify(l_chain, s_chain_atom_notify_callback_wrapper, NULL, l_ctx);
    s_chain_cb_track(l_chain, CF_CALLBACK_TYPE_CHAIN_ATOM_NOTIFY, l_ctx);

    log_it(L_DEBUG, "Registered atom notify callback for chain '%s' (ID: %s)", l_chain->name, l_callback_id);
    Py_RETURN_NONE;
}

/**
 * @brief Notify atom addition (Python binding)
 */
PyObject* dap_chain_atom_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    PyObject *l_hash_obj = Py_None;
    PyObject *l_atom_obj = Py_None;
    unsigned long long l_atom_time = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOOO|K", &l_chain_capsule, &l_cell_id_obj, &l_hash_obj, &l_atom_obj, &l_atom_time)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    if (l_hash_obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "Hash must be bytes");
        return NULL;
    }
    dap_hash_fast_t *l_hash = NULL;
    if (cf_chain_parse_hash(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }
    
    const uint8_t *l_atom_bytes = NULL;
    size_t l_atom_size = 0;
    if (!l_atom_obj || l_atom_obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "Atom must be bytes");
        return NULL;
    }
    if (!PyBytes_Check(l_atom_obj)) {
        PyErr_SetString(PyExc_TypeError, "Atom must be bytes");
        return NULL;
    }
    l_atom_bytes = (const uint8_t *)PyBytes_AsString(l_atom_obj);
    l_atom_size = (size_t)PyBytes_Size(l_atom_obj);

    if (!cf_chain_cell_is_configured(l_chain)) {
        if (dap_proc_thread_get_count() > 0) {
            dap_chain_atom_notify(l_chain, l_cell_id, l_hash, l_atom_bytes, l_atom_size, (dap_time_t)l_atom_time);
            Py_RETURN_NONE;
        }
        python_chain_callback_ctx_t **l_items = NULL;
        size_t l_count = s_chain_cb_snapshot(l_chain, CF_CALLBACK_TYPE_CHAIN_ATOM_NOTIFY, &l_items);
        for (size_t i = 0; i < l_count; ++i) {
            s_chain_atom_notify_callback_wrapper(l_items[i], l_chain, l_cell_id, l_hash,
                                                 (void *)l_atom_bytes, l_atom_size, (dap_time_t)l_atom_time);
        }
        DAP_DELETE(l_items);
        Py_RETURN_NONE;
    }
    
    dap_chain_atom_notify(l_chain, l_cell_id, l_hash, l_atom_bytes, l_atom_size, (dap_time_t)l_atom_time);
    Py_RETURN_NONE;
}

/**
 * @brief Notify atom removal (Python binding)
 */

PyObject* dap_chain_atom_remove_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    unsigned long long l_prev_atom_time = 0;
    
    if (!PyArg_ParseTuple(a_args, "OO|K", &l_chain_capsule, &l_cell_id_obj, &l_prev_atom_time)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_chain_atom_remove_notify(l_chain, l_cell_id, (dap_time_t)l_prev_atom_time);
    Py_RETURN_NONE;
}

/**
 * @brief Notify datum addition to index (Python binding)
 */

PyObject* dap_chain_datum_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    PyObject *l_hash_obj = Py_None;
    PyObject *l_atom_hash_obj = Py_None;
    PyObject *l_datum_obj = NULL;
    int l_ret_code = 0;
    unsigned int l_action = 0;
    unsigned long long l_uid = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOOOOiIK", &l_chain_capsule, &l_cell_id_obj, &l_hash_obj, &l_atom_hash_obj,
                          &l_datum_obj, &l_ret_code, &l_action, &l_uid)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    if (!PyBytes_Check(l_datum_obj)) {
        PyErr_SetString(PyExc_TypeError, "Datum must be bytes");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    if (l_hash_obj == Py_None || l_atom_hash_obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "Hashes must be bytes");
        return NULL;
    }
    
    dap_hash_fast_t *l_hash = NULL;
    dap_hash_fast_t *l_atom_hash = NULL;
    if (cf_chain_parse_hash(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }
    if (cf_chain_parse_hash(l_atom_hash_obj, &l_atom_hash) != 0) {
        return NULL;
    }
    
    const uint8_t *l_datum_bytes = (const uint8_t *)PyBytes_AsString(l_datum_obj);
    size_t l_datum_size = (size_t)PyBytes_Size(l_datum_obj);
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};

    if (!cf_chain_cell_is_configured(l_chain)) {
        if (dap_proc_thread_get_count() > 0) {
            dap_chain_datum_notify(l_chain, l_cell_id, l_hash, l_atom_hash, l_datum_bytes, l_datum_size,
                                   l_ret_code, l_action, l_uid_struct);
            Py_RETURN_NONE;
        }
        python_chain_callback_ctx_t **l_items = NULL;
        size_t l_count = s_chain_cb_snapshot(l_chain, CF_CALLBACK_TYPE_CHAIN_DATUM_INDEX, &l_items);
        for (size_t i = 0; i < l_count; ++i) {
            s_chain_datum_notify_callback_wrapper(l_items[i], l_hash, l_atom_hash, (void *)l_datum_bytes,
                                                  l_datum_size, l_ret_code, l_action, l_uid_struct);
        }
        DAP_DELETE(l_items);
        Py_RETURN_NONE;
    }
    
    dap_chain_datum_notify(l_chain, l_cell_id, l_hash, l_atom_hash, l_datum_bytes, l_datum_size,
                           l_ret_code, l_action, l_uid_struct);
    Py_RETURN_NONE;
}

/**
 * @brief Notify datum removal from index (Python binding)
 */

PyObject* dap_chain_datum_removed_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    PyObject *l_hash_obj = Py_None;
    PyObject *l_datum_capsule = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OOOO", &l_chain_capsule, &l_cell_id_obj, &l_hash_obj, &l_datum_capsule)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    if (l_hash_obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "Hash must be bytes");
        return NULL;
    }
    dap_hash_fast_t *l_hash = NULL;
    if (cf_chain_parse_hash(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }
    
    dap_chain_datum_t *l_datum = NULL;
    if (l_datum_capsule && l_datum_capsule != Py_None) {
        if (!PyCapsule_CheckExact(l_datum_capsule)) {
            PyErr_SetString(PyExc_TypeError, "Datum must be a capsule or None");
            return NULL;
        }
        l_datum = (dap_chain_datum_t*)PyCapsule_GetPointer(l_datum_capsule, "dap_chain_datum_t");
        if (!l_datum) {
            PyErr_SetString(PyExc_ValueError, "Invalid datum capsule");
            return NULL;
        }
    }

    if (!cf_chain_cell_is_configured(l_chain)) {
        if (dap_proc_thread_get_count() > 0) {
            dap_chain_datum_removed_notify(l_chain, l_cell_id, l_hash, l_datum);
            Py_RETURN_NONE;
        }
        python_chain_callback_ctx_t **l_items = NULL;
        size_t l_count = s_chain_cb_snapshot(l_chain, CF_CALLBACK_TYPE_CHAIN_DATUM_REMOVED, &l_items);
        for (size_t i = 0; i < l_count; ++i) {
            s_chain_datum_removed_notify_callback_wrapper(l_items[i], l_hash, l_datum);
        }
        DAP_DELETE(l_items);
        Py_RETURN_NONE;
    }
    
    dap_chain_datum_removed_notify(l_chain, l_cell_id, l_hash, l_datum);
    Py_RETURN_NONE;
}

// =========================================
// CALLBACK WRAPPERS FOR CHAIN NOTIFICATIONS
// =========================================

/**
 * @brief C callback wrapper for atom notify - calls Python callback
 * @note Called from C SDK when atom is added to chain
 * Signature: void (*dap_chain_callback_notify_t)(void *a_arg, dap_chain_t *a_chain, 
 *                                                  dap_chain_cell_id_t a_id, 
 *                                                  dap_chain_hash_fast_t *a_atom_hash, 
 *                                                  void *a_atom, size_t a_atom_size, 
 *                                                  dap_time_t a_atom_time)
 */
static void s_chain_atom_notify_callback_wrapper(void *a_arg, dap_chain_t *a_chain, 
                                                   dap_chain_cell_id_t a_id,
                                                   dap_chain_hash_fast_t *a_atom_hash, 
                                                   void *a_atom, size_t a_atom_size, 
                                                   dap_time_t a_atom_time) {
    python_chain_callback_ctx_t *l_ctx = (python_chain_callback_ctx_t*)a_arg;
    if (!l_ctx || !l_ctx->callback) {
        log_it(L_ERROR, "Invalid chain atom notify callback context");
        return;
    }
    
    // Acquire GIL for Python callback
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    // Build Python arguments
    PyObject *l_chain_capsule = PyCapsule_New(a_chain, "dap_chain_t", NULL);
    PyObject *l_cell_id = PyLong_FromUnsignedLongLong(a_id.uint64);
    PyObject *l_atom_hash = NULL;
    PyObject *l_atom = NULL;
    if (a_atom_hash) {
        l_atom_hash = PyBytes_FromStringAndSize((const char*)a_atom_hash, sizeof(dap_hash_fast_t));
    } else {
        Py_INCREF(Py_None);
        l_atom_hash = Py_None;
    }
    if (a_atom) {
        l_atom = PyBytes_FromStringAndSize((const char*)a_atom, a_atom_size);
    } else {
        Py_INCREF(Py_None);
        l_atom = Py_None;
    }
    PyObject *l_atom_size = PyLong_FromSize_t(a_atom_size);
    PyObject *l_atom_time = PyLong_FromUnsignedLongLong(a_atom_time);
    
    if (!l_chain_capsule || !l_cell_id || !l_atom_hash || !l_atom || !l_atom_size || !l_atom_time) {
        log_it(L_ERROR, "Failed to create Python objects for atom notify callback");
        Py_XDECREF(l_chain_capsule);
        Py_XDECREF(l_cell_id);
        Py_XDECREF(l_atom_hash);
        Py_XDECREF(l_atom);
        Py_XDECREF(l_atom_size);
        Py_XDECREF(l_atom_time);
        PyGILState_Release(l_gstate);
        return;
    }
    
    // Call Python callback
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->callback, l_chain_capsule, l_cell_id, l_atom_hash, l_atom, l_atom_size, l_atom_time, l_ctx->user_data, NULL
    );
    
    // Cleanup
    Py_DECREF(l_chain_capsule);
    Py_DECREF(l_cell_id);
    Py_XDECREF(l_atom_hash);
    Py_XDECREF(l_atom);
    Py_DECREF(l_atom_size);
    Py_DECREF(l_atom_time);
    
    if (!l_result) {
        log_it(L_ERROR, "Python atom notify callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }
    
    PyGILState_Release(l_gstate);
}

/**
 * @brief C callback wrapper for datum notify - calls Python callback
 * @note Called from C SDK when datum is added to chain index
 * Signature: void (*dap_chain_callback_datum_notify_t)(void *a_arg, dap_chain_hash_fast_t *a_datum_hash, 
 *                                                        dap_chain_hash_fast_t *a_atom_hash, void *a_datum, 
 *                                                        size_t a_datum_size, int a_ret_code, 
 *                                                        uint32_t a_action, dap_chain_srv_uid_t a_uid)
 */

static void s_chain_datum_notify_callback_wrapper(void *a_arg, dap_chain_hash_fast_t *a_datum_hash, 
                                                    dap_chain_hash_fast_t *a_atom_hash, void *a_datum, 
                                                    size_t a_datum_size, int a_ret_code, 
                                                    uint32_t a_action, dap_chain_srv_uid_t a_uid) {
    python_chain_callback_ctx_t *l_ctx = (python_chain_callback_ctx_t*)a_arg;
    if (!l_ctx || !l_ctx->callback) {
        log_it(L_ERROR, "Invalid chain datum notify callback context");
        return;
    }
    
    // Acquire GIL for Python callback
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    // Build Python arguments
    PyObject *l_datum_hash = NULL;
    PyObject *l_atom_hash = NULL;
    PyObject *l_datum = NULL;
    if (a_datum_hash) {
        l_datum_hash = PyBytes_FromStringAndSize((const char*)a_datum_hash, sizeof(dap_hash_fast_t));
    } else {
        Py_INCREF(Py_None);
        l_datum_hash = Py_None;
    }
    if (a_atom_hash) {
        l_atom_hash = PyBytes_FromStringAndSize((const char*)a_atom_hash, sizeof(dap_hash_fast_t));
    } else {
        Py_INCREF(Py_None);
        l_atom_hash = Py_None;
    }
    if (a_datum) {
        l_datum = PyBytes_FromStringAndSize((const char*)a_datum, a_datum_size);
    } else {
        Py_INCREF(Py_None);
        l_datum = Py_None;
    }
    PyObject *l_datum_size = PyLong_FromSize_t(a_datum_size);
    PyObject *l_ret_code = PyLong_FromLong(a_ret_code);
    PyObject *l_action = PyLong_FromUnsignedLong(a_action);
    PyObject *l_uid = PyLong_FromUnsignedLongLong(a_uid.uint64);
    
    if (!l_datum_hash || !l_atom_hash || !l_datum || !l_datum_size || !l_ret_code || !l_action || !l_uid) {
        log_it(L_ERROR, "Failed to create Python objects for datum notify callback");
        Py_XDECREF(l_datum_hash);
        Py_XDECREF(l_atom_hash);
        Py_XDECREF(l_datum);
        Py_XDECREF(l_datum_size);
        Py_XDECREF(l_ret_code);
        Py_XDECREF(l_action);
        Py_XDECREF(l_uid);
        PyGILState_Release(l_gstate);
        return;
    }
    
    // Call Python callback
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->callback, l_datum_hash, l_atom_hash, l_datum, l_datum_size, l_ret_code, l_action, l_uid, l_ctx->user_data, NULL
    );
    
    // Cleanup
    Py_XDECREF(l_datum_hash);
    Py_XDECREF(l_atom_hash);
    Py_XDECREF(l_datum);
    Py_XDECREF(l_datum_size);
    Py_DECREF(l_ret_code);
    Py_DECREF(l_action);
    Py_DECREF(l_uid);
    
    if (!l_result) {
        log_it(L_ERROR, "Python datum notify callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }
    
    PyGILState_Release(l_gstate);
}

/**
 * @brief C callback wrapper for datum removed notify - calls Python callback
 * @note Called from C SDK when datum is removed from chain index
 * Signature: void (*dap_chain_callback_datum_removed_notify_t)(void *a_arg, 
 *                                                                dap_chain_hash_fast_t *a_datum_hash, 
 *                                                                dap_chain_datum_t *a_datum)
 */

static void s_chain_datum_removed_notify_callback_wrapper(void *a_arg, dap_chain_hash_fast_t *a_datum_hash, 
                                                           dap_chain_datum_t *a_datum) {
    python_chain_callback_ctx_t *l_ctx = (python_chain_callback_ctx_t*)a_arg;
    if (!l_ctx || !l_ctx->callback) {
        log_it(L_ERROR, "Invalid chain datum removed notify callback context");
        return;
    }
    
    // Acquire GIL for Python callback
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    // Build Python arguments
    PyObject *l_datum_hash = NULL;
    PyObject *l_datum_capsule = NULL;
    if (a_datum_hash) {
        l_datum_hash = PyBytes_FromStringAndSize((const char*)a_datum_hash, sizeof(dap_hash_fast_t));
    } else {
        Py_INCREF(Py_None);
        l_datum_hash = Py_None;
    }
    if (a_datum) {
        l_datum_capsule = PyCapsule_New(a_datum, "dap_chain_datum_t", NULL);
    } else {
        Py_INCREF(Py_None);
        l_datum_capsule = Py_None;
    }
    
    if (!l_datum_hash || !l_datum_capsule) {
        log_it(L_ERROR, "Failed to create Python objects for datum removed notify callback");
        Py_XDECREF(l_datum_hash);
        Py_XDECREF(l_datum_capsule);
        PyGILState_Release(l_gstate);
        return;
    }
    
    // Call Python callback
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->callback, l_datum_hash, l_datum_capsule, l_ctx->user_data, NULL
    );
    
    // Cleanup
    Py_XDECREF(l_datum_hash);
    Py_XDECREF(l_datum_capsule);
    
    if (!l_result) {
        log_it(L_ERROR, "Python datum removed notify callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }
    
    PyGILState_Release(l_gstate);
}

/**
 * @brief C callback wrapper for blockchain timer - calls Python callback
 * @note Called from C SDK periodically for blockchain maintenance
 * Signature: void (*dap_chain_callback_blockchain_timer_t)(dap_chain_t *a_chain, 
 *                                                            dap_time_t a_time, 
 *                                                            void *a_arg, 
 *                                                            bool a_reverse)
 */

static void s_chain_timer_callback_wrapper(dap_chain_t *a_chain, dap_time_t a_time, void *a_arg, bool a_reverse) {
    python_chain_callback_ctx_t *l_ctx = (python_chain_callback_ctx_t*)a_arg;
    if (!l_ctx || !l_ctx->callback) {
        log_it(L_ERROR, "Invalid chain timer callback context");
        return;
    }
    
    // Acquire GIL for Python callback
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    // Build Python arguments
    PyObject *l_chain_capsule = PyCapsule_New(a_chain, "dap_chain_t", NULL);
    PyObject *l_time = PyLong_FromUnsignedLongLong(a_time);
    PyObject *l_reverse = PyBool_FromLong(a_reverse);
    
    if (!l_chain_capsule || !l_time || !l_reverse) {
        log_it(L_ERROR, "Failed to create Python objects for timer callback");
        Py_XDECREF(l_chain_capsule);
        Py_XDECREF(l_time);
        Py_XDECREF(l_reverse);
        PyGILState_Release(l_gstate);
        return;
    }
    
    // Call Python callback
    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->callback, l_chain_capsule, l_time, l_reverse, l_ctx->user_data, NULL
    );
    
    // Cleanup
    Py_DECREF(l_chain_capsule);
    Py_DECREF(l_time);
    Py_DECREF(l_reverse);
    
    if (!l_result) {
        log_it(L_ERROR, "Python timer callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }
    
    PyGILState_Release(l_gstate);
}

/**
 * @brief Add callback for datum index notifications
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, callback function, optional user_data)
 * @return None
 */

PyObject* dap_chain_add_callback_datum_index_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO|O", &l_chain_obj, &l_callback, &l_user_data)) {
        PyErr_SetString(PyExc_TypeError, "Expected (chain, callback, [user_data])");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be callable");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    // Allocate callback context (will be freed by registry cleanup)
    python_chain_callback_ctx_t *l_ctx = DAP_NEW_Z(python_chain_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    
    // Register in cleanup registry (owns ctx)
    char l_callback_id[128];
    snprintf(l_callback_id, sizeof(l_callback_id), "chain_datum_notify_%s_%p", l_chain->name, l_callback);
    if (cf_callbacks_registry_add_ex(CF_CALLBACK_TYPE_CHAIN_DATUM_INDEX, l_callback, l_user_data,
                                     l_ctx, l_callback_id, s_chain_callback_ctx_free) != 0) {
        DAP_DELETE(l_ctx);
        PyErr_SetString(PyExc_RuntimeError, "Failed to register chain datum index callback");
        return NULL;
    }
    
    // Register C callback with SDK
    dap_chain_add_callback_datum_index_notify(l_chain, s_chain_datum_notify_callback_wrapper, NULL, l_ctx);
    s_chain_cb_track(l_chain, CF_CALLBACK_TYPE_CHAIN_DATUM_INDEX, l_ctx);
    
    log_it(L_DEBUG, "Registered datum index notify callback for chain '%s' (ID: %s)", l_chain->name, l_callback_id);
    
    Py_RETURN_NONE;
}

/**
 * @brief Add callback for datum removed from index notifications
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, callback function, optional user_data)
 * @return None
 */

PyObject* dap_chain_add_callback_datum_removed_from_index_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO|O", &l_chain_obj, &l_callback, &l_user_data)) {
        PyErr_SetString(PyExc_TypeError, "Expected (chain, callback, [user_data])");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be callable");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    // Allocate callback context (will be freed by registry cleanup)
    python_chain_callback_ctx_t *l_ctx = DAP_NEW_Z(python_chain_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    
    // Register in cleanup registry (owns ctx)
    char l_callback_id[128];
    snprintf(l_callback_id, sizeof(l_callback_id), "chain_datum_removed_%s_%p", l_chain->name, l_callback);
    if (cf_callbacks_registry_add_ex(CF_CALLBACK_TYPE_CHAIN_DATUM_REMOVED, l_callback, l_user_data,
                                     l_ctx, l_callback_id, s_chain_callback_ctx_free) != 0) {
        DAP_DELETE(l_ctx);
        PyErr_SetString(PyExc_RuntimeError, "Failed to register chain datum removed callback");
        return NULL;
    }
    
    // Register C callback with SDK
    dap_chain_add_callback_datum_removed_from_index_notify(l_chain, s_chain_datum_removed_notify_callback_wrapper, NULL, l_ctx);
    s_chain_cb_track(l_chain, CF_CALLBACK_TYPE_CHAIN_DATUM_REMOVED, l_ctx);
    
    log_it(L_DEBUG, "Registered datum removed notify callback for chain '%s' (ID: %s)", l_chain->name, l_callback_id);
    
    Py_RETURN_NONE;
}

/**
 * @brief Add atom confirmed notification callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, callback function, optional user_data, optional conf_cnt)
 * @return None
 */

PyObject* dap_chain_atom_confirmed_notify_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    unsigned long long l_conf_cnt = 1;
    
    if (!PyArg_ParseTuple(a_args, "OO|OK", &l_chain_obj, &l_callback, &l_user_data, &l_conf_cnt)) {
        PyErr_SetString(PyExc_TypeError, "Expected (chain, callback, [user_data], [conf_cnt])");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be callable");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    // Allocate callback context (will be freed by registry cleanup)
    python_chain_callback_ctx_t *l_ctx = DAP_NEW_Z(python_chain_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    
    // Register in cleanup registry (owns ctx)
    char l_callback_id[128];
    snprintf(l_callback_id, sizeof(l_callback_id), "chain_atom_confirmed_%s_%p", l_chain->name, l_callback);
    if (cf_callbacks_registry_add_ex(CF_CALLBACK_TYPE_CHAIN_ATOM_CONFIRMED, l_callback, l_user_data,
                                     l_ctx, l_callback_id, s_chain_callback_ctx_free) != 0) {
        DAP_DELETE(l_ctx);
        PyErr_SetString(PyExc_RuntimeError, "Failed to register chain atom confirmed callback");
        return NULL;
    }
    
    // Register C callback with SDK
    dap_chain_atom_confirmed_notify_add(l_chain, s_chain_atom_notify_callback_wrapper, l_ctx, l_conf_cnt);
    s_chain_cb_track(l_chain, CF_CALLBACK_TYPE_CHAIN_ATOM_CONFIRMED, l_ctx);
    
    log_it(L_DEBUG, "Registered atom confirmed notify callback for chain '%s' (conf_cnt=%llu, ID: %s)", 
           l_chain->name, l_conf_cnt, l_callback_id);
    
    Py_RETURN_NONE;
}

/**
 * @brief Add timer callback for blockchain
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, callback function, optional user_data)
 * @return Integer result code (0 = success)
 */

PyObject* dap_chain_add_callback_timer_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO|O", &l_chain_obj, &l_callback, &l_user_data)) {
        PyErr_SetString(PyExc_TypeError, "Expected (chain, callback, [user_data])");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be callable");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    // Allocate callback context (will be freed by registry cleanup)
    python_chain_callback_ctx_t *l_ctx = DAP_NEW_Z(python_chain_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    
    // Register in cleanup registry (owns ctx)
    char l_callback_id[128];
    snprintf(l_callback_id, sizeof(l_callback_id), "chain_timer_%s_%p", l_chain->name, l_callback);
    if (cf_callbacks_registry_add_ex(CF_CALLBACK_TYPE_CHAIN_TIMER, l_callback, l_user_data,
                                     l_ctx, l_callback_id, s_chain_callback_ctx_free) != 0) {
        DAP_DELETE(l_ctx);
        PyErr_SetString(PyExc_RuntimeError, "Failed to register chain timer callback");
        return NULL;
    }
    
    // Register C callback with SDK
    int l_result = dap_chain_add_callback_timer(l_chain, s_chain_timer_callback_wrapper, l_ctx);
    if (l_result != 0) {
        cf_callbacks_registry_remove(l_ctx);
        PyErr_Format(PyExc_RuntimeError, "Failed to register timer callback: %d", l_result);
        return NULL;
    }
    s_chain_cb_track(l_chain, CF_CALLBACK_TYPE_CHAIN_TIMER, l_ctx);
    
    log_it(L_DEBUG, "Registered timer callback for chain '%s' (ID: %s)", l_chain->name, l_callback_id);
    
    return PyLong_FromLong(0);  // Success
}
