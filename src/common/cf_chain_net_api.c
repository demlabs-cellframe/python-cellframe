#include "cf_common_utils.h"
#include "dap_chain_net_api.h"

#define LOG_TAG "python_cellframe_common"

PyObject *dap_chain_net_api_add_reward_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_reward_obj = NULL;
    unsigned long long l_block_num = 0;

    if (!PyArg_ParseTuple(a_args, "OOK", &l_net_obj, &l_reward_obj, &l_block_num)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    uint256_t l_reward = {0};
    if (cf_common_parse_uint256(l_reward_obj, &l_reward) != 0) {
        return NULL;
    }

    int l_rc = dap_chain_net_api_add_reward(l_net, l_reward, l_block_num);
    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_net_api_by_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_net_t *l_net = dap_chain_net_api_by_id(l_net_id_struct);
    if (!l_net) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_net, "dap_chain_net_t", NULL);
}

PyObject *dap_chain_net_api_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_name)) {
        return NULL;
    }
    dap_chain_net_t *l_net = dap_chain_net_api_by_name(l_name);
    if (!l_net) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_net, "dap_chain_net_t", NULL);
}

PyObject *dap_chain_net_api_datum_add_to_mempool_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_datum_obj = NULL;
    PyObject *l_chain_obj = NULL;
    const char *l_hash_type = NULL;

    if (!PyArg_ParseTuple(a_args, "OO|s", &l_datum_obj, &l_chain_obj, &l_hash_type)) {
        return NULL;
    }

    dap_chain_datum_t *l_datum = (dap_chain_datum_t *)cf_common_capsule_get_pointer(
        l_datum_obj, "dap_chain_datum_t", "First argument must be a datum capsule"
    );
    if (!l_datum) {
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)cf_common_capsule_get_pointer(
        l_chain_obj, "dap_chain_t", "Second argument must be a chain capsule"
    );
    if (!l_chain) {
        return NULL;
    }

    char *l_hash = dap_chain_net_api_datum_add_to_mempool(l_datum, l_chain, l_hash_type);
    if (!l_hash) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(l_hash);
}

PyObject *dap_chain_net_api_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_net_api_deinit();
    Py_RETURN_NONE;
}

PyObject *dap_chain_net_api_get_chain_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    const char *l_name = NULL;

    if (!PyArg_ParseTuple(a_args, "Os", &l_net_obj, &l_name)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    dap_chain_t *l_chain = dap_chain_net_api_get_chain_by_name(l_net, l_name);
    if (!l_chain) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

PyObject *dap_chain_net_api_get_chain_by_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    int l_type = 0;

    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_obj, &l_type)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    dap_chain_t *l_chain = dap_chain_net_api_get_chain_by_type(l_net, (dap_chain_type_t)l_type);
    if (!l_chain) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

PyObject *dap_chain_net_api_get_cur_cell_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    dap_chain_cell_id_t *l_cell_id = dap_chain_net_api_get_cur_cell(l_net);
    if (!l_cell_id) {
        Py_RETURN_NONE;
    }

    PyObject *l_raw = PyBytes_FromStringAndSize((const char *)l_cell_id->raw, DAP_CHAIN_SHARD_ID_SIZE);
    if (!l_raw) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = Py_BuildValue("{s:K,s:O}", "uint64", l_cell_id->uint64, "raw", l_raw);
    Py_DECREF(l_raw);
    return l_result;
}

PyObject *dap_chain_net_api_get_default_chain_by_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    int l_type = 0;

    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_obj, &l_type)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    dap_chain_t *l_chain = dap_chain_net_api_get_default_chain_by_type(l_net, (dap_chain_type_t)l_type);
    if (!l_chain) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

PyObject *dap_chain_net_api_get_load_mode_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    bool l_mode = dap_chain_net_api_get_load_mode(l_net);
    return PyBool_FromLong(l_mode ? 1 : 0);
}

PyObject *dap_chain_net_api_get_reward_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    unsigned long long l_block_num = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_net_obj, &l_block_num)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    uint256_t l_reward = dap_chain_net_api_get_reward(l_net, l_block_num);
    return PyBytes_FromStringAndSize((const char *)&l_reward, sizeof(uint256_t));
}

PyObject *dap_chain_net_api_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_net_api_init();
    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_net_api_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_registry_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_registry_obj)) {
        return NULL;
    }

    dap_chain_net_api_registry_t *l_registry = (dap_chain_net_api_registry_t *)cf_common_capsule_get_pointer(
        l_registry_obj, "dap_chain_net_api_registry_t", "Argument must be a registry capsule"
    );
    if (!l_registry) {
        return NULL;
    }

    int l_rc = dap_chain_net_api_register(l_registry);
    return PyLong_FromLong(l_rc);
}

int cf_common_register_chain_net_api(PyObject *module) {
    static PyMethodDef methods[] = {
        {"dap_chain_net_api_add_reward", dap_chain_net_api_add_reward_py, METH_VARARGS,
         "Add reward to network"},
        {"dap_chain_net_api_by_id", dap_chain_net_api_by_id_py, METH_VARARGS,
         "Get network by ID"},
        {"dap_chain_net_api_by_name", dap_chain_net_api_by_name_py, METH_VARARGS,
         "Get network by name"},
        {"dap_chain_net_api_datum_add_to_mempool", dap_chain_net_api_datum_add_to_mempool_py, METH_VARARGS,
         "Add datum to mempool"},
        {"dap_chain_net_api_deinit", dap_chain_net_api_deinit_py, METH_NOARGS,
         "Deinitialize network API registry"},
        {"dap_chain_net_api_get_chain_by_name", dap_chain_net_api_get_chain_by_name_py, METH_VARARGS,
         "Get chain by name from network"},
        {"dap_chain_net_api_get_chain_by_type", dap_chain_net_api_get_chain_by_type_py, METH_VARARGS,
         "Get chain by type from network"},
        {"dap_chain_net_api_get_cur_cell", dap_chain_net_api_get_cur_cell_py, METH_VARARGS,
         "Get current cell ID from network"},
        {"dap_chain_net_api_get_default_chain_by_type", dap_chain_net_api_get_default_chain_by_type_py, METH_VARARGS,
         "Get default chain by type"},
        {"dap_chain_net_api_get_load_mode", dap_chain_net_api_get_load_mode_py, METH_VARARGS,
         "Get network load mode"},
        {"dap_chain_net_api_get_reward", dap_chain_net_api_get_reward_py, METH_VARARGS,
         "Get reward for block"},
        {"dap_chain_net_api_init", dap_chain_net_api_init_py, METH_NOARGS,
         "Initialize network API registry"},
        {"dap_chain_net_api_register", dap_chain_net_api_register_py, METH_VARARGS,
         "Register network API functions"},
        {NULL, NULL, 0, NULL}
    };

    return cf_common_register_methods(module, methods);
}
