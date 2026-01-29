#include "cellframe.h"
#include "dap_chain_net_ch_pkt.h"

#define LOG_TAG "python_cellframe_net_ch_pkt"

PyObject* dap_chain_net_ch_pkt_write_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ch_obj = NULL;
    int l_type = 0;
    unsigned long long l_net_id = 0;
    const char *l_data = NULL;
    Py_ssize_t l_data_size = 0;

    if (!PyArg_ParseTuple(a_args, "OiKy#", &l_ch_obj, &l_type, &l_net_id, &l_data, &l_data_size)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ch_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a stream channel capsule");
        return NULL;
    }

    if (l_type < 0 || l_type > 255) {
        PyErr_SetString(PyExc_ValueError, "type must be between 0 and 255");
        return NULL;
    }

    dap_stream_ch_t *l_ch = (dap_stream_ch_t *)PyCapsule_GetPointer(l_ch_obj, "dap_stream_ch_t");
    if (!l_ch) {
        PyErr_SetString(PyExc_ValueError, "Invalid stream channel capsule");
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    size_t l_written = dap_chain_net_ch_pkt_write(l_ch, (uint8_t)l_type, l_net_id_struct, l_data, (size_t)l_data_size);
    return PyLong_FromSize_t(l_written);
}

PyObject* dap_chain_net_ch_pkt_write_f_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ch_obj = NULL;
    int l_type = 0;
    unsigned long long l_net_id = 0;
    const char *l_fmt = NULL;
    PyObject *l_fmt_args = Py_None;

    if (!PyArg_ParseTuple(a_args, "OiKs|O", &l_ch_obj, &l_type, &l_net_id, &l_fmt, &l_fmt_args)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_ch_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a stream channel capsule");
        return NULL;
    }

    if (l_type < 0 || l_type > 255) {
        PyErr_SetString(PyExc_ValueError, "type must be between 0 and 255");
        return NULL;
    }

    dap_stream_ch_t *l_ch = (dap_stream_ch_t *)PyCapsule_GetPointer(l_ch_obj, "dap_stream_ch_t");
    if (!l_ch) {
        PyErr_SetString(PyExc_ValueError, "Invalid stream channel capsule");
        return NULL;
    }

    PyObject *l_fmt_obj = PyUnicode_FromString(l_fmt);
    if (!l_fmt_obj) {
        return NULL;
    }

    PyObject *l_args_tuple = NULL;
    if (!l_fmt_args || l_fmt_args == Py_None) {
        l_args_tuple = PyTuple_New(0);
    } else if (PyList_Check(l_fmt_args)) {
        l_args_tuple = PyList_AsTuple(l_fmt_args);
    } else if (PyTuple_Check(l_fmt_args) || PyDict_Check(l_fmt_args)) {
        Py_INCREF(l_fmt_args);
        l_args_tuple = l_fmt_args;
    } else {
        Py_DECREF(l_fmt_obj);
        PyErr_SetString(PyExc_TypeError, "format args must be a tuple, list, dict, or None");
        return NULL;
    }

    if (!l_args_tuple) {
        Py_DECREF(l_fmt_obj);
        return NULL;
    }

    PyObject *l_formatted = PyUnicode_Format(l_fmt_obj, l_args_tuple);
    Py_DECREF(l_fmt_obj);
    Py_DECREF(l_args_tuple);
    if (!l_formatted) {
        return NULL;
    }

    const char *l_data = PyUnicode_AsUTF8(l_formatted);
    if (!l_data) {
        Py_DECREF(l_formatted);
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    size_t l_written = dap_chain_net_ch_pkt_write(l_ch, (uint8_t)l_type, l_net_id_struct, l_data, strlen(l_data));
    Py_DECREF(l_formatted);
    return PyLong_FromSize_t(l_written);
}

PyMethodDef* cellframe_net_ch_pkt_get_methods(void) {
    static PyMethodDef methods[] = {
        {"net_ch_pkt_write", (PyCFunction)dap_chain_net_ch_pkt_write_py, METH_VARARGS, "Write net channel packet"},
        {"net_ch_pkt_write_f", (PyCFunction)dap_chain_net_ch_pkt_write_f_py, METH_VARARGS,
         "Write formatted net channel packet"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}

int cellframe_net_ch_pkt_init(PyObject *module) {
    PyMethodDef *methods = cellframe_net_ch_pkt_get_methods();
    if (!methods) {
        log_it(L_ERROR, "Failed to get net channel packet methods");
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

    log_it(L_DEBUG, "Net channel packet module initialized successfully");
    return 0;
}
