#include "wrapping_dap_chain_net_srv_client.h"

#define LOG_TAG "wrapping_dap_chain_net_srv_client"

void _wrapping_dap_chain_net_srv_client_callback_connected(dap_chain_net_srv_client_t* a_client, void *a_arg){
    if(PyCallable_Check(a_arg)) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject *l_call = (PyObject*)a_arg;
        PyDapChainNetSrvClientObject *client = PyObject_New(PyDapChainNetSrvClientObject,
                                                            &DapChainNetSrvClientObject_DapChainNetSrvClientObjectType);
        PyObject_Dir((PyObject *) client);
        client->srv_client = a_client;
        PyObject *l_args = Py_BuildValue("(O)", client);
        Py_INCREF(l_args);
        Py_INCREF(l_call);
        PyEval_CallObject(l_call, l_args);
        Py_XDECREF(l_args);
        Py_XDECREF(l_call);
        PyGILState_Release(state);
    }else{
        log_it(L_ERROR, "Can't called handler Python in callbacks");
    }
}
void _wrapping_dap_chain_net_srv_client_callback_disconnected(dap_chain_net_srv_client_t* a_client, void *a_arg){}
void _wrapping_dap_chain_net_srv_client_callback_deleted(dap_chain_net_srv_client_t* a_client, void *a_arg){}
void _wrapping_dap_chain_net_srv_client_callback_pkt_it(dap_chain_net_srv_client_t* a_client, void *a_arg){}

int PyDapChainNetSrvClient_init(PyDapChainNetSrvClientObject* self, PyObject *args, PyObject *kwds){
    const char *kwlist[] = {
            "net",
            "addr",
            "port",
            "handler",
            NULL
    };
    PyObject *obj_net, *obj_hander;
    const char *addr;
    uint16_t port;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OsH", kwlist, &obj_net, &addr, &port, &obj_hander)){
        return -1;
    }
    if (!PyCallable_Check(obj_hander)){
        return -1;
    }
    dap_chain_net_srv_client_callbacks_t callbacks = {0};
    callbacks.connected = _wrapping_dap_chain_net_srv_client_callback_connected;
    callbacks.disconnected = _wrapping_dap_chain_net_srv_client_callback_disconnected;
    callbacks.deleted = _wrapping_dap_chain_net_srv_client_callback_deleted;
//    callbacks.pkt_in = _w
    dap_chain_net_srv_client_create_n_connect(((PyDapChainNetObject*)obj_net)->chain_net, addr, port, &callbacks, obj_hander);
}
PyObject *wrapping_dap_chain_net_srv_client_write(PyObject *self, PyObject *args){
    return Py_None;
}