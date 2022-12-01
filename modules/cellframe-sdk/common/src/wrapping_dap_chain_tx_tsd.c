#include "wrapping_dap_chain_tx_tsd.h"

PyGetSetDef PyDapChainTxTSDGetsSetsDef[] = {
    {"data", (getter)wrapping_dap_chain_tx_get_tsd_data, NULL, NULL, NULL},
    {"type", (getter)wrapping_dap_chain_tx_get_tsd_type, NULL, NULL, NULL},
    {NULL}
};

PyTypeObject DapChainTxTSDObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxTSD",       /* tp_name */
        sizeof(PyDapChainTxTSDObject),      /* tp_basicsize */
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
        "Chain tx tsd object",              /* tp_doc */
        0,		                                 /* tp_traverse */
        0,		                                 /* tp_clear */
        0,		                                 /* tp_richcompare */
        0,                                       /* tp_weaklistoffset */
        0,		                                 /* tp_iter */
        0,		                                 /* tp_iternext */
        0,                                       /* tp_methods */
        0,                                       /* tp_members */
        PyDapChainTxTSDGetsSetsDef,                                       /* tp_getset */
        0,                                       /* tp_base */
        0,                                       /* tp_dict */
        0,                                       /* tp_descr_get */
        0,                                       /* tp_descr_set */
        0,                                       /* tp_dictoffset */
        0,                                       /* tp_init */
        0,                                       /* tp_alloc */
        PyType_GenericNew,                       /* tp_new */
};

PyObject *wrapping_dap_chain_tx_get_tsd_data(PyObject *self, void *closure) {
    (void)closure;
    dap_chain_tx_tsd_t *l_item = ((PyDapChainTxTSDObject*)self)->tsd;
    int l_type;
    size_t l_size;
    byte_t *l_data = dap_chain_datum_tx_item_get_data(l_item, &l_type, &l_size);
    if (!l_data || !l_type || !l_size)
        Py_RETURN_NONE;
    return PyBytes_FromStringAndSize((char*)l_data, l_size);
}

PyObject *wrapping_dap_chain_tx_get_tsd_type(PyObject *self, void *closure) {
    (void)closure;
    dap_chain_tx_tsd_t *l_item = ((PyDapChainTxTSDObject*)self)->tsd;
    int l_type;
    size_t l_size;
    dap_chain_datum_tx_item_get_data(l_item, &l_type, &l_size);
    return Py_BuildValue("i", l_type);
}
