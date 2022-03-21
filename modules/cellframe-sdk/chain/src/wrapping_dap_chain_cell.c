#include "wrapping_dap_chain_cell.h"

/* Dap chain cell */

PyMethodDef PyDapChainCellObjectMethods[] ={
        {"load", dap_chain_cell_load_py, METH_VARARGS | METH_STATIC, ""},
        {"update", dap_chain_cell_file_update_py, METH_VARARGS, ""},
        {"append", dap_chain_cell_file_append_py, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainCellObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainCell",       /* tp_name */
        sizeof(PyDapChainCellObject),      /* tp_basicsize */
        0,                                       /* tp_itemsize */
        0,                                       /* tp_dealloc */
        0,                                       /* tp_print */
        0,                                       /* tp_getattr */
        0,                                       /* tp_setattr */
        0,                                       /* tp_reserved */
        0,                                       /* tp_repr */
        0,                                       /* tp_as_number */
        0,                                       /* tp_as_sequence */
        0,                                       /* tp_as_mapping */
        0,                                       /* tp_hash  */
        0,                                       /* tp_call */
        0,                                       /* tp_str */
        0,                                       /* tp_getattro */
        0,                                       /* tp_setattro */
        0,                                       /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                 /* tp_flags */
        "Chain cell object",                     /* tp_doc */
        0,		                                 /* tp_traverse */
        0,		                                 /* tp_clear */
        0,		                                 /* tp_richcompare */
        0,                                       /* tp_weaklistoffset */
        0,		                                 /* tp_iter */
        0,		                                 /* tp_iternext */
        PyDapChainCellObjectMethods,             /* tp_methods */
        0,                                       /* tp_members */
        0,                                       /* tp_getset */
        0,                                       /* tp_base */
        0,                                       /* tp_dict */
        0,                                       /* tp_descr_get */
        0,                                       /* tp_descr_set */
        0,                                       /* tp_dictoffset */
        0,                                       /* tp_init */
        0,                                       /* tp_alloc */
        PyType_GenericNew,                       /* tp_new */
};

void DapChainCellObject_delete(PyDapChainCellObject* object){
    dap_chain_cell_delete(object->cell);
    Py_TYPE(object)->tp_free((PyObject*)object);
}

/*PyObject *DapChainCellObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds){
    PyDapChainCellObject *obj = (PyDapChainCellObject*)PyType_GenericNew(type_object, args, kwds);
    obj->cell = dap_chain_cell_create();
    return (PyObject *)obj;
}*/

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

/* Dap chain cell decl req */

PyTypeObject DapChainCellDeclReq_DapChainCellDeclReqObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.DapChainCellDeclReq",         /* tp_name */
        sizeof(PyDapChainCellDeclReqObject),     /* tp_basicsize */
        0,                                       /* tp_itemsize */
        0,                                       /* tp_dealloc */
        0,                                       /* tp_print */
        0,                                       /* tp_getattr */
        0,                                       /* tp_setattr */
        0,                                       /* tp_reserved */
        0,                                       /* tp_repr */
        0,                                       /* tp_as_number */
        0,                                       /* tp_as_sequence */
        0,                                       /* tp_as_mapping */
        0,                                       /* tp_hash  */
        0,                                       /* tp_call */
        0,                                       /* tp_str */
        0,                                       /* tp_getattro */
        0,                                       /* tp_setattro */
        0,                                       /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                 /* tp_flags */
        "Chain cell decl req object",                     /* tp_doc */
        0,		                                 /* tp_traverse */
        0,		                                 /* tp_clear */
        0,		                                 /* tp_richcompare */
        0,                                       /* tp_weaklistoffset */
        0,		                                 /* tp_iter */
        0,		                                 /* tp_iternext */
        0,                                       /* tp_methods */
        0,                                       /* tp_members */
        0,                                       /* tp_getset */
        0,                                       /* tp_base */
        0,                                       /* tp_dict */
        0,                                       /* tp_descr_get */
        0,                                       /* tp_descr_set */
        0,                                       /* tp_dictoffset */
        0,                                       /* tp_init */
        0,                                       /* tp_alloc */
        PyType_GenericNew,                       /* tp_new */
};

/* Dap chain decl */

PyTypeObject DapChainDecl_DapChainDeclObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.DapChainCellDecl",       /* tp_name */
        sizeof(PyDapChainDeclObject),      /* tp_basicsize */
        0,                                       /* tp_itemsize */
        0,                                       /* tp_dealloc */
        0,                                       /* tp_print */
        0,                                       /* tp_getattr */
        0,                                       /* tp_setattr */
        0,                                       /* tp_reserved */
        0,                                       /* tp_repr */
        0,                                       /* tp_as_number */
        0,                                       /* tp_as_sequence */
        0,                                       /* tp_as_mapping */
        0,                                       /* tp_hash  */
        0,                                       /* tp_call */
        0,                                       /* tp_str */
        0,                                       /* tp_getattro */
        0,                                       /* tp_setattro */
        0,                                       /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                 /* tp_flags */
        "Chain cell decl object",                     /* tp_doc */
        0,		                                 /* tp_traverse */
        0,		                                 /* tp_clear */
        0,		                                 /* tp_richcompare */
        0,                                       /* tp_weaklistoffset */
        0,		                                 /* tp_iter */
        0,		                                 /* tp_iternext */
        0,                                       /* tp_methods */
        0,                                       /* tp_members */
        0,                                       /* tp_getset */
        0,                                       /* tp_base */
        0,                                       /* tp_dict */
        0,                                       /* tp_descr_get */
        0,                                       /* tp_descr_set */
        0,                                       /* tp_dictoffset */
        0,                                       /* tp_init */
        0,                                       /* tp_alloc */
        PyType_GenericNew,                       /* tp_new */
};
