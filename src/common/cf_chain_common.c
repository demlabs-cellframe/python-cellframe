#include "cf_common_utils.h"
#include "dap_json.h"

#define LOG_TAG "python_cellframe_common"

PyObject *py_dap_chain_addr_from_str(PyObject *self, PyObject *args) {
    (void)self;
    const char *addr_str = NULL;

    if (!PyArg_ParseTuple(args, "s", &addr_str)) {
        return NULL;
    }

    dap_chain_addr_t *addr = dap_chain_addr_from_str(addr_str);
    if (!addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid address string");
        return NULL;
    }

    return cf_common_capsule_new_addr(addr);
}

PyObject *py_dap_chain_addr_is_blank(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &addr_obj)) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_common_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    bool is_blank = dap_chain_addr_is_blank(&addr);
    return PyBool_FromLong(is_blank ? 1 : 0);
}

PyObject *py_dap_chain_addr_check_sum(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &addr_obj)) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_common_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    int result = dap_chain_addr_check_sum(&addr);
    return PyLong_FromLong(result);
}

PyObject *dap_chain_addr_compare_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_addr1_obj = NULL;
    PyObject *l_addr2_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_addr1_obj, &l_addr2_obj)) {
        return NULL;
    }

    dap_chain_addr_t l_addr1 = {0};
    dap_chain_addr_t l_addr2 = {0};
    if (cf_common_parse_addr(l_addr1_obj, &l_addr1) != 0 ||
        cf_common_parse_addr(l_addr2_obj, &l_addr2) != 0) {
        return NULL;
    }

    bool l_equal = dap_chain_addr_compare(&l_addr1, &l_addr2);
    return PyBool_FromLong(l_equal ? 1 : 0);
}

PyObject *dap_chain_addr_fill_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_sign_type = 0;
    PyObject *l_hash_obj = NULL;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "IOK", &l_sign_type, &l_hash_obj, &l_net_id)) {
        return NULL;
    }

    dap_chain_addr_t *l_addr = DAP_NEW_Z(dap_chain_addr_t);
    if (!l_addr) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }

    dap_hash_fast_t l_hash = {0};
    if (cf_common_parse_hash_fast(l_hash_obj, &l_hash) != 0) {
        DAP_DELETE(l_addr);
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_sign_type_t l_type = {.raw = l_sign_type};

    dap_chain_addr_fill(l_addr, l_type, &l_hash, l_net_id_struct);

    return cf_common_capsule_new_addr(l_addr);
}

PyObject *dap_chain_addr_fill_from_key_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_key_obj = NULL;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_key_obj, &l_net_id)) {
        return NULL;
    }

    dap_enc_key_t *l_key = (dap_enc_key_t *)cf_common_capsule_get_pointer(
        l_key_obj, "dap_enc_key_t", "First argument must be a dap_enc_key_t capsule"
    );
    if (!l_key) {
        return NULL;
    }

    dap_chain_addr_t *l_addr = DAP_NEW_Z(dap_chain_addr_t);
    if (!l_addr) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_rc = dap_chain_addr_fill_from_key(l_addr, l_key, l_net_id_struct);

    PyObject *l_tuple = PyTuple_New(2);
    if (!l_tuple) {
        DAP_DELETE(l_addr);
        return NULL;
    }

    PyObject *l_rc_obj = PyLong_FromLong(l_rc);
    if (!l_rc_obj) {
        Py_DECREF(l_tuple);
        DAP_DELETE(l_addr);
        return NULL;
    }
    PyTuple_SetItem(l_tuple, 0, l_rc_obj);

    if (l_rc != 0) {
        DAP_DELETE(l_addr);
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 1, Py_None);
        return l_tuple;
    }

    PyObject *l_capsule = cf_common_capsule_new_addr(l_addr);
    if (!l_capsule) {
        Py_DECREF(l_tuple);
        return NULL;
    }
    PyTuple_SetItem(l_tuple, 1, l_capsule);
    return l_tuple;
}

PyObject *dap_chain_addr_fill_from_sign_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_sign_obj = NULL;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_sign_obj, &l_net_id)) {
        return NULL;
    }

    dap_sign_t *l_sign = (dap_sign_t *)cf_common_capsule_get_pointer(
        l_sign_obj, "dap_sign_t", "First argument must be a dap_sign_t capsule"
    );
    if (!l_sign) {
        return NULL;
    }

    dap_chain_addr_t *l_addr = DAP_NEW_Z(dap_chain_addr_t);
    if (!l_addr) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_rc = dap_chain_addr_fill_from_sign(l_addr, l_sign, l_net_id_struct);

    PyObject *l_tuple = PyTuple_New(2);
    if (!l_tuple) {
        DAP_DELETE(l_addr);
        return NULL;
    }

    PyObject *l_rc_obj = PyLong_FromLong(l_rc);
    if (!l_rc_obj) {
        Py_DECREF(l_tuple);
        DAP_DELETE(l_addr);
        return NULL;
    }
    PyTuple_SetItem(l_tuple, 0, l_rc_obj);

    if (l_rc != 0) {
        DAP_DELETE(l_addr);
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 1, Py_None);
        return l_tuple;
    }

    PyObject *l_capsule = cf_common_capsule_new_addr(l_addr);
    if (!l_capsule) {
        Py_DECREF(l_tuple);
        return NULL;
    }
    PyTuple_SetItem(l_tuple, 1, l_capsule);
    return l_tuple;
}

PyObject *dap_chain_addr_from_str_array_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_addr_str = NULL;

    if (!PyArg_ParseTuple(a_args, "s", &l_addr_str)) {
        return NULL;
    }

    dap_chain_addr_t *l_addr_array = NULL;
    size_t l_count = dap_chain_addr_from_str_array(l_addr_str, &l_addr_array);
    if (!l_count || !l_addr_array) {
        PyErr_SetString(PyExc_ValueError, "Failed to parse address list");
        return NULL;
    }

    PyObject *l_list = PyList_New((Py_ssize_t)l_count);
    if (!l_list) {
        DAP_DELETE(l_addr_array);
        return NULL;
    }

    for (size_t i = 0; i < l_count; i++) {
        dap_chain_addr_t *l_copy = DAP_NEW_Z(dap_chain_addr_t);
        if (!l_copy) {
            PyErr_NoMemory();
            Py_DECREF(l_list);
            DAP_DELETE(l_addr_array);
            return NULL;
        }
        *l_copy = l_addr_array[i];
        PyObject *l_capsule = cf_common_capsule_new_addr(l_copy);
        if (!l_capsule) {
            Py_DECREF(l_list);
            DAP_DELETE(l_addr_array);
            return NULL;
        }
        PyList_SetItem(l_list, (Py_ssize_t)i, l_capsule);
    }

    DAP_DELETE(l_addr_array);
    return l_list;
}

PyObject *dap_chain_addr_to_str_static_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_addr_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_addr_obj)) {
        return NULL;
    }

    dap_chain_addr_t l_addr = {0};
    if (cf_common_parse_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    dap_chain_addr_str_t l_addr_str = dap_chain_addr_to_str_static_(&l_addr);
    return PyUnicode_FromString(l_addr_str.s);
}

PyObject *dap_chain_balance_scan_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_obj)) {
        return NULL;
    }

    PyObject *l_str_obj = PyObject_Str(l_obj);
    if (!l_str_obj) {
        return NULL;
    }
    const char *l_str = PyUnicode_AsUTF8(l_str_obj);
    if (!l_str) {
        Py_DECREF(l_str_obj);
        return NULL;
    }

    uint256_t l_val = dap_chain_balance_scan(l_str);
    Py_DECREF(l_str_obj);
    return PyBytes_FromStringAndSize((const char *)&l_val, sizeof(uint256_t));
}

PyObject *dap_chain_balance_to_coins_uint64_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_val_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_val_obj)) {
        return NULL;
    }

    uint256_t l_val = {0};
    if (cf_common_parse_uint256(l_val_obj, &l_val) != 0) {
        return NULL;
    }

    uint64_t l_out = dap_chain_balance_to_coins_uint64(l_val);
    return PyLong_FromUnsignedLongLong(l_out);
}

PyObject *dap_chain_hash_slow_kind_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_hash_obj = NULL;
    int l_head = 0;

    if (!PyArg_ParseTuple(a_args, "Oi", &l_hash_obj, &l_head)) {
        return NULL;
    }

    dap_chain_hash_slow_t l_hash = {0};
    if (cf_common_parse_hash_slow(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_chain_hash_slow_kind_t l_kind = dap_chain_hash_slow_kind_check(&l_hash, (uint8_t)l_head);
    return PyLong_FromLong((long)l_kind);
}

PyObject *dap_chain_hash_slow_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_hash_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_hash_obj)) {
        return NULL;
    }

    dap_chain_hash_slow_t l_hash = {0};
    if (cf_common_parse_hash_slow(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    size_t l_buf_size = sizeof(dap_chain_hash_slow_t) * 2 + 3;
    char *l_buf = DAP_NEW_Z_SIZE(char, l_buf_size);
    if (!l_buf) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate hash buffer");
        return NULL;
    }

    if (!dap_chain_hash_slow_to_str(&l_hash, l_buf, l_buf_size)) {
        DAP_DELETE(l_buf);
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert hash to string");
        return NULL;
    }

    PyObject *l_str = PyUnicode_FromString(l_buf);
    DAP_DELETE(l_buf);
    return l_str;
}

PyObject *dap_chain_hash_slow_to_str_new_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_hash_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_hash_obj)) {
        return NULL;
    }

    dap_chain_hash_slow_t l_hash = {0};
    if (cf_common_parse_hash_slow(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    char *l_str = dap_chain_hash_slow_to_str_new(&l_hash);
    if (!l_str) {
        Py_RETURN_NONE;
    }

    PyObject *l_py_str = PyUnicode_FromString(l_str);
    DAP_DELETE(l_str);
    return l_py_str;
}

PyObject *dap_chain_mempool_group_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_prefix = NULL;
    const char *l_chain_name = NULL;

    if (!PyArg_ParseTuple(a_args, "ss", &l_prefix, &l_chain_name)) {
        return NULL;
    }

    char *l_name = dap_chain_mempool_group_name(l_prefix, l_chain_name);
    if (!l_name) {
        Py_RETURN_NONE;
    }

    PyObject *l_py_name = PyUnicode_FromString(l_name);
    DAP_DELETE(l_name);
    return l_py_name;
}

PyObject *dap_chain_node_role_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_role = 0;

    if (!PyArg_ParseTuple(a_args, "I", &l_role)) {
        return NULL;
    }

    dap_chain_node_role_t l_role_union = {.enums = l_role};
    const char *l_str = dap_chain_node_role_to_str(l_role_union);
    return PyUnicode_FromString(l_str ? l_str : "none");
}

PyObject *dap_chain_set_offset_limit_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_json_obj = NULL;
    unsigned long long l_start_in = 0;
    unsigned long long l_end_in = 0;
    unsigned long long l_limit = 0;
    unsigned long long l_offset = 0;
    unsigned long long l_end_count = 0;
    int l_last = 0;

    if (!PyArg_ParseTuple(a_args, "OKKKKKp", &l_json_obj, &l_start_in, &l_end_in,
                          &l_limit, &l_offset, &l_end_count, &l_last)) {
        return NULL;
    }

    dap_json_t *l_json = (dap_json_t *)cf_common_capsule_get_pointer(
        l_json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!l_json) {
        return NULL;
    }

    size_t l_start = (size_t)l_start_in;
    size_t l_end = (size_t)l_end_in;

    dap_chain_set_offset_limit_json(l_json, &l_start, &l_end, (size_t)l_limit,
                                    (size_t)l_offset, (size_t)l_end_count, l_last != 0);

    return Py_BuildValue("(KK)", (unsigned long long)l_start, (unsigned long long)l_end);
}

PyObject *dap_chain_srv_str_to_unit_enum_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_str = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_str)) {
        return NULL;
    }
    dap_chain_srv_unit_enum_t l_unit = dap_chain_srv_str_to_unit_enum(l_str);
    return PyLong_FromUnsignedLong((unsigned long)l_unit);
}

PyObject *dap_chain_srv_uid_from_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_str = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_str)) {
        return NULL;
    }
    dap_chain_srv_uid_t l_uid = dap_chain_srv_uid_from_str(l_str);
    return PyLong_FromUnsignedLongLong(l_uid.uint64);
}

PyObject *dap_chain_srv_unit_enum_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_unit = 0;
    if (!PyArg_ParseTuple(a_args, "I", &l_unit)) {
        return NULL;
    }
    const char *l_str = dap_chain_srv_unit_enum_to_str((dap_chain_srv_unit_enum_t)l_unit);
    return PyUnicode_FromString(l_str ? l_str : "UNDEFINED");
}

PyObject *dap_chain_uint128_from_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_val = 0;
    if (!PyArg_ParseTuple(a_args, "K", &l_val)) {
        return NULL;
    }
    uint128_t l_out = dap_chain_uint128_from((uint64_t)l_val);
    return PyBytes_FromStringAndSize((const char *)&l_out, sizeof(uint128_t));
}

PyObject *dap_chain_uint256_from_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_val = 0;
    if (!PyArg_ParseTuple(a_args, "K", &l_val)) {
        return NULL;
    }
    uint256_t l_out = dap_chain_uint256_from((uint64_t)l_val);
    return PyBytes_FromStringAndSize((const char *)&l_out, sizeof(uint256_t));
}

PyObject *dap_chain_uint256_from_uint128_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_val_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &l_val_obj)) {
        return NULL;
    }
    uint128_t l_val = {0};
    if (cf_common_parse_uint128(l_val_obj, &l_val) != 0) {
        return NULL;
    }
    uint256_t l_out = dap_chain_uint256_from_uint128(l_val);
    return PyBytes_FromStringAndSize((const char *)&l_out, sizeof(uint256_t));
}

int cf_common_register_chain_common(PyObject *module) {
    static PyMethodDef methods[] = {
        {"dap_chain_addr_from_str", py_dap_chain_addr_from_str, METH_VARARGS,
         "Create chain address from string"},
        {"dap_chain_addr_is_blank", py_dap_chain_addr_is_blank, METH_VARARGS,
         "Check if chain address is blank"},
        {"dap_chain_addr_check_sum", py_dap_chain_addr_check_sum, METH_VARARGS,
         "Check chain address checksum"},
        {"dap_chain_addr_compare", dap_chain_addr_compare_py, METH_VARARGS,
         "Compare two addresses"},
        {"dap_chain_addr_fill", dap_chain_addr_fill_py, METH_VARARGS,
         "Fill address from sign type and pkey hash"},
        {"dap_chain_addr_fill_from_key", dap_chain_addr_fill_from_key_py, METH_VARARGS,
         "Fill address from enc key"},
        {"dap_chain_addr_fill_from_sign", dap_chain_addr_fill_from_sign_py, METH_VARARGS,
         "Fill address from sign"},
        {"dap_chain_addr_from_str_array", dap_chain_addr_from_str_array_py, METH_VARARGS,
         "Parse comma-separated address list"},
        {"dap_chain_addr_to_str_static_", dap_chain_addr_to_str_static_py, METH_VARARGS,
         "Convert address to string (static)"},
        {"dap_chain_balance_scan", dap_chain_balance_scan_py, METH_VARARGS,
         "Parse balance string into uint256"},
        {"dap_chain_balance_to_coins_uint64", dap_chain_balance_to_coins_uint64_py, METH_VARARGS,
         "Convert uint256 balance to uint64 coins"},
        {"dap_chain_hash_slow_kind_check", dap_chain_hash_slow_kind_check_py, METH_VARARGS,
         "Check slow hash kind"},
        {"dap_chain_hash_slow_to_str", dap_chain_hash_slow_to_str_py, METH_VARARGS,
         "Convert slow hash to string"},
        {"dap_chain_hash_slow_to_str_new", dap_chain_hash_slow_to_str_new_py, METH_VARARGS,
         "Convert slow hash to new string"},
        {"dap_chain_mempool_group_name", dap_chain_mempool_group_name_py, METH_VARARGS,
         "Compose mempool group name"},
        {"dap_chain_node_role_to_str", dap_chain_node_role_to_str_py, METH_VARARGS,
         "Convert node role to string"},
        {"dap_chain_set_offset_limit_json", dap_chain_set_offset_limit_json_py, METH_VARARGS,
         "Set offset/limit in JSON"},
        {"dap_chain_srv_str_to_unit_enum", dap_chain_srv_str_to_unit_enum_py, METH_VARARGS,
         "Convert service unit string to enum"},
        {"dap_chain_srv_uid_from_str", dap_chain_srv_uid_from_str_py, METH_VARARGS,
         "Parse service UID from string"},
        {"dap_chain_srv_unit_enum_to_str", dap_chain_srv_unit_enum_to_str_py, METH_VARARGS,
         "Convert service unit enum to string"},
        {"dap_chain_uint128_from", dap_chain_uint128_from_py, METH_VARARGS,
         "Convert uint64 to uint128"},
        {"dap_chain_uint256_from", dap_chain_uint256_from_py, METH_VARARGS,
         "Convert uint64 to uint256"},
        {"dap_chain_uint256_from_uint128", dap_chain_uint256_from_uint128_py, METH_VARARGS,
         "Convert uint128 to uint256"},
        {NULL, NULL, 0, NULL}
    };

    return cf_common_register_methods(module, methods);
}
