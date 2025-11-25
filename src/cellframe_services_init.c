#include "cellframe.h"
#include "include/cellframe_services.h"

#define LOG_TAG "python_cellframe_services"

/**
 * @brief Initialize services module and register methods
 * @param module Python module object
 * @return 0 on success, -1 on error
 */
int cellframe_services_init(PyObject *module) {
    PyMethodDef *methods = cellframe_services_get_methods();
    
    if (!methods) {
        log_it(L_ERROR, "Failed to get services methods");
        return -1;
    }
    
    // Register all services methods
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
    
    log_it(L_DEBUG, "Services module initialized successfully");
    return 0;
}

