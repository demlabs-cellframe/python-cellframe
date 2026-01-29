#include "cf_common_utils.h"
#include "dap_chain_types.h"

#define LOG_TAG "python_cellframe_common"

PyObject *dap_chain_type_from_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_str = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_str)) {
        return NULL;
    }
    dap_chain_type_t l_type = dap_chain_type_from_str(l_str);
    return PyLong_FromLong((long)l_type);
}

PyObject *dap_chain_type_to_datum_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_type = 0;
    if (!PyArg_ParseTuple(a_args, "i", &l_type)) {
        return NULL;
    }
    uint16_t l_datum_type = dap_chain_type_to_datum_type((dap_chain_type_t)l_type);
    return PyLong_FromUnsignedLong((unsigned long)l_datum_type);
}

PyObject *dap_chain_type_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_type = 0;
    if (!PyArg_ParseTuple(a_args, "i", &l_type)) {
        return NULL;
    }
    const char *l_str = dap_chain_type_to_str((dap_chain_type_t)l_type);
    return PyUnicode_FromString(l_str ? l_str : "custom");
}

PyObject *dap_datum_type_to_chain_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_type = 0;
    if (!PyArg_ParseTuple(a_args, "I", &l_type)) {
        return NULL;
    }
    dap_chain_type_t l_chain_type = dap_datum_type_to_chain_type((uint16_t)l_type);
    return PyLong_FromLong((long)l_chain_type);
}

PyObject *dap_datum_type_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_type = 0;
    if (!PyArg_ParseTuple(a_args, "I", &l_type)) {
        return NULL;
    }
    const char *l_str = dap_datum_type_to_str((uint16_t)l_type);
    return PyUnicode_FromString(l_str ? l_str : "custom");
}

int cf_common_register_chain_types(PyObject *module) {
    static PyMethodDef methods[] = {
        {"dap_chain_type_from_str", dap_chain_type_from_str_py, METH_VARARGS,
         "Convert chain type string to enum"},
        {"dap_chain_type_to_datum_type", dap_chain_type_to_datum_type_py, METH_VARARGS,
         "Convert chain type to datum type"},
        {"dap_chain_type_to_str", dap_chain_type_to_str_py, METH_VARARGS,
         "Convert chain type to string"},
        {"dap_datum_type_to_chain_type", dap_datum_type_to_chain_type_py, METH_VARARGS,
         "Convert datum type to chain type"},
        {"dap_datum_type_to_str", dap_datum_type_to_str_py, METH_VARARGS,
         "Convert datum type to string"},
        {NULL, NULL, 0, NULL}
    };

    return cf_common_register_methods(module, methods);
}
