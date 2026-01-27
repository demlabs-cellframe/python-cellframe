#include "include/cf_ledger_internal.h"
#include "dap_chain_ledger_cli_error_codes.h"

PyObject* dap_chain_ledger_cli_error_codes_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    dap_chain_ledger_cli_error_codes_init();
    Py_RETURN_NONE;
}

PyMethodDef* cellframe_ledger_cli_error_codes_get_methods(void) {
    static PyMethodDef methods[] = {
        {"ledger_cli_error_codes_init", (PyCFunction)dap_chain_ledger_cli_error_codes_init_py, METH_VARARGS,
         "Register ledger CLI error codes"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
