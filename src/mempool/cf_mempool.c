#include "cellframe.h"
#include "dap_chain_cs.h"
#include "dap_chain.h"
#include "dap_chain_datum.h"
#include "dap_chain_mempool.h"
#include "dap_chain_mempool_cli.h"
#include "dap_enc_key.h"
#include "dap_math_ops.h"
#include "dap_global_db.h"
#include "dap_json.h"
#include "cf_common_utils.h"

#define LOG_TAG "python_cellframe_mempool"

// =============================================================================
// MEMPOOL WRAPPERS
// =============================================================================

static int cf_mempool_cli_build_argv(PyObject *argv_list, char ***argv_out, int *argc_out) {
    if (!PyList_Check(argv_list)) {
        PyErr_SetString(PyExc_TypeError, "argv must be a list");
        return -1;
    }

    Py_ssize_t argc = PyList_Size(argv_list);
    char **argv = DAP_NEW_Z_COUNT(char*, (size_t)argc + 1);
    if (!argv) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate argv");
        return -1;
    }

    for (Py_ssize_t i = 0; i < argc; i++) {
        PyObject *item = PyList_GetItem(argv_list, i);
        if (PyUnicode_Check(item)) {
            argv[i] = (char *)PyUnicode_AsUTF8(item);
        } else if (PyBytes_Check(item)) {
            argv[i] = PyBytes_AsString(item);
        } else {
            PyErr_SetString(PyExc_TypeError, "argv items must be str or bytes");
            DAP_DELETE(argv);
            return -1;
        }

        if (!argv[i]) {
            PyErr_SetString(PyExc_ValueError, "Failed to parse argv item");
            DAP_DELETE(argv);
            return -1;
        }
    }

    argv[argc] = NULL;
    *argv_out = argv;
    *argc_out = (int)argc;
    return 0;
}

static void cf_mempool_cli_free_argv(char **argv) {
    DAP_DELETE(argv);
}

static PyObject* cf_mempool_cli_exec(int (*func)(int, char **, dap_json_t *, int),
                                     const char *func_name,
                                     PyObject *argv_list,
                                     PyObject *json_reply_obj,
                                     int version) {
    if (!func) {
        PyErr_Format(PyExc_RuntimeError, "CLI function '%s' is not linked", func_name);
        return NULL;
    }

    char **argv = NULL;
    int argc = 0;
    if (cf_mempool_cli_build_argv(argv_list, &argv, &argc) != 0) {
        return NULL;
    }

    dap_json_t *json_reply = NULL;
    bool created = false;
    if (json_reply_obj && json_reply_obj != Py_None) {
        if (!PyCapsule_CheckExact(json_reply_obj)) {
            cf_mempool_cli_free_argv(argv);
            PyErr_SetString(PyExc_TypeError, "json_reply must be a JSON capsule or None");
            return NULL;
        }
        json_reply = (dap_json_t *)PyCapsule_GetPointer(json_reply_obj, "dap_json_t");
        if (!json_reply) {
            cf_mempool_cli_free_argv(argv);
            PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
            return NULL;
        }
    } else {
        json_reply = dap_json_array_new();
        if (!json_reply) {
            cf_mempool_cli_free_argv(argv);
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate JSON reply");
            return NULL;
        }
        created = true;
    }

    int result = func(argc, argv, json_reply, version);
    cf_mempool_cli_free_argv(argv);

    PyObject *json_str_obj = Py_None;
    if (json_reply) {
        char *json_str = dap_json_to_string(json_reply);
        if (json_str) {
            json_str_obj = PyUnicode_FromString(json_str);
            DAP_DELETE(json_str);
        } else {
            Py_INCREF(Py_None);
        }
    } else {
        Py_INCREF(Py_None);
    }

    if (created && json_reply) {
        dap_json_object_free(json_reply);
    }

    PyObject *result_tuple = PyTuple_New(2);
    PyTuple_SetItem(result_tuple, 0, PyLong_FromLong(result));
    PyTuple_SetItem(result_tuple, 1, json_str_obj);
    return result_tuple;
}

/**
 * @brief Add datum to mempool
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, datum capsule, hash_out_type string)
 * @return String hash or None
 */
static PyObject* dap_chain_cs_mempool_datum_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_datum_obj;
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
        log_it(L_DEBUG, "Failed to add datum to mempool");
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = PyUnicode_FromString(l_hash);
    log_it(L_DEBUG, "Added datum to mempool, hash: %s", l_hash);
    return l_result;
}

/**
 * @brief Create transaction in mempool
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain, key_from, addr_from, addr_to_list, ticker, value, fee, hash_out_type)
 * @return String hash or None
 */
PyObject* dap_chain_mempool_tx_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj, *l_key_from_obj, *l_addr_from_obj, *l_addr_to_list, *l_value_obj;
    const char *l_ticker;
    unsigned long long l_fee_uint;
    const char *l_hash_out_type = "hex";
    
    if (!PyArg_ParseTuple(a_args, "OOOOssK|s", &l_chain_obj, &l_key_from_obj, &l_addr_from_obj, 
                          &l_addr_to_list, &l_ticker, &l_value_obj, &l_fee_uint, &l_hash_out_type)) {
        return NULL;
    }
    
    // Validate and extract capsules
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "chain must be a capsule");
        return NULL;
    }
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    
    if (!PyCapsule_CheckExact(l_key_from_obj)) {
        PyErr_SetString(PyExc_TypeError, "key_from must be a capsule");
        return NULL;
    }
    dap_enc_key_t *l_key_from = (dap_enc_key_t *)PyCapsule_GetPointer(l_key_from_obj, "dap_enc_key_t");
    
    if (!PyCapsule_CheckExact(l_addr_from_obj)) {
        PyErr_SetString(PyExc_TypeError, "addr_from must be a capsule");
        return NULL;
    }
    dap_chain_addr_t *l_addr_from = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_from_obj, "dap_chain_addr_t");
    
    if (!PyList_Check(l_addr_to_list)) {
        PyErr_SetString(PyExc_TypeError, "addr_to_list must be a list");
        return NULL;
    }
    
    // Process addr_to list
    size_t l_count = PyList_Size(l_addr_to_list);
    if (l_count == 0) {
        PyErr_SetString(PyExc_ValueError, "addr_to_list cannot be empty");
        return NULL;
    }

    dap_chain_addr_t **l_addr_to_arr = DAP_NEW_Z_COUNT(dap_chain_addr_t*, l_count);
    if (!l_addr_to_arr) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for addresses");
        return NULL;
    }
    
    for (size_t i = 0; i < l_count; i++) {
        PyObject *l_item = PyList_GetItem(l_addr_to_list, i);
        if (!PyCapsule_CheckExact(l_item)) {
            DAP_DELETE(l_addr_to_arr);
            PyErr_SetString(PyExc_TypeError, "addr_to_list items must be capsules");
            return NULL;
        }
        l_addr_to_arr[i] = (dap_chain_addr_t *)PyCapsule_GetPointer(l_item, "dap_chain_addr_t");
    }
    
    // Process values
    uint256_t *l_value_arr = DAP_NEW_Z_COUNT(uint256_t, l_count);
    if (!l_value_arr) {
        DAP_DELETE(l_addr_to_arr);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for values");
        return NULL;
    }

    if (PyList_Check(l_value_obj)) {
        if (PyList_Size(l_value_obj) != (Py_ssize_t)l_count) {
            DAP_DELETE(l_addr_to_arr);
            DAP_DELETE(l_value_arr);
            PyErr_SetString(PyExc_ValueError, "value list size must match address list size");
            return NULL;
        }
        for (size_t i = 0; i < l_count; i++) {
            PyObject *l_item = PyList_GetItem(l_value_obj, i);
            if (!PyLong_Check(l_item)) {
                DAP_DELETE(l_addr_to_arr);
                DAP_DELETE(l_value_arr);
                PyErr_SetString(PyExc_TypeError, "value list items must be integers");
                return NULL;
            }
            l_value_arr[i] = GET_256_FROM_64(PyLong_AsUnsignedLongLong(l_item));
        }
    } else if (PyLong_Check(l_value_obj)) {
        uint256_t l_val = GET_256_FROM_64(PyLong_AsUnsignedLongLong(l_value_obj));
        for (size_t i = 0; i < l_count; i++) {
            l_value_arr[i] = l_val;
        }
    } else {
        DAP_DELETE(l_addr_to_arr);
        DAP_DELETE(l_value_arr);
        PyErr_SetString(PyExc_TypeError, "value must be an integer or a list of integers");
        return NULL;
    }

    uint256_t l_fee = GET_256_FROM_64(l_fee_uint);
    
    // Allocate time_unlock array (initialized to zero by NEW_Z)
    dap_time_t *l_time_unlock_arr = DAP_NEW_Z_COUNT(dap_time_t, l_count);
    if (!l_time_unlock_arr) {
        DAP_DELETE(l_addr_to_arr);
        DAP_DELETE(l_value_arr);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for timestamps");
        return NULL;
    }
    
    DAP_DELETE(l_addr_to_arr);
    DAP_DELETE(l_value_arr);
    DAP_DELETE(l_time_unlock_arr);
    
    // DEPRECATED: Old mempool TX creation API removed in Phase 3.5
    // Use new TX Compose API instead:
    //   1. Register TX builder: dap_chain_tx_compose_register(tx_type, builder_cb, ctx)
    //   2. Create TX via registry: dap_chain_tx_compose_create(tx_type, ledger, utxo, params)
    //   3. Add to mempool: dap_chain_mempool_datum_add(datum, chain, hash)
    // 
    // Python API: CellFrame.Chain.TxCompose.register(tx_type, builder_callback)
    //             CellFrame.Chain.TxCompose.create(tx_type, ledger, utxo_list, params)
    //
    // See: src/compose/cf_compose.c for Python bindings
    PyErr_SetString(PyExc_DeprecationWarning, 
        "dap_chain_mempool_tx_create: Deprecated. Use CellFrame.Chain.TxCompose API instead");
    return NULL;
}

/**
 * @brief Get datum from mempool
 * @param a_self Python self object (unused)
 * @param a_args Arguments (chain capsule, hash string)
 * @return Datum capsule or None
 * 
 * @note After SDK refactoring, dap_chain_mempool_datum_get() was removed.
 *       Now using dap_global_db_get_sync() with mempool GDB group.
 */
PyObject* dap_chain_mempool_datum_get_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj;
    const char *l_hash_str;
    
    if (!PyArg_ParseTuple(a_args, "Os", &l_chain_obj, &l_hash_str)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "chain must be a capsule");
        return NULL;
    }
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    // Get mempool GDB group name for this chain
    char *l_gdb_group_mempool = dap_chain_mempool_group_new(l_chain);
    if (!l_gdb_group_mempool) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get mempool GDB group");
        return NULL;
    }
    
    // Get datum from Global DB
    size_t l_datum_size = 0;
    dap_chain_datum_t *l_datum = (dap_chain_datum_t*)dap_global_db_get_sync(
        l_gdb_group_mempool,
        l_hash_str,
        &l_datum_size,
        NULL,
        NULL
    );
    
    DAP_DELETE(l_gdb_group_mempool);
    
    if (!l_datum) {
        Py_RETURN_NONE;
    }
    
    // Verify datum size consistency
    size_t l_datum_size2 = dap_chain_datum_size(l_datum);
    if (l_datum_size != l_datum_size2) {
        log_it(L_ERROR, "Corrupted datum %s: size by headers %zu != GDB size %zu",
               l_hash_str, l_datum_size2, l_datum_size);
        DAP_DELETE(l_datum);
        PyErr_SetString(PyExc_RuntimeError, "Corrupted datum in mempool");
        return NULL;
    }
    
    return PyCapsule_New(l_datum, "dap_chain_datum_t", NULL);
}

PyObject* dap_chain_mempool_add_proc_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_http_server_obj = NULL;
    const char *l_url = NULL;

    if (!PyArg_ParseTuple(a_args, "Os", &l_http_server_obj, &l_url)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_http_server_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a http server capsule");
        return NULL;
    }

    dap_http_server_t *l_http_server = (dap_http_server_t *)PyCapsule_GetPointer(l_http_server_obj, "dap_http_server_t");
    if (!l_http_server) {
        PyErr_SetString(PyExc_ValueError, "Invalid http server capsule");
        return NULL;
    }

    dap_chain_mempool_add_proc(l_http_server, l_url);
    Py_RETURN_NONE;
}

PyObject* dap_chain_mempool_group_new_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "chain must be a capsule");
        return NULL;
    }
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    char *l_group = dap_chain_mempool_group_new(l_chain);
    if (!l_group) {
        Py_RETURN_NONE;
    }

    PyObject *l_group_obj = PyUnicode_FromString(l_group);
    DAP_DELETE(l_group);
    return l_group_obj;
}

PyObject* dap_chain_mempool_delete_callback_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int rc = dap_chain_mempool_delete_callback_init();
    return PyLong_FromLong(rc);
}

PyObject* dap_datum_mempool_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int rc = dap_datum_mempool_init();
    return PyLong_FromLong(rc);
}

PyObject* dap_chain_mempool_filter_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "chain must be a capsule");
        return NULL;
    }
    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    int l_removed = 0;
    dap_chain_mempool_filter(l_chain, &l_removed);
    return PyLong_FromLong(l_removed);
}

PyObject* dap_chain_mempool_out_is_used_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_hash_obj = NULL;
    unsigned int l_out_idx = 0;

    if (!PyArg_ParseTuple(a_args, "OOI", &l_net_obj, &l_hash_obj, &l_out_idx)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "net must be a capsule");
        return NULL;
    }
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid net capsule");
        return NULL;
    }

    dap_hash_fast_t l_hash = {0};
    if (cf_common_parse_hash_fast(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    bool l_used = dap_chain_mempool_out_is_used(l_net, &l_hash, l_out_idx);
    return PyBool_FromLong(l_used);
}

PyObject* dap_chain_mempool_cli_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int rc = dap_chain_mempool_cli_init();
    return PyLong_FromLong(rc);
}

PyObject* dap_chain_mempool_cli_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_mempool_cli_deinit();
    Py_RETURN_NONE;
}

PyObject* mempool_com_mempool_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_argv_list = NULL;
    PyObject *l_json_reply_obj = Py_None;
    int l_version = 1;

    if (!PyArg_ParseTuple(a_args, "O|Oi", &l_argv_list, &l_json_reply_obj, &l_version)) {
        return NULL;
    }

    return cf_mempool_cli_exec(com_mempool, "com_mempool", l_argv_list, l_json_reply_obj, l_version);
}

extern int com_mempool_add(int a_argc, char **a_argv, dap_json_t *a_json_arr_reply, int a_version);

PyObject* mempool_com_mempool_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_argv_list = NULL;
    PyObject *l_json_reply_obj = Py_None;
    int l_version = 1;

    if (!PyArg_ParseTuple(a_args, "O|Oi", &l_argv_list, &l_json_reply_obj, &l_version)) {
        return NULL;
    }

    return cf_mempool_cli_exec(com_mempool_add, "com_mempool_add", l_argv_list, l_json_reply_obj, l_version);
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

/**
 * @brief Get method definitions for mempool module
 */
PyMethodDef* cellframe_mempool_get_methods(void) {
    static PyMethodDef mempool_methods[] = {
        {"chain_cs_mempool_datum_add", (PyCFunction)dap_chain_cs_mempool_datum_add_py, METH_VARARGS,
         "Add datum to mempool (consensus)"},
        {"chain_mempool_tx_create", (PyCFunction)dap_chain_mempool_tx_create_py, METH_VARARGS,
         "Create transaction in mempool"},
        {"chain_mempool_datum_get", (PyCFunction)dap_chain_mempool_datum_get_py, METH_VARARGS,
         "Get datum from mempool"},
        {"dap_chain_mempool_add_proc", (PyCFunction)dap_chain_mempool_add_proc_py, METH_VARARGS,
         "Register mempool HTTP processor"},
        {"dap_chain_mempool_group_new", (PyCFunction)dap_chain_mempool_group_new_py, METH_VARARGS,
         "Get mempool group name for chain"},
        {"dap_chain_mempool_delete_callback_init", (PyCFunction)dap_chain_mempool_delete_callback_init_py, METH_NOARGS,
         "Initialize mempool delete callback"},
        {"dap_datum_mempool_init", (PyCFunction)dap_datum_mempool_init_py, METH_NOARGS,
         "Initialize mempool subsystem"},
        {"dap_chain_mempool_filter", (PyCFunction)dap_chain_mempool_filter_py, METH_VARARGS,
         "Filter mempool datums"},
        {"dap_chain_mempool_out_is_used", (PyCFunction)dap_chain_mempool_out_is_used_py, METH_VARARGS,
         "Check if output is used in mempool"},
        {"dap_chain_mempool_cli_init", (PyCFunction)dap_chain_mempool_cli_init_py, METH_NOARGS,
         "Initialize mempool CLI"},
        {"dap_chain_mempool_cli_deinit", (PyCFunction)dap_chain_mempool_cli_deinit_py, METH_NOARGS,
         "Deinitialize mempool CLI"},
        {"com_mempool", (PyCFunction)mempool_com_mempool_py, METH_VARARGS,
         "Mempool CLI command"},
        {"com_mempool_add", (PyCFunction)mempool_com_mempool_add_py, METH_VARARGS,
         "Add TX from JSON to mempool (CLI)"},
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return mempool_methods;
}
