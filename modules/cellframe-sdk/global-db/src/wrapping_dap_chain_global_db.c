#include "wrapping_dap_chain_global_db.h"

PyMethodDef DapChainGlobalDBMethods[] = {
        {"get", (PyCFunction)wrapping_dap_chain_global_db_gr_get, METH_VARARGS | METH_STATIC, ""},
        {"set", (PyCFunction)wrapping_dap_chain_global_db_gr_set, METH_VARARGS | METH_STATIC, ""},
        {"delete", (PyCFunction)wrapping_dap_chain_global_db_gr_del, METH_VARARGS | METH_STATIC, ""},
        {"pin", (PyCFunction)wrapping_dap_chain_global_db_gr_pin, METH_VARARGS | METH_STATIC, ""},
        {"grLoad", (PyCFunction)wrapping_dap_chain_global_db_gr_load, METH_VARARGS | METH_STATIC, ""},
        {"grDelete", (PyCFunction)wrapping_dap_chain_global_db_gr_delete_group, METH_VARARGS | METH_STATIC, ""},
        {"addSyncExtraGroup", (PyCFunction)wrapping_dap_chain_global_db_add_sync_extra_group, METH_VARARGS | METH_STATIC, ""},
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
        Py_RETURN_NONE;
    PyObject *l_obj_bytes = PyBytes_FromStringAndSize(l_bytes, (Py_ssize_t)l_size_data);
    DAP_DELETE(l_bytes);
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
    void *l_bytes = PyBytes_AsString(obj_byte);
    size_t l_bytes_size = PyBytes_Size(obj_byte);
    bool ret = dap_chain_global_db_gr_set(l_key, l_bytes, l_bytes_size, l_group);
    if (ret == true){
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}
PyObject *wrapping_dap_chain_global_db_gr_del(PyObject *self, PyObject *args){
    (void)self;
    const char *l_key;
    const char *l_group;
    if (!PyArg_ParseTuple(args, "ss",&l_key, &l_group)){
        return NULL;
    }
    bool ret = dap_chain_global_db_gr_del(l_key, l_group);
    if (ret == true)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject *wrapping_dap_chain_global_db_gr_delete_group(PyObject *self, PyObject *args){
    (void)self;
    
    const char *l_group;
    if (!PyArg_ParseTuple(args, "s", &l_group)){
        return NULL;
    }

    bool ret = dap_chain_global_db_gr_del(NULL, l_group);
    if (ret == true)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}


PyObject *wrapping_dap_chain_global_db_gr_pin(PyObject *self, PyObject *args){
    Py_RETURN_NONE;
}

PyObject *wrapping_dap_chain_global_db_gr_load(PyObject *self, PyObject *args){
    (void)self;
    char *l_group;
    if (!PyArg_ParseTuple(args, "s", &l_group)){
        return NULL;
    }
    size_t l_data_out = 0;
    dap_global_db_obj_t *l_db_obj = dap_chain_global_db_gr_load(l_group, &l_data_out);
    if (l_data_out == 0){
        Py_RETURN_NONE;
    }
    PyObject* l_list = PyList_New(l_data_out);
    for (size_t i = 0; i < l_data_out; i++){
        PyDapChainGlobalDBContainerObject *l_obj = PyObject_New(PyDapChainGlobalDBContainerObject ,
                                                                &DapChainGlobalDBContainerObjectType);
        l_obj->obj = l_db_obj[i];
        PyList_SetItem(l_list, i, (PyObject*)l_obj);
    }
    return l_list;
}

typedef struct _wrapping_dap_chain_global_db_add_sync_extra_group_callback{
    PyObject *func;
    PyObject *arg;
}_wrapping_dap_chain_global_db_add_sync_extra_group_callback_t;

void pvt_wrapping_dap_chain_global_db_add_sync_extra_group_func_callback(void * a_arg, const char a_op_code, const char * a_group,
                                                                     const char * a_key, const void * a_value, const size_t a_value_len){
    if (!a_arg)
        return;
    _wrapping_dap_chain_global_db_add_sync_extra_group_callback_t *l_callback = (_wrapping_dap_chain_global_db_add_sync_extra_group_callback_t *)a_arg;
    PyObject *l_obj_value = NULL;
    char l_op_code[2];
    l_op_code[0] = a_op_code;
    l_op_code[1] = '\0';
    PyGILState_STATE state = PyGILState_Ensure();
    if (a_value == NULL || a_value_len == 0){
        l_obj_value = Py_None;
    } else {
        l_obj_value = PyBytes_FromStringAndSize(a_value, (Py_ssize_t)a_value_len);
    }
    PyObject *argv = Py_BuildValue("sssOO", l_op_code, a_group, a_key, l_obj_value, l_callback->arg);
    PyEval_CallObject(l_callback->func, argv);
    Py_DECREF(argv);
    PyGILState_Release(state);
}

PyObject *wrapping_dap_chain_global_db_add_sync_extra_group(PyObject *self, PyObject *args){
    (void)self;
    char *net_name;
    char *group_mask;
    PyObject *call_func;
    PyObject *args_func;
    if(!PyArg_ParseTuple(args, "ssOO", &net_name, &group_mask, &call_func, &args_func)){
        return NULL;
    }
    if (!PyCallable_Check(call_func)){
        PyErr_SetString(PyExc_AttributeError, "Argument must be callable");
        return NULL;
    }
    _wrapping_dap_chain_global_db_add_sync_extra_group_callback_t *l_callback = DAP_NEW(_wrapping_dap_chain_global_db_add_sync_extra_group_callback_t);
    l_callback->func = call_func;
    l_callback->arg = args_func;
    Py_INCREF(call_func);
    Py_INCREF(args_func);
    dap_chain_global_db_add_sync_extra_group(net_name, group_mask, pvt_wrapping_dap_chain_global_db_add_sync_extra_group_func_callback, l_callback);
    Py_RETURN_NONE;
}
