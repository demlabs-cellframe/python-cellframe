#include "cellframe.h"
#include "cf_services_ext.h"
#include "cf_services_ext_helpers.h"
#include "dap_chain_net_srv_stake_ext.h"
#include "../../datum/cf_datum_internal.h"

// =============================================================================
// STAKE EXT
// =============================================================================

PyObject* dap_chain_net_srv_stake_ext_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    int l_res = dap_chain_net_srv_stake_ext_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_srv_stake_ext_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    dap_chain_net_srv_stake_ext_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_stake_ext_find_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj = NULL;
    PyObject *a_hash_obj = NULL;
    int a_raw = 0;
    if (!PyArg_ParseTuple(a_args, "OO|p", &a_net_obj, &a_hash_obj, &a_raw))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(a_hash_obj, &l_hash) != 0) {
        return NULL;
    }
    
    dap_chain_net_srv_stake_ext_t *l_item = dap_chain_net_srv_stake_ext_find(l_net, &l_hash);
    if (!l_item) Py_RETURN_NONE;

    if (a_raw) {
        return PyCapsule_New(l_item, "dap_chain_net_srv_stake_ext_t", NULL);
    }

    PyObject *l_dict = cf_services_stake_ext_to_dict(l_item, false);
    dap_chain_net_srv_stake_ext_delete(l_item);
    return l_dict;
}

PyObject* dap_chain_net_srv_stake_ext_get_list_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj = NULL;
    int a_status = DAP_STAKE_EXT_STATUS_UNKNOWN;
    int a_include_positions = 0;
    if (!PyArg_ParseTuple(a_args, "O|ip", &a_net_obj, &a_status, &a_include_positions))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    dap_list_t *l_list = dap_chain_net_srv_stake_ext_get_list(
        l_net, (dap_chain_srv_stake_ext_status_t)a_status, a_include_positions ? true : false);
    
    PyObject *l_py_list = PyList_New(0);
    for (dap_list_t *l_ptr = l_list; l_ptr; l_ptr = l_ptr->next) {
        dap_chain_net_srv_stake_ext_t *l_item = (dap_chain_net_srv_stake_ext_t*)l_ptr->data;
        PyObject *l_dict = cf_services_stake_ext_to_dict(l_item, a_include_positions ? true : false);
        if (l_dict) {
            PyList_Append(l_py_list, l_dict);
            Py_DECREF(l_dict);
        }
        dap_chain_net_srv_stake_ext_delete(l_item);
    }
    dap_list_free(l_list);
    return l_py_list;
}

PyObject* dap_chain_net_srv_stake_ext_get_detailed_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj = NULL;
    PyObject *a_hash_obj = NULL;
    int a_raw = 0;
    if (!PyArg_ParseTuple(a_args, "OO|p", &a_net_obj, &a_hash_obj, &a_raw)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_hash_fast_t l_hash = {};
    if (cf_datum_parse_hash_fast(a_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_chain_net_srv_stake_ext_t *l_item = dap_chain_net_srv_stake_ext_get_detailed(l_net, &l_hash);
    if (!l_item) {
        Py_RETURN_NONE;
    }

    if (a_raw) {
        return PyCapsule_New(l_item, "dap_chain_net_srv_stake_ext_t", NULL);
    }

    PyObject *l_dict = cf_services_stake_ext_to_dict(l_item, true);
    dap_chain_net_srv_stake_ext_delete(l_item);
    return l_dict;
}

PyObject* dap_chain_net_srv_stake_ext_get_stats_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj;
    if (!PyArg_ParseTuple(a_args, "O", &a_net_obj))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    dap_chain_srv_stake_ext_stats_t *l_stats = dap_chain_net_srv_stake_ext_get_stats(l_net);
    if (!l_stats) Py_RETURN_NONE;
    
    PyObject *l_dict = PyDict_New();
    PyObject *l_total = PyLong_FromLong(l_stats->total_stake_ext);
    PyObject *l_active = PyLong_FromLong(l_stats->active_stake_ext);
    PyObject *l_ended = PyLong_FromLong(l_stats->ended_stake_ext);
    PyObject *l_cancelled = PyLong_FromLong(l_stats->cancelled_stake_ext);
    PyObject *l_total_locks = PyLong_FromLong(l_stats->total_locks);
    PyObject *l_total_positions = PyLong_FromLong(l_stats->total_positions);
    PyDict_SetItemString(l_dict, "total_stake_ext", l_total);
    PyDict_SetItemString(l_dict, "active_stake_ext", l_active);
    PyDict_SetItemString(l_dict, "ended_stake_ext", l_ended);
    PyDict_SetItemString(l_dict, "cancelled_stake_ext", l_cancelled);
    PyDict_SetItemString(l_dict, "total_locks", l_total_locks);
    PyDict_SetItemString(l_dict, "total_positions", l_total_positions);
    Py_DECREF(l_total);
    Py_DECREF(l_active);
    Py_DECREF(l_ended);
    Py_DECREF(l_cancelled);
    Py_DECREF(l_total_locks);
    Py_DECREF(l_total_positions);
    DAP_DELETE(l_stats);
    return l_dict;
}

PyObject* dap_chain_net_srv_stake_ext_lock_create_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj, *a_key_obj;
    char *a_stake_hash_str;
    PyObject *a_amount_obj;
    long long a_lock_time;
    unsigned int a_position_id;
    PyObject *a_fee_obj;
    
    if (!PyArg_ParseTuple(a_args, "OOsOLiO", &a_net_obj, &a_key_obj, &a_stake_hash_str, &a_amount_obj, &a_lock_time, &a_position_id, &a_fee_obj))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj) || !PyCapsule_CheckExact(a_key_obj)) {
        PyErr_SetString(PyExc_TypeError, "Invalid capsule arguments");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    dap_enc_key_t *l_key = (dap_enc_key_t *)PyCapsule_GetPointer(a_key_obj, "dap_enc_key_t");
    dap_chain_hash_fast_t l_stake_hash;
    dap_chain_hash_fast_from_str(a_stake_hash_str, &l_stake_hash);
    
    uint256_t l_amount = py_obj_to_uint256(a_amount_obj);
    uint256_t l_fee = py_obj_to_uint256(a_fee_obj);
    
    int l_ret_code = 0;
    char *l_tx_hash = dap_chain_net_srv_stake_ext_lock_create(l_net, l_key, &l_stake_hash, l_amount, (dap_time_t)a_lock_time, (uint32_t)a_position_id, l_fee, &l_ret_code);
    
    if (!l_tx_hash) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create lock transaction");
        return NULL;
    }
    
    PyObject *l_result = PyUnicode_FromString(l_tx_hash);
    DAP_DELETE(l_tx_hash);
    return l_result;
}

PyObject* dap_chain_net_srv_stake_ext_unlock_create_py(PyObject *a_self, PyObject *a_args) {
    PyObject *a_net_obj, *a_key_obj;
    char *a_lock_hash_str;
    PyObject *a_fee_obj;
    
    if (!PyArg_ParseTuple(a_args, "OOsO", &a_net_obj, &a_key_obj, &a_lock_hash_str, &a_fee_obj))
        return NULL;
        
    if (!PyCapsule_CheckExact(a_net_obj) || !PyCapsule_CheckExact(a_key_obj)) {
        PyErr_SetString(PyExc_TypeError, "Invalid capsule arguments");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(a_net_obj, "dap_chain_net_t");
    dap_enc_key_t *l_key = (dap_enc_key_t *)PyCapsule_GetPointer(a_key_obj, "dap_enc_key_t");
    dap_chain_hash_fast_t l_lock_hash;
    dap_chain_hash_fast_from_str(a_lock_hash_str, &l_lock_hash);
    
    uint256_t l_fee = py_obj_to_uint256(a_fee_obj);
    uint256_t l_value = {0};
    int l_ret_code = 0;
    
    char *l_tx_hash = dap_chain_net_srv_stake_ext_unlock_create(l_net, l_key, &l_lock_hash, l_fee, &l_value, &l_ret_code);
    
    if (!l_tx_hash) {
         PyErr_SetString(PyExc_RuntimeError, "Failed to create unlock transaction");
         return NULL;
    }
    
    PyObject *l_result = PyUnicode_FromString(l_tx_hash);
    DAP_DELETE(l_tx_hash);
    return l_result;
}

PyObject* dap_chain_net_srv_stake_ext_delete_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *a_stake_ext_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &a_stake_ext_obj))
        return NULL;

    if (!PyCapsule_CheckExact(a_stake_ext_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a stake_ext capsule");
        return NULL;
    }

    dap_chain_net_srv_stake_ext_t *l_item = (dap_chain_net_srv_stake_ext_t *)PyCapsule_GetPointer(
        a_stake_ext_obj, "dap_chain_net_srv_stake_ext_t");
    if (!l_item) {
        PyErr_SetString(PyExc_ValueError, "Invalid stake_ext capsule");
        return NULL;
    }

    dap_chain_net_srv_stake_ext_delete(l_item);
    Py_RETURN_NONE;
}

// =============================================================================
// STAKE EXT HELPERS + CACHE
// =============================================================================

PyObject* dap_chain_srv_stake_ext_status_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int a_status = 0;
    if (!PyArg_ParseTuple(a_args, "i", &a_status))
        return NULL;

    const char *l_str = dap_chain_srv_stake_ext_status_to_str((dap_chain_srv_stake_ext_status_t)a_status);
    return PyUnicode_FromString(l_str ? l_str : "");
}

PyObject* dap_chain_srv_stake_ext_status_from_event_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int a_event_type = 0;
    if (!PyArg_ParseTuple(a_args, "i", &a_event_type))
        return NULL;

    dap_chain_srv_stake_ext_status_t l_status = dap_chain_srv_stake_ext_status_from_event_type((uint16_t)a_event_type);
    return PyLong_FromLong((long)l_status);
}

PyObject* dap_chain_tx_event_data_time_unit_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int a_time_unit = 0;
    if (!PyArg_ParseTuple(a_args, "i", &a_time_unit))
        return NULL;

    const char *l_str = dap_chain_tx_event_data_time_unit_to_str((dap_chain_tx_event_data_time_unit_t)a_time_unit);
    return PyUnicode_FromString(l_str ? l_str : "");
}

PyObject* dap_chain_srv_stake_ext_started_tx_event_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int a_multiplier = 0;
    unsigned long long a_duration = 0;
    int a_time_unit = 0;
    unsigned int a_calculation_rule_id = 0;
    unsigned int a_total_positions = 0;
    PyObject *a_positions_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "IKiIIO", &a_multiplier, &a_duration, &a_time_unit,
                          &a_calculation_rule_id, &a_total_positions, &a_positions_obj))
        return NULL;

    if (!PyList_Check(a_positions_obj)) {
        PyErr_SetString(PyExc_TypeError, "positions must be a list of integers");
        return NULL;
    }

    Py_ssize_t l_list_size = PyList_Size(a_positions_obj);
    if ((unsigned int)l_list_size != a_total_positions) {
        PyErr_SetString(PyExc_ValueError, "positions length must match total_positions");
        return NULL;
    }

    if (a_total_positions > UINT8_MAX) {
        PyErr_SetString(PyExc_ValueError, "total_positions must be <= 255");
        return NULL;
    }

    uint32_t *l_positions = NULL;
    if (a_total_positions > 0) {
        l_positions = DAP_NEW_Z_SIZE(uint32_t, sizeof(uint32_t) * a_total_positions);
        if (!l_positions) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate positions array");
            return NULL;
        }
        for (Py_ssize_t i = 0; i < l_list_size; i++) {
            PyObject *l_item = PyList_GetItem(a_positions_obj, i);
            unsigned long l_val = PyLong_AsUnsignedLong(l_item);
            if (PyErr_Occurred()) {
                DAP_DELETE(l_positions);
                return NULL;
            }
            l_positions[i] = (uint32_t)l_val;
        }
    }

    size_t l_data_size = 0;
    byte_t *l_data = dap_chain_srv_stake_ext_started_tx_event_create(
        &l_data_size, a_multiplier, (dap_time_t)a_duration,
        (dap_chain_tx_event_data_time_unit_t)a_time_unit,
        a_calculation_rule_id, (uint8_t)a_total_positions, l_positions);

    DAP_DELETE(l_positions);

    if (!l_data) {
        Py_RETURN_NONE;
    }

    PyObject *l_bytes = PyBytes_FromStringAndSize((const char *)l_data, (Py_ssize_t)l_data_size);
    DAP_DELETE(l_data);
    return l_bytes;
}

PyObject* dap_chain_srv_stake_ext_ended_tx_event_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long a_end_time = 0;
    unsigned int a_winners_cnt = 0;
    PyObject *a_winners_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "KIO", &a_end_time, &a_winners_cnt, &a_winners_obj))
        return NULL;

    if (!PyList_Check(a_winners_obj)) {
        PyErr_SetString(PyExc_TypeError, "winners must be a list of integers");
        return NULL;
    }

    Py_ssize_t l_list_size = PyList_Size(a_winners_obj);
    if ((unsigned int)l_list_size != a_winners_cnt) {
        PyErr_SetString(PyExc_ValueError, "winners length must match winners_cnt");
        return NULL;
    }

    if (a_winners_cnt > UINT8_MAX) {
        PyErr_SetString(PyExc_ValueError, "winners_cnt must be <= 255");
        return NULL;
    }

    uint32_t *l_winners = NULL;
    if (a_winners_cnt > 0) {
        l_winners = DAP_NEW_Z_SIZE(uint32_t, sizeof(uint32_t) * a_winners_cnt);
        if (!l_winners) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate winners array");
            return NULL;
        }
        for (Py_ssize_t i = 0; i < l_list_size; i++) {
            PyObject *l_item = PyList_GetItem(a_winners_obj, i);
            unsigned long l_val = PyLong_AsUnsignedLong(l_item);
            if (PyErr_Occurred()) {
                DAP_DELETE(l_winners);
                return NULL;
            }
            l_winners[i] = (uint32_t)l_val;
        }
    }

    size_t l_data_size = 0;
    byte_t *l_data = dap_chain_srv_stake_ext_ended_tx_event_create(
        &l_data_size, (dap_time_t)a_end_time, (uint8_t)a_winners_cnt, l_winners);

    DAP_DELETE(l_winners);

    if (!l_data) {
        Py_RETURN_NONE;
    }

    PyObject *l_bytes = PyBytes_FromStringAndSize((const char *)l_data, (Py_ssize_t)l_data_size);
    DAP_DELETE(l_data);
    return l_bytes;
}

