#ifndef _WRAPPING_DAP_CHAIN_CS_
#define _WRAPPING_DAP_CHAIN_CS_

#include "Python.h"
#include "dap_chain_cs.h"
#include "libdap-chain-python.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapChainCs{
    PyObject_HEAD
    dap_chain_cs_t *cs;
    dap_chain_callback_new_cfg_t callback_new_cfg;
}PyDapChainCsObject;
static PyObject *binded_object_callback_new_cfg = NULL;

PyObject *DapChainCSObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds);

int dap_chain_cs_init_py(void);
void dap_chain_cs_deinit_py(void);

PyObject *dap_chain_cs_add_py (PyObject *self, PyObject *args);
PyObject *dap_chain_cs_create_py(PyObject *self, PyObject *args);

PyObject *dap_chain_class_add_py (PyObject *self, PyObject *args);
PyObject *dap_chain_class_create_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainTypeMethods[] = {
    {"csAdd", (PyCFunction)dap_chain_cs_add_py, METH_VARARGS, ""},
    {"csCreate", (PyCFunction)dap_chain_cs_create_py, METH_VARARGS, ""},
    {"classAdd", (PyCFunction)dap_chain_class_add_py, METH_VARARGS, ""},
    {"classCreate", (PyCFunction)dap_chain_class_create_py, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainCsObject_DapChainCsObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainCS",             /* tp_name */
    sizeof(PyDapChainCsObject),      /* tp_basicsize */
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
    "Chain id object",           /* tp_doc */
    0,		                         /* tp_traverse */
    0,		                         /* tp_clear */
    0,		                         /* tp_richcompare */
    0,                               /* tp_weaklistoffset */
    0,		                         /* tp_iter */
    0,		                         /* tp_iternext */
    0,                               /* tp_methods */
    0,                               /* tp_members */
    0,                               /* tp_getset */
    0,                               /* tp_base */
    0,                               /* tp_dict */
    0,                               /* tp_descr_get */
    0,                               /* tp_descr_set */
    0,                               /* tp_dictoffset */
    0,                               /* tp_init */
    0,                               /* tp_alloc */
    DapChainCSObject_new,               /* tp_new */
};


#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_CS_
