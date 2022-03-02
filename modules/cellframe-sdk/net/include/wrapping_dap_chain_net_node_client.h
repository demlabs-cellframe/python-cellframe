#ifndef _WRAPPING_DAP_CHAIN_NODE_CLIENT_
#define _WRAPPING_DAP_CHAIN_NODE_CLIENT_

#include <Python.h>
#include "dap_chain_node_client.h"
#include "wrapping_dap_chain_net_node_info.h"
#include "libdap_client_python.h"
#include "wrapping_dap_client_stage.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapChainNodeClient{
    PyObject_HEAD
    dap_chain_node_client_t *node_client;
}PyDapChainNodeClientObject;

int dap_chain_node_client_init_py(void);
void dap_chain_node_client_deinit_py(void);

PyObject *dap_chain_client_connect_py(PyObject *self, PyObject *args);
PyObject *dap_chain_node_client_connect_py(PyObject *self, PyObject *args);
PyObject *dap_chain_node_client_close_py(PyObject *self, PyObject *args);
PyObject *dap_chain_node_client_send_ch_pkt_py(PyObject *self, PyObject *args);
PyObject *dap_chain_node_client_wait_py(PyObject *self, PyObject *args);
PyObject *dap_chain_node_client_set_callbacks_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainNodeClientMethods[] = {
    {"clientConnect", dap_chain_client_connect_py, METH_VARARGS | METH_STATIC, ""},
    {"nodeClientConnect", (PyCFunction)dap_chain_node_client_connect_py, METH_VARARGS | METH_STATIC, ""},
    {"close", (PyCFunction)dap_chain_node_client_close_py, METH_VARARGS, ""},
    {"sendChPkt", (PyCFunction)dap_chain_node_client_send_ch_pkt_py, METH_VARARGS, ""},
    {"wait", (PyCFunction)dap_chain_node_client_wait_py, METH_VARARGS, ""},
    {"setCallbacks", (PyCFunction)dap_chain_node_client_set_callbacks_py, METH_VARARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainNodeClientObject_DapChainNodeClientObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainNodeClient",            /* tp_name */
    sizeof(PyDapChainNodeClientObject),     /* tp_basicsize */
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
    "Chain net node client object",              /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainNodeClientMethods,              /* tp_methods */
    0,                               /* tp_members */
    0,                               /* tp_getset */
    0,                               /* tp_base */
    0,                               /* tp_dict */
    0,                               /* tp_descr_get */
    0,                               /* tp_descr_set */
    0,                               /* tp_dictoffset */
    0,                               /* tp_init */
    0,                               /* tp_alloc */
    PyType_GenericNew,       /* tp_new */
};

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_NODE_CLIENT_
