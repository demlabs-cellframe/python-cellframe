#include "cellframe.h"
#include "cf_services_ext.h"
#include "dap_chain_net_srv_datum.h"

// =============================================================================
// DATUM SERVICE
// =============================================================================

PyObject* dap_chain_net_srv_datum_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    int l_res = dap_chain_net_srv_datum_init();
    return PyLong_FromLong(l_res);
}

PyObject* dap_chain_net_srv_datum_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    if (!PyArg_ParseTuple(a_args, ""))
        return NULL;
    dap_chain_net_srv_datum_deinit();
    Py_RETURN_NONE;
}

PyObject* dap_chain_net_srv_file_datum_data_read_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_path = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_path))
        return NULL;

    size_t l_data_size = 0;
    uint8_t *l_data = dap_chain_net_srv_file_datum_data_read((char *)l_path, &l_data_size);
    if (!l_data) {
        Py_RETURN_NONE;
    }

    PyObject *l_bytes = PyBytes_FromStringAndSize((const char *)l_data, (Py_ssize_t)l_data_size);
    DAP_DELETE(l_data);
    return l_bytes;
}

PyObject* dap_chain_net_srv_datum_custom_add_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    const char *l_data = NULL;
    Py_ssize_t l_data_size = 0;

    if (!PyArg_ParseTuple(a_args, "Oy#", &l_chain_obj, &l_data, &l_data_size))
        return NULL;

    if (!PyCapsule_CheckExact(l_chain_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a chain capsule");
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)PyCapsule_GetPointer(l_chain_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    char *l_hash_str = dap_chain_net_srv_datum_custom_add(l_chain, (const uint8_t *)l_data, (size_t)l_data_size);
    if (!l_hash_str) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = PyUnicode_FromString(l_hash_str);
    DAP_DELETE(l_hash_str);
    return l_result;
}
