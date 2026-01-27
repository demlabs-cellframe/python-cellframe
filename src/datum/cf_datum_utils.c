#include "cf_datum_internal.h"
#include "dap_hash.h"

#include <string.h>

void *cf_datum_get_capsule(PyObject *obj, const char *name, const char *error_msg) {
    if (!PyCapsule_CheckExact(obj)) {
        PyErr_SetString(PyExc_TypeError, error_msg);
        return NULL;
    }
    void *ptr = PyCapsule_GetPointer(obj, name);
    if (!ptr) {
        PyErr_SetString(PyExc_ValueError, error_msg);
        return NULL;
    }
    return ptr;
}

const char *cf_datum_get_cstring(PyObject *obj, const char *error_msg) {
    if (PyUnicode_Check(obj)) {
        return PyUnicode_AsUTF8(obj);
    }
    if (PyBytes_Check(obj)) {
        return PyBytes_AsString(obj);
    }
    PyErr_SetString(PyExc_TypeError, error_msg);
    return NULL;
}

int cf_datum_parse_hash_fast(PyObject *obj, dap_hash_fast_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "hash value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        const char *name = PyCapsule_GetName(obj);
        if (!name || (strcmp(name, "dap_hash_fast_t") != 0 && strcmp(name, "dap_chain_hash_fast_t") != 0)) {
            PyErr_SetString(PyExc_ValueError, "Invalid hash capsule name");
            return -1;
        }
        dap_hash_fast_t *ptr = (dap_hash_fast_t *)PyCapsule_GetPointer(obj, name);
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid hash capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_hash_fast_t)) {
            PyErr_Format(PyExc_ValueError, "hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_hash_fast_t));
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *hash_str = PyUnicode_AsUTF8(obj);
        if (!hash_str) {
            return -1;
        }
        if (dap_chain_hash_fast_from_str(hash_str, out) != 0) {
            PyErr_SetString(PyExc_ValueError, "Invalid hash string");
            return -1;
        }
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "hash must be bytes, str, or dap_hash_fast_t capsule");
    return -1;
}

int cf_datum_parse_uint256(PyObject *obj, uint256_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "uint256 value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        uint256_t *ptr = (uint256_t *)PyCapsule_GetPointer(obj, "uint256_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid uint256_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(uint256_t)) {
            PyErr_Format(PyExc_ValueError, "uint256 value must be exactly %zu bytes", sizeof(uint256_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(uint256_t));
        return 0;
    }
    PyObject *str_obj = PyObject_Str(obj);
    if (!str_obj) {
        return -1;
    }
    const char *str_val = PyUnicode_AsUTF8(str_obj);
    if (!str_val) {
        Py_DECREF(str_obj);
        return -1;
    }
    *out = dap_chain_balance_scan(str_val);
    Py_DECREF(str_obj);
    return 0;
}

int cf_datum_parse_addr(PyObject *obj, dap_chain_addr_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "address is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_chain_addr_t *ptr = (dap_chain_addr_t *)PyCapsule_GetPointer(obj, "dap_chain_addr_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_chain_addr_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_chain_addr_t)) {
            PyErr_Format(PyExc_ValueError, "address must be exactly %zu bytes", sizeof(dap_chain_addr_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_chain_addr_t));
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *addr_str = PyUnicode_AsUTF8(obj);
        if (!addr_str) {
            return -1;
        }
        dap_chain_addr_t *addr = dap_chain_addr_from_str(addr_str);
        if (!addr) {
            PyErr_SetString(PyExc_ValueError, "Invalid address string");
            return -1;
        }
        *out = *addr;
        DAP_DELETE(addr);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "address must be bytes, str, or dap_chain_addr_t capsule");
    return -1;
}

int cf_datum_parse_node_addr(PyObject *obj, dap_chain_node_addr_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "node address value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_chain_node_addr_t *ptr = (dap_chain_node_addr_t *)PyCapsule_GetPointer(obj, "dap_chain_node_addr_t");
        if (!ptr) {
            ptr = (dap_chain_node_addr_t *)PyCapsule_GetPointer(obj, "dap_stream_node_addr_t");
        }
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid node address capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyLong_Check(obj)) {
        unsigned long long val = PyLong_AsUnsignedLongLong(obj);
        if (PyErr_Occurred()) {
            return -1;
        }
        out->uint64 = val;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_chain_node_addr_t)) {
            PyErr_Format(PyExc_ValueError, "node address must be exactly %zu bytes", sizeof(dap_chain_node_addr_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_chain_node_addr_t));
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *addr_str = PyUnicode_AsUTF8(obj);
        if (!addr_str) {
            return -1;
        }
        if (dap_chain_node_addr_from_str(out, addr_str) != 0) {
            PyErr_SetString(PyExc_ValueError, "Invalid node address string");
            return -1;
        }
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "node address must be int, bytes, str, or node address capsule");
    return -1;
}
