#include "wrapping_dap_chain_cell.h"

void DapChainCellObject_delete(PyDapChainCellObject* object){
    dap_chain_cell_delete(object->cell);
    Py_TYPE(object)->tp_free((PyObject*)object);
}
PyObject *dap_chain_cell_create_fill_py(PyTypeObject *type, PyObject *args, PyObject *kwds){
    PyObject *obj_chain, *obj_two = NULL;
    if (!PyArg_ParseTuple(args, "OO", &obj_chain, &obj_two)){
        PyErr_SetString(PyExc_ValueError, "This function must accept two objects Chain and CellID or string");
        return NULL;
    }
    if (!DapChainObject_check(obj_chain)){
        PyErr_SetString(PyExc_ValueError, "The first argument is of the wrong for this function. "
                                          "The first argument to the function must be an object of the Chain class");
        return NULL;
    }
    PyObject *obj = PyType_GenericNew(type, args, kwds);
    if (!DapChainCellIdObject_Check(obj_two)){
        Py_TYPE(obj)->tp_free((PyObject*)obj);
        PyErr_SetString(PyExc_ValueError, "The second argument is of the wrong for this function. "
                                          "The second argument to the function must be an object of the CellID class or a string");
        return NULL;
    } else{
        ((PyDapChainCellObject*)obj)->cell = dap_chain_cell_create_fill(((PyDapChainObject*)obj_chain)->chain_t,
                                                                        ((PyDapChainCellIDObject*)obj_two)->cell_id);
    }
    if (!PyUnicode_Check(obj_two)){
        Py_TYPE(obj)->tp_free((PyObject*)obj);
        PyErr_SetString(PyExc_ValueError, "The second argument is of the wrong for this function. "
                                          "The second argument to the function must be an object of the CellID class or a string");
        return NULL;
    } else {
        const char *l_filename = PyUnicode_AsUTF8(obj_two);
        ((PyDapChainCellObject*)obj)->cell = dap_chain_cell_create_fill2(((PyDapChainObject*)obj_chain)->chain_t, l_filename);
    }
    return obj;
}

PyObject *dap_chain_cell_load_py(PyObject *self, PyObject *args){
    PyObject *obj_chain;
    const char *cell_file_path;
    if (!PyArg_ParseTuple(args, "O|s", &obj_chain, &cell_file_path))
        return NULL;
    int res = dap_chain_cell_load(((PyDapChainObject*)obj_chain)->chain_t, cell_file_path);
    return PyLong_FromLong(res);
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
