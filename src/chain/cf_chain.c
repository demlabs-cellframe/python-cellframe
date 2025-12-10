#include "cellframe.h"
#include "dap_chain_common.h"
#include "dap_chain_net.h"
#include "dap_chain_mempool.h"
#include "dap_chain_block.h"
#include "dap_chain_datum.h"
#include <string.h>
#include <stdlib.h>

// Chain type - to be implemented
PyTypeObject PyCellframeChainType = {0};

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
 */
dap_chain_datum_t* dap_chain_mempool_tx_get_by_hash(dap_chain_t *a_chain, const char *a_tx_hash) {
    if (!a_chain || !a_tx_hash) {
        return NULL;
    }
    
    return dap_chain_mempool_datum_get(a_chain, a_tx_hash);
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
    PyObject *l_cell_id_dict = NULL;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_capsule, &l_cell_id_dict)) {
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
    
    // Extract cell_id from dict
    dap_chain_cell_id_t l_cell_id = {0};
    if (l_cell_id_dict && l_cell_id_dict != Py_None) {
        PyObject *l_uint64_obj = PyDict_GetItemString(l_cell_id_dict, "uint64");
        if (l_uint64_obj) {
            l_cell_id.uint64 = PyLong_AsUnsignedLongLong(l_uint64_obj);
        }
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
    PyObject *l_cell_id_dict = NULL;
    const char *l_atom_bytes = NULL;
    Py_ssize_t l_atom_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOy#", &l_chain_capsule, &l_cell_id_dict, &l_atom_bytes, &l_atom_size)) {
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
    
    // Extract cell_id from dict
    dap_chain_cell_id_t l_cell_id = {0};
    if (l_cell_id_dict && l_cell_id_dict != Py_None) {
        PyObject *l_uint64_obj = PyDict_GetItemString(l_cell_id_dict, "uint64");
        if (l_uint64_obj) {
            l_cell_id.uint64 = PyLong_AsUnsignedLongLong(l_uint64_obj);
        }
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
    
    // Note: Callback registration requires Python callback wrapper (similar to Network API)
    // For now, log a warning that this functionality requires extended implementation
    log_it(L_WARNING, "Chain callback registration not fully implemented yet - requires Python callback wrapper");
    PyErr_SetString(PyExc_NotImplementedError, "Chain callback registration not yet fully implemented");
    
    return NULL;
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
    
    dap_chain_delete(l_chain);
    
    // Invalidate the capsule pointer
    if (PyCapsule_SetPointer(l_chain_capsule, NULL) != 0) {
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
    PyObject *l_chain_obj;
    unsigned long long l_cell_id;
    
    if (!PyArg_ParseTuple(a_args, "OK", &l_chain_obj, &l_cell_id)) {
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
    
    dap_chain_cell_id_t l_cell_id_union = {.uint64 = l_cell_id};
    
    bool l_result = dap_chain_get_atom_last_hash_num_ts(l_chain, l_cell_id_union, 
                                                         &l_atom_hash, &l_atom_num, &l_atom_timestamp);
    if (!l_result) {
        log_it(L_DEBUG, "No last atom found for chain '%s' cell %llu", l_chain->name, l_cell_id);
        Py_RETURN_NONE;
    }
    
    PyObject *l_dict = PyDict_New();
    PyDict_SetItemString(l_dict, "atom_hash", PyBytes_FromStringAndSize((const char *)&l_atom_hash, sizeof(dap_hash_fast_t)));
    PyDict_SetItemString(l_dict, "atom_num", PyLong_FromUnsignedLongLong(l_atom_num));
    PyDict_SetItemString(l_dict, "atom_timestamp", PyLong_FromUnsignedLongLong(l_atom_timestamp));
    
    log_it(L_DEBUG, "Retrieved last atom info for chain '%s' cell %llu", l_chain->name, l_cell_id);
    return l_dict;
}

/**
 * @brief Add callback for datum index notifications
 * @note Callback functionality requires Python wrapper - stub implementation
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return None
 */
PyObject* dap_chain_add_callback_datum_index_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    // TODO: Implement full Python callback wrapper with GIL management
    // For now, just a stub that logs the request
    log_it(L_INFO, "Add datum index notify callback for chain '%s' (stub - callback not yet implemented)", l_chain->name);
    
    Py_RETURN_NONE;
}

/**
 * @brief Add callback for datum removed from index notifications
 * @note Callback functionality requires Python wrapper - stub implementation
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return None
 */
PyObject* dap_chain_add_callback_datum_removed_from_index_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    // TODO: Implement full Python callback wrapper with GIL management
    log_it(L_INFO, "Add datum removed notify callback for chain '%s' (stub - callback not yet implemented)", l_chain->name);
    
    Py_RETURN_NONE;
}

/**
 * @brief Add callback for atom confirmed notifications
 * @note Callback functionality requires Python wrapper - stub implementation
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, conf_cnt)
 * @return None
 */
PyObject* dap_chain_atom_confirmed_notify_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    unsigned long long l_conf_cnt = 1;
    
    if (!PyArg_ParseTuple(a_args, "O|K", &l_chain_obj, &l_conf_cnt)) {
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
    
    // TODO: Implement full Python callback wrapper with GIL management
    log_it(L_INFO, "Add atom confirmed notify callback for chain '%s' (conf_cnt=%llu, stub - callback not yet implemented)", 
           l_chain->name, l_conf_cnt);
    
    Py_RETURN_NONE;
}

/**
 * @brief Add timer callback for blockchain
 * @note Callback functionality requires Python wrapper - stub implementation
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Integer result code
 */
PyObject* dap_chain_add_callback_timer_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
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
    
    // TODO: Implement full Python callback wrapper with GIL management
    log_it(L_INFO, "Add timer callback for chain '%s' (stub - callback not yet implemented)", l_chain->name);
    
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
         "Add callback notify for chain (not yet fully implemented)"},
        
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
         "Add callback for datum index notifications (stub)"},
        {"chain_add_callback_datum_removed_from_index_notify", (PyCFunction)dap_chain_add_callback_datum_removed_from_index_notify_py, METH_VARARGS,
         "Add callback for datum removed from index notifications (stub)"},
        {"chain_atom_confirmed_notify_add", (PyCFunction)dap_chain_atom_confirmed_notify_add_py, METH_VARARGS,
         "Add callback for atom confirmed notifications (stub)"},
        {"chain_add_callback_timer", (PyCFunction)dap_chain_add_callback_timer_py, METH_VARARGS,
         "Add timer callback for blockchain (stub)"},
        
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

 