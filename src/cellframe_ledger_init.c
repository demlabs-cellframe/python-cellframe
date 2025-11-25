#include "cellframe_ledger_internal.h"

// Forward declarations for get_methods functions from all modules
PyMethodDef* cellframe_ledger_lifecycle_get_methods(void);
PyMethodDef* cellframe_ledger_callbacks_get_methods(void);
PyMethodDef* cellframe_ledger_tx_get_methods(void);
PyMethodDef* cellframe_ledger_token_get_methods(void);
PyMethodDef* cellframe_ledger_balance_get_methods(void);
PyMethodDef* cellframe_ledger_tx_lists_get_methods(void);
PyMethodDef* cellframe_ledger_cond_get_methods(void);
PyMethodDef* cellframe_ledger_decree_get_methods(void);
PyMethodDef* cellframe_ledger_anchor_get_methods(void);
PyMethodDef* cellframe_ledger_event_get_methods(void);
PyMethodDef* cellframe_ledger_utils_get_methods(void);

/**
 * @brief Helper macro to register methods from a module
 */
#define REGISTER_MODULE_METHODS(module_get_methods_func) \
    do { \
        PyMethodDef *methods = module_get_methods_func(); \
        for (PyMethodDef *method = methods; method->ml_name != NULL; method++) { \
            PyObject *func = PyCFunction_New(method, NULL); \
            if (!func) { \
                return -1; \
            } \
            if (PyModule_AddObject(module_obj, method->ml_name, func) < 0) { \
                Py_DECREF(func); \
                return -1; \
            } \
        } \
    } while (0)

/**
 * @brief Initialize ledger module - register all methods from all submodules
 * @param module_obj Python module object
 * @return 0 on success, -1 on error
 */
int cellframe_ledger_init(PyObject *module_obj) {
    // Register methods from all modules
    REGISTER_MODULE_METHODS(cellframe_ledger_lifecycle_get_methods);
    REGISTER_MODULE_METHODS(cellframe_ledger_tx_get_methods);
    REGISTER_MODULE_METHODS(cellframe_ledger_token_get_methods);
    REGISTER_MODULE_METHODS(cellframe_ledger_balance_get_methods);
    REGISTER_MODULE_METHODS(cellframe_ledger_tx_lists_get_methods);
    REGISTER_MODULE_METHODS(cellframe_ledger_cond_get_methods);
    REGISTER_MODULE_METHODS(cellframe_ledger_callbacks_get_methods);
    REGISTER_MODULE_METHODS(cellframe_ledger_decree_get_methods);
    REGISTER_MODULE_METHODS(cellframe_ledger_anchor_get_methods);
    REGISTER_MODULE_METHODS(cellframe_ledger_event_get_methods);
    REGISTER_MODULE_METHODS(cellframe_ledger_utils_get_methods);
    
    return 0;
}

