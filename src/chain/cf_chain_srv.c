#include "cf_chain_internal.h"
#include "dap_chain_srv.h"
#include "dap_json.h"

#define LOG_TAG "python_cellframe_chain"

// =========================================
// CHAIN SERVICE OPERATIONS
// =========================================

PyObject* dap_chain_srv_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_result = dap_chain_srv_init();
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_srv_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_srv_deinit();
    Py_RETURN_NONE;
}



PyObject* dap_chain_srv_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_uid = 0;
    PyObject *l_name_obj = NULL;
    PyObject *l_callbacks_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "KO|O", &l_uid, &l_name_obj, &l_callbacks_obj)) {
        return NULL;
    }
    
    const char *l_name = NULL;
    if (l_name_obj && l_name_obj != Py_None) {
        if (!PyUnicode_Check(l_name_obj)) {
            PyErr_SetString(PyExc_TypeError, "Service name must be a string or None");
            return NULL;
        }
        l_name = PyUnicode_AsUTF8(l_name_obj);
        if (!l_name) {
            return NULL;
        }
    }
    
    dap_chain_static_srv_callbacks_t *l_callbacks = NULL;
    if (l_callbacks_obj && l_callbacks_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_callbacks_obj)) {
            PyErr_SetString(PyExc_TypeError, "Callbacks must be a capsule or None");
            return NULL;
        }
        l_callbacks = (dap_chain_static_srv_callbacks_t*)PyCapsule_GetPointer(l_callbacks_obj, "dap_chain_static_srv_callbacks_t");
        if (!l_callbacks) {
            PyErr_SetString(PyExc_ValueError, "Invalid callbacks capsule");
            return NULL;
        }
    }
    
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    int l_result = dap_chain_srv_add(l_uid_struct, l_name, l_callbacks);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_srv_start_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    const char *l_name = NULL;
    PyObject *l_config_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "Ks|O", &l_net_id, &l_name, &l_config_obj)) {
        return NULL;
    }
    
    dap_config_t *l_config = g_config;
    if (l_config_obj && l_config_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_config_obj)) {
            PyErr_SetString(PyExc_TypeError, "Config must be a capsule or None");
            return NULL;
        }
        l_config = (dap_config_t *)PyCapsule_GetPointer(l_config_obj, "dap_config_t");
        if (!l_config) {
            PyErr_SetString(PyExc_ValueError, "Invalid config capsule");
            return NULL;
        }
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_result = dap_chain_srv_start(l_net_id_struct, l_name, l_config);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_srv_start_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_result = dap_chain_srv_start_all(l_net_id_struct);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_srv_delete_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_uid = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_uid)) {
        return NULL;
    }
    
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    int l_result = dap_chain_srv_delete(l_uid_struct);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_srv_get_internal_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_uid = 0;
    
    if (!PyArg_ParseTuple(a_args, "KK", &l_net_id, &l_uid)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    void *l_internal = dap_chain_srv_get_internal(l_net_id_struct, l_uid_struct);
    if (!l_internal) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_internal, "dap_chain_srv_internal_t", NULL);
}



PyObject* dap_chain_srv_get_uid_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name = NULL;
    
    if (!PyArg_ParseTuple(a_args, "s", &l_name)) {
        return NULL;
    }
    
    dap_chain_srv_uid_t l_uid = dap_chain_srv_get_uid_by_name(l_name);
    return PyLong_FromUnsignedLongLong(l_uid.uint64);
}



PyObject* dap_chain_srv_count_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    size_t l_count = dap_chain_srv_count(l_net_id_struct);
    return PyLong_FromSize_t(l_count);
}



PyObject* dap_chain_srv_list_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_list_t *l_list = dap_chain_srv_list(l_net_id_struct);
    if (!l_list) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}



PyObject* dap_chain_srv_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_uid = 0;
    
    if (!PyArg_ParseTuple(a_args, "KK", &l_net_id, &l_uid)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    dap_list_t *l_services = dap_chain_srv_list(l_net_id_struct);
    bool l_found = false;
    for (dap_list_t *it = l_services; it; it = it->next) {
        dap_chain_srv_uid_t *l_item_uid = it->data;
        if (l_item_uid && l_item_uid->uint64 == l_uid_struct.uint64) {
            l_found = true;
            break;
        }
    }
    dap_list_free_full(l_services, NULL);
    if (!l_found) {
        return PyLong_FromLong(0);
    }
    int l_result = dap_chain_srv_purge(l_net_id_struct, l_uid_struct);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_srv_purge_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_result = dap_chain_srv_purge_all(l_net_id_struct);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_srv_hardfork_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_hardfork_state_t *l_state = dap_chain_srv_hardfork_all(l_net_id_struct);
    if (!l_state) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_state, "dap_chain_srv_hardfork_state_t", NULL);
}



PyObject* dap_chain_srv_load_state_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_uid = 0;
    const char *l_state = NULL;
    Py_ssize_t l_state_size = 0;
    unsigned int l_state_count = 0;
    
    if (!PyArg_ParseTuple(a_args, "KKy#I", &l_net_id, &l_uid, &l_state, &l_state_size, &l_state_count)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    int l_result = dap_chain_srv_load_state(l_net_id_struct, l_uid_struct, (byte_t *)l_state,
                                            (uint64_t)l_state_size, l_state_count);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_srv_hardfork_complete_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_hardfork_complete_all(l_net_id_struct);
    Py_RETURN_NONE;
}



PyObject* dap_chain_srv_event_verify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_uid = 0;
    const char *l_event_group = NULL;
    int l_event_type = 0;
    const char *l_event_data = NULL;
    Py_ssize_t l_event_data_size = 0;
    PyObject *l_hash_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "KKsiy#O", &l_net_id, &l_uid, &l_event_group, &l_event_type,
                          &l_event_data, &l_event_data_size, &l_hash_obj)) {
        return NULL;
    }
    
    dap_hash_fast_t *l_hash = NULL;
    if (cf_chain_parse_hash(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    int l_result = dap_chain_srv_event_verify(l_net_id_struct, l_uid_struct, l_event_group, l_event_type,
                                              (void *)l_event_data, (size_t)l_event_data_size, l_hash);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_srv_decree_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_uid = 0;
    int l_apply = 0;
    PyObject *l_params_obj = Py_None;
    Py_ssize_t l_params_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "KKpOn", &l_net_id, &l_uid, &l_apply, &l_params_obj, &l_params_size)) {
        return NULL;
    }
    
    dap_tsd_t *l_params = NULL;
    if (l_params_obj && l_params_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_params_obj)) {
            PyErr_SetString(PyExc_TypeError, "Params must be a capsule or None");
            return NULL;
        }
        l_params = (dap_tsd_t *)PyCapsule_GetPointer(l_params_obj, "dap_tsd_t");
        if (!l_params) {
            PyErr_SetString(PyExc_ValueError, "Invalid params capsule");
            return NULL;
        }
    }
    
    if (l_params_size < 0) {
        PyErr_SetString(PyExc_ValueError, "Params size must be non-negative");
        return NULL;
    }
    if ((!l_params || l_params_obj == Py_None) && l_params_size > 0) {
        PyErr_SetString(PyExc_ValueError, "Params size must be 0 when params is None");
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_srv_uid_t l_uid_struct = {.uint64 = l_uid};
    int l_result = dap_chain_srv_decree(l_net_id_struct, l_uid_struct, l_apply != 0, l_params, (size_t)l_params_size);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_srv_get_fees_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_json_t *l_json = dap_chain_srv_get_fees(l_net_id_struct);
    if (!l_json) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}



PyObject* dap_chain_srv_fee_type_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_fee_type = 0;
    
    if (!PyArg_ParseTuple(a_args, "I", &l_fee_type)) {
        return NULL;
    }
    
    const char *l_str = dap_chain_srv_fee_type_to_str((dap_chain_srv_fee_type_t)l_fee_type);
    return PyUnicode_FromString(l_str ? l_str : "UNKNOWN");
}

// =========================================
// MODULE INITIALIZATION
// =========================================
// This function registers all chain functions with the Python module
