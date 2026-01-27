#include "cf_datum_internal.h"
#include "dap_chain_datum_tx.h"
#include "dap_chain_datum_tx_items.h"
#include "dap_chain_datum_tx_out_cond.h"
#include "dap_chain_datum_tx_event.h"
#include "dap_enc_key.h"
#include "dap_json.h"
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
