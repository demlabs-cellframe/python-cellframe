/*
 * Authors:
 * Cellframe      https://cellframe.net
 * DeM Labs Inc.  https://demlabs.net
 * Sources:       https://gitlab.demlabs.net/cellframe/python-cellframe
 */

#include "../../include/cf_tx.h"
#include "dap_chain_datum_tx.h"
#include "dap_chain_datum_tx_create.h"
#include "dap_chain_tx.h"
#include "dap_chain_tx_sign.h"
#include "dap_chain_mempool.h"
#include "dap_hash.h"
#include "cellframe.h"

#include <string.h>

#define LOG_TAG "python_cellframe_tx"

static void *cf_tx_get_capsule(PyObject *obj, const char *name, const char *error_msg) {
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

static const char *cf_tx_get_cstring(PyObject *obj, const char *error_msg) {
    if (PyUnicode_Check(obj)) {
        return PyUnicode_AsUTF8(obj);
    }
    if (PyBytes_Check(obj)) {
        return PyBytes_AsString(obj);
    }
    PyErr_SetString(PyExc_TypeError, error_msg);
    return NULL;
}

static int cf_tx_parse_hash_fast(PyObject *obj, dap_hash_fast_t *out) {
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
    PyErr_SetString(PyExc_TypeError, "hash must be bytes, str, or hash capsule");
    return -1;
}

static int cf_tx_parse_uint256(PyObject *obj, uint256_t *out) {
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

static int cf_tx_parse_addr(PyObject *obj, dap_chain_addr_t *out) {
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

static PyObject *cf_tx_make_rc_bytes_tuple(int rc, const void *data, size_t data_size) {
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

// Create new transaction
PyObject* py_dap_chain_datum_tx_create(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    dap_chain_datum_tx_t *tx = dap_chain_datum_tx_create();
    if (!tx) {
        PyErr_SetString(CellframeNetworkError, "Failed to create transaction");
        return NULL;
    }

    return PyCapsule_New(tx, "dap_chain_datum_tx_t", NULL);
}

// Delete transaction
PyObject* py_dap_chain_datum_tx_delete(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_datum_tx_delete(tx);
    Py_RETURN_NONE;
}

// Get transaction size
PyObject* py_dap_chain_datum_tx_get_size(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    size_t size = dap_chain_datum_tx_get_size(tx);
    return PyLong_FromSize_t(size);
}

// Insert raw item to transaction
PyObject* py_dap_chain_datum_tx_add_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *item_obj = NULL;
    Py_buffer view = {0};
    int has_view = 0;

    if (!PyArg_ParseTuple(args, "OO", &tx_capsule, &item_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    const void *item_ptr = NULL;
    if (PyCapsule_CheckExact(item_obj)) {
        const char *name = PyCapsule_GetName(item_obj);
        if (!name) {
            PyErr_SetString(PyExc_ValueError, "Invalid item capsule");
            return NULL;
        }
        item_ptr = PyCapsule_GetPointer(item_obj, name);
        if (!item_ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid item capsule pointer");
            return NULL;
        }
    } else {
        if (PyObject_GetBuffer(item_obj, &view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "Item must be a capsule or bytes-like object");
            return NULL;
        }
        item_ptr = view.buf;
        has_view = 1;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_item(&tx_ptr, item_ptr);

    if (has_view) {
        PyBuffer_Release(&view);
    }
    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add input item to transaction
PyObject* py_dap_chain_datum_tx_add_in_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *hash_obj = NULL;
    unsigned int prev_tx_out_idx = 0;

    if (!PyArg_ParseTuple(args, "OOI", &tx_capsule, &hash_obj, &prev_tx_out_idx)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    if (cf_tx_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_in_item(&tx_ptr, &hash, (uint32_t)prev_tx_out_idx);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add input items from list
PyObject* py_dap_chain_datum_tx_add_in_item_list(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *list_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &tx_capsule, &list_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_list_t *list = (dap_list_t *)cf_tx_get_capsule(
        list_obj, "dap_list_t", "Second argument must be a dap_list_t capsule"
    );
    if (!list) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    uint256_t result = dap_chain_datum_tx_add_in_item_list(&tx_ptr, list);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }

    return PyBytes_FromStringAndSize((const char *)&result, sizeof(uint256_t));
}

// Add conditional input item
PyObject* py_dap_chain_datum_tx_add_in_cond_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *hash_obj = NULL;
    unsigned int prev_tx_out_idx = 0;
    int receipt_idx = 0;

    if (!PyArg_ParseTuple(args, "OOIi", &tx_capsule, &hash_obj, &prev_tx_out_idx, &receipt_idx)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    if (cf_tx_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_in_cond_item(&tx_ptr, &hash, (uint32_t)prev_tx_out_idx, (int32_t)receipt_idx);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add conditional input items from list
PyObject* py_dap_chain_datum_tx_add_in_cond_item_list(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *list_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &tx_capsule, &list_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_list_t *list = (dap_list_t *)cf_tx_get_capsule(
        list_obj, "dap_list_t", "Second argument must be a dap_list_t capsule"
    );
    if (!list) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    uint256_t result = dap_chain_datum_tx_add_in_cond_item_list(&tx_ptr, list);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }

    return PyBytes_FromStringAndSize((const char *)&result, sizeof(uint256_t));
}

// Add reward input item
PyObject* py_dap_chain_datum_tx_add_in_reward_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *hash_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &tx_capsule, &hash_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    if (cf_tx_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_in_reward_item(&tx_ptr, &hash);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add output item
PyObject* py_dap_chain_datum_tx_add_out_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *addr_obj = NULL;
    PyObject *value_obj = NULL;

    if (!PyArg_ParseTuple(args, "OOO", &tx_capsule, &addr_obj, &value_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_tx_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_tx_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_out_item(&tx_ptr, &addr, value);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add fee item
PyObject* py_dap_chain_datum_tx_add_fee_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *value_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &tx_capsule, &value_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_tx_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_fee_item(&tx_ptr, value);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add extended output item
PyObject* py_dap_chain_datum_tx_add_out_ext_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *addr_obj = NULL;
    PyObject *value_obj = NULL;
    PyObject *ticker_obj = NULL;

    if (!PyArg_ParseTuple(args, "OOOO", &tx_capsule, &addr_obj, &value_obj, &ticker_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_tx_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_tx_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    const char *ticker = cf_tx_get_cstring(ticker_obj, "Fourth argument must be a string or bytes");
    if (!ticker) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_out_ext_item(&tx_ptr, &addr, value, ticker);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add standard output item
PyObject* py_dap_chain_datum_tx_add_out_std_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *addr_obj = NULL;
    PyObject *value_obj = NULL;
    PyObject *ticker_obj = NULL;
    unsigned long long ts_unlock = 0;

    if (!PyArg_ParseTuple(args, "OOOOK", &tx_capsule, &addr_obj, &value_obj, &ticker_obj, &ts_unlock)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_tx_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_tx_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    const char *ticker = cf_tx_get_cstring(ticker_obj, "Fourth argument must be a string or bytes");
    if (!ticker) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_out_std_item(&tx_ptr, &addr, value, ticker, (dap_time_t)ts_unlock);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add conditional output item
PyObject* py_dap_chain_datum_tx_add_out_cond_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *hash_obj = NULL;
    unsigned long long srv_uid_uint = 0;
    PyObject *value_obj = NULL;
    PyObject *value_max_obj = NULL;
    int unit_enm = 0;
    PyObject *cond_obj = Py_None;
    Py_buffer cond_view = {0};
    int has_view = 0;

    if (!PyArg_ParseTuple(args, "OOKOOi|O", &tx_capsule, &hash_obj, &srv_uid_uint,
                          &value_obj, &value_max_obj, &unit_enm, &cond_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_hash_fast_t key_hash = {0};
    if (cf_tx_parse_hash_fast(hash_obj, &key_hash) != 0) {
        return NULL;
    }

    uint256_t value = {0};
    if (cf_tx_parse_uint256(value_obj, &value) != 0) {
        return NULL;
    }

    uint256_t value_max = {0};
    if (cf_tx_parse_uint256(value_max_obj, &value_max) != 0) {
        return NULL;
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_net_srv_price_unit_uid_t unit = {.enm = (dap_chain_srv_unit_enum_t)unit_enm};

    const void *cond_ptr = NULL;
    size_t cond_size = 0;
    if (cond_obj != Py_None) {
        if (PyObject_GetBuffer(cond_obj, &cond_view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "Condition must be bytes-like or None");
            return NULL;
        }
        cond_ptr = cond_view.buf;
        cond_size = (size_t)cond_view.len;
        has_view = 1;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_out_cond_item(&tx_ptr, &key_hash, srv_uid, value,
                                                  value_max, unit, cond_ptr, cond_size);

    if (has_view) {
        PyBuffer_Release(&cond_view);
    }
    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add event item
PyObject* py_dap_chain_datum_tx_add_event_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *pkey_obj = NULL;
    unsigned long long srv_uid_uint = 0;
    PyObject *group_obj = NULL;
    unsigned int event_type = 0;
    PyObject *event_data_obj = Py_None;
    Py_buffer event_view = {0};
    int has_view = 0;

    if (!PyArg_ParseTuple(args, "OOKOi|O", &tx_capsule, &pkey_obj, &srv_uid_uint,
                          &group_obj, &event_type, &event_data_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_pkey_t *pkey = (dap_pkey_t *)cf_tx_get_capsule(
        pkey_obj, "dap_pkey_t", "Second argument must be a dap_pkey_t capsule"
    );
    if (!pkey) {
        return NULL;
    }

    const char *group_name = cf_tx_get_cstring(group_obj, "Fourth argument must be a string or bytes");
    if (!group_name) {
        return NULL;
    }

    const void *event_data = NULL;
    size_t event_size = 0;
    if (event_data_obj != Py_None) {
        if (PyObject_GetBuffer(event_data_obj, &event_view, PyBUF_SIMPLE) != 0) {
            PyErr_SetString(PyExc_TypeError, "Event data must be bytes-like or None");
            return NULL;
        }
        event_data = event_view.buf;
        event_size = (size_t)event_view.len;
        has_view = 1;
    }

    dap_chain_srv_uid_t srv_uid = {.uint64 = srv_uid_uint};
    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_event_item(&tx_ptr, pkey, srv_uid, group_name,
                                               (uint16_t)event_type, event_data, event_size);

    if (has_view) {
        PyBuffer_Release(&event_view);
    }
    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add signature item
PyObject* py_dap_chain_datum_tx_add_sign_item(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *key_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &tx_capsule, &key_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_enc_key_t *key = (dap_enc_key_t *)cf_tx_get_capsule(
        key_obj, "dap_enc_key_t", "Second argument must be a dap_enc_key_t capsule"
    );
    if (!key) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_sign_item(&tx_ptr, key);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add precomputed signature
PyObject* py_dap_chain_datum_tx_add_sign(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *sign_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &tx_capsule, &sign_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_sign_t *sign = (dap_sign_t *)cf_tx_get_capsule(
        sign_obj, "dap_sign_t", "Second argument must be a dap_sign_t capsule"
    );
    if (!sign) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_datum_tx_add_sign(&tx_ptr, sign);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Get signature by index
PyObject* py_dap_chain_datum_tx_get_sign(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    int sign_num = 0;

    if (!PyArg_ParseTuple(args, "Oi", &tx_capsule, &sign_num)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_sign_t *sign = dap_chain_datum_tx_get_sign(tx, sign_num);
    if (!sign) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(sign, "dap_sign_t", NULL);
}

// Get signing data
PyObject* py_dap_chain_datum_tx_get_sign_data(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    size_t data_size = 0;
    const void *data = dap_chain_datum_tx_get_sign_data(tx, &data_size);
    if (!data) {
        Py_RETURN_NONE;
    }
    return PyBytes_FromStringAndSize((const char *)data, (Py_ssize_t)data_size);
}

// Get fee value
PyObject* py_dap_chain_datum_tx_get_fee_value(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    uint256_t value = {0};
    int rc = dap_chain_datum_tx_get_fee_value(tx, &value);
    return cf_tx_make_rc_bytes_tuple(rc, &value, sizeof(uint256_t));
}

// Group items in transaction
PyObject* py_dap_chain_datum_tx_group_items(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_datum_tx_item_groups_t *groups = DAP_NEW_Z(dap_chain_datum_tx_item_groups_t);
    if (!groups) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate item groups");
        return NULL;
    }

    bool ok = dap_chain_datum_tx_group_items(tx, groups);
    if (!ok) {
        DAP_DELETE(groups);
        Py_RETURN_NONE;
    }

    return PyCapsule_New(groups, "dap_chain_datum_tx_item_groups_t", NULL);
}

// Free grouped items
PyObject* py_dap_chain_datum_tx_group_items_free(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *groups_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &groups_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_item_groups_t *groups = (dap_chain_datum_tx_item_groups_t *)cf_tx_get_capsule(
        groups_obj, "dap_chain_datum_tx_item_groups_t", "First argument must be a tx item groups capsule"
    );
    if (!groups) {
        return NULL;
    }

    dap_chain_datum_tx_group_items_free(groups);
    DAP_DELETE(groups);
    Py_RETURN_NONE;
}

// Get item with iterator
PyObject* py_dap_chain_datum_tx_item_get(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    int item_idx_start = 0;
    PyObject *iter_obj = NULL;
    int item_type = 0;

    if (!PyArg_ParseTuple(args, "OiOi", &tx_capsule, &item_idx_start, &iter_obj, &item_type)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    byte_t *iter = NULL;
    if (iter_obj != Py_None) {
        iter = (byte_t *)cf_tx_get_capsule(
            iter_obj, "dap_chain_tx_item_t", "Third argument must be a tx item capsule or None"
        );
        if (!iter) {
            return NULL;
        }
    }

    int item_idx = item_idx_start;
    size_t item_size = 0;
    uint8_t *item = dap_chain_datum_tx_item_get(tx, &item_idx, iter,
                                                (dap_chain_tx_item_type_t)item_type, &item_size);

    PyObject *tuple = PyTuple_New(3);
    if (!tuple) {
        return NULL;
    }

    if (!item) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 0, Py_None);
    } else {
        PyObject *capsule = PyCapsule_New(item, "dap_chain_tx_item_t", NULL);
        if (!capsule) {
            Py_DECREF(tuple);
            return NULL;
        }
        PyTuple_SET_ITEM(tuple, 0, capsule);
    }
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(item_size));
    PyTuple_SET_ITEM(tuple, 2, PyLong_FromLong(item_idx));
    return tuple;
}

// Get Nth item
PyObject* py_dap_chain_datum_tx_item_get_nth(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    int item_type = 0;
    int item_idx = 0;

    if (!PyArg_ParseTuple(args, "Oii", &tx_capsule, &item_type, &item_idx)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    uint8_t *item = dap_chain_datum_tx_item_get_nth(tx, (dap_chain_tx_item_type_t)item_type, item_idx);
    if (!item) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(item, "dap_chain_tx_item_t", NULL);
}

// Get items list
PyObject* py_dap_chain_datum_tx_items_get(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    int item_type = 0;

    if (!PyArg_ParseTuple(args, "Oi", &tx_capsule, &item_type)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    int count = 0;
    dap_list_t *items = dap_chain_datum_tx_items_get(tx, (dap_chain_tx_item_type_t)item_type, &count);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    if (!items) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 0, Py_None);
        PyTuple_SET_ITEM(tuple, 1, PyLong_FromLong(0));
        return tuple;
    }

    PyObject *capsule = PyCapsule_New(items, "dap_list_t", NULL);
    if (!capsule) {
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, capsule);
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromLong(count));
    return tuple;
}

// Get conditional output by subtype
PyObject* py_dap_chain_datum_tx_out_cond_get(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    int cond_subtype = 0;
    int out_num = 0;

    if (!PyArg_ParseTuple(args, "Oi|i", &tx_capsule, &cond_subtype, &out_num)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_tx_out_cond_t *out = dap_chain_datum_tx_out_cond_get(
        tx, (dap_chain_tx_out_cond_subtype_t)cond_subtype, &out_num
    );

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    if (!out) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 0, Py_None);
        PyTuple_SET_ITEM(tuple, 1, PyLong_FromLong(out_num));
        return tuple;
    }

    PyObject *capsule = PyCapsule_New(out, "dap_chain_tx_out_cond_t", NULL);
    if (!capsule) {
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, capsule);
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromLong(out_num));
    return tuple;
}

// Verify signature
PyObject* py_dap_chain_datum_tx_verify_sign(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    int sign_num = -1;

    if (!PyArg_ParseTuple(args, "O|i", &tx_capsule, &sign_num)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    int rc = (sign_num >= 0)
        ? dap_chain_datum_tx_verify_sign(tx, sign_num)
        : dap_chain_datum_tx_verify_sign_all(tx);

    return PyLong_FromLong(rc);
}

// Verify all signatures
PyObject* py_dap_chain_datum_tx_verify_sign_all(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    int rc = dap_chain_datum_tx_verify_sign_all(tx);
    return PyLong_FromLong(rc);
}

// Calculate tx hash
PyObject* py_dap_chain_node_datum_tx_calc_hash(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_hash_fast_t hash = dap_chain_node_datum_tx_calc_hash(tx);
    return PyBytes_FromStringAndSize((const char *)&hash, sizeof(dap_hash_fast_t));
}

// Wrap packed tx
PyObject* py_dap_chain_tx_wrap_packed(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_packed = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx_packed) {
        return NULL;
    }

    dap_chain_tx_t *tx = dap_chain_tx_wrap_packed(tx_packed);
    if (!tx) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(tx, "dap_chain_tx_t", NULL);
}

// Duplicate tx
PyObject* py_dap_chain_tx_dup(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }

    dap_chain_tx_t *tx = (dap_chain_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_tx_t *dup = dap_chain_tx_dup(tx);
    if (!dup) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(dup, "dap_chain_tx_t", NULL);
}

// Delete tx wrapper
PyObject* py_dap_chain_tx_delete(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }

    dap_chain_tx_t *tx = (dap_chain_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_tx_delete(tx);
    Py_RETURN_NONE;
}

// Add tx to hashtable
PyObject* py_dap_chain_tx_hh_add(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_hh_capsule = NULL;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "OO", &tx_hh_capsule, &tx_capsule)) {
        return NULL;
    }

    dap_chain_tx_t *tx_hh = NULL;
    if (tx_hh_capsule != Py_None) {
        tx_hh = (dap_chain_tx_t *)cf_tx_get_capsule(
            tx_hh_capsule, "dap_chain_tx_t", "First argument must be a tx hashtable capsule or None"
        );
        if (!tx_hh) {
            return NULL;
        }
    }

    dap_chain_tx_t *tx = (dap_chain_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_tx_t", "Second argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_tx_hh_add(&tx_hh, tx);

    if (tx_hh_capsule != Py_None) {
        if (PyCapsule_SetPointer(tx_hh_capsule, tx_hh) < 0) {
            return NULL;
        }
        Py_INCREF(tx_hh_capsule);
        return tx_hh_capsule;
    }

    return PyCapsule_New(tx_hh, "dap_chain_tx_t", NULL);
}

// Find tx in hashtable
PyObject* py_dap_chain_tx_hh_find(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_hh_capsule = NULL;
    PyObject *hash_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &tx_hh_capsule, &hash_obj)) {
        return NULL;
    }

    dap_chain_tx_t *tx_hh = (dap_chain_tx_t *)cf_tx_get_capsule(
        tx_hh_capsule, "dap_chain_tx_t", "First argument must be a tx hashtable capsule"
    );
    if (!tx_hh) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    if (cf_tx_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }

    dap_chain_tx_t *found = dap_chain_tx_hh_find(tx_hh, &hash);
    if (!found) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(found, "dap_chain_tx_t", NULL);
}

// Free hashtable
PyObject* py_dap_chain_tx_hh_free(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_hh_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_hh_capsule)) {
        return NULL;
    }

    dap_chain_tx_t *tx_hh = (dap_chain_tx_t *)cf_tx_get_capsule(
        tx_hh_capsule, "dap_chain_tx_t", "First argument must be a tx hashtable capsule"
    );
    if (!tx_hh) {
        return NULL;
    }

    dap_chain_tx_hh_free(tx_hh);
    Py_RETURN_NONE;
}

// Get signing data for tx_sign
PyObject* py_dap_chain_tx_get_signing_data(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_capsule)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    size_t data_size = 0;
    const void *data = dap_chain_tx_get_signing_data(tx, &data_size);
    if (!data) {
        Py_RETURN_NONE;
    }
    return PyBytes_FromStringAndSize((const char *)data, (Py_ssize_t)data_size);
}

// Add signature via tx_sign
PyObject* py_dap_chain_tx_sign_add(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_capsule = NULL;
    PyObject *sign_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &tx_capsule, &sign_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_tx_get_capsule(
        tx_capsule, "dap_chain_datum_tx_t", "First argument must be a tx capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_sign_t *sign = (dap_sign_t *)cf_tx_get_capsule(
        sign_obj, "dap_sign_t", "Second argument must be a dap_sign_t capsule"
    );
    if (!sign) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx_ptr = tx;
    int rc = dap_chain_tx_sign_add(&tx_ptr, sign);

    if (tx_ptr != tx) {
        if (PyCapsule_SetPointer(tx_capsule, tx_ptr) < 0) {
            return NULL;
        }
    }
    return PyLong_FromLong(rc);
}

// Add transaction to mempool
PyObject* py_dap_chain_mempool_datum_add(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *datum_capsule = NULL;
    PyObject *chain_capsule = NULL;
    const char *hash_out_type = NULL;

    if (!PyArg_ParseTuple(args, "OO|s", &datum_capsule, &chain_capsule, &hash_out_type)) {
        return NULL;
    }

    dap_chain_datum_t *datum = (dap_chain_datum_t *)cf_tx_get_capsule(
        datum_capsule, "dap_chain_datum_t", "First argument must be a datum capsule"
    );
    if (!datum) {
        return NULL;
    }

    dap_chain_t *chain = (dap_chain_t *)cf_tx_get_capsule(
        chain_capsule, "dap_chain_t", "Second argument must be a chain capsule"
    );
    if (!chain) {
        return NULL;
    }

    char *result = dap_chain_mempool_datum_add(datum, chain, hash_out_type);
    if (!result) {
        PyErr_SetString(CellframeNetworkError, "Failed to add datum to mempool");
        return NULL;
    }

    PyObject *py_result = PyUnicode_FromString(result);
    DAP_DELETE(result);
    return py_result;
}

// Module initialization function
int cellframe_tx_init(PyObject *module) {
    static PyMethodDef tx_methods[] = {
        {"dap_chain_datum_tx_create", py_dap_chain_datum_tx_create, METH_NOARGS,
         "Create new transaction"},
        {"dap_chain_datum_tx_delete", py_dap_chain_datum_tx_delete, METH_VARARGS,
         "Delete transaction"},
        {"dap_chain_datum_tx_get_size", py_dap_chain_datum_tx_get_size, METH_VARARGS,
         "Get transaction size"},
        {"dap_chain_datum_tx_add_item", py_dap_chain_datum_tx_add_item, METH_VARARGS,
         "Insert item into transaction"},
        {"dap_chain_datum_tx_add_in_item", py_dap_chain_datum_tx_add_in_item, METH_VARARGS,
         "Add input item to transaction"},
        {"dap_chain_datum_tx_add_in_item_list", py_dap_chain_datum_tx_add_in_item_list, METH_VARARGS,
         "Add input items from list"},
        {"dap_chain_datum_tx_add_in_cond_item", py_dap_chain_datum_tx_add_in_cond_item, METH_VARARGS,
         "Add conditional input item"},
        {"dap_chain_datum_tx_add_in_cond_item_list", py_dap_chain_datum_tx_add_in_cond_item_list, METH_VARARGS,
         "Add conditional input items from list"},
        {"dap_chain_datum_tx_add_in_reward_item", py_dap_chain_datum_tx_add_in_reward_item, METH_VARARGS,
         "Add reward input item"},
        {"dap_chain_datum_tx_add_out_item", py_dap_chain_datum_tx_add_out_item, METH_VARARGS,
         "Add output item"},
        {"dap_chain_datum_tx_add_fee_item", py_dap_chain_datum_tx_add_fee_item, METH_VARARGS,
         "Add fee item"},
        {"dap_chain_datum_tx_add_out_ext_item", py_dap_chain_datum_tx_add_out_ext_item, METH_VARARGS,
         "Add extended output item"},
        {"dap_chain_datum_tx_add_out_std_item", py_dap_chain_datum_tx_add_out_std_item, METH_VARARGS,
         "Add standard output item"},
        {"dap_chain_datum_tx_add_out_cond_item", py_dap_chain_datum_tx_add_out_cond_item, METH_VARARGS,
         "Add conditional output item"},
        {"dap_chain_datum_tx_add_event_item", py_dap_chain_datum_tx_add_event_item, METH_VARARGS,
         "Add event item"},
        {"dap_chain_datum_tx_add_sign_item", py_dap_chain_datum_tx_add_sign_item, METH_VARARGS,
         "Add signature item"},
        {"dap_chain_datum_tx_add_sign", py_dap_chain_datum_tx_add_sign, METH_VARARGS,
         "Add precomputed signature"},
        {"dap_chain_datum_tx_get_sign", py_dap_chain_datum_tx_get_sign, METH_VARARGS,
         "Get signature by index"},
        {"dap_chain_datum_tx_get_sign_data", py_dap_chain_datum_tx_get_sign_data, METH_VARARGS,
         "Get data to be signed"},
        {"dap_chain_datum_tx_get_fee_value", py_dap_chain_datum_tx_get_fee_value, METH_VARARGS,
         "Get fee value"},
        {"dap_chain_datum_tx_group_items", py_dap_chain_datum_tx_group_items, METH_VARARGS,
         "Group transaction items"},
        {"dap_chain_datum_tx_group_items_free", py_dap_chain_datum_tx_group_items_free, METH_VARARGS,
         "Free grouped transaction items"},
        {"dap_chain_datum_tx_item_get", py_dap_chain_datum_tx_item_get, METH_VARARGS,
         "Get transaction item"},
        {"dap_chain_datum_tx_item_get_nth", py_dap_chain_datum_tx_item_get_nth, METH_VARARGS,
         "Get Nth transaction item"},
        {"dap_chain_datum_tx_items_get", py_dap_chain_datum_tx_items_get, METH_VARARGS,
         "Get transaction items list"},
        {"dap_chain_datum_tx_out_cond_get", py_dap_chain_datum_tx_out_cond_get, METH_VARARGS,
         "Get conditional output item"},
        {"dap_chain_datum_tx_verify_sign", py_dap_chain_datum_tx_verify_sign, METH_VARARGS,
         "Verify transaction signature(s)"},
        {"dap_chain_datum_tx_verify_sign_all", py_dap_chain_datum_tx_verify_sign_all, METH_VARARGS,
         "Verify all transaction signatures"},
        {"dap_chain_node_datum_tx_calc_hash", py_dap_chain_node_datum_tx_calc_hash, METH_VARARGS,
         "Calculate transaction hash"},
        {"dap_chain_tx_wrap_packed", py_dap_chain_tx_wrap_packed, METH_VARARGS,
         "Wrap packed tx into dap_chain_tx_t"},
        {"dap_chain_tx_dup", py_dap_chain_tx_dup, METH_VARARGS,
         "Duplicate dap_chain_tx_t"},
        {"dap_chain_tx_delete", py_dap_chain_tx_delete, METH_VARARGS,
         "Delete dap_chain_tx_t"},
        {"dap_chain_tx_hh_add", py_dap_chain_tx_hh_add, METH_VARARGS,
         "Add tx to hashtable, returns head"},
        {"dap_chain_tx_hh_find", py_dap_chain_tx_hh_find, METH_VARARGS,
         "Find tx in hashtable"},
        {"dap_chain_tx_hh_free", py_dap_chain_tx_hh_free, METH_VARARGS,
         "Free tx hashtable"},
        {"dap_chain_tx_get_signing_data", py_dap_chain_tx_get_signing_data, METH_VARARGS,
         "Get signing data for tx"},
        {"dap_chain_tx_sign_add", py_dap_chain_tx_sign_add, METH_VARARGS,
         "Add signature to tx"},
        {"dap_chain_mempool_datum_add", py_dap_chain_mempool_datum_add, METH_VARARGS,
         "Add datum to mempool"},
        {NULL, NULL, 0, NULL}
    };

    for (PyMethodDef *method = tx_methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_NewEx(method, NULL, NULL);
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
