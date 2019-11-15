#ifndef _WRAPPING_DAP_CHAIN_DATUM_TX_
#define _WRAPPING_DAP_CHAIN_DATUM_TX_

#include "Python.h"
#include "dap_chain_datum_tx.h"
#include "wrapping_dap_chain_common.h"
#include "libdap_crypto_key_python.h"
#include "dap_chain_datum_tx_out_cond.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DAP chain tx iter type */
typedef struct PyDapChainTxItemType{
    PyObject_HEAD
}PyDapChainTxItemTypeObject;

PyObject *TX_ITEM_TYPE_IN_PY(void);
PyObject *TX_ITEM_TYPE_OUT_PY(void);
PyObject *TX_ITEM_TYPE_PKEY_PY(void);
PyObject *TX_ITEM_TYPE_SIG_PY(void);
PyObject *TX_ITEM_TYPE_TOKEN_PY(void);
PyObject *TX_ITEM_TYPE_IN_COND_PY(void);
PyObject *TX_ITEM_TYPE_OUT_COND_PY(void);
PyObject *TX_ITEM_TYPE_RECEIPT_PY(void);

static PyMethodDef PyDapChainTxItemTypeObjectMethods[] ={
    {"TX_ITEM_TYPE_IN", (PyCFunction)TX_ITEM_TYPE_IN_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_OUT", (PyCFunction)TX_ITEM_TYPE_OUT_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_PKEY", (PyCFunction)TX_ITEM_TYPE_PKEY_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_SIG", (PyCFunction)TX_ITEM_TYPE_SIG_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_TOKEN", (PyCFunction)TX_ITEM_TYPE_TOKEN_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_IN_COND", (PyCFunction)TX_ITEM_TYPE_IN_COND_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_OUT_COND", (PyCFunction)TX_ITEM_TYPE_OUT_COND_PY, METH_NOARGS | METH_STATIC, ""},
    {"TX_ITEM_TYPE_RECEIPT", (PyCFunction)TX_ITEM_TYPE_RECEIPT_PY,
                                                            METH_NOARGS | METH_STATIC, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainTxItemObject_DapChainTxItemTypeObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.TxItemType",        /* tp_name */
    sizeof(PyDapChainTxItemTypeObject), /* tp_basicsize */
    0,                                  /* tp_itemsize */
    0,                                  /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash  */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    0,                                  /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    "Chain tx item type object",        /* tp_doc */
    0,		                            /* tp_traverse */
    0,		                            /* tp_clear */
    0,		                            /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,		                            /* tp_iter */
    0,		                            /* tp_iternext */
    PyDapChainTxItemTypeObjectMethods,  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    PyType_GenericNew,                  /* tp_new */
};

/* -------------------------------------- */

/* DAP chain tx cond*/
typedef struct PyDapChainTxCondType{
    PyObject_HEAD
    dap_chain_tx_cond_type_t *tx_cond_type_t;
}PyDapChainTxCondTypeObject;

static PyTypeObject DapChainTxType_DapChainTxCondTypeObject = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.TxCondType",       /* tp_name */
    sizeof(PyDapChainTxCondTypeObject),/* tp_basicsize */
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
    "Chain tx cond type object",             /* tp_doc */
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

/* -------------------------------------- */

/* DAP chain datum tx */
typedef struct PyDapChainDatumTx{
    PyObject_HEAD
    dap_chain_datum_tx_t *datum_tx;
}PyDapChainDatumTxObject;

PyObject *PyDapChainDatumTxObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds);
void PyDapChainDatumTxObject_delete(PyDapChainDatumTxObject* datumTx);
PyObject *dap_chain_datum_tx_get_size_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_add_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_add_in_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_add_out_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_add_out_cond_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_add_sign_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_verify_sign_py(PyObject *self, PyObject *args);

static dap_chain_datum_tx_t **PyListToDapChainDatumTxArray(PyObject *a_in_obj);
static PyObject* DapChainDatumTxArrayToPyList(dap_chain_datum_tx_t** datum_txs);


static PyMethodDef PyDapChainDatumTxObjectMethods[] ={
    {"getSize", (PyCFunction)dap_chain_datum_tx_get_size_py, METH_VARARGS, ""},
    {"addItem", (PyCFunction)dap_chain_datum_tx_add_item_py, METH_VARARGS | METH_STATIC, ""},
    {"addInItem", (PyCFunction)dap_chain_datum_tx_add_in_item_py, METH_VARARGS | METH_STATIC, ""},
    {"addOutItem", (PyCFunction)dap_chain_datum_tx_add_out_item_py, METH_VARARGS | METH_STATIC, ""},
    {"addOutCond", (PyCFunction)dap_chain_datum_tx_add_out_cond_item_py, METH_VARARGS | METH_STATIC, ""},
    {"addSignItem", (PyCFunction)dap_chain_datum_tx_add_sign_item_py, METH_VARARGS | METH_STATIC, ""},
    {"verifySign", (PyCFunction)dap_chain_datum_tx_verify_sign_py, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainDatumTx_DapChainDatumTxObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.DatumTx",                      /* tp_name */
    sizeof(PyDapChainDatumTxObject),               /* tp_basicsize */
    0,                                             /* tp_itemsize */
    (destructor)PyDapChainDatumTxObject_delete,    /* tp_dealloc */
    0,                                              /* tp_print */
    0,                                              /* tp_getattr */
    0,                                              /* tp_setattr */
    0,                                              /* tp_reserved */
    0,                                              /* tp_repr */
    0,                                              /* tp_as_number */
    0,                                              /* tp_as_sequence */
    0,                                              /* tp_as_mapping */
    0,                                              /* tp_hash  */
    0,                                              /* tp_call */
    0,                                              /* tp_str */
    0,                                              /* tp_getattro */
    0,                                              /* tp_setattro */
    0,                                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                        /* tp_flags */
    "Chain datum tx object",                        /* tp_doc */
    0,		                                        /* tp_traverse */
    0,		                                        /* tp_clear */
    0,		                                        /* tp_richcompare */
    0,                                              /* tp_weaklistoffset */
    0,		                                        /* tp_iter */
    0,		                                        /* tp_iternext */
    PyDapChainDatumTxObjectMethods,                 /* tp_methods */
    0,                                              /* tp_members */
    0,                                              /* tp_getset */
    0,                                              /* tp_base */
    0,                                              /* tp_dict */
    0,                                              /* tp_descr_get */
    0,                                              /* tp_descr_set */
    0,                                              /* tp_dictoffset */
    0,                                              /* tp_init */
    0,                                              /* tp_alloc */
    PyDapChainDatumTxObject_create,                 /* tp_new */
};

/* -------------------------------------- */

typedef struct PyDapChainTxOutCond{
    PyObject_HEAD
    dap_chain_tx_out_cond_t *out_cond;
}PyDapChainTxOutCondObject;

static PyTypeObject DapChainTxOutCond_DapChainTxOutCondObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.TxOutCond",                      /* tp_name */
    sizeof(PyDapChainTxOutCondObject),               /* tp_basicsize */
    0,                                             /* tp_itemsize */
    0,                                             /* tp_dealloc */
    0,                                              /* tp_print */
    0,                                              /* tp_getattr */
    0,                                              /* tp_setattr */
    0,                                              /* tp_reserved */
    0,                                              /* tp_repr */
    0,                                              /* tp_as_number */
    0,                                              /* tp_as_sequence */
    0,                                              /* tp_as_mapping */
    0,                                              /* tp_hash  */
    0,                                              /* tp_call */
    0,                                              /* tp_str */
    0,                                              /* tp_getattro */
    0,                                              /* tp_setattro */
    0,                                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                        /* tp_flags */
    "Chain tx out cond object",                        /* tp_doc */
    0,		                                        /* tp_traverse */
    0,		                                        /* tp_clear */
    0,		                                        /* tp_richcompare */
    0,                                              /* tp_weaklistoffset */
    0,		                                        /* tp_iter */
    0,		                                        /* tp_iternext */
    0,                                              /* tp_methods */
    0,                                              /* tp_members */
    0,                                              /* tp_getset */
    0,                                              /* tp_base */
    0,                                              /* tp_dict */
    0,                                              /* tp_descr_get */
    0,                                              /* tp_descr_set */
    0,                                              /* tp_dictoffset */
    0,                                              /* tp_init */
    0,                                              /* tp_alloc */
    PyType_GenericNew,                              /* tp_new */
};

dap_chain_tx_out_cond_t **PyListToDapChainTxOutCond(PyObject *list);
PyObject *DapChainTxOutCondObjectToPyList(dap_chain_tx_out_cond_t **out_cond);

/* -------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_DATUM_TX_
