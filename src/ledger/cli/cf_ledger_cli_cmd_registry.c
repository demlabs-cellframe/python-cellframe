#include "../include/cf_ledger_internal.h"
#include "dap_chain_ledger_cli_cmd_registry.h"
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

PyObject* dap_ledger_cli_cmd_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_category = NULL;
    const char *l_command = NULL;
    PyObject *l_handler_obj = NULL;
    const char *l_description = NULL;

    if (!PyArg_ParseTuple(a_args, "ssO|s", &l_category, &l_command, &l_handler_obj, &l_description)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_handler_obj)) {
        PyErr_SetString(PyExc_TypeError, "Handler must be a capsule");
        return NULL;
    }

    dap_ledger_cli_cmd_handler_t l_handler = (dap_ledger_cli_cmd_handler_t)PyCapsule_GetPointer(
        l_handler_obj, "dap_ledger_cli_cmd_handler_t"
    );
    if (!l_handler) {
        PyErr_SetString(PyExc_ValueError, "Invalid handler capsule");
        return NULL;
    }

    int l_result = dap_ledger_cli_cmd_register(l_category, l_command, l_handler, l_description);
    return PyLong_FromLong(l_result);
}

PyObject* dap_ledger_cli_cmd_unregister_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_category = NULL;
    const char *l_command = NULL;

    if (!PyArg_ParseTuple(a_args, "ss", &l_category, &l_command)) {
        return NULL;
    }

    dap_ledger_cli_cmd_unregister(l_category, l_command);
    Py_RETURN_NONE;
}

PyObject* dap_ledger_cli_cmd_execute_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_category = NULL;
    const char *l_command = NULL;
    PyObject *l_argv_list = NULL;
    PyObject *l_json_reply_obj = Py_None;
    int l_version = 1;

    if (!PyArg_ParseTuple(a_args, "ssO|Oi", &l_category, &l_command, &l_argv_list,
                          &l_json_reply_obj, &l_version)) {
        return NULL;
    }

    char **l_argv = NULL;
    int l_argc = 0;
    if (cf_ledger_cli_build_argv(l_argv_list, &l_argv, &l_argc) != 0) {
        return NULL;
    }

    dap_json_t *l_json_reply = NULL;
    bool l_created = false;
    if (l_json_reply_obj && l_json_reply_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_json_reply_obj)) {
            cf_ledger_cli_free_argv(l_argv);
            PyErr_SetString(PyExc_TypeError, "json_reply must be a JSON capsule or None");
            return NULL;
        }
        l_json_reply = (dap_json_t *)PyCapsule_GetPointer(l_json_reply_obj, "dap_json_t");
        if (!l_json_reply) {
            cf_ledger_cli_free_argv(l_argv);
            PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
            return NULL;
        }
    } else {
        l_json_reply = dap_json_array_new();
        l_created = true;
    }

    int l_result = dap_ledger_cli_cmd_execute(l_category, l_command, l_argc, l_argv,
                                              l_json_reply, l_version);
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

PyObject* dap_ledger_cli_cmd_is_registered_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_category = NULL;
    PyObject *l_command_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "s|O", &l_category, &l_command_obj)) {
        return NULL;
    }

    const char *l_command = NULL;
    if (l_command_obj && l_command_obj != Py_None) {
        if (!PyUnicode_Check(l_command_obj) && !PyBytes_Check(l_command_obj)) {
            PyErr_SetString(PyExc_TypeError, "command must be str, bytes, or None");
            return NULL;
        }
        l_command = PyUnicode_Check(l_command_obj)
            ? PyUnicode_AsUTF8(l_command_obj)
            : PyBytes_AsString(l_command_obj);
        if (!l_command) {
            PyErr_SetString(PyExc_ValueError, "Invalid command value");
            return NULL;
        }
    }

    bool l_result = dap_ledger_cli_cmd_is_registered(l_category, l_command);
    return PyBool_FromLong(l_result ? 1 : 0);
}

PyObject* dap_ledger_cli_cmd_get_categories_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    size_t l_count = 0;
    const char **l_categories = dap_ledger_cli_cmd_get_categories(&l_count);

    PyObject *l_list = PyList_New((Py_ssize_t)l_count);
    if (!l_list) {
        return NULL;
    }

    for (size_t i = 0; i < l_count; i++) {
        PyObject *l_item = PyUnicode_FromString(l_categories && l_categories[i] ? l_categories[i] : "");
        PyList_SET_ITEM(l_list, (Py_ssize_t)i, l_item);
    }

    return l_list;
}

PyObject* dap_ledger_cli_cmd_get_commands_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_category = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_category)) {
        return NULL;
    }

    size_t l_count = 0;
    const char **l_commands = dap_ledger_cli_cmd_get_commands(l_category, &l_count);

    PyObject *l_list = PyList_New((Py_ssize_t)l_count);
    if (!l_list) {
        return NULL;
    }

    for (size_t i = 0; i < l_count; i++) {
        PyObject *l_item = PyUnicode_FromString(l_commands && l_commands[i] ? l_commands[i] : "");
        PyList_SET_ITEM(l_list, (Py_ssize_t)i, l_item);
    }

    return l_list;
}

PyObject* dap_ledger_cli_cmd_registry_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_result = dap_ledger_cli_cmd_registry_init();
    return PyLong_FromLong(l_result);
}

PyObject* dap_ledger_cli_cmd_registry_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_ledger_cli_cmd_registry_deinit();
    Py_RETURN_NONE;
}

PyMethodDef* cellframe_ledger_cli_cmd_registry_get_methods(void) {
    static PyMethodDef methods[] = {
        {"ledger_cli_cmd_register", (PyCFunction)dap_ledger_cli_cmd_register_py, METH_VARARGS,
         "Register ledger CLI command (expects handler capsule)"},
        {"ledger_cli_cmd_unregister", (PyCFunction)dap_ledger_cli_cmd_unregister_py, METH_VARARGS,
         "Unregister ledger CLI command"},
        {"ledger_cli_cmd_execute", (PyCFunction)dap_ledger_cli_cmd_execute_py, METH_VARARGS,
         "Execute ledger CLI command"},
        {"ledger_cli_cmd_is_registered", (PyCFunction)dap_ledger_cli_cmd_is_registered_py, METH_VARARGS,
         "Check if ledger CLI command is registered"},
        {"ledger_cli_cmd_get_categories", (PyCFunction)dap_ledger_cli_cmd_get_categories_py, METH_VARARGS,
         "Get registered ledger CLI categories"},
        {"ledger_cli_cmd_get_commands", (PyCFunction)dap_ledger_cli_cmd_get_commands_py, METH_VARARGS,
         "Get registered ledger CLI commands for category"},
        {"ledger_cli_cmd_registry_init", (PyCFunction)dap_ledger_cli_cmd_registry_init_py, METH_VARARGS,
         "Initialize ledger CLI command registry"},
        {"ledger_cli_cmd_registry_deinit", (PyCFunction)dap_ledger_cli_cmd_registry_deinit_py, METH_VARARGS,
         "Deinitialize ledger CLI command registry"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
