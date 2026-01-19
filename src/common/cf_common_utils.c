#include "cf_common_utils.h"
#include <string.h>

void cf_common_clear_ctx(python_callback_ctx_t **ctx_ptr) {
    if (!ctx_ptr || !*ctx_ptr) {
        return;
    }
    cf_callbacks_registry_remove(*ctx_ptr);
    DAP_DELETE(*ctx_ptr);
    *ctx_ptr = NULL;
}

int cf_common_parse_hash_fast(PyObject *obj, dap_hash_fast_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "hash value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_hash_fast_t *ptr = (dap_hash_fast_t *)PyCapsule_GetPointer(obj, "dap_hash_fast_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_hash_fast_t capsule");
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

int cf_common_parse_hash_slow(PyObject *obj, dap_chain_hash_slow_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "hash value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_chain_hash_slow_t *ptr = (dap_chain_hash_slow_t *)PyCapsule_GetPointer(obj, "dap_chain_hash_slow_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_chain_hash_slow_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_chain_hash_slow_t)) {
            PyErr_Format(PyExc_ValueError, "hash must be exactly %zu bytes", sizeof(dap_chain_hash_slow_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_chain_hash_slow_t));
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "hash must be bytes or dap_chain_hash_slow_t capsule");
    return -1;
}

int cf_common_parse_addr(PyObject *obj, dap_chain_addr_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "address value is required");
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

int cf_common_parse_uint256(PyObject *obj, uint256_t *out) {
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

int cf_common_parse_uint128(PyObject *obj, uint128_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "uint128 value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        uint128_t *ptr = (uint128_t *)PyCapsule_GetPointer(obj, "uint128_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid uint128_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(uint128_t)) {
            PyErr_Format(PyExc_ValueError, "uint128 value must be exactly %zu bytes", sizeof(uint128_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(uint128_t));
        return 0;
    }
    if (PyLong_Check(obj)) {
        unsigned long long val = PyLong_AsUnsignedLongLong(obj);
        if (PyErr_Occurred()) {
            return -1;
        }
        *out = dap_chain_uint128_from(val);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "uint128 value must be bytes, int, or uint128_t capsule");
    return -1;
}

void *cf_common_capsule_get_pointer(PyObject *obj, const char *name, const char *error_msg) {
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

void cf_common_capsule_addr_destructor(PyObject *capsule) {
    dap_chain_addr_t *addr = PyCapsule_GetPointer(capsule, "dap_chain_addr_t");
    if (addr) {
        DAP_DELETE(addr);
        return;
    }
    PyErr_Clear();
}

PyObject *cf_common_capsule_new_addr(dap_chain_addr_t *addr) {
    if (!addr) {
        return NULL;
    }
    PyObject *capsule = PyCapsule_New(addr, "dap_chain_addr_t", cf_common_capsule_addr_destructor);
    if (!capsule) {
        DAP_DELETE(addr);
    }
    return capsule;
}

int cf_common_register_methods(PyObject *module, PyMethodDef *methods) {
    for (PyMethodDef *method = methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
        if (!func) {
            return -1;
        }
        if (PyModule_AddObject(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            return -1;
        }
    }
    return 0;
}
