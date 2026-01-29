#include "cf_chain_internal.h"
#include "dap_file_utils.h"

#define LOG_TAG "python_cellframe_chain"

int cf_chain_parse_cell_id(PyObject *obj, dap_chain_cell_id_t *out) {
    if (!out) {
        PyErr_SetString(PyExc_RuntimeError, "Cell ID output pointer is NULL");
        return -1;
    }
    memset(out, 0, sizeof(*out));
    if (!obj || obj == Py_None) {
        return 0;
    }
    if (PyLong_Check(obj)) {
        unsigned long long value = PyLong_AsUnsignedLongLong(obj);
        if (PyErr_Occurred()) {
            PyErr_SetString(PyExc_ValueError, "Failed to parse cell_id as uint64");
            return -1;
        }
        out->uint64 = value;
        return 0;
    }
    if (PyDict_Check(obj)) {
        PyObject *raw_obj = PyDict_GetItemString(obj, "raw");
        if (raw_obj && raw_obj != Py_None) {
            if (!PyBytes_Check(raw_obj)) {
                PyErr_SetString(PyExc_TypeError, "cell_id.raw must be bytes");
                return -1;
            }
            if (PyBytes_Size(raw_obj) != DAP_CHAIN_SHARD_ID_SIZE) {
                PyErr_Format(PyExc_ValueError, "cell_id.raw must be %d bytes", DAP_CHAIN_SHARD_ID_SIZE);
                return -1;
            }
            memcpy(out->raw, PyBytes_AsString(raw_obj), DAP_CHAIN_SHARD_ID_SIZE);
            return 0;
        }
        PyObject *uint64_obj = PyDict_GetItemString(obj, "uint64");
        if (uint64_obj) {
            unsigned long long value = PyLong_AsUnsignedLongLong(uint64_obj);
            if (PyErr_Occurred()) {
                PyErr_SetString(PyExc_ValueError, "Failed to parse cell_id.uint64");
                return -1;
            }
            out->uint64 = value;
            return 0;
        }
        PyErr_SetString(PyExc_TypeError, "cell_id dict must contain 'uint64' or 'raw'");
        return -1;
    }
    if (PyBytes_Check(obj)) {
        if (PyBytes_Size(obj) != DAP_CHAIN_SHARD_ID_SIZE) {
            PyErr_Format(PyExc_ValueError, "cell_id bytes must be %d bytes", DAP_CHAIN_SHARD_ID_SIZE);
            return -1;
        }
        memcpy(out->raw, PyBytes_AsString(obj), DAP_CHAIN_SHARD_ID_SIZE);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "cell_id must be int, dict, bytes, or None");
    return -1;
}



static void cf_chain_ensure_storage_dir(dap_chain_t *a_chain) {
    if (!a_chain) {
        return;
    }
    dap_chain_pvt_t *l_chain_pvt = DAP_CHAIN_PVT(a_chain);
    if (!l_chain_pvt || l_chain_pvt->file_storage_dir) {
        return;
    }
    char l_path[256];
    snprintf(l_path, sizeof(l_path), "/tmp/python_cellframe/%" DAP_UINT64_FORMAT_U "/%" DAP_UINT64_FORMAT_U,
             a_chain->net_id.uint64, a_chain->id.uint64);
    l_chain_pvt->file_storage_dir = dap_strdup(l_path);
    if (l_chain_pvt->file_storage_dir) {
        dap_mkdir_with_parents(l_chain_pvt->file_storage_dir);
    }
}



bool cf_chain_cell_is_configured(dap_chain_t *a_chain) {
    if (!a_chain || !a_chain->config) {
        return false;
    }
    cf_chain_ensure_storage_dir(a_chain);
    dap_chain_pvt_t *l_chain_pvt = DAP_CHAIN_PVT(a_chain);
    return l_chain_pvt && l_chain_pvt->file_storage_dir;
}



int cf_chain_parse_hash(PyObject *obj, dap_hash_fast_t **out_hash) {
    if (!out_hash) {
        PyErr_SetString(PyExc_RuntimeError, "Hash output pointer is NULL");
        return -1;
    }
    *out_hash = NULL;
    if (!obj || obj == Py_None) {
        return 0;
    }
    if (!PyBytes_Check(obj)) {
        PyErr_SetString(PyExc_TypeError, "Hash must be bytes or None");
        return -1;
    }
    if (PyBytes_Size(obj) != (Py_ssize_t)sizeof(dap_hash_fast_t)) {
        PyErr_Format(PyExc_ValueError, "Hash size must be %zu bytes", sizeof(dap_hash_fast_t));
        return -1;
    }
    *out_hash = (dap_hash_fast_t *)PyBytes_AsString(obj);
    if (!*out_hash) {
        PyErr_SetString(PyExc_ValueError, "Failed to parse hash bytes");
        return -1;
    }
    return 0;
}
