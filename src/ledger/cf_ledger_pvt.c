#include "include/cf_ledger_internal.h"
#include "dap_chain_ledger_pvt.h"
#include "dap_global_db.h"

PyObject* dap_ledger_pvt_cache_gdb_load_tokens_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_dbi_obj = NULL;
    PyObject *l_values_obj = NULL;
    PyObject *l_arg_obj = Py_None;
    const char *l_group = NULL;
    unsigned long long l_values_total = 0;
    unsigned long long l_values_count = 0;
    int l_rc = 0;

    if (!PyArg_ParseTuple(a_args, "OisKKO|O", &l_dbi_obj, &l_rc, &l_group,
                          &l_values_total, &l_values_count, &l_values_obj, &l_arg_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_dbi_obj) || !PyCapsule_CheckExact(l_values_obj)) {
        PyErr_SetString(PyExc_TypeError, "dbi and values must be capsules");
        return NULL;
    }

    dap_global_db_instance_t *l_dbi = (dap_global_db_instance_t *)PyCapsule_GetPointer(l_dbi_obj, "dap_global_db_instance_t");
    dap_global_db_obj_t *l_values = (dap_global_db_obj_t *)PyCapsule_GetPointer(l_values_obj, "dap_global_db_obj_t");
    if (!l_dbi || !l_values) {
        PyErr_SetString(PyExc_ValueError, "Invalid dbi or values capsule");
        return NULL;
    }

    void *l_arg = NULL;
    if (l_arg_obj && l_arg_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_arg_obj)) {
            PyErr_SetString(PyExc_TypeError, "arg must be a capsule or None");
            return NULL;
        }
        l_arg = PyCapsule_GetPointer(l_arg_obj, NULL);
    }

    bool l_result = dap_ledger_pvt_cache_gdb_load_tokens_callback(
        l_dbi, l_rc, l_group, (size_t)l_values_total, (size_t)l_values_count, l_values, l_arg
    );
    return PyBool_FromLong(l_result ? 1 : 0);
}

PyObject* dap_ledger_pvt_cache_gdb_load_stake_lock_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_dbi_obj = NULL;
    PyObject *l_values_obj = NULL;
    PyObject *l_arg_obj = Py_None;
    const char *l_group = NULL;
    unsigned long long l_values_total = 0;
    unsigned long long l_values_count = 0;
    int l_rc = 0;

    if (!PyArg_ParseTuple(a_args, "OisKKO|O", &l_dbi_obj, &l_rc, &l_group,
                          &l_values_total, &l_values_count, &l_values_obj, &l_arg_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_dbi_obj) || !PyCapsule_CheckExact(l_values_obj)) {
        PyErr_SetString(PyExc_TypeError, "dbi and values must be capsules");
        return NULL;
    }

    dap_global_db_instance_t *l_dbi = (dap_global_db_instance_t *)PyCapsule_GetPointer(l_dbi_obj, "dap_global_db_instance_t");
    dap_global_db_obj_t *l_values = (dap_global_db_obj_t *)PyCapsule_GetPointer(l_values_obj, "dap_global_db_obj_t");
    if (!l_dbi || !l_values) {
        PyErr_SetString(PyExc_ValueError, "Invalid dbi or values capsule");
        return NULL;
    }

    void *l_arg = NULL;
    if (l_arg_obj && l_arg_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_arg_obj)) {
            PyErr_SetString(PyExc_TypeError, "arg must be a capsule or None");
            return NULL;
        }
        l_arg = PyCapsule_GetPointer(l_arg_obj, NULL);
    }

    bool l_result = dap_ledger_pvt_cache_gdb_load_stake_lock_callback(
        l_dbi, l_rc, l_group, (size_t)l_values_total, (size_t)l_values_count, l_values, l_arg
    );
    return PyBool_FromLong(l_result ? 1 : 0);
}

PyObject* dap_ledger_pvt_cache_gdb_load_balances_callback_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_dbi_obj = NULL;
    PyObject *l_values_obj = NULL;
    PyObject *l_arg_obj = Py_None;
    const char *l_group = NULL;
    unsigned long long l_values_total = 0;
    unsigned long long l_values_count = 0;
    int l_rc = 0;

    if (!PyArg_ParseTuple(a_args, "OisKKO|O", &l_dbi_obj, &l_rc, &l_group,
                          &l_values_total, &l_values_count, &l_values_obj, &l_arg_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_dbi_obj) || !PyCapsule_CheckExact(l_values_obj)) {
        PyErr_SetString(PyExc_TypeError, "dbi and values must be capsules");
        return NULL;
    }

    dap_global_db_instance_t *l_dbi = (dap_global_db_instance_t *)PyCapsule_GetPointer(l_dbi_obj, "dap_global_db_instance_t");
    dap_global_db_obj_t *l_values = (dap_global_db_obj_t *)PyCapsule_GetPointer(l_values_obj, "dap_global_db_obj_t");
    if (!l_dbi || !l_values) {
        PyErr_SetString(PyExc_ValueError, "Invalid dbi or values capsule");
        return NULL;
    }

    void *l_arg = NULL;
    if (l_arg_obj && l_arg_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_arg_obj)) {
            PyErr_SetString(PyExc_TypeError, "arg must be a capsule or None");
            return NULL;
        }
        l_arg = PyCapsule_GetPointer(l_arg_obj, NULL);
    }

    bool l_result = dap_ledger_pvt_cache_gdb_load_balances_callback(
        l_dbi, l_rc, l_group, (size_t)l_values_total, (size_t)l_values_count, l_values, l_arg
    );
    return PyBool_FromLong(l_result ? 1 : 0);
}

PyObject* dap_ledger_pvt_threshold_txs_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_tx_obj = NULL;
    const char *l_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;

    if (!PyArg_ParseTuple(a_args, "OOs#", &l_ledger_obj, &l_tx_obj, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj) || !PyCapsule_CheckExact(l_tx_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger and tx must be capsules");
        return NULL;
    }

    if ((size_t)l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t *)PyCapsule_GetPointer(l_tx_obj, "dap_chain_datum_tx_t");
    if (!l_ledger || !l_tx) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or tx capsule");
        return NULL;
    }

    dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_hash_bytes;
    int l_result = dap_ledger_pvt_threshold_txs_add(l_ledger, l_tx, l_hash);
    return PyLong_FromLong(l_result);
}

PyObject* dap_ledger_pvt_threshold_txs_proc_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_ledger_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger must be a capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_ledger_pvt_threshold_txs_proc(l_ledger);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_pvt_find_token_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    const char *l_ticker = NULL;

    if (!PyArg_ParseTuple(a_args, "Os", &l_ledger_obj, &l_ticker)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger must be a capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_ledger_token_item_t *l_item = dap_ledger_pvt_find_token(l_ledger, l_ticker);
    if (!l_item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_item, "dap_ledger_token_item_t", NULL);
}

PyObject* dap_ledger_pvt_token_supply_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_token_obj = NULL;
    const char *l_value_bytes = NULL;
    Py_ssize_t l_value_size = 0;

    if (!PyArg_ParseTuple(a_args, "Os#", &l_token_obj, &l_value_bytes, &l_value_size)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_token_obj)) {
        PyErr_SetString(PyExc_TypeError, "token_item must be a capsule");
        return NULL;
    }

    if ((size_t)l_value_size != sizeof(uint256_t)) {
        PyErr_Format(PyExc_ValueError, "value must be exactly %zu bytes", sizeof(uint256_t));
        return NULL;
    }

    dap_ledger_token_item_t *l_item = (dap_ledger_token_item_t *)PyCapsule_GetPointer(l_token_obj, "dap_ledger_token_item_t");
    if (!l_item) {
        PyErr_SetString(PyExc_ValueError, "Invalid token_item capsule");
        return NULL;
    }

    uint256_t l_value = *(uint256_t *)l_value_bytes;
    bool l_result = dap_ledger_pvt_token_supply_check(l_item, l_value);
    return PyBool_FromLong(l_result ? 1 : 0);
}

PyObject* dap_ledger_pvt_token_supply_check_update_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_token_obj = NULL;
    const char *l_value_bytes = NULL;
    Py_ssize_t l_value_size = 0;
    int l_for_removing = 0;

    if (!PyArg_ParseTuple(a_args, "OOs#p", &l_ledger_obj, &l_token_obj, &l_value_bytes,
                          &l_value_size, &l_for_removing)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj) || !PyCapsule_CheckExact(l_token_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger and token_item must be capsules");
        return NULL;
    }

    if ((size_t)l_value_size != sizeof(uint256_t)) {
        PyErr_Format(PyExc_ValueError, "value must be exactly %zu bytes", sizeof(uint256_t));
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_ledger_token_item_t *l_item = (dap_ledger_token_item_t *)PyCapsule_GetPointer(l_token_obj, "dap_ledger_token_item_t");
    if (!l_ledger || !l_item) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or token_item capsule");
        return NULL;
    }

    uint256_t l_value = *(uint256_t *)l_value_bytes;
    bool l_result = dap_ledger_pvt_token_supply_check_update(l_ledger, l_item, l_value, l_for_removing != 0);
    return PyBool_FromLong(l_result ? 1 : 0);
}

PyObject* dap_ledger_pvt_emission_item_find_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    const char *l_ticker = NULL;
    const char *l_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;

    if (!PyArg_ParseTuple(a_args, "Oss#", &l_ledger_obj, &l_ticker, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger must be a capsule");
        return NULL;
    }

    if ((size_t)l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_hash_bytes;
    dap_ledger_token_item_t *l_token_item = NULL;
    dap_ledger_token_emission_item_t *l_item = dap_ledger_pvt_emission_item_find(
        l_ledger, l_ticker, l_hash, &l_token_item
    );

    PyObject *l_item_obj = l_item ? PyCapsule_New(l_item, "dap_ledger_token_emission_item_t", NULL) : Py_None;
    PyObject *l_token_obj = l_token_item ? PyCapsule_New(l_token_item, "dap_ledger_token_item_t", NULL) : Py_None;
    if (!l_item) {
        Py_INCREF(Py_None);
    }
    if (!l_token_item) {
        Py_INCREF(Py_None);
    }

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, l_item_obj);
    PyTuple_SetItem(l_tuple, 1, l_token_obj);
    return l_tuple;
}

PyObject* dap_ledger_pvt_addr_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_token_obj = Py_None;
    PyObject *l_addr_obj = NULL;
    int l_receive = 0;

    if (!PyArg_ParseTuple(a_args, "OOOp", &l_ledger_obj, &l_token_obj, &l_addr_obj, &l_receive)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger must be a capsule");
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "addr must be a capsule");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_obj, "dap_chain_addr_t");
    if (!l_ledger || !l_addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or addr capsule");
        return NULL;
    }

    dap_ledger_token_item_t *l_token_item = NULL;
    if (l_token_obj && l_token_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_token_obj)) {
            PyErr_SetString(PyExc_TypeError, "token_item must be a capsule or None");
            return NULL;
        }
        l_token_item = (dap_ledger_token_item_t *)PyCapsule_GetPointer(l_token_obj, "dap_ledger_token_item_t");
        if (!l_token_item) {
            PyErr_SetString(PyExc_ValueError, "Invalid token_item capsule");
            return NULL;
        }
    }

    dap_ledger_check_error_t l_result = dap_ledger_pvt_addr_check(l_ledger, l_token_item, l_addr, l_receive != 0);
    return PyLong_FromLong((long)l_result);
}

PyObject* dap_ledger_pvt_emission_cache_update_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_item_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_ledger_obj, &l_item_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj) || !PyCapsule_CheckExact(l_item_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger and emission_item must be capsules");
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_ledger_token_emission_item_t *l_item = (dap_ledger_token_emission_item_t *)PyCapsule_GetPointer(
        l_item_obj, "dap_ledger_token_emission_item_t"
    );
    if (!l_ledger || !l_item) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or emission_item capsule");
        return NULL;
    }

    dap_ledger_pvt_emission_cache_update(l_ledger, l_item);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_pvt_balance_update_for_addr_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_addr_obj = NULL;
    const char *l_ticker = NULL;
    const char *l_value_bytes = NULL;
    Py_ssize_t l_value_size = 0;
    int l_reverse = 0;

    if (!PyArg_ParseTuple(a_args, "OOss#p", &l_ledger_obj, &l_addr_obj, &l_ticker,
                          &l_value_bytes, &l_value_size, &l_reverse)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj) || !PyCapsule_CheckExact(l_addr_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger and addr must be capsules");
        return NULL;
    }

    if ((size_t)l_value_size != sizeof(uint256_t)) {
        PyErr_Format(PyExc_ValueError, "value must be exactly %zu bytes", sizeof(uint256_t));
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_chain_addr_t *l_addr = (dap_chain_addr_t *)PyCapsule_GetPointer(l_addr_obj, "dap_chain_addr_t");
    if (!l_ledger || !l_addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or addr capsule");
        return NULL;
    }

    uint256_t l_value = *(uint256_t *)l_value_bytes;
    int l_result = dap_ledger_pvt_balance_update_for_addr(l_ledger, l_addr, l_ticker, l_value, l_reverse != 0);
    return PyLong_FromLong(l_result);
}

PyObject* dap_ledger_pvt_event_verify_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_tx_obj = NULL;
    const char *l_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;
    int l_apply = 0;
    int l_from_mempool = 0;

    if (!PyArg_ParseTuple(a_args, "OOs#pp", &l_ledger_obj, &l_tx_obj, &l_hash_bytes,
                          &l_hash_size, &l_apply, &l_from_mempool)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj) || !PyCapsule_CheckExact(l_tx_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger and tx must be capsules");
        return NULL;
    }

    if ((size_t)l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    dap_chain_datum_tx_t *l_tx = (dap_chain_datum_tx_t *)PyCapsule_GetPointer(l_tx_obj, "dap_chain_datum_tx_t");
    if (!l_ledger || !l_tx) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger or tx capsule");
        return NULL;
    }

    dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_hash_bytes;
    int l_result = dap_ledger_pvt_event_verify_add(l_ledger, l_hash, l_tx, l_apply != 0, l_from_mempool != 0);
    return PyLong_FromLong(l_result);
}

PyObject* dap_ledger_pvt_event_remove_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ledger_obj = NULL;
    const char *l_hash_bytes = NULL;
    Py_ssize_t l_hash_size = 0;

    if (!PyArg_ParseTuple(a_args, "Os#", &l_ledger_obj, &l_hash_bytes, &l_hash_size)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "ledger must be a capsule");
        return NULL;
    }

    if ((size_t)l_hash_size != sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)PyCapsule_GetPointer(l_ledger_obj, "dap_ledger_t");
    if (!l_ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_hash_fast_t *l_hash = (dap_hash_fast_t *)l_hash_bytes;
    int l_result = dap_ledger_pvt_event_remove(l_ledger, l_hash);
    return PyLong_FromLong(l_result);
}

PyMethodDef* cellframe_ledger_pvt_get_methods(void) {
    static PyMethodDef methods[] = {
        {"ledger_pvt_cache_gdb_load_tokens_callback", (PyCFunction)dap_ledger_pvt_cache_gdb_load_tokens_callback_py, METH_VARARGS,
         "Private: load tokens cache from GDB"},
        {"ledger_pvt_cache_gdb_load_stake_lock_callback", (PyCFunction)dap_ledger_pvt_cache_gdb_load_stake_lock_callback_py, METH_VARARGS,
         "Private: load stake lock cache from GDB"},
        {"ledger_pvt_cache_gdb_load_balances_callback", (PyCFunction)dap_ledger_pvt_cache_gdb_load_balances_callback_py, METH_VARARGS,
         "Private: load balances cache from GDB"},
        {"ledger_pvt_threshold_txs_add", (PyCFunction)dap_ledger_pvt_threshold_txs_add_py, METH_VARARGS,
         "Private: add threshold TX"},
        {"ledger_pvt_threshold_txs_proc", (PyCFunction)dap_ledger_pvt_threshold_txs_proc_py, METH_VARARGS,
         "Private: process threshold TXs"},
        {"ledger_pvt_find_token", (PyCFunction)dap_ledger_pvt_find_token_py, METH_VARARGS,
         "Private: find token item"},
        {"ledger_pvt_token_supply_check", (PyCFunction)dap_ledger_pvt_token_supply_check_py, METH_VARARGS,
         "Private: check token supply"},
        {"ledger_pvt_token_supply_check_update", (PyCFunction)dap_ledger_pvt_token_supply_check_update_py, METH_VARARGS,
         "Private: check/update token supply"},
        {"ledger_pvt_emission_item_find", (PyCFunction)dap_ledger_pvt_emission_item_find_py, METH_VARARGS,
         "Private: find emission item"},
        {"ledger_pvt_addr_check", (PyCFunction)dap_ledger_pvt_addr_check_py, METH_VARARGS,
         "Private: check address permissions"},
        {"ledger_pvt_emission_cache_update", (PyCFunction)dap_ledger_pvt_emission_cache_update_py, METH_VARARGS,
         "Private: update emission cache"},
        {"ledger_pvt_balance_update_for_addr", (PyCFunction)dap_ledger_pvt_balance_update_for_addr_py, METH_VARARGS,
         "Private: update balance for address"},
        {"ledger_pvt_event_verify_add", (PyCFunction)dap_ledger_pvt_event_verify_add_py, METH_VARARGS,
         "Private: verify and add event"},
        {"ledger_pvt_event_remove", (PyCFunction)dap_ledger_pvt_event_remove_py, METH_VARARGS,
         "Private: remove event"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
