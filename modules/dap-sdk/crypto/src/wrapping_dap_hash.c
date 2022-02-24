#include "wrapping_dap_hash.h"

PyObject *DAP_HASH_TYPE_KECCAK_PY(){
    PyObject *obj = _PyObject_New(&DapHashTypeObject_DapChainHashTypeObjectType);
    ((PyDapHashTypeObject*)obj)->hash_type = DAP_HASH_TYPE_KECCAK;
    return Py_BuildValue("O", obj);
}
PyObject *DAP_HASH_TYPE_SLOW_0_PY(){
    PyObject *obj = _PyObject_New(&DapHashTypeObject_DapChainHashTypeObjectType);
    ((PyDapHashTypeObject*)obj)->hash_type = DAP_HASH_TYPE_SLOW_0;
    return Py_BuildValue("O", obj);
}

PyObject *dap_chain_str_to_hash_fast_py(PyObject *self, PyObject *args){
    const char *hash_str;
    if (!PyArg_ParseTuple(args, "s", &hash_str))
        return NULL;
    PyDapHashFastObject *obj_hash_fast = PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
    PyObject_Dir((PyObject*)obj_hash_fast);
    obj_hash_fast->hash_fast = DAP_NEW(dap_hash_fast_t);
    int res = dap_chain_hash_fast_from_str(hash_str, obj_hash_fast->hash_fast);
    return Py_BuildValue("nO", res, (PyObject*)obj_hash_fast);
}

PyObject *dap_hash_fast_py(PyObject *self, PyObject *args){
    PyObject *obj_bytes;
    size_t data_in_size;
    if (!PyArg_ParseTuple(args, "O|n", &obj_bytes, &data_in_size))
        return NULL;
    const void *bytes = (void*)PyBytes_AsString(obj_bytes);
    bool res = dap_hash_fast(bytes, data_in_size, ((PyDapHashFastObject*)self)->hash_fast);
    if (res)
        return Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);
}

PyObject *dap_hash_fast_compare_py(PyObject *self, PyObject *args){
    PyObject *hash1;
    PyObject *hash2;
    if (!PyArg_ParseTuple(args, "O|O", &hash1, &hash2))
        return NULL;
    bool res = dap_hash_fast_compare(((PyDapHashFastObject*)hash1)->hash_fast, ((PyDapHashFastObject*)hash2)->hash_fast);
    if (res)
        return Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);
}

PyObject *dap_hash_fast_is_blank_py(PyObject *self, PyObject *args){
    bool res = dap_hash_fast_is_blank(((PyDapHashFastObject*)self)->hash_fast);
    if (res)
        return Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);
}

PyObject *dap_chain_hash_fast_to_str_py(PyObject *self, PyObject *args){
    char *str;
    size_t str_max;
    if (!PyArg_ParseTuple(args, "s|n", &str, &str_max))
        return NULL;
    int res = dap_chain_hash_fast_to_str(((PyDapHashFastObject*)self)->hash_fast, str, str_max);
    return Py_BuildValue("sn", &str, &str_max);
}

PyObject *dap_chain_hash_fast_to_str_new_py(PyObject *self, PyObject *args){
    char *res = dap_chain_hash_fast_to_str_new(((PyDapHashFastObject*)self)->hash_fast);
    return Py_BuildValue("s", res);
}

PyObject *wrapping_dap_hash_to_str(PyObject *self){
    char str[70];
    if (((PyDapHashFastObject*)self)->hash_fast == NULL){
        Py_BuildValue("s", "Hash is missing.");
    }
    int res = dap_chain_hash_fast_to_str(((PyDapHashFastObject*)self)->hash_fast, str, 70);
    PyObject *obj_str = PyUnicode_FromString(str);
//    PyObject *obj_str = PyUnicode_FromStringAndSize(str, res);
    return obj_str;
}
