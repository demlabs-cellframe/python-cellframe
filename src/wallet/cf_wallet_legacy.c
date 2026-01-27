#include "include/cf_wallet_internal.h"

#include "dap_chain_ledger.h"
#include "dap_math_ops.h"

static int cf_wallet_parse_net_id_legacy(PyObject *obj, dap_chain_net_id_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "net_id is required");
        return -1;
    }
    if (PyLong_Check(obj)) {
        out->uint64 = PyLong_AsUnsignedLongLong(obj);
        if (PyErr_Occurred()) {
            return -1;
        }
        return 0;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_chain_net_id_t *ptr = (dap_chain_net_id_t *)PyCapsule_GetPointer(obj, "dap_chain_net_id_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_chain_net_id_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "net_id must be int or dap_chain_net_id_t capsule");
    return -1;
}

PyObject *dap_chain_wallet_get_balance_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;
    PyObject *net_id_obj = NULL;
    const char *token_ticker = NULL;

    if (!PyArg_ParseTuple(args, "OOs", &wallet_obj, &net_id_obj, &token_ticker)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = (dap_chain_wallet_t *)cf_common_capsule_get_pointer(
        wallet_obj, "dap_chain_wallet_t", "First argument must be a dap_chain_wallet_t capsule"
    );
    if (!wallet) {
        return NULL;
    }

    dap_chain_net_t *net = NULL;
    if (PyUnicode_Check(net_id_obj)) {
        const char *net_name = PyUnicode_AsUTF8(net_id_obj);
        if (!net_name) {
            return NULL;
        }
        net = dap_chain_net_by_name(net_name);
    } else {
        dap_chain_net_id_t net_id = {0};
        if (cf_wallet_parse_net_id_legacy(net_id_obj, &net_id) != 0) {
            return NULL;
        }
        net = dap_chain_net_by_id(net_id);
    }

    if (!net || !net->pub.ledger) {
        PyErr_SetString(CellframeWalletError, "Failed to resolve ledger for network");
        return NULL;
    }

    dap_chain_addr_t *addr = dap_chain_wallet_get_addr(wallet, net->pub.id);
    if (!addr) {
        PyErr_SetString(CellframeWalletError, "Failed to get wallet address");
        return NULL;
    }

    uint256_t balance = dap_ledger_calc_balance(net->pub.ledger, addr, token_ticker);
    const char *balance_coins = NULL;
    const char *balance_datoshi = dap_uint256_to_char(balance, &balance_coins);
    DAP_DELETE(addr);

    if (!balance_datoshi) {
        PyErr_SetString(CellframeWalletError, "Failed to convert balance");
        return NULL;
    }

    return PyUnicode_FromString(balance_coins ? balance_coins : balance_datoshi);
}

PyMethodDef *cellframe_wallet_legacy_get_methods(void) {
    static PyMethodDef methods[] = {
        {"dap_chain_wallet_get_balance", dap_chain_wallet_get_balance_py, METH_VARARGS, "Get wallet balance (legacy)"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
