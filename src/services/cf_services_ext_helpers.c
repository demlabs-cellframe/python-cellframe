#include "cellframe.h"
#include "cf_services_ext_helpers.h"

#define LOG_TAG "python_cellframe_services_ext"

// Convert PyObject to uint256_t via string representation
uint256_t py_obj_to_uint256(PyObject *a_obj) {
    PyObject *str_obj = PyObject_Str(a_obj);
    if (!str_obj) {
        return uint256_0;
    }

    const char *str = PyUnicode_AsUTF8(str_obj);
    uint256_t result = str ? dap_chain_balance_scan(str) : uint256_0;
    Py_DECREF(str_obj);

    return result;
}

PyObject *cf_services_uint256_to_py_str(uint256_t a_value) {
    const char *l_value_str = dap_uint256_to_char(a_value, NULL);
    return PyUnicode_FromString(l_value_str ? l_value_str : "0");
}

PyObject *cf_services_stake_item_to_dict(const dap_chain_net_srv_stake_item_t *a_item) {
    if (!a_item) {
        Py_RETURN_NONE;
    }

    PyObject *l_dict = PyDict_New();
    if (!l_dict) {
        return NULL;
    }

    PyObject *l_active = PyBool_FromLong(a_item->is_active ? 1 : 0);
    PyDict_SetItemString(l_dict, "is_active", l_active);
    Py_DECREF(l_active);

    if (a_item->net) {
        PyObject *l_net_id = PyLong_FromUnsignedLongLong(a_item->net->pub.id.uint64);
        PyDict_SetItemString(l_dict, "net_id", l_net_id);
        Py_DECREF(l_net_id);
    } else {
        PyDict_SetItemString(l_dict, "net_id", Py_None);
    }

    PyObject *l_value = cf_services_uint256_to_py_str(a_item->value);
    PyDict_SetItemString(l_dict, "value", l_value);
    Py_DECREF(l_value);

    PyObject *l_locked_value = cf_services_uint256_to_py_str(a_item->locked_value);
    PyDict_SetItemString(l_dict, "locked_value", l_locked_value);
    Py_DECREF(l_locked_value);

    const char *l_signing_addr = dap_chain_addr_to_str(&a_item->signing_addr);
    if (l_signing_addr) {
        PyObject *l_addr_obj = PyUnicode_FromString(l_signing_addr);
        PyDict_SetItemString(l_dict, "signing_addr", l_addr_obj);
        Py_DECREF(l_addr_obj);
    } else {
        PyDict_SetItemString(l_dict, "signing_addr", Py_None);
    }

    const char *l_node_addr = dap_stream_node_addr_to_str_static(a_item->node_addr);
    if (l_node_addr) {
        PyObject *l_node_obj = PyUnicode_FromString(l_node_addr);
        PyDict_SetItemString(l_dict, "node_addr", l_node_obj);
        Py_DECREF(l_node_obj);
    } else {
        PyDict_SetItemString(l_dict, "node_addr", Py_None);
    }

    if (dap_hash_fast_is_blank(&a_item->tx_hash.hash)) {
        PyDict_SetItemString(l_dict, "tx_hash", Py_None);
    } else {
        char *l_tx_hash_str = dap_chain_hash_fast_to_str_new(&a_item->tx_hash.hash);
        if (l_tx_hash_str) {
            PyObject *l_hash_obj = PyUnicode_FromString(l_tx_hash_str);
            PyDict_SetItemString(l_dict, "tx_hash", l_hash_obj);
            Py_DECREF(l_hash_obj);
            DAP_DELETE(l_tx_hash_str);
        } else {
            PyDict_SetItemString(l_dict, "tx_hash", Py_None);
        }
    }

    if (dap_hash_fast_is_blank(&a_item->decree_hash.hash)) {
        PyDict_SetItemString(l_dict, "decree_hash", Py_None);
    } else {
        char *l_decree_hash_str = dap_chain_hash_fast_to_str_new(&a_item->decree_hash.hash);
        if (l_decree_hash_str) {
            PyObject *l_hash_obj = PyUnicode_FromString(l_decree_hash_str);
            PyDict_SetItemString(l_dict, "decree_hash", l_hash_obj);
            Py_DECREF(l_hash_obj);
            DAP_DELETE(l_decree_hash_str);
        } else {
            PyDict_SetItemString(l_dict, "decree_hash", Py_None);
        }
    }

    if (!dap_chain_addr_is_blank(&a_item->sovereign_addr)) {
        const char *l_sovereign_addr = dap_chain_addr_to_str(&a_item->sovereign_addr);
        if (l_sovereign_addr) {
            PyObject *l_addr_obj = PyUnicode_FromString(l_sovereign_addr);
            PyDict_SetItemString(l_dict, "sovereign_addr", l_addr_obj);
            Py_DECREF(l_addr_obj);
        } else {
            PyDict_SetItemString(l_dict, "sovereign_addr", Py_None);
        }
    } else {
        PyDict_SetItemString(l_dict, "sovereign_addr", Py_None);
    }

    PyObject *l_sovereign_tax = cf_services_uint256_to_py_str(a_item->sovereign_tax);
    PyDict_SetItemString(l_dict, "sovereign_tax", l_sovereign_tax);
    Py_DECREF(l_sovereign_tax);

    if (a_item->pkey) {
        PyObject *l_pkey = PyCapsule_New(a_item->pkey, "dap_pkey_t", NULL);
        PyDict_SetItemString(l_dict, "pkey", l_pkey);
        Py_DECREF(l_pkey);
    } else {
        PyDict_SetItemString(l_dict, "pkey", Py_None);
    }

    return l_dict;
}

PyObject *cf_services_stake_ext_to_dict(const dap_chain_net_srv_stake_ext_t *a_item, bool a_include_positions) {
    if (!a_item) {
        Py_RETURN_NONE;
    }

    PyObject *l_dict = PyDict_New();
    if (!l_dict) {
        return NULL;
    }

    char *l_hash_str = dap_chain_hash_fast_to_str_new(&a_item->stake_ext_hash);
    if (l_hash_str) {
        PyObject *l_hash_obj = PyUnicode_FromString(l_hash_str);
        PyDict_SetItemString(l_dict, "hash", l_hash_obj);
        Py_DECREF(l_hash_obj);
        DAP_DELETE(l_hash_str);
    } else {
        PyDict_SetItemString(l_dict, "hash", Py_None);
    }

    PyObject *l_status = PyLong_FromLong(a_item->status);
    PyDict_SetItemString(l_dict, "status", l_status);
    Py_DECREF(l_status);

    const char *l_name = a_item->guuid ? a_item->guuid : "";
    PyDict_SetItemString(l_dict, "name", PyUnicode_FromString(l_name));

    if (a_item->description) {
        PyDict_SetItemString(l_dict, "description", PyUnicode_FromString(a_item->description));
    } else {
        PyDict_SetItemString(l_dict, "description", Py_None);
    }

    PyDict_SetItemString(l_dict, "total_value", Py_None);

    PyObject *l_time_start = PyLong_FromUnsignedLongLong((unsigned long long)a_item->start_time);
    PyDict_SetItemString(l_dict, "time_start", l_time_start);
    Py_DECREF(l_time_start);

    PyObject *l_time_end = PyLong_FromUnsignedLongLong((unsigned long long)a_item->end_time);
    PyDict_SetItemString(l_dict, "time_end", l_time_end);
    Py_DECREF(l_time_end);

    PyDict_SetItemString(l_dict, "lock_duration", Py_None);
    PyDict_SetItemString(l_dict, "lock_unix_time", Py_None);
    PyDict_SetItemString(l_dict, "lock_state", Py_None);
    PyDict_SetItemString(l_dict, "reward_percent", Py_None);
    PyDict_SetItemString(l_dict, "penalty_percent", Py_None);

    PyObject *l_position_count = PyLong_FromUnsignedLong((unsigned long)a_item->positions_count);
    PyDict_SetItemString(l_dict, "positions_count", l_position_count);
    Py_DECREF(l_position_count);

    PyObject *l_locks_total = PyLong_FromUnsignedLong((unsigned long)a_item->locks_count);
    PyDict_SetItemString(l_dict, "locks_count", l_locks_total);
    Py_DECREF(l_locks_total);

    PyObject *l_has_winner = PyBool_FromLong(a_item->has_winner ? 1 : 0);
    PyDict_SetItemString(l_dict, "has_winner", l_has_winner);
    Py_DECREF(l_has_winner);

    if (a_item->winners_cnt && a_item->winners_ids) {
        PyObject *l_winners = PyList_New((Py_ssize_t)a_item->winners_cnt);
        for (uint8_t i = 0; i < a_item->winners_cnt; i++) {
            PyObject *l_winner = PyLong_FromUnsignedLong((unsigned long)a_item->winners_ids[i]);
            PyList_SetItem(l_winners, (Py_ssize_t)i, l_winner);
        }
        PyDict_SetItemString(l_dict, "winners_ids", l_winners);
        Py_DECREF(l_winners);
    } else {
        PyDict_SetItemString(l_dict, "winners_ids", Py_None);
    }

    PyDict_SetItemString(l_dict, "signing_addr", Py_None);
    PyDict_SetItemString(l_dict, "pkey", Py_None);

    if (a_include_positions) {
        PyObject *l_positions = PyList_New((Py_ssize_t)a_item->positions_count);
        for (size_t i = 0; i < a_item->positions_count; i++) {
            dap_chain_net_srv_stake_ext_position_t *l_pos = &a_item->positions[i];
            PyObject *l_pos_dict = PyDict_New();

            PyObject *l_pos_id = PyLong_FromUnsignedLongLong((unsigned long long)l_pos->position_id);
            PyDict_SetItemString(l_pos_dict, "id", l_pos_id);
            Py_DECREF(l_pos_id);

            PyDict_SetItemString(l_pos_dict, "start_time", Py_None);
            PyDict_SetItemString(l_pos_dict, "end_time", Py_None);
            PyDict_SetItemString(l_pos_dict, "reward", Py_None);
            PyDict_SetItemString(l_pos_dict, "penalty", Py_None);

            PyObject *l_total_amount = cf_services_uint256_to_py_str(l_pos->total_amount);
            PyDict_SetItemString(l_pos_dict, "total_amount", l_total_amount);
            Py_DECREF(l_total_amount);

            PyObject *l_locks = PyLong_FromUnsignedLong((unsigned long)l_pos->locks_count);
            PyDict_SetItemString(l_pos_dict, "locks_count", l_locks);
            Py_DECREF(l_locks);

            PyObject *l_active = PyLong_FromUnsignedLong((unsigned long)l_pos->active_locks_count);
            PyDict_SetItemString(l_pos_dict, "active_locks_count", l_active);
            Py_DECREF(l_active);

            PyList_SetItem(l_positions, (Py_ssize_t)i, l_pos_dict);
        }
        PyDict_SetItemString(l_dict, "positions", l_positions);
        Py_DECREF(l_positions);
    }

    return l_dict;
}

dap_chain_net_t *cf_services_get_net(PyObject *a_obj) {
    if (!PyCapsule_CheckExact(a_obj)) {
        PyErr_SetString(PyExc_TypeError, "Network capsule is required");
        return NULL;
    }
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    return l_net;
}
