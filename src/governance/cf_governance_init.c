#include "cellframe.h"
#include "../include/cf_governance.h"

#define LOG_TAG "python_cellframe_governance"

int cellframe_governance_init(PyObject *module) {
    PyMethodDef *methods = cellframe_governance_get_methods();

    if (!methods) {
        log_it(L_ERROR, "Failed to get governance methods");
        return -1;
    }

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

    log_it(L_DEBUG, "Governance module initialized successfully");
    return 0;
}
