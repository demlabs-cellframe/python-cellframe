#include "wrapping_dap_chain_global_db.h"

PyMethodDef DapChainGlobalDBMethods[] = {
        {"get", (PyCFunction)wrapping_dap_chain_global_db_gr_get, METH_VARARGS | METH_STATIC, ""},
        {"set", (PyCFunction)wrapping_dap_chain_global_db_gr_set, METH_VARARGS | METH_STATIC, ""},
        {"delete", (PyCFunction)wrapping_dap_chain_global_db_gr_del, METH_VARARGS | METH_STATIC, ""},
        {"pin", (PyCFunction)wrapping_dap_chain_global_db_gr_pin, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainGlobalDBObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainGlobalDB",                                            /* tp_name */
        sizeof(PyDapChainGlobalDBObject),                                     /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        0,                                                            /* tp_dealloc */
        0,                                                            /* tp_print */
        0,                                                            /* tp_getattr */
        0,                                                            /* tp_setattr */
        0,                                                            /* tp_reserved */
        0,                                                            /* tp_repr */
        0,                                                            /* tp_as_number */
        0,                                                            /* tp_as_sequence */
        0,                                                            /* tp_as_mapping */
        0,                                                            /* tp_hash  */
        0,                                                            /* tp_call */
        0,                                                            /* tp_str */
        0,                                                            /* tp_getattro */
        0,                                                            /* tp_setattro */
        0,                                                            /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                                      /* tp_flags */
        "Chain GlobalDB object",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        DapChainGlobalDBMethods,                                      /* tp_methods */
        0,                                                            /* tp_members */
        0,                                                            /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */
};

PyObject *wrapping_dap_chain_global_db_gr_get(PyObject *self, PyObject *args){
    (void)self;
    const char *l_key;
    const char *l_group;
    if (!PyArg_ParseTuple(args, "ss", &l_key, &l_group)){
        return NULL;
    }
    size_t l_size_data = 0;
    void *l_bytes = dap_chain_global_db_gr_get(l_key, &l_size_data, l_group);
    if (l_size_data == 0)
        return Py_None;
    PyObject *l_obj_bytes = PyBytes_FromStringAndSize(l_bytes, (Py_ssize_t)l_size_data);
    return l_obj_bytes;
}
PyObject *wrapping_dap_chain_global_db_gr_set(PyObject *self, PyObject *args){
    (void)self;
    char *l_key;
    char *l_group;
    PyObject *obj_byte;
    if (!PyArg_ParseTuple(args, "ssO", &l_key, &l_group, &obj_byte)){
        return NULL;
    }
    if (!PyBytes_Check(obj_byte)){
        return NULL;
    }
    char *l_key_dup = dap_strdup(l_key);
    void *l_bytes = PyBytes_AsString(obj_byte);
    size_t l_bytes_size = PyBytes_Size(obj_byte);
    void *l_bytes_dup = dap_strdup(l_bytes);
    bool ret = dap_chain_global_db_gr_set(l_key_dup, l_bytes_dup, l_bytes_size, l_group);
    if (ret == true){
        return Py_True;
    } else {
        return Py_False;
    }
}
PyObject *wrapping_dap_chain_global_db_gr_del(PyObject *self, PyObject *args){
    (void)self;
    const char *l_key;
    const char *l_group;
    if (!PyArg_ParseTuple(args, "ss",&l_key, &l_group)){
        return NULL;
    }
    char *l_key_dup = dap_strdup(l_key);
    char *l_group_dup = dap_strdup(l_group);
    bool ret = dap_chain_global_db_gr_del(l_key_dup, l_group_dup);
    if (ret == true)
        return Py_True;
    else
        return Py_False;
}

PyObject *wrapping_dap_chain_global_db_gr_pin(PyObject *self, PyObject *args){
    return Py_None;
}
