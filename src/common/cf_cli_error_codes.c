#include "cf_common_utils.h"
#include "dap_cli_error_codes.h"

#define LOG_TAG "python_cellframe_common"

PyObject *dap_cli_error_code_get_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_name)) {
        return NULL;
    }
    int l_code = dap_cli_error_code_get(l_name);
    return PyLong_FromLong(l_code);
}

PyObject *dap_cli_error_code_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name = NULL;
    int l_code = 0;
    const char *l_desc = NULL;

    if (!PyArg_ParseTuple(a_args, "si|z", &l_name, &l_code, &l_desc)) {
        return NULL;
    }
    int l_rc = dap_cli_error_code_register(l_name, l_code, l_desc);
    return PyLong_FromLong(l_rc);
}

PyObject *dap_cli_error_codes_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_cli_error_codes_deinit();
    Py_RETURN_NONE;
}

PyObject *dap_cli_error_codes_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_cli_error_codes_init();
    Py_RETURN_NONE;
}

int cf_common_register_cli_error_codes(PyObject *module) {
    static PyMethodDef methods[] = {
        {"dap_cli_error_code_get", dap_cli_error_code_get_py, METH_VARARGS,
         "Get CLI error code by name"},
        {"dap_cli_error_code_register", dap_cli_error_code_register_py, METH_VARARGS,
         "Register CLI error code"},
        {"dap_cli_error_codes_deinit", dap_cli_error_codes_deinit_py, METH_NOARGS,
         "Deinitialize CLI error codes"},
        {"dap_cli_error_codes_init", dap_cli_error_codes_init_py, METH_NOARGS,
         "Initialize CLI error codes"},
        {NULL, NULL, 0, NULL}
    };

    return cf_common_register_methods(module, methods);
}
