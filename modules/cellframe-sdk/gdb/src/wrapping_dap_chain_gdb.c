#include "wrapping_dap_chain_gdb.h"

int dap_chain_gdb_init_py(void){
    return dap_chain_gdb_init();
}

PyMethodDef DapChainGDBMethod[] = {
        {"new", (PyCFunction)dap_chain_gdb_new_py, METH_VARARGS | METH_STATIC, ""},
        {"delete", (PyCFunction)dap_chain_gdb_delete_py, METH_VARARGS | METH_STATIC, ""},
        {"getGroup", (PyCFunction)dap_chain_gdb_get_group_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainGdbObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainGDB",                                            /* tp_name */
        sizeof(PyDapChainGDBObject),                                     /* tp_basicsize */
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
        "Chain gdb object",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        DapChainGDBMethod,                                              /* tp_methods */
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

PyObject* dap_chain_gdb_new_py(PyObject* self, PyObject *args){
    PyObject *obj_chain;
    if (!PyArg_ParseTuple(args, "O", &obj_chain))
        return NULL;
    int res = dap_chain_gdb_new(((PyDapChainObject*)obj_chain)->chain_t, g_config);
    return PyLong_FromLong(res);
}

PyObject *dap_chain_gdb_delete_py(PyObject *self, PyObject *args){
    PyObject *obj_chain;
    if (!PyArg_ParseTuple(args, "O", &obj_chain))
        return NULL;
    dap_chain_gdb_delete(((PyDapChainObject*)obj_chain)->chain_t);
    return PyLong_FromLong(0);
}

PyObject *dap_chain_gdb_get_group_py(PyObject *self, PyObject *args){
    PyObject *obj_chain;
    if (!PyArg_ParseTuple(args, "O", &obj_chain))
        return NULL;
    const char *res = dap_chain_gdb_get_group(((PyDapChainObject*)obj_chain)->chain_t);
    return Py_BuildValue("s", res);
}
