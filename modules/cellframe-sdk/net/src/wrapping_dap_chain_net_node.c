#include "wrapping_dap_chain_net_node.h"

PyMethodDef DapChainNetNodeMethods[] = {
        {"genAddr", dap_chain_node_gen_addr_py, METH_VARARGS | METH_STATIC, ""},
        {"checkAddr", dap_chain_node_check_addr_py, METH_VARARGS | METH_STATIC, ""},
        {"aliasFind", dap_chain_node_alias_find_py, METH_VARARGS | METH_STATIC, ""},
        {"aliasRegister", dap_chain_node_alias_register_py, METH_VARARGS | METH_STATIC, ""},
        {"aliasDelete", dap_chain_node_alias_delete_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainNodeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainNode",            /* tp_name */
        sizeof(PyDapChainNodeObject),     /* tp_basicsize */
        0,                               /* tp_itemsize */
        0,                               /* tp_dealloc */
        0,                               /* tp_print */
        0,                               /* tp_getattr */
        0,                               /* tp_setattr */
        0,                               /* tp_reserved */
        0,                               /* tp_repr */
        0,                               /* tp_as_number */
        0,                               /* tp_as_sequence */
        0,                               /* tp_as_mapping */
        0,                               /* tp_hash  */
        0,                               /* tp_call */
        0,                               /* tp_str */
        0,                               /* tp_getattro */
        0,                               /* tp_setattro */
        0,                               /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
        "Chain net node object",              /* tp_doc */
        0,		                         /* tp_traverse */
        0,		                         /* tp_clear */
        0,		                         /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        0,		                         /* tp_iter */
        0,		                         /* tp_iternext */
        DapChainNetNodeMethods,              /* tp_methods */
        0,                               /* tp_members */
        0,                               /* tp_getset */
        0,                               /* tp_base */
        0,                               /* tp_dict */
        0,                               /* tp_descr_get */
        0,                               /* tp_descr_set */
        0,                               /* tp_dictoffset */
        0,                               /* tp_init */
        0,                               /* tp_alloc */
        PyType_GenericNew,               /* tp_new */
};

PyObject *dap_chain_node_gen_addr_py(PyObject *self, PyObject *args){
    PyObject *obj_net;
    PyObject *obj_cell_id;
    if (!PyArg_ParseTuple(args, "O|O", &obj_net, &obj_cell_id))
        return NULL;
    PyObject *obj_node_addr = _PyObject_New(&DapChainNodeAddrObjectType);
    ((PyDapChainNodeAddrObject*)obj_node_addr)->node_addr = dap_chain_node_gen_addr(((PyDapChainNetObject*)obj_net)->chain_net->pub.id);
    return Py_BuildValue("O", obj_node_addr);
}
PyObject *dap_chain_node_check_addr_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_net;
    PyObject *obj_node_addr;
    PyObject *obj_cell_id;
    if (!PyArg_ParseTuple(args, "O|O|O", &obj_chain_net, &obj_node_addr, &obj_cell_id))
        return NULL;
    bool ret = dap_chain_node_check_addr(((PyDapChainNetObject*)obj_chain_net)->chain_net, ((PyDapChainNodeAddrObject*)obj_node_addr)->node_addr);
    if (ret)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}
PyObject *dap_chain_node_alias_find_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_net;
    const char *alias;
    if (!PyArg_ParseTuple(args, "O|s", &obj_chain_net, &alias))
        return NULL;
    PyObject *obj_node_addr = _PyObject_New(&DapChainNodeAddrObjectType);
    ((PyDapChainNodeAddrObject*)obj_node_addr)->node_addr = dap_chain_node_alias_find(((PyDapChainNetObject*)obj_node_addr)->chain_net, alias);
    return Py_BuildValue("O", &obj_node_addr);
}
PyObject *dap_chain_node_alias_register_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_net;
    const char *alias;
    PyObject *obj_node_addr;
    if (!PyArg_ParseTuple(args, "O|s|O", &obj_chain_net, &alias, &obj_node_addr))
        return NULL;
    bool ret = dap_chain_node_alias_register(((PyDapChainNetObject*)obj_chain_net)->chain_net, alias, ((PyDapChainNodeAddrObject*)obj_node_addr)->node_addr);
    if (ret)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}
PyObject *dap_chain_node_alias_delete_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_net;
    const char *alias;
    if (!PyArg_ParseTuple(args, "O|s", &obj_chain_net, &alias))
        return NULL;
    bool ret = dap_chain_node_alias_delete(((PyDapChainNetObject*)obj_chain_net)->chain_net, alias);
    if (ret)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}
