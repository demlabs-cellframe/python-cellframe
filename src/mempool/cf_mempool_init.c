#include "cellframe.h"
#include "../include/cf_mempool.h"

#define LOG_TAG "python_cellframe_mempool"

/**
 * @brief Initialize mempool module and register methods
 * @param module Python module object
 * @return 0 on success, -1 on error
 */
int cellframe_mempool_init(PyObject *module) {
    PyMethodDef *methods = cellframe_mempool_get_methods();
    
    if (!methods) {
        log_it(L_ERROR, "Failed to get mempool methods");
        return -1;
    }
    
    // Register all mempool methods
    for (PyMethodDef *method = methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
        if (!func) {
            log_it(L_ERROR, "Failed to create function for %s", method->ml_name);
            return -1;
        }
        
        if (PyModule_AddObject(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            log_it(L_ERROR, "Failed to add function %s to module", method->ml_name);
            return -1;
        }
    }
    
    log_it(L_DEBUG, "Mempool module initialized successfully");
    return 0;
}

