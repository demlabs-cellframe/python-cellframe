#include "cellframe.h"
#include "dap_chain_net_tx.h"
#include "dap_chain_wallet_tx.h"
#include "dap_chain_token_cli.h"
#include "dap_chain_token_cli_error_codes.h"
#include "dap_chain_datum_tx.h"
#include "dap_json.h"
#include "../../datum/cf_datum_internal.h"

#define LOG_TAG "python_cellframe_net_tx"

typedef struct cf_net_tx_list_ctx {
    PyObject *list;
    int error;
} cf_net_tx_list_ctx_t;

static void cf_net_tx_collect_cb(dap_chain_net_t *a_net, dap_chain_datum_tx_t *a_tx, dap_hash_fast_t *a_tx_hash, void *a_arg) {
    (void)a_net;
    (void)a_tx_hash;
    cf_net_tx_list_ctx_t *ctx = (cf_net_tx_list_ctx_t *)a_arg;
    if (!ctx || ctx->error) {
        return;
    }

    PyObject *tx_capsule = PyCapsule_New(a_tx, "dap_chain_datum_tx_t", NULL);
    if (!tx_capsule) {
        ctx->error = 1;
        return;
    }

    if (PyList_Append(ctx->list, tx_capsule) < 0) {
        Py_DECREF(tx_capsule);
        ctx->error = 1;
        return;
    }
    Py_DECREF(tx_capsule);
}

static int cf_net_tx_parse_json_input(PyObject *obj, dap_json_t **json_out, bool *created) {
    *json_out = NULL;
    *created = false;

    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "json value is required");
        return -1;
    }

    if (PyCapsule_CheckExact(obj)) {
        dap_json_t *json = (dap_json_t *)PyCapsule_GetPointer(obj, "dap_json_t");
        if (!json) {
            PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
            return -1;
        }
        *json_out = json;
        return 0;
    }

    if (PyUnicode_Check(obj)) {
        const char *json_str = PyUnicode_AsUTF8(obj);
        if (!json_str) {
            return -1;
        }
        dap_json_t *json = dap_json_parse_string(json_str);
        if (!json) {
            PyErr_SetString(PyExc_ValueError, "Failed to parse JSON string");
            return -1;
        }
        *json_out = json;
        *created = true;
        return 0;
    }

    if (PyBytes_Check(obj)) {
        const char *json_bytes = NULL;
        Py_ssize_t json_size = 0;
        if (PyBytes_AsStringAndSize(obj, (char **)&json_bytes, &json_size) < 0) {
            return -1;
        }
        char *buf = DAP_NEW_Z_SIZE(char, (size_t)json_size + 1);
        if (!buf) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate JSON buffer");
            return -1;
        }
        memcpy(buf, json_bytes, (size_t)json_size);
        dap_json_t *json = dap_json_parse_string(buf);
        DAP_DELETE(buf);
        if (!json) {
            PyErr_SetString(PyExc_ValueError, "Failed to parse JSON bytes");
            return -1;
        }
        *json_out = json;
        *created = true;
        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "json must be a string, bytes, or JSON capsule");
    return -1;
}

static PyObject *cf_net_tx_json_to_py(dap_json_t *json) {
    if (!json) {
        Py_RETURN_NONE;
    }

    char *json_str = dap_json_to_string(json);
    if (!json_str) {
        Py_RETURN_NONE;
    }

    PyObject *py_str = PyUnicode_FromString(json_str);
    DAP_DELETE(json_str);
    if (!py_str) {
        Py_RETURN_NONE;
    }
    return py_str;
}

// =============================================================================
// NET TX CORE
// =============================================================================

PyObject* dap_chain_net_tx_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_res = dap_chain_net_tx_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_tx_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_net_tx_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_get_tx_by_hash_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_hash_obj = NULL;
    int l_search_type = TX_SEARCH_TYPE_NET;

    if (!PyArg_ParseTuple(a_args, "OO|i", &l_net_obj, &l_hash_obj, &l_search_type)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *l_tx = dap_chain_net_get_tx_by_hash(
        l_net, &l_hash, (dap_chain_net_tx_search_type_t)l_search_type);

    if (!l_tx) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

PyObject* dap_chain_net_get_tx_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    int l_search_type = TX_SEARCH_TYPE_NET;

    if (!PyArg_ParseTuple(a_args, "O|i", &l_net_obj, &l_search_type)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    PyObject *l_list = PyList_New(0);
    if (!l_list) {
        return NULL;
    }

    cf_net_tx_list_ctx_t ctx = {
        .list = l_list,
        .error = 0
    };

    dap_chain_net_get_tx_all(l_net, (dap_chain_net_tx_search_type_t)l_search_type, cf_net_tx_collect_cb, &ctx);

    if (ctx.error) {
        Py_DECREF(l_list);
        return NULL;
    }

    return l_list;
}

PyObject* dap_chain_net_get_tx_cond_chain_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_hash_obj = NULL;
    unsigned long long l_srv_uid = 0;

    if (!PyArg_ParseTuple(a_args, "OOK", &l_net_obj, &l_hash_obj, &l_srv_uid)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_chain_srv_uid_t l_srv_uid_struct = {.uint64 = l_srv_uid};
    dap_list_t *l_list = dap_chain_net_get_tx_cond_chain(l_net, &l_hash, l_srv_uid_struct);

    PyObject *l_result = PyList_New(0);
    if (!l_result) {
        dap_list_free(l_list);
        return NULL;
    }

    for (dap_list_t *l_item = l_list; l_item; l_item = l_item->next) {
        dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t *)l_item->data;
        PyObject *l_tx_capsule = PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
        if (!l_tx_capsule) {
            Py_DECREF(l_result);
            dap_list_free(l_list);
            return NULL;
        }
        PyList_Append(l_result, l_tx_capsule);
        Py_DECREF(l_tx_capsule);
    }

    dap_list_free(l_list);
    return l_result;
}

PyObject* dap_chain_net_get_tx_cond_all_by_srv_uid_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    unsigned long long l_srv_uid = 0;
    unsigned long long l_time_from = 0;
    unsigned long long l_time_to = 0;
    int l_search_type = TX_SEARCH_TYPE_NET;

    if (!PyArg_ParseTuple(a_args, "OK|KKi", &l_net_obj, &l_srv_uid, &l_time_from, &l_time_to, &l_search_type)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_srv_uid_t l_srv_uid_struct = {.uint64 = l_srv_uid};
    dap_list_t *l_list = dap_chain_net_get_tx_cond_all_by_srv_uid(
        l_net, l_srv_uid_struct, (dap_time_t)l_time_from, (dap_time_t)l_time_to,
        (dap_chain_net_tx_search_type_t)l_search_type);

    PyObject *l_result = PyList_New(0);
    if (!l_result) {
        return NULL;
    }

    for (dap_list_t *l_item = l_list; l_item; l_item = l_item->next) {
        dap_chain_datum_tx_cond_list_item_t *l_cond_item = (dap_chain_datum_tx_cond_list_item_t *)l_item->data;
        if (!l_cond_item) {
            continue;
        }

        PyObject *l_hash_bytes = PyBytes_FromStringAndSize((const char *)&l_cond_item->hash, sizeof(dap_hash_fast_t));
        if (!l_hash_bytes) {
            Py_DECREF(l_result);
            return NULL;
        }

        PyObject *l_tx_capsule = PyCapsule_New(l_cond_item->tx, "dap_chain_datum_tx_t", NULL);
        if (!l_tx_capsule) {
            Py_DECREF(l_hash_bytes);
            Py_DECREF(l_result);
            return NULL;
        }

        PyObject *l_dict = PyDict_New();
        if (!l_dict) {
            Py_DECREF(l_hash_bytes);
            Py_DECREF(l_tx_capsule);
            Py_DECREF(l_result);
            return NULL;
        }

        PyDict_SetItemString(l_dict, "hash", l_hash_bytes);
        PyDict_SetItemString(l_dict, "tx", l_tx_capsule);

        Py_DECREF(l_hash_bytes);
        Py_DECREF(l_tx_capsule);

        PyList_Append(l_result, l_dict);
        Py_DECREF(l_dict);
        DAP_DELETE(l_cond_item);
    }

    dap_list_free(l_list);
    return l_result;
}

PyObject* dap_chain_net_get_tx_cond_all_for_addr_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_addr_obj = NULL;
    unsigned long long l_srv_uid = 0;

    if (!PyArg_ParseTuple(a_args, "OOK", &l_net_obj, &l_addr_obj, &l_srv_uid)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_addr_t l_addr = {};
    if (cf_datum_parse_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    dap_chain_srv_uid_t l_srv_uid_struct = {.uint64 = l_srv_uid};
    dap_list_t *l_list = dap_chain_net_get_tx_cond_all_for_addr(l_net, &l_addr, l_srv_uid_struct);

    PyObject *l_result = PyList_New(0);
    if (!l_result) {
        dap_list_free(l_list);
        return NULL;
    }

    for (dap_list_t *l_item = l_list; l_item; l_item = l_item->next) {
        dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t *)l_item->data;
        PyObject *l_tx_capsule = PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
        if (!l_tx_capsule) {
            Py_DECREF(l_result);
            dap_list_free(l_list);
            return NULL;
        }
        PyList_Append(l_result, l_tx_capsule);
        Py_DECREF(l_tx_capsule);
    }

    dap_list_free(l_list);
    return l_result;
}

PyObject* dap_chain_net_get_tx_cond_all_with_spends_by_srv_uid_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    unsigned long long l_srv_uid = 0;
    unsigned long long l_time_from = 0;
    unsigned long long l_time_to = 0;
    int l_search_type = TX_SEARCH_TYPE_NET;

    if (!PyArg_ParseTuple(a_args, "OK|KKi", &l_net_obj, &l_srv_uid, &l_time_from, &l_time_to, &l_search_type)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_srv_uid_t l_srv_uid_struct = {.uint64 = l_srv_uid};
    dap_chain_datum_tx_spends_items_t *l_items = dap_chain_net_get_tx_cond_all_with_spends_by_srv_uid(
        l_net, l_srv_uid_struct, (dap_time_t)l_time_from, (dap_time_t)l_time_to,
        (dap_chain_net_tx_search_type_t)l_search_type);

    if (!l_items) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_items, "dap_chain_datum_tx_spends_items_t", NULL);
}

PyObject* dap_chain_datum_tx_spends_item_free_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_items_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_items_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_items_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a spends item capsule");
        return NULL;
    }

    dap_chain_datum_tx_spends_item_t *l_items = (dap_chain_datum_tx_spends_item_t *)PyCapsule_GetPointer(
        l_items_obj, "dap_chain_datum_tx_spends_item_t");
    if (!l_items) {
        PyErr_SetString(PyExc_ValueError, "Invalid spends item capsule");
        return NULL;
    }

    dap_chain_datum_tx_spends_item_free(l_items);
    Py_RETURN_NONE;
}

PyObject* dap_chain_datum_tx_spends_items_free_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_items_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_items_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_items_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a spends items capsule");
        return NULL;
    }

    dap_chain_datum_tx_spends_items_t *l_items = (dap_chain_datum_tx_spends_items_t *)PyCapsule_GetPointer(
        l_items_obj, "dap_chain_datum_tx_spends_items_t");
    if (!l_items) {
        PyErr_SetString(PyExc_ValueError, "Invalid spends items capsule");
        return NULL;
    }

    dap_chain_datum_tx_spends_items_free(l_items);
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_tx_create_by_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_json_obj = NULL;
    PyObject *l_net_obj = Py_None;
    PyObject *l_err_json_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|OO", &l_json_obj, &l_net_obj, &l_err_json_obj)) {
        return NULL;
    }

    dap_json_t *l_json = NULL;
    bool l_json_created = false;
    if (cf_net_tx_parse_json_input(l_json_obj, &l_json, &l_json_created) != 0) {
        return NULL;
    }

    dap_chain_net_t *l_net = NULL;
    if (l_net_obj && l_net_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_net_obj)) {
            if (l_json_created) {
                dap_json_object_free(l_json);
            }
            PyErr_SetString(PyExc_TypeError, "Second argument must be a network capsule or None");
            return NULL;
        }
        l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
        if (!l_net) {
            if (l_json_created) {
                dap_json_object_free(l_json);
            }
            PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
            return NULL;
        }
    }

    dap_json_t *l_err_json = NULL;
    bool l_err_json_created = false;
    if (l_err_json_obj && l_err_json_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_err_json_obj)) {
            if (l_json_created) {
                dap_json_object_free(l_json);
            }
            PyErr_SetString(PyExc_TypeError, "Third argument must be a JSON capsule or None");
            return NULL;
        }
        l_err_json = (dap_json_t *)PyCapsule_GetPointer(l_err_json_obj, "dap_json_t");
        if (!l_err_json) {
            if (l_json_created) {
                dap_json_object_free(l_json);
            }
            PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
            return NULL;
        }
    } else {
        l_err_json = dap_json_array_new();
        if (!l_err_json) {
            if (l_json_created) {
                dap_json_object_free(l_json);
            }
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate JSON errors");
            return NULL;
        }
        l_err_json_created = true;
    }

    dap_chain_datum_tx_t *l_tx = NULL;
    size_t l_items_count = 0;
    size_t l_items_ready = 0;
    int l_rc = dap_chain_net_tx_create_by_json(l_json, l_net, l_err_json, &l_tx, &l_items_count, &l_items_ready);

    PyObject *l_err_json_str = cf_net_tx_json_to_py(l_err_json);
    if (l_err_json_created && l_err_json) {
        dap_json_object_free(l_err_json);
    }
    if (l_json_created && l_json) {
        dap_json_object_free(l_json);
    }

    PyObject *l_tx_capsule = l_tx ? PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL) : Py_None;
    if (!l_tx_capsule) {
        Py_DECREF(l_err_json_str);
        return NULL;
    }

    PyObject *l_tuple = PyTuple_New(5);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    if (l_tx) {
        PyTuple_SetItem(l_tuple, 1, l_tx_capsule);
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 1, Py_None);
    }
    PyTuple_SetItem(l_tuple, 2, PyLong_FromSize_t(l_items_count));
    PyTuple_SetItem(l_tuple, 3, PyLong_FromSize_t(l_items_ready));
    if (l_err_json_str) {
        PyTuple_SetItem(l_tuple, 4, l_err_json_str);
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 4, Py_None);
    }
    return l_tuple;
}

PyObject* dap_chain_tx_datum_from_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_json_obj = NULL;
    PyObject *l_net_obj = Py_None;
    PyObject *l_err_json_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|OO", &l_json_obj, &l_net_obj, &l_err_json_obj)) {
        return NULL;
    }

    dap_json_t *l_json = NULL;
    bool l_json_created = false;
    if (cf_net_tx_parse_json_input(l_json_obj, &l_json, &l_json_created) != 0) {
        return NULL;
    }

    dap_chain_net_t *l_net = NULL;
    if (l_net_obj && l_net_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_net_obj)) {
            if (l_json_created) {
                dap_json_object_free(l_json);
            }
            PyErr_SetString(PyExc_TypeError, "Second argument must be a network capsule or None");
            return NULL;
        }
        l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
        if (!l_net) {
            if (l_json_created) {
                dap_json_object_free(l_json);
            }
            PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
            return NULL;
        }
    }

    dap_json_t *l_err_json = NULL;
    bool l_err_json_created = false;
    if (l_err_json_obj && l_err_json_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_err_json_obj)) {
            if (l_json_created) {
                dap_json_object_free(l_json);
            }
            PyErr_SetString(PyExc_TypeError, "Third argument must be a JSON capsule or None");
            return NULL;
        }
        l_err_json = (dap_json_t *)PyCapsule_GetPointer(l_err_json_obj, "dap_json_t");
        if (!l_err_json) {
            if (l_json_created) {
                dap_json_object_free(l_json);
            }
            PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
            return NULL;
        }
    } else {
        l_err_json = dap_json_array_new();
        if (!l_err_json) {
            if (l_json_created) {
                dap_json_object_free(l_json);
            }
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate JSON errors");
            return NULL;
        }
        l_err_json_created = true;
    }

    dap_chain_datum_tx_t *l_tx = NULL;
    size_t l_items_count = 0;
    size_t l_items_ready = 0;
    int l_rc = dap_chain_tx_datum_from_json(l_json, l_net, l_err_json, &l_tx, &l_items_count, &l_items_ready);

    PyObject *l_err_json_str = cf_net_tx_json_to_py(l_err_json);
    if (l_err_json_created && l_err_json) {
        dap_json_object_free(l_err_json);
    }
    if (l_json_created && l_json) {
        dap_json_object_free(l_json);
    }

    PyObject *l_tx_capsule = l_tx ? PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL) : Py_None;
    if (!l_tx_capsule) {
        Py_DECREF(l_err_json_str);
        return NULL;
    }

    PyObject *l_tuple = PyTuple_New(5);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    if (l_tx) {
        PyTuple_SetItem(l_tuple, 1, l_tx_capsule);
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 1, Py_None);
    }
    PyTuple_SetItem(l_tuple, 2, PyLong_FromSize_t(l_items_count));
    PyTuple_SetItem(l_tuple, 3, PyLong_FromSize_t(l_items_ready));
    if (l_err_json_str) {
        PyTuple_SetItem(l_tuple, 4, l_err_json_str);
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 4, Py_None);
    }
    return l_tuple;
}

PyObject* dap_chain_net_tx_to_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_tx_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_tx_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *l_tx = NULL;
    if (PyCapsule_CheckExact(l_tx_obj)) {
        l_tx = (dap_chain_datum_tx_t *)PyCapsule_GetPointer(l_tx_obj, "dap_chain_datum_tx_t");
        if (!l_tx) {
            PyErr_SetString(PyExc_ValueError, "Invalid tx capsule");
            return NULL;
        }
    } else if (PyBytes_Check(l_tx_obj)) {
        Py_ssize_t l_tx_size = 0;
        const char *l_tx_bytes = NULL;
        if (PyBytes_AsStringAndSize(l_tx_obj, (char **)&l_tx_bytes, &l_tx_size) < 0) {
            return NULL;
        }
        if (l_tx_size < (Py_ssize_t)sizeof(dap_chain_datum_tx_t)) {
            PyErr_SetString(PyExc_ValueError, "tx bytes too small");
            return NULL;
        }
        l_tx = (dap_chain_datum_tx_t *)l_tx_bytes;
    } else {
        PyErr_SetString(PyExc_TypeError, "tx must be a capsule or bytes");
        return NULL;
    }

    dap_json_t *l_json = dap_json_object_new();
    if (!l_json) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate JSON object");
        return NULL;
    }

    int l_rc = dap_chain_net_tx_to_json(l_tx, l_json);
    PyObject *l_json_str = cf_net_tx_json_to_py(l_json);
    dap_json_object_free(l_json);

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    if (l_json_str) {
        PyTuple_SetItem(l_tuple, 1, l_json_str);
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 1, Py_None);
    }
    return l_tuple;
}

// =============================================================================
// WALLET TX
// =============================================================================

PyObject* dap_wallet_tx_builders_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_wallet_tx_builders_register();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_wallet_tx_builders_unregister_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_wallet_tx_builders_unregister();
    Py_RETURN_NONE;
}

PyObject* dap_wallet_tx_create_transfer_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_used_outs_obj = Py_None;
    PyObject *l_addr_obj = NULL;
    const char *l_token = NULL;
    PyObject *l_value_obj = NULL;
    PyObject *l_fee_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OOsOO", &l_used_outs_obj, &l_addr_obj, &l_token, &l_value_obj, &l_fee_obj)) {
        return NULL;
    }

    dap_list_t *l_used_outs = NULL;
    if (l_used_outs_obj && l_used_outs_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_used_outs_obj)) {
            PyErr_SetString(PyExc_TypeError, "First argument must be a dap_list_t capsule or None");
            return NULL;
        }
        l_used_outs = (dap_list_t *)PyCapsule_GetPointer(l_used_outs_obj, "dap_list_t");
        if (!l_used_outs) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_list_t capsule");
            return NULL;
        }
    }

    dap_chain_addr_t l_addr = {};
    if (cf_datum_parse_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    uint256_t l_value = {};
    if (cf_datum_parse_uint256(l_value_obj, &l_value) != 0) {
        return NULL;
    }

    uint256_t l_fee = {};
    if (cf_datum_parse_uint256(l_fee_obj, &l_fee) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *l_tx = dap_wallet_tx_create_transfer(
        l_used_outs, &l_addr, l_token, l_value, l_fee);
    if (!l_tx) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

PyObject* dap_wallet_tx_create_multi_transfer_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_used_outs_obj = Py_None;
    PyObject *l_addrs_obj = NULL;
    PyObject *l_values_obj = NULL;
    const char *l_token = NULL;
    PyObject *l_fee_obj = NULL;
    PyObject *l_time_unlock_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "OOOsO|O", &l_used_outs_obj, &l_addrs_obj, &l_values_obj, &l_token, &l_fee_obj, &l_time_unlock_obj)) {
        return NULL;
    }

    dap_list_t *l_used_outs = NULL;
    if (l_used_outs_obj && l_used_outs_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_used_outs_obj)) {
            PyErr_SetString(PyExc_TypeError, "First argument must be a dap_list_t capsule or None");
            return NULL;
        }
        l_used_outs = (dap_list_t *)PyCapsule_GetPointer(l_used_outs_obj, "dap_list_t");
        if (!l_used_outs) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_list_t capsule");
            return NULL;
        }
    }

    PyObject *l_addrs_seq = PySequence_Fast(l_addrs_obj, "Addresses must be a sequence");
    if (!l_addrs_seq) {
        return NULL;
    }

    PyObject *l_values_seq = PySequence_Fast(l_values_obj, "Values must be a sequence");
    if (!l_values_seq) {
        Py_DECREF(l_addrs_seq);
        return NULL;
    }

    Py_ssize_t l_count = PySequence_Fast_GET_SIZE(l_addrs_seq);
    if (l_count <= 0 || l_count != PySequence_Fast_GET_SIZE(l_values_seq)) {
        Py_DECREF(l_addrs_seq);
        Py_DECREF(l_values_seq);
        PyErr_SetString(PyExc_ValueError, "Addresses and values must have the same non-zero length");
        return NULL;
    }

    dap_chain_addr_t *l_addr_storage = DAP_NEW_Z_COUNT(dap_chain_addr_t, (size_t)l_count);
    uint256_t *l_values = DAP_NEW_Z_COUNT(uint256_t, (size_t)l_count);
    const dap_chain_addr_t **l_addr_ptrs = DAP_NEW_Z_COUNT(const dap_chain_addr_t *, (size_t)l_count);
    if (!l_addr_storage || !l_values || !l_addr_ptrs) {
        DAP_DEL_MULTY(l_addr_storage, l_values, l_addr_ptrs);
        Py_DECREF(l_addrs_seq);
        Py_DECREF(l_values_seq);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate transfer buffers");
        return NULL;
    }

    for (Py_ssize_t i = 0; i < l_count; i++) {
        PyObject *addr_obj = PySequence_Fast_GET_ITEM(l_addrs_seq, i);
        PyObject *value_obj = PySequence_Fast_GET_ITEM(l_values_seq, i);
        if (cf_datum_parse_addr(addr_obj, &l_addr_storage[i]) != 0 ||
            cf_datum_parse_uint256(value_obj, &l_values[i]) != 0) {
            DAP_DEL_MULTY(l_addr_storage, l_values, l_addr_ptrs);
            Py_DECREF(l_addrs_seq);
            Py_DECREF(l_values_seq);
            return NULL;
        }
        l_addr_ptrs[i] = &l_addr_storage[i];
    }

    dap_time_t *l_time_unlock = NULL;
    if (l_time_unlock_obj && l_time_unlock_obj != Py_None) {
        PyObject *l_time_seq = PySequence_Fast(l_time_unlock_obj, "time_unlock must be a sequence");
        if (!l_time_seq) {
            DAP_DEL_MULTY(l_addr_storage, l_values, l_addr_ptrs);
            Py_DECREF(l_addrs_seq);
            Py_DECREF(l_values_seq);
            return NULL;
        }
        if (PySequence_Fast_GET_SIZE(l_time_seq) != l_count) {
            Py_DECREF(l_time_seq);
            DAP_DEL_MULTY(l_addr_storage, l_values, l_addr_ptrs);
            Py_DECREF(l_addrs_seq);
            Py_DECREF(l_values_seq);
            PyErr_SetString(PyExc_ValueError, "time_unlock must match addresses length");
            return NULL;
        }
        l_time_unlock = DAP_NEW_Z_COUNT(dap_time_t, (size_t)l_count);
        if (!l_time_unlock) {
            Py_DECREF(l_time_seq);
            DAP_DEL_MULTY(l_addr_storage, l_values, l_addr_ptrs);
            Py_DECREF(l_addrs_seq);
            Py_DECREF(l_values_seq);
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate time_unlock array");
            return NULL;
        }
        for (Py_ssize_t i = 0; i < l_count; i++) {
            PyObject *time_obj = PySequence_Fast_GET_ITEM(l_time_seq, i);
            unsigned long long val = PyLong_AsUnsignedLongLong(time_obj);
            if (PyErr_Occurred()) {
                Py_DECREF(l_time_seq);
                DAP_DEL_MULTY(l_time_unlock, l_addr_storage, l_values, l_addr_ptrs);
                Py_DECREF(l_addrs_seq);
                Py_DECREF(l_values_seq);
                return NULL;
            }
            l_time_unlock[i] = (dap_time_t)val;
        }
        Py_DECREF(l_time_seq);
    }

    uint256_t l_fee = {};
    if (cf_datum_parse_uint256(l_fee_obj, &l_fee) != 0) {
        DAP_DEL_MULTY(l_time_unlock, l_addr_storage, l_values, l_addr_ptrs);
        Py_DECREF(l_addrs_seq);
        Py_DECREF(l_values_seq);
        return NULL;
    }

    dap_chain_datum_tx_t *l_tx = dap_wallet_tx_create_multi_transfer(
        l_used_outs, l_addr_ptrs, l_values, l_token, l_fee, (size_t)l_count, l_time_unlock);

    DAP_DEL_MULTY(l_time_unlock, l_addr_storage, l_values, l_addr_ptrs);
    Py_DECREF(l_addrs_seq);
    Py_DECREF(l_values_seq);

    if (!l_tx) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_tx, "dap_chain_datum_tx_t", NULL);
}

// =============================================================================
// TOKEN CLI
// =============================================================================

PyObject* dap_chain_token_cli_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_token_cli_init();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_token_cli_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_token_cli_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_token_cli_error_codes_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_token_cli_error_codes_init();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_token_cli_error_codes_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_token_cli_error_codes_deinit();
    Py_RETURN_NONE;
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

PyMethodDef* cellframe_net_tx_get_methods(void) {
    static PyMethodDef methods[] = {
        {"net_tx_init", (PyCFunction)dap_chain_net_tx_init_py, METH_VARARGS, "Initialize net tx module"},
        {"net_tx_deinit", (PyCFunction)dap_chain_net_tx_deinit_py, METH_VARARGS, "Deinitialize net tx module"},
        {"net_get_tx_by_hash", (PyCFunction)dap_chain_net_get_tx_by_hash_py, METH_VARARGS, "Get tx by hash"},
        {"net_get_tx_all", (PyCFunction)dap_chain_net_get_tx_all_py, METH_VARARGS, "Get all txs"},
        {"net_get_tx_cond_chain", (PyCFunction)dap_chain_net_get_tx_cond_chain_py, METH_VARARGS, "Get conditional tx chain"},
        {"net_get_tx_cond_all_by_srv_uid", (PyCFunction)dap_chain_net_get_tx_cond_all_by_srv_uid_py, METH_VARARGS,
         "Get conditional txs by service uid"},
        {"net_get_tx_cond_all_for_addr", (PyCFunction)dap_chain_net_get_tx_cond_all_for_addr_py, METH_VARARGS,
         "Get conditional txs for address"},
        {"net_get_tx_cond_all_with_spends_by_srv_uid", (PyCFunction)dap_chain_net_get_tx_cond_all_with_spends_by_srv_uid_py,
         METH_VARARGS, "Get conditional tx spends by service uid"},
        {"net_tx_create_by_json", (PyCFunction)dap_chain_net_tx_create_by_json_py, METH_VARARGS, "Create tx from JSON"},
        {"tx_datum_from_json", (PyCFunction)dap_chain_tx_datum_from_json_py, METH_VARARGS, "Create tx datum from JSON"},
        {"net_tx_to_json", (PyCFunction)dap_chain_net_tx_to_json_py, METH_VARARGS, "Convert tx to JSON"},
        {"datum_tx_spends_item_free", (PyCFunction)dap_chain_datum_tx_spends_item_free_py, METH_VARARGS,
         "Free tx spends item"},
        {"datum_tx_spends_items_free", (PyCFunction)dap_chain_datum_tx_spends_items_free_py, METH_VARARGS,
         "Free tx spends items"},
        {"wallet_tx_builders_register", (PyCFunction)dap_wallet_tx_builders_register_py, METH_VARARGS,
         "Register wallet tx builders"},
        {"wallet_tx_builders_unregister", (PyCFunction)dap_wallet_tx_builders_unregister_py, METH_VARARGS,
         "Unregister wallet tx builders"},
        {"wallet_tx_create_transfer", (PyCFunction)dap_wallet_tx_create_transfer_py, METH_VARARGS,
         "Create wallet transfer tx"},
        {"wallet_tx_create_multi_transfer", (PyCFunction)dap_wallet_tx_create_multi_transfer_py, METH_VARARGS,
         "Create wallet multi-transfer tx"},
        {"token_cli_init", (PyCFunction)dap_chain_token_cli_init_py, METH_VARARGS, "Initialize token CLI"},
        {"token_cli_deinit", (PyCFunction)dap_chain_token_cli_deinit_py, METH_VARARGS, "Deinitialize token CLI"},
        {"token_cli_error_codes_init", (PyCFunction)dap_chain_token_cli_error_codes_init_py, METH_VARARGS,
         "Initialize token CLI error codes"},
        {"token_cli_error_codes_deinit", (PyCFunction)dap_chain_token_cli_error_codes_deinit_py, METH_VARARGS,
         "Deinitialize token CLI error codes"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}

int cellframe_net_tx_init(PyObject *module) {
    PyMethodDef *methods = cellframe_net_tx_get_methods();
    if (!methods) {
        log_it(L_ERROR, "Failed to get net tx methods");
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

    log_it(L_DEBUG, "Net tx module initialized successfully");
    return 0;
}
