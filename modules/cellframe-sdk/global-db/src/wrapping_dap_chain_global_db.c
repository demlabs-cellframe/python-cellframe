#include "wrapping_dap_chain_global_db.h"

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
    const char *l_key;
    const char *l_group;
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
    bool ret = dap_chain_global_db_gr_del(l_key, l_group);
    if (ret == true)
        return Py_True;
    else
        return Py_False;
}

PyObject *wrapping_dap_chain_global_db_gr_pin(PyObject *self, PyObject *args){
    return Py_None;
}
