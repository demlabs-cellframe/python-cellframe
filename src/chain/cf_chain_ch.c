#include "cf_chain_internal.h"
#include "dap_chain_ch.h"

#define LOG_TAG "python_cellframe_chain"

// =========================================
// CHAIN CH OPERATIONS
// =========================================

PyObject* dap_chain_ch_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_result = dap_chain_ch_init();
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_ch_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_ch_deinit();
    Py_RETURN_NONE;
}



PyObject* dap_stream_ch_write_error_unsafe_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ch_capsule = NULL;
    unsigned long long l_net_id = 0;
    unsigned long long l_chain_id = 0;
    PyObject *l_cell_id_obj = Py_None;
    unsigned int l_error_type = 0;
    
    if (!PyArg_ParseTuple(a_args, "OKKOI", &l_ch_capsule, &l_net_id, &l_chain_id, &l_cell_id_obj, &l_error_type)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_ch_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected stream channel capsule");
        return NULL;
    }
    
    dap_stream_ch_t *l_ch = (dap_stream_ch_t*)PyCapsule_GetPointer(l_ch_capsule, "dap_stream_ch_t");
    if (!l_ch) {
        PyErr_SetString(PyExc_ValueError, "Invalid stream channel capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    dap_stream_ch_write_error_unsafe(l_ch, l_net_id_struct, l_chain_id_struct, l_cell_id,
                                     (dap_chain_ch_error_type_t)l_error_type);
    Py_RETURN_NONE;
}

// =========================================
// CHAIN CH PACKET OPERATIONS
// =========================================
