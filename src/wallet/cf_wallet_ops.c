#include "include/cf_wallet_internal.h"

PyObject *dap_chain_wallet_op_tx_request_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;
    PyObject *chain_source_obj = NULL;
    PyObject *chain_request_obj = NULL;
    PyObject *dest_obj = NULL;
    unsigned int wallet_key_idx = 0;
    unsigned long long value = 0;

    if (!PyArg_ParseTuple(args, "OIOKOO", &wallet_obj, &wallet_key_idx, &chain_source_obj, &value, &chain_request_obj, &dest_obj)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = (dap_chain_wallet_t *)cf_common_capsule_get_pointer(
        wallet_obj, "dap_chain_wallet_t", "First argument must be a dap_chain_wallet_t capsule"
    );
    if (!wallet) {
        return NULL;
    }

    dap_chain_t *chain_source = (dap_chain_t *)cf_common_capsule_get_pointer(
        chain_source_obj, "dap_chain_t", "Third argument must be a dap_chain_t capsule"
    );
    if (!chain_source) {
        return NULL;
    }

    dap_chain_t *chain_request = (dap_chain_t *)cf_common_capsule_get_pointer(
        chain_request_obj, "dap_chain_t", "Fifth argument must be a dap_chain_t capsule"
    );
    if (!chain_request) {
        return NULL;
    }

    dap_chain_addr_t dest_addr = {0};
    if (cf_common_parse_addr(dest_obj, &dest_addr) != 0) {
        return NULL;
    }

    dap_chain_wallet_op_tx_request(wallet, wallet_key_idx, chain_source, value, chain_request, dest_addr);
    Py_RETURN_NONE;
}

PyMethodDef *cellframe_wallet_ops_get_methods(void) {
    static PyMethodDef methods[] = {
        {"dap_chain_wallet_op_tx_request", dap_chain_wallet_op_tx_request_py, METH_VARARGS, "Request wallet tx"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
