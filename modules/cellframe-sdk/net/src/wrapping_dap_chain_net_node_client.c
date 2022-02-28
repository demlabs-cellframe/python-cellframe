#include "wrapping_dap_chain_net_node_client.h"

int dap_chain_node_client_init_py(void){
    return dap_chain_node_client_init();
}
void dap_chain_node_client_deinit_py(void){
    dap_chain_node_client_deinit();
}

PyObject *dap_chain_client_connect_py(PyObject *self, PyObject *args){
    PyObject *obj_net;
    PyObject *obj_node_info;
    const char *active_channels;
    if (!PyArg_ParseTuple(args, "O|O|s",&obj_net, &obj_node_info, &active_channels))
        return NULL;
    PyObject *obj_node_client = _PyObject_New(&DapChainNodeClientObject_DapChainNodeClientObjectType);
    ((PyDapChainNodeClientObject*)obj_node_client)->node_client =dap_chain_node_client_connect_channels(
                ((PyDapChainNetObject*) obj_net)->chain_net,
                ((PyDapChainNodeInfoObject*)obj_node_info)->node_info, active_channels);
    return Py_BuildValue("O", obj_node_client);
}

PyObject *dap_chain_node_client_connect_py(PyObject *self, PyObject *args){
    if (self != NULL){
        PyErr_SetString(PyExc_SyntaxWarning, "method must be called statically");
        return NULL;
    }
    PyObject *obj_net;
    PyObject *obj_node_info;
    if (!PyArg_ParseTuple(args, "O|O",&obj_net, &obj_node_info))
        return NULL;
    PyObject *obj_node_client = _PyObject_New(&DapChainNodeClientObject_DapChainNodeClientObjectType);
    ((PyDapChainNodeClientObject*)obj_node_client)->node_client = dap_chain_node_client_connect(
                ((PyDapChainNetObject*) obj_net)->chain_net,
                ((PyDapChainNodeInfoObject*)obj_node_info)->node_info);
    return Py_BuildValue("O", obj_node_client);
}
PyObject *dap_chain_node_client_close_py(PyObject *self, PyObject *args){
    dap_chain_node_client_close(((PyDapChainNodeClientObject*)self)->node_client);
    return PyLong_FromLong(0);
}
PyObject *dap_chain_node_client_send_ch_pkt_py(PyObject *self, PyObject *args){
    uint8_t ch_id;
    uint8_t type;
    PyObject *obj_buf;
    void *buf;
    size_t buf_size;
    if (!PyArg_ParseTuple(args, "b|b|O", &ch_id, &type, &obj_buf))
        return NULL;
    buf = PyBytes_AsString(obj_buf);
    buf_size = (size_t)PyBytes_Size(buf);
    int res = dap_chain_node_client_send_ch_pkt(((PyDapChainNodeClientObject*)self)->node_client, ch_id, type, buf, buf_size);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_node_client_wait_py(PyObject *self, PyObject *args){
    int waited_state;
    int timeout_ms;
    if (!PyArg_ParseTuple(args, "i|i", &waited_state, &timeout_ms))
        return NULL;
    int res = dap_chain_node_client_wait(((PyDapChainNodeClientObject*)self)->node_client, waited_state, timeout_ms);
    return PyLong_FromLong(res);
}
PyObject *dap_chain_node_client_set_callbacks_py(PyObject *self, PyObject *args){
    PyObject *obj_dap_client;
    uint8_t ch_id;
    if (!PyArg_ParseTuple(args, "O|b", &obj_dap_client, &ch_id))
        return NULL;
    int res = dap_chain_node_client_set_callbacks(((PyDapClientObject*)obj_dap_client)->client, ch_id);
    return PyLong_FromLong(res);
}
