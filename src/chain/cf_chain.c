#include "cellframe.h"
#include "dap_chain_common.h"
#include "dap_chain_net.h"
#include "dap_chain_mempool.h"
#include "dap_chain_block.h"
#include "dap_chain_datum.h"
#include "dap_chain_cell.h"
#include "dap_chain_ch.h"
#include "dap_chain_ch_pkt.h"
#include "dap_chain_net_utils.h"
#include "dap_chain_policy.h"
#include "dap_chain_srv.h"
#include "dap_global_db.h"
#include "dap_file_utils.h"
#include "../common/cf_callbacks_registry.h"

#define LOG_TAG "python_cellframe_chain"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Chain type - to be implemented
PyTypeObject PyCellframeChainType = {0};

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

static int cf_chain_parse_cell_id(PyObject *obj, dap_chain_cell_id_t *out) {
    if (!out) {
        PyErr_SetString(PyExc_RuntimeError, "Cell ID output pointer is NULL");
        return -1;
    }
    memset(out, 0, sizeof(*out));
    if (!obj || obj == Py_None) {
        return 0;
    }
    if (PyLong_Check(obj)) {
        unsigned long long value = PyLong_AsUnsignedLongLong(obj);
        if (PyErr_Occurred()) {
            PyErr_SetString(PyExc_ValueError, "Failed to parse cell_id as uint64");
            return -1;
        }
        out->uint64 = value;
        return 0;
    }
    if (PyDict_Check(obj)) {
        PyObject *raw_obj = PyDict_GetItemString(obj, "raw");
        if (raw_obj && raw_obj != Py_None) {
            if (!PyBytes_Check(raw_obj)) {
                PyErr_SetString(PyExc_TypeError, "cell_id.raw must be bytes");
                return -1;
            }
            if (PyBytes_Size(raw_obj) != DAP_CHAIN_SHARD_ID_SIZE) {
                PyErr_Format(PyExc_ValueError, "cell_id.raw must be %d bytes", DAP_CHAIN_SHARD_ID_SIZE);
                return -1;
            }
            memcpy(out->raw, PyBytes_AsString(raw_obj), DAP_CHAIN_SHARD_ID_SIZE);
            return 0;
        }
        PyObject *uint64_obj = PyDict_GetItemString(obj, "uint64");
        if (uint64_obj) {
            unsigned long long value = PyLong_AsUnsignedLongLong(uint64_obj);
            if (PyErr_Occurred()) {
                PyErr_SetString(PyExc_ValueError, "Failed to parse cell_id.uint64");
                return -1;
            }
            out->uint64 = value;
            return 0;
        }
        PyErr_SetString(PyExc_TypeError, "cell_id dict must contain 'uint64' or 'raw'");
        return -1;
    }
    if (PyBytes_Check(obj)) {
        if (PyBytes_Size(obj) != DAP_CHAIN_SHARD_ID_SIZE) {
            PyErr_Format(PyExc_ValueError, "cell_id bytes must be %d bytes", DAP_CHAIN_SHARD_ID_SIZE);
            return -1;
        }
        memcpy(out->raw, PyBytes_AsString(obj), DAP_CHAIN_SHARD_ID_SIZE);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "cell_id must be int, dict, bytes, or None");
    return -1;
}

static void cf_chain_ensure_storage_dir(dap_chain_t *a_chain) {
    if (!a_chain) {
        return;
    }
    dap_chain_pvt_t *l_chain_pvt = DAP_CHAIN_PVT(a_chain);
    if (!l_chain_pvt || l_chain_pvt->file_storage_dir) {
        return;
    }
    char l_path[256];
    snprintf(l_path, sizeof(l_path), "/tmp/python_cellframe/%" DAP_UINT64_FORMAT_U "/%" DAP_UINT64_FORMAT_U,
             a_chain->net_id.uint64, a_chain->id.uint64);
    l_chain_pvt->file_storage_dir = dap_strdup(l_path);
    if (l_chain_pvt->file_storage_dir) {
        dap_mkdir_with_parents(l_chain_pvt->file_storage_dir);
    }
}

static bool cf_chain_cell_is_configured(dap_chain_t *a_chain) {
    if (!a_chain || !a_chain->config) {
        return false;
    }
    cf_chain_ensure_storage_dir(a_chain);
    dap_chain_pvt_t *l_chain_pvt = DAP_CHAIN_PVT(a_chain);
    return l_chain_pvt && l_chain_pvt->file_storage_dir;
}

static int cf_chain_parse_hash(PyObject *obj, dap_hash_fast_t **out_hash) {
    if (!out_hash) {
        PyErr_SetString(PyExc_RuntimeError, "Hash output pointer is NULL");
        return -1;
    }
    *out_hash = NULL;
    if (!obj || obj == Py_None) {
        return 0;
    }
    if (!PyBytes_Check(obj)) {
        PyErr_SetString(PyExc_TypeError, "Hash must be bytes or None");
        return -1;
    }
    if (PyBytes_Size(obj) != (Py_ssize_t)sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return -1;
    }
    *out_hash = (dap_hash_fast_t *)PyBytes_AsString(obj);
    if (!*out_hash) {
        PyErr_SetString(PyExc_ValueError, "Failed to parse hash bytes");
        return -1;
    }
    return 0;
}

static void s_chain_atom_notify_callback_wrapper(void *a_arg, dap_chain_t *a_chain, 
                                                   dap_chain_cell_id_t a_id,
                                                   dap_chain_hash_fast_t *a_atom_hash, 
                                                   void *a_atom, size_t a_atom_size, 
                                                   dap_time_t a_atom_time);

PyObject* PyCellframeChain_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    (void)type; (void)args; (void)kwds;
    PyErr_SetString(PyExc_NotImplementedError, "Chain type not yet implemented");
    return NULL;
}

int PyCellframeChain_init(PyCellframeChain *self, PyObject *args, PyObject *kwds) {
    (void)self; (void)args; (void)kwds;
    return -1;
}

void PyCellframeChain_dealloc(PyCellframeChain *self) {
    (void)self;
}

PyObject* PyCellframeChain_create(PyObject *self, PyObject *args) {
    (void)self; (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "Chain create not yet implemented");
    return NULL;
}

PyObject* PyCellframeChain_get_stats(PyCellframeChain *self, PyObject *args) {
    (void)self; (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "Chain get_stats not yet implemented");
    return NULL;
}

// =============================================================================
// CHAIN FUNCTIONS IMPLEMENTATION
// =============================================================================

/**
 * @brief Get network ID from chain address
 * @param a_addr Chain address pointer
 * @return Network ID as uint64_t
 */
uint64_t dap_chain_addr_get_net_id(dap_chain_addr_t *a_addr) {
    if (!a_addr) {
        return 0;
    }
    return a_addr->net_id.uint64;
}

/**
 * @brief Create chain atom (currently just a placeholder)
 * @param a_size Size of atom data
 * @return Pointer to allocated memory
 */
void* dap_chain_atom_create(size_t a_size) {
    if (a_size == 0) {
        return NULL;
    }
    return DAP_NEW_Z_SIZE(void, a_size);
}

/**
 * @brief Get mempool by chain name
 * @param a_chain_name Chain name
 * @return Pointer to chain or NULL if not found
 */
dap_chain_t* dap_chain_mempool_by_chain_name(const char *a_chain_name) {
    if (!a_chain_name) {
        return NULL;
    }
    
    // Find network by chain name
    dap_chain_net_t *l_net = dap_chain_net_by_name(a_chain_name);
    if (!l_net) {
        return NULL;
    }
    
    // Get default chain for transactions
    return dap_chain_net_get_default_chain_by_chain_type(l_net, CHAIN_TYPE_TX);
}

/**
 * @brief Get transaction from mempool by hash
 * @param a_chain Chain pointer
 * @param a_tx_hash Transaction hash string
 * @return Transaction datum or NULL if not found
 * 
 * @note After SDK refactoring, dap_chain_mempool_datum_get() was removed.
 *       Now using dap_global_db_get_sync() with mempool GDB group.
 */
dap_chain_datum_t* dap_chain_mempool_tx_get_by_hash(dap_chain_t *a_chain, const char *a_tx_hash) {
    if (!a_chain || !a_tx_hash) {
        return NULL;
    }
    
    // Get mempool GDB group name for this chain
    char *l_gdb_group_mempool = dap_chain_mempool_group_new(a_chain);
    if (!l_gdb_group_mempool) {
        log_it(L_ERROR, "Failed to get mempool GDB group for chain");
        return NULL;
    }
    
    // Get datum from Global DB
    size_t l_datum_size = 0;
    dap_chain_datum_t *l_datum = (dap_chain_datum_t*)dap_global_db_get_sync(
        l_gdb_group_mempool,
        a_tx_hash,
        &l_datum_size,
        NULL,
        NULL
    );
    
    DAP_DELETE(l_gdb_group_mempool);
    
    if (!l_datum) {
        log_it(L_WARNING, "Datum %s not found in mempool", a_tx_hash);
        return NULL;
    }
    
    // Verify datum size consistency
    size_t l_datum_size2 = dap_chain_datum_size(l_datum);
    if (l_datum_size != l_datum_size2) {
        log_it(L_ERROR, "Corrupted datum %s: size by headers %zu != GDB size %zu",
               a_tx_hash, l_datum_size2, l_datum_size);
        DAP_DELETE(l_datum);
        return NULL;
    }
    
    return l_datum;
}

// =============================================================================
// PYTHON WRAPPER FUNCTIONS FOR CHAIN
// =============================================================================

#define LOG_TAG "python_cellframe_chain"

/**
 * @brief Get atom by hash from chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, hash bytes)
 * @return Atom bytes or None if not found
 */
PyObject* dap_chain_get_atom_by_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    const char *l_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "Oy#", &l_chain_capsule, &l_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for chain_get_atom_by_hash");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    if (l_hash_size != sizeof(dap_hash_fast_t)) {
        log_it(L_ERROR, "Invalid hash size: %zd, expected %zu", l_hash_size, sizeof(dap_hash_fast_t));
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    dap_hash_fast_t *l_atom_hash = (dap_hash_fast_t*)l_hash_bytes;
    size_t l_atom_size = 0;
    
    dap_chain_atom_ptr_t l_atom = dap_chain_get_atom_by_hash(l_chain, l_atom_hash, &l_atom_size);
    if (!l_atom || l_atom_size == 0) {
        log_it(L_DEBUG, "Atom not found by hash in chain '%s'", l_chain->name);
        Py_RETURN_NONE;
    }
    
    // Return atom as bytes
    PyObject *l_result = PyBytes_FromStringAndSize((const char*)l_atom, l_atom_size);
    if (!l_result) {
        log_it(L_ERROR, "Failed to create bytes object for atom");
        Py_RETURN_NONE;
    }
    
    return l_result;
}

/**
 * @brief Get last atom hash, number and timestamp (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, cell_id dict)
 * @return Dict with 'hash' (bytes), 'num' (int), 'timestamp' (int) or None
 */
PyObject* dap_chain_get_atom_last_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "O|O", &l_chain_capsule, &l_cell_id_obj)) {
        log_it(L_ERROR, "Invalid arguments for chain_get_atom_last");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_hash_fast_t l_atom_hash = {0};
    uint64_t l_atom_num = 0;
    dap_time_t l_atom_timestamp = 0;
    
    bool l_result = dap_chain_get_atom_last_hash_num_ts(l_chain, l_cell_id, &l_atom_hash, &l_atom_num, &l_atom_timestamp);
    if (!l_result) {
        log_it(L_DEBUG, "No last atom found in chain '%s'", l_chain->name);
        Py_RETURN_NONE;
    }
    
    // Return dict with hash, num, timestamp
    PyObject *l_hash_bytes = PyBytes_FromStringAndSize((const char*)&l_atom_hash, sizeof(dap_hash_fast_t));
    if (!l_hash_bytes) {
        log_it(L_ERROR, "Failed to create bytes object for hash");
        Py_RETURN_NONE;
    }
    
    PyObject *l_result_dict = Py_BuildValue("{s:O,s:K,s:K}", 
                                            "hash", l_hash_bytes,
                                            "num", l_atom_num,
                                            "timestamp", l_atom_timestamp);
    Py_DECREF(l_hash_bytes);
    
    return l_result_dict;
}

/**
 * @brief Load all chain data from storage (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_load_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_load_all");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    int l_result = dap_chain_load_all(l_chain);
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to load chain '%s': %d", l_chain->name, l_result);
    } else {
        log_it(L_INFO, "Chain '%s' loaded successfully", l_chain->name);
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Check if chain has file store (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return True if has file store, False otherwise
 */
PyObject* dap_chain_has_file_store_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_has_file_store");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    bool l_result = dap_chain_has_file_store(l_chain);
    
    return PyBool_FromLong(l_result ? 1 : 0);
}

/**
 * @brief Purge chain data (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_purge");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }

    if (!cf_chain_cell_is_configured(l_chain)) {
        return PyLong_FromLong(-1);
    }
    
    int l_result = dap_chain_purge(l_chain);
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to purge chain '%s': %d", l_chain->name, l_result);
    } else {
        log_it(L_INFO, "Chain '%s' purged successfully", l_chain->name);
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Save atom to chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, cell_id dict, atom bytes)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_atom_save_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    const char *l_atom_bytes = NULL;
    Py_ssize_t l_atom_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOy#", &l_chain_capsule, &l_cell_id_obj, &l_atom_bytes, &l_atom_size)) {
        log_it(L_ERROR, "Invalid arguments for chain_atom_save");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_hash_fast_t l_new_atom_hash = {0};
    char *l_atom_map = NULL;
    
    int l_result = dap_chain_atom_save(l_chain, l_cell_id, (const uint8_t*)l_atom_bytes, l_atom_size, &l_new_atom_hash, &l_atom_map);
    
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to save atom to chain '%s': %d", l_chain->name, l_result);
    } else {
        log_it(L_INFO, "Atom saved successfully to chain '%s'", l_chain->name);
    }
    
    if (l_atom_map) {
        DAP_DELETE(l_atom_map);
    }
    
    // Return dict with result code and atom hash
    PyObject *l_hash_bytes = PyBytes_FromStringAndSize((const char*)&l_new_atom_hash, sizeof(dap_hash_fast_t));
    if (!l_hash_bytes) {
        return Py_BuildValue("i", l_result);
    }
    
    PyObject *l_result_dict = Py_BuildValue("{s:i,s:O}", 
                                            "code", l_result,
                                            "hash", l_hash_bytes);
    Py_DECREF(l_hash_bytes);
    
    return l_result_dict;
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

    Py_INCREF(l_callback);
    Py_INCREF(l_user_data);

    dap_chain_add_callback_notify(l_chain, s_chain_atom_notify_callback_wrapper, NULL, l_ctx);

    char l_callback_id[128];
    snprintf(l_callback_id, sizeof(l_callback_id), "chain_atom_notify_%s_%p", l_chain->name, l_callback);
    cf_callbacks_registry_add(CF_CALLBACK_TYPE_CHAIN_ATOM_NOTIFY, l_callback, l_user_data, l_ctx, l_callback_id);

    log_it(L_DEBUG, "Registered atom notify callback for chain '%s' (ID: %s)", l_chain->name, l_callback_id);
    Py_RETURN_NONE;
}

/**
 * @brief Python wrapper for dap_chain_addr_from_str
 */
PyObject* py_dap_chain_addr_from_str(PyObject *self, PyObject *args) {
    (void)self;
    const char *addr_str;
    
    if (!PyArg_ParseTuple(args, "s", &addr_str)) {
        return NULL;
    }
    
    dap_chain_addr_t *addr = dap_chain_addr_from_str(addr_str);
    if (!addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid address string");
        return NULL;
    }
    
    return PyCapsule_New(addr, "dap_chain_addr_t", NULL);
}

/**
 * @brief Python wrapper for dap_chain_addr_get_net_id
 */
PyObject* py_dap_chain_addr_get_net_id(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_capsule;
    
    if (!PyArg_ParseTuple(args, "O", &addr_capsule)) {
        return NULL;
    }
    
    dap_chain_addr_t *addr = (dap_chain_addr_t*)PyCapsule_GetPointer(addr_capsule, "dap_chain_addr_t");
    if (!addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid address object");
        return NULL;
    }
    
    uint64_t net_id = dap_chain_addr_get_net_id(addr);
    return PyLong_FromUnsignedLongLong(net_id);
}

/**
 * @brief Python wrapper for dap_chain_addr_to_str
 */
PyObject* py_dap_chain_addr_to_str(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_capsule;
    
    if (!PyArg_ParseTuple(args, "O", &addr_capsule)) {
        return NULL;
    }
    
    dap_chain_addr_t *addr = (dap_chain_addr_t*)PyCapsule_GetPointer(addr_capsule, "dap_chain_addr_t");
    if (!addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid address object");
        return NULL;
    }
    
    const char *addr_str = dap_chain_addr_to_str(addr);
    if (!addr_str) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert address to string");
        return NULL;
    }
    
    return PyUnicode_FromString(addr_str);
}

/**
 * @brief Python wrapper for dap_chain_addr_is_blank
 */
PyObject* py_dap_chain_addr_is_blank(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_capsule;
    
    if (!PyArg_ParseTuple(args, "O", &addr_capsule)) {
        return NULL;
    }
    
    dap_chain_addr_t *addr = (dap_chain_addr_t*)PyCapsule_GetPointer(addr_capsule, "dap_chain_addr_t");
    if (!addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid address object");
        return NULL;
    }
    
    bool is_blank = dap_chain_addr_is_blank(addr);
    return PyBool_FromLong(is_blank ? 1 : 0);
}

/**
 * @brief Python wrapper for dap_chain_addr_check_sum
 */
PyObject* py_dap_chain_addr_check_sum(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_capsule;
    
    if (!PyArg_ParseTuple(args, "O", &addr_capsule)) {
        return NULL;
    }
    
    dap_chain_addr_t *addr = (dap_chain_addr_t*)PyCapsule_GetPointer(addr_capsule, "dap_chain_addr_t");
    if (!addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid address object");
        return NULL;
    }
    
    int result = dap_chain_addr_check_sum(addr);
    return PyLong_FromLong(result);
}

/**
 * @brief Python wrapper for dap_chain_atom_create
 */
PyObject* py_dap_chain_atom_create(PyObject *self, PyObject *args) {
    (void)self;
    size_t size;
    
    if (!PyArg_ParseTuple(args, "n", &size)) {
        return NULL;
    }
    
    void *atom = dap_chain_atom_create(size);
    if (!atom) {
        PyErr_SetString(PyExc_MemoryError, "Failed to create atom");
        return NULL;
    }
    
    // Return capsule with size information stored in name
    char capsule_name[64];
    snprintf(capsule_name, sizeof(capsule_name), "dap_chain_atom:%zu", size);
    return PyCapsule_New(atom, capsule_name, NULL);
}

/**
 * @brief Get atom data from atom pointer
 * @param a_self Python self object (unused)
 * @param a_args Arguments (atom capsule)
 * @return Atom data as bytes or None
 */
PyObject* py_dap_chain_atom_get_data(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *atom_capsule;
    
    if (!PyArg_ParseTuple(args, "O", &atom_capsule)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(atom_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected atom capsule");
        return NULL;
    }
    
    void *atom = PyCapsule_GetPointer(atom_capsule, NULL);
    if (!atom) {
        PyErr_SetString(PyExc_ValueError, "Invalid atom capsule");
        return NULL;
    }
    
    // Extract size from capsule name if available
    const char *capsule_name = PyCapsule_GetName(atom_capsule);
    size_t atom_size = 0;
    if (capsule_name && strncmp(capsule_name, "dap_chain_atom:", 15) == 0) {
        atom_size = (size_t)atoi(capsule_name + 15);
    }
    
    if (atom_size == 0) {
        PyErr_SetString(PyExc_ValueError, "Atom size not available");
        return NULL;
    }
    
    // Return atom data as bytes
    return PyBytes_FromStringAndSize((const char*)atom, atom_size);
}

/**
 * @brief Python wrapper for dap_chain_mempool_by_chain_name
 */
PyObject* py_dap_chain_mempool_by_chain_name(PyObject *self, PyObject *args) {
    (void)self;
    const char *chain_name;
    
    if (!PyArg_ParseTuple(args, "s", &chain_name)) {
        return NULL;
    }
    
    dap_chain_t *chain = dap_chain_mempool_by_chain_name(chain_name);
    if (!chain) {
        PyErr_SetString(PyExc_ValueError, "Chain not found");
        return NULL;
    }
    
    return PyCapsule_New(chain, "dap_chain_t", NULL);
}

/**
 * @brief Python wrapper for dap_chain_mempool_tx_get_by_hash
 */
PyObject* py_dap_chain_mempool_tx_get_by_hash(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_capsule;
    const char *tx_hash;
    
    if (!PyArg_ParseTuple(args, "Os", &chain_capsule, &tx_hash)) {
        return NULL;
    }
    
    dap_chain_t *chain = (dap_chain_t*)PyCapsule_GetPointer(chain_capsule, "dap_chain_t");
    if (!chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain object");
        return NULL;
    }
    
    dap_chain_datum_t *datum = dap_chain_mempool_tx_get_by_hash(chain, tx_hash);
    if (!datum) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(datum, "dap_chain_datum_t", NULL);
}

/**
 * @brief Create a new chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (net_name, chain_name, net_id, chain_id)
 * @return Chain capsule or None on failure
 */
PyObject* dap_chain_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name = NULL;
    const char *l_chain_name = NULL;
    unsigned long long l_net_id = 0;
    unsigned long long l_chain_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "ssKK", &l_net_name, &l_chain_name, &l_net_id, &l_chain_id)) {
        log_it(L_ERROR, "Invalid arguments for chain_create");
        return NULL;
    }
    
    if (!l_net_name || !l_chain_name) {
        log_it(L_ERROR, "Network name and chain name must be provided");
        PyErr_SetString(PyExc_ValueError, "Network name and chain name are required");
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    
    dap_chain_t *l_chain = dap_chain_create(l_net_name, l_chain_name, l_net_id_struct, l_chain_id_struct);
    if (!l_chain) {
        log_it(L_ERROR, "Failed to create chain '%s' in network '%s'", l_chain_name, l_net_name);
        PyErr_Format(PyExc_RuntimeError, "Failed to create chain '%s'", l_chain_name);
        return NULL;
    }
    
    log_it(L_INFO, "Chain '%s' created successfully in network '%s'", l_chain_name, l_net_name);
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

/**
 * @brief Delete a chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return None
 */
PyObject* dap_chain_delete_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_delete");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    const char *l_chain_name = l_chain->name ? l_chain->name : "unknown";
    log_it(L_INFO, "Deleting chain '%s'", l_chain_name);

    dap_chain_pvt_t *l_chain_pvt = DAP_CHAIN_PVT(l_chain);
    if (l_chain_pvt) {
        if (!l_chain_pvt->cs_type) {
            l_chain_pvt->cs_type = (char *)"";
        }
        if (!l_chain_pvt->cs_name) {
            l_chain_pvt->cs_name = (char *)"unknown";
        }
    }

    dap_chain_delete(l_chain);
    
    // Invalidate the capsule by changing the name so PyCapsule_GetPointer fails.
    if (PyCapsule_SetName(l_chain_capsule, "dap_chain_t.deleted") != 0) {
        log_it(L_WARNING, "Failed to invalidate chain capsule after deletion");
    }
    
    Py_RETURN_NONE;
}

/**
 * @brief Set consensus type for chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, cs_type string)
 * @return None
 */
PyObject* dap_chain_set_cs_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    const char *l_cs_type = NULL;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_chain_capsule, &l_cs_type)) {
        log_it(L_ERROR, "Invalid arguments for chain_set_cs_type");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    if (!l_cs_type) {
        log_it(L_ERROR, "Consensus type must be provided");
        PyErr_SetString(PyExc_ValueError, "Consensus type is required");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    log_it(L_INFO, "Setting consensus type '%s' for chain '%s'", l_cs_type, l_chain->name);
    dap_chain_set_cs_type(l_chain, l_cs_type);
    
    Py_RETURN_NONE;
}

/**
 * @brief Set consensus name for chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, cs_name string)
 * @return None
 */
PyObject* dap_chain_set_cs_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    const char *l_cs_name = NULL;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_chain_capsule, &l_cs_name)) {
        log_it(L_ERROR, "Invalid arguments for chain_set_cs_name");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    if (!l_cs_name) {
        log_it(L_ERROR, "Consensus name must be provided");
        PyErr_SetString(PyExc_ValueError, "Consensus name is required");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    log_it(L_INFO, "Setting consensus name '%s' for chain '%s'", l_cs_name, l_chain->name);
    dap_chain_set_cs_name(l_chain, l_cs_name);
    
    Py_RETURN_NONE;
}

/**
 * @brief Add atom from threshold (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return None
 */
PyObject* dap_chain_atom_add_from_threshold_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_atom_add_from_threshold");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    log_it(L_DEBUG, "Adding atom from threshold for chain '%s'", l_chain->name);
    dap_chain_atom_add_from_threshold(l_chain);
    
    Py_RETURN_NONE;
}

/**
 * @brief Dump chain info to log (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return None
 */
// REMOVED: dap_chain_info_dump_log_py
// Function dap_chain_info_dump_log() was removed from CellFrame SDK
// No replacement available

/**
 * @brief Find chain by ID (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (net_id, chain_id)
 * @return Chain capsule or None if not found
 */
PyObject* dap_chain_find_by_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_chain_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "KK", &l_net_id, &l_chain_id)) {
        log_it(L_ERROR, "Invalid arguments for chain_find_by_id");
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    
    dap_chain_t *l_chain = dap_chain_find_by_id(l_net_id_struct, l_chain_id_struct);
    if (!l_chain) {
        log_it(L_DEBUG, "Chain with net_id=%llu, chain_id=%llu not found", l_net_id, l_chain_id);
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Found chain '%s' by net_id=%llu, chain_id=%llu", l_chain->name, l_net_id, l_chain_id);
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

/**
 * @brief Check if datum type is supported by chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, datum_type)
 * @return True if supported, False otherwise
 */
PyObject* dap_chain_datum_type_supported_by_chain_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    unsigned int l_datum_type = 0;
    
    if (!PyArg_ParseTuple(a_args, "OI", &l_chain_capsule, &l_datum_type)) {
        log_it(L_ERROR, "Invalid arguments for chain_datum_type_supported_by_chain");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    bool l_supported = dap_chain_datum_type_supported_by_chain(l_chain, (uint16_t)l_datum_type);
    log_it(L_DEBUG, "Datum type %u %s by chain '%s'", 
           l_datum_type, l_supported ? "supported" : "not supported", l_chain->name);
    
    return PyBool_FromLong(l_supported ? 1 : 0);
}

/**
 * @brief Check if chain generation is banned (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, generation)
 * @return True if banned, False otherwise
 */
PyObject* dap_chain_generation_banned_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    unsigned int l_generation = 0;
    
    if (!PyArg_ParseTuple(a_args, "OI", &l_chain_capsule, &l_generation)) {
        log_it(L_ERROR, "Invalid arguments for chain_generation_banned");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    bool l_banned = dap_chain_generation_banned(l_chain, (uint16_t)l_generation);
    log_it(L_DEBUG, "Generation %u is %s for chain '%s'", 
           l_generation, l_banned ? "banned" : "allowed", l_chain->name);
    
    return PyBool_FromLong(l_banned ? 1 : 0);
}

/**
 * @brief Ban a chain generation (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, generation)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_generation_ban_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    unsigned int l_generation = 0;
    
    if (!PyArg_ParseTuple(a_args, "OI", &l_chain_capsule, &l_generation)) {
        log_it(L_ERROR, "Invalid arguments for chain_generation_ban");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    int l_result = dap_chain_generation_ban(l_chain, (uint16_t)l_generation);
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to ban generation %u for chain '%s': %d", 
               l_generation, l_chain->name, l_result);
    } else {
        log_it(L_INFO, "Generation %u banned for chain '%s'", l_generation, l_chain->name);
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Get last atom hash, number and timestamp
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, cell_id)
 * @return Dict with atom_hash (bytes), atom_num (int), atom_timestamp (int) or None
 */
PyObject* dap_chain_get_atom_last_hash_num_ts_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "O|O", &l_chain_obj, &l_cell_id_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_hash_fast_t l_atom_hash = {0};
    uint64_t l_atom_num = 0;
    dap_time_t l_atom_timestamp = 0;
    
    dap_chain_cell_id_t l_cell_id_union = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id_union) != 0) {
        return NULL;
    }
    
    bool l_result = dap_chain_get_atom_last_hash_num_ts(l_chain, l_cell_id_union, 
                                                         &l_atom_hash, &l_atom_num, &l_atom_timestamp);
    if (!l_result) {
        log_it(L_DEBUG, "No last atom found for chain '%s'", l_chain->name);
        Py_RETURN_NONE;
    }
    
    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "atom_hash", PyBytes_FromStringAndSize((const char *)&l_atom_hash, sizeof(dap_hash_fast_t)));
    PyDict_SetItemString(l_dict, "atom_num", PyLong_FromUnsignedLongLong(l_atom_num));
    PyDict_SetItemString(l_dict, "atom_timestamp", PyLong_FromUnsignedLongLong(l_atom_timestamp));
    
    log_it(L_DEBUG, "Retrieved last atom info for chain '%s'", l_chain->name);
    return l_dict;
}

/**
 * @brief Initialize chain subsystem (Python binding)
 */
PyObject* dap_chain_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_result = dap_chain_init();
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize chain subsystem (Python binding)
 */
PyObject* dap_chain_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_deinit();
    Py_RETURN_NONE;
}

/**
 * @brief Parse chain ID from string (Python binding)
 */
PyObject* dap_chain_id_parse_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_id_str = NULL;
    
    if (!PyArg_ParseTuple(a_args, "s", &l_id_str)) {
        return NULL;
    }
    
    dap_chain_id_t l_chain_id = {0};
    int l_result = dap_chain_id_parse(l_id_str, &l_chain_id);
    if (l_result != 0) {
        PyErr_Format(PyExc_ValueError, "Invalid chain id string: %s", l_id_str);
        return NULL;
    }
    
    return PyLong_FromUnsignedLongLong(l_chain_id.uint64);
}

/**
 * @brief Get last atom hash only (Python binding)
 */
PyObject* dap_chain_get_atom_last_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "O|O", &l_chain_capsule, &l_cell_id_obj)) {
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

    if (!cf_chain_cell_is_configured(l_chain)) {
        Py_RETURN_NONE;
    }
    
    dap_hash_fast_t l_hash = {0};
    if (!dap_chain_get_atom_last_hash(l_chain, l_cell_id, &l_hash)) {
        Py_RETURN_NONE;
    }
    
    return PyBytes_FromStringAndSize((const char *)&l_hash, sizeof(dap_hash_fast_t));
}

/**
 * @brief Get blockchain time for chain or cell (Python binding)
 */
PyObject* dap_chain_get_blockhain_time_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "O|O", &l_chain_capsule, &l_cell_id_obj)) {
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

    if (!cf_chain_cell_is_configured(l_chain)) {
        return PyLong_FromLong(-1);
    }
    
    dap_time_t l_time = dap_chain_get_blockhain_time(l_chain, l_cell_id);
    return PyLong_FromUnsignedLongLong((unsigned long long)l_time);
}

/**
 * @brief Get chain storage path (Python binding)
 */
PyObject* dap_chain_get_path_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
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
    
    const char *l_path = dap_chain_get_path(l_chain);
    if (!l_path) {
        Py_RETURN_NONE;
    }
    
    return PyUnicode_FromString(l_path);
}

/**
 * @brief Get consensus type for chain (Python binding)
 */
PyObject* dap_chain_get_cs_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
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
    
    const char *l_cs_type = dap_chain_get_cs_type(l_chain);
    if (!l_cs_type) {
        dap_chain_pvt_t *l_chain_pvt = DAP_CHAIN_PVT(l_chain);
        if (l_chain_pvt) {
            l_cs_type = l_chain_pvt->cs_type;
        }
    }
    if (!l_cs_type) {
        Py_RETURN_NONE;
    }
    
    return PyUnicode_FromString(l_cs_type);
}

/**
 * @brief Load chain from config (Python binding)
 */
PyObject* dap_chain_load_from_cfg_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name = NULL;
    unsigned long long l_net_id = 0;
    PyObject *l_config_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "sK|O", &l_net_name, &l_net_id, &l_config_obj)) {
        return NULL;
    }
    
    dap_config_t *l_config = g_config;
    if (l_config_obj && l_config_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_config_obj)) {
            PyErr_SetString(PyExc_TypeError, "Config must be a capsule or None");
            return NULL;
        }
        l_config = (dap_config_t *)PyCapsule_GetPointer(l_config_obj, "dap_config_t");
        if (!l_config) {
            PyErr_SetString(PyExc_ValueError, "Invalid config capsule");
            return NULL;
        }
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_t *l_chain = dap_chain_load_from_cfg(l_net_name, l_net_id_struct, l_config);
    if (!l_chain) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to load chain from config");
        return NULL;
    }
    
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

/**
 * @brief Dump chain info to log (Python binding)
 */
PyObject* dap_chain_info_dump_log_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
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
    
    dap_chain_info_dump_log(l_chain);
    Py_RETURN_NONE;
}

/**
 * @brief Get next generation for chain (Python binding)
 */
PyObject* dap_chain_generation_next_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
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
    
    uint16_t l_gen = dap_chain_generation_next(l_chain);
    return PyLong_FromUnsignedLong((unsigned long)l_gen);
}

/**
 * @brief Save certificate chain file (Python binding)
 */
PyObject* dap_cert_chain_file_save_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_datum_capsule = NULL;
    const char *l_net_name = NULL;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_datum_capsule, &l_net_name)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_datum_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected datum capsule");
        return NULL;
    }
    
    dap_chain_datum_t *l_datum = (dap_chain_datum_t*)PyCapsule_GetPointer(l_datum_capsule, "dap_chain_datum_t");
    if (!l_datum) {
        PyErr_SetString(PyExc_ValueError, "Invalid datum capsule");
        return NULL;
    }
    
    int l_result = dap_cert_chain_file_save(l_datum, (char *)l_net_name);
    return PyLong_FromLong(l_result);
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
        pthread_rwlock_rdlock(&l_chain->rwlock);
        for (dap_list_t *it = l_chain->atom_notifiers; it; it = it->next) {
            dap_chain_atom_notifier_t *l_notifier = (dap_chain_atom_notifier_t *)it->data;
            if (l_notifier && l_notifier->callback) {
                l_notifier->callback(l_notifier->arg, l_chain, l_cell_id, l_hash,
                                     (void *)l_atom_bytes, l_atom_size, (dap_time_t)l_atom_time);
            }
        }
        pthread_rwlock_unlock(&l_chain->rwlock);
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
        pthread_rwlock_rdlock(&l_chain->rwlock);
        for (dap_list_t *it = l_chain->datum_notifiers; it; it = it->next) {
            cf_chain_datum_notifier_t *l_notifier = (cf_chain_datum_notifier_t *)it->data;
            if (l_notifier && l_notifier->callback) {
                l_notifier->callback(l_notifier->arg, l_hash, l_atom_hash, (void *)l_datum_bytes,
                                     l_datum_size, l_ret_code, l_action, l_uid_struct);
            }
        }
        pthread_rwlock_unlock(&l_chain->rwlock);
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
        pthread_rwlock_rdlock(&l_chain->rwlock);
        for (dap_list_t *it = l_chain->datum_removed_notifiers; it; it = it->next) {
            cf_chain_datum_removed_notifier_t *l_notifier = (cf_chain_datum_removed_notifier_t *)it->data;
            if (l_notifier && l_notifier->callback) {
                l_notifier->callback(l_notifier->arg, l_hash, l_datum);
            }
        }
        pthread_rwlock_unlock(&l_chain->rwlock);
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
    
    // Increment reference counts (will be decremented on cleanup)
    Py_INCREF(l_callback);
    Py_INCREF(l_user_data);
    
    // Register C callback with SDK
    dap_chain_add_callback_datum_index_notify(l_chain, s_chain_datum_notify_callback_wrapper, NULL, l_ctx);
    
    // Register in cleanup registry
    char l_callback_id[128];
    snprintf(l_callback_id, sizeof(l_callback_id), "chain_datum_notify_%s_%p", l_chain->name, l_callback);
    cf_callbacks_registry_add(CF_CALLBACK_TYPE_CHAIN_DATUM_INDEX, l_callback, l_user_data, l_ctx, l_callback_id);
    
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
    
    // Increment reference counts (will be decremented on cleanup)
    Py_INCREF(l_callback);
    Py_INCREF(l_user_data);
    
    // Register C callback with SDK
    dap_chain_add_callback_datum_removed_from_index_notify(l_chain, s_chain_datum_removed_notify_callback_wrapper, NULL, l_ctx);
    
    // Register in cleanup registry
    char l_callback_id[128];
    snprintf(l_callback_id, sizeof(l_callback_id), "chain_datum_removed_%s_%p", l_chain->name, l_callback);
    cf_callbacks_registry_add(CF_CALLBACK_TYPE_CHAIN_DATUM_REMOVED, l_callback, l_user_data, l_ctx, l_callback_id);
    
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
    
    // Increment reference counts (will be decremented on cleanup)
    Py_INCREF(l_callback);
    Py_INCREF(l_user_data);
    
    // Register C callback with SDK
    dap_chain_atom_confirmed_notify_add(l_chain, s_chain_atom_notify_callback_wrapper, l_ctx, l_conf_cnt);
    
    // Register in cleanup registry
    char l_callback_id[128];
    snprintf(l_callback_id, sizeof(l_callback_id), "chain_atom_confirmed_%s_%p", l_chain->name, l_callback);
    cf_callbacks_registry_add(CF_CALLBACK_TYPE_CHAIN_ATOM_CONFIRMED, l_callback, l_user_data, l_ctx, l_callback_id);
    
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
    
    // Increment reference counts (will be decremented on cleanup)
    Py_INCREF(l_callback);
    Py_INCREF(l_user_data);
    
    // Register C callback with SDK
    int l_result = dap_chain_add_callback_timer(l_chain, s_chain_timer_callback_wrapper, l_ctx);
    if (l_result != 0) {
        Py_DECREF(l_callback);
        Py_DECREF(l_user_data);
        DAP_DELETE(l_ctx);
        PyErr_Format(PyExc_RuntimeError, "Failed to register timer callback: %d", l_result);
        return NULL;
    }
    
    // Register in cleanup registry
    char l_callback_id[128];
    snprintf(l_callback_id, sizeof(l_callback_id), "chain_timer_%s_%p", l_chain->name, l_callback);
    cf_callbacks_registry_add(CF_CALLBACK_TYPE_CHAIN_TIMER, l_callback, l_user_data, l_ctx, l_callback_id);
    
    log_it(L_DEBUG, "Registered timer callback for chain '%s' (ID: %s)", l_chain->name, l_callback_id);
    
    return PyLong_FromLong(0);  // Success
}

// =========================================
// BLOCK OPERATIONS
// =========================================

/**
 * @brief Create new block (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (prev_block_hash bytes or None)
 * @return PyCapsule wrapping dap_chain_block_t* or None on error
 */
PyObject* py_dap_chain_block_new(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_prev_hash_obj = NULL;
    const char *l_prev_hash_bytes = NULL;
    Py_ssize_t l_prev_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "|O", &l_prev_hash_obj)) {
        return NULL;
    }
    
    dap_chain_hash_fast_t *l_prev_hash = NULL;
    if (l_prev_hash_obj && l_prev_hash_obj != Py_None) {
        if (!PyArg_Parse(l_prev_hash_obj, "y#", &l_prev_hash_bytes, &l_prev_hash_size)) {
            PyErr_SetString(PyExc_TypeError, "prev_hash must be bytes or None");
            return NULL;
        }
        
        if (l_prev_hash_size != sizeof(dap_chain_hash_fast_t)) {
            PyErr_Format(PyExc_ValueError, "prev_hash must be exactly %zu bytes", sizeof(dap_chain_hash_fast_t));
            return NULL;
        }
        
        l_prev_hash = (dap_chain_hash_fast_t*)l_prev_hash_bytes;
    }
    
    size_t l_block_size = 0;
    dap_chain_block_t *l_block = dap_chain_block_new(l_prev_hash, &l_block_size);
    if (!l_block) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create block");
        return NULL;
    }
    
    log_it(L_DEBUG, "Created new block (size=%zu)", l_block_size);
    return PyCapsule_New(l_block, "dap_chain_block_t", NULL);
}

/**
 * @brief Add datum to block (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (block capsule, datum capsule)
 * @return New block size or None on error
 */
PyObject* py_dap_chain_block_datum_add(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_block_obj = NULL;
    PyObject *l_datum_obj = NULL;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_block_obj, &l_datum_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_block_obj) || !PyCapsule_CheckExact(l_datum_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected block and datum capsules");
        return NULL;
    }
    
    dap_chain_block_t *l_block = (dap_chain_block_t*)PyCapsule_GetPointer(l_block_obj, "dap_chain_block_t");
    dap_chain_datum_t *l_datum = (dap_chain_datum_t*)PyCapsule_GetPointer(l_datum_obj, "dap_chain_datum_t");
    
    if (!l_block || !l_datum) {
        PyErr_SetString(PyExc_ValueError, "Invalid block or datum capsule");
        return NULL;
    }
    
    size_t l_block_size = dap_chain_block_get_size(l_block);
    uint64_t l_datum_size = dap_chain_datum_size(l_datum);
    
    size_t l_new_size = dap_chain_block_datum_add(&l_block, l_block_size, l_datum, (size_t)l_datum_size);
    if (l_new_size == 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add datum to block");
        return NULL;
    }
    
    log_it(L_DEBUG, "Added datum to block (new_size=%zu)", l_new_size);
    return PyLong_FromSize_t(l_new_size);
}

/**
 * @brief Get datums from block (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (block capsule)
 * @return List of datum capsules or None on error
 */
PyObject* py_dap_chain_block_get_datums(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_block_obj = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_block_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_block_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected block capsule");
        return NULL;
    }
    
    dap_chain_block_t *l_block = (dap_chain_block_t*)PyCapsule_GetPointer(l_block_obj, "dap_chain_block_t");
    if (!l_block) {
        PyErr_SetString(PyExc_ValueError, "Invalid block capsule");
        return NULL;
    }
    
    size_t l_block_size = dap_chain_block_get_size(l_block);
    size_t l_datums_count = 0;
    
    dap_chain_datum_t **l_datums = dap_chain_block_get_datums(l_block, l_block_size, &l_datums_count);
    if (!l_datums) {
        log_it(L_DEBUG, "No datums found in block");
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = PyList_New(l_datums_count);
    if (!l_result) {
        DAP_DELETE(l_datums);
        return NULL;
    }
    
    for (size_t i = 0; i < l_datums_count; i++) {
        PyObject *l_datum_capsule = PyCapsule_New(l_datums[i], "dap_chain_datum_t", NULL);
        if (!l_datum_capsule) {
            Py_DECREF(l_result);
            DAP_DELETE(l_datums);
            return NULL;
        }
        PyList_SET_ITEM(l_result, i, l_datum_capsule);
    }
    
    DAP_DELETE(l_datums);
    log_it(L_DEBUG, "Retrieved %zu datums from block", l_datums_count);
    return l_result;
}

/**
 * @brief Add metadata to block (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (block capsule, meta_type, data bytes)
 * @return New block size or None on error
 */
PyObject* py_dap_chain_block_meta_add(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_block_obj = NULL;
    unsigned int l_meta_type = 0;
    const char *l_data_bytes = NULL;
    Py_ssize_t l_data_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "OIy#", &l_block_obj, &l_meta_type, &l_data_bytes, &l_data_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_block_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected block capsule");
        return NULL;
    }
    
    dap_chain_block_t *l_block = (dap_chain_block_t*)PyCapsule_GetPointer(l_block_obj, "dap_chain_block_t");
    if (!l_block) {
        PyErr_SetString(PyExc_ValueError, "Invalid block capsule");
        return NULL;
    }
    
    size_t l_block_size = dap_chain_block_get_size(l_block);
    
    size_t l_new_size = dap_chain_block_meta_add(&l_block, l_block_size, (uint8_t)l_meta_type, l_data_bytes, l_data_size);
    if (l_new_size == 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add metadata to block");
        return NULL;
    }
    
    log_it(L_DEBUG, "Added metadata (type=%u) to block (new_size=%zu)", l_meta_type, l_new_size);
    return PyLong_FromSize_t(l_new_size);
}

/**
 * @brief Add signature to block (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (block capsule, key capsule)
 * @return New block size or None on error
 */
PyObject* py_dap_chain_block_sign_add(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_block_obj = NULL;
    PyObject *l_key_obj = NULL;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_block_obj, &l_key_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_block_obj) || !PyCapsule_CheckExact(l_key_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected block and key capsules");
        return NULL;
    }
    
    dap_chain_block_t *l_block = (dap_chain_block_t*)PyCapsule_GetPointer(l_block_obj, "dap_chain_block_t");
    dap_enc_key_t *l_key = (dap_enc_key_t*)PyCapsule_GetPointer(l_key_obj, "dap_enc_key_t");
    
    if (!l_block || !l_key) {
        PyErr_SetString(PyExc_ValueError, "Invalid block or key capsule");
        return NULL;
    }
    
    size_t l_block_size = dap_chain_block_get_size(l_block);
    
    size_t l_new_size = dap_chain_block_sign_add(&l_block, l_block_size, l_key);
    if (l_new_size == 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to add signature to block");
        return NULL;
    }
    
    log_it(L_DEBUG, "Added signature to block (new_size=%zu)", l_new_size);
    return PyLong_FromSize_t(l_new_size);
}

// =========================================
// CHAIN CELL OPERATIONS
// =========================================

PyObject* dap_chain_cell_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_result = dap_chain_cell_init();
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_cell_open_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    const char *l_mode_str = NULL;
    
    if (!PyArg_ParseTuple(a_args, "OOs", &l_chain_capsule, &l_cell_id_obj, &l_mode_str)) {
        return NULL;
    }
    
    if (!l_mode_str || strlen(l_mode_str) != 1) {
        PyErr_SetString(PyExc_ValueError, "Mode must be a single character");
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

    if (!cf_chain_cell_is_configured(l_chain)) {
        return PyLong_FromLong(-1);
    }
    
    int l_result = dap_chain_cell_open(l_chain, l_cell_id, l_mode_str[0]);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_cell_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_capsule, &l_cell_id_obj)) {
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

    if (!cf_chain_cell_is_configured(l_chain)) {
        return PyLong_FromLong(0);
    }
    
    int l_result = dap_chain_cell_create(l_chain, l_cell_id);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_cell_find_by_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_capsule, &l_cell_id_obj)) {
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
    
    dap_chain_cell_t *l_cell = dap_chain_cell_find_by_id(l_chain, l_cell_id);
    if (!l_cell) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_cell, "dap_chain_cell_t", NULL);
}

PyObject* dap_chain_cell_capture_by_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_capsule, &l_cell_id_obj)) {
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
    
    dap_chain_cell_t *l_cell = dap_chain_cell_capture_by_id(l_chain, l_cell_id);
    if (!l_cell) {
        dap_chain_cell_remit(l_chain);
        Py_RETURN_NONE;
    }
    
    PyObject *l_capsule = PyCapsule_New(l_cell, "dap_chain_cell_t", NULL);
    if (!l_capsule) {
        dap_chain_cell_remit(l_chain);
        return NULL;
    }
    
    return l_capsule;
}

PyObject* dap_chain_cell_remit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
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
    
    dap_chain_cell_remit(l_chain);
    Py_RETURN_NONE;
}

PyObject* dap_chain_cell_close_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_capsule, &l_cell_id_obj)) {
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
    
    dap_chain_cell_close(l_chain, l_cell_id);
    Py_RETURN_NONE;
}

PyObject* dap_chain_cell_close_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
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
    
    dap_chain_cell_close_all(l_chain);
    Py_RETURN_NONE;
}

PyObject* dap_chain_cell_file_append_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    const char *l_atom_bytes = NULL;
    Py_ssize_t l_atom_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOy#", &l_chain_capsule, &l_cell_id_obj, &l_atom_bytes, &l_atom_size)) {
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
    
    char *l_atom_map = NULL;
    int l_result = dap_chain_cell_file_append(l_chain, l_cell_id, l_atom_bytes, (size_t)l_atom_size, &l_atom_map);
    if (l_atom_map) {
        DAP_DELETE(l_atom_map);
    }
    
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_cell_remove_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    int l_archivate = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOp", &l_chain_capsule, &l_cell_id_obj, &l_archivate)) {
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
    
    int l_result = dap_chain_cell_remove(l_chain, l_cell_id, l_archivate != 0);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_cell_truncate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    Py_ssize_t l_delta = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOn", &l_chain_capsule, &l_cell_id_obj, &l_delta)) {
        return NULL;
    }
    
    if (l_delta < 0) {
        PyErr_SetString(PyExc_ValueError, "Delta must be non-negative");
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
    
    int l_result = dap_chain_cell_truncate(l_chain, l_cell_id, (size_t)l_delta);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_cell_set_load_skip_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_cell_set_load_skip();
    Py_RETURN_NONE;
}

// =========================================
// CHAIN CH OPERATIONS
// =========================================

PyObject* dap_chain_ch_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_result = dap_chain_ch_init();
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_ch_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_ch_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_stream_ch_write_error_unsafe_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ch_capsule = NULL;
    unsigned long long l_net_id = 0;
    unsigned long long l_chain_id = 0;
    PyObject *l_cell_id_obj = Py_None;
    unsigned int l_error_type = 0;
    
    if (!PyArg_ParseTuple(a_args, "OKKOI", &l_ch_capsule, &l_net_id, &l_chain_id, &l_cell_id_obj, &l_error_type)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ch_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected stream channel capsule");
        return NULL;
    }
    
    dap_stream_ch_t *l_ch = (dap_stream_ch_t*)PyCapsule_GetPointer(l_ch_capsule, "dap_stream_ch_t");
    if (!l_ch) {
        PyErr_SetString(PyExc_ValueError, "Invalid stream channel capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    dap_stream_ch_write_error_unsafe(l_ch, l_net_id_struct, l_chain_id_struct, l_cell_id,
                                     (dap_chain_ch_error_type_t)l_error_type);
    Py_RETURN_NONE;
}

// =========================================
// CHAIN CH PACKET OPERATIONS
// =========================================

PyObject* dap_chain_ch_pkt_get_size_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_pkt_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_pkt_capsule)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_pkt_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain packet capsule");
        return NULL;
    }
    
    dap_chain_ch_pkt_t *l_pkt = (dap_chain_ch_pkt_t*)PyCapsule_GetPointer(l_pkt_capsule, "dap_chain_ch_pkt_t");
    if (!l_pkt) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain packet capsule");
        return NULL;
    }
    
    size_t l_size = dap_chain_ch_pkt_get_size(l_pkt);
    return PyLong_FromSize_t(l_size);
}

PyObject* dap_chain_ch_pkt_new_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_chain_id = 0;
    PyObject *l_cell_id_obj = Py_None;
    const char *l_data = NULL;
    Py_ssize_t l_data_size = 0;
    unsigned int l_version = DAP_CHAIN_CH_PKT_VERSION_CURRENT;
    
    if (!PyArg_ParseTuple(a_args, "KKOy#|I", &l_net_id, &l_chain_id, &l_cell_id_obj, &l_data, &l_data_size, &l_version)) {
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    dap_chain_ch_pkt_t *l_pkt = dap_chain_ch_pkt_new(l_net_id_struct, l_chain_id_struct, l_cell_id,
                                                     l_data, (size_t)l_data_size, (uint8_t)l_version);
    if (!l_pkt) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create chain packet");
        return NULL;
    }
    
    return PyCapsule_New(l_pkt, "dap_chain_ch_pkt_t", NULL);
}

PyObject* dap_chain_ch_pkt_write_unsafe_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ch_capsule = NULL;
    unsigned int l_type = 0;
    unsigned long long l_net_id = 0;
    unsigned long long l_chain_id = 0;
    PyObject *l_cell_id_obj = Py_None;
    unsigned int l_data_size_param = 0;
    const char *l_data = NULL;
    Py_ssize_t l_data_size = 0;
    unsigned int l_version = DAP_CHAIN_CH_PKT_VERSION_CURRENT;
    
    if (!PyArg_ParseTuple(a_args, "OIKKOIy#|I", &l_ch_capsule, &l_type, &l_net_id, &l_chain_id,
                          &l_cell_id_obj, &l_data_size_param, &l_data, &l_data_size, &l_version)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ch_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected stream channel capsule");
        return NULL;
    }
    
    dap_stream_ch_t *l_ch = (dap_stream_ch_t*)PyCapsule_GetPointer(l_ch_capsule, "dap_stream_ch_t");
    if (!l_ch) {
        PyErr_SetString(PyExc_ValueError, "Invalid stream channel capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    size_t l_payload_size = (l_data_size_param > 0 && l_data_size_param <= (unsigned int)l_data_size)
                                ? (size_t)l_data_size_param
                                : (size_t)l_data_size;
    size_t l_written = dap_chain_ch_pkt_write_unsafe(l_ch, (uint8_t)l_type, l_net_id_struct, l_chain_id_struct,
                                                     l_cell_id, l_data, l_payload_size, (uint8_t)l_version);
    return PyLong_FromSize_t(l_written);
}

PyObject* dap_chain_ch_pkt_write_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_worker_capsule = NULL;
    unsigned int l_ch_uuid = 0;
    unsigned int l_type = 0;
    unsigned long long l_net_id = 0;
    unsigned long long l_chain_id = 0;
    PyObject *l_cell_id_obj = Py_None;
    const char *l_data = NULL;
    Py_ssize_t l_data_size = 0;
    unsigned int l_version = DAP_CHAIN_CH_PKT_VERSION_CURRENT;
    
    if (!PyArg_ParseTuple(a_args, "OIIKKOy#|I", &l_worker_capsule, &l_ch_uuid, &l_type, &l_net_id, &l_chain_id,
                          &l_cell_id_obj, &l_data, &l_data_size, &l_version)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_worker_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected stream worker capsule");
        return NULL;
    }
    
    dap_stream_worker_t *l_worker = (dap_stream_worker_t*)PyCapsule_GetPointer(l_worker_capsule, "dap_stream_worker_t");
    if (!l_worker) {
        PyErr_SetString(PyExc_ValueError, "Invalid stream worker capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    size_t l_written = dap_chain_ch_pkt_write(l_worker, (dap_stream_ch_uuid_t)l_ch_uuid, (uint8_t)l_type,
                                              l_net_id_struct, l_chain_id_struct, l_cell_id,
                                              l_data, (size_t)l_data_size, (uint8_t)l_version);
    return PyLong_FromSize_t(l_written);
}

PyObject* dap_chain_ch_pkt_type_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_type = 0;
    
    if (!PyArg_ParseTuple(a_args, "I", &l_type)) {
        return NULL;
    }
    
    const char *l_str = dap_chain_ch_pkt_type_to_str((uint8_t)l_type);
    return PyUnicode_FromString(l_str ? l_str : "DAP_CHAIN_CH_PKT_TYPE_UNKNOWN");
}

// =========================================
// CHAIN NET UTILS
// =========================================

PyObject* dap_chain_net_get_default_chain_by_chain_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    unsigned int l_chain_type = 0;
    
    if (!PyArg_ParseTuple(a_args, "OI", &l_net_capsule, &l_chain_type)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = dap_chain_net_get_default_chain_by_chain_type(l_net, (dap_chain_type_t)l_chain_type);
    if (!l_chain) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

// =========================================
// CHAIN POLICY OPERATIONS
// =========================================

PyObject* dap_chain_policy_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_result = dap_chain_policy_init();
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_policy_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_policy_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_policy_create_activate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_num = 0;
    long long l_ts_start = 0;
    unsigned long long l_block_start = 0;
    unsigned long long l_chain_id = 0;
    unsigned int l_generation = 0;
    
    if (!PyArg_ParseTuple(a_args, "ILKKI", &l_num, &l_ts_start, &l_block_start, &l_chain_id, &l_generation)) {
        return NULL;
    }
    
    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    dap_chain_policy_t *l_policy = dap_chain_policy_create_activate(l_num, l_ts_start, l_block_start, l_chain_id_struct,
                                                                    (uint16_t)l_generation);
    if (!l_policy) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create activate policy");
        return NULL;
    }
    
    return PyCapsule_New(l_policy, "dap_chain_policy_t", NULL);
}

PyObject* dap_chain_policy_create_deactivate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_nums_obj = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_nums_obj)) {
        return NULL;
    }
    
    PyObject *l_seq = PySequence_Fast(l_nums_obj, "Expected a sequence of policy numbers");
    if (!l_seq) {
        return NULL;
    }
    
    Py_ssize_t l_count = PySequence_Fast_GET_SIZE(l_seq);
    if (l_count <= 0) {
        Py_DECREF(l_seq);
        PyErr_SetString(PyExc_ValueError, "Policy numbers sequence must not be empty");
        return NULL;
    }
    
    char **l_nums = DAP_NEW_Z_COUNT(char *, (size_t)l_count);
    PyObject **l_keepalive = DAP_NEW_Z_COUNT(PyObject *, (size_t)l_count);
    if (!l_nums || !l_keepalive) {
        Py_DECREF(l_seq);
        DAP_DELETE(l_nums);
        DAP_DELETE(l_keepalive);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate policy numbers array");
        return NULL;
    }
    
    for (Py_ssize_t i = 0; i < l_count; i++) {
        PyObject *l_item = PySequence_Fast_GET_ITEM(l_seq, i);
        PyObject *l_str_obj = PyObject_Str(l_item);
        if (!l_str_obj) {
            for (Py_ssize_t j = 0; j < i; j++) {
                Py_XDECREF(l_keepalive[j]);
            }
            Py_DECREF(l_seq);
            DAP_DELETE(l_nums);
            DAP_DELETE(l_keepalive);
            return NULL;
        }
        l_keepalive[i] = l_str_obj;
        const char *l_str = PyUnicode_AsUTF8(l_str_obj);
        if (!l_str) {
            for (Py_ssize_t j = 0; j <= i; j++) {
                Py_XDECREF(l_keepalive[j]);
            }
            Py_DECREF(l_seq);
            DAP_DELETE(l_nums);
            DAP_DELETE(l_keepalive);
            return NULL;
        }
        l_nums[i] = (char *)l_str;
    }
    
    dap_chain_policy_t *l_policy = dap_chain_policy_create_deactivate(l_nums, (uint32_t)l_count);
    
    for (Py_ssize_t i = 0; i < l_count; i++) {
        Py_XDECREF(l_keepalive[i]);
    }
    Py_DECREF(l_seq);
    DAP_DELETE(l_nums);
    DAP_DELETE(l_keepalive);
    
    if (!l_policy) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create deactivate policy");
        return NULL;
    }
    
    return PyCapsule_New(l_policy, "dap_chain_policy_t", NULL);
}

PyObject* dap_chain_policy_net_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    PyObject *l_config_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "K|O", &l_net_id, &l_config_obj)) {
        return NULL;
    }
    
    dap_config_t *l_config = g_config;
    if (l_config_obj && l_config_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_config_obj)) {
            PyErr_SetString(PyExc_TypeError, "Config must be a capsule or None");
            return NULL;
        }
        l_config = (dap_config_t *)PyCapsule_GetPointer(l_config_obj, "dap_config_t");
        if (!l_config) {
            PyErr_SetString(PyExc_ValueError, "Invalid config capsule");
            return NULL;
        }
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_result = dap_chain_policy_net_add(l_net_id_struct, l_config);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_policy_net_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_policy_net_purge(l_net_id_struct);
    Py_RETURN_NONE;
}

PyObject* dap_chain_policy_net_remove_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_policy_net_remove(l_net_id_struct);
    Py_RETURN_NONE;
}

PyObject* dap_chain_policy_apply_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_policy_capsule = NULL;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "OK", &l_policy_capsule, &l_net_id)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_policy_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected policy capsule");
        return NULL;
    }
    
    dap_chain_policy_t *l_policy = (dap_chain_policy_t*)PyCapsule_GetPointer(l_policy_capsule, "dap_chain_policy_t");
    if (!l_policy) {
        PyErr_SetString(PyExc_ValueError, "Invalid policy capsule");
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_result = dap_chain_policy_apply(l_policy, l_net_id_struct);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_policy_update_last_num_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned int l_num = 0;
    
    if (!PyArg_ParseTuple(a_args, "KI", &l_net_id, &l_num)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_policy_update_last_num(l_net_id_struct, l_num);
    Py_RETURN_NONE;
}

PyObject* dap_chain_policy_get_last_num_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    uint32_t l_num = dap_chain_policy_get_last_num(l_net_id_struct);
    return PyLong_FromUnsignedLong(l_num);
}

PyObject* dap_chain_policy_activate_json_collect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned int l_num = 0;
    
    if (!PyArg_ParseTuple(a_args, "KI", &l_net_id, &l_num)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_json_t *l_json = dap_chain_policy_activate_json_collect(l_net_id_struct, l_num);
    if (!l_json) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

PyObject* dap_chain_policy_json_collect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_policy_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_policy_capsule)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_policy_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected policy capsule");
        return NULL;
    }
    
    dap_chain_policy_t *l_policy = (dap_chain_policy_t*)PyCapsule_GetPointer(l_policy_capsule, "dap_chain_policy_t");
    if (!l_policy) {
        PyErr_SetString(PyExc_ValueError, "Invalid policy capsule");
        return NULL;
    }
    
    dap_json_t *l_json = dap_chain_policy_json_collect(l_policy);
    if (!l_json) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

PyObject* dap_chain_policy_list_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    int l_version = 0;
    
    if (!PyArg_ParseTuple(a_args, "Ki", &l_net_id, &l_version)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_json_t *l_json = dap_chain_policy_list(l_net_id_struct, l_version);
    if (!l_json) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

PyObject* dap_chain_policy_is_exist_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned int l_num = 0;
    
    if (!PyArg_ParseTuple(a_args, "KI", &l_net_id, &l_num)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    bool l_result = dap_chain_policy_is_exist(l_net_id_struct, l_num);
    return PyBool_FromLong(l_result ? 1 : 0);
}

PyObject* dap_chain_policy_is_activated_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned int l_num = 0;
    
    if (!PyArg_ParseTuple(a_args, "KI", &l_net_id, &l_num)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    bool l_result = dap_chain_policy_is_activated(l_net_id_struct, l_num);
    return PyBool_FromLong(l_result ? 1 : 0);
}

PyObject* dap_chain_policy_get_size_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_policy_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_policy_capsule)) {
        return NULL;
    }
    
    dap_chain_policy_t *l_policy = NULL;
    if (l_policy_capsule != Py_None) {
        if (!PyCapsule_CheckExact(l_policy_capsule)) {
            PyErr_SetString(PyExc_TypeError, "Expected policy capsule or None");
            return NULL;
        }
        l_policy = (dap_chain_policy_t*)PyCapsule_GetPointer(l_policy_capsule, "dap_chain_policy_t");
        if (!l_policy) {
            PyErr_SetString(PyExc_ValueError, "Invalid policy capsule");
            return NULL;
        }
    }
    
    size_t l_size = dap_chain_policy_get_size(l_policy);
    return PyLong_FromSize_t(l_size);
}

PyObject* dap_chain_policy_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_policy_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_policy_capsule)) {
        return NULL;
    }
    
    dap_chain_policy_t *l_policy = NULL;
    if (l_policy_capsule != Py_None) {
        if (!PyCapsule_CheckExact(l_policy_capsule)) {
            PyErr_SetString(PyExc_TypeError, "Expected policy capsule or None");
            return NULL;
        }
        l_policy = (dap_chain_policy_t*)PyCapsule_GetPointer(l_policy_capsule, "dap_chain_policy_t");
        if (!l_policy) {
            PyErr_SetString(PyExc_ValueError, "Invalid policy capsule");
            return NULL;
        }
    }
    
    const char *l_str = dap_chain_policy_to_str(l_policy);
    return PyUnicode_FromString(l_str ? l_str : "<null>");
}

// =========================================
// CHAIN SERVICE OPERATIONS
// =========================================

PyObject* dap_chain_srv_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_result = dap_chain_srv_init();
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_srv_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_srv_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_srv_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_uid = 0;
    PyObject *l_name_obj = NULL;
    PyObject *l_callbacks_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "KO|O", &l_uid, &l_name_obj, &l_callbacks_obj)) {
        return NULL;
    }
    
    const char *l_name = NULL;
    if (l_name_obj && l_name_obj != Py_None) {
        if (!PyUnicode_Check(l_name_obj)) {
            PyErr_SetString(PyExc_TypeError, "Service name must be a string or None");
            return NULL;
        }
        l_name = PyUnicode_AsUTF8(l_name_obj);
        if (!l_name) {
            return NULL;
        }
    }
    
    dap_chain_static_srv_callbacks_t *l_callbacks = NULL;
    if (l_callbacks_obj && l_callbacks_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_callbacks_obj)) {
            PyErr_SetString(PyExc_TypeError, "Callbacks must be a capsule or None");
            return NULL;
        }
        l_callbacks = (dap_chain_static_srv_callbacks_t*)PyCapsule_GetPointer(l_callbacks_obj, "dap_chain_static_srv_callbacks_t");
        if (!l_callbacks) {
            PyErr_SetString(PyExc_ValueError, "Invalid callbacks capsule");
            return NULL;
        }
    }
    
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    int l_result = dap_chain_srv_add(l_uid_struct, l_name, l_callbacks);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_srv_start_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    const char *l_name = NULL;
    PyObject *l_config_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "Ks|O", &l_net_id, &l_name, &l_config_obj)) {
        return NULL;
    }
    
    dap_config_t *l_config = g_config;
    if (l_config_obj && l_config_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_config_obj)) {
            PyErr_SetString(PyExc_TypeError, "Config must be a capsule or None");
            return NULL;
        }
        l_config = (dap_config_t *)PyCapsule_GetPointer(l_config_obj, "dap_config_t");
        if (!l_config) {
            PyErr_SetString(PyExc_ValueError, "Invalid config capsule");
            return NULL;
        }
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_result = dap_chain_srv_start(l_net_id_struct, l_name, l_config);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_srv_start_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_result = dap_chain_srv_start_all(l_net_id_struct);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_srv_delete_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_uid = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_uid)) {
        return NULL;
    }
    
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    int l_result = dap_chain_srv_delete(l_uid_struct);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_srv_get_internal_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_uid = 0;
    
    if (!PyArg_ParseTuple(a_args, "KK", &l_net_id, &l_uid)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    void *l_internal = dap_chain_srv_get_internal(l_net_id_struct, l_uid_struct);
    if (!l_internal) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_internal, "dap_chain_srv_internal_t", NULL);
}

PyObject* dap_chain_srv_get_uid_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name = NULL;
    
    if (!PyArg_ParseTuple(a_args, "s", &l_name)) {
        return NULL;
    }
    
    dap_chain_srv_uid_t l_uid = dap_chain_srv_get_uid_by_name(l_name);
    return PyLong_FromUnsignedLongLong(l_uid.uint64);
}

PyObject* dap_chain_srv_count_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    size_t l_count = dap_chain_srv_count(l_net_id_struct);
    return PyLong_FromSize_t(l_count);
}

PyObject* dap_chain_srv_list_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_list_t *l_list = dap_chain_srv_list(l_net_id_struct);
    if (!l_list) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

PyObject* dap_chain_srv_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_uid = 0;
    
    if (!PyArg_ParseTuple(a_args, "KK", &l_net_id, &l_uid)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    dap_list_t *l_services = dap_chain_srv_list(l_net_id_struct);
    bool l_found = false;
    for (dap_list_t *it = l_services; it; it = it->next) {
        dap_chain_srv_uid_t *l_item_uid = it->data;
        if (l_item_uid && l_item_uid->uint64 == l_uid_struct.uint64) {
            l_found = true;
            break;
        }
    }
    dap_list_free_full(l_services, NULL);
    if (!l_found) {
        return PyLong_FromLong(0);
    }
    int l_result = dap_chain_srv_purge(l_net_id_struct, l_uid_struct);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_srv_purge_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_result = dap_chain_srv_purge_all(l_net_id_struct);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_srv_hardfork_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_hardfork_state_t *l_state = dap_chain_srv_hardfork_all(l_net_id_struct);
    if (!l_state) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_state, "dap_chain_srv_hardfork_state_t", NULL);
}

PyObject* dap_chain_srv_load_state_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_uid = 0;
    const char *l_state = NULL;
    Py_ssize_t l_state_size = 0;
    unsigned int l_state_count = 0;
    
    if (!PyArg_ParseTuple(a_args, "KKy#I", &l_net_id, &l_uid, &l_state, &l_state_size, &l_state_count)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    int l_result = dap_chain_srv_load_state(l_net_id_struct, l_uid_struct, (byte_t *)l_state,
                                            (uint64_t)l_state_size, l_state_count);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_srv_hardfork_complete_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_hardfork_complete_all(l_net_id_struct);
    Py_RETURN_NONE;
}

PyObject* dap_chain_srv_event_verify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_uid = 0;
    const char *l_event_group = NULL;
    int l_event_type = 0;
    const char *l_event_data = NULL;
    Py_ssize_t l_event_data_size = 0;
    PyObject *l_hash_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "KKsiy#O", &l_net_id, &l_uid, &l_event_group, &l_event_type,
                          &l_event_data, &l_event_data_size, &l_hash_obj)) {
        return NULL;
    }
    
    dap_hash_fast_t *l_hash = NULL;
    if (cf_chain_parse_hash(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    int l_result = dap_chain_srv_event_verify(l_net_id_struct, l_uid_struct, l_event_group, l_event_type,
                                              (void *)l_event_data, (size_t)l_event_data_size, l_hash);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_srv_decree_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_uid = 0;
    int l_apply = 0;
    PyObject *l_params_obj = Py_None;
    Py_ssize_t l_params_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "KKpOn", &l_net_id, &l_uid, &l_apply, &l_params_obj, &l_params_size)) {
        return NULL;
    }
    
    dap_tsd_t *l_params = NULL;
    if (l_params_obj && l_params_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_params_obj)) {
            PyErr_SetString(PyExc_TypeError, "Params must be a capsule or None");
            return NULL;
        }
        l_params = (dap_tsd_t *)PyCapsule_GetPointer(l_params_obj, "dap_tsd_t");
        if (!l_params) {
            PyErr_SetString(PyExc_ValueError, "Invalid params capsule");
            return NULL;
        }
    }
    
    if (l_params_size < 0) {
        PyErr_SetString(PyExc_ValueError, "Params size must be non-negative");
        return NULL;
    }
    if ((!l_params || l_params_obj == Py_None) && l_params_size > 0) {
        PyErr_SetString(PyExc_ValueError, "Params size must be 0 when params is None");
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    int l_result = dap_chain_srv_decree(l_net_id_struct, l_uid_struct, l_apply != 0, l_params, (size_t)l_params_size);
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_srv_get_fees_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_json_t *l_json = dap_chain_srv_get_fees(l_net_id_struct);
    if (!l_json) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

PyObject* dap_chain_srv_fee_type_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_fee_type = 0;
    
    if (!PyArg_ParseTuple(a_args, "I", &l_fee_type)) {
        return NULL;
    }
    
    const char *l_str = dap_chain_srv_fee_type_to_str((dap_chain_srv_fee_type_t)l_fee_type);
    return PyUnicode_FromString(l_str ? l_str : "UNKNOWN");
}

// =========================================
// MODULE INITIALIZATION
// =========================================
// This function registers all chain functions with the Python module
int cellframe_chain_init(PyObject *module) {
    // Define chain method definitions
    static PyMethodDef chain_methods[] = {
        // Chain address functions
        {"dap_chain_addr_from_str", py_dap_chain_addr_from_str, METH_VARARGS,
         "Create chain address from string"},
        {"dap_chain_addr_get_net_id", py_dap_chain_addr_get_net_id, METH_VARARGS,
         "Get network ID from chain address"},
        {"dap_chain_addr_to_str", py_dap_chain_addr_to_str, METH_VARARGS,
         "Convert chain address to string"},
        {"dap_chain_addr_is_blank", py_dap_chain_addr_is_blank, METH_VARARGS,
         "Check if chain address is blank"},
        {"dap_chain_addr_check_sum", py_dap_chain_addr_check_sum, METH_VARARGS,
         "Check chain address checksum"},
        
        // Chain operations
        {"dap_chain_atom_create", py_dap_chain_atom_create, METH_VARARGS,
         "Create chain atom"},
        {"dap_chain_atom_get_data", py_dap_chain_atom_get_data, METH_VARARGS,
         "Get atom data from atom pointer"},
        
        // Mempool operations
        {"dap_chain_mempool_by_chain_name", py_dap_chain_mempool_by_chain_name, METH_VARARGS,
         "Get mempool by chain name"},
        {"dap_chain_mempool_tx_get_by_hash", py_dap_chain_mempool_tx_get_by_hash, METH_VARARGS,
         "Get transaction from mempool by hash"},
        
        // Chain atom operations
        {"chain_get_atom_by_hash", (PyCFunction)dap_chain_get_atom_by_hash_py, METH_VARARGS,
         "Get atom from chain by hash"},
        {"chain_get_atom_last", (PyCFunction)dap_chain_get_atom_last_py, METH_VARARGS,
         "Get last atom hash, number and timestamp"},
        
        // Chain management operations
        {"chain_load_all", (PyCFunction)dap_chain_load_all_py, METH_VARARGS,
         "Load all chain data from storage"},
        {"chain_has_file_store", (PyCFunction)dap_chain_has_file_store_py, METH_VARARGS,
         "Check if chain has file store"},
        {"chain_purge", (PyCFunction)dap_chain_purge_py, METH_VARARGS,
         "Purge chain data"},
        {"chain_atom_save", (PyCFunction)dap_chain_atom_save_py, METH_VARARGS,
         "Save atom to chain"},
        {"chain_add_callback_notify", (PyCFunction)dap_chain_add_callback_notify_py, METH_VARARGS,
         "Add callback notify for chain"},
        
        // Block operations
        {"dap_chain_block_new", py_dap_chain_block_new, METH_VARARGS,
         "Create new block"},
        {"dap_chain_block_datum_add", py_dap_chain_block_datum_add, METH_VARARGS,
         "Add datum to block"},
        {"dap_chain_block_get_datums", py_dap_chain_block_get_datums, METH_VARARGS,
         "Get datums from block"},
        {"dap_chain_block_meta_add", py_dap_chain_block_meta_add, METH_VARARGS,
         "Add metadata to block"},
        {"dap_chain_block_sign_add", py_dap_chain_block_sign_add, METH_VARARGS,
         "Add signature to block"},
        
        // Chain lifecycle operations
        {"chain_create", (PyCFunction)dap_chain_create_py, METH_VARARGS,
         "Create a new chain"},
        {"chain_delete", (PyCFunction)dap_chain_delete_py, METH_VARARGS,
         "Delete a chain"},
        
        // Chain configuration
        {"chain_set_cs_type", (PyCFunction)dap_chain_set_cs_type_py, METH_VARARGS,
         "Set consensus type for chain"},
        {"chain_set_cs_name", (PyCFunction)dap_chain_set_cs_name_py, METH_VARARGS,
         "Set consensus name for chain"},
        
        // Chain advanced operations
        {"chain_atom_add_from_threshold", (PyCFunction)dap_chain_atom_add_from_threshold_py, METH_VARARGS,
         "Add atom from threshold to chain"},
        
        // Chain lookup and utility
        {"chain_find_by_id", (PyCFunction)dap_chain_find_by_id_py, METH_VARARGS,
         "Find chain by network ID and chain ID"},
        {"chain_datum_type_supported_by_chain", (PyCFunction)dap_chain_datum_type_supported_by_chain_py, METH_VARARGS,
         "Check if datum type is supported by chain"},
        
        // Chain generation management
        {"chain_generation_banned", (PyCFunction)dap_chain_generation_banned_py, METH_VARARGS,
         "Check if chain generation is banned"},
        {"chain_generation_ban", (PyCFunction)dap_chain_generation_ban_py, METH_VARARGS,
         "Ban a chain generation"},
        
        // Chain extended operations
        {"chain_get_atom_last_hash_num_ts", (PyCFunction)dap_chain_get_atom_last_hash_num_ts_py, METH_VARARGS,
         "Get last atom hash, number and timestamp with full details"},
        {"chain_add_callback_datum_index_notify", (PyCFunction)dap_chain_add_callback_datum_index_notify_py, METH_VARARGS,
         "Add callback for datum index notifications"},
        {"chain_add_callback_datum_removed_from_index_notify", (PyCFunction)dap_chain_add_callback_datum_removed_from_index_notify_py, METH_VARARGS,
         "Add callback for datum removed from index notifications"},
        {"chain_atom_confirmed_notify_add", (PyCFunction)dap_chain_atom_confirmed_notify_add_py, METH_VARARGS,
         "Add callback for atom confirmed notifications"},
        {"chain_add_callback_timer", (PyCFunction)dap_chain_add_callback_timer_py, METH_VARARGS,
         "Add timer callback for blockchain"},
        
        // Chain SDK core operations
        {"dap_chain_init", (PyCFunction)dap_chain_init_py, METH_NOARGS,
         "Initialize chain subsystem"},
        {"dap_chain_deinit", (PyCFunction)dap_chain_deinit_py, METH_NOARGS,
         "Deinitialize chain subsystem"},
        {"dap_chain_id_parse", (PyCFunction)dap_chain_id_parse_py, METH_VARARGS,
         "Parse chain ID from string"},
        {"dap_chain_load_from_cfg", (PyCFunction)dap_chain_load_from_cfg_py, METH_VARARGS,
         "Load chain from config"},
        {"dap_chain_info_dump_log", (PyCFunction)dap_chain_info_dump_log_py, METH_VARARGS,
         "Dump chain info to log"},
        {"dap_chain_get_path", (PyCFunction)dap_chain_get_path_py, METH_VARARGS,
         "Get chain storage path"},
        {"dap_chain_get_cs_type", (PyCFunction)dap_chain_get_cs_type_py, METH_VARARGS,
         "Get chain consensus type"},
        {"dap_chain_get_blockhain_time", (PyCFunction)dap_chain_get_blockhain_time_py, METH_VARARGS,
         "Get blockchain time for chain or cell"},
        {"dap_chain_get_atom_last_hash", (PyCFunction)dap_chain_get_atom_last_hash_py, METH_VARARGS,
         "Get last atom hash"},
        {"dap_chain_generation_next", (PyCFunction)dap_chain_generation_next_py, METH_VARARGS,
         "Get next chain generation"},
        {"dap_cert_chain_file_save", (PyCFunction)dap_cert_chain_file_save_py, METH_VARARGS,
         "Save certificate chain file"},
        
        // Chain SDK notify triggers
        {"dap_chain_atom_notify", (PyCFunction)dap_chain_atom_notify_py, METH_VARARGS,
         "Notify atom addition"},
        {"dap_chain_atom_remove_notify", (PyCFunction)dap_chain_atom_remove_notify_py, METH_VARARGS,
         "Notify atom removal"},
        {"dap_chain_datum_notify", (PyCFunction)dap_chain_datum_notify_py, METH_VARARGS,
         "Notify datum addition to index"},
        {"dap_chain_datum_removed_notify", (PyCFunction)dap_chain_datum_removed_notify_py, METH_VARARGS,
         "Notify datum removal from index"},
        
        // Chain SDK aliases for legacy chain_* names
        {"dap_chain_get_atom_by_hash", (PyCFunction)dap_chain_get_atom_by_hash_py, METH_VARARGS,
         "Get atom from chain by hash"},
        {"dap_chain_get_atom_last_hash_num_ts", (PyCFunction)dap_chain_get_atom_last_hash_num_ts_py, METH_VARARGS,
         "Get last atom hash, number and timestamp"},
        {"dap_chain_load_all", (PyCFunction)dap_chain_load_all_py, METH_VARARGS,
         "Load all chain data from storage"},
        {"dap_chain_has_file_store", (PyCFunction)dap_chain_has_file_store_py, METH_VARARGS,
         "Check if chain has file store"},
        {"dap_chain_purge", (PyCFunction)dap_chain_purge_py, METH_VARARGS,
         "Purge chain data"},
        {"dap_chain_atom_save", (PyCFunction)dap_chain_atom_save_py, METH_VARARGS,
         "Save atom to chain"},
        {"dap_chain_add_callback_notify", (PyCFunction)dap_chain_add_callback_notify_py, METH_VARARGS,
         "Add callback notify for chain"},
        {"dap_chain_create", (PyCFunction)dap_chain_create_py, METH_VARARGS,
         "Create a new chain"},
        {"dap_chain_delete", (PyCFunction)dap_chain_delete_py, METH_VARARGS,
         "Delete a chain"},
        {"dap_chain_set_cs_type", (PyCFunction)dap_chain_set_cs_type_py, METH_VARARGS,
         "Set consensus type for chain"},
        {"dap_chain_set_cs_name", (PyCFunction)dap_chain_set_cs_name_py, METH_VARARGS,
         "Set consensus name for chain"},
        {"dap_chain_atom_add_from_threshold", (PyCFunction)dap_chain_atom_add_from_threshold_py, METH_VARARGS,
         "Add atom from threshold to chain"},
        {"dap_chain_find_by_id", (PyCFunction)dap_chain_find_by_id_py, METH_VARARGS,
         "Find chain by network ID and chain ID"},
        {"dap_chain_datum_type_supported_by_chain", (PyCFunction)dap_chain_datum_type_supported_by_chain_py, METH_VARARGS,
         "Check if datum type is supported by chain"},
        {"dap_chain_generation_banned", (PyCFunction)dap_chain_generation_banned_py, METH_VARARGS,
         "Check if chain generation is banned"},
        {"dap_chain_generation_ban", (PyCFunction)dap_chain_generation_ban_py, METH_VARARGS,
         "Ban a chain generation"},
        {"dap_chain_add_callback_datum_index_notify", (PyCFunction)dap_chain_add_callback_datum_index_notify_py, METH_VARARGS,
         "Add callback for datum index notifications"},
        {"dap_chain_add_callback_datum_removed_from_index_notify", (PyCFunction)dap_chain_add_callback_datum_removed_from_index_notify_py, METH_VARARGS,
         "Add callback for datum removed from index notifications"},
        {"dap_chain_atom_confirmed_notify_add", (PyCFunction)dap_chain_atom_confirmed_notify_add_py, METH_VARARGS,
         "Add callback for atom confirmed notifications"},
        {"dap_chain_add_callback_timer", (PyCFunction)dap_chain_add_callback_timer_py, METH_VARARGS,
         "Add timer callback for blockchain"},
        
        // Chain cell operations
        {"dap_chain_cell_init", (PyCFunction)dap_chain_cell_init_py, METH_NOARGS,
         "Initialize chain cells"},
        {"dap_chain_cell_open", (PyCFunction)dap_chain_cell_open_py, METH_VARARGS,
         "Open chain cell"},
        {"dap_chain_cell_create", (PyCFunction)dap_chain_cell_create_py, METH_VARARGS,
         "Create chain cell"},
        {"dap_chain_cell_find_by_id", (PyCFunction)dap_chain_cell_find_by_id_py, METH_VARARGS,
         "Find chain cell by ID"},
        {"dap_chain_cell_capture_by_id", (PyCFunction)dap_chain_cell_capture_by_id_py, METH_VARARGS,
         "Capture chain cell by ID"},
        {"dap_chain_cell_remit", (PyCFunction)dap_chain_cell_remit_py, METH_VARARGS,
         "Remit chain cell lock"},
        {"dap_chain_cell_close", (PyCFunction)dap_chain_cell_close_py, METH_VARARGS,
         "Close chain cell"},
        {"dap_chain_cell_close_all", (PyCFunction)dap_chain_cell_close_all_py, METH_VARARGS,
         "Close all chain cells"},
        {"dap_chain_cell_file_append", (PyCFunction)dap_chain_cell_file_append_py, METH_VARARGS,
         "Append atom to chain cell file"},
        {"dap_chain_cell_remove", (PyCFunction)dap_chain_cell_remove_py, METH_VARARGS,
         "Remove chain cell"},
        {"dap_chain_cell_truncate", (PyCFunction)dap_chain_cell_truncate_py, METH_VARARGS,
         "Truncate chain cell file"},
        {"dap_chain_cell_set_load_skip", (PyCFunction)dap_chain_cell_set_load_skip_py, METH_NOARGS,
         "Set chain cell load skip"},
        
        // Chain ch operations
        {"dap_chain_ch_init", (PyCFunction)dap_chain_ch_init_py, METH_NOARGS,
         "Initialize chain channel"},
        {"dap_chain_ch_deinit", (PyCFunction)dap_chain_ch_deinit_py, METH_NOARGS,
         "Deinitialize chain channel"},
        {"dap_stream_ch_write_error_unsafe", (PyCFunction)dap_stream_ch_write_error_unsafe_py, METH_VARARGS,
         "Write chain channel error (unsafe)"},
        
        // Chain ch packet operations
        {"dap_chain_ch_pkt_get_size", (PyCFunction)dap_chain_ch_pkt_get_size_py, METH_VARARGS,
         "Get chain channel packet size"},
        {"dap_chain_ch_pkt_new", (PyCFunction)dap_chain_ch_pkt_new_py, METH_VARARGS,
         "Create chain channel packet"},
        {"dap_chain_ch_pkt_type_to_str", (PyCFunction)dap_chain_ch_pkt_type_to_str_py, METH_VARARGS,
         "Get chain channel packet type string"},
        {"dap_chain_ch_pkt_write", (PyCFunction)dap_chain_ch_pkt_write_py, METH_VARARGS,
         "Write chain channel packet"},
        {"dap_chain_ch_pkt_write_unsafe", (PyCFunction)dap_chain_ch_pkt_write_unsafe_py, METH_VARARGS,
         "Write chain channel packet (unsafe)"},
        
        // Chain net utils
        {"dap_chain_net_get_default_chain_by_chain_type", (PyCFunction)dap_chain_net_get_default_chain_by_chain_type_py, METH_VARARGS,
         "Get default chain by type"},
        
        // Chain policy operations
        {"dap_chain_policy_init", (PyCFunction)dap_chain_policy_init_py, METH_NOARGS,
         "Initialize chain policy subsystem"},
        {"dap_chain_policy_deinit", (PyCFunction)dap_chain_policy_deinit_py, METH_NOARGS,
         "Deinitialize chain policy subsystem"},
        {"dap_chain_policy_create_activate", (PyCFunction)dap_chain_policy_create_activate_py, METH_VARARGS,
         "Create activate policy"},
        {"dap_chain_policy_create_deactivate", (PyCFunction)dap_chain_policy_create_deactivate_py, METH_VARARGS,
         "Create deactivate policy"},
        {"dap_chain_policy_net_add", (PyCFunction)dap_chain_policy_net_add_py, METH_VARARGS,
         "Add policy net"},
        {"dap_chain_policy_net_purge", (PyCFunction)dap_chain_policy_net_purge_py, METH_VARARGS,
         "Purge policy net"},
        {"dap_chain_policy_net_remove", (PyCFunction)dap_chain_policy_net_remove_py, METH_VARARGS,
         "Remove policy net"},
        {"dap_chain_policy_apply", (PyCFunction)dap_chain_policy_apply_py, METH_VARARGS,
         "Apply policy"},
        {"dap_chain_policy_update_last_num", (PyCFunction)dap_chain_policy_update_last_num_py, METH_VARARGS,
         "Update policy last number"},
        {"dap_chain_policy_get_last_num", (PyCFunction)dap_chain_policy_get_last_num_py, METH_VARARGS,
         "Get policy last number"},
        {"dap_chain_policy_activate_json_collect", (PyCFunction)dap_chain_policy_activate_json_collect_py, METH_VARARGS,
         "Collect activate policy JSON"},
        {"dap_chain_policy_json_collect", (PyCFunction)dap_chain_policy_json_collect_py, METH_VARARGS,
         "Collect policy JSON"},
        {"dap_chain_policy_list", (PyCFunction)dap_chain_policy_list_py, METH_VARARGS,
         "List policies as JSON"},
        {"dap_chain_policy_is_exist", (PyCFunction)dap_chain_policy_is_exist_py, METH_VARARGS,
         "Check if policy exists"},
        {"dap_chain_policy_is_activated", (PyCFunction)dap_chain_policy_is_activated_py, METH_VARARGS,
         "Check if policy is activated"},
        {"dap_chain_policy_get_size", (PyCFunction)dap_chain_policy_get_size_py, METH_VARARGS,
         "Get policy size"},
        {"dap_chain_policy_to_str", (PyCFunction)dap_chain_policy_to_str_py, METH_VARARGS,
         "Get policy type string"},
        
        // Chain service operations
        {"dap_chain_srv_init", (PyCFunction)dap_chain_srv_init_py, METH_NOARGS,
         "Initialize chain services"},
        {"dap_chain_srv_deinit", (PyCFunction)dap_chain_srv_deinit_py, METH_NOARGS,
         "Deinitialize chain services"},
        {"dap_chain_srv_add", (PyCFunction)dap_chain_srv_add_py, METH_VARARGS,
         "Add chain service"},
        {"dap_chain_srv_start", (PyCFunction)dap_chain_srv_start_py, METH_VARARGS,
         "Start chain service"},
        {"dap_chain_srv_start_all", (PyCFunction)dap_chain_srv_start_all_py, METH_VARARGS,
         "Start all chain services"},
        {"dap_chain_srv_delete", (PyCFunction)dap_chain_srv_delete_py, METH_VARARGS,
         "Delete chain service"},
        {"dap_chain_srv_get_internal", (PyCFunction)dap_chain_srv_get_internal_py, METH_VARARGS,
         "Get chain service internal pointer"},
        {"dap_chain_srv_get_uid_by_name", (PyCFunction)dap_chain_srv_get_uid_by_name_py, METH_VARARGS,
         "Get chain service UID by name"},
        {"dap_chain_srv_count", (PyCFunction)dap_chain_srv_count_py, METH_VARARGS,
         "Get chain services count"},
        {"dap_chain_srv_list", (PyCFunction)dap_chain_srv_list_py, METH_VARARGS,
         "List chain services"},
        {"dap_chain_srv_purge", (PyCFunction)dap_chain_srv_purge_py, METH_VARARGS,
         "Purge chain service"},
        {"dap_chain_srv_purge_all", (PyCFunction)dap_chain_srv_purge_all_py, METH_VARARGS,
         "Purge all chain services"},
        {"dap_chain_srv_hardfork_all", (PyCFunction)dap_chain_srv_hardfork_all_py, METH_VARARGS,
         "Prepare hardfork state for all services"},
        {"dap_chain_srv_load_state", (PyCFunction)dap_chain_srv_load_state_py, METH_VARARGS,
         "Load hardfork state for service"},
        {"dap_chain_srv_hardfork_complete_all", (PyCFunction)dap_chain_srv_hardfork_complete_all_py, METH_VARARGS,
         "Complete hardfork for all services"},
        {"dap_chain_srv_event_verify", (PyCFunction)dap_chain_srv_event_verify_py, METH_VARARGS,
         "Verify chain service event"},
        {"dap_chain_srv_decree", (PyCFunction)dap_chain_srv_decree_py, METH_VARARGS,
         "Apply chain service decree"},
        {"dap_chain_srv_get_fees", (PyCFunction)dap_chain_srv_get_fees_py, METH_VARARGS,
         "Get chain services fees"},
        {"dap_chain_srv_fee_type_to_str", (PyCFunction)dap_chain_srv_fee_type_to_str_py, METH_VARARGS,
         "Get chain service fee type string"},
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    
    // Add all methods to the module
    for (PyMethodDef *method = chain_methods; method->ml_name != NULL; method++) {
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

 
