#ifndef _WRAPPING_DAP_CHAIN_CELL_
#define _WRAPPING_DAP_CHAIN_CELL_
#include "Python.h"
#include "dap_chain_cell.h"
#include "libdap-chain-python.h"

#ifdef __cplusplus
extern "C" {
#endif

int init(void);

/* DAP chain cell */
typedef struct PyDapChainCell{
    PyObject_HEAD
    dap_chain_cell_t *cell;
}PyDapChainCellObject;

void DapChainCellObject_delete(PyDapChainCellObject* object);
PyObject *DapChainCellObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds);

PyObject *dap_chain_cell_load_py(PyObject *self, PyObject *args);
PyObject *dap_chain_cell_file_update_py(PyObject *self, PyObject *args);
PyObject *dap_chain_cell_file_append_py(PyObject *self, PyObject *args);

static PyMethodDef PyDapChainCellObjectMethods[] ={
    {"load", dap_chain_cell_load_py, METH_VARARGS | METH_STATIC, ""},
    {"update", dap_chain_cell_file_update_py, METH_VARARGS, ""},
    {"append", dap_chain_cell_file_append_py, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainCell_DapChainCellObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.Cell",       /* tp_name */
    sizeof(PyDapChainCellObject),      /* tp_basicsize */
    0,                                       /* tp_itemsize */
    0,                                       /* tp_dealloc */
    0,                                       /* tp_print */
    0,                                       /* tp_getattr */
    0,                                       /* tp_setattr */
    0,                                       /* tp_reserved */
    0,                                       /* tp_repr */
    0,                                       /* tp_as_number */
    0,                                       /* tp_as_sequence */
    0,                                       /* tp_as_mapping */
    0,                                       /* tp_hash  */
    0,                                       /* tp_call */
    0,                                       /* tp_str */
    0,                                       /* tp_getattro */
    0,                                       /* tp_setattro */
    0,                                       /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                 /* tp_flags */
    "Chain cell object",                     /* tp_doc */
    0,		                                 /* tp_traverse */
    0,		                                 /* tp_clear */
    0,		                                 /* tp_richcompare */
    0,                                       /* tp_weaklistoffset */
    0,		                                 /* tp_iter */
    0,		                                 /* tp_iternext */
    PyDapChainCellObjectMethods,             /* tp_methods */
    0,                                       /* tp_members */
    0,                                       /* tp_getset */
    0,                                       /* tp_base */
    0,                                       /* tp_dict */
    0,                                       /* tp_descr_get */
    0,                                       /* tp_descr_set */
    0,                                       /* tp_dictoffset */
    0,                                       /* tp_init */
    0,                                       /* tp_alloc */
    PyType_GenericNew,                       /* tp_new */
};

static bool PyDapChainCell_Check(PyObject *self){
    return PyObject_TypeCheck(self, &DapChainCell_DapChainCellObjectType);
}

/* ------------------------------------------- */

typedef struct PyDapChainCellDeclReq{
    PyObject_HEAD
    dap_chain_cell_decl_req_t *decl_req;
}PyDapChainCellDeclReqObject;

static PyTypeObject DapChainCellDeclReq_DapChainCellDeclReqObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.DapChainCellDeclReq",         /* tp_name */
    sizeof(PyDapChainCellDeclReqObject),     /* tp_basicsize */
    0,                                       /* tp_itemsize */
    0,                                       /* tp_dealloc */
    0,                                       /* tp_print */
    0,                                       /* tp_getattr */
    0,                                       /* tp_setattr */
    0,                                       /* tp_reserved */
    0,                                       /* tp_repr */
    0,                                       /* tp_as_number */
    0,                                       /* tp_as_sequence */
    0,                                       /* tp_as_mapping */
    0,                                       /* tp_hash  */
    0,                                       /* tp_call */
    0,                                       /* tp_str */
    0,                                       /* tp_getattro */
    0,                                       /* tp_setattro */
    0,                                       /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                 /* tp_flags */
    "Chain cell decl req object",                     /* tp_doc */
    0,		                                 /* tp_traverse */
    0,		                                 /* tp_clear */
    0,		                                 /* tp_richcompare */
    0,                                       /* tp_weaklistoffset */
    0,		                                 /* tp_iter */
    0,		                                 /* tp_iternext */
    0,                                       /* tp_methods */
    0,                                       /* tp_members */
    0,                                       /* tp_getset */
    0,                                       /* tp_base */
    0,                                       /* tp_dict */
    0,                                       /* tp_descr_get */
    0,                                       /* tp_descr_set */
    0,                                       /* tp_dictoffset */
    0,                                       /* tp_init */
    0,                                       /* tp_alloc */
    PyType_GenericNew,                       /* tp_new */
};

typedef struct PyDapChainCellDecl{
    PyObject_HEAD
    dap_chain_cell_decl_t* decl;
}PyDapChainDeclObject;

static PyTypeObject DapChainDecl_DapChainDeclObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.DapChainCellDecl",       /* tp_name */
    sizeof(PyDapChainDeclObject),      /* tp_basicsize */
    0,                                       /* tp_itemsize */
    0,                                       /* tp_dealloc */
    0,                                       /* tp_print */
    0,                                       /* tp_getattr */
    0,                                       /* tp_setattr */
    0,                                       /* tp_reserved */
    0,                                       /* tp_repr */
    0,                                       /* tp_as_number */
    0,                                       /* tp_as_sequence */
    0,                                       /* tp_as_mapping */
    0,                                       /* tp_hash  */
    0,                                       /* tp_call */
    0,                                       /* tp_str */
    0,                                       /* tp_getattro */
    0,                                       /* tp_setattro */
    0,                                       /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                 /* tp_flags */
    "Chain cell decl object",                     /* tp_doc */
    0,		                                 /* tp_traverse */
    0,		                                 /* tp_clear */
    0,		                                 /* tp_richcompare */
    0,                                       /* tp_weaklistoffset */
    0,		                                 /* tp_iter */
    0,		                                 /* tp_iternext */
    0,                                       /* tp_methods */
    0,                                       /* tp_members */
    0,                                       /* tp_getset */
    0,                                       /* tp_base */
    0,                                       /* tp_dict */
    0,                                       /* tp_descr_get */
    0,                                       /* tp_descr_set */
    0,                                       /* tp_dictoffset */
    0,                                       /* tp_init */
    0,                                       /* tp_alloc */
    PyType_GenericNew,                       /* tp_new */
};

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_CELL_
