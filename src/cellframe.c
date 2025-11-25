#include "cellframe.h"
#include <string.h>

// Forward declarations for chain functions
PyObject* py_dap_chain_addr_from_str(PyObject *self, PyObject *args);
PyObject* py_dap_chain_addr_get_net_id(PyObject *self, PyObject *args);
PyObject* py_dap_chain_atom_create(PyObject *self, PyObject *args);
PyObject* py_dap_chain_mempool_by_chain_name(PyObject *self, PyObject *args);
PyObject* py_dap_chain_mempool_tx_get_by_hash(PyObject *self, PyObject *args);

// Forward declarations for submodule init functions  
int cellframe_network_init(PyObject *module);
int cellframe_wallet_init(PyObject *module);
int cellframe_chain_init(PyObject *module);
int cellframe_ledger_init(PyObject *module);
int cellframe_tx_init(PyObject *module);
int cellframe_consensus_init(PyObject *module);
int cellframe_stake_init(PyObject *module);
int cellframe_mempool_init(PyObject *module);
int cellframe_services_init(PyObject *module);
int cellframe_node_init(PyObject *module);
int cellframe_compose_init(PyObject *module);
int cellframe_net_balancer_init(PyObject *module);
int cellframe_services_ext_init(PyObject *module);

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
}

PyObject* cellframe_deinitialize(PyObject *self) {
    (void)self;
    
    // Real Cellframe SDK deinitialization
    dap_common_deinit();
    return PyBool_FromLong(1);
}

// =========================================
// MODULE METHODS
// =========================================
static PyMethodDef CellframeMethods[] = {
    {"get_version", (PyCFunction)(void*)cellframe_get_version, METH_NOARGS, 
     "Get Python Cellframe SDK version"},
    {"initialize", (PyCFunction)(void*)cellframe_initialize, METH_VARARGS | METH_KEYWORDS,
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
    {"deinitialize", (PyCFunction)(void*)cellframe_deinitialize, METH_NOARGS,
     "Deinitialize Cellframe SDK"},
    {"is_sdk_available", (PyCFunction)(void*)cellframe_sdk_is_available_wrapper, METH_NOARGS,
     "Check if Cellframe SDK is available"},
    
    // NOTE: All specific functions (network, wallet, chain, ledger, etc.) are now registered
    // by their respective modules through init functions. This keeps only core
    // module functions here
    
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
    
    PyModule_AddStringConstant(module, "BUILD_MODE", "dynamic_with_sdk");
    
    // Initialize and add types
    // Note: Chain, Network, Wallet, Node types would be added here
    // Initialize submodules by calling their init functions
    // This replaces the old method of listing all functions in one big array
    
    // Initialize wallet module
    if (cellframe_wallet_init(module) < 0) {
        Py_DECREF(module);
        return NULL;
    }
    
    // Initialize chain module  
    if (cellframe_chain_init(module) < 0) {
        Py_DECREF(module);
        return NULL;
    }
    
    // Initialize ledger module
    if (cellframe_ledger_init(module) < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to initialize ledger module");
        Py_DECREF(module);
        return NULL;
    }

    if (cellframe_tx_init(module) < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to initialize TX module");
        Py_DECREF(module);
        return NULL;
    }
    
    // Initialize network module
    if (cellframe_network_init(module) < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to initialize network module");
        Py_DECREF(module);
        return NULL;
    }
    
    // Initialize consensus module
    if (cellframe_consensus_init(module) < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to initialize consensus module");
        Py_DECREF(module);
        return NULL;
    }
    
    // Initialize stake module
    if (cellframe_stake_init(module) < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to initialize stake module");
        Py_DECREF(module);
        return NULL;
    }
    
    // Initialize mempool module
    if (cellframe_mempool_init(module) < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to initialize mempool module");
        Py_DECREF(module);
        return NULL;
    }
    
    // Initialize services module
    if (cellframe_services_init(module) < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to initialize services module");
        Py_DECREF(module);
        return NULL;
    }
    
    // Initialize node CLI module
    if (cellframe_node_init(module) < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to initialize node CLI module");
        Py_DECREF(module);
        return NULL;
    }
    
    // Initialize compose module
    if (cellframe_compose_init(module) < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to initialize compose module");
        Py_DECREF(module);
        return NULL;
    }
    
    // Initialize net balancer module
    if (cellframe_net_balancer_init(module) < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to initialize net balancer module");
        Py_DECREF(module);
        return NULL;
    }

    if (cellframe_services_ext_init(module) < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to initialize services ext module");
        Py_DECREF(module);
        return NULL;
    }
    
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

 