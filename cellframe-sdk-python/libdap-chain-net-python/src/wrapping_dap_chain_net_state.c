#include "wrapping_dap_chain_net_state.h"

PyObject *NET_STATE_OFFLINE_PY(void){
    PyObject *obj = _PyObject_New(&DapChainNetStateObject_DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_OFFLINE;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_LINKS_PREPARE_PY(void){
    PyObject *obj = _PyObject_New(&DapChainNetStateObject_DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_LINKS_PREPARE;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_LINKS_CONNECTING_PY(void){
    PyObject *obj = _PyObject_New(&DapChainNetStateObject_DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_LINKS_CONNECTING;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_LINKS_ESTABLISHED_PY(void){
    PyObject *obj = _PyObject_New(&DapChainNetStateObject_DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_LINKS_ESTABLISHED;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_ADDR_REQUEST_PY(void){ // Waiting for address assign
    PyObject *obj = _PyObject_New(&DapChainNetStateObject_DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_ADDR_REQUEST;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_ONLINE_PY(void){
    PyObject *obj = _PyObject_New(&DapChainNetStateObject_DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_ONLINE;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_SYNC_GDB_PY(void){
    PyObject *obj = _PyObject_New(&DapChainNetStateObject_DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_SYNC_GDB;
    return Py_BuildValue("O", obj);
}
PyObject *NET_STATE_SYNC_CHAINS_PY(void){
    PyObject *obj = _PyObject_New(&DapChainNetStateObject_DapChainNetStateObjectType);
    ((PyDapChainNetStateObject*)obj)->state = NET_STATE_SYNC_CHAINS;
    return Py_BuildValue("O", obj);
}
