#include "../include/cf_ledger_internal.h"
PyMethodDef* cellframe_ledger_cli_get_methods(void) {
    static PyMethodDef methods[] = {
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
