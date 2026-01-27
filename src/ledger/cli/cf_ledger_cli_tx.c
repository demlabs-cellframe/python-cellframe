#include "../include/cf_ledger_internal.h"
#include "dap_chain_ledger_cli_tx.h"
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

static PyObject* cf_ledger_cli_exec_with_reply(int (*a_func)(int, char **, dap_json_t *, int),
                                               PyObject *a_argv_list,
                                               PyObject *a_json_reply_obj,
                                               int a_version) {
    char **l_argv = NULL;
    int l_argc = 0;
    if (cf_ledger_cli_build_argv(a_argv_list, &l_argv, &l_argc) != 0) {
        return NULL;
    }

    dap_json_t *l_json_reply = NULL;
    bool l_created = false;
    if (a_json_reply_obj && a_json_reply_obj != Py_None) {
        if (!PyCapsule_CheckExact(a_json_reply_obj)) {
            cf_ledger_cli_free_argv(l_argv);
            PyErr_SetString(PyExc_TypeError, "json_reply must be a JSON capsule or None");
            return NULL;
        }
        l_json_reply = (dap_json_t *)PyCapsule_GetPointer(a_json_reply_obj, "dap_json_t");
        if (!l_json_reply) {
            cf_ledger_cli_free_argv(l_argv);
            PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
            return NULL;
        }
    } else {
        l_json_reply = dap_json_array_new();
        l_created = true;
    }

    int l_result = a_func(l_argc, l_argv, l_json_reply, a_version);
    cf_ledger_cli_free_argv(l_argv);

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

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_result));
    PyTuple_SetItem(l_tuple, 1, l_json_str_obj);
    return l_tuple;
}

PyObject* ledger_cli_tx_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_argv_list = NULL;
    PyObject *l_json_reply_obj = Py_None;
    int l_version = 1;

    if (!PyArg_ParseTuple(a_args, "O|Oi", &l_argv_list, &l_json_reply_obj, &l_version)) {
        return NULL;
    }

    return cf_ledger_cli_exec_with_reply(ledger_cli_tx_create, l_argv_list, l_json_reply_obj, l_version);
}

PyObject* ledger_cli_tx_create_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_argv_list = NULL;
    PyObject *l_json_reply_obj = Py_None;
    int l_version = 1;

    if (!PyArg_ParseTuple(a_args, "O|Oi", &l_argv_list, &l_json_reply_obj, &l_version)) {
        return NULL;
    }

    return cf_ledger_cli_exec_with_reply(ledger_cli_tx_create_json, l_argv_list, l_json_reply_obj, l_version);
}

PyObject* ledger_cli_tx_verify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_argv_list = NULL;
    PyObject *l_json_reply_obj = Py_None;
    int l_version = 1;

    if (!PyArg_ParseTuple(a_args, "O|Oi", &l_argv_list, &l_json_reply_obj, &l_version)) {
        return NULL;
    }

    return cf_ledger_cli_exec_with_reply(ledger_cli_tx_verify, l_argv_list, l_json_reply_obj, l_version);
}

PyObject* ledger_cli_tx_history_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_argv_list = NULL;
    PyObject *l_json_reply_obj = Py_None;
    int l_version = 1;

    if (!PyArg_ParseTuple(a_args, "O|Oi", &l_argv_list, &l_json_reply_obj, &l_version)) {
        return NULL;
    }

    return cf_ledger_cli_exec_with_reply(ledger_cli_tx_history, l_argv_list, l_json_reply_obj, l_version);
}

PyObject* dap_chain_ledger_cli_tx_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_result = dap_chain_ledger_cli_tx_init();
    return PyLong_FromLong(l_result);
}

PyObject* dap_chain_ledger_cli_tx_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_ledger_cli_tx_deinit();
    Py_RETURN_NONE;
}

PyMethodDef* cellframe_ledger_cli_tx_get_methods(void) {
    static PyMethodDef methods[] = {
        {"ledger_cli_tx_create", (PyCFunction)ledger_cli_tx_create_py, METH_VARARGS,
         "Create transaction via ledger CLI TX module"},
        {"ledger_cli_tx_create_json", (PyCFunction)ledger_cli_tx_create_json_py, METH_VARARGS,
         "Create transaction from JSON via ledger CLI TX module"},
        {"ledger_cli_tx_verify", (PyCFunction)ledger_cli_tx_verify_py, METH_VARARGS,
         "Verify transaction via ledger CLI TX module"},
        {"ledger_cli_tx_history", (PyCFunction)ledger_cli_tx_history_py, METH_VARARGS,
         "Show transaction history via ledger CLI TX module"},
        {"ledger_cli_tx_init", (PyCFunction)dap_chain_ledger_cli_tx_init_py, METH_VARARGS,
         "Initialize ledger CLI TX module"},
        {"ledger_cli_tx_deinit", (PyCFunction)dap_chain_ledger_cli_tx_deinit_py, METH_VARARGS,
         "Deinitialize ledger CLI TX module"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
