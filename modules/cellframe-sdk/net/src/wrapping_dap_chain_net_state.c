#include "libdap-python.h"
#include "wrapping_dap_chain_net_state.h"

static PyMethodDef PyDapChainNetStateMethods[] = {
        {"NET_STATE_OFFLINE", (PyCFunction)NET_STATE_OFFLINE_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_LINKS_PREPARE", (PyCFunction)NET_STATE_LINKS_PREPARE_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_LINKS_CONNECTING", (PyCFunction)NET_STATE_LINKS_CONNECTING_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_LINKS_ESTABLISHED", (PyCFunction)NET_STATE_LINKS_ESTABLISHED_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_ADDR_REQUEST", (PyCFunction)NET_STATE_ADDR_REQUEST_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_SYNC_GDB", (PyCFunction)NET_STATE_SYNC_GDB_PY, METH_NOARGS | METH_STATIC, ""},
        {"NET_STATE_SYNC_CHAINS", (PyCFunction)NET_STATE_SYNC_CHAINS_PY, METH_NOARGS | METH_STATIC, ""},
        /*{"csAdd", (PyCFunction)dap_chain_cs_add_py, METH_VARARGS, ""},
        {"csCreate", (PyCFunction)dap_chain_cs_create_py, METH_VARARGS, ""},
        {"classAdd", (PyCFunction)dap_chain_class_add_py, METH_VARARGS, ""},
        {"classCreate", (PyCFunction)dap_chain_class_create_py, METH_VARARGS, ""},*/
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainNetStateObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainNetState", sizeof(PyDapChainNetStateObject),
        "Chain net staties object",
        .tp_methods = PyDapChainNetStateMethods);

PyObject *NET_STATE_OFFLINE_PY(){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_OFFLINE;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_LINKS_PREPARE_PY(){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_LINKS_PREPARE;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_LINKS_CONNECTING_PY(){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_LINKS_CONNECTING;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_LINKS_ESTABLISHED_PY(){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_LINKS_ESTABLISHED;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_ADDR_REQUEST_PY(){ // Waiting for address assign
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_ADDR_REQUEST;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_ONLINE_PY(){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_ONLINE;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_SYNC_GDB_PY(){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_SYNC_GDB;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_SYNC_CHAINS_PY(){
    PyObject *obj = _PyObject_New(&DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_SYNC_CHAINS;
    return Py_BuildValue("O", obj);
}
