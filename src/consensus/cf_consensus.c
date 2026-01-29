#include "cellframe.h"
#include "dap_chain_cs.h"
#include "dap_chain_type_dag_poa.h"
#include "dap_chain_cs_esbocs.h"
#include "dap_chain_cs_esbocs_decree.h"
#include "dap_chain_validator_api.h"
#include "dap_chain.h"
#include "dap_chain_datum.h"
#include "dap_config.h"
#include "dap_json.h"
#include <string.h>

#define LOG_TAG "python_cellframe_consensus"

// =============================================================================
// INTERNAL HELPERS
// =============================================================================

static int cf_consensus_parse_hash_fast_optional(PyObject *obj, dap_hash_fast_t *out, dap_hash_fast_t **out_ptr) {
    if (out_ptr) {
        *out_ptr = NULL;
    }
    if (!obj || obj == Py_None) {
        return 0;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_hash_fast_t *ptr = (dap_hash_fast_t *)PyCapsule_GetPointer(obj, "dap_hash_fast_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_hash_fast_t capsule");
            return -1;
        }
        *out = *ptr;
        if (out_ptr) {
            *out_ptr = out;
        }
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_hash_fast_t)) {
            PyErr_Format(PyExc_ValueError, "hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_hash_fast_t));
        if (out_ptr) {
            *out_ptr = out;
        }
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *hash_str = PyUnicode_AsUTF8(obj);
        if (!hash_str) {
            return -1;
        }
        if (dap_chain_hash_fast_from_str(hash_str, out) != 0) {
            PyErr_SetString(PyExc_ValueError, "Invalid hash string");
            return -1;
        }
        if (out_ptr) {
            *out_ptr = out;
        }
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "hash must be bytes, str, or dap_hash_fast_t capsule");
    return -1;
}

static int cf_consensus_parse_hash_fast_required(PyObject *obj, dap_hash_fast_t *out) {
    dap_hash_fast_t *out_ptr = NULL;
    if (cf_consensus_parse_hash_fast_optional(obj, out, &out_ptr) != 0) {
        return -1;
    }
    if (!out_ptr) {
        PyErr_SetString(PyExc_TypeError, "hash value is required");
        return -1;
    }
    return 0;
}

static int cf_consensus_parse_chain_addr(PyObject *obj, dap_chain_addr_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "address value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_chain_addr_t *ptr = (dap_chain_addr_t *)PyCapsule_GetPointer(obj, "dap_chain_addr_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_chain_addr_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_chain_addr_t)) {
            PyErr_Format(PyExc_ValueError, "address must be exactly %zu bytes", sizeof(dap_chain_addr_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_chain_addr_t));
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *addr_str = PyUnicode_AsUTF8(obj);
        if (!addr_str) {
            return -1;
        }
        dap_chain_addr_t *addr = dap_chain_addr_from_str(addr_str);
        if (!addr) {
            PyErr_SetString(PyExc_ValueError, "Invalid address string");
            return -1;
        }
        *out = *addr;
        DAP_DELETE(addr);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "address must be bytes, str, or dap_chain_addr_t capsule");
    return -1;
}

static int cf_consensus_require_esbocs_chain(dap_chain_t *chain) {
    if (!chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain pointer");
        return -1;
    }
    const char *cs_name = dap_chain_get_cs_type(chain);
    if (!cs_name || strcmp(cs_name, DAP_CHAIN_ESBOCS_CS_TYPE_STR) != 0) {
        PyErr_SetString(PyExc_ValueError, "Chain consensus must be ESBOCS");
        return -1;
    }
    if (!chain->_inheritor) {
        PyErr_SetString(PyExc_ValueError, "Chain type blocks is not initialized");
        return -1;
    }
    dap_chain_type_blocks_t *blocks = DAP_CHAIN_TYPE_BLOCKS(chain);
    if (!blocks || !blocks->_inheritor) {
        PyErr_SetString(PyExc_ValueError, "ESBOCS consensus is not initialized");
        return -1;
    }
    return 0;
}

static int cf_consensus_require_chain_cs_name(dap_chain_t *chain) {
    if (!chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain pointer");
        return -1;
    }
    dap_chain_pvt_t *pvt = DAP_CHAIN_PVT(chain);
    if (!pvt || !pvt->cs_name || pvt->cs_name[0] == '\0') {
        PyErr_SetString(PyExc_ValueError, "Chain consensus name is not set");
        return -1;
    }
    return 0;
}

static int cf_consensus_require_dag_poa_chain(dap_chain_t *chain) {
    if (!chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain pointer");
        return -1;
    }
    const char *cs_name = dap_chain_get_cs_type(chain);
    if (!cs_name || strcmp(cs_name, "dag_poa") != 0) {
        PyErr_SetString(PyExc_ValueError, "Chain consensus must be dag_poa");
        return -1;
    }
    if (!chain->_inheritor) {
        PyErr_SetString(PyExc_ValueError, "Chain DAG type is not initialized");
        return -1;
    }
    dap_chain_type_dag_t *dag = DAP_CHAIN_TYPE_DAG(chain);
    if (!dag || !dag->_inheritor) {
        PyErr_SetString(PyExc_ValueError, "DAG-POA consensus is not initialized");
        return -1;
    }
    return 0;
}

static int cf_consensus_parse_node_addr(PyObject *obj, dap_chain_node_addr_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "node address value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_chain_node_addr_t *ptr = (dap_chain_node_addr_t *)PyCapsule_GetPointer(obj, "dap_chain_node_addr_t");
        if (!ptr) {
            ptr = (dap_chain_node_addr_t *)PyCapsule_GetPointer(obj, "dap_stream_node_addr_t");
        }
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid node address capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyLong_Check(obj)) {
        unsigned long long val = PyLong_AsUnsignedLongLong(obj);
        if (PyErr_Occurred()) {
            return -1;
        }
        out->uint64 = val;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_chain_node_addr_t)) {
            PyErr_Format(PyExc_ValueError, "node address must be exactly %zu bytes", sizeof(dap_chain_node_addr_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_chain_node_addr_t));
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *addr_str = PyUnicode_AsUTF8(obj);
        if (!addr_str) {
            return -1;
        }
        if (dap_chain_node_addr_from_str(out, addr_str) != 0) {
            PyErr_SetString(PyExc_ValueError, "Invalid node address string");
            return -1;
        }
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "node address must be int, bytes, str, or node address capsule");
    return -1;
}

static int cf_consensus_parse_uint256(PyObject *obj, uint256_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "uint256 value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        uint256_t *ptr = (uint256_t *)PyCapsule_GetPointer(obj, "uint256_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid uint256_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(uint256_t)) {
            PyErr_Format(PyExc_ValueError, "uint256 value must be exactly %zu bytes", sizeof(uint256_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(uint256_t));
        return 0;
    }
    if (PyLong_Check(obj)) {
        unsigned long long val = PyLong_AsUnsignedLongLong(obj);
        if (PyErr_Occurred()) {
            return -1;
        }
        *out = dap_chain_uint256_from(val);
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *str_val = PyUnicode_AsUTF8(obj);
        if (!str_val) {
            return -1;
        }
        *out = dap_chain_balance_scan(str_val);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "uint256 value must be bytes, int, str, or uint256_t capsule");
    return -1;
}

// =============================================================================
// CONSENSUS SYSTEM INITIALIZATION
// =============================================================================

/**
 * @brief Initialize consensus subsystem globally
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    int l_result = dap_chain_cs_init();
    
    log_it(L_DEBUG, "Consensus subsystem initialized, result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize consensus subsystem globally
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_chain_cs_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    dap_chain_cs_deinit();
    
    log_it(L_DEBUG, "Consensus subsystem deinitialized");
    Py_RETURN_NONE;
}

// =============================================================================
// DAG-POA CONSENSUS
// =============================================================================

/**
 * @brief Initialize dag-poa consensus
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_type_dag_poa_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    int l_result = dap_chain_type_dag_poa_init();
    log_it(L_DEBUG, "DAG-POA consensus initialized, result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize dag-poa consensus
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_chain_type_dag_poa_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    dap_chain_type_dag_poa_deinit();
    log_it(L_DEBUG, "DAG-POA consensus deinitialized");
    Py_RETURN_NONE;
}

/**
 * @brief Get authorized certs for dag-poa consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Tuple (certs list capsule, certs_count, verify_count) or None
 */
PyObject* dap_chain_type_dag_poa_get_auth_certs_py(PyObject *a_self, PyObject *a_args) {
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
    if (cf_consensus_require_dag_poa_chain(l_chain) != 0) {
        return NULL;
    }

    size_t l_certs_count = 0;
    uint16_t l_verify_count = 0;
    dap_list_t *l_certs = dap_chain_type_dag_poa_get_auth_certs(l_chain, &l_certs_count, &l_verify_count);
    if (!l_certs) {
        Py_RETURN_NONE;
    }

    PyObject *l_list_capsule = PyCapsule_New(l_certs, "dap_list_t", NULL);
    if (!l_list_capsule) {
        return NULL;
    }

    PyObject *l_tuple = PyTuple_New(3);
    if (!l_tuple) {
        Py_DECREF(l_list_capsule);
        return NULL;
    }

    PyTuple_SET_ITEM(l_tuple, 0, l_list_capsule);
    PyTuple_SET_ITEM(l_tuple, 1, PyLong_FromSize_t(l_certs_count));
    PyTuple_SET_ITEM(l_tuple, 2, PyLong_FromUnsignedLong((unsigned long)l_verify_count));

    return l_tuple;
}

/**
 * @brief Set presign callback for dag-poa consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, callback capsule or None, arg capsule or None)
 * @return None
 */
PyObject* dap_chain_type_dag_poa_presign_callback_set_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    PyObject *l_cb_obj = Py_None;
    PyObject *l_arg_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|OO", &l_chain_obj, &l_cb_obj, &l_arg_obj)) {
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
    if (cf_consensus_require_dag_poa_chain(l_chain) != 0) {
        return NULL;
    }

    dap_chain_type_dag_poa_callback_t l_callback = NULL;
    if (l_cb_obj && l_cb_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_cb_obj)) {
            PyErr_SetString(PyExc_TypeError, "Callback must be a capsule or None");
            return NULL;
        }
        l_callback = (dap_chain_type_dag_poa_callback_t)PyCapsule_GetPointer(
            l_cb_obj, "dap_chain_type_dag_poa_callback_t");
        if (!l_callback) {
            PyErr_SetString(PyExc_ValueError, "Invalid dag-poa callback capsule");
            return NULL;
        }
    }

    void *l_arg = NULL;
    if (l_arg_obj && l_arg_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_arg_obj)) {
            PyErr_SetString(PyExc_TypeError, "Callback arg must be a capsule or None");
            return NULL;
        }
        l_arg = PyCapsule_GetPointer(l_arg_obj, "void_ptr");
        if (!l_arg) {
            PyErr_SetString(PyExc_ValueError, "Invalid callback arg capsule");
            return NULL;
        }
    }

    dap_chain_type_dag_poa_presign_callback_set(l_chain, l_callback, l_arg);
    log_it(L_DEBUG, "DAG-POA presign callback %s", l_callback ? "set" : "cleared");
    Py_RETURN_NONE;
}

// =============================================================================
// CONSENSUS LIFECYCLE OPERATIONS
// =============================================================================

/**
 * @brief Create consensus from config
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, config capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_config_obj;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_obj, &l_config_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_config_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a config capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    dap_config_t *l_config = (dap_config_t *)PyCapsule_GetPointer(l_config_obj, "dap_config_t");
    
    if (!l_chain || !l_config) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain or config capsule");
        return NULL;
    }
    if (cf_consensus_require_chain_cs_name(l_chain) != 0) {
        return NULL;
    }

    int l_result = dap_chain_cs_create(l_chain, l_config);
    
    log_it(L_DEBUG, "Consensus create result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Load consensus from config
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, config capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_load_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_config_obj;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_obj, &l_config_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_config_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a config capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    dap_config_t *l_config = (dap_config_t *)PyCapsule_GetPointer(l_config_obj, "dap_config_t");
    
    if (!l_chain || !l_config) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain or config capsule");
        return NULL;
    }
    if (cf_consensus_require_chain_cs_name(l_chain) != 0) {
        return NULL;
    }

    int l_result = dap_chain_cs_load(l_chain, l_config);
    
    log_it(L_DEBUG, "Consensus load result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Start consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_start_py(PyObject *a_self, PyObject *a_args) {
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
    if (cf_consensus_require_chain_cs_name(l_chain) != 0) {
        return NULL;
    }
    
    int l_result = dap_chain_cs_start(l_chain);
    
    log_it(L_DEBUG, "Consensus start result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Stop consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_stop_py(PyObject *a_self, PyObject *a_args) {
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
    if (cf_consensus_require_chain_cs_name(l_chain) != 0) {
        return NULL;
    }
    
    int l_result = dap_chain_cs_stop(l_chain);
    
    log_it(L_DEBUG, "Consensus stop result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Purge consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_purge_py(PyObject *a_self, PyObject *a_args) {
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
    if (cf_consensus_require_chain_cs_name(l_chain) != 0) {
        return NULL;
    }
    
    int l_result = dap_chain_cs_purge(l_chain);
    
    log_it(L_DEBUG, "Consensus purge result: %d", l_result);
    return PyLong_FromLong(l_result);
}

// =============================================================================
// CONSENSUS CALLBACKS MANAGEMENT
// =============================================================================

/**
 * @brief Set callbacks for chain
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, callbacks capsule)
 * @return None
 */
PyObject* dap_chain_cs_set_callbacks_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_callbacks_obj;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_obj, &l_callbacks_obj)) {
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
    
    dap_chain_cs_callbacks_t *l_callbacks = NULL;
    if (l_callbacks_obj && l_callbacks_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_callbacks_obj)) {
            PyErr_SetString(PyExc_TypeError, "Second argument must be a callbacks capsule or None");
            return NULL;
        }
        l_callbacks = (dap_chain_cs_callbacks_t *)PyCapsule_GetPointer(l_callbacks_obj, "dap_chain_cs_callbacks_t");
        if (!l_callbacks) {
            PyErr_SetString(PyExc_ValueError, "Invalid callbacks capsule");
            return NULL;
        }
    }
    
    dap_chain_cs_set_callbacks(l_chain, l_callbacks);
    
    log_it(L_DEBUG, "Consensus callbacks %s for chain", l_callbacks ? "set" : "cleared");
    Py_RETURN_NONE;
}

/**
 * @brief Get registered callbacks for chain
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return PyCapsule wrapping dap_chain_cs_callbacks_t* or None
 */
PyObject* dap_chain_cs_get_callbacks_py(PyObject *a_self, PyObject *a_args) {
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
    
    dap_chain_cs_callbacks_t *l_callbacks = dap_chain_cs_get_callbacks(l_chain);
    if (!l_callbacks) {
        log_it(L_DEBUG, "No callbacks registered for chain");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved consensus callbacks for chain");
    return PyCapsule_New(l_callbacks, "dap_chain_cs_callbacks_t", NULL);
}

// =============================================================================
// CONSENSUS WRAPPER FUNCTIONS
// =============================================================================

/**
 * @brief Get fee group for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, net_name string)
 * @return String or None
 */
PyObject* dap_chain_cs_get_fee_group_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    const char *l_net_name;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_chain_obj, &l_net_name)) {
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
    
    char *l_fee_group = dap_chain_cs_get_fee_group(l_chain, l_net_name);
    if (!l_fee_group) {
        log_it(L_DEBUG, "No fee group found");
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = PyUnicode_FromString(l_fee_group);
    log_it(L_DEBUG, "Retrieved fee group: %s", l_fee_group);
    return l_result;
}

/**
 * @brief Get reward group for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, net_name string)
 * @return String or None
 */
PyObject* dap_chain_cs_get_reward_group_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    const char *l_net_name;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_chain_obj, &l_net_name)) {
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
    
    char *l_reward_group = dap_chain_cs_get_reward_group(l_chain, l_net_name);
    if (!l_reward_group) {
        log_it(L_DEBUG, "No reward group found");
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = PyUnicode_FromString(l_reward_group);
    log_it(L_DEBUG, "Retrieved reward group: %s", l_reward_group);
    return l_result;
}

/**
 * @brief Get fee for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Bytes representing uint256_t
 */
PyObject* dap_chain_cs_get_fee_py(PyObject *a_self, PyObject *a_args) {
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
    
    uint256_t l_fee = dap_chain_cs_get_fee(l_chain);
    
    log_it(L_DEBUG, "Retrieved consensus fee");
    return PyBytes_FromStringAndSize((const char *)&l_fee, sizeof(uint256_t));
}

/**
 * @brief Get sign public key for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return PyCapsule wrapping dap_pkey_t* or None
 */
PyObject* dap_chain_cs_get_sign_pkey_py(PyObject *a_self, PyObject *a_args) {
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
    
    dap_pkey_t *l_pkey = dap_chain_cs_get_sign_pkey(l_chain);
    if (!l_pkey) {
        log_it(L_DEBUG, "No sign pkey found");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Retrieved consensus sign pkey");
    return PyCapsule_New(l_pkey, "dap_pkey_t", NULL);
}

/**
 * @brief Get collecting level for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Bytes representing uint256_t
 */
PyObject* dap_chain_cs_get_collecting_level_py(PyObject *a_self, PyObject *a_args) {
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
    
    uint256_t l_level = dap_chain_cs_get_collecting_level(l_chain);
    
    log_it(L_DEBUG, "Retrieved consensus collecting level");
    return PyBytes_FromStringAndSize((const char *)&l_level, sizeof(uint256_t));
}

/**
 * @brief Get autocollect status for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Boolean
 */
PyObject* dap_chain_cs_get_autocollect_status_py(PyObject *a_self, PyObject *a_args) {
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
    
    bool l_status = dap_chain_cs_get_autocollect_status(l_chain);
    
    log_it(L_DEBUG, "Consensus autocollect status: %s", l_status ? "true" : "false");
    return PyBool_FromLong(l_status);
}

/**
 * @brief Add block collect for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, block_cache capsule, params capsule, type int)
 * @return None
 */
PyObject* dap_chain_cs_add_block_collect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_block_cache_obj, *l_params_obj;
    int l_type;
    
    if (!PyArg_ParseTuple(a_args, "OOOi", &l_chain_obj, &l_block_cache_obj, &l_params_obj, &l_type)) {
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
    
    void *l_block_cache = NULL;
    if (l_block_cache_obj && l_block_cache_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_block_cache_obj)) {
            PyErr_SetString(PyExc_TypeError, "Second argument must be a capsule or None");
            return NULL;
        }
        l_block_cache = PyCapsule_GetPointer(l_block_cache_obj, NULL);
    }
    
    void *l_params = NULL;
    if (l_params_obj && l_params_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_params_obj)) {
            PyErr_SetString(PyExc_TypeError, "Third argument must be a capsule or None");
            return NULL;
        }
        l_params = PyCapsule_GetPointer(l_params_obj, NULL);
    }
    
    dap_chain_cs_add_block_collect(l_chain, l_block_cache, l_params, l_type);
    
    log_it(L_DEBUG, "Added block collect for consensus (type=%d)", l_type);
    Py_RETURN_NONE;
}

/**
 * @brief Set hardfork state for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, state bool)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_set_hardfork_state_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    int l_state;
    
    if (!PyArg_ParseTuple(a_args, "Op", &l_chain_obj, &l_state)) {
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
    
    int l_result = dap_chain_cs_set_hardfork_state(l_chain, (bool)l_state);
    
    log_it(L_DEBUG, "Consensus hardfork state set to %s, result: %d", l_state ? "true" : "false", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Check if hardfork is engaged
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Boolean
 */
PyObject* dap_chain_cs_hardfork_engaged_py(PyObject *a_self, PyObject *a_args) {
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
    
    bool l_engaged = dap_chain_cs_hardfork_engaged(l_chain);
    
    log_it(L_DEBUG, "Consensus hardfork engaged: %s", l_engaged ? "true" : "false");
    return PyBool_FromLong(l_engaged);
}

/**
 * @brief Get sign key for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return PyCapsule wrapping dap_enc_key_t* or None
 */
PyObject* dap_chain_cs_get_sign_key_py(PyObject *a_self, PyObject *a_args) {
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

    dap_enc_key_t *l_key = dap_chain_cs_get_sign_key(l_chain);
    if (!l_key) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_key, "dap_enc_key_t", NULL);
}

/**
 * @brief Get minimum validators count for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Minimum validators count
 */
PyObject* dap_chain_cs_get_min_validators_count_py(PyObject *a_self, PyObject *a_args) {
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

    uint16_t l_count = dap_chain_cs_get_min_validators_count(l_chain);
    return PyLong_FromUnsignedLong((unsigned long)l_count);
}

/**
 * @brief Set minimum validators count for consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, count)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_set_min_validators_count_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    unsigned long long l_count = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_chain_obj, &l_count)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    if (l_count > UINT16_MAX) {
        PyErr_SetString(PyExc_ValueError, "count must fit into uint16");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    int l_result = dap_chain_cs_set_min_validators_count(l_chain, (uint16_t)l_count);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Add validator to consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, node addr)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_add_validator_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    PyObject *l_node_addr_obj;

    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_obj, &l_node_addr_obj)) {
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

    dap_chain_node_addr_t l_node_addr = {0};
    if (cf_consensus_parse_node_addr(l_node_addr_obj, &l_node_addr) != 0) {
        return NULL;
    }

    int l_result = dap_chain_cs_add_validator(l_chain, &l_node_addr);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Remove validator from consensus
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, node addr)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_remove_validator_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    PyObject *l_node_addr_obj;

    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_obj, &l_node_addr_obj)) {
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

    dap_chain_node_addr_t l_node_addr = {0};
    if (cf_consensus_parse_node_addr(l_node_addr_obj, &l_node_addr) != 0) {
        return NULL;
    }

    int l_result = dap_chain_cs_remove_validator(l_chain, &l_node_addr);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Check if consensus is started
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Boolean
 */
PyObject* dap_chain_cs_is_started_py(PyObject *a_self, PyObject *a_args) {
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

    bool l_started = dap_chain_cs_is_started(l_chain);
    return PyBool_FromLong(l_started);
}

/**
 * @brief Add datum to mempool via consensus wrapper
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, datum capsule, hash_out_type string)
 * @return String hash or None
 */
PyObject* dap_chain_cs_mempool_datum_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    PyObject *l_datum_obj;
    const char *l_hash_out_type;

    if (!PyArg_ParseTuple(a_args, "OOs", &l_chain_obj, &l_datum_obj, &l_hash_out_type)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_datum_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a datum capsule");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    dap_chain_datum_t *l_datum = (dap_chain_datum_t *)PyCapsule_GetPointer(l_datum_obj, "dap_chain_datum_t");
    if (!l_chain || !l_datum) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain or datum capsule");
        return NULL;
    }

    char *l_hash = dap_chain_cs_mempool_datum_add(l_chain, l_datum, l_hash_out_type);
    if (!l_hash) {
        Py_RETURN_NONE;
    }

    return PyUnicode_FromString(l_hash);
}

/**
 * @brief Check stake pkey hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, pkey_hash)
 * @return Tuple (result_code, sovereign_tax bytes, sovereign_addr capsule)
 */
PyObject* dap_chain_cs_stake_check_pkey_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    PyObject *l_hash_obj;

    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_obj, &l_hash_obj)) {
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

    dap_hash_fast_t l_pkey_hash = {0};
    if (cf_consensus_parse_hash_fast_required(l_hash_obj, &l_pkey_hash) != 0) {
        return NULL;
    }

    uint256_t l_sovereign_tax = uint256_0;
    dap_chain_addr_t l_sovereign_addr = {0};

    int l_result = dap_chain_cs_stake_check_pkey_hash(l_chain, &l_pkey_hash, &l_sovereign_tax, &l_sovereign_addr);

    PyObject *l_tuple = PyTuple_New(3);
    if (!l_tuple) {
        return NULL;
    }

    PyTuple_SET_ITEM(l_tuple, 0, PyLong_FromLong(l_result));
    PyTuple_SET_ITEM(l_tuple, 1, PyBytes_FromStringAndSize((const char *)&l_sovereign_tax, sizeof(uint256_t)));

    dap_chain_addr_t *l_addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    if (!l_addr_copy) {
        Py_DECREF(l_tuple);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }
    *l_addr_copy = l_sovereign_addr;

    PyObject *l_addr_capsule = PyCapsule_New(l_addr_copy, "dap_chain_addr_t", NULL);
    if (!l_addr_capsule) {
        DAP_DELETE(l_addr_copy);
        Py_DECREF(l_tuple);
        return NULL;
    }

    PyTuple_SET_ITEM(l_tuple, 2, l_addr_capsule);
    return l_tuple;
}

/**
 * @brief Import stake hardfork data
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, decree_hash)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_stake_hardfork_data_import_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    PyObject *l_hash_obj;

    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_obj, &l_hash_obj)) {
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

    dap_hash_fast_t l_decree_hash = {0};
    if (cf_consensus_parse_hash_fast_required(l_hash_obj, &l_decree_hash) != 0) {
        return NULL;
    }

    int l_result = dap_chain_cs_stake_hardfork_data_import(l_chain, &l_decree_hash);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Switch stake table
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, to_sandbox bool)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_stake_switch_table_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    int l_to_sandbox = 0;

    if (!PyArg_ParseTuple(a_args, "Op", &l_chain_obj, &l_to_sandbox)) {
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

    int l_result = dap_chain_cs_stake_switch_table(l_chain, (bool)l_to_sandbox);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Register consensus implementation
 * @param a_self Python self object (unused)
 * @param a_args Arguments (cs_name string, optional callbacks capsule)
 * @return None
 */
PyObject* dap_chain_cs_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name = NULL;
    PyObject *l_callbacks_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "s|O", &l_name, &l_callbacks_obj)) {
        return NULL;
    }

    dap_chain_cs_lifecycle_t l_callbacks = {0};
    if (l_callbacks_obj && l_callbacks_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_callbacks_obj)) {
            PyErr_SetString(PyExc_TypeError, "Callbacks must be a capsule or None");
            return NULL;
        }
        dap_chain_cs_lifecycle_t *l_ptr = (dap_chain_cs_lifecycle_t *)PyCapsule_GetPointer(
            l_callbacks_obj, "dap_chain_cs_lifecycle_t");
        if (!l_ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid lifecycle callbacks capsule");
            return NULL;
        }
        l_callbacks = *l_ptr;
    }

    dap_chain_cs_add(l_name, l_callbacks);
    Py_RETURN_NONE;
}

// =============================================================================
// ESBOCS CONSENSUS
// =============================================================================

/**
 * @brief Initialize ESBOCS consensus
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_esbocs_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    int l_result = dap_chain_cs_esbocs_init();
    log_it(L_DEBUG, "ESBOCS consensus initialized, result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize ESBOCS consensus
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_chain_cs_esbocs_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    dap_chain_cs_esbocs_deinit();
    log_it(L_DEBUG, "ESBOCS consensus deinitialized");
    Py_RETURN_NONE;
}

/**
 * @brief Check if ESBOCS consensus started
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id)
 * @return Boolean
 */
PyObject* dap_chain_esbocs_started_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    bool l_started = dap_chain_esbocs_started(l_net_id_struct);
    return PyBool_FromLong(l_started);
}

/**
 * @brief Stop ESBOCS timer
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id)
 * @return None
 */
PyObject* dap_chain_esbocs_stop_timer_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_esbocs_stop_timer(l_net_id_struct);
    Py_RETURN_NONE;
}

/**
 * @brief Start ESBOCS timer
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id)
 * @return None
 */
PyObject* dap_chain_esbocs_start_timer_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_esbocs_start_timer(l_net_id_struct);
    Py_RETURN_NONE;
}

/**
 * @brief Get ESBOCS sign public key
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id)
 * @return PyCapsule wrapping dap_pkey_t* or None
 */
PyObject* dap_chain_esbocs_get_sign_pkey_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_pkey_t *l_pkey = dap_chain_esbocs_get_sign_pkey(l_net_id_struct);
    if (!l_pkey) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_pkey, "dap_pkey_t", NULL);
}

/**
 * @brief Get ESBOCS fee
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id)
 * @return Bytes representing uint256_t
 */
PyObject* dap_chain_esbocs_get_fee_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    uint256_t l_fee = dap_chain_esbocs_get_fee(l_net_id_struct);
    return PyBytes_FromStringAndSize((const char *)&l_fee, sizeof(uint256_t));
}

/**
 * @brief Get ESBOCS autocollect status
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id)
 * @return Boolean
 */
PyObject* dap_chain_esbocs_get_autocollect_status_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    bool l_status = dap_chain_esbocs_get_autocollect_status(l_net_id_struct);
    return PyBool_FromLong(l_status);
}

/**
 * @brief Add block collect for ESBOCS
 * @param a_self Python self object (unused)
 * @param a_args Arguments (block_cache capsule, params capsule, type int)
 * @return None
 */
PyObject* dap_chain_esbocs_add_block_collect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_block_cache_obj;
    PyObject *l_params_obj;
    int l_type;

    if (!PyArg_ParseTuple(a_args, "OOi", &l_block_cache_obj, &l_params_obj, &l_type)) {
        return NULL;
    }

    dap_chain_block_cache_t *l_block_cache = NULL;
    if (l_block_cache_obj && l_block_cache_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_block_cache_obj)) {
            PyErr_SetString(PyExc_TypeError, "First argument must be a block cache capsule or None");
            return NULL;
        }
        l_block_cache = (dap_chain_block_cache_t *)PyCapsule_GetPointer(l_block_cache_obj, "dap_chain_block_cache_t");
        if (!l_block_cache) {
            PyErr_SetString(PyExc_ValueError, "Invalid block cache capsule");
            return NULL;
        }
    }

    dap_chain_esbocs_block_collect_t *l_params = NULL;
    if (l_params_obj && l_params_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_params_obj)) {
            PyErr_SetString(PyExc_TypeError, "Second argument must be a params capsule or None");
            return NULL;
        }
        l_params = (dap_chain_esbocs_block_collect_t *)PyCapsule_GetPointer(
            l_params_obj, "dap_chain_esbocs_block_collect_t");
        if (!l_params) {
            PyErr_SetString(PyExc_ValueError, "Invalid params capsule");
            return NULL;
        }
    }

    dap_chain_esbocs_add_block_collect(l_block_cache, l_params, (dap_chain_block_autocollect_type_t)l_type);
    Py_RETURN_NONE;
}

/**
 * @brief Add validator to ESBOCS clusters
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id, validator_addr)
 * @return Boolean
 */
PyObject* dap_chain_esbocs_add_validator_to_clusters_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    PyObject *l_addr_obj;

    if (!PyArg_ParseTuple(a_args, "KO", &l_net_id, &l_addr_obj)) {
        return NULL;
    }

    dap_chain_node_addr_t l_addr = {0};
    if (cf_consensus_parse_node_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    bool l_result = dap_chain_esbocs_add_validator_to_clusters(l_net_id_struct, &l_addr);
    return PyBool_FromLong(l_result);
}

/**
 * @brief Remove validator from ESBOCS clusters
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id, validator_addr)
 * @return Boolean
 */
PyObject* dap_chain_esbocs_remove_validator_from_clusters_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    PyObject *l_addr_obj;

    if (!PyArg_ParseTuple(a_args, "KO", &l_net_id, &l_addr_obj)) {
        return NULL;
    }

    dap_chain_node_addr_t l_addr = {0};
    if (cf_consensus_parse_node_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    bool l_result = dap_chain_esbocs_remove_validator_from_clusters(l_net_id_struct, &l_addr);
    return PyBool_FromLong(l_result);
}

/**
 * @brief Get ESBOCS collecting level
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Bytes representing uint256_t
 */
PyObject* dap_chain_esbocs_get_collecting_level_py(PyObject *a_self, PyObject *a_args) {
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
    if (cf_consensus_require_esbocs_chain(l_chain) != 0) {
        return NULL;
    }

    uint256_t l_level = dap_chain_esbocs_get_collecting_level(l_chain);
    return PyBytes_FromStringAndSize((const char *)&l_level, sizeof(uint256_t));
}

/**
 * @brief Get ESBOCS sign key
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return PyCapsule wrapping dap_enc_key_t* or None
 */
PyObject* dap_chain_esbocs_get_sign_key_py(PyObject *a_self, PyObject *a_args) {
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
    if (cf_consensus_require_esbocs_chain(l_chain) != 0) {
        return NULL;
    }

    dap_enc_key_t *l_key = dap_chain_esbocs_get_sign_key(l_chain);
    if (!l_key) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_key, "dap_enc_key_t", NULL);
}

/**
 * @brief Set ESBOCS minimum validators count
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, count)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_esbocs_set_min_validators_count_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    unsigned long long l_count = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_chain_obj, &l_count)) {
        return NULL;
    }

    if (l_count > UINT16_MAX) {
        PyErr_SetString(PyExc_ValueError, "count must fit into uint16");
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
    if (cf_consensus_require_esbocs_chain(l_chain) != 0) {
        return NULL;
    }

    int l_result = dap_chain_esbocs_set_min_validators_count(l_chain, (uint16_t)l_count);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Get ESBOCS minimum validators count
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id)
 * @return Minimum validators count
 */
PyObject* dap_chain_esbocs_get_min_validators_count_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    uint16_t l_count = dap_chain_esbocs_get_min_validators_count(l_net_id_struct);
    return PyLong_FromUnsignedLong((unsigned long)l_count);
}

/**
 * @brief Set ESBOCS emergency validator
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, add bool, sign_type uint32, validator_hash)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_esbocs_set_emergency_validator_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    PyObject *l_hash_obj;
    int l_add = 0;
    unsigned long l_sign_type = 0;

    if (!PyArg_ParseTuple(a_args, "OpkO", &l_chain_obj, &l_add, &l_sign_type, &l_hash_obj)) {
        return NULL;
    }

    if (l_sign_type > UINT32_MAX) {
        PyErr_SetString(PyExc_ValueError, "sign_type must fit into uint32");
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
    if (cf_consensus_require_esbocs_chain(l_chain) != 0) {
        return NULL;
    }

    dap_hash_fast_t l_hash = {0};
    if (cf_consensus_parse_hash_fast_required(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    int l_result = dap_chain_esbocs_set_emergency_validator(
        l_chain, (bool)l_add, (uint32_t)l_sign_type, &l_hash);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Enable or disable ESBOCS signs structure check
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, enable bool)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_esbocs_set_signs_struct_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    int l_enable = 0;

    if (!PyArg_ParseTuple(a_args, "Op", &l_chain_obj, &l_enable)) {
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
    if (cf_consensus_require_esbocs_chain(l_chain) != 0) {
        return NULL;
    }

    int l_result = dap_chain_esbocs_set_signs_struct_check(l_chain, (bool)l_enable);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Prepare ESBOCS hardfork
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, generation, block_num, trusted_addrs capsule, changed_addrs capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_esbocs_set_hardfork_prepare_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    PyObject *l_trusted_obj = Py_None;
    PyObject *l_changed_obj = Py_None;
    unsigned long long l_generation = 0;
    unsigned long long l_block_num = 0;

    if (!PyArg_ParseTuple(a_args, "OKK|OO", &l_chain_obj, &l_generation, &l_block_num,
                          &l_trusted_obj, &l_changed_obj)) {
        return NULL;
    }

    if (l_generation > UINT16_MAX) {
        PyErr_SetString(PyExc_ValueError, "generation must fit into uint16");
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
    if (cf_consensus_require_esbocs_chain(l_chain) != 0) {
        return NULL;
    }

    dap_list_t *l_trusted_addrs = NULL;
    if (l_trusted_obj && l_trusted_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_trusted_obj)) {
            PyErr_SetString(PyExc_TypeError, "trusted_addrs must be a list capsule or None");
            return NULL;
        }
        l_trusted_addrs = (dap_list_t *)PyCapsule_GetPointer(l_trusted_obj, "dap_list_t");
        if (!l_trusted_addrs) {
            PyErr_SetString(PyExc_ValueError, "Invalid trusted_addrs capsule");
            return NULL;
        }
    }

    dap_json_t *l_changed_addrs = NULL;
    if (l_changed_obj && l_changed_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_changed_obj)) {
            PyErr_SetString(PyExc_TypeError, "changed_addrs must be a JSON capsule or None");
            return NULL;
        }
        l_changed_addrs = (dap_json_t *)PyCapsule_GetPointer(l_changed_obj, "dap_json_t");
        if (!l_changed_addrs) {
            PyErr_SetString(PyExc_ValueError, "Invalid changed_addrs capsule");
            return NULL;
        }
    }

    int l_result = dap_chain_esbocs_set_hardfork_prepare(
        l_chain, (uint16_t)l_generation, (uint64_t)l_block_num, l_trusted_addrs, l_changed_addrs);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Complete ESBOCS hardfork
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_esbocs_set_hardfork_complete_py(PyObject *a_self, PyObject *a_args) {
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
    if (cf_consensus_require_esbocs_chain(l_chain) != 0) {
        return NULL;
    }

    int l_result = dap_chain_esbocs_set_hardfork_complete(l_chain);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Check if ESBOCS hardfork engaged
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule)
 * @return Boolean
 */
PyObject* dap_chain_esbocs_hardfork_engaged_py(PyObject *a_self, PyObject *a_args) {
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
    if (cf_consensus_require_esbocs_chain(l_chain) != 0) {
        return NULL;
    }

    bool l_engaged = dap_chain_esbocs_hardfork_engaged(l_chain);
    return PyBool_FromLong(l_engaged);
}

/**
 * @brief Set ESBOCS hardfork state
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, state bool)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_esbocs_set_hardfork_state_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    int l_state = 0;

    if (!PyArg_ParseTuple(a_args, "Op", &l_chain_obj, &l_state)) {
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
    if (cf_consensus_require_esbocs_chain(l_chain) != 0) {
        return NULL;
    }

    int l_result = dap_chain_esbocs_set_hardfork_state(l_chain, (bool)l_state);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Set ESBOCS custom metadata callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id, callback capsule or None)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_esbocs_set_custom_metadata_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    PyObject *l_cb_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "K|O", &l_net_id, &l_cb_obj)) {
        return NULL;
    }

    dap_chain_esbocs_callback_set_custom_metadata_t l_callback = NULL;
    if (l_cb_obj && l_cb_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_cb_obj)) {
            PyErr_SetString(PyExc_TypeError, "Callback must be a capsule or None");
            return NULL;
        }
        l_callback = (dap_chain_esbocs_callback_set_custom_metadata_t)PyCapsule_GetPointer(
            l_cb_obj, "dap_chain_esbocs_callback_set_custom_metadata_t");
        if (!l_callback) {
            PyErr_SetString(PyExc_ValueError, "Invalid custom metadata callback capsule");
            return NULL;
        }
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_result = dap_chain_esbocs_set_custom_metadata_callback(l_net_id_struct, l_callback);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Set ESBOCS presign callback
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id, callback capsule or None)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_esbocs_set_presign_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    PyObject *l_cb_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "K|O", &l_net_id, &l_cb_obj)) {
        return NULL;
    }

    dap_chain_esbocs_callback_presign_t l_callback = NULL;
    if (l_cb_obj && l_cb_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_cb_obj)) {
            PyErr_SetString(PyExc_TypeError, "Callback must be a capsule or None");
            return NULL;
        }
        l_callback = (dap_chain_esbocs_callback_presign_t)PyCapsule_GetPointer(
            l_cb_obj, "dap_chain_esbocs_callback_presign_t");
        if (!l_callback) {
            PyErr_SetString(PyExc_ValueError, "Invalid presign callback capsule");
            return NULL;
        }
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_result = dap_chain_esbocs_set_presign_callback(l_net_id_struct, l_callback);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Set ESBOCS empty block generation period
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, blockgen_period)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_esbocs_set_empty_block_every_times_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    unsigned long long l_blockgen_period = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_chain_obj, &l_blockgen_period)) {
        return NULL;
    }

    if (l_blockgen_period > UINT16_MAX) {
        PyErr_SetString(PyExc_ValueError, "blockgen_period must fit into uint16");
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
    if (cf_consensus_require_esbocs_chain(l_chain) != 0) {
        return NULL;
    }

    int l_result = dap_chain_esbocs_set_empty_block_every_times(l_chain, (uint16_t)l_blockgen_period);
    return PyLong_FromLong(l_result);
}

// =============================================================================
// ESBOCS DECREE HANDLERS
// =============================================================================

/**
 * @brief Initialize ESBOCS decree handlers
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_cs_esbocs_decree_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    int l_result = dap_chain_cs_esbocs_decree_init();
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize ESBOCS decree handlers
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_chain_cs_esbocs_decree_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    dap_chain_cs_esbocs_decree_deinit();
    Py_RETURN_NONE;
}

// =============================================================================
// VALIDATOR API WRAPPERS
// =============================================================================

/**
 * @brief Initialize validator API
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_validator_api_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    int l_result = dap_chain_validator_api_init();
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize validator API
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_chain_validator_api_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    dap_chain_validator_api_deinit();
    Py_RETURN_NONE;
}

/**
 * @brief Register validator API implementation
 * @param a_self Python self object (unused)
 * @param a_args Arguments (registry capsule)
 * @return None
 */
PyObject* dap_chain_validator_api_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_registry_obj;

    if (!PyArg_ParseTuple(a_args, "O", &l_registry_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_registry_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a registry capsule");
        return NULL;
    }

    dap_chain_validator_api_registry_t *l_registry = (dap_chain_validator_api_registry_t *)PyCapsule_GetPointer(
        l_registry_obj, "dap_chain_validator_api_registry_t");
    if (!l_registry) {
        PyErr_SetString(PyExc_ValueError, "Invalid registry capsule");
        return NULL;
    }

    dap_chain_validator_api_register(l_registry);
    Py_RETURN_NONE;
}

/**
 * @brief Get validators list
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id, only_active bool)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_chain_validator_api_get_validators_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    int l_only_active = 0;

    if (!PyArg_ParseTuple(a_args, "Kp", &l_net_id, &l_only_active)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_list_t *l_list = dap_chain_validator_api_get_validators(l_net_id_struct, (bool)l_only_active, NULL);
    if (!l_list) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

/**
 * @brief Check if key is delegated
 * @param a_self Python self object (unused)
 * @param a_args Arguments (signing_addr)
 * @return Integer result code
 */
PyObject* dap_chain_validator_api_check_key_delegated_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_addr_obj;

    if (!PyArg_ParseTuple(a_args, "O", &l_addr_obj)) {
        return NULL;
    }

    dap_chain_addr_t l_addr = {0};
    if (cf_consensus_parse_chain_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    int l_result = dap_chain_validator_api_check_key_delegated(&l_addr);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Mark validator active/inactive
 * @param a_self Python self object (unused)
 * @param a_args Arguments (signing_addr, active bool)
 * @return Integer result code
 */
PyObject* dap_chain_validator_api_mark_validator_active_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_addr_obj;
    int l_active = 0;

    if (!PyArg_ParseTuple(a_args, "Op", &l_addr_obj, &l_active)) {
        return NULL;
    }

    dap_chain_addr_t l_addr = {0};
    if (cf_consensus_parse_chain_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    int l_result = dap_chain_validator_api_mark_validator_active(&l_addr, (bool)l_active);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Get public key by hash
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id, hash)
 * @return PyCapsule wrapping dap_pkey_t* or None
 */
PyObject* dap_chain_validator_api_get_pkey_by_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    PyObject *l_hash_obj;

    if (!PyArg_ParseTuple(a_args, "KO", &l_net_id, &l_hash_obj)) {
        return NULL;
    }

    dap_hash_fast_t l_hash = {0};
    if (cf_consensus_parse_hash_fast_required(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_pkey_t *l_pkey = dap_chain_validator_api_get_pkey_by_hash(l_net_id_struct, &l_hash);
    if (!l_pkey) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_pkey, "dap_pkey_t", NULL);
}

/**
 * @brief Delegate key via validator API
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule, signing_addr, decree_hash, tx_hash, value, node_addr, pkey)
 * @return None
 */
PyObject* dap_chain_validator_api_key_delegate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj;
    PyObject *l_addr_obj;
    PyObject *l_decree_hash_obj;
    PyObject *l_tx_hash_obj;
    PyObject *l_value_obj;
    PyObject *l_node_addr_obj;
    PyObject *l_pkey_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "OOOOOO|O", &l_net_obj, &l_addr_obj, &l_decree_hash_obj,
                          &l_tx_hash_obj, &l_value_obj, &l_node_addr_obj, &l_pkey_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a net capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid net capsule");
        return NULL;
    }

    dap_chain_addr_t l_addr = {0};
    if (cf_consensus_parse_chain_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    dap_hash_fast_t l_decree_hash = {0};
    dap_hash_fast_t *l_decree_hash_ptr = NULL;
    if (cf_consensus_parse_hash_fast_optional(l_decree_hash_obj, &l_decree_hash, &l_decree_hash_ptr) != 0) {
        return NULL;
    }

    dap_hash_fast_t l_tx_hash = {0};
    dap_hash_fast_t *l_tx_hash_ptr = NULL;
    if (cf_consensus_parse_hash_fast_optional(l_tx_hash_obj, &l_tx_hash, &l_tx_hash_ptr) != 0) {
        return NULL;
    }

    uint256_t l_value = {0};
    if (cf_consensus_parse_uint256(l_value_obj, &l_value) != 0) {
        return NULL;
    }

    dap_chain_node_addr_t l_node_addr = {0};
    if (cf_consensus_parse_node_addr(l_node_addr_obj, &l_node_addr) != 0) {
        return NULL;
    }

    dap_pkey_t *l_pkey = NULL;
    if (l_pkey_obj && l_pkey_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_pkey_obj)) {
            PyErr_SetString(PyExc_TypeError, "pkey must be a capsule or None");
            return NULL;
        }
        l_pkey = (dap_pkey_t *)PyCapsule_GetPointer(l_pkey_obj, "dap_pkey_t");
        if (!l_pkey) {
            PyErr_SetString(PyExc_ValueError, "Invalid pkey capsule");
            return NULL;
        }
    }

    dap_chain_validator_api_key_delegate(
        l_net, &l_addr, l_decree_hash_ptr, l_tx_hash_ptr, l_value, &l_node_addr, l_pkey);
    Py_RETURN_NONE;
}

/**
 * @brief Get minimum allowed stake value
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_id)
 * @return Bytes representing uint256_t
 */
PyObject* dap_chain_validator_api_get_allowed_min_value_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    uint256_t l_value = dap_chain_validator_api_get_allowed_min_value(l_net_id_struct);
    return PyBytes_FromStringAndSize((const char *)&l_value, sizeof(uint256_t));
}

/**
 * @brief Update hardfork transaction via validator API
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule)
 * @return None
 */
PyObject* dap_chain_validator_api_hardfork_tx_update_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a net capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid net capsule");
        return NULL;
    }

    dap_chain_validator_api_hardfork_tx_update(l_net);
    Py_RETURN_NONE;
}

/**
 * @brief Get node address from validator item
 * @param a_self Python self object (unused)
 * @param a_args Arguments (validator_item capsule)
 * @return Node address as uint64 or None
 */
PyObject* dap_chain_validator_api_get_node_addr_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_item_obj;

    if (!PyArg_ParseTuple(a_args, "O", &l_item_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_item_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a validator item capsule");
        return NULL;
    }

    dap_chain_validator_item_t l_item = PyCapsule_GetPointer(l_item_obj, "dap_chain_validator_item_t");
    if (!l_item) {
        PyErr_SetString(PyExc_ValueError, "Invalid validator item capsule");
        return NULL;
    }

    dap_stream_node_addr_t *l_addr = dap_chain_validator_api_get_node_addr(l_item);
    if (!l_addr) {
        Py_RETURN_NONE;
    }

    return PyLong_FromUnsignedLongLong(l_addr->uint64);
}

/**
 * @brief Get value from validator item
 * @param a_self Python self object (unused)
 * @param a_args Arguments (validator_item capsule)
 * @return Bytes representing uint256_t
 */
PyObject* dap_chain_validator_api_get_value_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_item_obj;

    if (!PyArg_ParseTuple(a_args, "O", &l_item_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_item_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a validator item capsule");
        return NULL;
    }

    dap_chain_validator_item_t l_item = PyCapsule_GetPointer(l_item_obj, "dap_chain_validator_item_t");
    if (!l_item) {
        PyErr_SetString(PyExc_ValueError, "Invalid validator item capsule");
        return NULL;
    }

    uint256_t l_value = dap_chain_validator_api_get_value(l_item);
    return PyBytes_FromStringAndSize((const char *)&l_value, sizeof(uint256_t));
}


// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

/**
 * @brief Get method definitions for consensus module
 */
PyMethodDef* cellframe_consensus_get_methods(void) {
    static PyMethodDef consensus_methods[] = {
        // DAG-POA consensus
        {"dap_chain_type_dag_poa_init", (PyCFunction)dap_chain_type_dag_poa_init_py, METH_VARARGS,
         "Initialize dag-poa consensus"},
        {"dap_chain_type_dag_poa_deinit", (PyCFunction)dap_chain_type_dag_poa_deinit_py, METH_VARARGS,
         "Deinitialize dag-poa consensus"},
        {"dap_chain_type_dag_poa_get_auth_certs", (PyCFunction)dap_chain_type_dag_poa_get_auth_certs_py, METH_VARARGS,
         "Get authorized certs for dag-poa consensus"},
        {"dap_chain_type_dag_poa_presign_callback_set", (PyCFunction)dap_chain_type_dag_poa_presign_callback_set_py, METH_VARARGS,
         "Set dag-poa presign callback"},

        // ESBOCS consensus
        {"dap_chain_cs_esbocs_init", (PyCFunction)dap_chain_cs_esbocs_init_py, METH_VARARGS,
         "Initialize ESBOCS consensus"},
        {"dap_chain_cs_esbocs_deinit", (PyCFunction)dap_chain_cs_esbocs_deinit_py, METH_VARARGS,
         "Deinitialize ESBOCS consensus"},
        {"dap_chain_esbocs_started", (PyCFunction)dap_chain_esbocs_started_py, METH_VARARGS,
         "Check ESBOCS started"},
        {"dap_chain_esbocs_stop_timer", (PyCFunction)dap_chain_esbocs_stop_timer_py, METH_VARARGS,
         "Stop ESBOCS timer"},
        {"dap_chain_esbocs_start_timer", (PyCFunction)dap_chain_esbocs_start_timer_py, METH_VARARGS,
         "Start ESBOCS timer"},
        {"dap_chain_esbocs_get_sign_pkey", (PyCFunction)dap_chain_esbocs_get_sign_pkey_py, METH_VARARGS,
         "Get ESBOCS sign public key"},
        {"dap_chain_esbocs_get_fee", (PyCFunction)dap_chain_esbocs_get_fee_py, METH_VARARGS,
         "Get ESBOCS fee"},
        {"dap_chain_esbocs_get_autocollect_status", (PyCFunction)dap_chain_esbocs_get_autocollect_status_py, METH_VARARGS,
         "Get ESBOCS autocollect status"},
        {"dap_chain_esbocs_add_block_collect", (PyCFunction)dap_chain_esbocs_add_block_collect_py, METH_VARARGS,
         "Add ESBOCS block collect"},
        {"dap_chain_esbocs_add_validator_to_clusters", (PyCFunction)dap_chain_esbocs_add_validator_to_clusters_py, METH_VARARGS,
         "Add ESBOCS validator to clusters"},
        {"dap_chain_esbocs_remove_validator_from_clusters", (PyCFunction)dap_chain_esbocs_remove_validator_from_clusters_py, METH_VARARGS,
         "Remove ESBOCS validator from clusters"},
        {"dap_chain_esbocs_get_collecting_level", (PyCFunction)dap_chain_esbocs_get_collecting_level_py, METH_VARARGS,
         "Get ESBOCS collecting level"},
        {"dap_chain_esbocs_get_sign_key", (PyCFunction)dap_chain_esbocs_get_sign_key_py, METH_VARARGS,
         "Get ESBOCS sign key"},
        {"dap_chain_esbocs_set_min_validators_count", (PyCFunction)dap_chain_esbocs_set_min_validators_count_py, METH_VARARGS,
         "Set ESBOCS minimum validators count"},
        {"dap_chain_esbocs_get_min_validators_count", (PyCFunction)dap_chain_esbocs_get_min_validators_count_py, METH_VARARGS,
         "Get ESBOCS minimum validators count"},
        {"dap_chain_esbocs_set_emergency_validator", (PyCFunction)dap_chain_esbocs_set_emergency_validator_py, METH_VARARGS,
         "Set ESBOCS emergency validator"},
        {"dap_chain_esbocs_set_signs_struct_check", (PyCFunction)dap_chain_esbocs_set_signs_struct_check_py, METH_VARARGS,
         "Set ESBOCS signs structure check"},
        {"dap_chain_esbocs_set_hardfork_prepare", (PyCFunction)dap_chain_esbocs_set_hardfork_prepare_py, METH_VARARGS,
         "Prepare ESBOCS hardfork"},
        {"dap_chain_esbocs_set_hardfork_complete", (PyCFunction)dap_chain_esbocs_set_hardfork_complete_py, METH_VARARGS,
         "Complete ESBOCS hardfork"},
        {"dap_chain_esbocs_hardfork_engaged", (PyCFunction)dap_chain_esbocs_hardfork_engaged_py, METH_VARARGS,
         "Check ESBOCS hardfork engaged"},
        {"dap_chain_esbocs_set_hardfork_state", (PyCFunction)dap_chain_esbocs_set_hardfork_state_py, METH_VARARGS,
         "Set ESBOCS hardfork state"},
        {"dap_chain_esbocs_set_custom_metadata_callback", (PyCFunction)dap_chain_esbocs_set_custom_metadata_callback_py, METH_VARARGS,
         "Set ESBOCS custom metadata callback"},
        {"dap_chain_esbocs_set_presign_callback", (PyCFunction)dap_chain_esbocs_set_presign_callback_py, METH_VARARGS,
         "Set ESBOCS presign callback"},
        {"dap_chain_esbocs_set_empty_block_every_times", (PyCFunction)dap_chain_esbocs_set_empty_block_every_times_py, METH_VARARGS,
         "Set ESBOCS empty block generation period"},

        // ESBOCS decree handlers
        {"dap_chain_cs_esbocs_decree_init", (PyCFunction)dap_chain_cs_esbocs_decree_init_py, METH_VARARGS,
         "Initialize ESBOCS decree handlers"},
        {"dap_chain_cs_esbocs_decree_deinit", (PyCFunction)dap_chain_cs_esbocs_decree_deinit_py, METH_VARARGS,
         "Deinitialize ESBOCS decree handlers"},

        // Consensus core wrappers
        {"dap_chain_cs_init", (PyCFunction)dap_chain_cs_init_py, METH_VARARGS,
         "Initialize consensus subsystem globally"},
        {"dap_chain_cs_deinit", (PyCFunction)dap_chain_cs_deinit_py, METH_VARARGS,
         "Deinitialize consensus subsystem globally"},
        {"dap_chain_cs_add", (PyCFunction)dap_chain_cs_add_py, METH_VARARGS,
         "Register consensus implementation"},
        {"dap_chain_cs_create", (PyCFunction)dap_chain_cs_create_py, METH_VARARGS,
         "Create consensus from config"},
        {"dap_chain_cs_load", (PyCFunction)dap_chain_cs_load_py, METH_VARARGS,
         "Load consensus from config"},
        {"dap_chain_cs_start", (PyCFunction)dap_chain_cs_start_py, METH_VARARGS,
         "Start consensus"},
        {"dap_chain_cs_stop", (PyCFunction)dap_chain_cs_stop_py, METH_VARARGS,
         "Stop consensus"},
        {"dap_chain_cs_purge", (PyCFunction)dap_chain_cs_purge_py, METH_VARARGS,
         "Purge consensus"},
        {"dap_chain_cs_set_callbacks", (PyCFunction)dap_chain_cs_set_callbacks_py, METH_VARARGS,
         "Set callbacks for chain"},
        {"dap_chain_cs_get_callbacks", (PyCFunction)dap_chain_cs_get_callbacks_py, METH_VARARGS,
         "Get registered callbacks for chain"},
        {"dap_chain_cs_get_fee_group", (PyCFunction)dap_chain_cs_get_fee_group_py, METH_VARARGS,
         "Get fee group for consensus"},
        {"dap_chain_cs_get_reward_group", (PyCFunction)dap_chain_cs_get_reward_group_py, METH_VARARGS,
         "Get reward group for consensus"},
        {"dap_chain_cs_get_fee", (PyCFunction)dap_chain_cs_get_fee_py, METH_VARARGS,
         "Get fee for consensus"},
        {"dap_chain_cs_get_sign_pkey", (PyCFunction)dap_chain_cs_get_sign_pkey_py, METH_VARARGS,
         "Get sign public key for consensus"},
        {"dap_chain_cs_get_sign_key", (PyCFunction)dap_chain_cs_get_sign_key_py, METH_VARARGS,
         "Get sign key for consensus"},
        {"dap_chain_cs_get_collecting_level", (PyCFunction)dap_chain_cs_get_collecting_level_py, METH_VARARGS,
         "Get collecting level for consensus"},
        {"dap_chain_cs_get_autocollect_status", (PyCFunction)dap_chain_cs_get_autocollect_status_py, METH_VARARGS,
         "Get autocollect status for consensus"},
        {"dap_chain_cs_add_block_collect", (PyCFunction)dap_chain_cs_add_block_collect_py, METH_VARARGS,
         "Add block collect for consensus"},
        {"dap_chain_cs_set_hardfork_state", (PyCFunction)dap_chain_cs_set_hardfork_state_py, METH_VARARGS,
         "Set hardfork state for consensus"},
        {"dap_chain_cs_hardfork_engaged", (PyCFunction)dap_chain_cs_hardfork_engaged_py, METH_VARARGS,
         "Check if hardfork is engaged"},
        {"dap_chain_cs_get_min_validators_count", (PyCFunction)dap_chain_cs_get_min_validators_count_py, METH_VARARGS,
         "Get minimum validators count for consensus"},
        {"dap_chain_cs_set_min_validators_count", (PyCFunction)dap_chain_cs_set_min_validators_count_py, METH_VARARGS,
         "Set minimum validators count for consensus"},
        {"dap_chain_cs_add_validator", (PyCFunction)dap_chain_cs_add_validator_py, METH_VARARGS,
         "Add validator to consensus"},
        {"dap_chain_cs_remove_validator", (PyCFunction)dap_chain_cs_remove_validator_py, METH_VARARGS,
         "Remove validator from consensus"},
        {"dap_chain_cs_is_started", (PyCFunction)dap_chain_cs_is_started_py, METH_VARARGS,
         "Check if consensus is started"},
        {"dap_chain_cs_mempool_datum_add", (PyCFunction)dap_chain_cs_mempool_datum_add_py, METH_VARARGS,
         "Add datum to mempool via consensus"},
        {"dap_chain_cs_stake_check_pkey_hash", (PyCFunction)dap_chain_cs_stake_check_pkey_hash_py, METH_VARARGS,
         "Check stake pkey hash"},
        {"dap_chain_cs_stake_hardfork_data_import", (PyCFunction)dap_chain_cs_stake_hardfork_data_import_py, METH_VARARGS,
         "Import stake hardfork data"},
        {"dap_chain_cs_stake_switch_table", (PyCFunction)dap_chain_cs_stake_switch_table_py, METH_VARARGS,
         "Switch stake table"},

        // Validator API
        {"dap_chain_validator_api_init", (PyCFunction)dap_chain_validator_api_init_py, METH_VARARGS,
         "Initialize validator API"},
        {"dap_chain_validator_api_deinit", (PyCFunction)dap_chain_validator_api_deinit_py, METH_VARARGS,
         "Deinitialize validator API"},
        {"dap_chain_validator_api_register", (PyCFunction)dap_chain_validator_api_register_py, METH_VARARGS,
         "Register validator API implementation"},
        {"dap_chain_validator_api_get_validators", (PyCFunction)dap_chain_validator_api_get_validators_py, METH_VARARGS,
         "Get validators list"},
        {"dap_chain_validator_api_check_key_delegated", (PyCFunction)dap_chain_validator_api_check_key_delegated_py, METH_VARARGS,
         "Check if key is delegated"},
        {"dap_chain_validator_api_mark_validator_active", (PyCFunction)dap_chain_validator_api_mark_validator_active_py, METH_VARARGS,
         "Mark validator active/inactive"},
        {"dap_chain_validator_api_get_pkey_by_hash", (PyCFunction)dap_chain_validator_api_get_pkey_by_hash_py, METH_VARARGS,
         "Get public key by hash"},
        {"dap_chain_validator_api_key_delegate", (PyCFunction)dap_chain_validator_api_key_delegate_py, METH_VARARGS,
         "Delegate key via validator API"},
        {"dap_chain_validator_api_get_allowed_min_value", (PyCFunction)dap_chain_validator_api_get_allowed_min_value_py, METH_VARARGS,
         "Get minimum allowed stake value"},
        {"dap_chain_validator_api_hardfork_tx_update", (PyCFunction)dap_chain_validator_api_hardfork_tx_update_py, METH_VARARGS,
         "Update hardfork transaction via validator API"},
        {"dap_chain_validator_api_get_node_addr", (PyCFunction)dap_chain_validator_api_get_node_addr_py, METH_VARARGS,
         "Get node address from validator item"},
        {"dap_chain_validator_api_get_value", (PyCFunction)dap_chain_validator_api_get_value_py, METH_VARARGS,
         "Get value from validator item"},

        // Legacy names (chain_cs_*)
        {"chain_cs_init", (PyCFunction)dap_chain_cs_init_py, METH_VARARGS,
         "Initialize consensus subsystem globally"},
        {"chain_cs_deinit", (PyCFunction)dap_chain_cs_deinit_py, METH_VARARGS,
         "Deinitialize consensus subsystem globally"},
        {"chain_cs_create", (PyCFunction)dap_chain_cs_create_py, METH_VARARGS,
         "Create consensus from config"},
        {"chain_cs_load", (PyCFunction)dap_chain_cs_load_py, METH_VARARGS,
         "Load consensus from config"},
        {"chain_cs_start", (PyCFunction)dap_chain_cs_start_py, METH_VARARGS,
         "Start consensus"},
        {"chain_cs_stop", (PyCFunction)dap_chain_cs_stop_py, METH_VARARGS,
         "Stop consensus"},
        {"chain_cs_purge", (PyCFunction)dap_chain_cs_purge_py, METH_VARARGS,
         "Purge consensus"},
        {"chain_cs_set_callbacks", (PyCFunction)dap_chain_cs_set_callbacks_py, METH_VARARGS,
         "Set callbacks for chain"},
        {"chain_cs_get_callbacks", (PyCFunction)dap_chain_cs_get_callbacks_py, METH_VARARGS,
         "Get registered callbacks for chain"},
        {"chain_cs_get_fee_group", (PyCFunction)dap_chain_cs_get_fee_group_py, METH_VARARGS,
         "Get fee group for consensus"},
        {"chain_cs_get_reward_group", (PyCFunction)dap_chain_cs_get_reward_group_py, METH_VARARGS,
         "Get reward group for consensus"},
        {"chain_cs_get_fee", (PyCFunction)dap_chain_cs_get_fee_py, METH_VARARGS,
         "Get fee for consensus"},
        {"chain_cs_get_sign_pkey", (PyCFunction)dap_chain_cs_get_sign_pkey_py, METH_VARARGS,
         "Get sign public key for consensus"},
        {"chain_cs_get_collecting_level", (PyCFunction)dap_chain_cs_get_collecting_level_py, METH_VARARGS,
         "Get collecting level for consensus"},
        {"chain_cs_get_autocollect_status", (PyCFunction)dap_chain_cs_get_autocollect_status_py, METH_VARARGS,
         "Get autocollect status for consensus"},
        {"chain_cs_set_hardfork_state", (PyCFunction)dap_chain_cs_set_hardfork_state_py, METH_VARARGS,
         "Set hardfork state for consensus"},
        {"chain_cs_hardfork_engaged", (PyCFunction)dap_chain_cs_hardfork_engaged_py, METH_VARARGS,
         "Check if hardfork is engaged"},

        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return consensus_methods;
}
