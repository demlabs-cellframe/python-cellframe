#include "cf_datum_internal.h"
#include "dap_chain_datum.h"
#include "dap_chain_datum_token.h"
#include "dap_enc_key.h"
#include "dap_json.h"
#include "dap_string.h"
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
