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

PyObject* cellframe_get_version(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyUnicode_FromString(PYTHON_CELLFRAME_VERSION);
}

PyObject* cellframe_initialize(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    
#ifdef CELLFRAME_SDK_EMBEDDED
    // Real Cellframe SDK initialization
    // TODO: Add actual dap_common_init() call
    return PyBool_FromLong(1);
#else
    // Stub mode - no real initialization
    PyErr_SetString(CellframeError, "Cellframe SDK not embedded in this build");
    return NULL;
#endif
}

PyObject* cellframe_deinitialize(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    
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
    {"get_version", cellframe_get_version, METH_NOARGS, 
     "Get Python Cellframe SDK version"},
    {"initialize", cellframe_initialize, METH_NOARGS,
     "Initialize Cellframe SDK"},
    {"deinitialize", cellframe_deinitialize, METH_NOARGS,
     "Deinitialize Cellframe SDK"},
    {"is_sdk_available", (PyCFunction)cellframe_sdk_is_available, METH_NOARGS,
     "Check if Cellframe SDK is available"},
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
    CellframeMethods
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