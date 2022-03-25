#include "wrapping_dap_chain_tx_sig.h"

PyGetSetDef DapChainTxSigGetsSetsDef[] = {
        {"sign", (getter)wrapping_dap_chain_tx_sig_get_sign, NULL, NULL, NULL},
        {"sigSize", (getter)wrapping_dap_chain_tx_sig_get_sig_size, NULL, NULL, NULL},
        {NULL}
};

PyMethodDef PyDapChainTxSigObjectMethods[] ={
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainTxSigObject_DapChainTxSigTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxSig",        /* tp_name */
        sizeof(PyDapChainTXSigObject), /* tp_basicsize */
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
        "Chain tx signature object",        /* tp_doc */
        0,		                            /* tp_traverse */
        0,		                            /* tp_clear */
        0,		                            /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,		                            /* tp_iter */
        0,		                            /* tp_iternext */
        PyDapChainTxSigObjectMethods,  /* tp_methods */
        0,                                  /* tp_members */
        DapChainTxSigGetsSetsDef,                                  /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        (initproc)PyDapChainTXSigObject_init,   /* tp_init */
        0,                                  /* tp_alloc */
        PyType_GenericNew,                  /* tp_new */
};

int PyDapChainTXSigObject_init(PyDapChainTXSigObject* self, PyObject *argv, PyObject *kwds){
    const char *kwlist[] = {
            "key",
            "data",
            NULL
    };
    PyObject *obj_key;
    PyObject *obj_data;
    if (!PyArg_ParseTupleAndKeywords(argv, kwds, "OO", kwlist, &obj_key, &obj_data)){
        return -1;
    }
    void *l_data = PyBytes_AsString(obj_data);
    size_t l_data_size = PyBytes_Size(obj_data);
    self->tx_sig = dap_chain_datum_tx_item_sign_create(
            ((PyCryptoKeyObject*)obj_key)->key, l_data, l_data_size);
    return 0;
}
bool PyDapChainTXSigObject_check(PyObject *self){
    return PyObject_TypeCheck(self, &DapChainTxSigObject_DapChainTxSigTypeObjectType);
}

PyObject *wrapping_dap_chain_tx_sig_get_sign(PyObject *self, void *closure){
    (void)closure;
    PyDapSignObject *obj_sign = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
    PyObject_Dir((PyObject*)obj_sign);
    obj_sign->sign = (dap_sign_t*)((PyDapChainTXSigObject*)self)->tx_sig->sig;
    return (PyObject*)obj_sign;
}

PyObject *wrapping_dap_chain_tx_sig_get_sig_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainTXSigObject*)self)->tx_sig->header.sig_size);
}