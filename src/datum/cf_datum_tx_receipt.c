#include "cf_datum_internal.h"
#include "dap_chain_datum_tx_receipt.h"
#include "dap_enc_key.h"
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
