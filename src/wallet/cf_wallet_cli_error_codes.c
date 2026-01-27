#include "include/cf_wallet_internal.h"

PyObject *dap_chain_wallet_cli_error_codes_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    int rc = dap_chain_wallet_cli_error_codes_init();
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_cli_error_codes_deinit_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    dap_chain_wallet_cli_error_codes_deinit();
    Py_RETURN_NONE;
}

PyMethodDef *cellframe_wallet_cli_error_codes_get_methods(void) {
    static PyMethodDef methods[] = {
        {"dap_chain_wallet_cli_error_codes_init", dap_chain_wallet_cli_error_codes_init_py, METH_NOARGS, "Init wallet CLI error codes"},
        {"dap_chain_wallet_cli_error_codes_deinit", dap_chain_wallet_cli_error_codes_deinit_py, METH_NOARGS, "Deinit wallet CLI error codes"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
