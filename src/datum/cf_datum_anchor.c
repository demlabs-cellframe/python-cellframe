#include "cf_datum_internal.h"
#include "dap_chain_datum_anchor.h"
#include "dap_hash.h"
#include "dap_json.h"
#include "dap_string.h"
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
