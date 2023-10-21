#include "dap_global_db_remote.h"
#include "libdap-python.h"
#include "wrapping_dap_global_db.h"
#include "dap_events.h"
#include "dap_proc_thread.h"

static PyMethodDef DapChainGlobalDBMethods[] = {
        {"get", (PyCFunction)wrapping_dap_global_db_gr_get, METH_VARARGS | METH_STATIC, ""},
        {"set", (PyCFunction)wrapping_dap_global_db_gr_set, METH_VARARGS | METH_STATIC, ""},
        {"delete", (PyCFunction)wrapping_dap_global_db_gr_del, METH_VARARGS | METH_STATIC, ""},
        {"pin", (PyCFunction)wrapping_dap_global_db_gr_pin, METH_VARARGS | METH_STATIC, ""},
        {"unpin", (PyCFunction)wrapping_dap_global_db_gr_unpin, METH_VARARGS | METH_STATIC, ""},
        {"grLoad", (PyCFunction)wrapping_dap_global_db_gr_load, METH_VARARGS | METH_STATIC, ""},
        {"addSyncExtraGroup", (PyCFunction)wrapping_dap_global_db_add_sync_extra_group, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainGlobalDBObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainGlobalDB", sizeof(PyDapChainGlobalDBObject),
        "Chain GlobalDB object",
        .tp_methods = DapChainGlobalDBMethods);

PyObject *wrapping_dap_global_db_gr_get(PyObject *self, PyObject *args){
    (void)self;
    const char *l_key;
    const char *l_group;
    if (!PyArg_ParseTuple(args, "ss", &l_key, &l_group)){
        return NULL;
    }
    size_t l_size_data = 0;
    void *l_bytes = dap_global_db_get_sync(l_group, l_key, &l_size_data, NULL, NULL);
    if (l_size_data == 0)
        Py_RETURN_NONE;
    PyObject *l_obj_bytes = PyBytes_FromStringAndSize(l_bytes, (Py_ssize_t)l_size_data);
    DAP_DELETE(l_bytes);
    return l_obj_bytes;
}

/**
 * @brief wrapping_dap_global_db_gr_set
 * @param self
 * @param args
 * @return
 */
PyObject *wrapping_dap_global_db_gr_set(PyObject *self, PyObject *args){
    (void)self;
    char *l_key;
    char *l_group;
    bool l_is_pinned = true;
    PyObject *obj_byte;
    if (!PyArg_ParseTuple(args, "ssO|p", &l_key, &l_group, &obj_byte, &l_is_pinned)){
        return NULL;
    }
    if (!PyBytes_Check(obj_byte)){
        PyErr_SetString(PyExc_AttributeError, "In the set function of the globalDB object, the third "
                                              "argument must take an object of type bytes.");
        return NULL;
    }
    void *l_bytes = PyBytes_AsString(obj_byte);
    size_t l_bytes_size = PyBytes_Size(obj_byte);
    int ret = dap_global_db_set(l_group, l_key, l_bytes, l_bytes_size, l_is_pinned, NULL, NULL);
    if (ret == DAP_GLOBAL_DB_RC_SUCCESS) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

/**
 * @brief wrapping_dap_global_db_gr_del
 * @param self
 * @param args
 * @return
 */
PyObject *wrapping_dap_global_db_gr_del(PyObject *self, PyObject *args){
    (void)self;
    const char *l_key;
    const char *l_group;
    if (!PyArg_ParseTuple(args, "ss",&l_key, &l_group)){
        return NULL;
    }
    int ret = dap_global_db_del(l_group, l_key, NULL, NULL);
    if (ret == DAP_GLOBAL_DB_RC_SUCCESS)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject *wrapping_dap_global_db_gr_pin(PyObject *self, PyObject *args){
    (void)self;
    char *l_key;
    char *l_group;
    if (!PyArg_ParseTuple(args, "ss", &l_key, &l_group)){
        return NULL;
    }

    int ret = dap_global_db_pin(l_group, l_key, NULL, NULL);
    if (ret == DAP_GLOBAL_DB_RC_SUCCESS)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject *wrapping_dap_global_db_gr_unpin(PyObject *self, PyObject *args){
    (void)self;
    char *l_key;
    char *l_group;

    if (!PyArg_ParseTuple(args, "ss", &l_key, &l_group)){
        return NULL;
    }

    int ret = dap_global_db_unpin(l_group, l_key, NULL, NULL);
    if (ret == DAP_GLOBAL_DB_RC_SUCCESS)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}


PyObject *wrapping_dap_global_db_gr_load(PyObject *self, PyObject *args){
    (void)self;
    char *l_group;
    if (!PyArg_ParseTuple(args, "s", &l_group)){
        return NULL;
    }
    size_t l_data_out = 0;
    dap_global_db_obj_t *l_db_obj = dap_global_db_get_all_sync(l_group, &l_data_out);
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

    DAP_DELETE(l_db_obj); 
    
    return l_list;
}

typedef struct _wrapping_dap_global_db_add_sync_extra_group_callback{
    PyObject *func;
    PyObject *arg;
    dap_store_obj_t *store_obj;
}_wrapping_dap_global_db_add_sync_extra_group_callback_t;

bool dap_py_chain_net_extra_group_notifier(UNUSED_ARG dap_proc_thread_t *a_poc_thread, void *a_arg) {
    if (!a_arg)
        return false;

    _wrapping_dap_global_db_add_sync_extra_group_callback_t *l_callback = (_wrapping_dap_global_db_add_sync_extra_group_callback_t *)a_arg;
    PyGILState_STATE state = PyGILState_Ensure();
    char l_op_code[2];
    l_op_code[0] = l_callback->store_obj->type;
    l_op_code[1] = '\0';
    PyObject *l_obj_value = NULL;
    if (!l_callback->store_obj->value || !l_callback->store_obj->value_len)
        l_obj_value = Py_None;
    else
        l_obj_value = PyBytes_FromStringAndSize((char *)l_callback->store_obj->value, (Py_ssize_t)l_callback->store_obj->value_len);
    PyObject *argv = Py_BuildValue("sssOO", l_op_code, l_callback->store_obj->group, l_callback->store_obj->key, l_obj_value, l_callback->arg);
    Py_XINCREF(l_callback->func);
    Py_XINCREF(l_callback->arg);
    PyObject_CallObject(l_callback->func, argv);
    Py_DECREF(argv);
    Py_XDECREF(l_callback->func);
    Py_XDECREF(l_callback->arg);
    PyGILState_Release(state);
    dap_store_obj_free_one(l_callback->store_obj);
    return false;
}

void pvt_wrapping_dap_global_db_add_sync_extra_group_func_callback(dap_global_db_instance_t *a_dbi, dap_store_obj_t *a_obj, void *a_arg)
{
    if (!a_arg)
        return;
    _wrapping_dap_global_db_add_sync_extra_group_callback_t *l_obj = DAP_NEW(_wrapping_dap_global_db_add_sync_extra_group_callback_t);
    if (!l_obj) 
        return;
    l_obj->store_obj = dap_store_obj_copy(a_obj, 1);
    l_obj->func = ((_wrapping_dap_global_db_add_sync_extra_group_callback_t*)a_arg)->func;
    l_obj->arg = ((_wrapping_dap_global_db_add_sync_extra_group_callback_t*)a_arg)->arg;
    dap_proc_thread_callback_add(NULL, dap_py_chain_net_extra_group_notifier, l_obj);
}

PyObject *wrapping_dap_global_db_add_sync_extra_group(PyObject *self, PyObject *args){
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
    _wrapping_dap_global_db_add_sync_extra_group_callback_t *l_callback = DAP_NEW(_wrapping_dap_global_db_add_sync_extra_group_callback_t);
    l_callback->func = call_func;
    l_callback->arg = args_func;
    Py_INCREF(call_func);
    Py_INCREF(args_func);
    // TODO implement GlobalDB cluster wrapping
    //dap_global_db_add_sync_extra_group(net_name, group_mask, pvt_wrapping_dap_global_db_add_sync_extra_group_func_callback, l_callback);
    Py_RETURN_NONE;
}
