#include "wrapping_dap_chain_net_srv_datum.h"

PyObject *wrapping_dap_chain_net_srv_datum_read(PyObject *self, PyObject *args){
    (void)self;
    char *str_path;
    if (!PyArg_ParseTuple(args, "s", &str_path)){
        return NULL;
    }
    size_t l_size_data = 0;
    void* l_data = dap_chain_net_srv_file_datum_data_read(str_path, &l_size_data);
    PyObject *obj_bytes = PyBytes_FromStringAndSize(l_data, l_size_data);
    return obj_bytes;
}

PyObject *wrapping_dap_chain_net_srv_datum_create(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_chain;
    PyObject *obj_data_in_datum;
    if (!PyArg_ParseTuple(args, "OO", &obj_chain, &obj_data_in_datum)){
        return NULL;
    }
    if (!PyBytes_Check(obj_data_in_datum)){
        PyErr_SetString(PyExc_ValueError, "This function takes an object of type bytes as its second argument.");
        return NULL;
    }
    if (!PyDapChain_Check(obj_chain)){
        PyErr_SetString(PyExc_ValueError, "The first argument to this function must be an object of type Chain.");
        return NULL;
    }
    void *l_data = PyBytes_AsString(obj_data_in_datum);
    size_t l_data_size = PyBytes_Size(obj_data_in_datum);
    char* res = dap_chain_net_srv_datum_custom_add(((PyDapChainObject*)obj_chain)->chain_t, l_data, l_data_size);
    if (res == NULL)
        return Py_None;
    return Py_BuildValue("s", res);
}
