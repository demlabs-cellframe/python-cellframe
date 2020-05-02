#include "wrapping_dap_chain_net_node.h"

PyObject *dap_chain_node_gen_addr_py(PyObject *self, PyObject *args){
    PyObject *obj_net;
    PyObject *obj_cell_id;
    if (!PyArg_ParseTuple(args, "O|O", &obj_net, &obj_cell_id))
        return NULL;
    PyObject *obj_node_addr = _PyObject_New(&DapChainNodeAddrObject_DapChainNodeAddrObjectType);
    ((PyDapChainNodeAddrObject*)obj_node_addr)->node_addr = dap_chain_node_gen_addr(((PyDapChainNetObject*)obj_net)->chain_net, &(((PyDapChainCellIDObject*)obj_cell_id)->cell_id));
    return Py_BuildValue("O", obj_node_addr);
}
PyObject *dap_chain_node_check_addr_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_net;
    PyObject *obj_node_addr;
    PyObject *obj_cell_id;
    if (!PyArg_ParseTuple(args, "O|O|O", &obj_chain_net, &obj_node_addr, &obj_cell_id))
        return NULL;
    bool ret = dap_chain_node_check_addr(((PyDapChainNetObject*)obj_chain_net)->chain_net, ((PyDapChainNodeAddrObject*)obj_node_addr)->node_addr, &(((PyDapChainCellIDObject*)obj_cell_id)->cell_id));
    if (ret)
        return Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);
}
PyObject *dap_chain_node_alias_find_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_net;
    const char *alias;
    if (!PyArg_ParseTuple(args, "O|s", &obj_chain_net, &alias))
        return NULL;
    PyObject *obj_node_addr = _PyObject_New(&DapChainNodeAddrObject_DapChainNodeAddrObjectType);
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
        return Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);
}
PyObject *dap_chain_node_alias_delete_py(PyObject *self, PyObject *args){
    PyObject *obj_chain_net;
    const char *alias;
    if (!PyArg_ParseTuple(args, "O|s", &obj_chain_net, &alias))
        return NULL;
    bool ret = dap_chain_node_alias_delete(((PyDapChainNetObject*)obj_chain_net)->chain_net, alias);
    if (ret)
        return Py_BuildValue("O", Py_True);
    else
        return Py_BuildValue("O", Py_False);
}
