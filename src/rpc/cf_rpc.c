#include "cellframe.h"
#include "dap_chain_node_rpc.h"
#include "dap_json.h"
#include "../datum/cf_datum_internal.h"

#define LOG_TAG "python_cellframe_rpc"

static PyObject *cf_rpc_json_to_py(dap_json_t *json) {
    if (!json) {
        Py_RETURN_NONE;
    }
    char *json_str = dap_json_to_string(json);
    if (!json_str) {
        Py_RETURN_NONE;
    }
    PyObject *py_str = PyUnicode_FromString(json_str);
    DAP_DELETE(json_str);
    if (!py_str) {
        Py_RETURN_NONE;
    }
    return py_str;
}

PyObject* dap_chain_node_rpc_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_cfg_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_cfg_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_cfg_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a config capsule");
        return NULL;
    }

    dap_config_t *l_cfg = (dap_config_t *)PyCapsule_GetPointer(l_cfg_obj, "dap_config_t");
    if (!l_cfg) {
        PyErr_SetString(PyExc_ValueError, "Invalid config capsule");
        return NULL;
    }

    dap_chain_node_rpc_init(l_cfg);
    Py_RETURN_NONE;
}

PyObject* dap_chain_node_rpc_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_node_rpc_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_node_rpc_info_save_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_info_obj = NULL;
    int l_force = 0;

    if (!PyArg_ParseTuple(a_args, "O|p", &l_info_obj, &l_force)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_info_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a node info capsule");
        return NULL;
    }

    dap_chain_node_info_t *l_info = (dap_chain_node_info_t *)PyCapsule_GetPointer(l_info_obj, "dap_chain_node_info_t");
    if (!l_info) {
        PyErr_SetString(PyExc_ValueError, "Invalid node info capsule");
        return NULL;
    }

    int l_rc = dap_chain_node_rpc_info_save(l_info, (bool)l_force);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_node_rpc_info_del_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_addr_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_addr_obj)) {
        return NULL;
    }

    dap_chain_node_addr_t l_addr = {};
    if (cf_datum_parse_node_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    int l_rc = dap_chain_node_rpc_info_del(l_addr);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_node_rpc_list_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_json_t *l_json = dap_chain_node_rpc_list();
    if (!l_json) {
        Py_RETURN_NONE;
    }

    PyObject *l_str = cf_rpc_json_to_py(l_json);
    dap_json_object_free(l_json);
    return l_str;
}

PyObject* dap_chain_node_rpc_get_states_sort_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    size_t l_count = 0;
    dap_chain_node_rpc_states_info_t *l_infos = dap_chain_node_rpc_get_states_sort(&l_count);

    PyObject *l_result = PyList_New(0);
    if (!l_result) {
        DAP_DELETE(l_infos);
        return NULL;
    }

    if (!l_infos || !l_count) {
        DAP_DELETE(l_infos);
        return l_result;
    }

    for (size_t i = 0; i < l_count; i++) {
        dap_chain_node_rpc_states_info_t *l_info = &l_infos[i];
        PyObject *l_dict = PyDict_New();
        PyObject *l_sysinfo = PyDict_New();
        if (!l_dict || !l_sysinfo) {
            Py_XDECREF(l_dict);
            Py_XDECREF(l_sysinfo);
            Py_DECREF(l_result);
            DAP_DELETE(l_infos);
            return NULL;
        }

        PyDict_SetItemString(l_dict, "version", PyLong_FromUnsignedLong(l_info->version));
        PyDict_SetItemString(l_dict, "address", PyLong_FromUnsignedLongLong(l_info->address.uint64));
        PyDict_SetItemString(l_dict, "location", PyLong_FromUnsignedLong(l_info->location));
        PyDict_SetItemString(l_dict, "links_count", PyLong_FromUnsignedLong(l_info->links_count));
        PyDict_SetItemString(l_dict, "cli_thread_count", PyLong_FromUnsignedLong(l_info->cli_thread_count));

        PyObject *l_loads = PyList_New(3);
        if (l_loads) {
            PyList_SET_ITEM(l_loads, 0, PyLong_FromUnsignedLong(l_info->system_info.loads[0]));
            PyList_SET_ITEM(l_loads, 1, PyLong_FromUnsignedLong(l_info->system_info.loads[1]));
            PyList_SET_ITEM(l_loads, 2, PyLong_FromUnsignedLong(l_info->system_info.loads[2]));
            PyDict_SetItemString(l_sysinfo, "loads", l_loads);
            Py_DECREF(l_loads);
        }

        PyDict_SetItemString(l_sysinfo, "uptime", PyLong_FromLong(l_info->system_info.uptime));
        PyDict_SetItemString(l_sysinfo, "totalram", PyLong_FromUnsignedLong(l_info->system_info.totalram));
        PyDict_SetItemString(l_sysinfo, "freeram", PyLong_FromUnsignedLong(l_info->system_info.freeram));
        PyDict_SetItemString(l_sysinfo, "sharedram", PyLong_FromUnsignedLong(l_info->system_info.sharedram));
        PyDict_SetItemString(l_sysinfo, "bufferram", PyLong_FromUnsignedLong(l_info->system_info.bufferram));
        PyDict_SetItemString(l_sysinfo, "totalswap", PyLong_FromUnsignedLong(l_info->system_info.totalswap));
        PyDict_SetItemString(l_sysinfo, "freeswap", PyLong_FromUnsignedLong(l_info->system_info.freeswap));
        PyDict_SetItemString(l_sysinfo, "procs", PyLong_FromUnsignedLong(l_info->system_info.procs));
        PyDict_SetItemString(l_sysinfo, "totalhigh", PyLong_FromUnsignedLong(l_info->system_info.totalhigh));
        PyDict_SetItemString(l_sysinfo, "freehigh", PyLong_FromUnsignedLong(l_info->system_info.freehigh));
        PyDict_SetItemString(l_sysinfo, "mem_unit", PyLong_FromUnsignedLong(l_info->system_info.mem_unit));

        PyDict_SetItemString(l_dict, "system_info", l_sysinfo);
        Py_DECREF(l_sysinfo);

        PyList_Append(l_result, l_dict);
        Py_DECREF(l_dict);
    }

    DAP_DELETE(l_infos);
    return l_result;
}

PyObject* dap_chain_node_rpc_get_states_info_size_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_info_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_info_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_info_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a states info capsule");
        return NULL;
    }

    dap_chain_node_rpc_states_info_t *l_info = (dap_chain_node_rpc_states_info_t *)PyCapsule_GetPointer(
        l_info_obj, "dap_chain_node_rpc_states_info_t");
    if (!l_info) {
        PyErr_SetString(PyExc_ValueError, "Invalid states info capsule");
        return NULL;
    }

    size_t l_size = dap_chain_node_rpc_get_states_info_size(l_info);
    return PyLong_FromSize_t(l_size);
}

PyObject* dap_chain_node_rpc_states_info_read_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_addr_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "|O", &l_addr_obj)) {
        return NULL;
    }

    dap_chain_node_addr_t l_addr = {};
    if (l_addr_obj && l_addr_obj != Py_None) {
        if (cf_datum_parse_node_addr(l_addr_obj, &l_addr) != 0) {
            return NULL;
        }
    }

    dap_json_t *l_json = dap_chain_node_rpc_states_info_read(l_addr);
    if (!l_json) {
        Py_RETURN_NONE;
    }

    PyObject *l_str = cf_rpc_json_to_py(l_json);
    dap_json_object_free(l_json);
    return l_str;
}

PyObject* dap_chain_node_rpc_is_my_node_authorized_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    return PyBool_FromLong(dap_chain_node_rpc_is_my_node_authorized());
}

PyObject* dap_chain_node_rpc_is_balancer_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    return PyBool_FromLong(dap_chain_node_rpc_is_balancer());
}

PyObject* dap_chain_node_rpc_is_root_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    return PyBool_FromLong(dap_chain_node_rpc_is_root());
}

PyMethodDef* cellframe_rpc_get_methods(void) {
    static PyMethodDef methods[] = {
        {"node_rpc_init", (PyCFunction)dap_chain_node_rpc_init_py, METH_VARARGS, "Initialize node RPC"},
        {"node_rpc_deinit", (PyCFunction)dap_chain_node_rpc_deinit_py, METH_VARARGS, "Deinitialize node RPC"},
        {"node_rpc_info_save", (PyCFunction)dap_chain_node_rpc_info_save_py, METH_VARARGS,
         "Save node RPC info"},
        {"node_rpc_info_del", (PyCFunction)dap_chain_node_rpc_info_del_py, METH_VARARGS,
         "Delete node RPC info"},
        {"node_rpc_list", (PyCFunction)dap_chain_node_rpc_list_py, METH_VARARGS, "List RPC nodes"},
        {"node_rpc_get_states_sort", (PyCFunction)dap_chain_node_rpc_get_states_sort_py, METH_VARARGS,
         "Get RPC states list"},
        {"node_rpc_get_states_info_size", (PyCFunction)dap_chain_node_rpc_get_states_info_size_py, METH_VARARGS,
         "Get RPC states info size"},
        {"node_rpc_states_info_read", (PyCFunction)dap_chain_node_rpc_states_info_read_py, METH_VARARGS,
         "Read RPC states info"},
        {"node_rpc_is_my_node_authorized", (PyCFunction)dap_chain_node_rpc_is_my_node_authorized_py, METH_VARARGS,
         "Check RPC authorization"},
        {"node_rpc_is_balancer", (PyCFunction)dap_chain_node_rpc_is_balancer_py, METH_VARARGS,
         "Check RPC balancer role"},
        {"node_rpc_is_root", (PyCFunction)dap_chain_node_rpc_is_root_py, METH_VARARGS,
         "Check RPC root role"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}

int cellframe_rpc_init(PyObject *module) {
    PyMethodDef *methods = cellframe_rpc_get_methods();
    if (!methods) {
        log_it(L_ERROR, "Failed to get RPC methods");
        return -1;
    }

    for (PyMethodDef *method = methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
        if (!func) {
            log_it(L_ERROR, "Failed to create function for %s", method->ml_name);
            return -1;
        }
        if (PyModule_AddObject(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            log_it(L_ERROR, "Failed to add function %s to module", method->ml_name);
            return -1;
        }
    }

    log_it(L_DEBUG, "RPC module initialized successfully");
    return 0;
}
