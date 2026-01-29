#include "cellframe.h"
#include "dap_chain_node.h"
#include "dap_chain_node_sync_client.h"
#include "dap_chain_datum.h"
#include "dap_json.h"
#include "dap_string.h"
#include "../../datum/cf_datum_internal.h"

#define LOG_TAG "python_cellframe_node"

static void cf_node_list_free(void *data) {
    DAP_DELETE(data);
}

static int cf_node_build_argv(PyObject *argv_list, char ***argv_out, int *argc_out) {
    if (!PyList_Check(argv_list) && !PyTuple_Check(argv_list)) {
        PyErr_SetString(PyExc_TypeError, "argv must be a list or tuple");
        return -1;
    }

    Py_ssize_t argc = PySequence_Size(argv_list);
    if (argc < 0) {
        return -1;
    }

    char **argv = DAP_NEW_Z_COUNT(char *, (size_t)argc + 1);
    if (!argv) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate argv");
        return -1;
    }

    for (Py_ssize_t i = 0; i < argc; i++) {
        PyObject *item = PySequence_GetItem(argv_list, i);
        if (!item) {
            DAP_DELETE(argv);
            return -1;
        }

        if (PyUnicode_Check(item)) {
            argv[i] = (char *)PyUnicode_AsUTF8(item);
        } else if (PyBytes_Check(item)) {
            argv[i] = PyBytes_AsString(item);
        } else {
            Py_DECREF(item);
            PyErr_SetString(PyExc_TypeError, "argv items must be str or bytes");
            DAP_DELETE(argv);
            return -1;
        }

        Py_DECREF(item);
        if (!argv[i]) {
            PyErr_SetString(PyExc_ValueError, "Failed to parse argv item");
            DAP_DELETE(argv);
            return -1;
        }
    }

    argv[argc] = NULL;
    *argv_out = argv;
    *argc_out = (int)argc;
    return 0;
}

static void cf_node_free_argv(char **argv) {
    DAP_DELETE(argv);
}

static int cf_node_parse_json_reply(PyObject *json_reply_obj, dap_json_t **json_out, bool *created) {
    *json_out = NULL;
    *created = false;

    if (json_reply_obj && json_reply_obj != Py_None) {
        if (!PyCapsule_CheckExact(json_reply_obj)) {
            PyErr_SetString(PyExc_TypeError, "json_reply must be a JSON capsule or None");
            return -1;
        }
        *json_out = (dap_json_t *)PyCapsule_GetPointer(json_reply_obj, "dap_json_t");
        if (!*json_out) {
            PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
            return -1;
        }
        return 0;
    }

    *json_out = dap_json_array_new();
    if (!*json_out) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate JSON reply");
        return -1;
    }
    *created = true;
    return 0;
}

static PyObject *cf_node_json_to_py(dap_json_t *json) {
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

static void cf_node_info_capsule_destructor(PyObject *capsule) {
    dap_chain_node_info_t *info = (dap_chain_node_info_t *)PyCapsule_GetPointer(capsule, "dap_chain_node_info_t");
    if (info) {
        DAP_DELETE(info);
    }
}

static bool cf_node_sync_simple_matcher(dap_chain_node_sync_request_t *a_request, uint8_t a_pkt_type,
                                        const void *a_pkt_data, size_t a_pkt_size, void *a_arg) {
    (void)a_request;
    (void)a_pkt_data;
    (void)a_pkt_size;
    uint8_t expected = (uint8_t)(uintptr_t)a_arg;
    return a_pkt_type == expected;
}

// =============================================================================
// NODE BASE OPERATIONS
// =============================================================================

PyObject* dap_chain_node_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_node_init();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_node_list_clean_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_node_list_clean_init();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_node_alias_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    const char *l_alias = NULL;
    PyObject *l_addr_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OsO", &l_net_obj, &l_alias, &l_addr_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_node_addr_t l_addr = {};
    if (cf_datum_parse_node_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    bool l_rc = dap_chain_node_alias_register(l_net, l_alias, &l_addr);
    return PyBool_FromLong(l_rc);
}

PyObject* dap_chain_node_alias_find_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    const char *l_alias = NULL;

    if (!PyArg_ParseTuple(a_args, "Os", &l_net_obj, &l_alias)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_node_addr_t *l_addr = dap_chain_node_alias_find(l_net, l_alias);
    if (!l_addr) {
        Py_RETURN_NONE;
    }

    return PyLong_FromUnsignedLongLong(l_addr->uint64);
}

PyObject* dap_chain_node_alias_delete_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    const char *l_alias = NULL;

    if (!PyArg_ParseTuple(a_args, "Os", &l_net_obj, &l_alias)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    bool l_rc = dap_chain_node_alias_delete(l_net, l_alias);
    return PyBool_FromLong(l_rc);
}

PyObject* dap_chain_node_info_save_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_info_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_net_obj, &l_info_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_info_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a node info capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    dap_chain_node_info_t *l_info = (dap_chain_node_info_t *)PyCapsule_GetPointer(l_info_obj, "dap_chain_node_info_t");
    if (!l_net || !l_info) {
        PyErr_SetString(PyExc_ValueError, "Invalid capsule arguments");
        return NULL;
    }

    int l_rc = dap_chain_node_info_save(l_net, l_info);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_node_info_del_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_info_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_net_obj, &l_info_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_info_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a node info capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    dap_chain_node_info_t *l_info = (dap_chain_node_info_t *)PyCapsule_GetPointer(l_info_obj, "dap_chain_node_info_t");
    if (!l_net || !l_info) {
        PyErr_SetString(PyExc_ValueError, "Invalid capsule arguments");
        return NULL;
    }

    int l_rc = dap_chain_node_info_del(l_net, l_info);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_node_info_read_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_addr_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_net_obj, &l_addr_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_node_addr_t l_addr = {};
    if (cf_datum_parse_node_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    dap_chain_node_info_t *l_info = dap_chain_node_info_read(l_net, &l_addr);
    if (!l_info) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_info, "dap_chain_node_info_t", cf_node_info_capsule_destructor);
}

PyObject* dap_chain_node_info_get_size_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_info_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_info_obj)) {
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

    size_t l_size = dap_chain_node_info_get_size(l_info);
    return PyLong_FromSize_t(l_size);
}

PyObject* dap_chain_node_states_info_read_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = Py_None;
    PyObject *l_addr_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "|OO", &l_net_obj, &l_addr_obj)) {
        return NULL;
    }

    dap_chain_net_t *l_net = NULL;
    if (l_net_obj && l_net_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_net_obj)) {
            PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule or None");
            return NULL;
        }
        l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
        if (!l_net) {
            PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
            return NULL;
        }
    }

    dap_chain_node_addr_t l_addr = {};
    if (l_addr_obj && l_addr_obj != Py_None) {
        if (cf_datum_parse_node_addr(l_addr_obj, &l_addr) != 0) {
            return NULL;
        }
    }

    dap_string_t *l_info = dap_chain_node_states_info_read(l_net, l_addr);
    if (!l_info) {
        Py_RETURN_NONE;
    }

    char *l_str = dap_string_free(l_info, false);
    if (!l_str) {
        Py_RETURN_NONE;
    }

    PyObject *l_py_str = PyUnicode_FromString(l_str);
    DAP_DELETE(l_str);
    if (!l_py_str) {
        Py_RETURN_NONE;
    }
    return l_py_str;
}

PyObject* dap_chain_node_get_states_list_sort_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_ignored_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|O", &l_net_obj, &l_ignored_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_chain_node_addr_t *l_ignored = NULL;
    size_t l_ignored_count = 0;
    PyObject *l_ignored_seq = NULL;
    if (l_ignored_obj && l_ignored_obj != Py_None) {
        l_ignored_seq = PySequence_Fast(l_ignored_obj, "ignored list must be a sequence");
        if (!l_ignored_seq) {
            return NULL;
        }
        l_ignored_count = (size_t)PySequence_Fast_GET_SIZE(l_ignored_seq);
        if (l_ignored_count) {
            l_ignored = DAP_NEW_Z_COUNT(dap_chain_node_addr_t, l_ignored_count);
            if (!l_ignored) {
                Py_DECREF(l_ignored_seq);
                PyErr_SetString(PyExc_MemoryError, "Failed to allocate ignored nodes");
                return NULL;
            }
            for (size_t i = 0; i < l_ignored_count; i++) {
                PyObject *addr_obj = PySequence_Fast_GET_ITEM(l_ignored_seq, (Py_ssize_t)i);
                if (cf_datum_parse_node_addr(addr_obj, &l_ignored[i]) != 0) {
                    DAP_DELETE(l_ignored);
                    Py_DECREF(l_ignored_seq);
                    return NULL;
                }
            }
        }
    }

    dap_list_t *l_list = dap_chain_node_get_states_list_sort(l_net, l_ignored, l_ignored_count);
    DAP_DELETE(l_ignored);
    Py_XDECREF(l_ignored_seq);

    PyObject *l_result = PyList_New(0);
    if (!l_result) {
        dap_list_free_full(l_list, cf_node_list_free);
        return NULL;
    }

    for (dap_list_t *l_item = l_list; l_item; l_item = l_item->next) {
        dap_chain_node_states_info_t *l_info = (dap_chain_node_states_info_t *)l_item->data;
        if (!l_info) {
            continue;
        }

        PyObject *l_dict = PyDict_New();
        if (!l_dict) {
            Py_DECREF(l_result);
            dap_list_free_full(l_list, cf_node_list_free);
            return NULL;
        }

        PyDict_SetItemString(l_dict, "node_addr", PyLong_FromUnsignedLongLong(l_info->link_info.node_addr.uint64));
        PyDict_SetItemString(l_dict, "uplink_addr", PyUnicode_FromString(l_info->link_info.uplink_addr));
        PyDict_SetItemString(l_dict, "uplink_port", PyLong_FromUnsignedLong(l_info->link_info.uplink_port));
        PyDict_SetItemString(l_dict, "role", PyLong_FromLong(l_info->role.enums));
        PyDict_SetItemString(l_dict, "events_count", PyLong_FromUnsignedLongLong(l_info->events_count));
        PyDict_SetItemString(l_dict, "atoms_count", PyLong_FromUnsignedLongLong(l_info->atoms_count));
        PyDict_SetItemString(l_dict, "downlinks_count", PyLong_FromUnsignedLong(l_info->downlinks_count));
        PyDict_SetItemString(l_dict, "timestamp", PyLong_FromUnsignedLongLong(l_info->timestamp));

        PyList_Append(l_result, l_dict);
        Py_DECREF(l_dict);
    }

    dap_list_free_full(l_list, cf_node_list_free);
    return l_result;
}

PyObject* dap_chain_node_cli_cmd_values_parse_net_chain_for_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_argv_obj = NULL;
    int l_arg_index = 0;
    int l_default_chain_type = CHAIN_TYPE_INVALID;
    PyObject *l_json_reply_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "Oi|iO", &l_argv_obj, &l_arg_index, &l_default_chain_type, &l_json_reply_obj)) {
        return NULL;
    }

    char **l_argv = NULL;
    int l_argc = 0;
    if (cf_node_build_argv(l_argv_obj, &l_argv, &l_argc) != 0) {
        return NULL;
    }

    dap_json_t *l_json_reply = NULL;
    bool l_json_created = false;
    if (cf_node_parse_json_reply(l_json_reply_obj, &l_json_reply, &l_json_created) != 0) {
        cf_node_free_argv(l_argv);
        return NULL;
    }

    dap_chain_t *l_chain = NULL;
    dap_chain_net_t *l_net = NULL;
    int l_rc = dap_chain_node_cli_cmd_values_parse_net_chain_for_json(
        l_json_reply, &l_arg_index, l_argc, l_argv, &l_chain, &l_net, (dap_chain_type_t)l_default_chain_type);

    PyObject *l_json_str = cf_node_json_to_py(l_json_reply);
    if (l_json_created && l_json_reply) {
        dap_json_object_free(l_json_reply);
    }
    cf_node_free_argv(l_argv);

    PyObject *l_tuple = PyTuple_New(5);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    PyTuple_SetItem(l_tuple, 1, PyLong_FromLong(l_arg_index));
    if (l_chain) {
        PyTuple_SetItem(l_tuple, 2, PyCapsule_New(l_chain, "dap_chain_t", NULL));
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 2, Py_None);
    }
    if (l_net) {
        PyTuple_SetItem(l_tuple, 3, PyCapsule_New(l_net, "dap_chain_net_t", NULL));
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 3, Py_None);
    }
    if (l_json_str) {
        PyTuple_SetItem(l_tuple, 4, l_json_str);
    } else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 4, Py_None);
    }
    return l_tuple;
}

// =============================================================================
// NODE MEMPOOL / HARDFORK
// =============================================================================

PyObject* dap_chain_node_mempool_need_process_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    const char *l_datum_bytes = NULL;
    Py_ssize_t l_datum_size = 0;

    if (!PyArg_ParseTuple(a_args, "Oy#", &l_chain_obj, &l_datum_bytes, &l_datum_size)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    if (l_datum_size < (Py_ssize_t)sizeof(dap_chain_datum_t)) {
        PyErr_SetString(PyExc_ValueError, "Datum size too small");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    dap_chain_datum_t *l_datum = (dap_chain_datum_t *)l_datum_bytes;
    bool l_rc = dap_chain_node_mempool_need_process(l_chain, l_datum);
    return PyBool_FromLong(l_rc);
}

PyObject* dap_chain_node_mempool_process_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    const char *l_datum_bytes = NULL;
    Py_ssize_t l_datum_size = 0;
    const char *l_datum_hash_str = NULL;

    if (!PyArg_ParseTuple(a_args, "Oy#s", &l_chain_obj, &l_datum_bytes, &l_datum_size, &l_datum_hash_str)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    if (l_datum_size < (Py_ssize_t)sizeof(dap_chain_datum_t)) {
        PyErr_SetString(PyExc_ValueError, "Datum size too small");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    dap_chain_datum_t *l_datum = (dap_chain_datum_t *)l_datum_bytes;
    int l_ret = 0;
    bool l_delete = dap_chain_node_mempool_process(l_chain, l_datum, l_datum_hash_str, &l_ret);

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyBool_FromLong(l_delete));
    PyTuple_SetItem(l_tuple, 1, PyLong_FromLong(l_ret));
    return l_tuple;
}

PyObject* dap_chain_node_mempool_process_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    int l_force = 0;

    if (!PyArg_ParseTuple(a_args, "Op", &l_chain_obj, &l_force)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    dap_chain_node_mempool_process_all(l_chain, (bool)l_force);
    Py_RETURN_NONE;
}

PyObject* dap_chain_node_mempool_autoproc_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    bool l_rc = dap_chain_node_mempool_autoproc_init();
    return PyBool_FromLong(l_rc);
}

PyObject* dap_chain_node_mempool_autoproc_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_node_mempool_autoproc_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_node_hardfork_process_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    int l_rc = dap_chain_node_hardfork_process(l_chain);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_node_hardfork_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    const char *l_datum_bytes = NULL;
    Py_ssize_t l_datum_size = 0;

    if (!PyArg_ParseTuple(a_args, "Oy#", &l_chain_obj, &l_datum_bytes, &l_datum_size)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    if (l_datum_size < (Py_ssize_t)sizeof(dap_chain_datum_t)) {
        PyErr_SetString(PyExc_ValueError, "Datum size too small");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    dap_chain_datum_t *l_datum = (dap_chain_datum_t *)l_datum_bytes;
    int l_rc = dap_chain_node_hardfork_check(l_chain, l_datum);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_node_hardfork_confirm_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    const char *l_datum_bytes = NULL;
    Py_ssize_t l_datum_size = 0;

    if (!PyArg_ParseTuple(a_args, "Oy#", &l_chain_obj, &l_datum_bytes, &l_datum_size)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    if (l_datum_size < (Py_ssize_t)sizeof(dap_chain_datum_t)) {
        PyErr_SetString(PyExc_ValueError, "Datum size too small");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    dap_chain_datum_t *l_datum = (dap_chain_datum_t *)l_datum_bytes;
    int l_rc = dap_chain_node_hardfork_confirm(l_chain, l_datum);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_node_hardfork_data_cleanup_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_chain_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    dap_chain_node_hardfork_data_cleanup(l_chain);
    Py_RETURN_NONE;
}

// =============================================================================
// NODE SYNC CLIENT
// =============================================================================

PyObject* dap_chain_node_sync_client_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_node_sync_client_init();
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_node_sync_client_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_node_sync_client_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_node_sync_client_connect_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_node_info_obj = NULL;
    const char *l_channels = NULL;
    int l_timeout_ms = 0;

    if (!PyArg_ParseTuple(a_args, "OOsi", &l_net_obj, &l_node_info_obj, &l_channels, &l_timeout_ms)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj) || !PyCapsule_CheckExact(l_node_info_obj)) {
        PyErr_SetString(PyExc_TypeError, "First two arguments must be network and node info capsules");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    dap_chain_node_info_t *l_node_info = (dap_chain_node_info_t *)PyCapsule_GetPointer(l_node_info_obj, "dap_chain_node_info_t");
    if (!l_net || !l_node_info) {
        PyErr_SetString(PyExc_ValueError, "Invalid capsules");
        return NULL;
    }

    dap_chain_node_sync_client_t *l_client = dap_chain_node_sync_client_connect(l_net, l_node_info, l_channels, l_timeout_ms);
    if (!l_client) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_client, "dap_chain_node_sync_client_t", NULL);
}

PyObject* dap_chain_node_sync_handshake_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_node_info_obj = NULL;
    const char *l_channels = NULL;
    int l_timeout_ms = 0;

    if (!PyArg_ParseTuple(a_args, "OOsi", &l_net_obj, &l_node_info_obj, &l_channels, &l_timeout_ms)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj) || !PyCapsule_CheckExact(l_node_info_obj)) {
        PyErr_SetString(PyExc_TypeError, "First two arguments must be network and node info capsules");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    dap_chain_node_info_t *l_node_info = (dap_chain_node_info_t *)PyCapsule_GetPointer(l_node_info_obj, "dap_chain_node_info_t");
    if (!l_net || !l_node_info) {
        PyErr_SetString(PyExc_ValueError, "Invalid capsules");
        return NULL;
    }

    int l_rc = dap_chain_node_sync_handshake(l_net, l_node_info, l_channels, l_timeout_ms);
    return PyLong_FromLong(l_rc);
}

PyObject* dap_chain_node_sync_request_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_client_obj = NULL;
    int l_channel_id = 0;
    int l_request_type = 0;
    PyObject *l_data_obj = Py_None;
    int l_expected_response = 0;
    int l_timeout_ms = 0;

    if (!PyArg_ParseTuple(a_args, "OiiOii", &l_client_obj, &l_channel_id, &l_request_type, &l_data_obj, &l_expected_response, &l_timeout_ms)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_client_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a sync client capsule");
        return NULL;
    }

    if (l_channel_id < 0 || l_channel_id > 255 || l_request_type < 0 || l_request_type > 255 ||
        l_expected_response < 0 || l_expected_response > 255) {
        PyErr_SetString(PyExc_ValueError, "packet types must be between 0 and 255");
        return NULL;
    }

    dap_chain_node_sync_client_t *l_client = (dap_chain_node_sync_client_t *)PyCapsule_GetPointer(
        l_client_obj, "dap_chain_node_sync_client_t");
    if (!l_client) {
        PyErr_SetString(PyExc_ValueError, "Invalid sync client capsule");
        return NULL;
    }

    const void *l_data = NULL;
    size_t l_data_size = 0;
    if (l_data_obj && l_data_obj != Py_None) {
        if (!PyBytes_Check(l_data_obj)) {
            PyErr_SetString(PyExc_TypeError, "request_data must be bytes or None");
            return NULL;
        }
        Py_ssize_t l_size = 0;
        if (PyBytes_AsStringAndSize(l_data_obj, (char **)&l_data, &l_size) < 0) {
            return NULL;
        }
        l_data_size = (size_t)l_size;
    }

    void *l_out_data = NULL;
    size_t l_out_size = 0;
    int l_rc = dap_chain_node_sync_request(
        l_client, (uint8_t)l_channel_id, (uint8_t)l_request_type,
        l_data, l_data_size, (uint8_t)l_expected_response, &l_out_data, &l_out_size, l_timeout_ms);

    PyObject *l_resp = Py_None;
    if (l_out_data && l_out_size) {
        l_resp = PyBytes_FromStringAndSize((const char *)l_out_data, (Py_ssize_t)l_out_size);
        if (!l_resp) {
            DAP_DELETE(l_out_data);
            return NULL;
        }
    } else {
        Py_INCREF(Py_None);
        l_resp = Py_None;
    }
    DAP_DELETE(l_out_data);

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    PyTuple_SetItem(l_tuple, 1, l_resp);
    return l_tuple;
}

PyObject* dap_chain_node_sync_request_ex_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_client_obj = NULL;
    int l_channel_id = 0;
    int l_request_type = 0;
    PyObject *l_data_obj = Py_None;
    int l_expected_response = 0;
    int l_timeout_ms = 0;

    if (!PyArg_ParseTuple(a_args, "OiiOii", &l_client_obj, &l_channel_id, &l_request_type, &l_data_obj, &l_expected_response, &l_timeout_ms)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_client_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a sync client capsule");
        return NULL;
    }

    if (l_channel_id < 0 || l_channel_id > 255 || l_request_type < 0 || l_request_type > 255 ||
        l_expected_response < 0 || l_expected_response > 255) {
        PyErr_SetString(PyExc_ValueError, "packet types must be between 0 and 255");
        return NULL;
    }

    dap_chain_node_sync_client_t *l_client = (dap_chain_node_sync_client_t *)PyCapsule_GetPointer(
        l_client_obj, "dap_chain_node_sync_client_t");
    if (!l_client) {
        PyErr_SetString(PyExc_ValueError, "Invalid sync client capsule");
        return NULL;
    }

    const void *l_data = NULL;
    size_t l_data_size = 0;
    if (l_data_obj && l_data_obj != Py_None) {
        if (!PyBytes_Check(l_data_obj)) {
            PyErr_SetString(PyExc_TypeError, "request_data must be bytes or None");
            return NULL;
        }
        Py_ssize_t l_size = 0;
        if (PyBytes_AsStringAndSize(l_data_obj, (char **)&l_data, &l_size) < 0) {
            return NULL;
        }
        l_data_size = (size_t)l_size;
    }

    void *l_out_data = NULL;
    size_t l_out_size = 0;
    int l_rc = dap_chain_node_sync_request_ex(
        l_client, (uint8_t)l_channel_id, (uint8_t)l_request_type,
        l_data, l_data_size, cf_node_sync_simple_matcher,
        (void *)(uintptr_t)l_expected_response, &l_out_data, &l_out_size, l_timeout_ms);

    PyObject *l_resp = Py_None;
    if (l_out_data && l_out_size) {
        l_resp = PyBytes_FromStringAndSize((const char *)l_out_data, (Py_ssize_t)l_out_size);
        if (!l_resp) {
            DAP_DELETE(l_out_data);
            return NULL;
        }
    } else {
        Py_INCREF(Py_None);
        l_resp = Py_None;
    }
    DAP_DELETE(l_out_data);

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    PyTuple_SetItem(l_tuple, 1, l_resp);
    return l_tuple;
}

PyObject* dap_chain_node_sync_client_close_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_client_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_client_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_client_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a sync client capsule");
        return NULL;
    }

    dap_chain_node_sync_client_t *l_client = (dap_chain_node_sync_client_t *)PyCapsule_GetPointer(
        l_client_obj, "dap_chain_node_sync_client_t");
    if (!l_client) {
        PyErr_SetString(PyExc_ValueError, "Invalid sync client capsule");
        return NULL;
    }

    dap_chain_node_sync_client_close(l_client);
    Py_RETURN_NONE;
}

PyObject* dap_chain_node_sync_client_is_connected_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_client_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_client_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_client_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a sync client capsule");
        return NULL;
    }

    dap_chain_node_sync_client_t *l_client = (dap_chain_node_sync_client_t *)PyCapsule_GetPointer(
        l_client_obj, "dap_chain_node_sync_client_t");
    if (!l_client) {
        PyErr_SetString(PyExc_ValueError, "Invalid sync client capsule");
        return NULL;
    }

    return PyBool_FromLong(dap_chain_node_sync_client_is_connected(l_client));
}

PyObject* dap_chain_node_sync_error_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_err = 0;

    if (!PyArg_ParseTuple(a_args, "i", &l_err)) {
        return NULL;
    }

    const char *l_str = dap_chain_node_sync_error_str((dap_chain_node_sync_error_t)l_err);
    if (!l_str) {
        Py_RETURN_NONE;
    }

    return PyUnicode_FromString(l_str);
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

PyMethodDef* cellframe_node_get_methods(void) {
    static PyMethodDef methods[] = {
        {"node_init", (PyCFunction)dap_chain_node_init_py, METH_VARARGS, "Initialize node module"},
        {"node_list_clean_init", (PyCFunction)dap_chain_node_list_clean_init_py, METH_VARARGS,
         "Initialize node list cleaning"},
        {"node_alias_register", (PyCFunction)dap_chain_node_alias_register_py, METH_VARARGS, "Register node alias"},
        {"node_alias_find", (PyCFunction)dap_chain_node_alias_find_py, METH_VARARGS, "Find node alias"},
        {"node_alias_delete", (PyCFunction)dap_chain_node_alias_delete_py, METH_VARARGS, "Delete node alias"},
        {"node_info_save", (PyCFunction)dap_chain_node_info_save_py, METH_VARARGS, "Save node info"},
        {"node_info_del", (PyCFunction)dap_chain_node_info_del_py, METH_VARARGS, "Delete node info"},
        {"node_info_read", (PyCFunction)dap_chain_node_info_read_py, METH_VARARGS, "Read node info"},
        {"node_info_get_size", (PyCFunction)dap_chain_node_info_get_size_py, METH_VARARGS, "Get node info size"},
        {"node_states_info_read", (PyCFunction)dap_chain_node_states_info_read_py, METH_VARARGS,
         "Read node states info"},
        {"node_get_states_list_sort", (PyCFunction)dap_chain_node_get_states_list_sort_py, METH_VARARGS,
         "Get sorted node states list"},
        {"node_cli_cmd_values_parse_net_chain_for_json",
         (PyCFunction)dap_chain_node_cli_cmd_values_parse_net_chain_for_json_py, METH_VARARGS,
         "Parse net/chain CLI args for JSON reply"},
        {"node_mempool_need_process", (PyCFunction)dap_chain_node_mempool_need_process_py, METH_VARARGS,
         "Check if datum needs mempool processing"},
        {"node_mempool_process", (PyCFunction)dap_chain_node_mempool_process_py, METH_VARARGS,
         "Process mempool datum"},
        {"node_mempool_process_all", (PyCFunction)dap_chain_node_mempool_process_all_py, METH_VARARGS,
         "Process all mempool datums"},
        {"node_mempool_autoproc_init", (PyCFunction)dap_chain_node_mempool_autoproc_init_py, METH_VARARGS,
         "Initialize mempool autoproc"},
        {"node_mempool_autoproc_deinit", (PyCFunction)dap_chain_node_mempool_autoproc_deinit_py, METH_VARARGS,
         "Deinitialize mempool autoproc"},
        {"node_hardfork_process", (PyCFunction)dap_chain_node_hardfork_process_py, METH_VARARGS,
         "Process hardfork"},
        {"node_hardfork_check", (PyCFunction)dap_chain_node_hardfork_check_py, METH_VARARGS,
         "Check hardfork datum"},
        {"node_hardfork_confirm", (PyCFunction)dap_chain_node_hardfork_confirm_py, METH_VARARGS,
         "Confirm hardfork datum"},
        {"node_hardfork_data_cleanup", (PyCFunction)dap_chain_node_hardfork_data_cleanup_py, METH_VARARGS,
         "Cleanup hardfork data"},
        {"node_sync_client_init", (PyCFunction)dap_chain_node_sync_client_init_py, METH_VARARGS,
         "Initialize sync client module"},
        {"node_sync_client_deinit", (PyCFunction)dap_chain_node_sync_client_deinit_py, METH_VARARGS,
         "Deinitialize sync client module"},
        {"node_sync_client_connect", (PyCFunction)dap_chain_node_sync_client_connect_py, METH_VARARGS,
         "Connect sync client"},
        {"node_sync_handshake", (PyCFunction)dap_chain_node_sync_handshake_py, METH_VARARGS,
         "Perform sync handshake"},
        {"node_sync_request", (PyCFunction)dap_chain_node_sync_request_py, METH_VARARGS,
         "Send sync request"},
        {"node_sync_request_ex", (PyCFunction)dap_chain_node_sync_request_ex_py, METH_VARARGS,
         "Send sync request with matcher"},
        {"node_sync_client_close", (PyCFunction)dap_chain_node_sync_client_close_py, METH_VARARGS,
         "Close sync client"},
        {"node_sync_client_is_connected", (PyCFunction)dap_chain_node_sync_client_is_connected_py, METH_VARARGS,
         "Check sync client connection"},
        {"node_sync_error_str", (PyCFunction)dap_chain_node_sync_error_str_py, METH_VARARGS,
         "Get sync error string"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}

int cellframe_node_init(PyObject *module) {
    PyMethodDef *methods = cellframe_node_get_methods();
    if (!methods) {
        log_it(L_ERROR, "Failed to get node methods");
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

    log_it(L_DEBUG, "Node module initialized successfully");
    return 0;
}
