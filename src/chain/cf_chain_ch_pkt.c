#include "cf_chain_internal.h"
#include "dap_chain_ch_pkt.h"

#define LOG_TAG "python_cellframe_chain"

// =========================================
// CHAIN CH PACKET OPERATIONS
// =========================================

PyObject* dap_chain_ch_pkt_get_size_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_pkt_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_pkt_capsule)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_pkt_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain packet capsule");
        return NULL;
    }
    
    dap_chain_ch_pkt_t *l_pkt = (dap_chain_ch_pkt_t*)PyCapsule_GetPointer(l_pkt_capsule, "dap_chain_ch_pkt_t");
    if (!l_pkt) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain packet capsule");
        return NULL;
    }
    
    size_t l_size = dap_chain_ch_pkt_get_size(l_pkt);
    return PyLong_FromSize_t(l_size);
}



PyObject* dap_chain_ch_pkt_new_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    unsigned long long l_chain_id = 0;
    PyObject *l_cell_id_obj = Py_None;
    const char *l_data = NULL;
    Py_ssize_t l_data_size = 0;
    unsigned int l_version = DAP_CHAIN_CH_PKT_VERSION_CURRENT;
    
    if (!PyArg_ParseTuple(a_args, "KKOy#|I", &l_net_id, &l_chain_id, &l_cell_id_obj, &l_data, &l_data_size, &l_version)) {
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    dap_chain_ch_pkt_t *l_pkt = dap_chain_ch_pkt_new(l_net_id_struct, l_chain_id_struct, l_cell_id,
                                                     l_data, (size_t)l_data_size, (uint8_t)l_version);
    if (!l_pkt) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create chain packet");
        return NULL;
    }
    
    return PyCapsule_New(l_pkt, "dap_chain_ch_pkt_t", NULL);
}



PyObject* dap_chain_ch_pkt_write_unsafe_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_ch_capsule = NULL;
    unsigned int l_type = 0;
    unsigned long long l_net_id = 0;
    unsigned long long l_chain_id = 0;
    PyObject *l_cell_id_obj = Py_None;
    unsigned int l_data_size_param = 0;
    const char *l_data = NULL;
    Py_ssize_t l_data_size = 0;
    unsigned int l_version = DAP_CHAIN_CH_PKT_VERSION_CURRENT;
    
    if (!PyArg_ParseTuple(a_args, "OIKKOIy#|I", &l_ch_capsule, &l_type, &l_net_id, &l_chain_id,
                          &l_cell_id_obj, &l_data_size_param, &l_data, &l_data_size, &l_version)) {
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
    size_t l_payload_size = (l_data_size_param > 0 && l_data_size_param <= (unsigned int)l_data_size)
                                ? (size_t)l_data_size_param
                                : (size_t)l_data_size;
    size_t l_written = dap_chain_ch_pkt_write_unsafe(l_ch, (uint8_t)l_type, l_net_id_struct, l_chain_id_struct,
                                                     l_cell_id, l_data, l_payload_size, (uint8_t)l_version);
    return PyLong_FromSize_t(l_written);
}



PyObject* dap_chain_ch_pkt_write_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_worker_capsule = NULL;
    unsigned int l_ch_uuid = 0;
    unsigned int l_type = 0;
    unsigned long long l_net_id = 0;
    unsigned long long l_chain_id = 0;
    PyObject *l_cell_id_obj = Py_None;
    const char *l_data = NULL;
    Py_ssize_t l_data_size = 0;
    unsigned int l_version = DAP_CHAIN_CH_PKT_VERSION_CURRENT;
    
    if (!PyArg_ParseTuple(a_args, "OIIKKOy#|I", &l_worker_capsule, &l_ch_uuid, &l_type, &l_net_id, &l_chain_id,
                          &l_cell_id_obj, &l_data, &l_data_size, &l_version)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_worker_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected stream worker capsule");
        return NULL;
    }
    
    dap_stream_worker_t *l_worker = (dap_stream_worker_t*)PyCapsule_GetPointer(l_worker_capsule, "dap_stream_worker_t");
    if (!l_worker) {
        PyErr_SetString(PyExc_ValueError, "Invalid stream worker capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_id_t l_chain_id_struct = {.uint64 = l_chain_id};
    size_t l_written = dap_chain_ch_pkt_write(l_worker, (dap_stream_ch_uuid_t)l_ch_uuid, (uint8_t)l_type,
                                              l_net_id_struct, l_chain_id_struct, l_cell_id,
                                              l_data, (size_t)l_data_size, (uint8_t)l_version);
    return PyLong_FromSize_t(l_written);
}



PyObject* dap_chain_ch_pkt_type_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_type = 0;
    
    if (!PyArg_ParseTuple(a_args, "I", &l_type)) {
        return NULL;
    }
    
    const char *l_str = dap_chain_ch_pkt_type_to_str((uint8_t)l_type);
    return PyUnicode_FromString(l_str ? l_str : "DAP_CHAIN_CH_PKT_TYPE_UNKNOWN");
}

// =========================================
// CHAIN NET UTILS
// =========================================
