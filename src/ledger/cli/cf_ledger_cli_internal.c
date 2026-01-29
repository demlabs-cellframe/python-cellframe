#include "../include/cf_ledger_internal.h"
#include "dap_chain_ledger_cli_internal.h"
#include "dap_json.h"

static int cf_ledger_cli_build_argv(PyObject *a_argv_list, char ***a_argv_out, int *a_argc_out) {
    if (!PyList_Check(a_argv_list)) {
        PyErr_SetString(PyExc_TypeError, "argv must be a list");
        return -1;
    }

    Py_ssize_t l_argc = PyList_Size(a_argv_list);
    char **l_argv = DAP_NEW_Z_COUNT(char*, (size_t)l_argc + 1);
    if (!l_argv) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate argv");
        return -1;
    }

    for (Py_ssize_t i = 0; i < l_argc; i++) {
        PyObject *l_item = PyList_GetItem(a_argv_list, i);
        if (PyUnicode_Check(l_item)) {
            l_argv[i] = (char *)PyUnicode_AsUTF8(l_item);
        } else if (PyBytes_Check(l_item)) {
            l_argv[i] = PyBytes_AsString(l_item);
        } else {
            PyErr_SetString(PyExc_TypeError, "argv items must be str or bytes");
            DAP_DELETE(l_argv);
            return -1;
        }

        if (!l_argv[i]) {
            PyErr_SetString(PyExc_ValueError, "Failed to parse argv item");
            DAP_DELETE(l_argv);
            return -1;
        }
    }

    l_argv[l_argc] = NULL;
    *a_argv_out = l_argv;
    *a_argc_out = (int)l_argc;
    return 0;
}

static void cf_ledger_cli_free_argv(char **a_argv) {
    DAP_DELETE(a_argv);
}

PyObject* cli_parse_hash_out_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_argv_list = NULL;
    int l_arg_index = 0;

    if (!PyArg_ParseTuple(a_args, "Oi", &l_argv_list, &l_arg_index)) {
        return NULL;
    }

    char **l_argv = NULL;
    int l_argc = 0;
    if (cf_ledger_cli_build_argv(l_argv_list, &l_argv, &l_argc) != 0) {
        return NULL;
    }

    const char *l_hash_out_type = NULL;
    int l_result = cli_parse_hash_out_type(l_argv, l_arg_index, l_argc, &l_hash_out_type);
    cf_ledger_cli_free_argv(l_argv);

    PyObject *l_hash_obj = l_hash_out_type ? PyUnicode_FromString(l_hash_out_type) : Py_None;
    if (!l_hash_out_type) {
        Py_INCREF(Py_None);
    }

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_result));
    PyTuple_SetItem(l_tuple, 1, l_hash_obj);
    return l_tuple;
}

PyObject* cli_get_ledger_by_net_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name = NULL;
    PyObject *l_json_reply_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "s|O", &l_net_name, &l_json_reply_obj)) {
        return NULL;
    }

    dap_json_t *l_json_reply = NULL;
    bool l_created = false;
    if (l_json_reply_obj && l_json_reply_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_json_reply_obj)) {
            PyErr_SetString(PyExc_TypeError, "json_reply must be a JSON capsule or None");
            return NULL;
        }
        l_json_reply = (dap_json_t *)PyCapsule_GetPointer(l_json_reply_obj, "dap_json_t");
        if (!l_json_reply) {
            PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
            return NULL;
        }
    } else {
        l_json_reply = dap_json_array_new();
        l_created = true;
    }

    dap_ledger_t *l_ledger = cli_get_ledger_by_net_name(l_net_name, l_json_reply);

    PyObject *l_json_str_obj = Py_None;
    if (l_json_reply) {
        char *l_json_str = dap_json_to_string(l_json_reply);
        if (l_json_str) {
            l_json_str_obj = PyUnicode_FromString(l_json_str);
            DAP_DELETE(l_json_str);
        } else {
            Py_INCREF(Py_None);
        }
    } else {
        Py_INCREF(Py_None);
    }

    if (l_created && l_json_reply) {
        dap_json_object_free(l_json_reply);
    }

    PyObject *l_ledger_obj = l_ledger ? PyCapsule_New(l_ledger, "dap_ledger_t", NULL) : Py_None;
    if (!l_ledger) {
        Py_INCREF(Py_None);
    }

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, l_ledger_obj);
    PyTuple_SetItem(l_tuple, 1, l_json_str_obj);
    return l_tuple;
}

PyObject* cli_parse_pagination_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_argv_list = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_argv_list)) {
        return NULL;
    }

    char **l_argv = NULL;
    int l_argc = 0;
    if (cf_ledger_cli_build_argv(l_argv_list, &l_argv, &l_argc) != 0) {
        return NULL;
    }

    size_t l_limit = 0;
    size_t l_offset = 0;
    int l_result = cli_parse_pagination(l_argv, l_argc, &l_limit, &l_offset);
    cf_ledger_cli_free_argv(l_argv);

    PyObject *l_tuple = PyTuple_New(3);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_result));
    PyTuple_SetItem(l_tuple, 1, PyLong_FromSize_t(l_limit));
    PyTuple_SetItem(l_tuple, 2, PyLong_FromSize_t(l_offset));
    return l_tuple;
}

PyMethodDef* cellframe_ledger_cli_internal_get_methods(void) {
    static PyMethodDef methods[] = {
        {"cli_parse_hash_out_type", (PyCFunction)cli_parse_hash_out_type_py, METH_VARARGS,
         "Parse hash output type from CLI args"},
        {"cli_get_ledger_by_net_name", (PyCFunction)cli_get_ledger_by_net_name_py, METH_VARARGS,
         "Get ledger by network name"},
        {"cli_parse_pagination", (PyCFunction)cli_parse_pagination_py, METH_VARARGS,
         "Parse pagination parameters from CLI args"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
