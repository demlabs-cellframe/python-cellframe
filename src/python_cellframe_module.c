#include "python_cellframe.h"
#include <string.h>

// =========================================
// ERROR OBJECTS
// =========================================
PyObject *CellframeError = NULL;
PyObject *CellframeChainError = NULL;
PyObject *CellframeNetworkError = NULL;
PyObject *CellframeWalletError = NULL;
PyObject *CellframeNodeError = NULL;

// =========================================
// UTILITY FUNCTIONS
// =========================================
bool cellframe_sdk_is_available(void) {
    return CELLFRAME_AVAILABLE;
}

PyObject* cellframe_sdk_is_available_wrapper(PyObject *self) {
    (void)self;
    
    if (cellframe_sdk_is_available()) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject* cellframe_get_version(PyObject *self) {
    (void)self;
    return PyUnicode_FromString(PYTHON_CELLFRAME_VERSION);
}

PyObject* cellframe_initialize(PyObject *self, PyObject *args, PyObject *kwds) {
    (void)self;
    
    // Optional initialization parameters
    const char* app_name = NULL;
    const char* working_dir = NULL;
    const char* config_dir = NULL;
    const char* temp_dir = NULL;
    const char* log_file = NULL;
    int events_threads = 1;
    int events_timeout = 10000;
    int debug_mode = 0;
    
    static char *kwlist[] = {
        "app_name", "working_dir", "config_dir", "temp_dir", 
        "log_file", "events_threads", "events_timeout", "debug_mode", 
        NULL
    };
    
    // Parse optional keyword arguments
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|sssssiii", kwlist,
                                     &app_name, &working_dir, &config_dir, &temp_dir,
                                     &log_file, &events_threads, &events_timeout, &debug_mode)) {
        return NULL;
    }
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // If no parameters provided, assume DAP SDK is already initialized
    if (!app_name && !working_dir) {
        // Check if DAP SDK is already initialized
        // For now, just return success
        Py_RETURN_TRUE;
    }
    
    // Try to import python_dap and call dap_sdk_init
    PyObject* dap_module = PyImport_ImportModule("python_dap");
    if (!dap_module) {
        PyErr_SetString(CellframeError, "Failed to import python_dap module for initialization");
        return NULL;
    }
    
    PyObject* init_func = PyObject_GetAttrString(dap_module, "dap_sdk_init");
    Py_DECREF(dap_module);
    
    if (!init_func || !PyCallable_Check(init_func)) {
        Py_XDECREF(init_func);
        PyErr_SetString(CellframeError, "Failed to find dap_sdk_init function in python_dap");
        return NULL;
    }
    
    // Call dap_sdk_init with parameters
    PyObject* init_args = PyTuple_New(8);
    if (!init_args) {
        Py_DECREF(init_func);
        return NULL;
    }
    
    // Set tuple items, using empty strings for NULL strings
    PyTuple_SET_ITEM(init_args, 0, PyUnicode_FromString(app_name ? app_name : "cellframe"));
    PyTuple_SET_ITEM(init_args, 1, PyUnicode_FromString(working_dir ? working_dir : ""));
    PyTuple_SET_ITEM(init_args, 2, PyUnicode_FromString(config_dir ? config_dir : ""));
    PyTuple_SET_ITEM(init_args, 3, PyUnicode_FromString(temp_dir ? temp_dir : ""));
    PyTuple_SET_ITEM(init_args, 4, PyUnicode_FromString(log_file ? log_file : ""));
    PyTuple_SET_ITEM(init_args, 5, PyLong_FromLong(events_threads));
    PyTuple_SET_ITEM(init_args, 6, PyLong_FromLong(events_timeout));
    PyTuple_SET_ITEM(init_args, 7, PyBool_FromLong(debug_mode));
    
    PyObject* result = PyObject_CallObject(init_func, init_args);
    Py_DECREF(init_func);
    Py_DECREF(init_args);
    
    if (!result) {
        return NULL;  // Error already set by called function
    }
    
    // Check if initialization was successful (result should be 0)
    long init_result = PyLong_AsLong(result);
    Py_DECREF(result);
    
    if (init_result != 0) {
        PyErr_Format(CellframeError, "DAP SDK initialization failed with code %ld", init_result);
        return NULL;
    }
    
    Py_RETURN_TRUE;
#else
    // Stub mode - no real initialization
    PyErr_SetString(CellframeError, "Cellframe SDK not embedded in this build");
    return NULL;
#endif
}

PyObject* cellframe_deinitialize(PyObject *self) {
    (void)self;
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // Real Cellframe SDK deinitialization
    // TODO: Add actual dap_common_deinit() call
    return PyBool_FromLong(1);
#else
    // Stub mode - no real deinitialization needed
    return PyBool_FromLong(1);
#endif
}

// =========================================
// MODULE METHODS
// =========================================
static PyMethodDef CellframeMethods[] = {
    {"get_version", (PyCFunction)cellframe_get_version, METH_NOARGS, 
     "Get Python Cellframe SDK version"},
    {"initialize", (PyCFunction)cellframe_initialize, METH_VARARGS | METH_KEYWORDS,
     "Initialize Cellframe SDK\n"
     "Args:\n"
     "    app_name (str, optional): Application name (default: 'cellframe')\n"
     "    working_dir (str, optional): Working directory path\n"
     "    config_dir (str, optional): Config directory path\n"
     "    temp_dir (str, optional): Temp directory path\n"
     "    log_file (str, optional): Log file path\n"
     "    events_threads (int, optional): Number of event threads (default: 1)\n"
     "    events_timeout (int, optional): Events timeout in ms (default: 10000)\n"
     "    debug_mode (bool, optional): Enable debug mode (default: False)\n"
     "\n"
     "If called without parameters, assumes DAP SDK is already initialized."},
    {"deinitialize", (PyCFunction)cellframe_deinitialize, METH_NOARGS,
     "Deinitialize Cellframe SDK"},
    {"is_sdk_available", (PyCFunction)cellframe_sdk_is_available_wrapper, METH_NOARGS,
     "Check if Cellframe SDK is available"},
    
    // Wallet functions
    {"dap_chain_wallet_create", py_dap_chain_wallet_create, METH_VARARGS,
     "Create a new wallet"},
    {"dap_chain_wallet_create_with_seed", py_dap_chain_wallet_create_with_seed, METH_VARARGS,
     "Create a new wallet with seed"},
    {"dap_chain_wallet_create_with_seed_multi", py_dap_chain_wallet_create_with_seed_multi, METH_VARARGS,
     "Create a new wallet with seed and multiple signatures"},
    {"dap_chain_wallet_open", py_dap_chain_wallet_open, METH_VARARGS,
     "Open an existing wallet"},
    {"dap_chain_wallet_open_ext", py_dap_chain_wallet_open_ext, METH_VARARGS,
     "Open an existing wallet with extended parameters"},
    {"dap_chain_wallet_close", py_dap_chain_wallet_close, METH_VARARGS,
     "Close a wallet"},
    {"dap_chain_wallet_save", py_dap_chain_wallet_save, METH_VARARGS,
     "Save a wallet"},
    {"dap_chain_wallet_get_addr", py_dap_chain_wallet_get_addr, METH_VARARGS,
     "Get wallet address"},
    {"dap_chain_wallet_get_balance", py_dap_chain_wallet_get_balance, METH_VARARGS,
     "Get wallet balance"},
    {"dap_chain_wallet_get_key", py_dap_chain_wallet_get_key, METH_VARARGS,
     "Get wallet key"},
    {"dap_chain_wallet_get_pkey", py_dap_chain_wallet_get_pkey, METH_VARARGS,
     "Get wallet public key"},
    {"dap_chain_wallet_activate", py_dap_chain_wallet_activate, METH_VARARGS,
     "Activate a wallet"},
    {"dap_chain_wallet_deactivate", py_dap_chain_wallet_deactivate, METH_VARARGS,
     "Deactivate a wallet"},
    
    {NULL, NULL, 0, NULL}  // Sentinel
};

// =========================================
// MODULE DEFINITION
// =========================================
static struct PyModuleDef cellframemodule = {
    PyModuleDef_HEAD_INIT,
    PYTHON_CELLFRAME_MODULE_NAME,   // Module name
    "Python bindings for Cellframe Network SDK",  // Module documentation
    -1,       // Size of per-interpreter state or -1 if global
    CellframeMethods,
    NULL,     // m_slots
    NULL,     // m_traverse  
    NULL,     // m_clear
    NULL      // m_free
};

// =========================================
// MODULE INITIALIZATION
// =========================================
PyMODINIT_FUNC PyInit_python_cellframe(void) {
    PyObject *module;
    
    // Create module
    module = PyModule_Create(&cellframemodule);
    if (module == NULL) {
        return NULL;
    }
    
    // Create exception types
    CellframeError = PyErr_NewException("python_cellframe.CellframeError", NULL, NULL);
    if (CellframeError == NULL) {
        Py_DECREF(module);
        return NULL;
    }
    Py_INCREF(CellframeError);
    PyModule_AddObject(module, "CellframeError", CellframeError);
    
    CellframeChainError = PyErr_NewException("python_cellframe.CellframeChainError", 
                                             CellframeError, NULL);
    if (CellframeChainError == NULL) {
        Py_DECREF(module);
        return NULL;
    }
    Py_INCREF(CellframeChainError);
    PyModule_AddObject(module, "CellframeChainError", CellframeChainError);
    
    CellframeNetworkError = PyErr_NewException("python_cellframe.CellframeNetworkError", 
                                               CellframeError, NULL);
    if (CellframeNetworkError == NULL) {
        Py_DECREF(module);
        return NULL;
    }
    Py_INCREF(CellframeNetworkError);
    PyModule_AddObject(module, "CellframeNetworkError", CellframeNetworkError);
    
    CellframeWalletError = PyErr_NewException("python_cellframe.CellframeWalletError", 
                                              CellframeError, NULL);
    if (CellframeWalletError == NULL) {
        Py_DECREF(module);
        return NULL;
    }
    Py_INCREF(CellframeWalletError);
    PyModule_AddObject(module, "CellframeWalletError", CellframeWalletError);
    
    CellframeNodeError = PyErr_NewException("python_cellframe.CellframeNodeError", 
                                            CellframeError, NULL);
    if (CellframeNodeError == NULL) {
        Py_DECREF(module);
        return NULL;
    }
    Py_INCREF(CellframeNodeError);
    PyModule_AddObject(module, "CellframeNodeError", CellframeNodeError);
    
    // Add module constants
    PyModule_AddStringConstant(module, "__version__", PYTHON_CELLFRAME_VERSION);
    PyModule_AddIntConstant(module, "SDK_AVAILABLE", CELLFRAME_AVAILABLE);
    PyModule_AddIntConstant(module, "DAP_CHAIN_TICKER_SIZE_MAX", DAP_CHAIN_TICKER_SIZE_MAX);
    
#ifdef CELLFRAME_SDK_EMBEDDED
    PyModule_AddStringConstant(module, "BUILD_MODE", "dynamic_with_sdk");
#else
    PyModule_AddStringConstant(module, "BUILD_MODE", "static_no_sdk");
#endif
    
    // Initialize and add types
    // Note: Chain, Network, Wallet, Node types would be added here
    // For now, we'll add them in separate files
    
    return module;
}

// =========================================
// HELPER FUNCTIONS FOR ERROR HANDLING
// =========================================
void set_cellframe_error(const char *message) {
    PyErr_SetString(CellframeError, message);
}

void set_cellframe_chain_error(const char *message) {
    PyErr_SetString(CellframeChainError, message);
}

void set_cellframe_network_error(const char *message) {
    PyErr_SetString(CellframeNetworkError, message);
}

void set_cellframe_wallet_error(const char *message) {
    PyErr_SetString(CellframeWalletError, message);
}

void set_cellframe_node_error(const char *message) {
    PyErr_SetString(CellframeNodeError, message);
} 