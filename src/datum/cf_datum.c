#include "cellframe.h"
#include "dap_chain_datum.h"
#include "dap_chain_datum_anchor.h"
#include "dap_chain_datum_converter.h"
#include "dap_chain_datum_decree.h"
#include "dap_chain_datum_tx.h"
#include "dap_chain_datum_tx_items.h"
#include "dap_chain_datum_tx_receipt.h"
#include "dap_chain_datum_tx_voting.h"
#include "dap_chain_datum_token.h"
#include "dap_chain_decree_registry.h"
#include "dap_json.h"
#include "dap_string.h"
#include "dap_hash.h"
#include "../common/cf_callbacks_registry.h"

#include <string.h>

#define LOG_TAG "python_cellframe_datum"

typedef struct {
    PyObject *callback;
} python_datum_dump_ctx_t;

static python_datum_dump_ctx_t *s_datum_dump_decree_ctx = NULL;
static python_datum_dump_ctx_t *s_datum_dump_anchor_ctx = NULL;

typedef struct {
    uint16_t decree_type;
    uint16_t decree_subtype;
    PyObject *callback;
    char *handler_name;
} python_decree_handler_ctx_t;

static python_decree_handler_ctx_t **s_decree_handlers = NULL;
static size_t s_decree_handlers_count = 0;
static pthread_mutex_t s_decree_handlers_mutex = PTHREAD_MUTEX_INITIALIZER;

static void cf_datum_clear_ctx(python_datum_dump_ctx_t **ctx_ptr) {
    if (!ctx_ptr || !*ctx_ptr) {
        return;
    }
    cf_callbacks_registry_remove(*ctx_ptr);
    DAP_DELETE(*ctx_ptr);
    *ctx_ptr = NULL;
}

static python_decree_handler_ctx_t *cf_datum_decree_handler_find(uint16_t type, uint16_t subtype, size_t *index_out) {
    for (size_t i = 0; i < s_decree_handlers_count; i++) {
        python_decree_handler_ctx_t *ctx = s_decree_handlers[i];
        if (ctx && ctx->decree_type == type && ctx->decree_subtype == subtype) {
            if (index_out) {
                *index_out = i;
            }
            return ctx;
        }
    }
    return NULL;
}

static PyObject *cf_datum_decree_handler_get_callback(uint16_t type, uint16_t subtype) {
    PyObject *callback = NULL;
    pthread_mutex_lock(&s_decree_handlers_mutex);
    python_decree_handler_ctx_t *ctx = cf_datum_decree_handler_find(type, subtype, NULL);
    if (ctx && ctx->callback) {
        callback = ctx->callback;
        Py_INCREF(callback);
    }
    pthread_mutex_unlock(&s_decree_handlers_mutex);
    return callback;
}

static void cf_datum_decree_handler_free(python_decree_handler_ctx_t *ctx) {
    if (!ctx) {
        return;
    }
    cf_callbacks_registry_remove(ctx);
    DAP_DELETE(ctx->handler_name);
    DAP_DELETE(ctx);
}

static void cf_datum_decree_handlers_clear(void) {
    pthread_mutex_lock(&s_decree_handlers_mutex);
    for (size_t i = 0; i < s_decree_handlers_count; i++) {
        cf_datum_decree_handler_free(s_decree_handlers[i]);
    }
    DAP_DELETE(s_decree_handlers);
    s_decree_handlers = NULL;
    s_decree_handlers_count = 0;
    pthread_mutex_unlock(&s_decree_handlers_mutex);
}

static void *cf_datum_get_capsule(PyObject *obj, const char *name, const char *error_msg) {
    if (!PyCapsule_CheckExact(obj)) {
        PyErr_SetString(PyExc_TypeError, error_msg);
        return NULL;
    }
    void *ptr = PyCapsule_GetPointer(obj, name);
    if (!ptr) {
        PyErr_SetString(PyExc_ValueError, error_msg);
        return NULL;
    }
    return ptr;
}

static const char *cf_datum_get_cstring(PyObject *obj, const char *error_msg) {
    if (PyUnicode_Check(obj)) {
        return PyUnicode_AsUTF8(obj);
    }
    if (PyBytes_Check(obj)) {
        return PyBytes_AsString(obj);
    }
    PyErr_SetString(PyExc_TypeError, error_msg);
    return NULL;
}

static int cf_datum_parse_hash_fast(PyObject *obj, dap_hash_fast_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "hash value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        const char *name = PyCapsule_GetName(obj);
        if (!name || (strcmp(name, "dap_hash_fast_t") != 0 && strcmp(name, "dap_chain_hash_fast_t") != 0)) {
            PyErr_SetString(PyExc_ValueError, "Invalid hash capsule name");
            return -1;
        }
        dap_hash_fast_t *ptr = (dap_hash_fast_t *)PyCapsule_GetPointer(obj, name);
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid hash capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_hash_fast_t)) {
            PyErr_Format(PyExc_ValueError, "hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_hash_fast_t));
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *hash_str = PyUnicode_AsUTF8(obj);
        if (!hash_str) {
            return -1;
        }
        if (dap_chain_hash_fast_from_str(hash_str, out) != 0) {
            PyErr_SetString(PyExc_ValueError, "Invalid hash string");
            return -1;
        }
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "hash must be bytes, str, or dap_hash_fast_t capsule");
    return -1;
}

static int cf_datum_parse_uint256(PyObject *obj, uint256_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "uint256 value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        uint256_t *ptr = (uint256_t *)PyCapsule_GetPointer(obj, "uint256_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid uint256_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(uint256_t)) {
            PyErr_Format(PyExc_ValueError, "uint256 value must be exactly %zu bytes", sizeof(uint256_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(uint256_t));
        return 0;
    }
    PyObject *str_obj = PyObject_Str(obj);
    if (!str_obj) {
        return -1;
    }
    const char *str_val = PyUnicode_AsUTF8(str_obj);
    if (!str_val) {
        Py_DECREF(str_obj);
        return -1;
    }
    *out = dap_chain_balance_scan(str_val);
    Py_DECREF(str_obj);
    return 0;
}

static int cf_datum_parse_addr(PyObject *obj, dap_chain_addr_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "address is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_chain_addr_t *ptr = (dap_chain_addr_t *)PyCapsule_GetPointer(obj, "dap_chain_addr_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_chain_addr_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_chain_addr_t)) {
            PyErr_Format(PyExc_ValueError, "address must be exactly %zu bytes", sizeof(dap_chain_addr_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_chain_addr_t));
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *addr_str = PyUnicode_AsUTF8(obj);
        if (!addr_str) {
            return -1;
        }
        dap_chain_addr_t *addr = dap_chain_addr_from_str(addr_str);
        if (!addr) {
            PyErr_SetString(PyExc_ValueError, "Invalid address string");
            return -1;
        }
        *out = *addr;
        DAP_DELETE(addr);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "address must be bytes, str, or dap_chain_addr_t capsule");
    return -1;
}

static int cf_datum_parse_node_addr(PyObject *obj, dap_chain_node_addr_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "node address value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_chain_node_addr_t *ptr = (dap_chain_node_addr_t *)PyCapsule_GetPointer(obj, "dap_chain_node_addr_t");
        if (!ptr) {
            ptr = (dap_chain_node_addr_t *)PyCapsule_GetPointer(obj, "dap_stream_node_addr_t");
        }
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid node address capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyLong_Check(obj)) {
        unsigned long long val = PyLong_AsUnsignedLongLong(obj);
        if (PyErr_Occurred()) {
            return -1;
        }
        out->uint64 = val;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_chain_node_addr_t)) {
            PyErr_Format(PyExc_ValueError, "node address must be exactly %zu bytes", sizeof(dap_chain_node_addr_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_chain_node_addr_t));
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *addr_str = PyUnicode_AsUTF8(obj);
        if (!addr_str) {
            return -1;
        }
        if (dap_chain_node_addr_from_str(out, addr_str) != 0) {
            PyErr_SetString(PyExc_ValueError, "Invalid node address string");
            return -1;
        }
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "node address must be int, bytes, str, or node address capsule");
    return -1;
}

static PyObject *cf_datum_make_rc_bytes_tuple(int rc, const void *data, size_t data_size) {
    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyObject *bytes_obj = PyBytes_FromStringAndSize((const char *)data, (Py_ssize_t)data_size);
    if (!bytes_obj) {
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 1, bytes_obj);
    return tuple;
}

static void cf_datum_dump_callback_invoke(python_datum_dump_ctx_t *ctx, dap_json_t *json_out,
                                          const void *data, size_t size, const char *hash_out_type, int version) {
    if (!ctx || !ctx->callback) {
        return;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject *json_obj = NULL;
    if (json_out) {
        json_obj = PyCapsule_New(json_out, "dap_json_t", NULL);
    } else {
        Py_INCREF(Py_None);
        json_obj = Py_None;
    }

    PyObject *data_obj = NULL;
    if (data) {
        data_obj = PyBytes_FromStringAndSize((const char *)data, (Py_ssize_t)size);
    } else {
        Py_INCREF(Py_None);
        data_obj = Py_None;
    }

    PyObject *size_obj = PyLong_FromSize_t(size);
    PyObject *hash_obj = PyUnicode_FromString(hash_out_type ? hash_out_type : "");
    PyObject *version_obj = PyLong_FromLong(version);

    if (!json_obj || !data_obj || !size_obj || !hash_obj || !version_obj) {
        Py_XDECREF(json_obj);
        Py_XDECREF(data_obj);
        Py_XDECREF(size_obj);
        Py_XDECREF(hash_obj);
        Py_XDECREF(version_obj);
        PyGILState_Release(gstate);
        return;
    }

    PyObject *result = PyObject_CallFunctionObjArgs(
        ctx->callback, json_obj, data_obj, size_obj, hash_obj, version_obj, NULL
    );

    Py_DECREF(json_obj);
    Py_DECREF(data_obj);
    Py_DECREF(size_obj);
    Py_DECREF(hash_obj);
    Py_DECREF(version_obj);

    if (!result) {
        log_it(L_ERROR, "Python datum dump callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(result);
    }

    PyGILState_Release(gstate);
}

static void s_datum_dump_decree_callback(dap_json_t *a_json_out, const void *a_data,
                                         size_t a_size, const char *a_hash_out_type, int a_version) {
    cf_datum_dump_callback_invoke(s_datum_dump_decree_ctx, a_json_out, a_data, a_size, a_hash_out_type, a_version);
}

static void s_datum_dump_anchor_callback(dap_json_t *a_json_out, const void *a_data,
                                         size_t a_size, const char *a_hash_out_type, int a_version) {
    cf_datum_dump_callback_invoke(s_datum_dump_anchor_ctx, a_json_out, a_data, a_size, a_hash_out_type, a_version);
}

static int s_decree_registry_handler_wrapper(dap_chain_datum_decree_t *a_decree, dap_chain_net_t *a_net,
                                              bool a_apply, bool a_anchored) {
    if (!a_decree || !a_net) {
        log_it(L_ERROR, "Invalid decree registry handler arguments");
        return -1;
    }

    PyObject *callback = cf_datum_decree_handler_get_callback(a_decree->header.type, a_decree->header.sub_type);
    if (!callback) {
        log_it(L_ERROR, "No Python decree handler registered for type=%u subtype=%u",
               a_decree->header.type, a_decree->header.sub_type);
        return -404;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject *decree_capsule = PyCapsule_New(a_decree, "dap_chain_datum_decree_t", NULL);
    PyObject *net_capsule = PyCapsule_New(a_net, "dap_chain_net_t", NULL);
    PyObject *apply_obj = PyBool_FromLong(a_apply ? 1 : 0);
    PyObject *anchored_obj = PyBool_FromLong(a_anchored ? 1 : 0);

    if (!decree_capsule || !net_capsule || !apply_obj || !anchored_obj) {
        Py_XDECREF(decree_capsule);
        Py_XDECREF(net_capsule);
        Py_XDECREF(apply_obj);
        Py_XDECREF(anchored_obj);
        Py_DECREF(callback);
        PyGILState_Release(gstate);
        return -1;
    }

    PyObject *result = PyObject_CallFunctionObjArgs(
        callback, decree_capsule, net_capsule, apply_obj, anchored_obj, NULL
    );

    Py_DECREF(decree_capsule);
    Py_DECREF(net_capsule);
    Py_DECREF(apply_obj);
    Py_DECREF(anchored_obj);
    Py_DECREF(callback);

    int rc = -1;
    if (!result) {
        log_it(L_ERROR, "Python decree handler raised an exception");
        PyErr_Print();
    } else {
        if (PyLong_Check(result)) {
            rc = (int)PyLong_AsLong(result);
        } else {
            log_it(L_WARNING, "Decree handler returned non-integer result");
        }
        Py_DECREF(result);
    }

    PyGILState_Release(gstate);
    return rc;
}

PyObject *dap_chain_datum_calc_hash_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *datum_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &datum_obj)) {
        return NULL;
    }

    dap_chain_datum_t *datum = (dap_chain_datum_t *)cf_datum_get_capsule(
        datum_obj, "dap_chain_datum_t", "First argument must be a datum capsule"
    );
    if (!datum) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    dap_chain_datum_calc_hash(datum, &hash);
    return PyBytes_FromStringAndSize((const char *)&hash, sizeof(dap_hash_fast_t));
}

PyObject *dap_chain_datum_create_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int type_id = 0;
    PyObject *data_obj = Py_None;
    const void *data_ptr = NULL;
    Py_ssize_t data_size = 0;
    Py_buffer view = {0};
    bool has_view = false;

    if (!PyArg_ParseTuple(args, "I|O", &type_id, &data_obj)) {
        return NULL;
    }

    if (data_obj != Py_None) {
        if (PyObject_GetBuffer(data_obj, &view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "data must be bytes-like or None");
            return NULL;
        }
        data_ptr = view.buf;
        data_size = view.len;
        has_view = true;
    }

    dap_chain_datum_t *datum = dap_chain_datum_create((uint16_t)type_id, data_ptr, (size_t)data_size);

    if (has_view) {
        PyBuffer_Release(&view);
    }

    if (!datum) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create datum");
        return NULL;
    }

    return PyCapsule_New(datum, "dap_chain_datum_t", NULL);
}

PyObject *dap_chain_datum_dump_tx_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *json_arr_obj = NULL;
    PyObject *tx_obj = NULL;
    PyObject *ticker_obj = NULL;
    PyObject *json_obj_out = NULL;
    PyObject *hash_out_type_obj = NULL;
    PyObject *tx_hash_obj = NULL;
    unsigned long long net_id = 0;
    int version = 0;

    if (!PyArg_ParseTuple(args, "OOOOOOKi", &json_arr_obj, &tx_obj, &ticker_obj,
                          &json_obj_out, &hash_out_type_obj, &tx_hash_obj, &net_id, &version)) {
        return NULL;
    }

    dap_json_t *json_arr = NULL;
    if (json_arr_obj != Py_None) {
        json_arr = (dap_json_t *)cf_datum_get_capsule(
            json_arr_obj, "dap_json_t", "First argument must be a JSON capsule or None"
        );
        if (!json_arr) {
            return NULL;
        }
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_datum_get_capsule(
        tx_obj, "dap_chain_datum_tx_t", "Second argument must be a tx datum capsule"
    );
    if (!tx) {
        return NULL;
    }

    const char *ticker = NULL;
    if (ticker_obj != Py_None) {
        ticker = cf_datum_get_cstring(ticker_obj, "Third argument must be a string, bytes, or None");
        if (!ticker) {
            return NULL;
        }
    }

    dap_json_t *json_out = (dap_json_t *)cf_datum_get_capsule(
        json_obj_out, "dap_json_t", "Fourth argument must be a JSON capsule"
    );
    if (!json_out) {
        return NULL;
    }

    const char *hash_out_type = cf_datum_get_cstring(hash_out_type_obj, "Fifth argument must be a string or bytes");
    if (!hash_out_type) {
        return NULL;
    }

    dap_hash_fast_t tx_hash = {0};
    if (cf_datum_parse_hash_fast(tx_hash_obj, &tx_hash) != 0) {
        return NULL;
    }

    dap_chain_net_id_t net_id_struct = {.uint64 = net_id};
    bool ok = dap_chain_datum_dump_tx_json(
        json_arr, tx, ticker, json_out, hash_out_type, &tx_hash, net_id_struct, version
    );
    return PyBool_FromLong(ok ? 1 : 0);
}

PyObject *dap_chain_datum_register_dump_anchor_callback_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *callback = NULL;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }

    cf_datum_clear_ctx(&s_datum_dump_anchor_ctx);

    python_datum_dump_ctx_t *ctx = DAP_NEW_Z(python_datum_dump_ctx_t);
    if (!ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }

    ctx->callback = callback;
    s_datum_dump_anchor_ctx = ctx;

    dap_chain_datum_register_dump_anchor_callback(s_datum_dump_anchor_callback);

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_DATUM_DUMP_ANCHOR, callback, NULL,
                                  ctx, "datum_dump_anchor") != 0) {
        log_it(L_WARNING, "Failed to register datum anchor dump callback in registry");
    }

    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_register_dump_decree_callback_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *callback = NULL;

    if (!PyArg_ParseTuple(args, "O", &callback)) {
        return NULL;
    }
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }

    cf_datum_clear_ctx(&s_datum_dump_decree_ctx);

    python_datum_dump_ctx_t *ctx = DAP_NEW_Z(python_datum_dump_ctx_t);
    if (!ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }

    ctx->callback = callback;
    s_datum_dump_decree_ctx = ctx;

    dap_chain_datum_register_dump_decree_callback(s_datum_dump_decree_callback);

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_DATUM_DUMP_DECREE, callback, NULL,
                                  ctx, "datum_dump_decree") != 0) {
        log_it(L_WARNING, "Failed to register datum decree dump callback in registry");
    }

    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_size_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *datum_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &datum_obj)) {
        return NULL;
    }

    dap_chain_datum_t *datum = (dap_chain_datum_t *)cf_datum_get_capsule(
        datum_obj, "dap_chain_datum_t", "First argument must be a datum capsule"
    );
    if (!datum) {
        return NULL;
    }

    uint64_t size = dap_chain_datum_size(datum);
    return PyLong_FromUnsignedLongLong(size);
}

PyObject *dap_chain_datum_type_id_to_str_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int type_id = 0;

    if (!PyArg_ParseTuple(args, "I", &type_id)) {
        return NULL;
    }

    const char *str = dap_chain_datum_type_id_to_str((uint16_t)type_id);
    return PyUnicode_FromString(str ? str : "DATUM_UNKNOWN");
}

PyObject *dap_datum_token_dump_tsd_to_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *json_obj = NULL;
    PyObject *token_obj = NULL;
    Py_ssize_t token_size = 0;
    const char *hash_out_type = NULL;

    if (!PyArg_ParseTuple(args, "OOns", &json_obj, &token_obj, &token_size, &hash_out_type)) {
        return NULL;
    }

    if (token_size < 0) {
        PyErr_SetString(PyExc_ValueError, "token_size must be non-negative");
        return NULL;
    }

    dap_json_t *json = (dap_json_t *)cf_datum_get_capsule(
        json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!json) {
        return NULL;
    }

    dap_chain_datum_token_t *token = (dap_chain_datum_token_t *)cf_datum_get_capsule(
        token_obj, "dap_chain_datum_token_t", "Second argument must be a token capsule"
    );
    if (!token) {
        return NULL;
    }

    dap_datum_token_dump_tsd_to_json(json, token, (size_t)token_size, hash_out_type);
    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_token_flag_to_str_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long flag = 0;

    if (!PyArg_ParseTuple(args, "k", &flag)) {
        return NULL;
    }

    const char *str = dap_chain_datum_token_flag_to_str((uint32_t)flag);
    if (!str) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(str);
}

PyObject *dap_chain_datum_token_flag_from_str_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *str_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &str_obj)) {
        return NULL;
    }

    const char *str = cf_datum_get_cstring(str_obj, "First argument must be a string or bytes");
    if (!str) {
        return NULL;
    }

    uint32_t flag = dap_chain_datum_token_flag_from_str(str);
    return PyLong_FromUnsignedLong((unsigned long)flag);
}

PyObject *dap_chain_datum_token_get_delegated_ticker_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *ticker_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &ticker_obj)) {
        return NULL;
    }

    const char *ticker = cf_datum_get_cstring(ticker_obj, "First argument must be a string or bytes");
    if (!ticker) {
        return NULL;
    }

    char buf[DAP_CHAIN_TICKER_SIZE_MAX] = {0};
    int rc = dap_chain_datum_token_get_delegated_ticker(buf, ticker);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyTuple_SET_ITEM(tuple, 1, PyUnicode_FromString(buf));
    return tuple;
}

PyObject *dap_chain_datum_token_is_old_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int type = 0;

    if (!PyArg_ParseTuple(args, "I", &type)) {
        return NULL;
    }

    bool is_old = dap_chain_datum_token_is_old((uint8_t)type);
    return PyBool_FromLong(is_old ? 1 : 0);
}

PyObject *dap_chain_datum_token_check_ticker_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *ticker_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &ticker_obj)) {
        return NULL;
    }

    const char *ticker = cf_datum_get_cstring(ticker_obj, "First argument must be a string or bytes");
    if (!ticker) {
        return NULL;
    }

    bool ok = dap_chain_datum_token_check_ticker(ticker);
    return PyBool_FromLong(ok ? 1 : 0);
}

PyObject *dap_chain_datum_token_flags_dump_to_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *json_obj = NULL;
    PyObject *key_obj = NULL;
    unsigned int flags = 0;

    if (!PyArg_ParseTuple(args, "OOI", &json_obj, &key_obj, &flags)) {
        return NULL;
    }

    dap_json_t *json = (dap_json_t *)cf_datum_get_capsule(
        json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!json) {
        return NULL;
    }

    const char *key = cf_datum_get_cstring(key_obj, "Second argument must be a string or bytes");
    if (!key) {
        return NULL;
    }

    dap_chain_datum_token_flags_dump_to_json(json, key, (uint16_t)flags);
    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_token_certs_dump_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *signs = NULL;
    Py_ssize_t signs_size = 0;
    const char *hash_out_type = NULL;

    if (!PyArg_ParseTuple(args, "y#s", &signs, &signs_size, &hash_out_type)) {
        return NULL;
    }

    dap_string_t *out = dap_string_new(NULL);
    if (!out) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate output string");
        return NULL;
    }

    dap_chain_datum_token_certs_dump(out, (byte_t *)signs, (size_t)signs_size, hash_out_type);

    PyObject *result = PyUnicode_FromString(out->str ? out->str : "");
    dap_string_free(out, true);
    return result;
}

PyObject *dap_chain_datum_token_certs_dump_to_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *json_obj = NULL;
    const char *signs = NULL;
    Py_ssize_t signs_size = 0;
    const char *hash_out_type = NULL;
    int version = 0;

    if (!PyArg_ParseTuple(args, "Oy#si", &json_obj, &signs, &signs_size, &hash_out_type, &version)) {
        return NULL;
    }

    dap_json_t *json = (dap_json_t *)cf_datum_get_capsule(
        json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!json) {
        return NULL;
    }

    dap_chain_datum_token_certs_dump_to_json(json, (byte_t *)signs, (size_t)signs_size, hash_out_type, version);
    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_token_read_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *token_serial = NULL;
    Py_ssize_t token_size = 0;

    if (!PyArg_ParseTuple(args, "y#", &token_serial, &token_size)) {
        return NULL;
    }

    size_t token_size_out = (size_t)token_size;
    dap_chain_datum_token_t *token = dap_chain_datum_token_read((const byte_t *)token_serial, &token_size_out);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }

    if (!token) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 0, Py_None);
        PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(0));
        return tuple;
    }

    PyObject *capsule = PyCapsule_New(token, "dap_chain_datum_token_t", NULL);
    if (!capsule) {
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, capsule);
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(token_size_out));
    return tuple;
}

PyObject *dap_chain_datum_token_tsd_get_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *token_obj = NULL;
    Py_ssize_t token_size = 0;

    if (!PyArg_ParseTuple(args, "On", &token_obj, &token_size)) {
        return NULL;
    }
    if (token_size < 0) {
        PyErr_SetString(PyExc_ValueError, "token_size must be non-negative");
        return NULL;
    }

    dap_chain_datum_token_t *token = (dap_chain_datum_token_t *)cf_datum_get_capsule(
        token_obj, "dap_chain_datum_token_t", "First argument must be a token capsule"
    );
    if (!token) {
        return NULL;
    }

    dap_tsd_t *tsd = dap_chain_datum_token_tsd_get(token, (size_t)token_size);
    if (!tsd) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(tsd, "dap_tsd_t", NULL);
}

PyObject *dap_chain_datum_emission_type_str_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int type = 0;

    if (!PyArg_ParseTuple(args, "I", &type)) {
        return NULL;
    }

    const char *str = dap_chain_datum_emission_type_str((uint8_t)type);
    if (!str) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(str);
}

PyObject *dap_chain_datum_emission_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *value_obj = NULL;
    PyObject *ticker_obj = NULL;
    PyObject *addr_obj = NULL;

    if (!PyArg_ParseTuple(args, "OOO", &value_obj, &ticker_obj, &addr_obj)) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    const char *ticker = cf_datum_get_cstring(ticker_obj, "Second argument must be a string or bytes");
    if (!ticker) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_datum_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    dap_chain_datum_token_emission_t *emission = dap_chain_datum_emission_create(value, ticker, &addr);
    if (!emission) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(emission, "dap_chain_datum_token_emission_t", NULL);
}

PyObject *dap_chain_datum_emission_add_tsd_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *emission_obj = NULL;
    int tsd_type = 0;
    PyObject *data_obj = Py_None;
    Py_buffer view = {0};
    bool has_view = false;
    const void *data_ptr = NULL;
    Py_ssize_t data_size = 0;

    if (!PyArg_ParseTuple(args, "Oi|O", &emission_obj, &tsd_type, &data_obj)) {
        return NULL;
    }

    dap_chain_datum_token_emission_t *emission = (dap_chain_datum_token_emission_t *)cf_datum_get_capsule(
        emission_obj, "dap_chain_datum_token_emission_t", "First argument must be an emission capsule"
    );
    if (!emission) {
        return NULL;
    }

    if (data_obj != Py_None) {
        if (PyObject_GetBuffer(data_obj, &view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "data must be bytes-like or None");
            return NULL;
        }
        data_ptr = view.buf;
        data_size = view.len;
        has_view = true;
    }

    dap_chain_datum_token_emission_t *result = dap_chain_datum_emission_add_tsd(
        emission, tsd_type, (size_t)data_size, (void *)data_ptr
    );

    if (has_view) {
        PyBuffer_Release(&view);
    }

    if (!result) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(result, "dap_chain_datum_token_emission_t", NULL);
}

PyObject *dap_chain_emission_get_tsd_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *emission_obj = NULL;
    int tsd_type = 0;

    if (!PyArg_ParseTuple(args, "Oi", &emission_obj, &tsd_type)) {
        return NULL;
    }

    dap_chain_datum_token_emission_t *emission = (dap_chain_datum_token_emission_t *)cf_datum_get_capsule(
        emission_obj, "dap_chain_datum_token_emission_t", "First argument must be an emission capsule"
    );
    if (!emission) {
        return NULL;
    }

    size_t tsd_size = 0;
    byte_t *tsd_data = dap_chain_emission_get_tsd(emission, tsd_type, &tsd_size);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }

    if (!tsd_data) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 0, Py_None);
        PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(0));
        return tuple;
    }

    PyObject *data_obj = PyBytes_FromStringAndSize((const char *)tsd_data, (Py_ssize_t)tsd_size);
    if (!data_obj) {
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, data_obj);
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(tsd_size));
    return tuple;
}

PyObject *dap_chain_datum_emission_get_size_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *emission_serial = NULL;
    Py_ssize_t emission_size = 0;

    if (!PyArg_ParseTuple(args, "y#", &emission_serial, &emission_size)) {
        return NULL;
    }
    if (emission_size < (Py_ssize_t)sizeof(struct dap_chain_emission_header_v0)) {
        PyErr_SetString(PyExc_ValueError, "emission_serial is too small");
        return NULL;
    }

    size_t size = dap_chain_datum_emission_get_size((uint8_t *)emission_serial);
    return PyLong_FromSize_t(size);
}

PyObject *dap_chain_datum_emission_read_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *emission_serial = NULL;
    Py_ssize_t emission_size = 0;

    if (!PyArg_ParseTuple(args, "y#", &emission_serial, &emission_size)) {
        return NULL;
    }
    if (emission_size < (Py_ssize_t)sizeof(struct dap_chain_emission_header_v0)) {
        PyErr_SetString(PyExc_ValueError, "emission_serial is too small");
        return NULL;
    }

    size_t emission_size_out = (size_t)emission_size;
    dap_chain_datum_token_emission_t *emission = dap_chain_datum_emission_read(
        (byte_t *)emission_serial, &emission_size_out
    );

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }

    if (!emission) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 0, Py_None);
        PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(0));
        return tuple;
    }

    PyObject *capsule = PyCapsule_New(emission, "dap_chain_datum_token_emission_t", NULL);
    if (!capsule) {
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, capsule);
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(emission_size_out));
    return tuple;
}

PyObject *dap_chain_datum_emission_add_sign_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *key_obj = NULL;
    PyObject *emission_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &key_obj, &emission_obj)) {
        return NULL;
    }

    dap_enc_key_t *key = (dap_enc_key_t *)cf_datum_get_capsule(
        key_obj, "dap_enc_key_t", "First argument must be a dap_enc_key_t capsule"
    );
    if (!key) {
        return NULL;
    }

    dap_chain_datum_token_emission_t *emission = (dap_chain_datum_token_emission_t *)cf_datum_get_capsule(
        emission_obj, "dap_chain_datum_token_emission_t", "Second argument must be an emission capsule"
    );
    if (!emission) {
        return NULL;
    }

    dap_chain_datum_token_emission_t *result = dap_chain_datum_emission_add_sign(key, emission);
    if (!result) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(result, "dap_chain_datum_token_emission_t", NULL);
}

PyObject *dap_chain_datum_emission_append_sign_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *sign_obj = NULL;
    PyObject *emission_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &sign_obj, &emission_obj)) {
        return NULL;
    }

    dap_sign_t *sign = (dap_sign_t *)cf_datum_get_capsule(
        sign_obj, "dap_sign_t", "First argument must be a dap_sign_t capsule"
    );
    if (!sign) {
        return NULL;
    }

    dap_chain_datum_token_emission_t *emission = (dap_chain_datum_token_emission_t *)cf_datum_get_capsule(
        emission_obj, "dap_chain_datum_token_emission_t", "Second argument must be an emission capsule"
    );
    if (!emission) {
        return NULL;
    }

    dap_chain_datum_token_emission_t *result = dap_chain_datum_emission_append_sign(sign, emission);
    if (!result) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(result, "dap_chain_datum_token_emission_t", NULL);
}

PyObject *dap_chain_datum_emission_get_signs_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *emission_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &emission_obj)) {
        return NULL;
    }

    dap_chain_datum_token_emission_t *emission = (dap_chain_datum_token_emission_t *)cf_datum_get_capsule(
        emission_obj, "dap_chain_datum_token_emission_t", "First argument must be an emission capsule"
    );
    if (!emission) {
        return NULL;
    }

    size_t signs_count = 0;
    dap_sign_t *signs = dap_chain_datum_emission_get_signs(emission, &signs_count);

    size_t total_size = 0;
    if (signs && signs_count > 0) {
        dap_sign_t *cur = signs;
        for (size_t i = 0; i < signs_count; i++) {
            size_t sign_size = dap_sign_get_size(cur);
            if (sign_size == 0) {
                signs_count = i;
                break;
            }
            total_size += sign_size;
            cur = (dap_sign_t *)((byte_t *)cur + sign_size);
        }
    }

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        if (signs) {
            DAP_DELETE(signs);
        }
        return NULL;
    }

    PyObject *signs_obj = PyBytes_FromStringAndSize((const char *)signs, (Py_ssize_t)total_size);
    if (signs) {
        DAP_DELETE(signs);
    }
    if (!signs_obj) {
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, signs_obj);
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(signs_count));
    return tuple;
}

PyObject *dap_chain_datum_anchor_certs_dump_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *signs = NULL;
    Py_ssize_t signs_size = 0;
    const char *hash_out_type = NULL;

    if (!PyArg_ParseTuple(args, "y#s", &signs, &signs_size, &hash_out_type)) {
        return NULL;
    }

    dap_string_t *out = dap_string_new(NULL);
    if (!out) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate output string");
        return NULL;
    }

    dap_chain_datum_anchor_certs_dump(out, (byte_t *)signs, (size_t)signs_size, hash_out_type);

    PyObject *result = PyUnicode_FromString(out->str ? out->str : "");
    dap_string_free(out, true);
    return result;
}

PyObject *dap_chain_datum_anchor_certs_dump_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *json_obj = NULL;
    const char *signs = NULL;
    Py_ssize_t signs_size = 0;
    const char *hash_out_type = NULL;
    int version = 0;

    if (!PyArg_ParseTuple(args, "Oy#si", &json_obj, &signs, &signs_size, &hash_out_type, &version)) {
        return NULL;
    }

    dap_json_t *json = (dap_json_t *)cf_datum_get_capsule(
        json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!json) {
        return NULL;
    }

    dap_chain_datum_anchor_certs_dump_json(json, (byte_t *)signs, (size_t)signs_size, hash_out_type, version);
    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_anchor_get_hash_from_data_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *anchor_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &anchor_obj)) {
        return NULL;
    }

    dap_chain_datum_anchor_t *anchor = (dap_chain_datum_anchor_t *)cf_datum_get_capsule(
        anchor_obj, "dap_chain_datum_anchor_t", "First argument must be an anchor capsule"
    );
    if (!anchor) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    int rc = dap_chain_datum_anchor_get_hash_from_data(anchor, &hash);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SetItem(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SetItem(tuple, 1, Py_None);
        return tuple;
    }

    PyObject *hash_bytes = PyBytes_FromStringAndSize((const char *)&hash, sizeof(dap_hash_fast_t));
    if (!hash_bytes) {
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SetItem(tuple, 1, hash_bytes);
    return tuple;
}

PyObject *dap_chain_datum_anchor_get_size_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *anchor_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &anchor_obj)) {
        return NULL;
    }

    dap_chain_datum_anchor_t *anchor = (dap_chain_datum_anchor_t *)cf_datum_get_capsule(
        anchor_obj, "dap_chain_datum_anchor_t", "First argument must be an anchor capsule"
    );
    if (!anchor) {
        return NULL;
    }

    size_t size = dap_chain_datum_anchor_get_size(anchor);
    return PyLong_FromUnsignedLongLong((unsigned long long)size);
}

PyObject *dap_chain_datum_from_tx_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_datum_get_capsule(
        tx_obj, "dap_chain_datum_tx_t", "First argument must be a tx datum capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_datum_t *datum = dap_chain_datum_from_tx(tx);
    if (!datum) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(datum, "dap_chain_datum_t", NULL);
}

PyObject *dap_chain_datum_decree_certs_dump_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *json_obj = NULL;
    const char *signs = NULL;
    Py_ssize_t signs_size = 0;
    const char *hash_out_type = NULL;
    int version = 0;

    if (!PyArg_ParseTuple(args, "Oy#si", &json_obj, &signs, &signs_size, &hash_out_type, &version)) {
        return NULL;
    }

    dap_json_t *json = (dap_json_t *)cf_datum_get_capsule(
        json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!json) {
        return NULL;
    }

    dap_chain_datum_decree_certs_dump_json(json, (byte_t *)signs, (size_t)signs_size, hash_out_type, version);
    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_decree_dump_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *json_obj = NULL;
    PyObject *decree_obj = NULL;
    Py_ssize_t decree_size = 0;
    const char *hash_out_type = NULL;
    int version = 0;

    if (!PyArg_ParseTuple(args, "OOnsi", &json_obj, &decree_obj, &decree_size, &hash_out_type, &version)) {
        return NULL;
    }
    if (decree_size < 0) {
        PyErr_SetString(PyExc_ValueError, "decree_size must be non-negative");
        return NULL;
    }

    dap_json_t *json = (dap_json_t *)cf_datum_get_capsule(
        json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!json) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "Second argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_datum_decree_dump_json(json, decree, (size_t)decree_size, hash_out_type, version);
    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_decree_get_action_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint8_t action = 0;
    int rc = dap_chain_datum_decree_get_action(decree, &action);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromUnsignedLong((unsigned long)action));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_atom_num_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint64_t atom_num = 0;
    int rc = dap_chain_datum_decree_get_atom_num(decree, &atom_num);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromUnsignedLongLong(atom_num));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_ban_addr_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    const char *addr = NULL;
    int rc = dap_chain_datum_decree_get_ban_addr(decree, &addr);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0 || !addr) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyTuple_SET_ITEM(tuple, 1, PyUnicode_FromString(addr));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_empty_block_every_times_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint16_t blockgen_period = 0;
    int rc = dap_chain_datum_decree_get_empty_block_every_times(decree, &blockgen_period);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromUnsignedLong((unsigned long)blockgen_period));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_fee_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t fee_value = {0};
    int rc = dap_chain_datum_decree_get_fee(decree, &fee_value);
    return cf_datum_make_rc_bytes_tuple(rc, &fee_value, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_get_fee_addr_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_addr_t fee_addr = {0};
    int rc = dap_chain_datum_decree_get_fee_addr(decree, &fee_addr);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }

    dap_chain_addr_t *addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    if (!addr_copy) {
        Py_DECREF(tuple);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }
    *addr_copy = fee_addr;

    PyObject *capsule = PyCapsule_New(addr_copy, "dap_chain_addr_t", NULL);
    if (!capsule) {
        DAP_DELETE(addr_copy);
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 1, capsule);
    return tuple;
}

PyObject *dap_chain_datum_decree_get_hash_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    int rc = dap_chain_datum_decree_get_hash(decree, &hash);
    return cf_datum_make_rc_bytes_tuple(rc, &hash, sizeof(dap_hash_fast_t));
}

PyObject *dap_chain_datum_decree_get_min_owners_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t min_owners = {0};
    int rc = dap_chain_datum_decree_get_min_owners(decree, &min_owners);
    return cf_datum_make_rc_bytes_tuple(rc, &min_owners, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_get_node_addr_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_node_addr_t node_addr = {0};
    int rc = dap_chain_datum_decree_get_node_addr(decree, &node_addr);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }

    dap_chain_node_addr_t *addr_copy = DAP_NEW_Z(dap_chain_node_addr_t);
    if (!addr_copy) {
        Py_DECREF(tuple);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate node address");
        return NULL;
    }
    *addr_copy = node_addr;

    PyObject *capsule = PyCapsule_New(addr_copy, "dap_chain_node_addr_t", NULL);
    if (!capsule) {
        DAP_DELETE(addr_copy);
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 1, capsule);
    return tuple;
}

PyObject *dap_chain_datum_decree_get_owners_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint16_t owners_num = 0;
    dap_list_t *owners = dap_chain_datum_decree_get_owners(decree, &owners_num);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }

    if (!owners) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 0, Py_None);
    } else {
        PyObject *capsule = PyCapsule_New(owners, "dap_list_t", NULL);
        if (!capsule) {
            Py_DECREF(tuple);
            return NULL;
        }
        PyTuple_SET_ITEM(tuple, 0, capsule);
    }
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromUnsignedLong((unsigned long)owners_num));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_pkey_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_pkey_t *pkey = dap_chain_datum_decree_get_pkey(decree);
    if (!pkey) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(pkey, "dap_pkey_t", NULL);
}

PyObject *dap_chain_datum_decree_get_policy_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_policy_t *policy = dap_chain_datum_decree_get_policy(decree);
    if (!policy) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(policy, "dap_chain_policy_t", NULL);
}

PyObject *dap_chain_datum_decree_get_signature_type_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint32_t signature_type = 0;
    int rc = dap_chain_datum_decree_get_signature_type(decree, &signature_type);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromUnsignedLong((unsigned long)signature_type));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_signs_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    size_t signs_size = 0;
    dap_sign_t *signs = dap_chain_datum_decree_get_signs(decree, &signs_size);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }

    PyObject *signs_obj = PyBytes_FromStringAndSize((const char *)signs, (Py_ssize_t)signs_size);
    if (!signs_obj) {
        Py_DECREF(tuple);
        return NULL;
    }

    PyTuple_SET_ITEM(tuple, 0, signs_obj);
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(signs_size));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_size_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    size_t size = dap_chain_datum_decree_get_size(decree);
    return PyLong_FromUnsignedLongLong((unsigned long long)size);
}

PyObject *dap_chain_datum_decree_get_stake_min_signers_count_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t min_signers = {0};
    int rc = dap_chain_datum_decree_get_stake_min_signers_count(decree, &min_signers);
    return cf_datum_make_rc_bytes_tuple(rc, &min_signers, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_get_stake_min_value_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t min_value = {0};
    int rc = dap_chain_datum_decree_get_stake_min_value(decree, &min_value);
    return cf_datum_make_rc_bytes_tuple(rc, &min_value, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_get_stake_signing_addr_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_addr_t signing_addr = {0};
    int rc = dap_chain_datum_decree_get_stake_signing_addr(decree, &signing_addr);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }

    dap_chain_addr_t *addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    if (!addr_copy) {
        Py_DECREF(tuple);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }
    *addr_copy = signing_addr;

    PyObject *capsule = PyCapsule_New(addr_copy, "dap_chain_addr_t", NULL);
    if (!capsule) {
        DAP_DELETE(addr_copy);
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 1, capsule);
    return tuple;
}

PyObject *dap_chain_datum_decree_get_stake_value_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t stake_value = {0};
    int rc = dap_chain_datum_decree_get_stake_value(decree, &stake_value);
    return cf_datum_make_rc_bytes_tuple(rc, &stake_value, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_get_value_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t value = {0};
    int rc = dap_chain_datum_decree_get_value(decree, &value);
    return cf_datum_make_rc_bytes_tuple(rc, &value, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_new_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long net_id = 0;
    unsigned long long chain_id = 0;
    unsigned long long cell_id = 0;
    unsigned long long total_tsd_size = 0;

    if (!PyArg_ParseTuple(args, "KKKK", &net_id, &chain_id, &cell_id, &total_tsd_size)) {
        return NULL;
    }

    dap_chain_net_id_t net_id_struct = {.uint64 = net_id};
    dap_chain_id_t chain_id_struct = {.uint64 = chain_id};
    dap_chain_cell_id_t cell_id_struct = {.uint64 = cell_id};

    dap_chain_datum_decree_t *decree = dap_chain_datum_decree_new(
        net_id_struct, chain_id_struct, cell_id_struct, (size_t)total_tsd_size
    );
    if (!decree) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create decree");
        return NULL;
    }

    return PyCapsule_New(decree, "dap_chain_datum_decree_t", NULL);
}

PyObject *dap_chain_datum_decree_sign_in_cycle_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *certs_obj = NULL;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &certs_obj, &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "Second argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    if (!PyList_Check(certs_obj) && !PyTuple_Check(certs_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a list or tuple of certificates");
        return NULL;
    }

    Py_ssize_t certs_count = PySequence_Size(certs_obj);
    if (certs_count < 0) {
        return NULL;
    }

    dap_cert_t **certs = NULL;
    if (certs_count > 0) {
        certs = DAP_NEW_Z_COUNT(dap_cert_t *, (size_t)certs_count);
        if (!certs) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate certificates array");
            return NULL;
        }
    }

    for (Py_ssize_t i = 0; i < certs_count; i++) {
        PyObject *item = PySequence_GetItem(certs_obj, i);
        if (!item) {
            DAP_DELETE(certs);
            return NULL;
        }
        if (!PyCapsule_CheckExact(item)) {
            Py_DECREF(item);
            DAP_DELETE(certs);
            PyErr_SetString(PyExc_TypeError, "Certificate must be a dap_cert_t capsule");
            return NULL;
        }
        certs[i] = (dap_cert_t *)PyCapsule_GetPointer(item, "dap_cert_t");
        Py_DECREF(item);
        if (!certs[i]) {
            DAP_DELETE(certs);
            PyErr_SetString(PyExc_ValueError, "Invalid certificate capsule");
            return NULL;
        }
    }

    size_t total_sign_count = 0;
    dap_chain_datum_decree_t *result = dap_chain_datum_decree_sign_in_cycle(
        certs, decree, (size_t)certs_count, &total_sign_count
    );

    DAP_DELETE(certs);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }

    if (!result) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 0, Py_None);
        PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(total_sign_count));
        return tuple;
    }

    if (result != decree) {
        PyCapsule_SetPointer(decree_obj, result);
    }

    PyObject *capsule = PyCapsule_New(result, "dap_chain_datum_decree_t", NULL);
    if (!capsule) {
        Py_DECREF(tuple);
        return NULL;
    }

    PyTuple_SET_ITEM(tuple, 0, capsule);
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(total_sign_count));
    return tuple;
}

PyObject *dap_chain_datum_decree_subtype_to_str_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int subtype = 0;

    if (!PyArg_ParseTuple(args, "I", &subtype)) {
        return NULL;
    }

    const char *str = dap_chain_datum_decree_subtype_to_str((uint16_t)subtype);
    return PyUnicode_FromString(str ? str : "DECREE_SUBTYPE_UNKNOWN");
}

PyObject *dap_chain_datum_decree_tsd_type_to_str_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int tsd_type = 0;

    if (!PyArg_ParseTuple(args, "I", &tsd_type)) {
        return NULL;
    }

    const char *str = dap_chain_datum_decree_tsd_type_to_str((uint16_t)tsd_type);
    return PyUnicode_FromString(str ? str : "DECREE_TSD_TYPE_UNKNOWN");
}

PyObject *dap_chain_datum_decree_type_from_str_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *type_str = NULL;

    if (!PyArg_ParseTuple(args, "s", &type_str)) {
        return NULL;
    }

    uint16_t type = dap_chain_datum_decree_type_from_str(type_str);
    return PyLong_FromUnsignedLong((unsigned long)type);
}

PyObject *dap_chain_decree_registry_deinit_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    dap_chain_decree_registry_deinit();
    cf_datum_decree_handlers_clear();
    Py_RETURN_NONE;
}

PyObject *dap_chain_decree_registry_has_handler_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int decree_type = 0;
    unsigned int decree_subtype = 0;

    if (!PyArg_ParseTuple(args, "II", &decree_type, &decree_subtype)) {
        return NULL;
    }

    bool has = dap_chain_decree_registry_has_handler((uint16_t)decree_type, (uint16_t)decree_subtype);
    return PyBool_FromLong(has ? 1 : 0);
}

PyObject *dap_chain_decree_registry_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    int rc = dap_chain_decree_registry_init();
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_decree_registry_process_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;
    PyObject *net_obj = NULL;
    int apply = 0;
    int anchored = 0;

    if (!PyArg_ParseTuple(args, "OOpp", &decree_obj, &net_obj, &apply, &anchored)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_net_t *net = (dap_chain_net_t *)cf_datum_get_capsule(
        net_obj, "dap_chain_net_t", "Second argument must be a net capsule"
    );
    if (!net) {
        return NULL;
    }

    int rc = dap_chain_decree_registry_process(decree, net, apply != 0, anchored != 0);
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_decree_registry_register_handler_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int decree_type = 0;
    unsigned int decree_subtype = 0;
    PyObject *callback = NULL;
    const char *handler_name = NULL;

    if (!PyArg_ParseTuple(args, "IIOs", &decree_type, &decree_subtype, &callback, &handler_name)) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Handler must be callable");
        return NULL;
    }

    python_decree_handler_ctx_t *ctx = DAP_NEW_Z(python_decree_handler_ctx_t);
    if (!ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate handler context");
        return NULL;
    }

    ctx->decree_type = (uint16_t)decree_type;
    ctx->decree_subtype = (uint16_t)decree_subtype;
    ctx->callback = callback;
    ctx->handler_name = dap_strdup(handler_name);
    if (!ctx->handler_name) {
        DAP_DELETE(ctx);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate handler name");
        return NULL;
    }

    int rc = dap_chain_decree_registry_register_handler(
        ctx->decree_type, ctx->decree_subtype, s_decree_registry_handler_wrapper, ctx->handler_name
    );
    if (rc != 0) {
        DAP_DELETE(ctx->handler_name);
        DAP_DELETE(ctx);
        return PyLong_FromLong(rc);
    }

    pthread_mutex_lock(&s_decree_handlers_mutex);
    python_decree_handler_ctx_t **tmp = DAP_REALLOC(
        s_decree_handlers, (s_decree_handlers_count + 1) * sizeof(python_decree_handler_ctx_t *)
    );
    if (!tmp) {
        pthread_mutex_unlock(&s_decree_handlers_mutex);
        dap_chain_decree_registry_unregister_handler(ctx->decree_type, ctx->decree_subtype);
        DAP_DELETE(ctx->handler_name);
        DAP_DELETE(ctx);
        PyErr_SetString(PyExc_MemoryError, "Failed to grow handler registry");
        return NULL;
    }
    s_decree_handlers = tmp;
    s_decree_handlers[s_decree_handlers_count++] = ctx;
    pthread_mutex_unlock(&s_decree_handlers_mutex);

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_DECREE_REGISTRY, callback, NULL, ctx, NULL) != 0) {
        log_it(L_WARNING, "Failed to register decree handler in callbacks registry");
    }

    return PyLong_FromLong(rc);
}

PyObject *dap_chain_decree_registry_unregister_handler_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int decree_type = 0;
    unsigned int decree_subtype = 0;

    if (!PyArg_ParseTuple(args, "II", &decree_type, &decree_subtype)) {
        return NULL;
    }

    int rc = dap_chain_decree_registry_unregister_handler((uint16_t)decree_type, (uint16_t)decree_subtype);
    if (rc != 0) {
        return PyLong_FromLong(rc);
    }

    python_decree_handler_ctx_t *ctx = NULL;
    pthread_mutex_lock(&s_decree_handlers_mutex);
    size_t index = 0;
    ctx = cf_datum_decree_handler_find((uint16_t)decree_type, (uint16_t)decree_subtype, &index);
    if (ctx) {
        for (size_t i = index + 1; i < s_decree_handlers_count; i++) {
            s_decree_handlers[i - 1] = s_decree_handlers[i];
        }
        s_decree_handlers_count--;
        if (s_decree_handlers_count == 0) {
            DAP_DELETE(s_decree_handlers);
            s_decree_handlers = NULL;
        } else {
            python_decree_handler_ctx_t **tmp = DAP_REALLOC(
                s_decree_handlers, s_decree_handlers_count * sizeof(python_decree_handler_ctx_t *)
            );
            if (tmp) {
                s_decree_handlers = tmp;
            }
        }
    }
    pthread_mutex_unlock(&s_decree_handlers_mutex);

    if (ctx) {
        cf_datum_decree_handler_free(ctx);
    }

    return PyLong_FromLong(rc);
}

PyObject *dap_chain_datum_tx_event_to_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *json_obj = NULL;
    PyObject *event_obj = NULL;
    PyObject *hash_out_type_obj = NULL;

    if (!PyArg_ParseTuple(args, "OOO", &json_obj, &event_obj, &hash_out_type_obj)) {
        return NULL;
    }

    dap_json_t *json = (dap_json_t *)cf_datum_get_capsule(
        json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!json) {
        return NULL;
    }

    dap_chain_tx_event_t *event = (dap_chain_tx_event_t *)cf_datum_get_capsule(
        event_obj, "dap_chain_tx_event_t", "Second argument must be a tx event capsule"
    );
    if (!event) {
        return NULL;
    }

    const char *hash_out_type = cf_datum_get_cstring(hash_out_type_obj, "Third argument must be a string or bytes");
    if (!hash_out_type) {
        return NULL;
    }

    int rc = dap_chain_datum_tx_event_to_json(json, event, hash_out_type);
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_datum_tx_item_event_to_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *json_obj = NULL;
    PyObject *event_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &json_obj, &event_obj)) {
        return NULL;
    }

    dap_json_t *json = (dap_json_t *)cf_datum_get_capsule(
        json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!json) {
        return NULL;
    }

    dap_chain_tx_item_event_t *event = (dap_chain_tx_item_event_t *)cf_datum_get_capsule(
        event_obj, "dap_chain_tx_item_event_t", "Second argument must be a tx item event capsule"
    );
    if (!event) {
        return NULL;
    }

    int rc = dap_chain_datum_tx_item_event_to_json(json, event);
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_tx_item_event_type_from_str_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *str_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &str_obj)) {
        return NULL;
    }

    const char *event_type_str = cf_datum_get_cstring(str_obj, "First argument must be a string or bytes");
    if (!event_type_str) {
        return NULL;
    }

    int event_type = dap_chain_tx_item_event_type_from_str(event_type_str);
    return PyLong_FromLong(event_type);
}

PyObject *dap_chain_tx_item_event_type_to_str_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int event_type = 0;

    if (!PyArg_ParseTuple(args, "I", &event_type)) {
        return NULL;
    }

    const char *event_type_str = dap_chain_tx_item_event_type_to_str((uint16_t)event_type);
    return PyUnicode_FromString(event_type_str ? event_type_str : "unknown");
}

PyObject *dap_chain_datum_item_tx_get_size_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *item_obj = NULL;
    Py_ssize_t max_size = -1;
    Py_buffer view = {0};
    bool has_view = false;

    if (!PyArg_ParseTuple(args, "O|n", &item_obj, &max_size)) {
        return NULL;
    }

    const byte_t *item_ptr = NULL;
    if (PyCapsule_CheckExact(item_obj)) {
        const char *name = PyCapsule_GetName(item_obj);
        if (!name) {
            PyErr_SetString(PyExc_ValueError, "Invalid item capsule");
            return NULL;
        }
        item_ptr = (const byte_t *)PyCapsule_GetPointer(item_obj, name);
        if (!item_ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid item capsule pointer");
            return NULL;
        }
    } else {
        if (PyObject_GetBuffer(item_obj, &view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "Item must be a capsule or bytes-like object");
            return NULL;
        }
        item_ptr = (const byte_t *)view.buf;
        has_view = true;
        if (max_size < 0) {
            max_size = view.len;
        }
    }

    if (max_size < 0) {
        max_size = 0;
    }

    size_t size = dap_chain_datum_item_tx_get_size(item_ptr, (size_t)max_size);

    if (has_view) {
        PyBuffer_Release(&view);
    }

    return PyLong_FromSize_t(size);
}

PyObject *dap_chain_datum_tx_event_create_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long srv_uid_uint = 0;
    PyObject *group_obj = NULL;
    unsigned int event_type = 0;
    unsigned long long timestamp = 0;

    if (!PyArg_ParseTuple(args, "KOIK", &srv_uid_uint, &group_obj, &event_type, &timestamp)) {
        return NULL;
    }

    const char *group_name = cf_datum_get_cstring(group_obj, "Second argument must be a string or bytes");
    if (!group_name) {
        return NULL;
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_tx_item_event_t *event = dap_chain_datum_tx_event_create(
        srv_uid, group_name, (uint16_t)event_type, (dap_time_t)timestamp
    );
    if (!event) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(event, "dap_chain_tx_item_event_t", NULL);
}

PyObject *dap_chain_datum_tx_item_get_data_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tsd_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &tsd_obj)) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tx_tsd = (dap_chain_tx_tsd_t *)cf_datum_get_capsule(
        tsd_obj, "dap_chain_tx_tsd_t", "First argument must be a tx tsd capsule"
    );
    if (!tx_tsd) {
        return NULL;
    }

    int type = 0;
    size_t size = 0;
    byte_t *data = dap_chain_datum_tx_item_get_data(tx_tsd, &type, &size);

    PyObject *tuple = PyTuple_New(3);
    if (!tuple) {
        return NULL;
    }

    if (!data) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 0, Py_None);
        PyTuple_SET_ITEM(tuple, 1, PyLong_FromLong(0));
        PyTuple_SET_ITEM(tuple, 2, PyLong_FromSize_t(0));
        return tuple;
    }

    PyObject *data_obj = PyBytes_FromStringAndSize((const char *)data, (Py_ssize_t)size);
    if (!data_obj) {
        Py_DECREF(tuple);
        return NULL;
    }

    PyTuple_SET_ITEM(tuple, 0, data_obj);
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromLong(type));
    PyTuple_SET_ITEM(tuple, 2, PyLong_FromSize_t(size));
    return tuple;
}

PyObject *dap_chain_datum_tx_item_get_tsd_by_type_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_obj = NULL;
    int tsd_type = 0;

    if (!PyArg_ParseTuple(args, "Oi", &tx_obj, &tsd_type)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_datum_get_capsule(
        tx_obj, "dap_chain_datum_tx_t", "First argument must be a tx datum capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_tx_item_get_tsd_by_type(tx, tsd_type);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_tx_item_in_cond_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *hash_obj = NULL;
    unsigned int prev_tx_out_idx = 0;
    unsigned int receipt_idx = 0;

    if (!PyArg_ParseTuple(args, "OII", &hash_obj, &prev_tx_out_idx, &receipt_idx)) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }

    dap_chain_tx_in_cond_t *item = dap_chain_datum_tx_item_in_cond_create(
        &hash, (uint32_t)prev_tx_out_idx, (uint32_t)receipt_idx
    );
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_in_cond_t", NULL);
}

PyObject *dap_chain_datum_tx_item_in_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *hash_obj = NULL;
    unsigned int prev_tx_out_idx = 0;

    if (!PyArg_ParseTuple(args, "OI", &hash_obj, &prev_tx_out_idx)) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }

    dap_chain_tx_in_t *item = dap_chain_datum_tx_item_in_create(&hash, (uint32_t)prev_tx_out_idx);
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_in_t", NULL);
}

PyObject *dap_chain_datum_tx_item_in_ems_create_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long chain_id_uint = 0;
    PyObject *hash_obj = NULL;
    PyObject *ticker_obj = NULL;

    if (!PyArg_ParseTuple(args, "KOO", &chain_id_uint, &hash_obj, &ticker_obj)) {
        return NULL;
    }

    dap_hash_fast_t token_hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &token_hash) != 0) {
        return NULL;
    }

    const char *ticker = cf_datum_get_cstring(ticker_obj, "Third argument must be a string or bytes");
    if (!ticker) {
        return NULL;
    }

    dap_chain_id_t chain_id = {.uint64 = chain_id_uint};
    dap_chain_tx_in_ems_t *item = dap_chain_datum_tx_item_in_ems_create(chain_id, &token_hash, ticker);
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_in_ems_t", NULL);
}

PyObject *dap_chain_datum_tx_item_in_reward_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *hash_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &hash_obj)) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }

    dap_chain_tx_in_reward_t *item = dap_chain_datum_tx_item_in_reward_create(&hash);
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_in_reward_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_cond_create_fee_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *value_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &value_obj)) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    dap_chain_tx_out_cond_t *item = dap_chain_datum_tx_item_out_cond_create_fee(value);
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_cond_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_cond_create_srv_pay_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *pkey_obj = NULL;
    unsigned long long srv_uid_uint = 0;
    PyObject *value_obj = NULL;
    PyObject *value_max_obj = NULL;
    int unit_enm = 0;
    PyObject *params_obj = Py_None;
    Py_buffer params_view = {0};
    bool has_view = false;

    if (!PyArg_ParseTuple(args, "OKOOi|O", &pkey_obj, &srv_uid_uint,
                          &value_obj, &value_max_obj, &unit_enm, &params_obj)) {
        return NULL;
    }

    dap_pkey_t *pkey = (dap_pkey_t *)cf_datum_get_capsule(
        pkey_obj, "dap_pkey_t", "First argument must be a dap_pkey_t capsule"
    );
    if (!pkey) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    uint256_t value_max = {0};
    if (cf_datum_parse_uint256(value_max_obj, &value_max) != 0) {
        return NULL;
    }

    const void *params_ptr = NULL;
    size_t params_size = 0;
    if (params_obj != Py_None) {
        if (PyObject_GetBuffer(params_obj, &params_view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "Params must be bytes-like or None");
            return NULL;
        }
        params_ptr = params_view.buf;
        params_size = (size_t)params_view.len;
        has_view = true;
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_net_srv_price_unit_uid_t unit = {.enm = (dap_chain_srv_unit_enum_t)unit_enm};
    dap_chain_tx_out_cond_t *item = dap_chain_datum_tx_item_out_cond_create_srv_pay(
        pkey, srv_uid, value, value_max, unit, params_ptr, params_size
    );

    if (has_view) {
        PyBuffer_Release(&params_view);
    }

    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_cond_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *hash_obj = NULL;
    unsigned long long srv_uid_uint = 0;
    PyObject *value_obj = NULL;
    PyObject *value_max_obj = NULL;
    int unit_enm = 0;
    PyObject *params_obj = Py_None;
    Py_buffer params_view = {0};
    bool has_view = false;

    if (!PyArg_ParseTuple(args, "OKOOi|O", &hash_obj, &srv_uid_uint,
                          &value_obj, &value_max_obj, &unit_enm, &params_obj)) {
        return NULL;
    }

    dap_hash_fast_t key_hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &key_hash) != 0) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    uint256_t value_max = {0};
    if (cf_datum_parse_uint256(value_max_obj, &value_max) != 0) {
        return NULL;
    }

    const void *params_ptr = NULL;
    size_t params_size = 0;
    if (params_obj != Py_None) {
        if (PyObject_GetBuffer(params_obj, &params_view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "Params must be bytes-like or None");
            return NULL;
        }
        params_ptr = params_view.buf;
        params_size = (size_t)params_view.len;
        has_view = true;
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_net_srv_price_unit_uid_t unit = {.enm = (dap_chain_srv_unit_enum_t)unit_enm};
    dap_chain_tx_out_cond_t *item = dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash(
        &key_hash, srv_uid, value, value_max, unit, params_ptr, params_size
    );

    if (has_view) {
        PyBuffer_Release(&params_view);
    }

    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_cond_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_cond_create_srv_stake_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long srv_uid_uint = 0;
    PyObject *value_obj = NULL;
    PyObject *signing_addr_obj = NULL;
    PyObject *signer_node_addr_obj = NULL;
    PyObject *sovereign_addr_obj = NULL;
    PyObject *sovereign_tax_obj = NULL;
    PyObject *pkey_obj = NULL;

    if (!PyArg_ParseTuple(args, "KOOOOOO", &srv_uid_uint, &value_obj, &signing_addr_obj,
                          &signer_node_addr_obj, &sovereign_addr_obj, &sovereign_tax_obj, &pkey_obj)) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    dap_chain_addr_t signing_addr = {0};
    if (cf_datum_parse_addr(signing_addr_obj, &signing_addr) != 0) {
        return NULL;
    }

    dap_chain_node_addr_t signer_node_addr = {0};
    if (cf_datum_parse_node_addr(signer_node_addr_obj, &signer_node_addr) != 0) {
        return NULL;
    }

    uint256_t sovereign_tax = {0};
    if (cf_datum_parse_uint256(sovereign_tax_obj, &sovereign_tax) != 0) {
        return NULL;
    }

    dap_chain_addr_t sovereign_addr = {0};
    dap_chain_addr_t *sovereign_addr_ptr = NULL;
    if (sovereign_addr_obj != Py_None) {
        if (cf_datum_parse_addr(sovereign_addr_obj, &sovereign_addr) != 0) {
            return NULL;
        }
        sovereign_addr_ptr = &sovereign_addr;
    }

    dap_pkey_t *pkey = NULL;
    if (pkey_obj != Py_None) {
        pkey = (dap_pkey_t *)cf_datum_get_capsule(
            pkey_obj, "dap_pkey_t", "Seventh argument must be a dap_pkey_t capsule or None"
        );
        if (!pkey) {
            return NULL;
        }
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_tx_out_cond_t *item = dap_chain_datum_tx_item_out_cond_create_srv_stake(
        srv_uid, value, &signing_addr, &signer_node_addr, sovereign_addr_ptr, sovereign_tax, pkey
    );
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_cond_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long srv_uid_uint = 0;
    PyObject *value_obj = NULL;
    PyObject *signing_addr_obj = NULL;
    PyObject *signer_node_addr_obj = NULL;
    PyObject *sovereign_tax_obj = NULL;
    PyObject *params_obj = Py_None;
    Py_buffer params_view = {0};
    bool has_view = false;

    if (!PyArg_ParseTuple(args, "KOOOO|O", &srv_uid_uint, &value_obj, &signing_addr_obj,
                          &signer_node_addr_obj, &sovereign_tax_obj, &params_obj)) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    dap_chain_addr_t signing_addr = {0};
    if (cf_datum_parse_addr(signing_addr_obj, &signing_addr) != 0) {
        return NULL;
    }

    dap_chain_node_addr_t signer_node_addr = {0};
    if (cf_datum_parse_node_addr(signer_node_addr_obj, &signer_node_addr) != 0) {
        return NULL;
    }

    uint256_t sovereign_tax = {0};
    if (cf_datum_parse_uint256(sovereign_tax_obj, &sovereign_tax) != 0) {
        return NULL;
    }

    const void *params_ptr = NULL;
    size_t params_size = 0;
    if (params_obj != Py_None) {
        if (PyObject_GetBuffer(params_obj, &params_view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "Params must be bytes-like or None");
            return NULL;
        }
        params_ptr = params_view.buf;
        params_size = (size_t)params_view.len;
        has_view = true;
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_tx_out_cond_t *item = dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate(
        srv_uid, value, &signing_addr, &signer_node_addr, sovereign_tax, params_ptr, params_size
    );

    if (has_view) {
        PyBuffer_Release(&params_view);
    }

    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_cond_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long srv_uid_uint = 0;
    PyObject *value_obj = NULL;
    PyObject *hash_obj = NULL;
    unsigned long long lock_time = 0;
    unsigned int position_id = 0;
    PyObject *params_obj = Py_None;
    Py_buffer params_view = {0};
    bool has_view = false;

    if (!PyArg_ParseTuple(args, "KOOKI|O", &srv_uid_uint, &value_obj, &hash_obj,
                          &lock_time, &position_id, &params_obj)) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    dap_hash_fast_t stake_ext_hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &stake_ext_hash) != 0) {
        return NULL;
    }

    const void *params_ptr = NULL;
    size_t params_size = 0;
    if (params_obj != Py_None) {
        if (PyObject_GetBuffer(params_obj, &params_view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "Params must be bytes-like or None");
            return NULL;
        }
        params_ptr = params_view.buf;
        params_size = (size_t)params_view.len;
        has_view = true;
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_tx_out_cond_t *item = dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock(
        srv_uid, value, &stake_ext_hash, (dap_time_t)lock_time, (uint32_t)position_id, params_ptr, params_size
    );

    if (has_view) {
        PyBuffer_Release(&params_view);
    }

    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_cond_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size_py(PyObject *self, PyObject *args) {
    (void)self;
    int has_sovereign_addr = 0;
    unsigned int pkey_size = 0;

    if (!PyArg_ParseTuple(args, "pI", &has_sovereign_addr, &pkey_size)) {
        return NULL;
    }

    uint32_t size = dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size(
        has_sovereign_addr != 0, (uint32_t)pkey_size
    );
    return PyLong_FromUnsignedLong((unsigned long)size);
}

PyObject *dap_chain_datum_tx_item_out_cond_create_srv_stake_lock_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long srv_uid_uint = 0;
    PyObject *value_obj = NULL;
    unsigned long long time_unlock = 0;
    PyObject *reinvest_obj = NULL;

    if (!PyArg_ParseTuple(args, "KOKO", &srv_uid_uint, &value_obj, &time_unlock, &reinvest_obj)) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    uint256_t reinvest_percent = {0};
    if (cf_datum_parse_uint256(reinvest_obj, &reinvest_percent) != 0) {
        return NULL;
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_tx_out_cond_t *item = dap_chain_datum_tx_item_out_cond_create_srv_stake_lock(
        srv_uid, value, (uint64_t)time_unlock, reinvest_percent
    );
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_cond_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_cond_create_srv_xchange_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long srv_uid_uint = 0;
    unsigned long long sell_net_id_uint = 0;
    PyObject *value_sell_obj = NULL;
    unsigned long long buy_net_id_uint = 0;
    PyObject *token_obj = NULL;
    PyObject *value_rate_obj = NULL;
    PyObject *seller_addr_obj = NULL;
    PyObject *params_obj = Py_None;
    Py_buffer params_view = {0};
    bool has_view = false;

    if (!PyArg_ParseTuple(args, "KKOKOOO|O", &srv_uid_uint, &sell_net_id_uint, &value_sell_obj,
                          &buy_net_id_uint, &token_obj, &value_rate_obj, &seller_addr_obj, &params_obj)) {
        return NULL;
    }

    uint256_t value_sell = {0};
    if (cf_datum_parse_uint256(value_sell_obj, &value_sell) != 0) {
        return NULL;
    }

    uint256_t value_rate = {0};
    if (cf_datum_parse_uint256(value_rate_obj, &value_rate) != 0) {
        return NULL;
    }

    const char *token = cf_datum_get_cstring(token_obj, "Fifth argument must be a string or bytes");
    if (!token) {
        return NULL;
    }

    dap_chain_addr_t seller_addr = {0};
    if (cf_datum_parse_addr(seller_addr_obj, &seller_addr) != 0) {
        return NULL;
    }

    const void *params_ptr = NULL;
    size_t params_size = 0;
    if (params_obj != Py_None) {
        if (PyObject_GetBuffer(params_obj, &params_view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "Params must be bytes-like or None");
            return NULL;
        }
        params_ptr = params_view.buf;
        params_size = (size_t)params_view.len;
        has_view = true;
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_net_id_t sell_net_id = {.uint64 = sell_net_id_uint};
    dap_chain_net_id_t buy_net_id = {.uint64 = buy_net_id_uint};
    dap_chain_tx_out_cond_t *item = dap_chain_datum_tx_item_out_cond_create_srv_xchange(
        srv_uid, sell_net_id, value_sell, buy_net_id, token, value_rate, &seller_addr, params_ptr, (uint32_t)params_size
    );

    if (has_view) {
        PyBuffer_Release(&params_view);
    }

    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_cond_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_cond_create_wallet_shared_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long srv_uid_uint = 0;
    PyObject *value_obj = NULL;
    unsigned int signs_min = 0;
    PyObject *hashes_obj = NULL;
    PyObject *tag_obj = Py_None;

    if (!PyArg_ParseTuple(args, "KOIO|O", &srv_uid_uint, &value_obj, &signs_min, &hashes_obj, &tag_obj)) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    if (!PyList_Check(hashes_obj) && !PyTuple_Check(hashes_obj)) {
        PyErr_SetString(PyExc_TypeError, "Fourth argument must be a list or tuple of hashes");
        return NULL;
    }

    Py_ssize_t hashes_count = PySequence_Size(hashes_obj);
    if (hashes_count < 0) {
        return NULL;
    }

    dap_hash_fast_t *hashes = NULL;
    if (hashes_count > 0) {
        hashes = DAP_NEW_Z_COUNT(dap_hash_fast_t, (size_t)hashes_count);
        if (!hashes) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate hashes array");
            return NULL;
        }
    }

    for (Py_ssize_t i = 0; i < hashes_count; i++) {
        PyObject *item = PySequence_GetItem(hashes_obj, i);
        if (!item) {
            DAP_DELETE(hashes);
            return NULL;
        }
        if (cf_datum_parse_hash_fast(item, &hashes[i]) != 0) {
            Py_DECREF(item);
            DAP_DELETE(hashes);
            return NULL;
        }
        Py_DECREF(item);
    }

    const char *tag_str = NULL;
    if (tag_obj != Py_None) {
        tag_str = cf_datum_get_cstring(tag_obj, "Fifth argument must be a string, bytes, or None");
        if (!tag_str) {
            DAP_DELETE(hashes);
            return NULL;
        }
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_tx_out_cond_t *item = dap_chain_datum_tx_item_out_cond_create_wallet_shared(
        srv_uid, value, (uint32_t)signs_min, hashes, (size_t)hashes_count, tag_str
    );

    DAP_DELETE(hashes);

    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_cond_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_obj = NULL;
    PyObject *value_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &addr_obj, &value_obj)) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_datum_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    dap_chain_tx_out_t *item = dap_chain_datum_tx_item_out_create(&addr, value);
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_ext_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_obj = NULL;
    PyObject *value_obj = NULL;
    PyObject *token_obj = NULL;

    if (!PyArg_ParseTuple(args, "OOO", &addr_obj, &value_obj, &token_obj)) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_datum_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    const char *token = cf_datum_get_cstring(token_obj, "Third argument must be a string or bytes");
    if (!token) {
        return NULL;
    }

    dap_chain_tx_out_ext_t *item = dap_chain_datum_tx_item_out_ext_create(&addr, value, token);
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_ext_t", NULL);
}

PyObject *dap_chain_datum_tx_item_out_std_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_obj = NULL;
    PyObject *value_obj = NULL;
    PyObject *token_obj = NULL;
    unsigned long long ts_unlock = 0;

    if (!PyArg_ParseTuple(args, "OOOK", &addr_obj, &value_obj, &token_obj, &ts_unlock)) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_datum_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    const char *token = cf_datum_get_cstring(token_obj, "Third argument must be a string or bytes");
    if (!token) {
        return NULL;
    }

    dap_chain_tx_out_std_t *item = dap_chain_datum_tx_item_out_std_create(
        &addr, value, token, (dap_time_t)ts_unlock
    );
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_out_std_t", NULL);
}

PyObject *dap_chain_datum_tx_item_sig_get_sign_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *sig_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &sig_obj)) {
        return NULL;
    }

    dap_chain_tx_sig_t *sig = (dap_chain_tx_sig_t *)cf_datum_get_capsule(
        sig_obj, "dap_chain_tx_sig_t", "First argument must be a tx sig capsule"
    );
    if (!sig) {
        return NULL;
    }

    dap_sign_t *sign = dap_chain_datum_tx_item_sig_get_sign(sig);
    if (!sign) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(sign, "dap_sign_t", NULL);
}

PyObject *dap_chain_datum_tx_item_sign_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *key_obj = NULL;
    PyObject *tx_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &key_obj, &tx_obj)) {
        return NULL;
    }

    dap_enc_key_t *key = (dap_enc_key_t *)cf_datum_get_capsule(
        key_obj, "dap_enc_key_t", "First argument must be a dap_enc_key_t capsule"
    );
    if (!key) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_datum_get_capsule(
        tx_obj, "dap_chain_datum_tx_t", "Second argument must be a tx datum capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_tx_sig_t *sig = dap_chain_datum_tx_item_sign_create(key, tx);
    if (!sig) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(sig, "dap_chain_tx_sig_t", NULL);
}

PyObject *dap_chain_datum_tx_item_sign_create_from_sign_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *sign_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &sign_obj)) {
        return NULL;
    }

    dap_sign_t *sign = (dap_sign_t *)cf_datum_get_capsule(
        sign_obj, "dap_sign_t", "First argument must be a dap_sign_t capsule"
    );
    if (!sign) {
        return NULL;
    }

    dap_chain_tx_sig_t *sig = dap_chain_datum_tx_item_sign_create_from_sign(sign);
    if (!sig) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(sig, "dap_chain_tx_sig_t", NULL);
}

PyObject *dap_chain_datum_tx_item_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *data_obj = NULL;
    int tsd_type = 0;
    Py_buffer view = {0};

    if (!PyArg_ParseTuple(args, "Oi", &data_obj, &tsd_type)) {
        return NULL;
    }

    if (PyObject_GetBuffer(data_obj, &view, PyBUF_SIMPLE) != 0) {
        PyErr_SetString(PyExc_TypeError, "Data must be bytes-like");
        return NULL;
    }
    if (view.len <= 0) {
        PyBuffer_Release(&view);
        PyErr_SetString(PyExc_ValueError, "Data must be non-empty");
        return NULL;
    }

    dap_chain_tx_tsd_t *item = dap_chain_datum_tx_item_tsd_create(view.buf, tsd_type, (size_t)view.len);
    PyBuffer_Release(&view);
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_tx_item_type_from_str_short_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *str_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &str_obj)) {
        return NULL;
    }

    const char *type_str = cf_datum_get_cstring(str_obj, "First argument must be a string or bytes");
    if (!type_str) {
        return NULL;
    }

    dap_chain_tx_item_type_t type = dap_chain_datum_tx_item_type_from_str_short(type_str);
    return PyLong_FromLong((long)type);
}

PyObject *dap_chain_datum_tx_item_type_to_str_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int type = 0;

    if (!PyArg_ParseTuple(args, "I", &type)) {
        return NULL;
    }

    const char *type_str = dap_chain_datum_tx_item_type_to_str((dap_chain_tx_item_type_t)type);
    return PyUnicode_FromString(type_str ? type_str : "UNDEFINED");
}

PyObject *dap_chain_datum_tx_item_type_to_str_short_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int type = 0;

    if (!PyArg_ParseTuple(args, "I", &type)) {
        return NULL;
    }

    const char *type_str = dap_chain_datum_tx_item_type_to_str_short((dap_chain_tx_item_type_t)type);
    return PyUnicode_FromString(type_str ? type_str : "UNDEFINED");
}

PyObject *dap_chain_datum_tx_sign_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *key_obj = NULL;
    PyObject *tx_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &key_obj, &tx_obj)) {
        return NULL;
    }

    dap_enc_key_t *key = (dap_enc_key_t *)cf_datum_get_capsule(
        key_obj, "dap_enc_key_t", "First argument must be a dap_enc_key_t capsule"
    );
    if (!key) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_datum_get_capsule(
        tx_obj, "dap_chain_datum_tx_t", "Second argument must be a tx datum capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_sign_t *sign = dap_chain_datum_tx_sign_create(key, tx);
    if (!sign) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(sign, "dap_sign_t", NULL);
}

PyObject *dap_chain_tx_event_copy_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *event_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &event_obj)) {
        return NULL;
    }

    dap_chain_tx_event_t *event = (dap_chain_tx_event_t *)cf_datum_get_capsule(
        event_obj, "dap_chain_tx_event_t", "First argument must be a tx event capsule"
    );
    if (!event) {
        return NULL;
    }

    dap_chain_tx_event_t *copy = dap_chain_tx_event_copy(event);
    if (!copy) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(copy, "dap_chain_tx_event_t", NULL);
}

PyObject *dap_chain_tx_event_delete_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *event_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &event_obj)) {
        return NULL;
    }

    dap_chain_tx_event_t *event = (dap_chain_tx_event_t *)cf_datum_get_capsule(
        event_obj, "dap_chain_tx_event_t", "First argument must be a tx event capsule"
    );
    if (!event) {
        return NULL;
    }

    dap_chain_tx_event_delete(event);
    Py_RETURN_NONE;
}

PyObject *dap_chain_tx_out_cond_subtype_from_str_short_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *str_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &str_obj)) {
        return NULL;
    }

    const char *subtype_str = cf_datum_get_cstring(str_obj, "First argument must be a string or bytes");
    if (!subtype_str) {
        return NULL;
    }

    dap_chain_tx_out_cond_subtype_t subtype = dap_chain_tx_out_cond_subtype_from_str_short(subtype_str);
    return PyLong_FromLong((long)subtype);
}

PyObject *dap_chain_tx_sig_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *sign_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &sign_obj)) {
        return NULL;
    }

    dap_sign_t *sign = (dap_sign_t *)cf_datum_get_capsule(
        sign_obj, "dap_sign_t", "First argument must be a dap_sign_t capsule"
    );
    if (!sign) {
        return NULL;
    }

    dap_chain_tx_sig_t *sig = dap_chain_tx_sig_create(sign);
    if (!sig) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(sig, "dap_chain_tx_sig_t", NULL);
}

PyObject *dap_chain_tx_out_cond_subtype_to_str_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int subtype = 0;

    if (!PyArg_ParseTuple(args, "I", &subtype)) {
        return NULL;
    }

    const char *subtype_str = dap_chain_tx_out_cond_subtype_to_str((dap_chain_tx_out_cond_subtype_t)subtype);
    return PyUnicode_FromString(subtype_str ? subtype_str : "UNDEFINED");
}

PyObject *dap_chain_tx_out_cond_subtype_to_str_short_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int subtype = 0;

    if (!PyArg_ParseTuple(args, "I", &subtype)) {
        return NULL;
    }

    const char *subtype_str = dap_chain_tx_out_cond_subtype_to_str_short((dap_chain_tx_out_cond_subtype_t)subtype);
    return PyUnicode_FromString(subtype_str ? subtype_str : "UNDEFINED");
}

PyObject *dap_chain_datum_tx_receipt_check_size_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *receipt_obj = NULL;
    Py_ssize_t control_size = 0;

    if (!PyArg_ParseTuple(args, "On", &receipt_obj, &control_size)) {
        return NULL;
    }
    if (control_size < 0) {
        PyErr_SetString(PyExc_ValueError, "control_size must be non-negative");
        return NULL;
    }

    dap_chain_datum_tx_receipt_t *receipt = (dap_chain_datum_tx_receipt_t *)cf_datum_get_capsule(
        receipt_obj, "dap_chain_datum_tx_receipt_t", "First argument must be a tx receipt capsule"
    );
    if (!receipt) {
        return NULL;
    }

    int rc = dap_chain_datum_tx_receipt_check_size(receipt, (size_t)control_size);
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_datum_tx_receipt_create_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long srv_uid_uint = 0;
    unsigned int unit_enm = 0;
    unsigned long long units = 0;
    PyObject *value_obj = NULL;
    PyObject *ext_obj = Py_None;
    PyObject *hash_obj = NULL;
    Py_buffer ext_view = {0};
    bool has_view = false;

    if (!PyArg_ParseTuple(args, "KIKOOO", &srv_uid_uint, &unit_enm, &units,
                          &value_obj, &ext_obj, &hash_obj)) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_datum_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    dap_hash_fast_t prev_hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &prev_hash) != 0) {
        return NULL;
    }

    const void *ext_ptr = NULL;
    size_t ext_size = 0;
    if (ext_obj != Py_None) {
        if (PyObject_GetBuffer(ext_obj, &ext_view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "Extension must be bytes-like or None");
            return NULL;
        }
        ext_ptr = ext_view.buf;
        ext_size = (size_t)ext_view.len;
        has_view = true;
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_net_srv_price_unit_uid_t units_type = {.enm = (dap_chain_srv_unit_enum_t)unit_enm};
    dap_chain_datum_tx_receipt_t *receipt = dap_chain_datum_tx_receipt_create(
        srv_uid, units_type, (uint64_t)units, value, ext_ptr, ext_size, &prev_hash
    );

    if (has_view) {
        PyBuffer_Release(&ext_view);
    }

    if (!receipt) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(receipt, "dap_chain_datum_tx_receipt_t", NULL);
}

PyObject *dap_chain_datum_tx_receipt_sign_add_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *receipt_obj = NULL;
    PyObject *key_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &receipt_obj, &key_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_receipt_t *receipt = (dap_chain_datum_tx_receipt_t *)cf_datum_get_capsule(
        receipt_obj, "dap_chain_datum_tx_receipt_t", "First argument must be a tx receipt capsule"
    );
    if (!receipt) {
        return NULL;
    }

    dap_enc_key_t *key = (dap_enc_key_t *)cf_datum_get_capsule(
        key_obj, "dap_enc_key_t", "Second argument must be a dap_enc_key_t capsule"
    );
    if (!key) {
        return NULL;
    }

    dap_chain_datum_tx_receipt_t *result = dap_chain_datum_tx_receipt_sign_add(receipt, key);
    if (!result) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(result, "dap_chain_datum_tx_receipt_t", NULL);
}

PyObject *dap_chain_datum_tx_receipt_sign_get_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *receipt_obj = NULL;
    Py_ssize_t receipt_size = 0;
    unsigned int sign_position = 0;

    if (!PyArg_ParseTuple(args, "OnI", &receipt_obj, &receipt_size, &sign_position)) {
        return NULL;
    }
    if (receipt_size < 0) {
        PyErr_SetString(PyExc_ValueError, "receipt_size must be non-negative");
        return NULL;
    }

    dap_chain_datum_tx_receipt_t *receipt = (dap_chain_datum_tx_receipt_t *)cf_datum_get_capsule(
        receipt_obj, "dap_chain_datum_tx_receipt_t", "First argument must be a tx receipt capsule"
    );
    if (!receipt) {
        return NULL;
    }

    dap_sign_t *sign = dap_chain_datum_tx_receipt_sign_get(
        receipt, (size_t)receipt_size, (uint16_t)sign_position
    );
    if (!sign) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(sign, "dap_sign_t", NULL);
}

PyObject *dap_chain_datum_tx_receipt_signs_count_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *receipt_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &receipt_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_receipt_t *receipt = (dap_chain_datum_tx_receipt_t *)cf_datum_get_capsule(
        receipt_obj, "dap_chain_datum_tx_receipt_t", "First argument must be a tx receipt capsule"
    );
    if (!receipt) {
        return NULL;
    }

    uint16_t count = dap_chain_datum_tx_receipt_signs_count(receipt);
    return PyLong_FromUnsignedLong((unsigned long)count);
}

PyObject *dap_chain_datum_tx_receipt_srv_uid_get_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *receipt_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &receipt_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_receipt_t *receipt = (dap_chain_datum_tx_receipt_t *)cf_datum_get_capsule(
        receipt_obj, "dap_chain_datum_tx_receipt_t", "First argument must be a tx receipt capsule"
    );
    if (!receipt) {
        return NULL;
    }

    uint64_t srv_uid = dap_chain_datum_tx_receipt_srv_uid_get(receipt);
    return PyLong_FromUnsignedLongLong(srv_uid);
}

PyObject *dap_chain_datum_tx_receipt_units_get_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *receipt_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &receipt_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_receipt_t *receipt = (dap_chain_datum_tx_receipt_t *)cf_datum_get_capsule(
        receipt_obj, "dap_chain_datum_tx_receipt_t", "First argument must be a tx receipt capsule"
    );
    if (!receipt) {
        return NULL;
    }

    uint64_t units = dap_chain_datum_tx_receipt_units_get(receipt);
    return PyLong_FromUnsignedLongLong(units);
}

PyObject *dap_chain_datum_tx_receipt_utype_get_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *receipt_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &receipt_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_receipt_t *receipt = (dap_chain_datum_tx_receipt_t *)cf_datum_get_capsule(
        receipt_obj, "dap_chain_datum_tx_receipt_t", "First argument must be a tx receipt capsule"
    );
    if (!receipt) {
        return NULL;
    }

    uint32_t utype = dap_chain_datum_tx_receipt_utype_get(receipt);
    return PyLong_FromUnsignedLong((unsigned long)utype);
}

PyObject *dap_chain_datum_tx_receipt_value_get_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *receipt_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &receipt_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_receipt_t *receipt = (dap_chain_datum_tx_receipt_t *)cf_datum_get_capsule(
        receipt_obj, "dap_chain_datum_tx_receipt_t", "First argument must be a tx receipt capsule"
    );
    if (!receipt) {
        return NULL;
    }

    uint256_t value = dap_chain_datum_tx_receipt_value_get(receipt);
    return PyBytes_FromStringAndSize((const char *)&value, sizeof(uint256_t));
}

PyObject *dap_chain_datum_tx_item_vote_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *hash_obj = NULL;
    unsigned long long answer_idx = 0;

    if (!PyArg_ParseTuple(args, "OK", &hash_obj, &answer_idx)) {
        return NULL;
    }

    dap_hash_fast_t voting_hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &voting_hash) != 0) {
        return NULL;
    }

    uint64_t answer_idx_value = (uint64_t)answer_idx;
    dap_chain_tx_vote_t *item = dap_chain_datum_tx_item_vote_create(&voting_hash, &answer_idx_value);
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_vote_t", NULL);
}

PyObject *dap_chain_datum_tx_item_vote_to_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *vote_obj = NULL;
    int version = 0;

    if (!PyArg_ParseTuple(args, "Oi", &vote_obj, &version)) {
        return NULL;
    }

    dap_chain_tx_vote_t *vote = (dap_chain_tx_vote_t *)cf_datum_get_capsule(
        vote_obj, "dap_chain_tx_vote_t", "First argument must be a tx vote capsule"
    );
    if (!vote) {
        return NULL;
    }

    dap_json_t *json = dap_chain_datum_tx_item_vote_to_json(vote, version);
    if (!json) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(json, "dap_json_t", NULL);
}

PyObject *dap_chain_datum_tx_item_voting_create_py(PyObject *self, PyObject *args) {
    (void)self;
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }

    dap_chain_tx_voting_t *item = dap_chain_datum_tx_item_voting_create();
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_voting_t", NULL);
}

PyObject *dap_chain_datum_tx_item_voting_tsd_to_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_obj = NULL;
    int version = 0;

    if (!PyArg_ParseTuple(args, "Oi", &tx_obj, &version)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_datum_get_capsule(
        tx_obj, "dap_chain_datum_tx_t", "First argument must be a tx datum capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_json_t *json = dap_chain_datum_tx_item_voting_tsd_to_json(tx, version);
    if (!json) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(json, "dap_json_t", NULL);
}

PyObject *dap_chain_datum_tx_voting_get_answer_text_by_idx_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_obj = NULL;
    unsigned long long answer_idx = 0;

    if (!PyArg_ParseTuple(args, "OK", &tx_obj, &answer_idx)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_datum_get_capsule(
        tx_obj, "dap_chain_datum_tx_t", "First argument must be a tx datum capsule"
    );
    if (!tx) {
        return NULL;
    }

    char *answer = dap_chain_datum_tx_voting_get_answer_text_by_idx(tx, (uint64_t)answer_idx);
    if (!answer) {
        Py_RETURN_NONE;
    }

    PyObject *answer_obj = PyUnicode_FromString(answer);
    DAP_DELETE(answer);
    return answer_obj;
}

PyObject *dap_chain_datum_tx_voting_params_delete_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *params_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &params_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_voting_params_t *params = (dap_chain_datum_tx_voting_params_t *)cf_datum_get_capsule(
        params_obj, "dap_chain_datum_tx_voting_params_t", "First argument must be a voting params capsule"
    );
    if (!params) {
        return NULL;
    }

    dap_chain_datum_tx_voting_params_delete(params);
    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_tx_voting_parse_tsd_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_datum_get_capsule(
        tx_obj, "dap_chain_datum_tx_t", "First argument must be a tx datum capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_datum_tx_voting_params_t *params = dap_chain_datum_tx_voting_parse_tsd(tx);
    if (!params) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(params, "dap_chain_datum_tx_voting_params_t", NULL);
}

PyObject *dap_chain_datum_voting_answer_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *answer = NULL;
    Py_ssize_t answer_len = 0;

    if (!PyArg_ParseTuple(args, "s#", &answer, &answer_len)) {
        return NULL;
    }
    if (answer_len <= 0) {
        PyErr_SetString(PyExc_ValueError, "answer must be non-empty");
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_answer_tsd_create(answer, (size_t)answer_len);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_cancel_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *hash_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &hash_obj)) {
        return NULL;
    }

    dap_hash_fast_t voting_hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &voting_hash) != 0) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_cancel_tsd_create(voting_hash);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_delegated_key_required_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    int delegated_required = 0;

    if (!PyArg_ParseTuple(args, "p", &delegated_required)) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_delegated_key_required_tsd_create(
        delegated_required ? true : false
    );
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_expire_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long expire = 0;

    if (!PyArg_ParseTuple(args, "K", &expire)) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_expire_tsd_create((dap_time_t)expire);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_max_votes_count_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long max_count = 0;

    if (!PyArg_ParseTuple(args, "K", &max_count)) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_max_votes_count_tsd_create((uint64_t)max_count);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_question_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *question = NULL;
    Py_ssize_t question_len = 0;

    if (!PyArg_ParseTuple(args, "s#", &question, &question_len)) {
        return NULL;
    }
    if (question_len <= 0) {
        PyErr_SetString(PyExc_ValueError, "question must be non-empty");
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_question_tsd_create(question, (size_t)question_len);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_token_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *token_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &token_obj)) {
        return NULL;
    }

    const char *token = cf_datum_get_cstring(token_obj, "First argument must be a string or bytes");
    if (!token) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_token_tsd_create(token);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_vote_changing_allowed_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    int changing_allowed = 0;

    if (!PyArg_ParseTuple(args, "p", &changing_allowed)) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_vote_changing_allowed_tsd_create(
        changing_allowed ? true : false
    );
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_vote_tx_cond_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *hash_obj = NULL;
    int out_idx = 0;

    if (!PyArg_ParseTuple(args, "Oi", &hash_obj, &out_idx)) {
        return NULL;
    }

    dap_hash_fast_t tx_hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &tx_hash) != 0) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_vote_tx_cond_tsd_create(tx_hash, out_idx);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

int cellframe_datum_init(PyObject *module) {
    static PyMethodDef datum_methods[] = {
        {"dap_chain_datum_calc_hash", dap_chain_datum_calc_hash_py, METH_VARARGS,
         "Calculate datum hash"},
        {"dap_chain_datum_create", dap_chain_datum_create_py, METH_VARARGS,
         "Create datum from type and data"},
        {"dap_chain_datum_dump_tx_json", dap_chain_datum_dump_tx_json_py, METH_VARARGS,
         "Dump transaction datum to JSON"},
        {"dap_chain_datum_register_dump_anchor_callback", dap_chain_datum_register_dump_anchor_callback_py, METH_VARARGS,
         "Register datum anchor JSON dump callback"},
        {"dap_chain_datum_register_dump_decree_callback", dap_chain_datum_register_dump_decree_callback_py, METH_VARARGS,
         "Register datum decree JSON dump callback"},
        {"dap_chain_datum_size", dap_chain_datum_size_py, METH_VARARGS,
         "Get datum size"},
        {"dap_chain_datum_type_id_to_str", dap_chain_datum_type_id_to_str_py, METH_VARARGS,
         "Convert datum type id to string"},
        {"dap_datum_token_dump_tsd_to_json", dap_datum_token_dump_tsd_to_json_py, METH_VARARGS,
         "Dump token TSD to JSON"},
        {"dap_chain_datum_token_flag_to_str", dap_chain_datum_token_flag_to_str_py, METH_VARARGS,
         "Convert token flag to string"},
        {"dap_chain_datum_token_flag_from_str", dap_chain_datum_token_flag_from_str_py, METH_VARARGS,
         "Convert token flag from string"},
        {"dap_chain_datum_token_get_delegated_ticker", dap_chain_datum_token_get_delegated_ticker_py, METH_VARARGS,
         "Get delegated ticker from token ticker"},
        {"dap_chain_datum_token_is_old", dap_chain_datum_token_is_old_py, METH_VARARGS,
         "Check if token type is old"},
        {"dap_chain_datum_token_check_ticker", dap_chain_datum_token_check_ticker_py, METH_VARARGS,
         "Check token ticker validity"},
        {"dap_chain_datum_token_flags_dump_to_json", dap_chain_datum_token_flags_dump_to_json_py, METH_VARARGS,
         "Dump token flags to JSON"},
        {"dap_chain_datum_token_certs_dump", dap_chain_datum_token_certs_dump_py, METH_VARARGS,
         "Dump token certificates to string"},
        {"dap_chain_datum_token_certs_dump_to_json", dap_chain_datum_token_certs_dump_to_json_py, METH_VARARGS,
         "Dump token certificates to JSON"},
        {"dap_chain_datum_token_read", dap_chain_datum_token_read_py, METH_VARARGS,
         "Read token datum from serialized data"},
        {"dap_chain_datum_token_tsd_get", dap_chain_datum_token_tsd_get_py, METH_VARARGS,
         "Get token TSD pointer"},
        {"dap_chain_datum_emission_type_str", dap_chain_datum_emission_type_str_py, METH_VARARGS,
         "Convert emission type to string"},
        {"dap_chain_datum_emission_create", dap_chain_datum_emission_create_py, METH_VARARGS,
         "Create token emission"},
        {"dap_chain_datum_emission_add_tsd", dap_chain_datum_emission_add_tsd_py, METH_VARARGS,
         "Add TSD to emission"},
        {"dap_chain_emission_get_tsd", dap_chain_emission_get_tsd_py, METH_VARARGS,
         "Get emission TSD data"},
        {"dap_chain_datum_emission_get_size", dap_chain_datum_emission_get_size_py, METH_VARARGS,
         "Get emission size from serialized data"},
        {"dap_chain_datum_emission_read", dap_chain_datum_emission_read_py, METH_VARARGS,
         "Read emission from serialized data"},
        {"dap_chain_datum_emission_add_sign", dap_chain_datum_emission_add_sign_py, METH_VARARGS,
         "Add signature to emission"},
        {"dap_chain_datum_emission_append_sign", dap_chain_datum_emission_append_sign_py, METH_VARARGS,
         "Append signature to emission"},
        {"dap_chain_datum_emission_get_signs", dap_chain_datum_emission_get_signs_py, METH_VARARGS,
         "Get emission signatures"},
        {"dap_chain_datum_anchor_certs_dump", dap_chain_datum_anchor_certs_dump_py, METH_VARARGS,
         "Dump anchor certificates to string"},
        {"dap_chain_datum_anchor_certs_dump_json", dap_chain_datum_anchor_certs_dump_json_py, METH_VARARGS,
         "Dump anchor certificates to JSON"},
        {"dap_chain_datum_anchor_get_hash_from_data", dap_chain_datum_anchor_get_hash_from_data_py, METH_VARARGS,
         "Get anchor hash from data"},
        {"dap_chain_datum_anchor_get_size", dap_chain_datum_anchor_get_size_py, METH_VARARGS,
         "Get anchor datum size"},
        {"dap_chain_datum_from_tx", dap_chain_datum_from_tx_py, METH_VARARGS,
         "Convert transaction datum to datum"},
        {"dap_chain_datum_tx_event_to_json", dap_chain_datum_tx_event_to_json_py, METH_VARARGS,
         "Dump tx event to JSON"},
        {"dap_chain_datum_tx_item_event_to_json", dap_chain_datum_tx_item_event_to_json_py, METH_VARARGS,
         "Dump tx item event to JSON"},
        {"dap_chain_tx_item_event_type_from_str", dap_chain_tx_item_event_type_from_str_py, METH_VARARGS,
         "Convert tx event type from string"},
        {"dap_chain_tx_item_event_type_to_str", dap_chain_tx_item_event_type_to_str_py, METH_VARARGS,
         "Convert tx event type to string"},
        {"dap_chain_datum_item_tx_get_size", dap_chain_datum_item_tx_get_size_py, METH_VARARGS,
         "Get transaction item size"},
        {"dap_chain_datum_tx_event_create", dap_chain_datum_tx_event_create_py, METH_VARARGS,
         "Create tx event item"},
        {"dap_chain_datum_tx_item_get_data", dap_chain_datum_tx_item_get_data_py, METH_VARARGS,
         "Get tx item data"},
        {"dap_chain_datum_tx_item_get_tsd_by_type", dap_chain_datum_tx_item_get_tsd_by_type_py, METH_VARARGS,
         "Get tx TSD by type"},
        {"dap_chain_datum_tx_item_in_cond_create", dap_chain_datum_tx_item_in_cond_create_py, METH_VARARGS,
         "Create tx in_cond item"},
        {"dap_chain_datum_tx_item_in_create", dap_chain_datum_tx_item_in_create_py, METH_VARARGS,
         "Create tx in item"},
        {"dap_chain_datum_tx_item_in_ems_create", dap_chain_datum_tx_item_in_ems_create_py, METH_VARARGS,
         "Create tx in_ems item"},
        {"dap_chain_datum_tx_item_in_reward_create", dap_chain_datum_tx_item_in_reward_create_py, METH_VARARGS,
         "Create tx in_reward item"},
        {"dap_chain_datum_tx_item_out_cond_create_fee", dap_chain_datum_tx_item_out_cond_create_fee_py, METH_VARARGS,
         "Create out_cond fee item"},
        {"dap_chain_datum_tx_item_out_cond_create_srv_pay", dap_chain_datum_tx_item_out_cond_create_srv_pay_py, METH_VARARGS,
         "Create out_cond service pay item"},
        {"dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash", dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash_py, METH_VARARGS,
         "Create out_cond service pay item with hash"},
        {"dap_chain_datum_tx_item_out_cond_create_srv_stake", dap_chain_datum_tx_item_out_cond_create_srv_stake_py, METH_VARARGS,
         "Create out_cond stake item"},
        {"dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate", dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate_py, METH_VARARGS,
         "Create out_cond stake delegate item"},
        {"dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock", dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock_py, METH_VARARGS,
         "Create out_cond stake ext lock item"},
        {"dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size", dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size_py, METH_VARARGS,
         "Get out_cond stake TSD size"},
        {"dap_chain_datum_tx_item_out_cond_create_srv_stake_lock", dap_chain_datum_tx_item_out_cond_create_srv_stake_lock_py, METH_VARARGS,
         "Create out_cond stake lock item"},
        {"dap_chain_datum_tx_item_out_cond_create_srv_xchange", dap_chain_datum_tx_item_out_cond_create_srv_xchange_py, METH_VARARGS,
         "Create out_cond xchange item"},
        {"dap_chain_datum_tx_item_out_cond_create_wallet_shared", dap_chain_datum_tx_item_out_cond_create_wallet_shared_py, METH_VARARGS,
         "Create out_cond wallet shared item"},
        {"dap_chain_datum_tx_item_out_create", dap_chain_datum_tx_item_out_create_py, METH_VARARGS,
         "Create out item"},
        {"dap_chain_datum_tx_item_out_ext_create", dap_chain_datum_tx_item_out_ext_create_py, METH_VARARGS,
         "Create out_ext item"},
        {"dap_chain_datum_tx_item_out_std_create", dap_chain_datum_tx_item_out_std_create_py, METH_VARARGS,
         "Create out_std item"},
        {"dap_chain_datum_tx_item_sig_get_sign", dap_chain_datum_tx_item_sig_get_sign_py, METH_VARARGS,
         "Get sign from tx sig item"},
        {"dap_chain_datum_tx_item_sign_create", dap_chain_datum_tx_item_sign_create_py, METH_VARARGS,
         "Create tx sig item"},
        {"dap_chain_datum_tx_item_sign_create_from_sign", dap_chain_datum_tx_item_sign_create_from_sign_py, METH_VARARGS,
         "Create tx sig item from sign"},
        {"dap_chain_datum_tx_item_tsd_create", dap_chain_datum_tx_item_tsd_create_py, METH_VARARGS,
         "Create tx TSD item"},
        {"dap_chain_datum_tx_item_type_from_str_short", dap_chain_datum_tx_item_type_from_str_short_py, METH_VARARGS,
         "Convert tx item type from short string"},
        {"dap_chain_datum_tx_item_type_to_str", dap_chain_datum_tx_item_type_to_str_py, METH_VARARGS,
         "Convert tx item type to string"},
        {"dap_chain_datum_tx_item_type_to_str_short", dap_chain_datum_tx_item_type_to_str_short_py, METH_VARARGS,
         "Convert tx item type to short string"},
        {"dap_chain_datum_tx_sign_create", dap_chain_datum_tx_sign_create_py, METH_VARARGS,
         "Create tx signature"},
        {"dap_chain_tx_event_copy", dap_chain_tx_event_copy_py, METH_VARARGS,
         "Copy tx event"},
        {"dap_chain_tx_event_delete", dap_chain_tx_event_delete_py, METH_VARARGS,
         "Delete tx event"},
        {"dap_chain_tx_out_cond_subtype_from_str_short", dap_chain_tx_out_cond_subtype_from_str_short_py, METH_VARARGS,
         "Convert out_cond subtype from short string"},
        {"dap_chain_tx_sig_create", dap_chain_tx_sig_create_py, METH_VARARGS,
         "Create tx sig item"},
        {"dap_chain_tx_out_cond_subtype_to_str", dap_chain_tx_out_cond_subtype_to_str_py, METH_VARARGS,
         "Convert out_cond subtype to string"},
        {"dap_chain_tx_out_cond_subtype_to_str_short", dap_chain_tx_out_cond_subtype_to_str_short_py, METH_VARARGS,
         "Convert out_cond subtype to short string"},
        {"dap_chain_datum_tx_receipt_check_size", dap_chain_datum_tx_receipt_check_size_py, METH_VARARGS,
         "Check tx receipt size"},
        {"dap_chain_datum_tx_receipt_create", dap_chain_datum_tx_receipt_create_py, METH_VARARGS,
         "Create tx receipt"},
        {"dap_chain_datum_tx_receipt_sign_add", dap_chain_datum_tx_receipt_sign_add_py, METH_VARARGS,
         "Add signature to tx receipt"},
        {"dap_chain_datum_tx_receipt_sign_get", dap_chain_datum_tx_receipt_sign_get_py, METH_VARARGS,
         "Get tx receipt signature"},
        {"dap_chain_datum_tx_receipt_signs_count", dap_chain_datum_tx_receipt_signs_count_py, METH_VARARGS,
         "Get tx receipt signatures count"},
        {"dap_chain_datum_tx_receipt_srv_uid_get", dap_chain_datum_tx_receipt_srv_uid_get_py, METH_VARARGS,
         "Get tx receipt service uid"},
        {"dap_chain_datum_tx_receipt_units_get", dap_chain_datum_tx_receipt_units_get_py, METH_VARARGS,
         "Get tx receipt units"},
        {"dap_chain_datum_tx_receipt_utype_get", dap_chain_datum_tx_receipt_utype_get_py, METH_VARARGS,
         "Get tx receipt unit type"},
        {"dap_chain_datum_tx_receipt_value_get", dap_chain_datum_tx_receipt_value_get_py, METH_VARARGS,
         "Get tx receipt value"},
        {"dap_chain_datum_tx_item_vote_create", dap_chain_datum_tx_item_vote_create_py, METH_VARARGS,
         "Create tx vote item"},
        {"dap_chain_datum_tx_item_vote_to_json", dap_chain_datum_tx_item_vote_to_json_py, METH_VARARGS,
         "Convert tx vote to JSON"},
        {"dap_chain_datum_tx_item_voting_create", dap_chain_datum_tx_item_voting_create_py, METH_VARARGS,
         "Create tx voting item"},
        {"dap_chain_datum_tx_item_voting_tsd_to_json", dap_chain_datum_tx_item_voting_tsd_to_json_py, METH_VARARGS,
         "Convert tx voting TSD to JSON"},
        {"dap_chain_datum_tx_voting_get_answer_text_by_idx", dap_chain_datum_tx_voting_get_answer_text_by_idx_py, METH_VARARGS,
         "Get voting answer text by index"},
        {"dap_chain_datum_tx_voting_params_delete", dap_chain_datum_tx_voting_params_delete_py, METH_VARARGS,
         "Delete voting params"},
        {"dap_chain_datum_tx_voting_parse_tsd", dap_chain_datum_tx_voting_parse_tsd_py, METH_VARARGS,
         "Parse voting params from tx"},
        {"dap_chain_datum_voting_answer_tsd_create", dap_chain_datum_voting_answer_tsd_create_py, METH_VARARGS,
         "Create voting answer TSD"},
        {"dap_chain_datum_voting_cancel_tsd_create", dap_chain_datum_voting_cancel_tsd_create_py, METH_VARARGS,
         "Create voting cancel TSD"},
        {"dap_chain_datum_voting_delegated_key_required_tsd_create", dap_chain_datum_voting_delegated_key_required_tsd_create_py, METH_VARARGS,
         "Create voting delegated key required TSD"},
        {"dap_chain_datum_voting_expire_tsd_create", dap_chain_datum_voting_expire_tsd_create_py, METH_VARARGS,
         "Create voting expire TSD"},
        {"dap_chain_datum_voting_max_votes_count_tsd_create", dap_chain_datum_voting_max_votes_count_tsd_create_py, METH_VARARGS,
         "Create voting max votes count TSD"},
        {"dap_chain_datum_voting_question_tsd_create", dap_chain_datum_voting_question_tsd_create_py, METH_VARARGS,
         "Create voting question TSD"},
        {"dap_chain_datum_voting_token_tsd_create", dap_chain_datum_voting_token_tsd_create_py, METH_VARARGS,
         "Create voting token TSD"},
        {"dap_chain_datum_voting_vote_changing_allowed_tsd_create", dap_chain_datum_voting_vote_changing_allowed_tsd_create_py, METH_VARARGS,
         "Create voting vote changing allowed TSD"},
        {"dap_chain_datum_voting_vote_tx_cond_tsd_create", dap_chain_datum_voting_vote_tx_cond_tsd_create_py, METH_VARARGS,
         "Create voting vote tx cond TSD"},
        {"dap_chain_datum_decree_certs_dump_json", dap_chain_datum_decree_certs_dump_json_py, METH_VARARGS,
         "Dump decree certificates to JSON"},
        {"dap_chain_datum_decree_dump_json", dap_chain_datum_decree_dump_json_py, METH_VARARGS,
         "Dump decree to JSON"},
        {"dap_chain_datum_decree_get_action", dap_chain_datum_decree_get_action_py, METH_VARARGS,
         "Get decree action"},
        {"dap_chain_datum_decree_get_atom_num", dap_chain_datum_decree_get_atom_num_py, METH_VARARGS,
         "Get decree atom number"},
        {"dap_chain_datum_decree_get_ban_addr", dap_chain_datum_decree_get_ban_addr_py, METH_VARARGS,
         "Get decree ban address"},
        {"dap_chain_datum_decree_get_empty_block_every_times", dap_chain_datum_decree_get_empty_block_every_times_py, METH_VARARGS,
         "Get decree empty block generation period"},
        {"dap_chain_datum_decree_get_fee", dap_chain_datum_decree_get_fee_py, METH_VARARGS,
         "Get decree fee value"},
        {"dap_chain_datum_decree_get_fee_addr", dap_chain_datum_decree_get_fee_addr_py, METH_VARARGS,
         "Get decree fee address"},
        {"dap_chain_datum_decree_get_hash", dap_chain_datum_decree_get_hash_py, METH_VARARGS,
         "Get decree hash"},
        {"dap_chain_datum_decree_get_min_owners", dap_chain_datum_decree_get_min_owners_py, METH_VARARGS,
         "Get decree minimum owners"},
        {"dap_chain_datum_decree_get_node_addr", dap_chain_datum_decree_get_node_addr_py, METH_VARARGS,
         "Get decree node address"},
        {"dap_chain_datum_decree_get_owners", dap_chain_datum_decree_get_owners_py, METH_VARARGS,
         "Get decree owners list"},
        {"dap_chain_datum_decree_get_pkey", dap_chain_datum_decree_get_pkey_py, METH_VARARGS,
         "Get decree public key"},
        {"dap_chain_datum_decree_get_policy", dap_chain_datum_decree_get_policy_py, METH_VARARGS,
         "Get decree policy"},
        {"dap_chain_datum_decree_get_signature_type", dap_chain_datum_decree_get_signature_type_py, METH_VARARGS,
         "Get decree signature type"},
        {"dap_chain_datum_decree_get_signs", dap_chain_datum_decree_get_signs_py, METH_VARARGS,
         "Get decree signs"},
        {"dap_chain_datum_decree_get_size", dap_chain_datum_decree_get_size_py, METH_VARARGS,
         "Get decree size"},
        {"dap_chain_datum_decree_get_stake_min_signers_count", dap_chain_datum_decree_get_stake_min_signers_count_py, METH_VARARGS,
         "Get decree stake min signers count"},
        {"dap_chain_datum_decree_get_stake_min_value", dap_chain_datum_decree_get_stake_min_value_py, METH_VARARGS,
         "Get decree stake min value"},
        {"dap_chain_datum_decree_get_stake_signing_addr", dap_chain_datum_decree_get_stake_signing_addr_py, METH_VARARGS,
         "Get decree stake signing address"},
        {"dap_chain_datum_decree_get_stake_value", dap_chain_datum_decree_get_stake_value_py, METH_VARARGS,
         "Get decree stake value"},
        {"dap_chain_datum_decree_get_value", dap_chain_datum_decree_get_value_py, METH_VARARGS,
         "Get decree value"},
        {"dap_chain_datum_decree_new", dap_chain_datum_decree_new_py, METH_VARARGS,
         "Create new decree"},
        {"dap_chain_datum_decree_sign_in_cycle", dap_chain_datum_decree_sign_in_cycle_py, METH_VARARGS,
         "Sign decree in cycle"},
        {"dap_chain_datum_decree_subtype_to_str", dap_chain_datum_decree_subtype_to_str_py, METH_VARARGS,
         "Convert decree subtype to string"},
        {"dap_chain_datum_decree_tsd_type_to_str", dap_chain_datum_decree_tsd_type_to_str_py, METH_VARARGS,
         "Convert decree TSD type to string"},
        {"dap_chain_datum_decree_type_from_str", dap_chain_datum_decree_type_from_str_py, METH_VARARGS,
         "Convert decree type from string"},
        {"dap_chain_decree_registry_deinit", dap_chain_decree_registry_deinit_py, METH_VARARGS,
         "Deinitialize decree registry"},
        {"dap_chain_decree_registry_has_handler", dap_chain_decree_registry_has_handler_py, METH_VARARGS,
         "Check decree registry handler"},
        {"dap_chain_decree_registry_init", dap_chain_decree_registry_init_py, METH_VARARGS,
         "Initialize decree registry"},
        {"dap_chain_decree_registry_process", dap_chain_decree_registry_process_py, METH_VARARGS,
         "Process decree via registry"},
        {"dap_chain_decree_registry_register_handler", dap_chain_decree_registry_register_handler_py, METH_VARARGS,
         "Register decree handler"},
        {"dap_chain_decree_registry_unregister_handler", dap_chain_decree_registry_unregister_handler_py, METH_VARARGS,
         "Unregister decree handler"},
        {NULL, NULL, 0, NULL}
    };

    for (PyMethodDef *method = datum_methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
        if (!func) {
            return -1;
        }
        if (PyModule_AddObject(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            return -1;
        }
    }

    return 0;
}
