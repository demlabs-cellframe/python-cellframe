#include "cellframe.h"
#include "dap_chain_net.h"
#include "dap_chain_net_core.h"
#include "dap_chain_net_tx.h"
#include "dap_chain_common.h"
#include "dap_chain_datum.h"
#include "dap_common.h"
#include "dap_global_db_cluster.h"
#include "cf_callbacks_registry.h"  // CRITICAL: For memory leak prevention

#define LOG_TAG "python_cellframe_network"

// Submodule init hooks (net/tx, net/ch, net/cli, net/decree)
int cellframe_net_tx_init(PyObject *a_module);
int cellframe_net_ch_init(PyObject *a_module);
int cellframe_net_ch_pkt_init(PyObject *a_module);
int cellframe_net_cli_init(PyObject *a_module);
int cellframe_net_decree_init(PyObject *a_module);

static void cf_network_list_free(void *data) {
    DAP_DELETE(data);
}

static int cf_network_build_argv(PyObject *argv_list, char ***argv_out, int *argc_out) {
    if (!PyList_Check(argv_list) && !PyTuple_Check(argv_list)) {
        PyErr_SetString(PyExc_TypeError, "argv must be a list or tuple");
        return -1;
    }

    Py_ssize_t argc = PySequence_Size(argv_list);
    if (argc < 0) {
        return -1;
    }

    char **argv = DAP_NEW_Z_COUNT(char *, (size_t)argc + 1);
    if (!argv) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate argv");
        return -1;
    }

    for (Py_ssize_t i = 0; i < argc; i++) {
        PyObject *item = PySequence_GetItem(argv_list, i);
        if (!item) {
            DAP_DELETE(argv);
            return -1;
        }

        if (PyUnicode_Check(item)) {
            argv[i] = (char *)PyUnicode_AsUTF8(item);
        } else if (PyBytes_Check(item)) {
            argv[i] = PyBytes_AsString(item);
        } else {
            Py_DECREF(item);
            PyErr_SetString(PyExc_TypeError, "argv items must be str or bytes");
            DAP_DELETE(argv);
            return -1;
        }

        Py_DECREF(item);
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

static void cf_network_free_argv(char **argv) {
    DAP_DELETE(argv);
}

static int cf_network_parse_uint256(PyObject *obj, uint256_t *out) {
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
        Py_ssize_t size = 0;
        const char *data = NULL;
        if (PyBytes_AsStringAndSize(obj, (char **)&data, &size) < 0) {
            PyErr_SetString(PyExc_ValueError, "Failed to read uint256 bytes");
            return -1;
        }
        if ((size_t)size != sizeof(uint256_t)) {
            PyErr_Format(PyExc_ValueError, "uint256 value must be exactly %zu bytes", sizeof(uint256_t));
            return -1;
        }
        memcpy(out, data, sizeof(uint256_t));
        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "uint256 value must be bytes or uint256_t capsule");
    return -1;
}

static int cf_network_parse_json_reply(PyObject *json_reply_obj, dap_json_t **json_out, bool *created) {
    *json_out = NULL;
    *created = false;

    if (json_reply_obj && json_reply_obj != Py_None) {
        if (!PyCapsule_CheckExact(json_reply_obj)) {
            PyErr_SetString(PyExc_TypeError, "json_reply must be a JSON capsule or None");
            return -1;
        }
        *json_out = (dap_json_t *)PyCapsule_GetPointer(json_reply_obj, "dap_json_t");
        if (!*json_out) {
            PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
            return -1;
        }
        return 0;
    }

    *json_out = dap_json_array_new();
    if (!*json_out) {
        PyErr_SetString(PyExc_MemoryError, "Failed to create JSON reply");
        return -1;
    }
    *created = true;
    return 0;
}

static PyObject *cf_network_json_to_str(dap_json_t *json) {
    if (!json) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    char *json_str = dap_json_to_string(json);
    if (!json_str) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    PyObject *result = PyUnicode_FromString(json_str);
    DAP_DELETE(json_str);
    return result ? result : NULL;
}

typedef struct cf_network_datum_filter_ctx {
    PyObject *callback;
    bool error;
} cf_network_datum_filter_ctx_t;

static bool cf_network_datum_filter_cb(dap_chain_datum_t *a_datum, dap_chain_t *a_chain, void *a_param) {
    cf_network_datum_filter_ctx_t *l_ctx = (cf_network_datum_filter_ctx_t *)a_param;
    if (!l_ctx || !l_ctx->callback) {
        return true;
    }

    PyGILState_STATE l_gil = PyGILState_Ensure();
    PyObject *l_datum_capsule = PyCapsule_New(a_datum, "dap_chain_datum_t", NULL);
    PyObject *l_chain_capsule = PyCapsule_New(a_chain, "dap_chain_t", NULL);
    if (!l_datum_capsule || !l_chain_capsule) {
        Py_XDECREF(l_datum_capsule);
        Py_XDECREF(l_chain_capsule);
        l_ctx->error = true;
        PyGILState_Release(l_gil);
        return false;
    }

    PyObject *l_result = PyObject_CallFunctionObjArgs(l_ctx->callback, l_datum_capsule, l_chain_capsule, NULL);
    Py_DECREF(l_datum_capsule);
    Py_DECREF(l_chain_capsule);

    if (!l_result) {
        l_ctx->error = true;
        PyGILState_Release(l_gil);
        return false;
    }

    int l_truth = PyObject_IsTrue(l_result);
    Py_DECREF(l_result);
    if (l_truth < 0) {
        l_ctx->error = true;
        PyGILState_Release(l_gil);
        return false;
    }

    PyGILState_Release(l_gil);
    return l_truth ? true : false;
}

/**
 * @brief Initialize network core module (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return 0 on success, negative on error
 */
PyObject* dap_chain_net_core_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    int l_result = dap_chain_net_core_init();
    return PyLong_FromLong(l_result);
}

/**
 * @brief Register network in core registry (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return None
 */
PyObject* dap_chain_net_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_net_register(l_net);
    Py_RETURN_NONE;
}

/**
 * @brief Unregister network from core registry (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return None
 */
PyObject* dap_chain_net_unregister_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_net_unregister(l_net);
    Py_RETURN_NONE;
}

/**
 * @brief Parse -net and -chain arguments from CLI (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (argv list/tuple, json_reply=None, arg_index=0, default_chain_type=CHAIN_TYPE_INVALID)
 * @return Tuple (rc, arg_index, chain capsule or None, net capsule or None, json string or None)
 */
PyObject* dap_chain_net_parse_net_chain_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_argv_obj = NULL;
    PyObject *l_json_reply_obj = Py_None;
    int l_arg_index = 0;
    int l_default_chain_type = CHAIN_TYPE_INVALID;

    if (!PyArg_ParseTuple(a_args, "O|Oii", &l_argv_obj, &l_json_reply_obj, &l_arg_index, &l_default_chain_type)) {
        return NULL;
    }

    char **l_argv = NULL;
    int l_argc = 0;
    if (cf_network_build_argv(l_argv_obj, &l_argv, &l_argc) != 0) {
        return NULL;
    }

    dap_json_t *l_json_reply = NULL;
    bool l_json_created = false;
    if (cf_network_parse_json_reply(l_json_reply_obj, &l_json_reply, &l_json_created) != 0) {
        cf_network_free_argv(l_argv);
        return NULL;
    }

    dap_chain_t *l_chain = NULL;
    dap_chain_net_t *l_net = NULL;
    int l_rc = dap_chain_net_parse_net_chain(l_json_reply, &l_arg_index, l_argc, l_argv,
                                             &l_chain, &l_net, (dap_chain_type_t)l_default_chain_type);

    cf_network_free_argv(l_argv);

    PyObject *l_json_str_obj = cf_network_json_to_str(l_json_reply);
    if (!l_json_str_obj) {
        if (l_json_created && l_json_reply) {
            dap_json_object_free(l_json_reply);
        }
        return NULL;
    }

    if (l_json_created && l_json_reply) {
        dap_json_object_free(l_json_reply);
    }

    PyObject *l_chain_obj = l_chain ? PyCapsule_New(l_chain, "dap_chain_t", NULL) : Py_None;
    if (!l_chain_obj) {
        Py_DECREF(l_json_str_obj);
        return NULL;
    }
    if (l_chain_obj == Py_None) {
        Py_INCREF(Py_None);
    }

    PyObject *l_net_obj = l_net ? PyCapsule_New(l_net, "dap_chain_net_t", NULL) : Py_None;
    if (!l_net_obj) {
        Py_DECREF(l_chain_obj);
        Py_DECREF(l_json_str_obj);
        return NULL;
    }
    if (l_net_obj == Py_None) {
        Py_INCREF(Py_None);
    }

    PyObject *l_tuple = PyTuple_New(5);
    if (!l_tuple) {
        Py_DECREF(l_chain_obj);
        Py_DECREF(l_net_obj);
        Py_DECREF(l_json_str_obj);
        return NULL;
    }

    PyObject *l_rc_obj = PyLong_FromLong(l_rc);
    PyObject *l_arg_index_obj = PyLong_FromLong(l_arg_index);
    if (!l_rc_obj || !l_arg_index_obj) {
        Py_XDECREF(l_rc_obj);
        Py_XDECREF(l_arg_index_obj);
        Py_DECREF(l_chain_obj);
        Py_DECREF(l_net_obj);
        Py_DECREF(l_json_str_obj);
        Py_DECREF(l_tuple);
        return NULL;
    }

    PyTuple_SetItem(l_tuple, 0, l_rc_obj);
    PyTuple_SetItem(l_tuple, 1, l_arg_index_obj);
    PyTuple_SetItem(l_tuple, 2, l_chain_obj);
    PyTuple_SetItem(l_tuple, 3, l_net_obj);
    PyTuple_SetItem(l_tuple, 4, l_json_str_obj);
    return l_tuple;
}

/**
 * @brief Get network by name (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "s", &l_net_name)) {
        log_it(L_ERROR, "Invalid arguments for net_by_name");
        return NULL;
    }
    
    // Additional validation
    if (!l_net_name || strlen(l_net_name) == 0) {
        log_it(L_ERROR, "Network name is NULL or empty");
        PyErr_SetString(PyExc_ValueError, "Network name cannot be empty");
        return NULL;
    }
    
    if (strlen(l_net_name) > DAP_CHAIN_NET_NAME_MAX) {
        log_it(L_ERROR, "Network name too long: %zu", strlen(l_net_name));
        PyErr_Format(PyExc_ValueError, "Network name too long (max %d)", DAP_CHAIN_NET_NAME_MAX);
        return NULL;
    }
    
    // Call DAP SDK function
    dap_chain_net_t *l_net = dap_chain_net_by_name(l_net_name);
    if (!l_net) {
        log_it(L_WARNING, "Network '%s' not found", l_net_name);
        PyErr_Format(PyExc_ValueError, "Network '%s' not found", l_net_name);
        return NULL;
    }
    
    return PyCapsule_New(l_net, "dap_chain_net_t", NULL);
}

/**
 * @brief Get network by ID (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_by_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id_uint = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id_uint)) {
        log_it(L_ERROR, "Invalid arguments for net_by_id");
        return NULL;
    }
    
    // Validate network ID
    if (l_net_id_uint == 0) {
        log_it(L_ERROR, "Network ID cannot be zero");
        PyErr_SetString(PyExc_ValueError, "Network ID cannot be zero");
        return NULL;
    }
    
    // Call DAP SDK function
    dap_chain_net_id_t l_net_id = {.uint64 = l_net_id_uint};
    dap_chain_net_t *l_net = dap_chain_net_by_id(l_net_id);
    
    if (!l_net) {
        log_it(L_WARNING, "Network with ID %llu not found", l_net_id_uint);
        PyErr_Format(PyExc_ValueError, "Network with ID %llu not found", l_net_id_uint);
        return NULL;
    }
    
    return PyCapsule_New(l_net, "dap_chain_net_t", NULL);
}

/**
 * @brief Get network ID by name (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network ID as uint64 or NULL with Python exception set
 */
PyObject* dap_chain_net_id_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "s", &l_net_name)) {
        log_it(L_ERROR, "Invalid arguments for net_id_by_name");
        return NULL;
    }
    
    // Additional validation
    if (!l_net_name || strlen(l_net_name) == 0) {
        log_it(L_ERROR, "Network name is NULL or empty");
        PyErr_SetString(PyExc_ValueError, "Network name cannot be empty");
        return NULL;
    }
    
    // Call DAP SDK function
    dap_chain_net_id_t l_net_id = dap_chain_net_id_by_name(l_net_name);
    
    if (!l_net_id.uint64) {
        log_it(L_WARNING, "Network '%s' not found", l_net_name);
        PyErr_Format(PyExc_ValueError, "Network '%s' not found", l_net_name);
        return NULL;
    }
    
    return Py_BuildValue("K", l_net_id.uint64);
}

/**
 * @brief Parse network ID from string (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network ID as uint64 or NULL with Python exception set
 */
PyObject* dap_chain_net_id_parse_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_id_str = NULL;

    if (!PyArg_ParseTuple(a_args, "s", &l_id_str)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id = {0};
    int l_result = dap_chain_net_id_parse(l_id_str, &l_net_id);
    if (l_result != 0) {
        PyErr_Format(PyExc_ValueError, "Invalid net id string: %s", l_id_str);
        return NULL;
    }

    return PyLong_FromUnsignedLongLong(l_net_id.uint64);
}

/**
 * @brief Get total count of networks (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return Number of networks
 */
PyObject* dap_chain_net_count_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    size_t l_count = dap_chain_net_count();
    log_it(L_DEBUG, "Total networks count: %zu", l_count);
    return Py_BuildValue("n", l_count);
}

/**
 * @brief Start iteration over networks (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return First network capsule or None if no networks
 */
PyObject* dap_chain_net_iter_start_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    dap_chain_net_t *l_net = dap_chain_net_iter_start();
    
    if (!l_net) {
        log_it(L_DEBUG, "No networks available for iteration");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Started network iteration, first net: %s", l_net->pub.name);
    return PyCapsule_New(l_net, "dap_chain_net_t", NULL);
}

/**
 * @brief Continue iteration over networks (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Next network capsule or None if end of iteration
 */
PyObject* dap_chain_net_iter_next_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_iter_next");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get next network
    dap_chain_net_t *l_net_next = dap_chain_net_iter_next(l_net);
    
    if (!l_net_next) {
        log_it(L_DEBUG, "End of network iteration");
        Py_RETURN_NONE;
    }
    
    log_it(L_DEBUG, "Next network in iteration: %s", l_net_next->pub.name);
    return PyCapsule_New(l_net_next, "dap_chain_net_t", NULL);
}

/**
 * @brief Iterate over networks (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (optional network capsule)
 * @return Next network capsule or None if end of iteration
 */
PyObject* dap_chain_net_iterate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = Py_None;

    if (!PyArg_ParseTuple(a_args, "|O", &l_net_capsule)) {
        return NULL;
    }

    dap_chain_net_t *l_net = NULL;
    if (l_net_capsule && l_net_capsule != Py_None) {
        if (!PyCapsule_CheckExact(l_net_capsule)) {
            PyErr_SetString(PyExc_TypeError, "Expected network capsule or None");
            return NULL;
        }
        l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
        if (!l_net) {
            PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
            return NULL;
        }
    }

    dap_chain_net_t *l_next = dap_chain_net_iterate(l_net);
    if (!l_next) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_next, "dap_chain_net_t", NULL);
}

/**
 * @brief Get network name from capsule (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network name string or NULL with Python exception set
 */
PyObject* dap_chain_net_get_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_name");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    return Py_BuildValue("s", l_net->pub.name);
}

/**
 * @brief Get network ID from capsule (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network ID as uint64 or NULL with Python exception set
 */
PyObject* dap_chain_net_get_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_id");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    return Py_BuildValue("K", l_net->pub.id.uint64);
}

/**
 * @brief Get network state (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Network state as int or NULL with Python exception set
 */
PyObject* dap_chain_net_get_state_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_state");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    dap_chain_net_state_t l_state = dap_chain_net_get_state(l_net);
    log_it(L_DEBUG, "Network '%s' state: %d", l_net->pub.name, (int)l_state);
    return Py_BuildValue("i", (int)l_state);
}

/**
 * @brief Convert network state to string (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (state int)
 * @return State string
 */
PyObject* dap_chain_net_state_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_state = 0;

    if (!PyArg_ParseTuple(a_args, "i", &l_state)) {
        return NULL;
    }

    const char *l_state_str = dap_chain_net_state_to_str((dap_chain_net_state_t)l_state);
    if (!l_state_str) {
        Py_RETURN_NONE;
    }

    return PyUnicode_FromString(l_state_str);
}

/**
 * @brief Convert network state to user-visible string (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return State string
 */
PyObject* dap_chain_net_state_to_str_user_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    const char *l_state_str = dap_chain_net_state_to_str_user(l_net);
    if (!l_state_str) {
        Py_RETURN_NONE;
    }

    return PyUnicode_FromString(l_state_str);
}

/**
 * @brief Get human-readable name for network state (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (state int)
 * @return State name string
 */
PyObject* dap_chain_net_get_state_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_state = 0;

    if (!PyArg_ParseTuple(a_args, "i", &l_state)) {
        return NULL;
    }

    const char *l_state_str = dap_chain_net_get_state_name((dap_chain_net_state_t)l_state);
    if (!l_state_str) {
        Py_RETURN_NONE;
    }

    return PyUnicode_FromString(l_state_str);
}

/**
 * @brief Change network state (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return 0 on success, negative on error
 */
PyObject* dap_chain_net_state_go_to_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    int l_new_state = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_capsule, &l_new_state)) {
        log_it(L_ERROR, "Invalid arguments for net_state_go_to");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Validate state value
    if (l_new_state < NET_STATE_LOADING || l_new_state > NET_STATE_ONLINE) {
        log_it(L_ERROR, "Invalid network state: %d", l_new_state);
        PyErr_Format(PyExc_ValueError, "Invalid network state: %d", l_new_state);
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    log_it(L_INFO, "Changing network '%s' state to %d", l_net->pub.name, l_new_state);
    int l_ret = dap_chain_net_state_go_to(l_net, (dap_chain_net_state_t)l_new_state);
    
    if (l_ret != 0) {
        log_it(L_ERROR, "Failed to change network '%s' state: %d", l_net->pub.name, l_ret);
    }
    
    return Py_BuildValue("i", l_ret);
}

/**
 * @brief Start network (go online) (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return 0 on success, negative on error
 */
PyObject* dap_chain_net_start_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_start");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    log_it(L_INFO, "Starting network '%s'", l_net->pub.name);
    int l_ret = dap_chain_net_start(l_net);
    
    if (l_ret != 0) {
        log_it(L_ERROR, "Failed to start network '%s': %d", l_net->pub.name, l_ret);
    }
    
    return Py_BuildValue("i", l_ret);
}

/**
 * @brief Stop network (go offline) (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return True on success, False on error
 */
PyObject* dap_chain_net_stop_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_stop");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    log_it(L_INFO, "Stopping network '%s'", l_net->pub.name);
    bool l_ret = dap_chain_net_stop(l_net);
    
    if (!l_ret) {
        log_it(L_ERROR, "Failed to stop network '%s'", l_net->pub.name);
    }
    
    return Py_BuildValue("O", l_ret ? Py_True : Py_False);
}

/**
 * @brief Get network native ticker (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple
 * @return Native ticker string or None
 */
PyObject* dap_chain_net_get_native_ticker_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_native_ticker");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    const char *l_ticker = l_net->pub.native_ticker;
    if (!l_ticker) {
        log_it(L_DEBUG, "Network '%s' has no native ticker", l_net->pub.name);
        Py_RETURN_NONE;
    }
    
    return Py_BuildValue("s", l_ticker);
}

/**
 * @brief Initialize network subsystem (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return 0 on success, negative on error
 */
PyObject* dap_chain_net_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    int l_result = dap_chain_net_init();
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize network subsystem (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return None
 */
PyObject* dap_chain_net_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    dap_chain_net_deinit();
    Py_RETURN_NONE;
}

/**
 * @brief Delete network instance (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return None
 */
PyObject* dap_chain_net_delete_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_net_delete(l_net);
    Py_RETURN_NONE;
}

/**
 * @brief Load all networks from config (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return None
 */
PyObject* dap_chain_net_load_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    log_it(L_INFO, "Loading all networks from config");
    dap_chain_net_load_all();
    log_it(L_INFO, "All networks loaded");
    
    Py_RETURN_NONE;
}

/**
 * @brief Try to bring all networks online (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return None
 */
PyObject* dap_chain_net_try_online_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    log_it(L_INFO, "Trying to bring all networks online");
    dap_chain_net_try_online_all();
    log_it(L_INFO, "All networks processing complete");
    
    Py_RETURN_NONE;
}

// =========================================
// CHAIN ACCESS FUNCTIONS (Phase 3)
// =========================================

/**
 * @brief Get chain by name from network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, chain name)
 * @return Chain capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_get_chain_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    const char *l_chain_name = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "Os", &l_net_capsule, &l_chain_name)) {
        log_it(L_ERROR, "Invalid arguments for net_get_chain_by_name");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Validate chain name
    if (!l_chain_name || strlen(l_chain_name) == 0) {
        log_it(L_ERROR, "Chain name is NULL or empty");
        PyErr_SetString(PyExc_ValueError, "Chain name cannot be empty");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get chain by name
    dap_chain_t *l_chain = dap_chain_net_get_chain_by_name(l_net, l_chain_name);
    if (!l_chain) {
        log_it(L_WARNING, "Chain '%s' not found in network '%s'", l_chain_name, l_net->pub.name);
        PyErr_Format(PyExc_ValueError, "Chain '%s' not found in network '%s'", l_chain_name, l_net->pub.name);
        return NULL;
    }
    
    log_it(L_DEBUG, "Found chain '%s' in network '%s'", l_chain_name, l_net->pub.name);
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

/**
 * @brief Get chain by ID from network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, chain ID uint64)
 * @return Chain capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_get_chain_by_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    unsigned long long l_chain_id_uint = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "OK", &l_net_capsule, &l_chain_id_uint)) {
        log_it(L_ERROR, "Invalid arguments for net_get_chain_by_id");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Validate chain ID
    if (l_chain_id_uint == 0) {
        log_it(L_ERROR, "Chain ID cannot be zero");
        PyErr_SetString(PyExc_ValueError, "Chain ID cannot be zero");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get chain by ID
    dap_chain_id_t l_chain_id = {.uint64 = l_chain_id_uint};
    dap_chain_t *l_chain = dap_chain_net_get_chain_by_id(l_net, l_chain_id);
    
    if (!l_chain) {
        log_it(L_WARNING, "Chain with ID %llu not found in network '%s'", l_chain_id_uint, l_net->pub.name);
        PyErr_Format(PyExc_ValueError, "Chain with ID %llu not found in network '%s'", 
                     l_chain_id_uint, l_net->pub.name);
        return NULL;
    }
    
    log_it(L_DEBUG, "Found chain ID %llu in network '%s'", l_chain_id_uint, l_net->pub.name);
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

/**
 * @brief Get chain by type from network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, chain type int)
 * @return Chain capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_get_chain_by_chain_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    int l_chain_type = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_capsule, &l_chain_type)) {
        log_it(L_ERROR, "Invalid arguments for net_get_chain_by_chain_type");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get chain by type
    dap_chain_t *l_chain = dap_chain_net_get_chain_by_chain_type(l_net, (dap_chain_type_t)l_chain_type);
    
    if (!l_chain) {
        log_it(L_WARNING, "Chain with type %d not found in network '%s'", l_chain_type, l_net->pub.name);
        PyErr_Format(PyExc_ValueError, "Chain with type %d not found in network '%s'", 
                     l_chain_type, l_net->pub.name);
        return NULL;
    }
    
    log_it(L_DEBUG, "Found chain with type %d in network '%s'", l_chain_type, l_net->pub.name);
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

/**
 * @brief Get default chain by type from network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, chain type int)
 * @return Chain capsule or NULL with Python exception set
 */
PyObject* dap_chain_net_get_default_chain_by_chain_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    int l_chain_type = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_capsule, &l_chain_type)) {
        log_it(L_ERROR, "Invalid arguments for net_get_default_chain_by_chain_type");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get default chain by type
    dap_chain_t *l_chain = dap_chain_net_get_default_chain_by_chain_type(l_net, (dap_chain_type_t)l_chain_type);
    
    if (!l_chain) {
        log_it(L_WARNING, "Default chain with type %d not found in network '%s'", l_chain_type, l_net->pub.name);
        PyErr_Format(PyExc_ValueError, "Default chain with type %d not found in network '%s'", 
                     l_chain_type, l_net->pub.name);
        return NULL;
    }
    
    log_it(L_DEBUG, "Found default chain with type %d in network '%s'", l_chain_type, l_net->pub.name);
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

/**
 * @brief Get chain name from capsule (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return Chain name string or NULL with Python exception set
 */
PyObject* dap_chain_get_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_get_name");
        return NULL;
    }
    
    // Validate capsule
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    // Extract chain pointer
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    return Py_BuildValue("s", l_chain->name);
}

/**
 * @brief Get chain ID from capsule (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return Chain ID as uint64 or NULL with Python exception set
 */
PyObject* dap_chain_get_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
        log_it(L_ERROR, "Invalid arguments for chain_get_id");
        return NULL;
    }
    
    // Validate capsule
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    // Extract chain pointer
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        log_it(L_ERROR, "Failed to extract chain pointer from capsule");
        return NULL;
    }
    
    return Py_BuildValue("K", l_chain->id.uint64);
}

// =========================================
// NODE INFO FUNCTIONS 
// =========================================

/**
 * @brief Get node role in network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Node role as int or NULL with Python exception set
 */
PyObject* dap_chain_net_get_role_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_role");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get node role
    dap_chain_node_role_t l_role = dap_chain_net_get_role(l_net);
    log_it(L_DEBUG, "Node role in network '%s': %u (%s)", l_net->pub.name, 
           l_role.enums, dap_chain_node_role_to_str(l_role));
    
    return Py_BuildValue("I", l_role.enums);  // Return as unsigned int
}

/**
 * @brief Check if node is authorized in network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return True/False or NULL with Python exception set
 */
PyObject* dap_chain_net_is_my_node_authorized_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_is_my_node_authorized");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Check authorization
    bool l_is_authorized = dap_chain_net_is_my_node_authorized(l_net);
    log_it(L_DEBUG, "Node authorized in network '%s': %s", l_net->pub.name, l_is_authorized ? "yes" : "no");
    
    return Py_BuildValue("O", l_is_authorized ? Py_True : Py_False);
}

/**
 * @brief Get current node address as integer (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Node address as uint64 or NULL with Python exception set
 */
PyObject* dap_chain_net_get_cur_addr_int_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_cur_addr_int");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Get current node address
    uint64_t l_addr = dap_chain_net_get_cur_addr_int(l_net);
    log_it(L_DEBUG, "Current node address in network '%s': %lu", l_net->pub.name, l_addr);
    
    return Py_BuildValue("K", l_addr);
}

/**
 * @brief Get ACL index for network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return ACL index as int or NULL with Python exception set
 */
PyObject* dap_chain_net_get_acl_idx_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    uint16_t l_acl_idx = dap_chain_net_get_acl_idx(l_net);
    return PyLong_FromUnsignedLong(l_acl_idx);
}

/**
 * @brief Get sync-from-zero flag (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return True/False or NULL with Python exception set
 */
PyObject* dap_chain_net_get_flag_sync_from_zero_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    bool l_flag = dap_chain_net_get_flag_sync_from_zero(l_net);
    return PyBool_FromLong(l_flag ? 1 : 0);
}

/**
 * @brief Set sync-from-zero flag (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, flag)
 * @return None
 */
PyObject* dap_chain_net_set_flag_sync_from_zero_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    int l_flag = 0;

    if (!PyArg_ParseTuple(a_args, "Op", &l_net_capsule, &l_flag)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_net_set_flag_sync_from_zero(l_net, l_flag ? true : false);
    Py_RETURN_NONE;
}

/**
 * @brief Get network load mode flag (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return True/False or NULL with Python exception set
 */
PyObject* dap_chain_net_get_load_mode_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    bool l_load_mode = dap_chain_net_get_load_mode(l_net);
    return PyBool_FromLong(l_load_mode ? 1 : 0);
}

/**
 * @brief Set global load-skip flag (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return None
 */
PyObject* dap_chain_net_set_load_skip_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    dap_chain_net_set_load_skip();
    Py_RETURN_NONE;
}

/**
 * @brief Check if network is bridged to another network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, net_id uint64)
 * @return True/False or NULL with Python exception set
 */
PyObject* dap_chain_net_is_bridged_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_net_capsule, &l_net_id)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_net_id_t l_other_id = {.uint64 = l_net_id};
    bool l_is_bridged = dap_chain_net_is_bridged(l_net, l_other_id);
    return PyBool_FromLong(l_is_bridged ? 1 : 0);
}

// =========================================
// NETWORK OPERATIONS FUNCTIONS
// =========================================

// REMOVED: dap_chain_net_links_dump was deleted from CellFrame SDK
// Function declaration removed but header not cleaned up

/**
 * @brief Add link to network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, addr uint64, host string, port int)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_net_link_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    unsigned long long l_addr_uint = 0;
    const char *l_host = NULL;
    int l_port = 0;
    
    // Validate and parse arguments
    if (!PyArg_ParseTuple(a_args, "OKsi", &l_net_capsule, &l_addr_uint, &l_host, &l_port)) {
        log_it(L_ERROR, "Invalid arguments for net_link_add");
        return NULL;
    }
    
    // Validate capsule
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    // Validate host
    if (!l_host || strlen(l_host) == 0) {
        log_it(L_ERROR, "Host is NULL or empty");
        PyErr_SetString(PyExc_ValueError, "Host cannot be empty");
        return NULL;
    }
    
    // Validate port
    if (l_port <= 0 || l_port > 65535) {
        log_it(L_ERROR, "Invalid port: %d", l_port);
        PyErr_SetString(PyExc_ValueError, "Port must be between 1 and 65535");
        return NULL;
    }
    
    // Extract network pointer
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Prepare address structure
    dap_stream_node_addr_t l_addr = {.uint64 = l_addr_uint};
    
    // Add link
    int l_result = dap_chain_net_link_add(l_net, &l_addr, l_host, (uint16_t)l_port);
    
    if (l_result != 0) {
        log_it(L_WARNING, "Failed to add link to network '%s': host=%s port=%d (error=%d)", 
               l_net->pub.name, l_host, l_port, l_result);
    } else {
        log_it(L_INFO, "Link added to network '%s': host=%s port=%d", l_net->pub.name, l_host, l_port);
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Get target state of network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Target state as int or NULL with Python exception set
 */
PyObject* dap_chain_net_get_target_state_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_target_state");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    dap_chain_net_state_t l_state = dap_chain_net_get_target_state(l_net);
    return Py_BuildValue("i", (int)l_state);
}

/**
 * @brief Sync network chains (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_net_sync_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_sync");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    int l_result = dap_chain_net_sync(l_net);
    log_it(L_INFO, "Network '%s' sync initiated: %d", l_net->pub.name, l_result);
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Establish network links (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_net_links_establish_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_links_establish");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    int l_result = dap_chain_net_links_establish(l_net);
    log_it(L_INFO, "Network '%s' links establish initiated: %d", l_net->pub.name, l_result);
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Purge network data (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return None
 */
PyObject* dap_chain_net_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_purge");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    log_it(L_WARNING, "Purging network '%s' data", l_net->pub.name);
    dap_chain_net_purge(l_net);
    log_it(L_INFO, "Network '%s' purged", l_net->pub.name);
    
    Py_RETURN_NONE;
}

/**
 * @brief Announce network address (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return None
 */
PyObject* dap_chain_net_announce_addr_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_announce_addr");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    dap_chain_net_announce_addr(l_net);
    log_it(L_DEBUG, "Network '%s' address announced", l_net->pub.name);
    
    Py_RETURN_NONE;
}

/**
 * @brief Announce all network addresses (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return None
 */
PyObject* dap_chain_net_announce_addr_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    dap_chain_net_announce_addr_all(NULL);
    log_it(L_DEBUG, "All network addresses announced");
    
    Py_RETURN_NONE;
}

/**
 * @brief Get network transaction fee (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Dict with 'value' (256-bit int as string) and 'addr' or None if no fee
 */
PyObject* dap_chain_net_tx_get_fee_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_tx_get_fee");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    uint256_t l_fee = {};
    dap_chain_addr_t l_addr = {};
    
    bool l_has_fee = dap_chain_net_tx_get_fee(l_net->pub.id, &l_fee, &l_addr);
    
    if (!l_has_fee) {
        log_it(L_DEBUG, "Network '%s' has no transaction fee", l_net->pub.name);
        Py_RETURN_NONE;
    }
    
    // Convert uint256_t to string (coins and datoshi)
    const char *l_fee_coins_str = NULL;
    const char *l_fee_datoshi_str = dap_uint256_to_char(l_fee, &l_fee_coins_str);
    
    if (!l_fee_datoshi_str) {
        log_it(L_ERROR, "Failed to convert fee to string");
        Py_RETURN_NONE;
    }
    
    // Convert address to string (static buffer, no need to free)
    const char *l_addr_str = dap_chain_addr_to_str(&l_addr);
    if (!l_addr_str) {
        log_it(L_ERROR, "Failed to convert address to string");
        Py_RETURN_NONE;
    }
    
    // Build result dict with coins and datoshi
    PyObject *l_result = Py_BuildValue("{s:s,s:s,s:s}", 
                                       "datoshi", l_fee_datoshi_str,
                                       "coins", l_fee_coins_str ? l_fee_coins_str : "0",
                                       "addr", l_addr_str);
    
    return l_result;
}

/**
 * @brief Get ACL GDB group name (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Group name string or None
 */
PyObject* dap_chain_net_get_gdb_group_acl_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    char *l_group = dap_chain_net_get_gdb_group_acl(l_net);
    if (!l_group) {
        Py_RETURN_NONE;
    }

    PyObject *l_str = PyUnicode_FromString(l_group);
    DAP_DELETE(l_group);
    return l_str;
}

/**
 * @brief Get no-chain GDB group name (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return Group name string or None
 */
PyObject* dap_chain_net_get_gdb_group_nochain_new_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
        return NULL;
    }

    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    char *l_group = dap_chain_net_get_gdb_group_nochain_new(l_chain);
    if (!l_group) {
        Py_RETURN_NONE;
    }

    PyObject *l_str = PyUnicode_FromString(l_group);
    DAP_DELETE(l_group);
    return l_str;
}

/**
 * @brief Get mempool GDB group by chain type (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, chain type int)
 * @return Group name string or None
 */
PyObject* dap_chain_net_get_gdb_group_mempool_by_chain_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    int l_chain_type = 0;

    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_capsule, &l_chain_type)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    char *l_group = dap_chain_net_get_gdb_group_mempool_by_chain_type(l_net, (dap_chain_type_t)l_chain_type);
    if (!l_group) {
        Py_RETURN_NONE;
    }

    PyObject *l_str = PyUnicode_FromString(l_group);
    DAP_DELETE(l_group);
    return l_str;
}

/**
 * @brief Collect network list JSON (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (version int)
 * @return JSON capsule or None
 */
PyObject* dap_chain_net_list_json_collect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_version = 0;

    if (!PyArg_ParseTuple(a_args, "i", &l_version)) {
        return NULL;
    }

    dap_json_t *l_json = dap_chain_net_list_json_collect(l_version);
    if (!l_json) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

/**
 * @brief Collect network states JSON (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, version int)
 * @return JSON capsule or None
 */
PyObject* dap_chain_net_states_json_collect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    int l_version = 0;

    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_capsule, &l_version)) {
        return NULL;
    }

    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_json_t *l_json = dap_chain_net_states_json_collect(l_net, l_version);
    if (!l_json) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

/**
 * @brief Collect networks info JSON (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (version int)
 * @return JSON capsule or None
 */
PyObject* dap_chain_nets_info_json_collect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_version = 0;

    if (!PyArg_ParseTuple(a_args, "i", &l_version)) {
        return NULL;
    }

    dap_json_t *l_json = dap_chain_nets_info_json_collect(l_version);
    if (!l_json) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_json, "dap_json_t", NULL);
}

/**
 * @brief Get networks and chains as CLI string (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (unused)
 * @return String or None
 */
PyObject* dap_cli_list_net_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    dap_string_t *l_str = dap_cli_list_net();
    if (!l_str) {
        Py_RETURN_NONE;
    }

    char *l_str_out = dap_string_free(l_str, false);
    if (!l_str_out) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = PyUnicode_FromString(l_str_out);
    DAP_DELETE(l_str_out);
    return l_result;
}

/**
 * @brief Get ledger by network name (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (net name string)
 * @return Ledger capsule or None
 */
PyObject* dap_ledger_by_net_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name = NULL;

    if (!PyArg_ParseTuple(a_args, "s", &l_net_name)) {
        return NULL;
    }

    dap_ledger_t *l_ledger = dap_ledger_by_net_name(l_net_name);
    if (!l_ledger) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_ledger, "dap_ledger_t", NULL);
}

/**
 * @brief Get current cell ID (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Dict with 'uint64' and 'raw' (bytes) or None if no cell
 */
PyObject* dap_chain_net_get_cur_cell_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_cur_cell");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t *l_cell_id = dap_chain_net_get_cur_cell(l_net);
    if (!l_cell_id) {
        log_it(L_DEBUG, "Network '%s' has no current cell", l_net->pub.name);
        Py_RETURN_NONE;
    }
    
    // Return dict with uint64 and raw bytes
    PyObject *l_raw_bytes = PyBytes_FromStringAndSize((const char*)l_cell_id->raw, DAP_CHAIN_SHARD_ID_SIZE);
    if (!l_raw_bytes) {
        log_it(L_ERROR, "Failed to create bytes object for cell ID");
        Py_RETURN_NONE;
    }
    
    PyObject *l_result = Py_BuildValue("{s:K,s:O}", 
                                       "uint64", l_cell_id->uint64,
                                       "raw", l_raw_bytes);
    Py_DECREF(l_raw_bytes);
    
    return l_result;
}

/**
 * @brief Get current node info (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return Dict with node info or None if no info available
 */
PyObject* dap_chain_net_get_my_node_info_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_my_node_info");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    dap_chain_node_info_t *l_node_info = dap_chain_net_get_my_node_info(l_net);
    if (!l_node_info) {
        log_it(L_DEBUG, "Network '%s' has no node info", l_net->pub.name);
        Py_RETURN_NONE;
    }
    
    // Convert address to uint64
    uint64_t l_addr_uint64 = l_node_info->address.uint64;
    
    // Convert cell_id
    uint64_t l_cell_id_uint64 = l_node_info->cell_id.uint64;
    
    // Get ext_host (variable length string at the end of structure)
    const char *l_ext_host = l_node_info->ext_host_len > 0 ? l_node_info->ext_host : "";
    
    // Build result dict
    PyObject *l_result = Py_BuildValue("{s:K,s:K,s:s,s:i,s:s}", 
                                       "address", l_addr_uint64,
                                       "cell_id", l_cell_id_uint64,
                                       "alias", l_node_info->alias,
                                       "ext_port", (int)l_node_info->ext_port,
                                       "ext_host", l_ext_host);
    
    return l_result;
}

/**
 * @brief Get authorized nodes (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule)
 * @return List of authorized node addresses (uint64) or empty list
 */
PyObject* dap_chain_net_get_authorized_nodes_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_net_capsule)) {
        log_it(L_ERROR, "Invalid arguments for net_get_authorized_nodes");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    size_t l_nodes_count = 0;
    dap_stream_node_addr_t *l_nodes = dap_chain_net_get_authorized_nodes(l_net, &l_nodes_count);
    
    // Create Python list
    PyObject *l_result = PyList_New(l_nodes_count);
    if (!l_result) {
        log_it(L_ERROR, "Failed to create list for authorized nodes");
        return NULL;
    }
    
    // Fill list with node addresses
    for (size_t i = 0; i < l_nodes_count; i++) {
        PyObject *l_addr = Py_BuildValue("K", l_nodes[i].uint64);
        if (!l_addr) {
            log_it(L_WARNING, "Failed to convert node address at index %zu", i);
            Py_DECREF(l_result);
            return NULL;
        }
        PyList_SET_ITEM(l_result, i, l_addr);  // Steals reference
    }
    
    log_it(L_DEBUG, "Network '%s' has %zu authorized nodes", l_net->pub.name, l_nodes_count);
    
    return l_result;
}

/**
 * @brief Verify datum for adding to chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, datum bytes, datum_hash bytes)
 * @return 0 on success, error code on failure
 */
PyObject* dap_chain_net_verify_datum_for_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    const char *l_datum_bytes = NULL;
    Py_ssize_t l_datum_size = 0;
    const char *l_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "Oy#y#", &l_chain_capsule, &l_datum_bytes, &l_datum_size, &l_hash_bytes, &l_hash_size)) {
        log_it(L_ERROR, "Invalid arguments for verify_datum_for_add");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    // Validate hash size
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
    
    // Cast datum bytes to datum structure
    if (l_datum_size < (Py_ssize_t)sizeof(dap_chain_datum_t)) {
        log_it(L_ERROR, "Datum size too small: %zd (minimum %zu)", l_datum_size, sizeof(dap_chain_datum_t));
        PyErr_SetString(PyExc_ValueError, "Datum size too small");
        return NULL;
    }
    
    dap_chain_datum_t *l_datum = (dap_chain_datum_t*)l_datum_bytes;
    dap_hash_fast_t *l_datum_hash = (dap_hash_fast_t*)l_hash_bytes;
    
    // Verify datum
    int l_result = dap_chain_net_verify_datum_for_add(l_chain, l_datum, l_datum_hash);
    
    if (l_result != 0) {
        const char *l_err_str = dap_chain_net_verify_datum_err_code_to_str(l_datum, l_result);
        log_it(L_WARNING, "Datum verification failed: code=%d, msg=%s", l_result, l_err_str ? l_err_str : "unknown");
    } else {
        log_it(L_DEBUG, "Datum verified successfully for chain '%s'", l_chain->name);
    }
    
    return Py_BuildValue("i", l_result);
}

/**
 * @brief Convert datum verification error code to string (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (datum bytes, error code)
 * @return Error string or None
 */
PyObject* dap_chain_net_verify_datum_err_code_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_datum_bytes = NULL;
    Py_ssize_t l_datum_size = 0;
    int l_code = 0;

    if (!PyArg_ParseTuple(a_args, "y#i", &l_datum_bytes, &l_datum_size, &l_code)) {
        log_it(L_ERROR, "Invalid arguments for net_verify_datum_err_code_to_str");
        return NULL;
    }

    if (l_datum_size < (Py_ssize_t)sizeof(dap_chain_datum_t)) {
        PyErr_SetString(PyExc_ValueError, "Datum size too small");
        return NULL;
    }

    const char *l_err_str = dap_chain_net_verify_datum_err_code_to_str((dap_chain_datum_t *)l_datum_bytes, l_code);
    if (!l_err_str) {
        Py_RETURN_NONE;
    }

    return PyUnicode_FromString(l_err_str);
}

/**
 * @brief Get datum list with optional Python filter (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (net capsule, chain capsule or None, filter callable or None)
 * @return List of datum bytes
 */
PyObject* dap_chain_datum_list_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_chain_obj = Py_None;
    PyObject *l_filter_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|OO", &l_net_obj, &l_chain_obj, &l_filter_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_t *l_chain = NULL;
    if (l_chain_obj && l_chain_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_chain_obj)) {
            PyErr_SetString(PyExc_TypeError, "Second argument must be a chain capsule or None");
            return NULL;
        }
        l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
        if (!l_chain) {
            PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
            return NULL;
        }
    }

    dap_chain_datum_filter_func_t *l_filter_cb = NULL;
    cf_network_datum_filter_ctx_t l_ctx = {0};
    if (l_filter_obj && l_filter_obj != Py_None) {
        if (!PyCallable_Check(l_filter_obj)) {
            PyErr_SetString(PyExc_TypeError, "Filter must be callable or None");
            return NULL;
        }
        l_ctx.callback = l_filter_obj;
        Py_INCREF(l_filter_obj);
        l_filter_cb = cf_network_datum_filter_cb;
    }

    dap_list_t *l_list = dap_chain_datum_list(l_net, l_chain, l_filter_cb, l_filter_cb ? &l_ctx : NULL);

    if (l_filter_obj && l_filter_obj != Py_None) {
        Py_DECREF(l_filter_obj);
    }

    if (l_ctx.error || PyErr_Occurred()) {
        dap_list_free_full(l_list, cf_network_list_free);
        return NULL;
    }

    PyObject *l_result = PyList_New(0);
    if (!l_result) {
        dap_list_free_full(l_list, cf_network_list_free);
        return NULL;
    }

    for (dap_list_t *it = l_list; it; it = it->next) {
        dap_chain_datum_t *l_datum = (dap_chain_datum_t *)it->data;
        if (!l_datum) {
            continue;
        }
        size_t l_size = sizeof(dap_chain_datum_t) + l_datum->header.data_size;
        PyObject *l_bytes = PyBytes_FromStringAndSize((const char *)l_datum, (Py_ssize_t)l_size);
        if (!l_bytes) {
            Py_DECREF(l_result);
            dap_list_free_full(l_list, cf_network_list_free);
            return NULL;
        }
        PyList_Append(l_result, l_bytes);
        Py_DECREF(l_bytes);
    }

    dap_list_free_full(l_list, cf_network_list_free);
    return l_result;
}

/**
 * @brief Add datum to chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, datum bytes, datum hash bytes, optional index capsule)
 * @return Integer result code
 */
PyObject* dap_chain_datum_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    PyObject *l_index_obj = Py_None;
    const char *l_datum_bytes = NULL;
    const char *l_hash_bytes = NULL;
    Py_ssize_t l_datum_size = 0;
    Py_ssize_t l_hash_size = 0;

    if (!PyArg_ParseTuple(a_args, "Oy#y#|O", &l_chain_obj, &l_datum_bytes, &l_datum_size, &l_hash_bytes, &l_hash_size, &l_index_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    if ((size_t)l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }

    if (l_datum_size < (Py_ssize_t)sizeof(dap_chain_datum_t)) {
        PyErr_SetString(PyExc_ValueError, "Datum size too small");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    void *l_index_data = NULL;
    if (l_index_obj && l_index_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_index_obj)) {
            PyErr_SetString(PyExc_TypeError, "Index data must be a capsule or None");
            return NULL;
        }
        l_index_data = PyCapsule_GetPointer(l_index_obj, NULL);
    }

    int l_rc = dap_chain_datum_add(l_chain,
                                   (dap_chain_datum_t *)l_datum_bytes,
                                   (size_t)l_datum_size,
                                   (dap_hash_fast_t *)l_hash_bytes,
                                   l_index_data);
    return PyLong_FromLong(l_rc);
}

/**
 * @brief Remove datum from chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, datum bytes, datum hash bytes)
 * @return Integer result code
 */
PyObject* dap_chain_datum_remove_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    const char *l_datum_bytes = NULL;
    const char *l_hash_bytes = NULL;
    Py_ssize_t l_datum_size = 0;
    Py_ssize_t l_hash_size = 0;

    if (!PyArg_ParseTuple(a_args, "Oy#y#", &l_chain_obj, &l_datum_bytes, &l_datum_size, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    if ((size_t)l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }

    if (l_datum_size < (Py_ssize_t)sizeof(dap_chain_datum_t)) {
        PyErr_SetString(PyExc_ValueError, "Datum size too small");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    int l_rc = dap_chain_datum_remove(l_chain,
                                      (dap_chain_datum_t *)l_datum_bytes,
                                      (size_t)l_datum_size,
                                      (dap_hash_fast_t *)l_hash_bytes);
    return PyLong_FromLong(l_rc);
}

/**
 * @brief Process mempool for network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (net capsule)
 * @return None
 */
PyObject* dap_chain_net_proc_mempool_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_net_proc_mempool(l_net);
    Py_RETURN_NONE;
}

/**
 * @brief Get mempool cluster (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule)
 * @return Cluster capsule or None
 */
PyObject* dap_chain_net_get_mempool_cluster_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;

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

    dap_global_db_cluster_t *l_cluster = dap_chain_net_get_mempool_cluster(l_chain);
    if (!l_cluster) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_cluster, "dap_global_db_cluster_t", NULL);
}

/**
 * @brief Add authorized nodes to cluster (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (net capsule, cluster capsule)
 * @return Integer result code
 */
PyObject* dap_chain_net_add_auth_nodes_to_cluster_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_cluster_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_net_obj, &l_cluster_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj) || !PyCapsule_CheckExact(l_cluster_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected network and cluster capsules");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    dap_global_db_cluster_t *l_cluster = (dap_global_db_cluster_t *)PyCapsule_GetPointer(l_cluster_obj, "dap_global_db_cluster_t");
    if (!l_net || !l_cluster) {
        PyErr_SetString(PyExc_ValueError, "Invalid network or cluster capsule");
        return NULL;
    }

    int l_rc = dap_chain_net_add_auth_nodes_to_cluster(l_net, l_cluster);
    return PyLong_FromLong(l_rc);
}

/**
 * @brief Add validator to clusters (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, node addr uint64)
 * @return True on success
 */
PyObject* dap_chain_net_add_validator_to_clusters_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    unsigned long long l_addr_uint = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_chain_obj, &l_addr_uint)) {
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

    dap_stream_node_addr_t l_addr = { .uint64 = l_addr_uint };
    bool l_rc = dap_chain_net_add_validator_to_clusters(l_chain, &l_addr);
    return PyBool_FromLong(l_rc ? 1 : 0);
}

/**
 * @brief Remove validator from clusters (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, node addr uint64)
 * @return True on success
 */
PyObject* dap_chain_net_remove_validator_from_clusters_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    unsigned long long l_addr_uint = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_chain_obj, &l_addr_uint)) {
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

    dap_stream_node_addr_t l_addr = { .uint64 = l_addr_uint };
    bool l_rc = dap_chain_net_remove_validator_from_clusters(l_chain, &l_addr);
    return PyBool_FromLong(l_rc ? 1 : 0);
}

/**
 * @brief Add reward to network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (net capsule, reward uint256 bytes/capsule, block number)
 * @return Integer result code
 */
PyObject* dap_chain_net_add_reward_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_reward_obj = NULL;
    unsigned long long l_block_num = 0;

    if (!PyArg_ParseTuple(a_args, "OOK", &l_net_obj, &l_reward_obj, &l_block_num)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    uint256_t l_reward = {};
    if (cf_network_parse_uint256(l_reward_obj, &l_reward) != 0) {
        return NULL;
    }

    int l_rc = dap_chain_net_add_reward(l_net, l_reward, (uint64_t)l_block_num);
    return PyLong_FromLong(l_rc);
}

/**
 * @brief Remove last reward from network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (net capsule)
 * @return None
 */
PyObject* dap_chain_net_remove_last_reward_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_net_remove_last_reward(l_net);
    Py_RETURN_NONE;
}

/**
 * @brief Get reward by block number (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (net capsule, block number)
 * @return Reward as uint256 bytes
 */
PyObject* dap_chain_net_get_reward_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    unsigned long long l_block_num = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_net_obj, &l_block_num)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    uint256_t l_reward = dap_chain_net_get_reward(l_net, (uint64_t)l_block_num);
    return PyBytes_FromStringAndSize((const char *)&l_reward, sizeof(uint256_t));
}

/**
 * @brief Set network transaction fee (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (net capsule, fee uint256 bytes/capsule, addr capsule)
 * @return True on success
 */
PyObject* dap_chain_net_tx_set_fee_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_fee_obj = NULL;
    PyObject *l_addr_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OOO", &l_net_obj, &l_fee_obj, &l_addr_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj) || !PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected network and addr capsules");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_obj, "dap_chain_addr_t");
    if (!l_net || !l_addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid network or addr capsule");
        return NULL;
    }

    uint256_t l_fee = {};
    if (cf_network_parse_uint256(l_fee_obj, &l_fee) != 0) {
        return NULL;
    }

    bool l_ok = dap_chain_net_tx_set_fee(l_net->pub.id, l_fee, *l_addr);
    return PyBool_FromLong(l_ok ? 1 : 0);
}

// =========================================
// CALLBACK FUNCTIONS
// =========================================

// Structure to hold Python callback information
typedef struct {
    PyObject *callback;      // Python callback function
    PyObject *user_data;     // User data to pass to callback
} python_callback_ctx_t;

/**
 * @brief C callback wrapper that calls Python callback
 * @param a_store_obj Store object
 * @param a_arg User data (python_callback_ctx_t*)
 */
static void s_python_callback_wrapper(dap_store_obj_t *a_store_obj, void *a_arg) {
    python_callback_ctx_t *l_ctx = (python_callback_ctx_t*)a_arg;
    if (!l_ctx || !l_ctx->callback) {
        log_it(L_ERROR, "Invalid callback context");
        return;
    }
    
    // Acquire GIL for Python callback
    PyGILState_STATE l_gstate = PyGILState_Ensure();
    
    // Call Python callback with capsule and user data
    PyObject *l_arg_capsule = PyCapsule_New(a_store_obj, "dap_store_obj_t", NULL);
    if (!l_arg_capsule) {
        log_it(L_ERROR, "Failed to create capsule for callback argument");
        PyGILState_Release(l_gstate);
        return;
    }
    
    PyObject *l_result = PyObject_CallFunctionObjArgs(l_ctx->callback, l_arg_capsule, l_ctx->user_data, NULL);
    
    Py_DECREF(l_arg_capsule);
    
    if (!l_result) {
        log_it(L_ERROR, "Python callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }
    
    PyGILState_Release(l_gstate);
}

/**
 * @brief Add mempool notify callback for chain (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (chain capsule, callback function, user_data)
 * @return None
 */
PyObject* dap_chain_add_mempool_notify_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO|O", &l_chain_capsule, &l_callback, &l_user_data)) {
        log_it(L_ERROR, "Invalid arguments for add_mempool_notify_callback");
        return NULL;
    }
    
    if (!l_chain_capsule || !PyCapsule_CheckExact(l_chain_capsule)) {
        log_it(L_ERROR, "Expected chain capsule, got invalid object");
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
    
    // Allocate callback context (will be freed by registry cleanup)
    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        log_it(L_ERROR, "Failed to allocate callback context");
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    
    // Register C callback
    dap_chain_add_mempool_notify_callback(l_chain, s_python_callback_wrapper, l_ctx);
    
    // Register in global registry for proper cleanup (prevents memory leak)
    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_CHAIN_MEMPOOL, l_callback, l_user_data, 
                                   l_ctx, l_chain->name) != 0) {
        log_it(L_WARNING, "Failed to register callback in global registry (potential leak!)");
    }
    
    log_it(L_INFO, "Mempool notify callback registered for chain '%s'", l_chain->name);
    
    Py_RETURN_NONE;
}

/**
 * @brief Add nodelist notify callback for network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, callback function, user_data)
 * @return None
 */
PyObject* dap_chain_net_add_nodelist_notify_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO|O", &l_net_capsule, &l_callback, &l_user_data)) {
        log_it(L_ERROR, "Invalid arguments for add_nodelist_notify_callback");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        log_it(L_ERROR, "Callback must be callable");
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Allocate callback context
    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        log_it(L_ERROR, "Failed to allocate callback context");
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    
    // Register C callback
    dap_chain_net_add_nodelist_notify_callback(l_net, s_python_callback_wrapper, l_ctx);
    
    // Register in global registry for proper cleanup (prevents memory leak)
    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_CHAIN_NODELIST, l_callback, l_user_data, 
                                   l_ctx, l_net->pub.name) != 0) {
        log_it(L_WARNING, "Failed to register callback in global registry (potential leak!)");
    }
    
    log_it(L_INFO, "Nodelist notify callback registered for network '%s'", l_net->pub.name);
    
    Py_RETURN_NONE;
}

/**
 * @brief Add service order notify callback for network (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Python arguments tuple (network capsule, callback function, user_data)
 * @return None
 */
PyObject* dap_chain_net_srv_order_add_notify_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_capsule = NULL;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO|O", &l_net_capsule, &l_callback, &l_user_data)) {
        log_it(L_ERROR, "Invalid arguments for srv_order_add_notify_callback");
        return NULL;
    }
    
    if (!l_net_capsule || !PyCapsule_CheckExact(l_net_capsule)) {
        log_it(L_ERROR, "Expected network capsule, got invalid object");
        PyErr_SetString(PyExc_TypeError, "Expected network capsule");
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        log_it(L_ERROR, "Callback must be callable");
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t*)PyCapsule_GetPointer(l_net_capsule, "dap_chain_net_t");
    if (!l_net) {
        log_it(L_ERROR, "Failed to extract network pointer from capsule");
        return NULL;
    }
    
    // Allocate callback context
    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        log_it(L_ERROR, "Failed to allocate callback context");
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    
    // Register C callback
    dap_chain_net_srv_order_add_notify_callback(l_net, s_python_callback_wrapper, l_ctx);
    
    // Register in global registry for proper cleanup (prevents memory leak)
    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_CHAIN_SERVICE_ORDER, l_callback, l_user_data, 
                                   l_ctx, l_net->pub.name) != 0) {
        log_it(L_WARNING, "Failed to register callback in global registry (potential leak!)");
    }
    
    log_it(L_INFO, "Service order notify callback registered for network '%s'", l_net->pub.name);
    
    Py_RETURN_NONE;
}

// =========================================
// MODULE INITIALIZATION
// =========================================

/**
 * @brief Initialize network module and register all methods
 * @param a_module Parent Python module
 * @return 0 on success, -1 on failure
 */
int cellframe_network_init(PyObject *a_module) {
    if (!a_module) {
        log_it(L_ERROR, "Module is NULL");
        return -1;
    }
    
    // Define network module methods
    static PyMethodDef l_network_methods[] = {
        // Network lookup and iteration functions
        {"net_core_init", (PyCFunction)dap_chain_net_core_init_py, METH_NOARGS,
         "Initialize network core module"},
        {"net_register", (PyCFunction)dap_chain_net_register_py, METH_VARARGS,
         "Register network in core registry"},
        {"net_unregister", (PyCFunction)dap_chain_net_unregister_py, METH_VARARGS,
         "Unregister network from core registry"},
        {"net_by_name", (PyCFunction)dap_chain_net_by_name_py, METH_VARARGS,
         "Get network by name"},
        {"net_by_id", (PyCFunction)dap_chain_net_by_id_py, METH_VARARGS,
         "Get network by ID"},
        {"net_parse_net_chain", (PyCFunction)dap_chain_net_parse_net_chain_py, METH_VARARGS,
         "Parse -net and -chain arguments"},
        {"net_id_by_name", (PyCFunction)dap_chain_net_id_by_name_py, METH_VARARGS,
         "Get network ID by name"},
        {"net_id_parse", (PyCFunction)dap_chain_net_id_parse_py, METH_VARARGS,
         "Parse network ID from string"},
        {"net_count", (PyCFunction)dap_chain_net_count_py, METH_NOARGS,
         "Get total count of networks"},
        {"net_iter_start", (PyCFunction)dap_chain_net_iter_start_py, METH_NOARGS,
         "Start iteration over networks"},
        {"net_iter_next", (PyCFunction)dap_chain_net_iter_next_py, METH_VARARGS,
         "Continue iteration over networks"},
        {"net_iterate", (PyCFunction)dap_chain_net_iterate_py, METH_VARARGS,
         "Iterate over networks"},
        
        // Network state management functions
        {"net_get_name", (PyCFunction)dap_chain_net_get_name_py, METH_VARARGS,
         "Get network name from capsule"},
        {"net_get_id", (PyCFunction)dap_chain_net_get_id_py, METH_VARARGS,
         "Get network ID from capsule"},
        {"net_get_state", (PyCFunction)dap_chain_net_get_state_py, METH_VARARGS,
         "Get network state"},
        {"net_state_to_str", (PyCFunction)dap_chain_net_state_to_str_py, METH_VARARGS,
         "Convert state to string"},
        {"net_state_to_str_user", (PyCFunction)dap_chain_net_state_to_str_user_py, METH_VARARGS,
         "Convert state to user-visible string"},
        {"net_get_state_name", (PyCFunction)dap_chain_net_get_state_name_py, METH_VARARGS,
         "Get state name"},
        {"net_state_go_to", (PyCFunction)dap_chain_net_state_go_to_py, METH_VARARGS,
         "Change network state"},
        {"net_start", (PyCFunction)dap_chain_net_start_py, METH_VARARGS,
         "Start network (go online)"},
        {"net_stop", (PyCFunction)dap_chain_net_stop_py, METH_VARARGS,
         "Stop network (go offline)"},
        {"net_get_native_ticker", (PyCFunction)dap_chain_net_get_native_ticker_py, METH_VARARGS,
         "Get network native ticker"},
        {"net_init", (PyCFunction)dap_chain_net_init_py, METH_NOARGS,
         "Initialize network subsystem"},
        {"net_deinit", (PyCFunction)dap_chain_net_deinit_py, METH_NOARGS,
         "Deinitialize network subsystem"},
        {"net_delete", (PyCFunction)dap_chain_net_delete_py, METH_VARARGS,
         "Delete network instance"},
        {"net_load_all", (PyCFunction)dap_chain_net_load_all_py, METH_NOARGS,
         "Load all networks from config"},
        {"net_try_online_all", (PyCFunction)dap_chain_net_try_online_all_py, METH_NOARGS,
         "Try to bring all networks online"},
        
        // Chain access functions
        {"net_get_chain_by_name", (PyCFunction)dap_chain_net_get_chain_by_name_py, METH_VARARGS,
         "Get chain by name from network"},
        {"net_get_chain_by_id", (PyCFunction)dap_chain_net_get_chain_by_id_py, METH_VARARGS,
         "Get chain by ID from network"},
        {"net_get_chain_by_type", (PyCFunction)dap_chain_net_get_chain_by_chain_type_py, METH_VARARGS,
         "Get chain by type from network"},
        {"net_get_default_chain_by_type", (PyCFunction)dap_chain_net_get_default_chain_by_chain_type_py, METH_VARARGS,
         "Get default chain by type from network"},
        {"chain_get_name", (PyCFunction)dap_chain_get_name_py, METH_VARARGS,
         "Get chain name from capsule"},
        {"chain_get_id", (PyCFunction)dap_chain_get_id_py, METH_VARARGS,
         "Get chain ID from capsule"},
        
        // Node info functions
        {"net_get_role", (PyCFunction)dap_chain_net_get_role_py, METH_VARARGS,
         "Get node role in network"},
        {"net_get_acl_idx", (PyCFunction)dap_chain_net_get_acl_idx_py, METH_VARARGS,
         "Get network ACL index"},
        {"net_is_my_node_authorized", (PyCFunction)dap_chain_net_is_my_node_authorized_py, METH_VARARGS,
         "Check if node is authorized in network"},
        {"net_get_cur_addr_int", (PyCFunction)dap_chain_net_get_cur_addr_int_py, METH_VARARGS,
         "Get current node address as integer"},
        {"net_get_flag_sync_from_zero", (PyCFunction)dap_chain_net_get_flag_sync_from_zero_py, METH_VARARGS,
         "Get sync-from-zero flag"},
        {"net_set_flag_sync_from_zero", (PyCFunction)dap_chain_net_set_flag_sync_from_zero_py, METH_VARARGS,
         "Set sync-from-zero flag"},
        {"net_get_load_mode", (PyCFunction)dap_chain_net_get_load_mode_py, METH_VARARGS,
         "Get load mode flag"},
        {"net_set_load_skip", (PyCFunction)dap_chain_net_set_load_skip_py, METH_NOARGS,
         "Set global load-skip flag"},
        {"net_is_bridged", (PyCFunction)dap_chain_net_is_bridged_py, METH_VARARGS,
         "Check if network is bridged"},
        
        // Network operations functions
        {"net_link_add", (PyCFunction)dap_chain_net_link_add_py, METH_VARARGS,
         "Add link to network"},
        {"net_get_target_state", (PyCFunction)dap_chain_net_get_target_state_py, METH_VARARGS,
         "Get target state of network"},
        {"net_sync", (PyCFunction)dap_chain_net_sync_py, METH_VARARGS,
         "Sync network chains"},
        {"net_links_establish", (PyCFunction)dap_chain_net_links_establish_py, METH_VARARGS,
         "Establish network links"},
        {"net_purge", (PyCFunction)dap_chain_net_purge_py, METH_VARARGS,
         "Purge network data"},
        {"net_announce_addr", (PyCFunction)dap_chain_net_announce_addr_py, METH_VARARGS,
         "Announce network address"},
        {"net_announce_addr_all", (PyCFunction)dap_chain_net_announce_addr_all_py, METH_NOARGS,
         "Announce all network addresses"},
        {"net_tx_get_fee", (PyCFunction)dap_chain_net_tx_get_fee_py, METH_VARARGS,
         "Get network transaction fee"},
        {"net_tx_set_fee", (PyCFunction)dap_chain_net_tx_set_fee_py, METH_VARARGS,
         "Set network transaction fee"},

        // GDB/JSON helper functions
        {"net_get_gdb_group_acl", (PyCFunction)dap_chain_net_get_gdb_group_acl_py, METH_VARARGS,
         "Get ACL GDB group"},
        {"net_get_gdb_group_nochain_new", (PyCFunction)dap_chain_net_get_gdb_group_nochain_new_py, METH_VARARGS,
         "Get no-chain GDB group"},
        {"net_get_gdb_group_mempool_by_chain_type", (PyCFunction)dap_chain_net_get_gdb_group_mempool_by_chain_type_py, METH_VARARGS,
         "Get mempool GDB group by chain type"},
        {"net_list_json_collect", (PyCFunction)dap_chain_net_list_json_collect_py, METH_VARARGS,
         "Collect network list JSON"},
        {"net_states_json_collect", (PyCFunction)dap_chain_net_states_json_collect_py, METH_VARARGS,
         "Collect network states JSON"},
        {"nets_info_json_collect", (PyCFunction)dap_chain_nets_info_json_collect_py, METH_VARARGS,
         "Collect networks info JSON"},
        {"cli_list_net", (PyCFunction)dap_cli_list_net_py, METH_NOARGS,
         "Get networks list as CLI string"},
        {"ledger_by_net_name", (PyCFunction)dap_ledger_by_net_name_py, METH_VARARGS,
         "Get ledger by network name"},
        
        // Advanced functions with complex structures
        {"net_get_cur_cell", (PyCFunction)dap_chain_net_get_cur_cell_py, METH_VARARGS,
         "Get current cell ID"},
        {"net_get_my_node_info", (PyCFunction)dap_chain_net_get_my_node_info_py, METH_VARARGS,
         "Get current node info"},
        {"net_get_authorized_nodes", (PyCFunction)dap_chain_net_get_authorized_nodes_py, METH_VARARGS,
         "Get authorized nodes list"},
        {"net_verify_datum_for_add", (PyCFunction)dap_chain_net_verify_datum_for_add_py, METH_VARARGS,
         "Verify datum for adding to chain"},
        {"net_verify_datum_err_code_to_str", (PyCFunction)dap_chain_net_verify_datum_err_code_to_str_py, METH_VARARGS,
         "Convert datum verification error code to string"},
        {"net_datum_list", (PyCFunction)dap_chain_datum_list_py, METH_VARARGS,
         "List datums with optional filter"},
        {"net_datum_add", (PyCFunction)dap_chain_datum_add_py, METH_VARARGS,
         "Add datum to chain"},
        {"net_datum_remove", (PyCFunction)dap_chain_datum_remove_py, METH_VARARGS,
         "Remove datum from chain"},
        {"net_proc_mempool", (PyCFunction)dap_chain_net_proc_mempool_py, METH_VARARGS,
         "Process network mempool"},
        {"net_get_mempool_cluster", (PyCFunction)dap_chain_net_get_mempool_cluster_py, METH_VARARGS,
         "Get mempool cluster for chain"},
        {"net_add_auth_nodes_to_cluster", (PyCFunction)dap_chain_net_add_auth_nodes_to_cluster_py, METH_VARARGS,
         "Add authorized nodes to cluster"},
        {"net_add_validator_to_clusters", (PyCFunction)dap_chain_net_add_validator_to_clusters_py, METH_VARARGS,
         "Add validator to clusters"},
        {"net_remove_validator_from_clusters", (PyCFunction)dap_chain_net_remove_validator_from_clusters_py, METH_VARARGS,
         "Remove validator from clusters"},
        {"net_add_reward", (PyCFunction)dap_chain_net_add_reward_py, METH_VARARGS,
         "Add reward to network"},
        {"net_remove_last_reward", (PyCFunction)dap_chain_net_remove_last_reward_py, METH_VARARGS,
         "Remove last reward from network"},
        {"net_get_reward", (PyCFunction)dap_chain_net_get_reward_py, METH_VARARGS,
         "Get reward for block"},
        
        // Callback functions
        {"chain_add_mempool_notify_callback", (PyCFunction)dap_chain_add_mempool_notify_callback_py, METH_VARARGS,
         "Add mempool notify callback for chain"},
        {"net_add_nodelist_notify_callback", (PyCFunction)dap_chain_net_add_nodelist_notify_callback_py, METH_VARARGS,
         "Add nodelist notify callback for network"},
        {"net_srv_order_add_notify_callback", (PyCFunction)dap_chain_net_srv_order_add_notify_callback_py, METH_VARARGS,
         "Add service order notify callback for network"},
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    
    // Register all methods
    for (PyMethodDef *l_method = l_network_methods; l_method->ml_name != NULL; l_method++) {
        PyObject *l_func = PyCFunction_New(l_method, NULL);
        if (!l_func) {
            log_it(L_ERROR, "Failed to create function %s", l_method->ml_name);
            return -1;
        }
        
        if (PyModule_AddObject(a_module, l_method->ml_name, l_func) < 0) {
            log_it(L_ERROR, "Failed to register function %s", l_method->ml_name);
            Py_DECREF(l_func);
            return -1;
        }
    }

    if (cellframe_net_tx_init(a_module) < 0) {
        log_it(L_ERROR, "Failed to initialize net tx module");
        return -1;
    }
    if (cellframe_net_ch_init(a_module) < 0) {
        log_it(L_ERROR, "Failed to initialize net ch module");
        return -1;
    }
    if (cellframe_net_ch_pkt_init(a_module) < 0) {
        log_it(L_ERROR, "Failed to initialize net ch pkt module");
        return -1;
    }
    if (cellframe_net_cli_init(a_module) < 0) {
        log_it(L_ERROR, "Failed to initialize net cli module");
        return -1;
    }
    if (cellframe_net_decree_init(a_module) < 0) {
        log_it(L_ERROR, "Failed to initialize net decree module");
        return -1;
    }
    
    log_it(L_INFO, "Network module initialized successfully");
    return 0;
}
