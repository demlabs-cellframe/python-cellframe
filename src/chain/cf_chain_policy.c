#include "cf_chain_internal.h"
#include "dap_chain_policy.h"

#define LOG_TAG "python_cellframe_chain"

// =========================================
// CHAIN POLICY OPERATIONS
// =========================================

PyObject* dap_chain_policy_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_result = dap_chain_policy_init();
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_policy_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_policy_deinit();
    Py_RETURN_NONE;
}



PyObject* dap_chain_policy_create_activate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_num = 0;
    long long l_ts_start = 0;
    unsigned long long l_block_start = 0;
    unsigned long long l_chain_id = 0;
    unsigned int l_generation = 0;
    
    if (!PyArg_ParseTuple(a_args, "ILKKI", &l_num, &l_ts_start, &l_block_start, &l_chain_id, &l_generation)) {
        return NULL;
    }
    
    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    dap_chain_policy_t *l_policy = dap_chain_policy_create_activate(l_num, l_ts_start, l_block_start, l_chain_id_struct,
                                                                    (uint16_t)l_generation);
    if (!l_policy) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create activate policy");
        return NULL;
    }
    
    return PyCapsule_New(l_policy, "dap_chain_policy_t", NULL);
}



PyObject* dap_chain_policy_create_deactivate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_nums_obj = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_nums_obj)) {
        return NULL;
    }
    
    PyObject *l_seq = PySequence_Fast(l_nums_obj, "Expected a sequence of policy numbers");
    if (!l_seq) {
        return NULL;
    }
    
    Py_ssize_t l_count = PySequence_Fast_GET_SIZE(l_seq);
    if (l_count <= 0) {
        Py_DECREF(l_seq);
        PyErr_SetString(PyExc_ValueError, "Policy numbers sequence must not be empty");
        return NULL;
    }
    
    char **l_nums = DAP_NEW_Z_COUNT(char *, (size_t)l_count);
    PyObject **l_keepalive = DAP_NEW_Z_COUNT(PyObject *, (size_t)l_count);
    if (!l_nums || !l_keepalive) {
        Py_DECREF(l_seq);
        DAP_DELETE(l_nums);
        DAP_DELETE(l_keepalive);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate policy numbers array");
        return NULL;
    }
    
    for (Py_ssize_t i = 0; i < l_count; i++) {
        PyObject *l_item = PySequence_Fast_GET_ITEM(l_seq, i);
        PyObject *l_str_obj = PyObject_Str(l_item);
        if (!l_str_obj) {
            for (Py_ssize_t j = 0; j < i; j++) {
                Py_XDECREF(l_keepalive[j]);
            }
            Py_DECREF(l_seq);
            DAP_DELETE(l_nums);
            DAP_DELETE(l_keepalive);
            return NULL;
        }
        l_keepalive[i] = l_str_obj;
        const char *l_str = PyUnicode_AsUTF8(l_str_obj);
        if (!l_str) {
            for (Py_ssize_t j = 0; j <= i; j++) {
                Py_XDECREF(l_keepalive[j]);
            }
            Py_DECREF(l_seq);
            DAP_DELETE(l_nums);
            DAP_DELETE(l_keepalive);
            return NULL;
        }
        l_nums[i] = (char *)l_str;
    }
    
    dap_chain_policy_t *l_policy = dap_chain_policy_create_deactivate(l_nums, (uint32_t)l_count);
    
    for (Py_ssize_t i = 0; i < l_count; i++) {
        Py_XDECREF(l_keepalive[i]);
    }
    Py_DECREF(l_seq);
    DAP_DELETE(l_nums);
    DAP_DELETE(l_keepalive);
    
    if (!l_policy) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create deactivate policy");
        return NULL;
    }
    
    return PyCapsule_New(l_policy, "dap_chain_policy_t", NULL);
}



PyObject* dap_chain_policy_net_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    PyObject *l_config_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "K|O", &l_net_id, &l_config_obj)) {
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
    int l_result = dap_chain_policy_net_add(l_net_id_struct, l_config);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_policy_net_purge_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_policy_net_purge(l_net_id_struct);
    Py_RETURN_NONE;
}



PyObject* dap_chain_policy_net_remove_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_policy_net_remove(l_net_id_struct);
    Py_RETURN_NONE;
}



PyObject* dap_chain_policy_apply_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_policy_capsule = NULL;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "OK", &l_policy_capsule, &l_net_id)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_policy_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected policy capsule");
        return NULL;
    }
    
    dap_chain_policy_t *l_policy = (dap_chain_policy_t*)PyCapsule_GetPointer(l_policy_capsule, "dap_chain_policy_t");
    if (!l_policy) {
        PyErr_SetString(PyExc_ValueError, "Invalid policy capsule");
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_result = dap_chain_policy_apply(l_policy, l_net_id_struct);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_policy_update_last_num_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned int l_num = 0;
    
    if (!PyArg_ParseTuple(a_args, "KI", &l_net_id, &l_num)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_policy_update_last_num(l_net_id_struct, l_num);
    Py_RETURN_NONE;
}



PyObject* dap_chain_policy_get_last_num_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    
    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    uint32_t l_num = dap_chain_policy_get_last_num(l_net_id_struct);
    return PyLong_FromUnsignedLong(l_num);
}



PyObject* dap_chain_policy_activate_json_collect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned int l_num = 0;
    
    if (!PyArg_ParseTuple(a_args, "KI", &l_net_id, &l_num)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_json_t *l_json = dap_chain_policy_activate_json_collect(l_net_id_struct, l_num);
    if (!l_json) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}



PyObject* dap_chain_policy_json_collect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_policy_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_policy_capsule)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_policy_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected policy capsule");
        return NULL;
    }
    
    dap_chain_policy_t *l_policy = (dap_chain_policy_t*)PyCapsule_GetPointer(l_policy_capsule, "dap_chain_policy_t");
    if (!l_policy) {
        PyErr_SetString(PyExc_ValueError, "Invalid policy capsule");
        return NULL;
    }
    
    dap_json_t *l_json = dap_chain_policy_json_collect(l_policy);
    if (!l_json) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}



PyObject* dap_chain_policy_list_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    int l_version = 0;
    
    if (!PyArg_ParseTuple(a_args, "Ki", &l_net_id, &l_version)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_json_t *l_json = dap_chain_policy_list(l_net_id_struct, l_version);
    if (!l_json) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_json, "dap_json_t", NULL);
}



PyObject* dap_chain_policy_is_exist_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned int l_num = 0;
    
    if (!PyArg_ParseTuple(a_args, "KI", &l_net_id, &l_num)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    bool l_result = dap_chain_policy_is_exist(l_net_id_struct, l_num);
    return PyBool_FromLong(l_result ? 1 : 0);
}



PyObject* dap_chain_policy_is_activated_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned int l_num = 0;
    
    if (!PyArg_ParseTuple(a_args, "KI", &l_net_id, &l_num)) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    bool l_result = dap_chain_policy_is_activated(l_net_id_struct, l_num);
    return PyBool_FromLong(l_result ? 1 : 0);
}



PyObject* dap_chain_policy_get_size_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_policy_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_policy_capsule)) {
        return NULL;
    }
    
    dap_chain_policy_t *l_policy = NULL;
    if (l_policy_capsule != Py_None) {
        if (!PyCapsule_CheckExact(l_policy_capsule)) {
            PyErr_SetString(PyExc_TypeError, "Expected policy capsule or None");
            return NULL;
        }
        l_policy = (dap_chain_policy_t*)PyCapsule_GetPointer(l_policy_capsule, "dap_chain_policy_t");
        if (!l_policy) {
            PyErr_SetString(PyExc_ValueError, "Invalid policy capsule");
            return NULL;
        }
    }
    
    size_t l_size = dap_chain_policy_get_size(l_policy);
    return PyLong_FromSize_t(l_size);
}



PyObject* dap_chain_policy_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_policy_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_policy_capsule)) {
        return NULL;
    }
    
    dap_chain_policy_t *l_policy = NULL;
    if (l_policy_capsule != Py_None) {
        if (!PyCapsule_CheckExact(l_policy_capsule)) {
            PyErr_SetString(PyExc_TypeError, "Expected policy capsule or None");
            return NULL;
        }
        l_policy = (dap_chain_policy_t*)PyCapsule_GetPointer(l_policy_capsule, "dap_chain_policy_t");
        if (!l_policy) {
            PyErr_SetString(PyExc_ValueError, "Invalid policy capsule");
            return NULL;
        }
    }
    
    const char *l_str = dap_chain_policy_to_str(l_policy);
    return PyUnicode_FromString(l_str ? l_str : "<null>");
}

// =========================================
// CHAIN SERVICE OPERATIONS
// =========================================
