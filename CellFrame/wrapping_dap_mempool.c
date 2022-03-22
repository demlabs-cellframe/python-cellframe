#include "wrapping_dap_mempool.h"

PyMethodDef  DapMempoolMethods[] = {
        {"addProc", dap_chain_mempool_add_proc_py, METH_VARARGS | METH_STATIC, ""},
        {"txCreate", dap_chain_mempool_tx_create_py, METH_VARARGS | METH_STATIC, ""},
        {NULL,NULL,0,NULL}
};

PyTypeObject DapChainMempoolObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.DapMempool",             /* tp_name */
        sizeof(PyDapMempoolObject),                   /* tp_basicsize */
        0,                                                 /* tp_itemsize */
        0,                                                 /* tp_dealloc */
        0,                                                 /* tp_print */
        0,                                                 /* tp_getattr */
        0,                                                 /* tp_setattr */
        0,                                                 /* tp_reserved */
        0,                                                 /* tp_repr */
        0,                                                 /* tp_as_number */
        0,                                                 /* tp_as_sequence */
        0,                                                 /* tp_as_mapping */
        0,                                                 /* tp_hash  */
        0,                                                 /* tp_call */
        0,                                                 /* tp_str */
        0,                                                 /* tp_getattro */
        0,                                                 /* tp_setattro */
        0,                                                 /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                           /* tp_flags */
        "Dap mempool object",                         /* tp_doc */
        0,		                                       /* tp_traverse */
        0,                        		               /* tp_clear */
        0,		                                       /* tp_richcompare */
        0,                        		               /* tp_weaklistoffset */
        0,		                                       /* tp_iter */
        0,                        		               /* tp_iternext */
        DapMempoolMethods,                   /* tp_methods */
        0,                                                 /* tp_members */
        0,                                                 /* tp_getset */
        0,                                                 /* tp_base */
        0,                                                 /* tp_dict */
        0,                                                 /* tp_descr_get */
        0,                                                 /* tp_descr_set */
        0,                                                 /* tp_dictoffset */
        0,                                                 /* tp_init */
        0,                                                 /* tp_alloc */
        PyType_GenericNew,                                 /* tp_new */
};

PyObject *dap_chain_mempool_add_proc_py(PyObject *self, PyObject *args){
    PyObject *obj_server;
    const char *MEMPOOL_URL;
    if (!PyArg_ParseTuple(args, "O|s", &obj_server, &MEMPOOL_URL))
        return NULL;
    dap_chain_mempool_add_proc(DAP_HTTP(((PyDapServerObject*)obj_server)->t_server), MEMPOOL_URL);
    return PyLong_FromLong(0);
}

PyObject *dap_chain_mempool_tx_create_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_chain;
    PyObject *obj_key_from;
    PyObject *obj_addr_from;
    PyObject *obj_addr_to;
    PyObject *obj_addr_fee;
    char *l_token_ticker;
    uint64_t l_value;
    uint64_t l_value_fee;
    if (!PyArg_ParseTuple(args, "OOOOOskk", &obj_chain, &obj_key_from, &obj_addr_from, &obj_addr_to, &obj_addr_fee,
                          &l_token_ticker, &l_value, &l_value_fee)){
        return NULL;
    }
    dap_chain_t *l_chain = ((PyDapChainObject*)obj_chain)->chain_t;
    dap_enc_key_t *l_key_from = ((PyCryptoKeyObject*)obj_key_from)->key;
    dap_chain_addr_t *l_addr_from = ((PyDapChainAddrObject*)obj_addr_from)->addr;
    dap_chain_addr_t *l_addr_to = ((PyDapChainAddrObject*)obj_addr_to)->addr;
    dap_chain_addr_t *l_addr_fee = ((PyDapChainAddrObject*)obj_addr_fee)->addr;
    uint256_t l_value_256 = dap_chain_uint256_from(l_value);
    uint256_t l_value_fee_256 = dap_chain_uint256_from(l_value_fee);
    dap_chain_hash_fast_t  *l_hash_tx = dap_chain_mempool_tx_create(l_chain, l_key_from,
                                                                    l_addr_from, l_addr_to,
                                                                    l_addr_fee, l_token_ticker,
                                                                    l_value_256, l_value_fee_256);
    if (l_hash_tx == NULL){
        return Py_None;
    } else {
        PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
        PyObject_Dir((PyObject*)obj_hf);
        obj_hf->hash_fast = l_hash_tx;
        return (PyObject*)obj_hf;
    }
}