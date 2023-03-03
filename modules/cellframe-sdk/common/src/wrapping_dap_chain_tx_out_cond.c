#include "wrapping_dap_chain_tx_out_cond.h"

PyGetSetDef PyDapChainTxOutCondGetsSetsDef[] = {
        {"tsExpires", (getter)wrapping_dap_chain_tx_out_cond_get_ts_expires, NULL, NULL, NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_cond_get_value, NULL, NULL, NULL},
        {"typeSubtype", (getter)wrapping_dap_chain_tx_out_cond_get_type_subtype, NULL, NULL, NULL},
        {"subtype", (getter)wrapping_dap_chain_tx_out_cond_get_subtype, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainTxOutCondObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxOutCond",       /* tp_name */
        sizeof(PyDapChainTxOutCondObject),      /* tp_basicsize */
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
        "Chain tx out cond object",              /* tp_doc */
        0,		                                 /* tp_traverse */
        0,		                                 /* tp_clear */
        0,		                                 /* tp_richcompare */
        0,                                       /* tp_weaklistoffset */
        0,		                                 /* tp_iter */
        0,		                                 /* tp_iternext */
        0,                                       /* tp_methods */
        0,                                       /* tp_members */
        PyDapChainTxOutCondGetsSetsDef,                                       /* tp_getset */
        0,                                       /* tp_base */
        0,                                       /* tp_dict */
        0,                                       /* tp_descr_get */
        0,                                       /* tp_descr_set */
        0,                                       /* tp_dictoffset */
        0,                                       /* tp_init */
        0,                                       /* tp_alloc */
        PyType_GenericNew,                       /* tp_new */
};

PyObject *wrapping_dap_chain_tx_out_cond_get_ts_expires(PyObject *self, void *closure){
    (void)closure;
    PyDateTime_IMPORT;
    PyObject *timestamp = Py_BuildValue("(k)", ((PyDapChainTxOutCondObject*)self)->out_cond->header.ts_expires);
    return PyDateTime_FromTimestamp(timestamp);
}
PyObject *wrapping_dap_chain_tx_out_cond_get_value(PyObject *self, void *closure){
    (void)closure;
    DapMathObject *obj_math = PyObject_New(DapMathObject, &DapMathObjectType);
    obj_math->value = ((PyDapChainTxOutCondObject*)self)->out_cond->header.value;
    return (PyObject*)obj_math;
}

// DapChaTxOutCondSubtype
PyTypeObject DapChainTxOutCondSubTypeObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxOutCondSubType",       /* tp_name */
        sizeof(PyDapChainTxOutCondSubTypeObject),      /* tp_basicsize */
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
        PyDapChainTxOutCondSubType_str,          /* tp_str */
        0,                                       /* tp_getattro */
        0,                                       /* tp_setattro */
        0,                                       /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                 /* tp_flags */
        "Chain tx out cond object",              /* tp_doc */
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

PyObject *wrapping_dap_chain_tx_out_cond_get_type_subtype(PyObject *self, void *closure){
    (void)closure;
    PyDapChainTxOutCondSubTypeObject *obj_type_subtype = PyObject_New(PyDapChainTxOutCondSubTypeObject,
                                              &DapChainTxOutCondSubTypeObjectType);
    obj_type_subtype->out_cond_subtype = &((PyDapChainTxOutCondObject*)self)->out_cond->header.subtype;
    return (PyObject*)obj_type_subtype;
}
PyObject *wrapping_dap_chain_tx_out_cond_get_subtype(PyObject *self, void *closure){
    (void)closure;
    Py_RETURN_NONE;
}

PyObject *PyDapChainTxOutCondSubType_str(PyObject *self){
    return Py_BuildValue("s",
                         dap_chain_tx_out_cond_subtype_to_str(*((PyDapChainTxOutCondSubTypeObject*)self)->out_cond_subtype)
                         );
}
