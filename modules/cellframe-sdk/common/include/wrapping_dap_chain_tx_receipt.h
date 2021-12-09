#ifndef _WRAPPING_DAP_CHAIN_TX_RECEIPT_
#define _WRAPPING_DAP_CHAIN_TX_RECEIPT_

#include <Python.h>
#include "dap_chain_datum_tx_receipt.h"
#include "wrapping_dap_sign.h"
#include "dap_chain_common.h"
#include "wrapping_dap_sign.h"
#include "dap_sign.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapChainTXReceipt{
    PyObject_HEAD
    dap_chain_datum_tx_receipt_t *tx_receipt;
}PyDapChainTXReceiptObject;

PyObject *wrapping_dap_chain_tx_receipt_get_size(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_receipt_get_ext_size(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_receipt_get_units(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_receipt_get_uid(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_receipt_get_units_type(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_receipt_get_value(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_receipt_get_sig_provider(PyObject *self, void *closure);
PyObject *wrapping_dap_chain_tx_receipt_get_sig_client(PyObject *self, void *closure);

static PyTypeObject DapChainTxReceiptObject_DapChainTxReceiptTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxReceipt",        /* tp_name */
        sizeof(PyDapChainTXReceiptObject), /* tp_basicsize */
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
        "Chain tx item receipt object",        /* tp_doc */
        0,		                            /* tp_traverse */
        0,		                            /* tp_clear */
        0,		                            /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,		                            /* tp_iter */
        0,		                            /* tp_iternext */
        0,  /* tp_methods */
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

#ifdef __cplusplus
extern "C"{
#endif

#endif //_WRAPPING_DAP_CHAIN_TX_RECEIPT_