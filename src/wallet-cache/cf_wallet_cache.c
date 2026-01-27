#include "../wallet/include/cf_wallet_internal.h"

#include "dap_chain_datum_tx.h"
#include "dap_chain_wallet_cache.h"
#include "dap_hash.h"
#include "dap_list.h"

#include <string.h>

static int cf_wallet_cache_parse_optional_str(PyObject *obj, const char **out, char **allocated) {
    *out = NULL;
    *allocated = NULL;
    if (!obj || obj == Py_None) {
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *val = PyUnicode_AsUTF8(obj);
        if (!val) {
            return -1;
        }
        *out = val;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        char *bytes = NULL;
        Py_ssize_t size = 0;
        if (PyBytes_AsStringAndSize(obj, &bytes, &size) != 0) {
            return -1;
        }
        char *buf = DAP_NEW_Z_SIZE(char, (size_t)size + 1);
        if (!buf) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate string buffer");
            return -1;
        }
        memcpy(buf, bytes, (size_t)size);
        buf[size] = '\0';
        *allocated = buf;
        *out = buf;
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "value must be str, bytes, or None");
    return -1;
}

static int cf_wallet_cache_parse_optional_hash(PyObject *obj, dap_hash_fast_t *out) {
    if (!out) {
        PyErr_SetString(PyExc_ValueError, "hash output pointer is required");
        return -1;
    }
    if (!obj || obj == Py_None) {
        memset(out, 0, sizeof(*out));
        return 0;
    }
    return cf_common_parse_hash_fast(obj, out);
}

static PyObject *cf_wallet_cache_tx_find_outs_common(PyObject *net_obj, PyObject *token_obj,
                                                     PyObject *addr_obj, bool mempool_check) {
    dap_chain_net_t *net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!net) {
        return NULL;
    }

    const char *token_ticker = NULL;
    char *token_alloc = NULL;
    if (cf_wallet_cache_parse_optional_str(token_obj, &token_ticker, &token_alloc) != 0) {
        return NULL;
    }
    if (!token_ticker) {
        PyErr_SetString(PyExc_TypeError, "token_ticker is required");
        DAP_DELETE(token_alloc);
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_common_parse_addr(addr_obj, &addr) != 0) {
        DAP_DELETE(token_alloc);
        return NULL;
    }

    dap_list_t *outs_list = NULL;
    uint256_t value_transfer = {0};
    int rc = dap_chain_wallet_cache_tx_find_outs_mempool_check(
        net, token_ticker, &addr, &outs_list, &value_transfer, mempool_check
    );

    DAP_DELETE(token_alloc);

    PyObject *list_obj = outs_list ? PyCapsule_New(outs_list, "dap_list_t", NULL) : Py_None;
    if (!outs_list) {
        Py_INCREF(Py_None);
    }

    PyObject *value_obj = PyBytes_FromStringAndSize((const char *)&value_transfer, sizeof(uint256_t));
    if (!value_obj) {
        Py_DECREF(list_obj);
        return NULL;
    }

    PyObject *tuple = PyTuple_New(3);
    if (!tuple) {
        Py_DECREF(list_obj);
        Py_DECREF(value_obj);
        return NULL;
    }

    PyTuple_SetItem(tuple, 0, PyLong_FromLong(rc));
    PyTuple_SetItem(tuple, 1, list_obj);
    PyTuple_SetItem(tuple, 2, value_obj);
    return tuple;
}

static PyObject *cf_wallet_cache_tx_find_outs_with_val_common(PyObject *net_obj, PyObject *token_obj,
                                                              PyObject *addr_obj, PyObject *value_obj,
                                                              bool mempool_check) {
    dap_chain_net_t *net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!net) {
        return NULL;
    }

    const char *token_ticker = NULL;
    char *token_alloc = NULL;
    if (cf_wallet_cache_parse_optional_str(token_obj, &token_ticker, &token_alloc) != 0) {
        return NULL;
    }
    if (!token_ticker) {
        PyErr_SetString(PyExc_TypeError, "token_ticker is required");
        DAP_DELETE(token_alloc);
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_common_parse_addr(addr_obj, &addr) != 0) {
        DAP_DELETE(token_alloc);
        return NULL;
    }

    uint256_t value_needed = {0};
    if (cf_common_parse_uint256(value_obj, &value_needed) != 0) {
        DAP_DELETE(token_alloc);
        return NULL;
    }

    dap_list_t *outs_list = NULL;
    uint256_t value_transfer = {0};
    int rc = dap_chain_wallet_cache_tx_find_outs_with_val_mempool_check(
        net, token_ticker, &addr, &outs_list, value_needed, &value_transfer, mempool_check
    );

    DAP_DELETE(token_alloc);

    PyObject *list_obj = outs_list ? PyCapsule_New(outs_list, "dap_list_t", NULL) : Py_None;
    if (!outs_list) {
        Py_INCREF(Py_None);
    }

    PyObject *value_transfer_obj = PyBytes_FromStringAndSize((const char *)&value_transfer, sizeof(uint256_t));
    if (!value_transfer_obj) {
        Py_DECREF(list_obj);
        return NULL;
    }

    PyObject *tuple = PyTuple_New(3);
    if (!tuple) {
        Py_DECREF(list_obj);
        Py_DECREF(value_transfer_obj);
        return NULL;
    }

    PyTuple_SetItem(tuple, 0, PyLong_FromLong(rc));
    PyTuple_SetItem(tuple, 1, list_obj);
    PyTuple_SetItem(tuple, 2, value_transfer_obj);
    return tuple;
}

PyObject *dap_chain_wallet_cache_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    int rc = dap_chain_wallet_cache_init();
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_cache_deinit_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    int rc = dap_chain_wallet_cache_deinit();
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_cache_register_chain_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    PyObject *net_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &chain_obj, &net_obj)) {
        return NULL;
    }

    dap_chain_t *chain = (dap_chain_t *)cf_common_capsule_get_pointer(
        chain_obj, "dap_chain_t", "First argument must be a dap_chain_t capsule"
    );
    if (!chain) {
        return NULL;
    }

    dap_chain_net_t *net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        net_obj, "dap_chain_net_t", "Second argument must be a network capsule"
    );
    if (!net) {
        return NULL;
    }

    int rc = dap_chain_wallet_cache_register_chain(chain, net);
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_cache_tx_find_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_obj = NULL;
    PyObject *token_obj = Py_None;
    PyObject *hash_obj = Py_None;

    if (!PyArg_ParseTuple(args, "O|OO", &addr_obj, &token_obj, &hash_obj)) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_common_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    const char *token = NULL;
    char *token_alloc = NULL;
    if (cf_wallet_cache_parse_optional_str(token_obj, &token, &token_alloc) != 0) {
        return NULL;
    }

    dap_hash_fast_t tx_hash_curr = {0};
    if (cf_wallet_cache_parse_optional_hash(hash_obj, &tx_hash_curr) != 0) {
        DAP_DELETE(token_alloc);
        return NULL;
    }

    dap_chain_datum_tx_t *tx = NULL;
    int ret_code = 0;
    int rc = dap_chain_wallet_cache_tx_find(&addr, (char *)token, &tx, &tx_hash_curr, &ret_code);

    DAP_DELETE(token_alloc);

    PyObject *tx_obj = tx ? PyCapsule_New(tx, "dap_chain_datum_tx_t", NULL) : Py_None;
    if (!tx) {
        Py_INCREF(Py_None);
    }

    PyObject *hash_out_obj = dap_hash_fast_is_blank(&tx_hash_curr)
                                 ? Py_None
                                 : PyBytes_FromStringAndSize((const char *)&tx_hash_curr, sizeof(tx_hash_curr));
    if (!hash_out_obj) {
        Py_DECREF(tx_obj);
        return NULL;
    }
    if (hash_out_obj == Py_None) {
        Py_INCREF(Py_None);
    }

    PyObject *tuple = PyTuple_New(4);
    if (!tuple) {
        Py_DECREF(tx_obj);
        Py_DECREF(hash_out_obj);
        return NULL;
    }

    PyTuple_SetItem(tuple, 0, PyLong_FromLong(rc));
    PyTuple_SetItem(tuple, 1, tx_obj);
    PyTuple_SetItem(tuple, 2, hash_out_obj);
    PyTuple_SetItem(tuple, 3, PyLong_FromLong(ret_code));
    return tuple;
}

PyObject *dap_chain_wallet_cache_tx_find_in_history_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_obj = NULL;
    PyObject *hash_obj = Py_None;

    if (!PyArg_ParseTuple(args, "O|O", &addr_obj, &hash_obj)) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_common_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    dap_hash_fast_t tx_hash_curr = {0};
    if (cf_wallet_cache_parse_optional_hash(hash_obj, &tx_hash_curr) != 0) {
        return NULL;
    }

    char *token = NULL;
    int ret_code = 0;
    dap_chain_tx_tag_action_type_t action = DAP_CHAIN_TX_TAG_ACTION_UNKNOWN;
    dap_chain_srv_uid_t uid = {0};
    dap_chain_datum_tx_t *tx = NULL;

    int rc = dap_chain_wallet_cache_tx_find_in_history(
        &addr, &token, &ret_code, &action, &uid, &tx, &tx_hash_curr
    );

    PyObject *token_obj = token ? PyUnicode_FromString(token) : Py_None;
    if (!token_obj) {
        return NULL;
    }
    if (token_obj == Py_None) {
        Py_INCREF(Py_None);
    }

    PyObject *tx_obj = tx ? PyCapsule_New(tx, "dap_chain_datum_tx_t", NULL) : Py_None;
    if (!tx) {
        Py_INCREF(Py_None);
    }

    PyObject *hash_out_obj = dap_hash_fast_is_blank(&tx_hash_curr)
                                 ? Py_None
                                 : PyBytes_FromStringAndSize((const char *)&tx_hash_curr, sizeof(tx_hash_curr));
    if (!hash_out_obj) {
        Py_DECREF(token_obj);
        Py_DECREF(tx_obj);
        return NULL;
    }
    if (hash_out_obj == Py_None) {
        Py_INCREF(Py_None);
    }

    PyObject *tuple = PyTuple_New(7);
    if (!tuple) {
        Py_DECREF(token_obj);
        Py_DECREF(tx_obj);
        Py_DECREF(hash_out_obj);
        return NULL;
    }

    PyTuple_SetItem(tuple, 0, PyLong_FromLong(rc));
    PyTuple_SetItem(tuple, 1, token_obj);
    PyTuple_SetItem(tuple, 2, PyLong_FromLong(ret_code));
    PyTuple_SetItem(tuple, 3, PyLong_FromLong((int)action));
    PyTuple_SetItem(tuple, 4, PyLong_FromUnsignedLongLong(uid.uint64));
    PyTuple_SetItem(tuple, 5, tx_obj);
    PyTuple_SetItem(tuple, 6, hash_out_obj);
    return tuple;
}

PyObject *dap_chain_wallet_cache_tx_find_outs_mempool_check_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *net_obj = NULL;
    PyObject *token_obj = NULL;
    PyObject *addr_obj = NULL;
    int mempool_check = 1;

    if (!PyArg_ParseTuple(args, "OOO|p", &net_obj, &token_obj, &addr_obj, &mempool_check)) {
        return NULL;
    }

    return cf_wallet_cache_tx_find_outs_common(net_obj, token_obj, addr_obj, mempool_check != 0);
}

PyObject *dap_chain_wallet_cache_tx_find_outs_with_val_mempool_check_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *net_obj = NULL;
    PyObject *token_obj = NULL;
    PyObject *addr_obj = NULL;
    PyObject *value_obj = NULL;
    int mempool_check = 1;

    if (!PyArg_ParseTuple(args, "OOOO|p", &net_obj, &token_obj, &addr_obj, &value_obj, &mempool_check)) {
        return NULL;
    }

    return cf_wallet_cache_tx_find_outs_with_val_common(net_obj, token_obj, addr_obj, value_obj, mempool_check != 0);
}

PyObject *dap_chain_wallet_cache_tx_find_outs_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *net_obj = NULL;
    PyObject *token_obj = NULL;
    PyObject *addr_obj = NULL;

    if (!PyArg_ParseTuple(args, "OOO", &net_obj, &token_obj, &addr_obj)) {
        return NULL;
    }

    return cf_wallet_cache_tx_find_outs_common(net_obj, token_obj, addr_obj, true);
}

PyObject *dap_chain_wallet_cache_tx_find_outs_with_val_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *net_obj = NULL;
    PyObject *token_obj = NULL;
    PyObject *addr_obj = NULL;
    PyObject *value_obj = NULL;

    if (!PyArg_ParseTuple(args, "OOOO", &net_obj, &token_obj, &addr_obj, &value_obj)) {
        return NULL;
    }

    return cf_wallet_cache_tx_find_outs_with_val_common(net_obj, token_obj, addr_obj, value_obj, true);
}

PyObject *dap_chain_wallet_cache_iter_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &addr_obj)) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_common_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    dap_chain_wallet_cache_iter_t *iter = dap_chain_wallet_cache_iter_create(addr);
    if (!iter) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(iter, "dap_chain_wallet_cache_iter_t", NULL);
}

PyObject *dap_chain_wallet_cache_iter_delete_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *iter_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &iter_obj)) {
        return NULL;
    }

    dap_chain_wallet_cache_iter_t *iter = (dap_chain_wallet_cache_iter_t *)cf_common_capsule_get_pointer(
        iter_obj, "dap_chain_wallet_cache_iter_t", "First argument must be a dap_chain_wallet_cache_iter_t capsule"
    );
    if (!iter) {
        return NULL;
    }

    dap_chain_wallet_cache_iter_delete(iter);
    Py_RETURN_NONE;
}

PyObject *dap_chain_wallet_cache_iter_get_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *iter_obj = NULL;
    int getting_type = 0;

    if (!PyArg_ParseTuple(args, "Oi", &iter_obj, &getting_type)) {
        return NULL;
    }

    if (getting_type < DAP_CHAIN_WALLET_CACHE_GET_FIRST || getting_type > DAP_CHAIN_WALLET_CACHE_GET_PREVIOUS) {
        PyErr_SetString(PyExc_ValueError, "getting_type must be between 0 and 3");
        return NULL;
    }

    dap_chain_wallet_cache_iter_t *iter = (dap_chain_wallet_cache_iter_t *)cf_common_capsule_get_pointer(
        iter_obj, "dap_chain_wallet_cache_iter_t", "First argument must be a dap_chain_wallet_cache_iter_t capsule"
    );
    if (!iter) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = dap_chain_wallet_cache_iter_get(
        iter, (dap_chain_wallet_getting_type_t)getting_type
    );
    if (!tx) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(tx, "dap_chain_datum_tx_t", NULL);
}

PyMethodDef *cellframe_wallet_cache_get_methods(void) {
    static PyMethodDef methods[] = {
        {"dap_chain_wallet_cache_init", dap_chain_wallet_cache_init_py, METH_NOARGS, "Init wallet cache"},
        {"dap_chain_wallet_cache_deinit", dap_chain_wallet_cache_deinit_py, METH_NOARGS, "Deinit wallet cache"},
        {"dap_chain_wallet_cache_register_chain", dap_chain_wallet_cache_register_chain_py, METH_VARARGS,
         "Register wallet cache callbacks for chain"},
        {"dap_chain_wallet_cache_tx_find", dap_chain_wallet_cache_tx_find_py, METH_VARARGS,
         "Find wallet cache transaction"},
        {"dap_chain_wallet_cache_tx_find_in_history", dap_chain_wallet_cache_tx_find_in_history_py, METH_VARARGS,
         "Find wallet cache transaction in history"},
        {"dap_chain_wallet_cache_tx_find_outs", dap_chain_wallet_cache_tx_find_outs_py, METH_VARARGS,
         "Find unspent outputs in wallet cache"},
        {"dap_chain_wallet_cache_tx_find_outs_mempool_check", dap_chain_wallet_cache_tx_find_outs_mempool_check_py, METH_VARARGS,
         "Find unspent outputs in wallet cache with mempool check"},
        {"dap_chain_wallet_cache_tx_find_outs_with_val", dap_chain_wallet_cache_tx_find_outs_with_val_py, METH_VARARGS,
         "Find unspent outputs in wallet cache with value requirement"},
        {"dap_chain_wallet_cache_tx_find_outs_with_val_mempool_check", dap_chain_wallet_cache_tx_find_outs_with_val_mempool_check_py, METH_VARARGS,
         "Find unspent outputs in wallet cache with value requirement and mempool check"},
        {"dap_chain_wallet_cache_iter_create", dap_chain_wallet_cache_iter_create_py, METH_VARARGS,
         "Create wallet cache iterator"},
        {"dap_chain_wallet_cache_iter_delete", dap_chain_wallet_cache_iter_delete_py, METH_VARARGS,
         "Delete wallet cache iterator"},
        {"dap_chain_wallet_cache_iter_get", dap_chain_wallet_cache_iter_get_py, METH_VARARGS,
         "Get transaction from wallet cache iterator"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
