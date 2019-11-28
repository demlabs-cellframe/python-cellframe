#ifndef _WRAPPING_DAP_CHAIN_NET_NODE_CLI_
#define _WRAPPING_DAP_CHAIN_NET_NODE_CLI_

#include <Python.h>
#include "dap_config.h"
#include "dap_chain_node_cli.h"
#include "dap_chain_node_cli_cmd.h"
#include "wrapping_dap_chain_common_objects.h"
#include "wrapping_dap_chain_net_node.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapChainNodeCli{
    PyObject_HEAD
    cmdfunc_t *func;
}PyDapChainNodeCliObject;
static PyObject *binded_object_cmdfunc = NULL;

int dap_chain_node_cli_init_py(dap_config_t *g_config);
void dap_chain_node_cli_delete_py(void);

PyObject *DapChainNodeCliObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds);

PyObject *dap_chain_node_cli_cmd_item_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_node_cli_set_reply_text_py(PyObject *self, PyObject *args);

PyObject *dap_chain_node_addr_get_by_alias_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainNodeCliMethods[] = {
    {"cmdItemCreate", dap_chain_node_cli_cmd_item_create_py, METH_VARARGS, ""},
    {"setReplyText", dap_chain_node_cli_set_reply_text_py, METH_VARARGS, ""},
    {"getByAlias", dap_chain_node_addr_get_by_alias_py, METH_VARARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainNodeCliObject_DapChainNodeCliObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.Node.cli",            /* tp_name */
    sizeof(PyDapChainNodeCliObject),     /* tp_basicsize */
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
    "Chain net node cli object",              /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    DapChainNodeCliMethods,              /* tp_methods */
    0,                               /* tp_members */
    0,                               /* tp_getset */
    0,                               /* tp_base */
    0,                               /* tp_dict */
    0,                               /* tp_descr_get */
    0,                               /* tp_descr_set */
    0,                               /* tp_dictoffset */
    0,                               /* tp_init */
    0,                               /* tp_alloc */
    DapChainNodeCliObject_new,       /* tp_new */
};

char **PyListToString(PyObject *list);
PyObject *stringToPyList(char **list);

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_NET_NODE_CLI_
