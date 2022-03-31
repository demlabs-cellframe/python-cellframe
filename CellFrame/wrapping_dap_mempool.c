#include "wrapping_dap_mempool.h"

PyMethodDef  DapMempoolMethods[] = {
        {"emissionPlace", wrapping_dap_mempool_emission_place, METH_VARARGS | METH_STATIC, ""},
        {"emissionGet", dap_chain_mempool_emission_get_py, METH_VARARGS | METH_STATIC, ""},
        {"addProc", dap_chain_mempool_add_proc_py, METH_VARARGS | METH_STATIC, ""},
        {"txCreate", dap_chain_mempool_tx_create_py, METH_VARARGS | METH_STATIC, ""},
        {"txCreateCond", dap_chain_mempool_tx_create_cond_py, METH_VARARGS | METH_STATIC, ""},
        {"txCreateCondInput", dap_chain_mempool_tx_create_cond_input_py, METH_VARARGS | METH_STATIC, ""},
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

PyObject *wrapping_dap_mempool_emission_place(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_chain;
    PyObject *obj_emission;
    if (!PyArg_ParseTuple(args, "OO", &obj_chain, &obj_emission)){
        return NULL;
    }
    if (!PyDapChain_Check(obj_chain)){
        PyErr_SetString(PyExc_AttributeError, "The second argument was incorrectly passed to this "
                                              "function, the first argument must be an object of type "
                                              "CellFrame.Chain.Chain.");
        return NULL;
    }
    if (!PyDapChainDatumTokenEmissionObject_check(obj_emission)){
        PyErr_SetString(PyExc_AttributeError, "The second argument was incorrectly passed"
                                              " to this function, the second argument must be an object of "
                                              "type ChainDatumTokenEmission. ");
        return NULL;
    }
    size_t l_emission_size = dap_chain_datum_emission_get_size(
            (uint8_t*)((PyDapChainDatumTokenEmissionObject*)obj_emission)->token_emission);
    dap_chain_datum_t *l_datum = dap_chain_datum_create(
            DAP_CHAIN_DATUM_TOKEN_EMISSION,
            ((PyDapChainDatumTokenEmissionObject*)obj_emission)->token_emission, l_emission_size);
    char *l_str = dap_chain_mempool_datum_add(l_datum, ((PyDapChainObject*)obj_chain)->chain_t);
    if (l_str == NULL){
        return Py_None;
    }
    return Py_BuildValue("s", l_str);
}

PyObject *dap_chain_mempool_emission_get_py(PyObject *self, PyObject * args){
    (void)self;
    PyObject *obj_chain;
    char *l_emission_hash;
    if (!PyArg_ParseTuple(args, "Os", &obj_chain, &l_emission_hash)){
        return NULL;
    }
    if (!PyDapChain_Check(obj_chain)){
        PyErr_SetString(PyExc_AttributeError, "The first argument passed to the wrong function, the first"
                                              " argument must be an object of type Chain.");
        return NULL;
    }
    dap_chain_datum_token_emission_t *l_token = dap_chain_mempool_emission_get(
            ((PyDapChainObject*)obj_chain)->chain_t, l_emission_hash);
    PyDapChainDatumTokenEmissionObject *l_emi = PyObject_New(PyDapChainDatumTokenEmissionObject,
                                                             &DapChainDatumTokenEmissionObjectType);
    l_emi->token_emission = l_token;
    l_emi->token_size = dap_chain_datum_emission_get_size((uint8_t*)l_token);
    return (PyObject*)l_emi;
}

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

PyObject *dap_chain_mempool_tx_create_cond_py(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainNetObject *obj_net;
    PyObject* obj_key_from;
    PyObject* obj_key_cond;
    char *l_token_ticker;
    uint64_t l_value;
    uint64_t l_value_per_unit_max;
    PyObject *obj_unit;
    PyObject *obj_srv_uid;
    uint64_t l_fee;
    PyObject *obj_cond;
    if (!PyArg_ParseTuple(args, "OOOskkOOkO", &obj_net, &obj_key_from, &obj_key_cond, &l_token_ticker, &l_value,
                          &l_value_per_unit_max, &obj_unit, &obj_srv_uid, &l_fee, &obj_cond)){
        PyErr_SetString(PyExc_AttributeError, "Function takes exactly ten arguments.");
        return NULL;
    }
    if (!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_AttributeError, "Invalid first argument passed. The first argument must "
                                              "be an instance of an object of type ChainNet. ");
        return NULL;
    }
    void *l_bytes_cond = PyBytes_AsString(obj_cond);
    size_t l_bytes_cond_size = PyBytes_Size(obj_cond);
    uint256_t l_value_256 = dap_chain_uint256_from(l_value);
    uint256_t l_value_per_unit_max_256 = dap_chain_uint256_from(l_value_per_unit_max);
    uint256_t l_fee_256  = dap_chain_uint256_from(l_fee);
    dap_hash_fast_t *l_hf = dap_chain_mempool_tx_create_cond(
            obj_net->chain_net,
            ((PyCryptoKeyObject*)obj_key_from)->key,
            ((PyDapPkeyObject *)obj_key_cond)->pkey,
            l_token_ticker,
            l_value_256,
            l_value_per_unit_max_256,
            ((PyDapChainNetSrvPriceUnitUIDObject*)obj_unit)->price_unit_uid,
            ((PyDapChainNetSrvUIDObject*)obj_srv_uid)->net_srv_uid,
            l_fee_256,
            l_bytes_cond,
            l_bytes_cond_size
    );
    if (!l_hf){
        return Py_None;
    }
    PyDapHashFastObject *l_obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    l_obj_hf->hash_fast = l_hf;
    return (PyObject*)l_hf;
}
PyObject *dap_chain_mempool_tx_create_cond_input_py(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainNetObject *obj_net;
    PyDapHashFastObject *obj_tx_prev_hash;
    PyObject *obj_addr_to;
    PyObject *obj_key_tx_sign;
    PyObject *obj_receipt;
    if (!PyArg_ParseTuple(args, "OOOOO", &obj_net, &obj_tx_prev_hash, &obj_addr_to, &obj_key_tx_sign, &obj_receipt)){
        PyErr_SetString(PyExc_AttributeError, "Function takes exactly five arguments.");
        return NULL;
    }
    if (!PyDapChainNet_Check(obj_net)) {
        PyErr_SetString(PyExc_AttributeError, "Invalid first argument passed. The first argument must "
                                              "be an instance of an object of type ChainNet. ");
        return NULL;
    }
    if (!PyDapHashFast_Check(obj_tx_prev_hash)){
        PyErr_SetString(PyExc_AttributeError, "Invalid second argument passed. The first argument must "
                                              "be an instance of an object of type ChainNet. ");
        return NULL;
    }
    if (!PyDapSignObject_Check(obj_key_tx_sign)){
        PyErr_SetString(PyExc_AttributeError, "Invalid fourth argument passed. The first argument must "
                                              "be an instance of an object of type DapSign.");
        return NULL;
    }
    dap_chain_hash_fast_t *l_hf =dap_chain_mempool_tx_create_cond_input(
            obj_net->chain_net,
            obj_tx_prev_hash->hash_fast,
            ((PyDapChainAddrObject *)obj_addr_to)->addr,
            ((PyCryptoKeyObject*)obj_key_tx_sign)->key,
            ((PyDapChainTXReceiptObject*)obj_receipt)->tx_receipt);
    if (!l_hf){
        return Py_None;
    }
    PyDapHashFastObject *l_obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    l_obj_hf->hash_fast = l_hf;
    return (PyObject*)l_hf;
}
