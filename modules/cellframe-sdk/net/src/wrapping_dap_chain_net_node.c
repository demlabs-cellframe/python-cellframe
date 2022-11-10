#include "wrapping_dap_chain_net_node.h"

static PyMethodDef DapChainNetNodeMethods[] = {
        {"genAddr", dap_chain_node_gen_addr_py, METH_VARARGS | METH_STATIC, ""},
        {"checkAddr", dap_chain_node_check_addr_py, METH_VARARGS | METH_STATIC, ""},
        {"aliasFind", dap_chain_node_alias_find_py, METH_VARARGS | METH_STATIC, ""},
        {"aliasRegister", dap_chain_node_alias_register_py, METH_VARARGS | METH_STATIC, ""},
        {"aliasDelete", dap_chain_node_alias_delete_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainNodeObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainNode", sizeof(PyDapChainNodeObject),
        "Chain net node object",
        .tp_methods = DapChainNetNodeMethods);

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
