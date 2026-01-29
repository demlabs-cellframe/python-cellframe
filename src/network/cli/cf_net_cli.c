#include "cellframe.h"
#include "dap_chain_net_cli.h"
#include "dap_chain_net_cli_error_codes.h"

#define LOG_TAG "python_cellframe_net_cli"

PyObject* dap_chain_net_cli_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_net_cli_init();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_net_cli_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_net_cli_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_cli_error_codes_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_net_cli_error_codes_init();
    Py_RETURN_NONE;
}

PyMethodDef* cellframe_net_cli_get_methods(void) {
    static PyMethodDef methods[] = {
        {"net_cli_init", (PyCFunction)dap_chain_net_cli_init_py, METH_VARARGS, "Initialize net CLI"},
        {"net_cli_deinit", (PyCFunction)dap_chain_net_cli_deinit_py, METH_VARARGS, "Deinitialize net CLI"},
        {"net_cli_error_codes_init", (PyCFunction)dap_chain_net_cli_error_codes_init_py, METH_VARARGS,
         "Initialize net CLI error codes"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}

int cellframe_net_cli_init(PyObject *module) {
    PyMethodDef *methods = cellframe_net_cli_get_methods();
    if (!methods) {
        log_it(L_ERROR, "Failed to get net CLI methods");
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

    log_it(L_DEBUG, "Net CLI module initialized successfully");
    return 0;
}
