#include "libdap_chain_global_db_python.h"

/* Callback created */
static void wrapping_dap_chain_global_db_add_history_callback_notify(void * a_arg, const char a_op_code,
                                                                     const char * a_prefix, const char * a_group,
                                                                     const char * a_key, const void * a_value,
                                                                     const size_t a_value_len){
    PyObject *arglist;
    PyObject *obj_arg = PyBytes_FromString(a_arg);
    PyObject *value = PyBytes_FromString(a_value);
    PyObject *value_len = PyLong_FromSize_t(a_value_len);
    arglist = Py_BuildValue("OsssssO", obj_arg, a_op_code, a_prefix, a_group, a_key, value, value_len);
    PyObject_CallObject(binding_callback_notify, arglist);
    Py_DECREF(arglist);
    Py_DECREF(obj_arg);
    Py_DECREF(value);
    Py_DECREF(value_len);
}
/*--------------------------------*/

int dap_chain_global_db_init_py(dap_config_t *config){
    int res = dap_chain_global_db_init_py(config);
    return res;
}
void dap_chain_global_db_deinit_py(){
    dap_chain_global_db_deinit();
}


void dap_chain_global_db_obj_delete_py(PyDapChainGlobalDBObject *obj){
    if (!obj->obj_clear)
        dap_chain_global_db_obj_delete(obj->db_object);
    Py_TYPE(obj)->tp_free((PyObject*) obj);
}

PyObject *PyDapChainGlobalDB_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapChainGlobalDBObject* obj = (PyDapChainGlobalDBObject*)PyType_GenericNew(type_object, args, kwds);
    obj->obj_clear = false;
    obj->callback_notify = wrapping_dap_chain_global_db_add_history_callback_notify;
    return (PyObject*)obj;
}

PyObject *dap_chain_global_db_flush_py(){
    dap_chain_global_db_flush();
    return PyLong_FromLong(0);
}

PyObject *dap_chain_global_db_objs_delete_py(PyObject *self, PyObject *args){
    Py_ssize_t count;
    if (!PyArg_ParseTuple(args, "n", &count))
        return NULL;
    ((PyDapChainGlobalDBObject*)self)->obj_clear = true;
    dap_chain_global_db_objs_delete(((PyDapChainGlobalDBObject*)self)->db_object, (size_t)count);
    Py_TYPE((PyDapChainGlobalDBObject*)self)->tp_dealloc(self);
    return PyLong_FromLong(0);
}

PyObject *dap_chain_global_db_add_history_group_prefix_py(PyObject *self, PyObject *args){
    (void)self;
    const char *group_prefix;
    const char *group_name_for_history;
    if (!PyArg_ParseTuple(args, "ss", &group_prefix, &group_name_for_history))
        return NULL;
    dap_chain_global_db_add_history_group_prefix(group_prefix, group_name_for_history);
    return PyLong_FromLong(0);
}

PyObject *dap_chain_global_db_add_history_callback_notify_py(PyObject *self, PyObject *args){
    const char *group_prefix;
    PyObject *obj_callback_notify;
    PyObject *obj_arg_bytes;
    if (!PyArg_ParseTuple(args, "s|O:set_callback|O", &group_prefix, &obj_callback_notify, &obj_arg_bytes)) {
        return NULL;
    } else {
        if (!PyCallable_Check(obj_callback_notify))
        {
            PyErr_SetString(PyExc_TypeError, "paramentr must be callable");
            return NULL;
        }

    }
    Py_XINCREF(obj_callback_notify);
    Py_XDECREF(binding_callback_notify);
    binding_callback_notify = obj_callback_notify;
    void *arg_bytes = PyBytes_AsString(obj_arg_bytes);
    dap_chain_global_db_add_history_callback_notify(group_prefix, ((PyDapChainGlobalDBObject*)self)->callback_notify, arg_bytes);
    return PyLong_FromLong(0);
}

//PyObject *dap_chain_global_db_obj_get_py(PyObject *self,PyObject *args){
//    return NULL;
//}

PyObject *dap_chain_global_db_gr_get_py(PyObject *self,PyObject *args){
    (void)self;
    const char *key;
    PyObject *obj_data_out_list;
    const char *group;
    if (!PyArg_ParseTuple(args, "s|O|s", &key, &obj_data_out_list, &group))
        return NULL;
    Py_ssize_t data_out = PyList_Size(obj_data_out_list);
    size_t *size_arr = calloc((size_t)data_out, sizeof(size_t));
    for (Py_ssize_t i=0; i<data_out;i++){
        PyObject *obj_data = PyList_GetItem(obj_data_out_list, i);
        size_arr[(size_t)i] = PyLong_AsSize_t(obj_data);
    }
    uint8_t *uint8_res = dap_chain_global_db_gr_get(key, size_arr, group);
    char *res = (char *)((unsigned char *)uint8_res);
    return Py_BuildValue("s", res);
}

PyObject *dap_chain_global_db_gr_set_py(PyObject *self,PyObject *args){
    (void)self;
    char *key;
    PyObject *obj_value;
    char *group;
    if (!PyArg_ParseTuple(args, "s|O|s", &key, &obj_value, &group))
        return NULL;
    void *value = (void*)PyBytes_AsString(obj_value);
    size_t size = sizeof(value)/sizeof(value[0]);
    bool res = dap_chain_global_db_gr_set(key, value, size, group);
    if (res){
        return Py_BuildValue("O", Py_True);
    }else{
        return Py_BuildValue("O", Py_False);
    }
}

PyObject *dap_chain_global_db_gr_del_py(PyObject *self,PyObject *args){
    (void)self;
    char *key;
    char *group;
    if (!PyArg_ParseTuple(args, "s|s", &key, &group))
        return NULL;
    bool res = dap_chain_global_db_gr_del(key, group);
    if (res){
        return Py_BuildValue("O", Py_True);
    }else{
        return Py_BuildValue("O", Py_False);
    }
}

PyObject *dap_chain_global_db_get_history_group_by_group_name_py(PyObject *self,PyObject *args){
    (void)self;
    const char *group_name;
    if (!PyArg_ParseTuple(args, "s", &group_name))
        return NULL;
    char *result = dap_chain_global_db_get_history_group_by_group_name(group_name);
    return Py_BuildValue("(s)", result);
}

PyObject *dap_chain_global_db_add_history_extra_group_py(PyObject *self,PyObject *args){
    (void)self;
    const char *group_name;
//    PyObject *obj_node_addr;
//    size_t count_nodes;
    if (!PyArg_ParseTuple(args, "s", &group_name))
        return NULL;
    const char *result = dap_chain_global_db_add_history_extra_group(group_name, NULL, NULL);
    return Py_BuildValue("(s)", result);
}
PyObject *dap_chain_global_db_obj_gr_get_py(PyObject *self,PyObject *args){
    (void)self;
    const char *key, *group;
    Py_ssize_t data_len_out;
    if (!PyArg_ParseTuple(args, "sns", &key, &data_len_out, &group))
        return NULL;
    PyObject *obj_store = _PyObject_New(&DapStoreObject_DapStoreType);
    ((PyDapStoreObj*)obj_store)->store_obj = dap_chain_global_db_obj_gr_get(key, (size_t*)data_len_out, group);
    return Py_BuildValue("(O)", obj_store);
}
PyObject *dap_chain_global_db_get_py(PyObject *self,PyObject *args){
    return NULL;
}
