#include "wrapping_dap_chain_tx_in_cond.h"

PyGetSetDef DapChainTxInCondGetsSetsDef[] = {
        {"receiptPrevIdx",(getter)wrapping_dap_chain_tx_in_cond_get_receipt_prev_idx, NULL, NULL, NULL},
        {"prevHash", (getter)wrapping_dap_chain_tx_in_cond_get_prev_hash, NULL, NULL, NULL},
        {"outPrevIdx", (getter)wrapping_dap_chain_tx_in_cond_get_out_prev_idx, NULL, NULL, NULL},
        {NULL}
};

PyMethodDef  DapChainTxInCondMethodsDef[] = {
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainTxInCondObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxInCond",        /* tp_name */
        sizeof(PyDapChainTXInCondObject), /* tp_basicsize */
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
        "Chain tx in cond object",        /* tp_doc */
        0,		                            /* tp_traverse */
        0,		                            /* tp_clear */
        0,		                            /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,		                            /* tp_iter */
        0,		                            /* tp_iternext */
        DapChainTxInCondMethodsDef,  /* tp_methods */
        0,                                  /* tp_members */
        DapChainTxInCondGetsSetsDef,                                  /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        (initproc)PyDapChainTxInCond_init,                                  /* tp_init */
        0,                                  /* tp_alloc */
        PyType_GenericNew,                  /* tp_new */
};

int PyDapChainTxInCond_init(PyDapChainTXInCondObject* self, PyObject *args, PyObject *kwds){
    const char* kwlist[] = {
            "prevHash",
            "outPrevIdx",
            "receiptIdx",
            NULL
    };
    PyDapHashFastObject *obj_tx_prev_hash;
    uint32_t l_tx_out_prev_idx;
    uint32_t l_receipt_idx;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OII", (char **)kwlist, &obj_tx_prev_hash, &l_tx_out_prev_idx, &l_receipt_idx)){
        return -1;
    }
    if (!PyDapHashFast_Check(obj_tx_prev_hash)){
        return -1;
    }
    self->tx_in_cond = dap_chain_datum_tx_item_in_cond_create(
            obj_tx_prev_hash->hash_fast,
            l_tx_out_prev_idx,
            l_receipt_idx);
    return 0;
}

PyObject *wrapping_dap_chain_tx_in_cond_get_receipt_prev_idx(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainTXInCondObject*)self)->tx_in_cond->header.receipt_idx);
}
PyObject *wrapping_dap_chain_tx_in_cond_get_prev_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hash->hash_fast = &((PyDapChainTXInCondObject*)self)->tx_in_cond->header.tx_prev_hash;
    obj_hash->origin = false;
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_chain_tx_in_cond_get_out_prev_idx(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainTXInCondObject*)self)->tx_in_cond->header.tx_out_prev_idx);
}
