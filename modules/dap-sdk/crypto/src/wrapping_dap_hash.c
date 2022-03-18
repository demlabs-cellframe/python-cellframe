#include "wrapping_dap_hash.h"

/* Hash type */
PyMethodDef DapHashTypeMethods[] = {
        {"DAP_HASH_TYPE_KECCAK", (PyCFunction)DAP_HASH_TYPE_KECCAK_PY, METH_NOARGS | METH_STATIC, ""},
        {"DAP_HASH_TYPE_SLOW_0", (PyCFunction)DAP_HASH_TYPE_SLOW_0_PY, METH_NOARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapHashTypeObject_DapChainHashTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.HashType",          /* tp_name */
        sizeof(PyDapHashTypeObject),   /* tp_basicsize */
        0,                               /* tp_itemsize */
        0,                               /* tp_dealloc */
        0,                               /* tp_print */
        0,                               /* tp_getattr */
        0,                               /* tp_setattr */
        0,                               /* tp_reserved */
        0,                               /* tp_repr */
        0,                               /* tp_as_number */
        0,                               /* tp_as_sequence */
        0,                               /* tp_as_mapping */
        0,                               /* tp_hash  */
        0,                               /* tp_call */
        0,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Hash type object",           /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapHashTypeMethods,         /* tp_methods */
        0,                               /* tp_members */
        0,                               /* tp_getset */
        0,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        0,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

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

/* Chain hash fast */
PyMethodDef DapHashFastMethods[] = {
        {"fromString", (PyCFunction)dap_chain_str_to_hash_fast_py, METH_VARARGS | METH_STATIC, ""},
        {"hashFast", (PyCFunction)dap_hash_fast_py, METH_VARARGS, ""},
        {"compare", (PyCFunction)dap_hash_fast_compare_py, METH_VARARGS | METH_STATIC, ""},
        {"isBlank", (PyCFunction)dap_hash_fast_is_blank_py, METH_VARARGS, ""},
        {"toStr", (PyCFunction)dap_chain_hash_fast_to_str_py, METH_VARARGS, ""},
        {"toStrNew", (PyCFunction)dap_chain_hash_fast_to_str_new_py, METH_VARARGS, ""},
        {"__str__", (PyCFunction)wrapping_dap_hash_to_str, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapHashFastObject_DapHashFastObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.HashFast",          /* tp_name */
        sizeof(PyDapHashFastObject),   /* tp_basicsize */
        0,                               /* tp_itemsize */
        0,                               /* tp_dealloc */
        0,                               /* tp_print */
        0,                               /* tp_getattr */
        0,                               /* tp_setattr */
        0,                               /* tp_reserved */
        0,                               /* tp_repr */
        0,                               /* tp_as_number */
        0,                               /* tp_as_sequence */
        0,                               /* tp_as_mapping */
        0,                               /* tp_hash  */
        0,                               /* tp_call */
        wrapping_dap_hash_to_str,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Hash fast object",           /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapHashFastMethods,         /* tp_methods */
        0,                               /* tp_members */
        0,                               /* tp_getset */
        0,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        0,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

PyObject *dap_chain_str_to_hash_fast_py(PyObject *self, PyObject *args){
    const char *hash_str;
    if (!PyArg_ParseTuple(args, "s", &hash_str))
        return NULL;
    PyDapHashFastObject *obj_hash_fast = self ? (PyDapHashFastObject *)self :
                                                 PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
    obj_hash_fast->hash_fast = DAP_NEW(dap_hash_fast_t);
    if (dap_chain_hash_fast_from_str(hash_str, obj_hash_fast->hash_fast))
        DAP_DEL_Z(obj_hash_fast->hash_fast);
    return (PyObject*)obj_hash_fast;
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
    return obj_str;
}
