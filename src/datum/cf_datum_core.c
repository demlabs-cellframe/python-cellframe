#include "cf_datum_internal.h"
#include "dap_chain_datum.h"
#include "dap_chain_datum_tx.h"
#include "dap_hash.h"
#include "dap_json.h"
#include "../common/cf_callbacks_registry.h"

#define LOG_TAG "python_cellframe_datum"

typedef struct {
    PyObject *callback;
} python_datum_dump_ctx_t;

static python_datum_dump_ctx_t *s_datum_dump_decree_ctx = NULL;
static python_datum_dump_ctx_t *s_datum_dump_anchor_ctx = NULL;

static void cf_datum_clear_ctx(python_datum_dump_ctx_t **ctx_ptr) {
    if (!ctx_ptr || !*ctx_ptr) {
        return;
    }
    cf_callbacks_registry_remove(*ctx_ptr);
    DAP_DELETE(*ctx_ptr);
    *ctx_ptr = NULL;
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
