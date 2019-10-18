#include "wrapping_dap_chain_cell.h"

void DapChainCellObject_delete(PyDapChainCellObject* object){
    dap_chain_cell_delete(object->cell);
    Py_TYPE(object)->tp_free((PyObject*)object);
}
PyObject *DapChainCellObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapChainCellObject *obj = (PyDapChainCellObject*)PyType_GenericNew(type_object, args, kwds);
    obj->cell = dap_chain_cell_create();
    return (PyObject *)obj;
}

PyObject *dap_chain_cell_load_py(PyObject *self, PyObject *args){
    //TODO
    return NULL;
}
PyObject *dap_chain_cell_file_update_py(PyObject *self, PyObject *args){
    int res = dap_chain_cell_file_update(((PyDapChainCellObject*)self)->cell);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_cell_file_append_py(PyObject *self, PyObject *args){
    PyObject *atom_bytes;
    size_t atom_size;
    if (!PyArg_ParseTuple(args, "S|n", &atom_bytes, &atom_size))
        return NULL;
    void *atom = PyBytes_AsString(atom_bytes);
    int res = dap_chain_cell_file_append(((PyDapChainCellObject*)self)->cell, atom, atom_size);
    return PyLong_FromLong(res);
}
