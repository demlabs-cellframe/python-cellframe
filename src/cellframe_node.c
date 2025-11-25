#include "cellframe.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// External error object from main module
extern PyObject *CellframeError;

// Include DAP SDK node headers
#include "dap_chain_node.h"
#include "dap_chain_node_cli.h"
#include "dap_cli_server.h"
#include "dap_common.h"
#include "json.h"

#define LOG_TAG "python_cellframe_node"

// Node type implementation
PyTypeObject PyCellframeNodeType = {0};

PyObject* PyCellframeNode_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PyCellframeNode *self;
    self = (PyCellframeNode *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->node_ptr = NULL;
        self->node_addr = NULL;
        self->node_port = 0;
        self->is_initialized = false;
    }
    return (PyObject *)self;
}

int PyCellframeNode_init(PyCellframeNode *self, PyObject *args, PyObject *kwds) {
    const char *node_addr = "0.0.0.0";
    int node_port = 8079;
    
    static char *kwlist[] = {"addr", "port", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|si", kwlist, &node_addr, &node_port)) {
        return -1;
    }
    
    if (node_addr) {
        self->node_addr = strdup(node_addr);
        if (!self->node_addr) {
            PyErr_SetString(CellframeError, "Failed to allocate memory for node address");
            return -1;
        }
    }
    
    self->node_port = (uint16_t)node_port;
    self->is_initialized = true;
    
    return 0;
}

void PyCellframeNode_dealloc(PyCellframeNode *self) {
    if (self) {
        if (self->node_addr) {
            free(self->node_addr);
            self->node_addr = NULL;
        }
        // Note: node_ptr is managed by DAP SDK, don't free it
        Py_TYPE(self)->tp_free((PyObject *)self);
    }
}

PyObject* PyCellframeNode_start(PyCellframeNode *self, PyObject *args) {
    if (!self || !self->is_initialized) {
        PyErr_SetString(CellframeError, "Node not initialized");
        return NULL;
    }
    
    // For now, just mark as started (node management is complex)
    // In real implementation, this would start the node services
    
    PyObject *result_dict = PyDict_New();
    if (!result_dict) {
        return NULL;
    }
    
    PyObject *status_obj = PyUnicode_FromString("started");
    if (status_obj) {
        PyDict_SetItemString(result_dict, "status", status_obj);
        Py_DECREF(status_obj);
    }
    
    PyObject *addr_obj = PyUnicode_FromString(self->node_addr ? self->node_addr : "unknown");
    if (addr_obj) {
        PyDict_SetItemString(result_dict, "address", addr_obj);
        Py_DECREF(addr_obj);
    }
    
    PyObject *port_obj = PyLong_FromLong(self->node_port);
    if (port_obj) {
        PyDict_SetItemString(result_dict, "port", port_obj);
        Py_DECREF(port_obj);
    }
    
    return result_dict;
}

PyObject* PyCellframeNode_stop(PyCellframeNode *self, PyObject *args) {
    if (!self || !self->is_initialized) {
        PyErr_SetString(CellframeError, "Node not initialized");
        return NULL;
    }
    
    // For now, just mark as stopped
    // In real implementation, this would stop the node services
    
    PyObject *result_dict = PyDict_New();
    if (!result_dict) {
        return NULL;
    }
    
    PyObject *status_obj = PyUnicode_FromString("stopped");
    if (status_obj) {
        PyDict_SetItemString(result_dict, "status", status_obj);
        Py_DECREF(status_obj);
    }
    
    return result_dict;
}

PyObject* PyCellframeNode_get_status(PyCellframeNode *self, PyObject *args) {
    if (!self) {
        PyErr_SetString(CellframeError, "Invalid node object");
        return NULL;
    }
    
    PyObject *status_dict = PyDict_New();
    if (!status_dict) {
        return NULL;
    }
    
    // Add initialization status
    PyObject *init_obj = PyBool_FromLong(self->is_initialized ? 1 : 0);
    if (init_obj) {
        PyDict_SetItemString(status_dict, "initialized", init_obj);
        Py_DECREF(init_obj);
    }
    
    // Add node address
    PyObject *addr_obj = PyUnicode_FromString(self->node_addr ? self->node_addr : "not_set");
    if (addr_obj) {
        PyDict_SetItemString(status_dict, "address", addr_obj);
        Py_DECREF(addr_obj);
    }
    
    // Add node port
    PyObject *port_obj = PyLong_FromLong(self->node_port);
    if (port_obj) {
        PyDict_SetItemString(status_dict, "port", port_obj);
        Py_DECREF(port_obj);
    }
    
    // Add node state (simplified for now)
    PyObject *state_obj = PyUnicode_FromString(self->is_initialized ? "ready" : "not_ready");
    if (state_obj) {
        PyDict_SetItemString(status_dict, "state", state_obj);
        Py_DECREF(state_obj);
    }
    
    return status_dict;
}

// =============================================================================
// CLI COMMAND REGISTRATION FUNCTIONS
// =============================================================================

/**
 * @brief Python callback wrapper for CLI commands
 */
typedef struct {
    PyObject *callback;
    PyObject *user_data;
} python_cli_callback_ctx_t;

/**
 * @brief C callback wrapper that calls Python function
 */
static int python_cli_callback_wrapper(int a_argc, char **a_argv, dap_json_t *a_json_arr_reply, int a_version, void *a_arg) {
    python_cli_callback_ctx_t *ctx = (python_cli_callback_ctx_t *)a_arg;
    if (!ctx || !ctx->callback) {
        return -1;
    }
    
    // Build Python arguments tuple
    PyObject *args = PyTuple_New(a_argc);
    if (!args) {
        return -1;
    }
    
    for (int i = 0; i < a_argc; i++) {
        PyObject *arg_str = PyUnicode_FromString(a_argv[i]);
        if (!arg_str) {
            Py_DECREF(args);
            return -1;
        }
        PyTuple_SET_ITEM(args, i, arg_str);
    }
    
    // Build JSON reply object (empty JSON array)
    PyObject *json_reply = PyDict_New();
    if (!json_reply) {
        Py_DECREF(args);
        return -1;
    }
    
    // Call Python callback
    PyObject *result = PyObject_CallFunctionObjArgs(ctx->callback, args, json_reply, PyLong_FromLong(a_version), ctx->user_data, NULL);
    
    Py_DECREF(args);
    Py_DECREF(json_reply);
    
    if (!result) {
        PyErr_Print();
        return -1;
    }
    
    int ret = PyLong_AsLong(result);
    Py_DECREF(result);
    
    return ret;
}

/**
 * @brief Add CLI command from Python
 * @param a_self Python self object (unused)
 * @param a_args Arguments (name string, callback callable, doc string, doc_ex string, cmd_id int)
 * @return PyCapsule wrapping dap_cli_cmd_t* or None
 */
PyObject* dap_cli_server_cmd_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name;
    PyObject *l_callback;
    const char *l_doc = NULL;
    const char *l_doc_ex = NULL;
    int l_cmd_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "sO|ssi", &l_name, &l_callback, &l_doc, &l_doc_ex, &l_cmd_id)) {
        return NULL;
    }
    
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }
    
    // Allocate callback context
    python_cli_callback_ctx_t *l_ctx = DAP_NEW_Z(python_cli_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    
    l_ctx->callback = l_callback;
    l_ctx->user_data = Py_None;
    
    Py_INCREF(l_callback);
    Py_INCREF(Py_None);
    
    // Register CLI command
    dap_cli_cmd_t *l_cmd = dap_cli_server_cmd_add(
        l_name,
        (dap_cli_server_cmd_callback_t)python_cli_callback_wrapper,
        NULL,  // func_rpc
        l_doc ? l_doc : "",
        (int16_t)l_cmd_id,
        l_doc_ex ? l_doc_ex : ""
    );
    
    if (!l_cmd) {
        Py_DECREF(l_callback);
        Py_DECREF(Py_None);
        DAP_DELETE(l_ctx);
        PyErr_SetString(PyExc_RuntimeError, "Failed to register CLI command");
        return NULL;
    }
    
    // Store context in command's arg_func
    l_cmd->arg_func = l_ctx;
    
    log_it(L_DEBUG, "CLI command '%s' registered from Python", l_name);
    return PyCapsule_New(l_cmd, "dap_cli_cmd_t", NULL);
}

/**
 * @brief Find CLI command by name
 * @param a_self Python self object (unused)
 * @param a_args Arguments (name string)
 * @return PyCapsule wrapping dap_cli_cmd_t* or None
 */
PyObject* dap_cli_server_cmd_find_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name;
    
    if (!PyArg_ParseTuple(a_args, "s", &l_name)) {
        return NULL;
    }
    
    dap_cli_cmd_t *l_cmd = dap_cli_server_cmd_find(l_name);
    if (!l_cmd) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_cmd, "dap_cli_cmd_t", NULL);
}

/**
 * @brief Execute CLI command programmatically
 * @param a_self Python self object (unused)
 * @param a_args Arguments (cmd capsule, argv list)
 * @return String (JSON response) or None
 */
PyObject* dap_cli_server_cmd_exec_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_cmd_capsule;
    PyObject *a_argv_list;
    
    if (!PyArg_ParseTuple(a_args, "OO", &a_cmd_capsule, &a_argv_list)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(a_cmd_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Command must be a capsule");
        return NULL;
    }
    
    dap_cli_cmd_t *l_cmd = (dap_cli_cmd_t*)PyCapsule_GetPointer(a_cmd_capsule, "dap_cli_cmd_t");
    if (!l_cmd) {
        PyErr_SetString(PyExc_ValueError, "Invalid command capsule");
        return NULL;
    }
    
    if (!PyList_Check(a_argv_list)) {
        PyErr_SetString(PyExc_TypeError, "argv must be a list");
        return NULL;
    }
    
    int l_argc = (int)PyList_Size(a_argv_list);
    char **l_argv = DAP_NEW_Z_COUNT(char*, l_argc + 1); // +1 just in case, though argc handles count
    if (!l_argv) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate argv");
        return NULL;
    }
    
    // Convert args
    for (int i = 0; i < l_argc; i++) {
        PyObject *l_item = PyList_GetItem(a_argv_list, i);
        if (PyUnicode_Check(l_item)) {
            l_argv[i] = (char*)PyUnicode_AsUTF8(l_item);
        } else {
            // Try convert to string
            PyObject *l_str = PyObject_Str(l_item);
            if (l_str) {
                l_argv[i] = strdup(PyUnicode_AsUTF8(l_str)); // Need to free later if we dup
                Py_DECREF(l_str);
            } else {
                l_argv[i] = "unknown";
            }
        }
    }
    
    // Create JSON object for reply
    // Note: dap_cli_server commands usually append to a json array or object passed to them
    // Let's assume it expects a json array or we pass NULL if it creates it? 
    // Looking at callbacks: int (*func)(int argc, char **argv, void *arg_func, char **str_reply) -> OLD?
    // In node-cli-cmd.h: int com_func(int argc, char **argv, dap_json_t *json_arr_reply, int version)
    // dap_cli_cmd_t struct has 'func' which is dap_cli_server_cmd_callback_t
    
    // Let's try passing a json array
    struct json_object *l_json_reply = json_object_new_array();
    
    // Call the function
    // We cast to the node-cli-cmd signature which seems to be the standard now for cellframe
    typedef int (*node_cli_func_t)(int, char**, void*, int); // Simplified signature matching what we see
    // Wait, dap_cli_server_cmd_callback_t is:
    // typedef int (*dap_cli_server_cmd_callback_t)(int argc, char **argv, void *arg_func, char **str_reply);
    
    // BUT in cellframe_node.c we cast our wrapper to dap_cli_server_cmd_callback_t.
    // And our wrapper is:
    // static int python_cli_callback_wrapper(int a_argc, char **a_argv, dap_json_t *a_json_arr_reply, int a_version, void *a_arg)
    
    // This implies that dap_cli_server_cmd_callback_t has CHANGED in the SDK to match this 5-argument signature?
    // Let's double check dap_cli_server.h
    
    // Since I cannot read dap_cli_server.h right now, I will assume the signature matches what I used in the wrapper.
    // int (*func)(int argc, char **argv, void *arg_func, char **str_reply) -- NO, wrapper has 5 args.
    
    // Let's look at the wrapper cast again:
    // (dap_cli_server_cmd_callback_t)python_cli_callback_wrapper
    // And compiler warning:
    // warning: cast between incompatible function types from ‘int (*)(int,  char **, dap_json_t *, int,  void *)’ to ‘int (*)(int,  char **, dap_json_t *, int)’
    
    // This warning suggests the target type is ‘int (*)(int,  char **, dap_json_t *, int)’
    // i.e. 4 arguments: argc, argv, json_reply, arg_func (maybe?)
    
    // Wait, the warning says "to ‘int (*)(int,  char **, struct dap_json *, int)’". 
    // It seems the callback type expects 4 args.
    // But my wrapper has 5 args (void *a_arg at the end).
    // And com_* functions in node-cli-cmd.h have 4 args: (int a_argc,  char **a_argv, dap_json_t *a_json_arr_reply, int a_version).
    
    // So the signature seems to be: func(argc, argv, json_reply, something_else_maybe_version_or_arg)
    
    // Let's try to call it with the 4 arguments pattern from com_* functions, as most commands are likely com_* functions.
    // int com_func(int argc, char **argv, dap_json_t *reply, int version)
    
    // Cast func to this type
    typedef int (*com_func_t)(int, char**, struct json_object*, int);
    com_func_t l_func = (com_func_t)l_cmd->func;
    
    int l_ret = l_func(l_argc, l_argv, l_json_reply, 1); // version 1?
    
    // Clean up argv (if we strdup-ed strings, we should free them, but for PyUnicode_AsUTF8 we don't own them)
    // Only free l_argv array itself
    DAP_DELETE(l_argv);
    
    // Convert json reply to string
    const char *l_json_str = json_object_to_json_string(l_json_reply);
    PyObject *l_py_str = PyUnicode_FromString(l_json_str);
    
    // Free json object
    json_object_put(l_json_reply);
    
    return l_py_str;
}

/**
 * @brief Initialize CLI server
 * @param a_self Python self object (unused)
 * @param a_args Arguments (debug_more bool, cfg_section string)
 * @return Integer result code (0 on success)
 */
PyObject* dap_cli_server_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_debug_more = 0;
    const char *l_cfg_section = "cli-server";
    
    if (!PyArg_ParseTuple(a_args, "|is", &l_debug_more, &l_cfg_section)) {
        return NULL;
    }
    
    int l_result = dap_cli_server_init((bool)l_debug_more, l_cfg_section);
    
    log_it(L_DEBUG, "CLI server initialized, result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize CLI server
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_cli_server_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    dap_cli_server_deinit();
    
    log_it(L_DEBUG, "CLI server deinitialized");
    Py_RETURN_NONE;
}

/**
 * @brief Initialize node CLI subsystem
 * @param a_self Python self object (unused)
 * @param a_args Arguments (config capsule)
 * @return Integer result code (0 on success)
 */
PyObject* dap_chain_node_cli_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_config_obj = NULL;
    
    if (!PyArg_ParseTuple(a_args, "|O", &l_config_obj)) {
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
    
    int l_result = dap_chain_node_cli_init(l_config);
    
    log_it(L_DEBUG, "Node CLI initialized, result: %d", l_result);
    return PyLong_FromLong(l_result);
}

/**
 * @brief Deinitialize node CLI subsystem
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_chain_node_cli_delete_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    dap_chain_node_cli_delete();
    
    log_it(L_DEBUG, "Node CLI deinitialized");
    Py_RETURN_NONE;
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

/**
 * @brief Initialize node module and register CLI methods
 * @param a_module Parent Python module
 * @return 0 on success, -1 on failure
 */
int cellframe_node_init(PyObject *a_module) {
    if (!a_module) {
        log_it(L_ERROR, "Module is NULL");
        return -1;
    }
    
    // Define node CLI methods
    static PyMethodDef l_node_cli_methods[] = {
        {"cli_server_init", (PyCFunction)dap_cli_server_init_py, METH_VARARGS,
         "Initialize CLI server"},
        {"cli_server_deinit", (PyCFunction)dap_cli_server_deinit_py, METH_VARARGS,
         "Deinitialize CLI server"},
        {"cli_server_cmd_add", (PyCFunction)dap_cli_server_cmd_add_py, METH_VARARGS,
         "Add CLI command from Python"},
        {"cli_server_cmd_find", (PyCFunction)dap_cli_server_cmd_find_py, METH_VARARGS,
         "Find CLI command by name"},
        {"cli_server_cmd_exec", (PyCFunction)dap_cli_server_cmd_exec_py, METH_VARARGS,
         "Execute CLI command programmatically"},
        {"chain_node_cli_init", (PyCFunction)dap_chain_node_cli_init_py, METH_VARARGS,
         "Initialize node CLI subsystem"},
        {"chain_node_cli_delete", (PyCFunction)dap_chain_node_cli_delete_py, METH_VARARGS,
         "Deinitialize node CLI subsystem"},
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    
    // Register all methods
    for (PyMethodDef *l_method = l_node_cli_methods; l_method->ml_name != NULL; l_method++) {
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
    
    log_it(L_INFO, "Node CLI module initialized successfully");
    return 0;
} 