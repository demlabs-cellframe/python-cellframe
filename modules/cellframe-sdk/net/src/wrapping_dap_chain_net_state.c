#include "wrapping_dap_chain_net_state.h"

PyMethodDef PyDapChainNetStateMethods[] = {
        {"NET_STATE_OFFLINE", NET_STATE_OFFLINE_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_LINKS_PREPARE", NET_STATE_LINKS_PREPARE_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_LINKS_CONNECTING", NET_STATE_LINKS_CONNECTING_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_LINKS_ESTABLISHED", NET_STATE_LINKS_ESTABLISHED_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_ADDR_REQUEST", NET_STATE_ADDR_REQUEST_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_SYNC_GDB", NET_STATE_SYNC_GDB_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_SYNC_CHAINS", NET_STATE_SYNC_CHAINS_PY, METH_NOARGS | METH_STATIC, ""},
        /*{"csAdd", (PyCFunction)dap_chain_cs_add_py, METH_VARARGS, ""},
        {"csCreate", (PyCFunction)dap_chain_cs_create_py, METH_VARARGS, ""},
        {"classAdd", (PyCFunction)dap_chain_class_add_py, METH_VARARGS, ""},
        {"classCreate", (PyCFunction)dap_chain_class_create_py, METH_VARARGS, ""},*/
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainNetStateObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNetState",        /* tp_name */
        sizeof(PyDapChainNetStateObject), /* tp_basicsize */
        0,                                /* tp_itemsize */
        0,                                /* tp_dealloc */
        0,                                /* tp_print */
        0,                                /* tp_getattr */
        0,                                /* tp_setattr */
        0,                                /* tp_reserved */
        0,                                /* tp_repr */
        0,                                /* tp_as_number */
        0,                                /* tp_as_sequence */
        0,                                /* tp_as_mapping */
        0,                                /* tp_hash  */
        0,                                /* tp_call */
        0,                                /* tp_str */
        0,                                /* tp_getattro */
        0,                                /* tp_setattro */
        0,                                /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,          /* tp_flags */
        "Chain net staties object",               /* tp_doc */
        0,		                          /* tp_traverse */
        0,		                          /* tp_clear */
        0,		                          /* tp_richcompare */
        0,                                /* tp_weaklistoffset */
        0,		                          /* tp_iter */
        0,		                          /* tp_iternext */
        PyDapChainNetStateMethods,        /* tp_methods */
        0,                                /* tp_members */
        0,                                /* tp_getset */
        0,                                /* tp_base */
        0,                                /* tp_dict */
        0,                                /* tp_descr_get */
        0,                                /* tp_descr_set */
        0,                                /* tp_dictoffset */
        0,                                /* tp_init */
        0,                                /* tp_alloc */
        PyType_GenericNew,                /* tp_new */
};

PyObject *NET_STATE_OFFLINE_PY(PyObject *self, PyObject *args){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_OFFLINE;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_LINKS_PREPARE_PY(PyObject *self, PyObject *args){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_LINKS_PREPARE;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_LINKS_CONNECTING_PY(PyObject *self, PyObject *args){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_LINKS_CONNECTING;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_LINKS_ESTABLISHED_PY(PyObject *self, PyObject *args){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_LINKS_ESTABLISHED;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_ADDR_REQUEST_PY(PyObject *self, PyObject *args){ // Waiting for address assign
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_ADDR_REQUEST;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_ONLINE_PY(PyObject *self, PyObject *args){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_ONLINE;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_SYNC_GDB_PY(PyObject *self, PyObject *args){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_SYNC_GDB;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_SYNC_CHAINS_PY(PyObject *self, PyObject *args){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_SYNC_CHAINS;
    return Py_BuildValue("O", obj);
}
