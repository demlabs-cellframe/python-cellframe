#include "wrapping_dap_chain_tx_out.h"

PyGetSetDef DapChainTxOutGetsSetsDef[] = {
        {"addr", (getter)wrapping_dap_chain_tx_out_get_addr, NULL, NULL, NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_get_value, NULL, NULL, NULL},
        {NULL}
};

PyMethodDef PyDapChainTxOutObjectMethods[] ={
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainTxOutObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxOut",        /* tp_name */
        sizeof(PyDapChainTXOutObject), /* tp_basicsize */
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
        "Chain tx out object",        /* tp_doc */
        0,		                            /* tp_traverse */
        0,		                            /* tp_clear */
        0,		                            /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,		                            /* tp_iter */
        0,		                            /* tp_iternext */
        PyDapChainTxOutObjectMethods,  /* tp_methods */
        0,                                  /* tp_members */
        DapChainTxOutGetsSetsDef,                                  /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        (initproc)PyDapChainTxOut_init,                                  /* tp_init */
        0,                                  /* tp_alloc */
        PyType_GenericNew,                  /* tp_new */
};

int PyDapChainTxOut_init(PyDapChainTXOutObject* self, PyObject* args, PyObject *kwds){
    const char *kwlist[] = {
            "addr",
            "value",
            NULL
    };
    PyObject *obj_addr;
    uint64_t l_value;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "Ok", (char**)kwlist, &obj_addr, &l_value)){
        return -1;
    }
    uint256_t l_value_256 = dap_chain_uint256_from(l_value);
    self->tx_out = dap_chain_datum_tx_item_out_create(((PyDapChainAddrObject*)obj_addr)->addr,
                                              l_value_256);
    return 0;
}

PyObject *wrapping_dap_chain_tx_out_get_addr(PyObject *self, void *closure){
    (void)closure;
    PyObject *obj_addr = _PyObject_New(&DapChainAddrObjectType);
    obj_addr = PyObject_Init(obj_addr, &DapChainAddrObjectType);
    PyObject_Dir(obj_addr);
    ((PyDapChainAddrObject*)obj_addr)->addr = &((PyDapChainTXOutObject*)self)->tx_out->addr;
    return obj_addr;
}
PyObject *wrapping_dap_chain_tx_out_get_value(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("K", ((PyDapChainTXOutObject*)self)->tx_out->header.value);
}
