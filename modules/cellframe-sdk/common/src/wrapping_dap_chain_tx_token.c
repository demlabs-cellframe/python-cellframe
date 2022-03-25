#include "wrapping_dap_chain_tx_token.h"

PyGetSetDef PyDapChainTxTokenGetsSetsDef[] = {
        {"ticker", (getter)wrapping_dap_chain_tx_token_get_ticker, NULL, NULL, NULL},
        {"tokenEmissionHash", (getter)wrapping_dap_chain_tx_token_get_token_emission_hash, NULL, NULL, NULL},
        {"tokenEmissionChainId", (getter)wrapping_dap_chain_tx_token_get_token_emission_chain_id, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainTxToken_DapChainTxTokenType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainTxToken",       /* tp_name */
        sizeof(PyDapChainTxTokenObject),      /* tp_basicsize */
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
        "Chain tx token object",              /* tp_doc */
        0,		                                 /* tp_traverse */
        0,		                                 /* tp_clear */
        0,		                                 /* tp_richcompare */
        0,                                       /* tp_weaklistoffset */
        0,		                                 /* tp_iter */
        0,		                                 /* tp_iternext */
        0,                                       /* tp_methods */
        0,                                       /* tp_members */
        PyDapChainTxTokenGetsSetsDef,                                       /* tp_getset */
        0,                                       /* tp_base */
        0,                                       /* tp_dict */
        0,                                       /* tp_descr_get */
        0,                                       /* tp_descr_set */
        0,                                       /* tp_dictoffset */
        0,                                       /* tp_init */
        0,                                       /* tp_alloc */
        PyType_GenericNew,                       /* tp_new */
};

int PyDapChainTxTokenObject_init(PyDapChainTxTokenObject *self, PyObject *args, PyObject *kwds){
    const char* kwlist[] = {
            "chain",
            "tokenHash",
            "ticker",
            NULL
    };
    PyObject *obj_chain;
    PyObject *obj_datum_token_hash;
    char *l_ticker;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOs", (char**)kwlist, &obj_chain, &obj_datum_token_hash, &l_ticker)){
        return -1;
    }
    if (!PyDapHashFast_Check((PyDapHashFastObject*)obj_datum_token_hash)){
        return  -1;
    }
    if (PyDapChain_Check(obj_chain)){
        self->token = dap_chain_datum_tx_item_token_create(
                ((PyDapChainObject*)obj_chain)->chain_t->id,
                ((PyDapHashFastObject*)obj_datum_token_hash)->hash_fast,
                l_ticker);
        return  0;
    }
    if (DapChainIdObject_Check(obj_chain)){
        self->token = dap_chain_datum_tx_item_token_create(
                *((PyDapChainIDObject *)obj_chain)->chain_id,
                ((PyDapHashFastObject*)obj_datum_token_hash)->hash_fast,
                l_ticker);
        return 0;
    }
    return -1;
}

bool PyDapChainTxTokenObject_check(PyObject *self){
    return PyObject_TypeCheck(self, &DapChainTxToken_DapChainTxTokenType);
}

PyObject *wrapping_dap_chain_tx_token_get_ticker(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainTxTokenObject*)self)->token->header.ticker);
}
PyObject *wrapping_dap_chain_tx_token_get_token_emission_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    PyObject_Dir((PyObject*)obj_hash);
    obj_hash->hash_fast = &((PyDapChainTxTokenObject*)self)->token->header.token_emission_hash;
    return (PyObject*)obj_hash;
}
PyObject *wrapping_dap_chain_tx_token_get_token_emission_chain_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainIDObject *obj_chain_id = PyObject_New(PyDapChainIDObject, &DapChainIdObjectType);
    PyObject_Dir((PyObject*)obj_chain_id);
    obj_chain_id->chain_id = &((PyDapChainTxTokenObject*)self)->token->header.token_emission_chain_id;
    return (PyObject*)obj_chain_id;
}
