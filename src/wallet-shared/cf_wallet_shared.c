#include "../wallet/include/cf_wallet_internal.h"

#include "dap_chain_datum_tx.h"
#include "dap_chain_wallet_shared.h"
#include "dap_json.h"
#include "dap_list.h"

#include <string.h>

static int cf_wallet_shared_build_argv(PyObject *argv_list, char ***argv_out, int *argc_out) {
    if (!PyList_Check(argv_list) && !PyTuple_Check(argv_list)) {
        PyErr_SetString(PyExc_TypeError, "argv must be a list or tuple");
        return -1;
    }

    Py_ssize_t argc = PySequence_Size(argv_list);
    if (argc < 0) {
        return -1;
    }

    char **argv = DAP_NEW_Z_COUNT(char *, (size_t)argc + 1);
    if (!argv) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate argv");
        return -1;
    }

    for (Py_ssize_t i = 0; i < argc; i++) {
        PyObject *item = PySequence_GetItem(argv_list, i);
        if (!item) {
            DAP_DELETE(argv);
            return -1;
        }
        if (PyUnicode_Check(item)) {
            argv[i] = (char *)PyUnicode_AsUTF8(item);
        } else if (PyBytes_Check(item)) {
            argv[i] = PyBytes_AsString(item);
        } else {
            Py_DECREF(item);
            PyErr_SetString(PyExc_TypeError, "argv items must be str or bytes");
            DAP_DELETE(argv);
            return -1;
        }
        Py_DECREF(item);
        if (!argv[i]) {
            PyErr_SetString(PyExc_ValueError, "Failed to parse argv item");
            DAP_DELETE(argv);
            return -1;
        }
    }

    argv[argc] = NULL;
    *argv_out = argv;
    *argc_out = (int)argc;
    return 0;
}

static void cf_wallet_shared_free_argv(char **argv) {
    DAP_DELETE(argv);
}

static int cf_wallet_shared_parse_json_reply(PyObject *json_reply_obj, dap_json_t **json_out, bool *created) {
    *json_out = NULL;
    *created = false;

    if (json_reply_obj && json_reply_obj != Py_None) {
        if (!PyCapsule_CheckExact(json_reply_obj)) {
            PyErr_SetString(PyExc_TypeError, "json_reply must be a JSON capsule or None");
            return -1;
        }
        *json_out = (dap_json_t *)PyCapsule_GetPointer(json_reply_obj, "dap_json_t");
        if (!*json_out) {
            PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
            return -1;
        }
        return 0;
    }

    *json_out = dap_json_array_new();
    if (!*json_out) {
        PyErr_SetString(PyExc_MemoryError, "Failed to create JSON reply");
        return -1;
    }
    *created = true;
    return 0;
}

static PyObject *cf_wallet_shared_json_to_str(dap_json_t *json) {
    if (!json) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    char *json_str = dap_json_to_string(json);
    if (!json_str) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    PyObject *result = PyUnicode_FromString(json_str);
    DAP_DELETE(json_str);
    return result ? result : NULL;
}

static int cf_wallet_shared_parse_tsd_list(PyObject *tsd_obj, dap_list_t **tsd_list_out, bool *created) {
    *tsd_list_out = NULL;
    *created = false;

    if (!tsd_obj || tsd_obj == Py_None) {
        return 0;
    }

    if (PyCapsule_CheckExact(tsd_obj)) {
        *tsd_list_out = (dap_list_t *)PyCapsule_GetPointer(tsd_obj, "dap_list_t");
        if (!*tsd_list_out) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_list_t capsule");
            return -1;
        }
        return 0;
    }

    if (!PyList_Check(tsd_obj) && !PyTuple_Check(tsd_obj)) {
        PyErr_SetString(PyExc_TypeError, "tsd_items must be a dap_list_t capsule, list, tuple, or None");
        return -1;
    }

    Py_ssize_t count = PySequence_Size(tsd_obj);
    if (count < 0) {
        return -1;
    }
    if (count == 0) {
        return 0;
    }

    dap_list_t *list = NULL;
    for (Py_ssize_t i = 0; i < count; i++) {
        PyObject *item = PySequence_GetItem(tsd_obj, i);
        if (!item) {
            dap_list_free(list);
            return -1;
        }
        dap_chain_tx_tsd_t *tsd = (dap_chain_tx_tsd_t *)cf_common_capsule_get_pointer(
            item, "dap_chain_tx_tsd_t", "tsd_items must contain dap_chain_tx_tsd_t capsules"
        );
        Py_DECREF(item);
        if (!tsd) {
            dap_list_free(list);
            return -1;
        }
        dap_list_t *next = dap_list_append(list, tsd);
        if (!next) {
            PyErr_SetString(PyExc_MemoryError, "Failed to append tsd item");
            dap_list_free(list);
            return -1;
        }
        list = next;
    }

    *tsd_list_out = list;
    *created = true;
    return 0;
}

PyObject *dap_chain_wallet_shared_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    int rc = dap_chain_wallet_shared_init();
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_shared_notify_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    int rc = dap_chain_wallet_shared_notify_init();
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_shared_register_chain_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &chain_obj)) {
        return NULL;
    }

    dap_chain_t *chain = (dap_chain_t *)cf_common_capsule_get_pointer(
        chain_obj, "dap_chain_t", "First argument must be a dap_chain_t capsule"
    );
    if (!chain) {
        return NULL;
    }

    int rc = dap_chain_wallet_shared_register_chain(chain);
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_shared_cli_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *argv_list = NULL;
    PyObject *json_reply_obj = Py_None;
    int version = 1;

    if (!PyArg_ParseTuple(args, "O|Oi", &argv_list, &json_reply_obj, &version)) {
        return NULL;
    }

    char **argv = NULL;
    int argc = 0;
    if (cf_wallet_shared_build_argv(argv_list, &argv, &argc) != 0) {
        return NULL;
    }

    dap_json_t *json_reply = NULL;
    bool created = false;
    if (cf_wallet_shared_parse_json_reply(json_reply_obj, &json_reply, &created) != 0) {
        cf_wallet_shared_free_argv(argv);
        return NULL;
    }

    int rc = dap_chain_wallet_shared_cli(argc, argv, json_reply, version);
    cf_wallet_shared_free_argv(argv);

    PyObject *json_str_obj = cf_wallet_shared_json_to_str(json_reply);
    if (!json_str_obj) {
        if (created && json_reply) {
            dap_json_object_free(json_reply);
        }
        return NULL;
    }

    if (created && json_reply) {
        dap_json_object_free(json_reply);
    }

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        Py_DECREF(json_str_obj);
        return NULL;
    }
    PyTuple_SetItem(tuple, 0, PyLong_FromLong(rc));
    PyTuple_SetItem(tuple, 1, json_str_obj);
    return tuple;
}

PyObject *dap_chain_shared_tx_find_in_mempool_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    PyObject *hash_obj = NULL;
    PyObject *json_reply_obj = Py_None;

    if (!PyArg_ParseTuple(args, "OO|O", &chain_obj, &hash_obj, &json_reply_obj)) {
        return NULL;
    }

    dap_chain_t *chain = (dap_chain_t *)cf_common_capsule_get_pointer(
        chain_obj, "dap_chain_t", "First argument must be a dap_chain_t capsule"
    );
    if (!chain) {
        return NULL;
    }

    dap_hash_fast_t tx_hash = {0};
    if (cf_common_parse_hash_fast(hash_obj, &tx_hash) != 0) {
        return NULL;
    }

    dap_json_t *json_reply = NULL;
    bool created = false;
    if (cf_wallet_shared_parse_json_reply(json_reply_obj, &json_reply, &created) != 0) {
        return NULL;
    }

    int count = dap_chain_shared_tx_find_in_mempool(chain, &tx_hash, json_reply);

    PyObject *json_str_obj = cf_wallet_shared_json_to_str(json_reply);
    if (!json_str_obj) {
        if (created && json_reply) {
            dap_json_object_free(json_reply);
        }
        return NULL;
    }

    if (created && json_reply) {
        dap_json_object_free(json_reply);
    }

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        Py_DECREF(json_str_obj);
        return NULL;
    }
    PyTuple_SetItem(tuple, 0, PyLong_FromLong(count));
    PyTuple_SetItem(tuple, 1, json_str_obj);
    return tuple;
}

PyObject *dap_chain_wallet_shared_refilling_tx_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *net_obj = NULL;
    PyObject *enc_key_obj = NULL;
    PyObject *value_obj = NULL;
    PyObject *fee_obj = NULL;
    PyObject *tx_hash_obj = NULL;
    PyObject *tsd_obj = Py_None;
    PyObject *json_reply_obj = Py_None;

    if (!PyArg_ParseTuple(args, "OOOOO|OO", &net_obj, &enc_key_obj, &value_obj, &fee_obj,
                          &tx_hash_obj, &tsd_obj, &json_reply_obj)) {
        return NULL;
    }

    dap_chain_net_t *net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!net) {
        return NULL;
    }

    dap_enc_key_t *enc_key = (dap_enc_key_t *)cf_common_capsule_get_pointer(
        enc_key_obj, "dap_enc_key_t", "Second argument must be a dap_enc_key_t capsule"
    );
    if (!enc_key) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_common_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    uint256_t fee = {0};
    if (cf_common_parse_uint256(fee_obj, &fee) != 0) {
        return NULL;
    }

    dap_hash_fast_t tx_hash = {0};
    if (cf_common_parse_hash_fast(tx_hash_obj, &tx_hash) != 0) {
        return NULL;
    }

    dap_list_t *tsd_list = NULL;
    bool tsd_created = false;
    if (cf_wallet_shared_parse_tsd_list(tsd_obj, &tsd_list, &tsd_created) != 0) {
        return NULL;
    }

    dap_json_t *json_reply = NULL;
    bool json_created = false;
    if (cf_wallet_shared_parse_json_reply(json_reply_obj, &json_reply, &json_created) != 0) {
        if (tsd_created) {
            dap_list_free(tsd_list);
        }
        return NULL;
    }

    dap_chain_datum_tx_t *tx = dap_chain_wallet_shared_refilling_tx_create(
        json_reply, net, enc_key, value, fee, &tx_hash, tsd_list
    );

    if (tsd_created) {
        dap_list_free(tsd_list);
    }

    PyObject *json_str_obj = cf_wallet_shared_json_to_str(json_reply);
    if (!json_str_obj) {
        if (json_created && json_reply) {
            dap_json_object_free(json_reply);
        }
        return NULL;
    }

    if (json_created && json_reply) {
        dap_json_object_free(json_reply);
    }

    PyObject *tx_obj = tx ? PyCapsule_New(tx, "dap_chain_datum_tx_t", NULL) : Py_None;
    if (!tx) {
        Py_INCREF(Py_None);
    }

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        Py_DECREF(tx_obj);
        Py_DECREF(json_str_obj);
        return NULL;
    }

    PyTuple_SetItem(tuple, 0, tx_obj);
    PyTuple_SetItem(tuple, 1, json_str_obj);
    return tuple;
}

PyObject *dap_chain_wallet_shared_taking_tx_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *net_obj = NULL;
    PyObject *enc_key_obj = NULL;
    PyObject *addr_list_obj = NULL;
    PyObject *value_list_obj = NULL;
    PyObject *fee_obj = NULL;
    PyObject *tx_hash_obj = NULL;
    PyObject *tsd_obj = Py_None;
    PyObject *json_reply_obj = Py_None;

    if (!PyArg_ParseTuple(args, "OOOOOO|OO", &net_obj, &enc_key_obj, &addr_list_obj,
                          &value_list_obj, &fee_obj, &tx_hash_obj, &tsd_obj, &json_reply_obj)) {
        return NULL;
    }

    dap_chain_net_t *net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!net) {
        return NULL;
    }

    dap_enc_key_t *enc_key = (dap_enc_key_t *)cf_common_capsule_get_pointer(
        enc_key_obj, "dap_enc_key_t", "Second argument must be a dap_enc_key_t capsule"
    );
    if (!enc_key) {
        return NULL;
    }

    if (!PyList_Check(addr_list_obj) && !PyTuple_Check(addr_list_obj)) {
        PyErr_SetString(PyExc_TypeError, "addr_list must be a list or tuple");
        return NULL;
    }

    if (!PyList_Check(value_list_obj) && !PyTuple_Check(value_list_obj)) {
        PyErr_SetString(PyExc_TypeError, "value_list must be a list or tuple");
        return NULL;
    }

    Py_ssize_t addr_count = PySequence_Size(addr_list_obj);
    Py_ssize_t value_count = PySequence_Size(value_list_obj);
    if (addr_count < 0 || value_count < 0) {
        return NULL;
    }
    if (addr_count == 0) {
        PyErr_SetString(PyExc_ValueError, "addr_list must not be empty");
        return NULL;
    }
    if (value_count != addr_count) {
        PyErr_SetString(PyExc_ValueError, "addr_list and value_list must have the same length");
        return NULL;
    }

    dap_chain_addr_t *addrs = DAP_NEW_Z_SIZE(dap_chain_addr_t, sizeof(dap_chain_addr_t) * (size_t)addr_count);
    uint256_t *values = DAP_NEW_Z_SIZE(uint256_t, sizeof(uint256_t) * (size_t)addr_count);
    if (!addrs || !values) {
        DAP_DELETE(addrs);
        DAP_DELETE(values);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address/value arrays");
        return NULL;
    }

    for (Py_ssize_t i = 0; i < addr_count; i++) {
        PyObject *addr_item = PySequence_GetItem(addr_list_obj, i);
        if (!addr_item) {
            DAP_DELETE(addrs);
            DAP_DELETE(values);
            return NULL;
        }
        if (cf_common_parse_addr(addr_item, &addrs[i]) != 0) {
            Py_DECREF(addr_item);
            DAP_DELETE(addrs);
            DAP_DELETE(values);
            return NULL;
        }
        Py_DECREF(addr_item);

        PyObject *value_item = PySequence_GetItem(value_list_obj, i);
        if (!value_item) {
            DAP_DELETE(addrs);
            DAP_DELETE(values);
            return NULL;
        }
        if (cf_common_parse_uint256(value_item, &values[i]) != 0) {
            Py_DECREF(value_item);
            DAP_DELETE(addrs);
            DAP_DELETE(values);
            return NULL;
        }
        Py_DECREF(value_item);
    }

    uint256_t fee = {0};
    if (cf_common_parse_uint256(fee_obj, &fee) != 0) {
        DAP_DELETE(addrs);
        DAP_DELETE(values);
        return NULL;
    }

    dap_hash_fast_t tx_hash = {0};
    if (cf_common_parse_hash_fast(tx_hash_obj, &tx_hash) != 0) {
        DAP_DELETE(addrs);
        DAP_DELETE(values);
        return NULL;
    }

    dap_list_t *tsd_list = NULL;
    bool tsd_created = false;
    if (cf_wallet_shared_parse_tsd_list(tsd_obj, &tsd_list, &tsd_created) != 0) {
        DAP_DELETE(addrs);
        DAP_DELETE(values);
        return NULL;
    }

    dap_json_t *json_reply = NULL;
    bool json_created = false;
    if (cf_wallet_shared_parse_json_reply(json_reply_obj, &json_reply, &json_created) != 0) {
        if (tsd_created) {
            dap_list_free(tsd_list);
        }
        DAP_DELETE(addrs);
        DAP_DELETE(values);
        return NULL;
    }

    dap_chain_datum_tx_t *tx = dap_chain_wallet_shared_taking_tx_create(
        json_reply, net, enc_key, addrs, values, (uint32_t)addr_count, fee, &tx_hash, tsd_list
    );

    if (tsd_created) {
        dap_list_free(tsd_list);
    }
    DAP_DELETE(addrs);
    DAP_DELETE(values);

    PyObject *json_str_obj = cf_wallet_shared_json_to_str(json_reply);
    if (!json_str_obj) {
        if (json_created && json_reply) {
            dap_json_object_free(json_reply);
        }
        return NULL;
    }

    if (json_created && json_reply) {
        dap_json_object_free(json_reply);
    }

    PyObject *tx_obj = tx ? PyCapsule_New(tx, "dap_chain_datum_tx_t", NULL) : Py_None;
    if (!tx) {
        Py_INCREF(Py_None);
    }

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        Py_DECREF(tx_obj);
        Py_DECREF(json_str_obj);
        return NULL;
    }

    PyTuple_SetItem(tuple, 0, tx_obj);
    PyTuple_SetItem(tuple, 1, json_str_obj);
    return tuple;
}

PyObject *dap_chain_wallet_shared_taking_tx_sign_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *net_obj = NULL;
    PyObject *enc_key_obj = NULL;
    PyObject *tx_obj = NULL;
    PyObject *json_reply_obj = Py_None;

    if (!PyArg_ParseTuple(args, "OOO|O", &net_obj, &enc_key_obj, &tx_obj, &json_reply_obj)) {
        return NULL;
    }

    dap_chain_net_t *net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!net) {
        return NULL;
    }

    dap_enc_key_t *enc_key = (dap_enc_key_t *)cf_common_capsule_get_pointer(
        enc_key_obj, "dap_enc_key_t", "Second argument must be a dap_enc_key_t capsule"
    );
    if (!enc_key) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_in = (dap_chain_datum_tx_t *)cf_common_capsule_get_pointer(
        tx_obj, "dap_chain_datum_tx_t", "Third argument must be a dap_chain_datum_tx_t capsule"
    );
    if (!tx_in) {
        return NULL;
    }

    dap_json_t *json_reply = NULL;
    bool json_created = false;
    if (cf_wallet_shared_parse_json_reply(json_reply_obj, &json_reply, &json_created) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = dap_chain_wallet_shared_taking_tx_sign(json_reply, net, enc_key, tx_in);

    PyObject *json_str_obj = cf_wallet_shared_json_to_str(json_reply);
    if (!json_str_obj) {
        if (json_created && json_reply) {
            dap_json_object_free(json_reply);
        }
        return NULL;
    }

    if (json_created && json_reply) {
        dap_json_object_free(json_reply);
    }

    PyObject *tx_out_obj = tx ? PyCapsule_New(tx, "dap_chain_datum_tx_t", NULL) : Py_None;
    if (!tx) {
        Py_INCREF(Py_None);
    }

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        Py_DECREF(tx_out_obj);
        Py_DECREF(json_str_obj);
        return NULL;
    }

    PyTuple_SetItem(tuple, 0, tx_out_obj);
    PyTuple_SetItem(tuple, 1, json_str_obj);
    return tuple;
}

PyObject *dap_chain_wallet_shared_hold_tx_add_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_obj = NULL;
    const char *net_name = NULL;

    if (!PyArg_ParseTuple(args, "Os", &tx_obj, &net_name)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_common_capsule_get_pointer(
        tx_obj, "dap_chain_datum_tx_t", "First argument must be a dap_chain_datum_tx_t capsule"
    );
    if (!tx) {
        return NULL;
    }

    int rc = dap_chain_wallet_shared_hold_tx_add(tx, net_name);
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_shared_get_tx_hashes_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *pkey_hash_obj = NULL;
    const char *net_name = NULL;

    if (!PyArg_ParseTuple(args, "Os", &pkey_hash_obj, &net_name)) {
        return NULL;
    }

    dap_hash_fast_t pkey_hash = {0};
    if (cf_common_parse_hash_fast(pkey_hash_obj, &pkey_hash) != 0) {
        return NULL;
    }

    dap_json_t *json_result = dap_chain_wallet_shared_get_tx_hashes_json(&pkey_hash, net_name);
    if (!json_result) {
        Py_RETURN_NONE;
    }

    char *json_str = dap_json_to_string(json_result);
    dap_json_object_free(json_result);

    if (!json_str) {
        Py_RETURN_NONE;
    }

    PyObject *result = PyUnicode_FromString(json_str);
    DAP_DELETE(json_str);
    return result;
}

PyMethodDef *cellframe_wallet_shared_get_methods(void) {
    static PyMethodDef methods[] = {
        {"dap_chain_wallet_shared_init", dap_chain_wallet_shared_init_py, METH_NOARGS, "Init wallet shared"},
        {"dap_chain_wallet_shared_notify_init", dap_chain_wallet_shared_notify_init_py, METH_NOARGS,
         "Init wallet shared notify"},
        {"dap_chain_wallet_shared_register_chain", dap_chain_wallet_shared_register_chain_py, METH_VARARGS,
         "Register wallet shared notify callback for chain"},
        {"dap_chain_wallet_shared_cli", dap_chain_wallet_shared_cli_py, METH_VARARGS, "Wallet shared CLI"},
        {"dap_chain_shared_tx_find_in_mempool", dap_chain_shared_tx_find_in_mempool_py, METH_VARARGS,
         "Find shared wallet tx in mempool"},
        {"dap_chain_wallet_shared_refilling_tx_create", dap_chain_wallet_shared_refilling_tx_create_py, METH_VARARGS,
         "Create shared wallet refilling transaction"},
        {"dap_chain_wallet_shared_taking_tx_create", dap_chain_wallet_shared_taking_tx_create_py, METH_VARARGS,
         "Create shared wallet taking transaction"},
        {"dap_chain_wallet_shared_taking_tx_sign", dap_chain_wallet_shared_taking_tx_sign_py, METH_VARARGS,
         "Sign shared wallet taking transaction"},
        {"dap_chain_wallet_shared_hold_tx_add", dap_chain_wallet_shared_hold_tx_add_py, METH_VARARGS,
         "Add shared wallet hold transaction"},
        {"dap_chain_wallet_shared_get_tx_hashes_json", dap_chain_wallet_shared_get_tx_hashes_json_py, METH_VARARGS,
         "Get shared wallet tx hashes JSON"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
