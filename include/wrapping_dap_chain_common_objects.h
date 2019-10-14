#ifndef _WRAPPING_DAP_CHAIN_COMMON_OBJECTS_
#define _WRAPPING_DAP_CHAIN_COMMON_OBJECTS_

#include "Python.h"
#include "dap_chain_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapChainId{
    PyObject_HEAD
    dap_chain_id_t *id;
}PyDapChainIdObject;

typedef struct PyDapChainCellId{
    PyObject_HEAD
    dap_chain_cell_id_t *id;
}PyDapChainCellId;

typedef struct PyDapChainNodeAddr{
    PyObject_HEAD
    dap_chain_node_addr_t *node_addr;
}PyDapChainNodeAddrObject;

typedef struct PyDapChainNodeRole{
    PyObject_HEAD
    dap_chain_node_role_t *node_role;
}PyDapChainNodeRoleObject;

/* DAP Chain Net ID */

typedef struct PyDapChainNetId{
    PyObject_HEAD
    dap_chain_net_id_t *net_id;
}PyDapChainNetIdObject; 

static PyTypeObject DapChainNetIdObject_DapChainNetIdObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainNetID",          /* tp_name */
    sizeof(PyDapChainNetIdObject),   /* tp_basicsize */
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
    "Chain net id object",           /* tp_doc */
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
    PyType_GenericNew,               /* tp_new */
};

/* ------------------------------- */

/* DAP chain Hash Slow Object */
typedef struct PyDapChainHashSlow{
    PyObject_HEAD
    dap_chain_hash_slow_t *hash_slow;
}PyDapChainHashSlowObject;

static PyTypeObject DapChainHashSlowObject_DapChainHashSlowObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainHashSlow",       /* tp_name */
    sizeof(PyDapChainHashSlowObject),/* tp_basicsize */
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
    "Chain hash slow object",        /* tp_doc */
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
    PyType_GenericNew,               /* tp_new */
};


/* ---------------------------------- */
/* DAP chain hash fast object */

typedef struct PyDapChainHashFast{
    PyObject_HEAD
    dap_chain_hash_fast_t *hash_fast;
}PyDapChainHashFastObject;

static PyTypeObject DapChainHashFastObject_DapChainHashFastObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainHashFast",       /* tp_name */
    sizeof(PyDapChainHashFastObject),/* tp_basicsize */
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
    "Chain hash fast object",        /* tp_doc */
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
    PyType_GenericNew,               /* tp_new */
};

/* ----------------------------------- */

typedef struct PyDapChainHashSlowKind{
    PyObject_HEAD
    dap_chain_hash_slow_kind_t *hash_slow_kind;
}PyDapChainHashSlowKindObject;

typedef struct PyDapChainPkeyType{
    PyObject_HEAD
    dap_chain_pkey_type_t *pkey_type;
}PyDapChainPkeyTypeObject;

typedef struct PyDapChainPkey{
    PyObject_HEAD
    dap_chain_pkey_t *pkey;
}PyDapChainPkeyObject;

typedef struct PyDapChainSignType{
    PyObject_HEAD
    dap_chain_sign_type_t *sign_type;
}PyDapChainSignTypeObject;

/* DAP chain addr object */
typedef struct PyDapChainAddr{
    PyObject_HEAD
    dap_chain_addr_t *addr;
}PyDapChainAddrObject;

static PyTypeObject DapChainAddrObject_DapChainAddrObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.ChainAddr",       /* tp_name */
    sizeof(PyDapChainAddrObject),/* tp_basicsize */
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
    "Chain addr object",             /* tp_doc */
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
    PyType_GenericNew,               /* tp_new */
};

/* ----------------------------- */

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_COMMON_OBJECTS_
