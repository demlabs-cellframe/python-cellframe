#include "wrapping_dap_mempool.h"
#define LOG_TAG "python-mempool"

PyMethodDef  DapMempoolMethods[] = {
        {"proc", dap_chain_mempool_proc_py, METH_VARARGS | METH_STATIC, ""},
        {"emissionPlace", wrapping_dap_mempool_emission_place, METH_VARARGS | METH_STATIC, ""},
        {"emissionGet", dap_chain_mempool_emission_get_py, METH_VARARGS | METH_STATIC, ""},
        {"emissionExtract", dap_chain_mempool_datum_emission_extract_py, METH_VARARGS | METH_STATIC, ""},
        {"txCreate", dap_chain_mempool_tx_create_py, METH_VARARGS | METH_STATIC, ""},
        {"baseTxCreate", dap_chain_mempool_base_tx_create_py, METH_VARARGS | METH_STATIC, ""},
        {"txCreateCond", dap_chain_mempool_tx_create_cond_py, METH_VARARGS | METH_STATIC, ""},
        {"txCreateCondInput", dap_chain_mempool_tx_create_cond_input_py, METH_VARARGS | METH_STATIC, ""},
        {"remove", dap_chain_mempool_remove_py, METH_VARARGS | METH_STATIC, ""},
        {"list", dap_chain_mempool_list_py, METH_VARARGS | METH_STATIC, ""},
        {"addDatum", dap_chain_mempool_add_datum_py, METH_VARARGS | METH_STATIC, ""},
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
    PyDapChainObject *obj_chain;
    PyDapChainDatumTokenEmissionObject *obj_emission;
    if (!PyArg_ParseTuple(args, "OO", &obj_chain, &obj_emission)){
        return NULL;
    }
    if (!PyDapChain_Check(obj_chain)){
        PyErr_SetString(PyExc_AttributeError, "The second argument was incorrectly passed to this "
                                              "function, the first argument must be an object of type "
                                              "CellFrame.Chain.Chain.");
        return NULL;
    }
    if (!PyDapChainDatumTokenEmissionObject_check((PyObject*)obj_emission)){
        PyErr_SetString(PyExc_AttributeError, "The second argument was incorrectly passed"
                                              " to this function, the second argument must be an object of "
                                              "type ChainDatumTokenEmission. ");
        return NULL;
    }
    size_t l_emission_size = dap_chain_datum_emission_get_size((uint8_t*)(obj_emission->token_emission));
    dap_chain_datum_t *l_datum = dap_chain_datum_create(
            DAP_CHAIN_DATUM_TOKEN_EMISSION,
            obj_emission->token_emission, l_emission_size);
    char *l_str = dap_chain_mempool_datum_add(l_datum, obj_chain->chain_t);
    if (l_str == NULL){
        Py_RETURN_NONE;
    }
    PyObject *l_str_obj = Py_BuildValue("s", l_str);
    DAP_DELETE(l_str);
    return l_str_obj;
}

PyObject *dap_chain_mempool_emission_get_py(PyObject *self, PyObject * args){
    PyDapChainObject *obj_chain;
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
            obj_chain->chain_t, l_emission_hash);
    if (l_token == NULL){
        Py_RETURN_NONE;
    }
    PyDapChainDatumTokenEmissionObject *l_emi = PyObject_New(PyDapChainDatumTokenEmissionObject,
                                                             &DapChainDatumTokenEmissionObjectType);
    l_emi->token_emission = l_token;
    l_emi->token_size = dap_chain_datum_emission_get_size((uint8_t*)l_token);
    l_emi->copy = true;
    return (PyObject*)l_emi;
}

PyObject* dap_chain_mempool_datum_emission_extract_py(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainObject *obj_chain;
    PyObject *obj_bytes;
    if (!PyArg_ParseTuple(args, "OO", &obj_chain, &obj_bytes)){
        return NULL;
    }
    if (!PyDapChain_Check(obj_chain)){
        PyErr_SetString(PyExc_AttributeError, "The first argument was not correctly passed to "
                                              "this function. The first argument must be an instance of an object of type Chain.");
        return NULL;
    }
    if (!PyBytes_Check(obj_bytes)){
        PyErr_SetString(PyExc_AttributeError, "The second argument of the function was passed incorrectly,"
                                              " this function takes an instance of an object of the bytes type as the "
                                              "second argument.");
        return NULL;
    }
    void *l_bytes = PyBytes_AsString(obj_bytes);
    size_t l_bytes_size = PyBytes_Size(obj_bytes);
    dap_chain_datum_token_emission_t *l_emi = dap_chain_mempool_datum_emission_extract(
            obj_chain->chain_t, l_bytes, l_bytes_size);
    if (l_emi == NULL){
        Py_RETURN_NONE;
    }
    PyDapChainDatumTokenEmissionObject *l_obj_emi = PyObject_New(PyDapChainDatumTokenEmissionObject,
                                                                 &DapChainDatumTokenEmissionObjectType);
    l_obj_emi->token_emission = l_emi;
    l_obj_emi->token_size = dap_chain_datum_emission_get_size((byte_t*)l_emi);
    l_obj_emi->copy = true;
    return (PyObject*)l_obj_emi;
}

PyObject *dap_chain_mempool_proc_py(PyObject *self, PyObject *args)
{
    UNUSED(self);
    PyDapChainObject *obj_chain = NULL;
    char *l_hash_str = NULL;
    if (!PyArg_ParseTuple(args, "sO", &l_hash_str, &obj_chain)) {
        return NULL;
    }
    if (!PyDapChain_Check(obj_chain)) {
        char *l_str = "The second function argument is invalid, it must be an "
                                              "instance of an object of type CellFrame.Chain.Chain ";
        PyErr_SetString(PyExc_AttributeError, l_str);
        log_it(L_ERROR, "%s", l_str);
        return NULL;
    }
    dap_chain_t *l_chain = obj_chain->chain_t;
    dap_chain_net_t *l_net = dap_chain_net_by_id(l_chain->net_id);
    // If full or light it doesnt work
    if(dap_chain_net_get_role(l_net).enums>= NODE_ROLE_FULL){
        char *l_str = dap_strdup_printf("Need master node role or higher for network %s to process this command", l_net->pub.name);
        PyErr_SetString(PyExc_RuntimeError, l_str);
        log_it(L_ERROR, "%s", l_str);
        DAP_DELETE(l_str);
        return NULL;
    }

    char *l_gdb_group_mempool = NULL;
    l_gdb_group_mempool = dap_chain_net_get_gdb_group_mempool(l_chain);

    size_t l_datum_size = 0;
    dap_chain_datum_t *l_datum = (dap_chain_datum_t*) dap_chain_global_db_gr_get(l_hash_str,
                                                              &l_datum_size, l_gdb_group_mempool);
    if (!l_datum){
        char *l_str = dap_strdup_printf("Failed to get data from chain %s on network %s using hash %s",
                                                                l_chain->name, l_net->pub.name, l_hash_str);
        PyErr_SetString(PyExc_AttributeError, l_str);
        log_it(L_ERROR, "%s", l_str);
        DAP_DELETE(l_str);
        DAP_DELETE(l_gdb_group_mempool);
        return NULL;
    }
    size_t l_datum_size2 = l_datum ? dap_chain_datum_size(l_datum) : 0;
    if (l_datum_size != l_datum_size2) {
        char *l_str = dap_strdup_printf("Error! Corrupted datum %s, size by datum headers is %zd when in mempool is only %zd bytes",
                                       l_datum_size2, l_datum_size);
        PyErr_SetString(PyExc_RuntimeError, l_str);
        log_it(L_ERROR, "%s", l_str);
        DAP_DELETE(l_str);
        DAP_DELETE(l_gdb_group_mempool);
        return NULL;
    }
    if (dap_chain_node_mempool_process(l_chain, l_datum)) {
        bool res_del_mempool = dap_chain_global_db_gr_del(l_hash_str, l_gdb_group_mempool);
        if (!res_del_mempool) {
            char *l_str = "Warning! Can't delete datum from mempool!";
            PyErr_SetString(PyExc_Warning, l_str);
            return NULL;
        }
    }
    DAP_DELETE(l_gdb_group_mempool);
    Py_RETURN_NONE;
}

PyObject *dap_chain_mempool_base_tx_create_py(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainObject *obj_chain, *obj_emi_chain;
    PyDapHashFastObject *obj_emi_hash;
    DapMathObject *obj_emission_value;
    char *l_ticker;
    PyDapChainAddrObject *obj_addr_to;
    PyObject *obj_certs;
    if (!PyArg_ParseTuple(args, "OOOOsOO", &obj_chain, &obj_emi_hash, &obj_emi_chain, &obj_emission_value,
                          &l_ticker, &obj_addr_to, &obj_certs)) {
        return NULL;
    }
    if (!PyDapChain_Check(obj_chain)){
        PyErr_SetString(PyExc_AttributeError, "The first argument was not correctly passed to "
                                              "this function. The first argument must be an instance of an object of type Chain. ");
        return NULL;
    }
    if (!PyDapHashFast_Check(obj_emi_hash)){
        PyErr_SetString(PyExc_AttributeError, "The second argument was not correctly passed to this "
                                              "function. The second argument must be an instance of a HashFast object"
                                              " containing the hash of the emission.");
        return NULL;
    }
    if (!PyDapChain_Check(obj_emi_chain)){
        PyErr_SetString(PyExc_AttributeError, "The third argument was not correctly passed to this "
                                              "function. The third argument must be an instance of an object of type "
                                              "Chain that contains the emission. ");
        return NULL;
    }
    if (!PyDapChainAddrObject_Check(obj_addr_to)){
        PyErr_SetString(PyExc_AttributeError, "The sixth argument was not correctly passed to this "
                                              "function. The sixth argument should be an instance of an object of type "
                                              "ChainAddr , which indicates which wallet address the underlying "
                                              "transaction should be made to. ");
        return NULL;
    }
    if (!PyList_Check(obj_certs)){
        PyErr_SetString(PyExc_AttributeError, "The seventh argument was not correctly passed to this "
                                              "function. The seventh argument must be an instance of an object of type "
                                              "list, which holds the list of certificates with which the underlying "
                                              "transaction is to be signed.");
        return NULL;
    }
    size_t l_certs_count = PyList_Size(obj_certs);
    dap_cert_t **l_certs = DAP_NEW_Z_SIZE(dap_cert_t*, l_certs_count);
    for (size_t i=0; i < l_certs_count; i++){
        PyCryptoCertObject *l_ptr = (PyCryptoCertObject*)PyList_GetItem(obj_certs, (Py_ssize_t)i);
        l_certs[i] = l_ptr->cert;
    }
    dap_chain_hash_fast_t *l_tx_hash = dap_chain_mempool_base_tx_create(
            obj_chain->chain_t, obj_emi_hash->hash_fast,
            obj_emi_chain->chain_t->id, obj_emission_value->value, l_ticker,
            obj_addr_to->addr, l_certs, l_certs_count);
    DAP_FREE(l_certs);
    if (l_tx_hash == NULL) {
        Py_RETURN_NONE;
    }
    PyDapHashFastObject *l_obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    l_obj_hf->hash_fast = l_tx_hash;
    l_obj_hf->origin = true;
    return (PyObject*)l_obj_hf;
}

PyObject *dap_chain_mempool_tx_create_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_chain;
    PyObject *obj_key_from;
    PyObject *obj_addr_from;
    PyObject *obj_addr_to;
    char *l_token_ticker;
    char * l_value;
    char * l_value_fee;
    if (!PyArg_ParseTuple(args, "OOOOsss", &obj_chain, &obj_key_from, &obj_addr_from, &obj_addr_to,
                          &l_token_ticker, &l_value, &l_value_fee)){
        return NULL;
    }
    dap_chain_t *l_chain = ((PyDapChainObject*)obj_chain)->chain_t;
    dap_enc_key_t *l_key_from = ((PyCryptoKeyObject*)obj_key_from)->key;
    dap_chain_addr_t *l_addr_from = ((PyDapChainAddrObject*)obj_addr_from)->addr;
    dap_chain_addr_t *l_addr_to = ((PyDapChainAddrObject*)obj_addr_to)->addr;
    uint256_t l_value_256 = dap_chain_balance_scan(l_value);
    uint256_t l_value_fee_256 = dap_chain_balance_scan(l_value_fee);
    dap_chain_hash_fast_t  *l_hash_tx = dap_chain_mempool_tx_create(l_chain, l_key_from,
                                                                    l_addr_from, l_addr_to,
                                                                    l_token_ticker,
                                                                    l_value_256, l_value_fee_256);
    if (l_hash_tx == NULL){
        Py_RETURN_NONE;
    } else {
        PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
        obj_hf->hash_fast = l_hash_tx;
        obj_hf->origin = true;
        return (PyObject*)obj_hf;
    }
}

PyObject *dap_chain_mempool_tx_create_cond_py(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainNetObject *obj_net;
    PyObject* obj_key_from;
    PyObject* obj_key_cond;
    char *l_token_ticker;
    char* l_value;
    char* l_value_per_unit_max;
    PyObject *obj_unit;
    PyObject *obj_srv_uid;
    char* l_fee;
    PyObject *obj_cond;
    if (!PyArg_ParseTuple(args, "OOOsssOOsO", &obj_net, &obj_key_from, &obj_key_cond, &l_token_ticker, &l_value,
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
    uint256_t l_value_256 = dap_chain_balance_scan(l_value);
    uint256_t l_value_per_unit_max_256 = dap_chain_balance_scan(l_value_per_unit_max);
    uint256_t l_fee_256  = dap_chain_balance_scan(l_fee);
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
        Py_RETURN_NONE;
    }
    PyDapHashFastObject *l_obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    l_obj_hf->hash_fast = l_hf;
    l_obj_hf->origin = true;
    return (PyObject*)l_obj_hf;
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
        Py_RETURN_NONE;
    }
    PyDapHashFastObject *l_obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    l_obj_hf->hash_fast = l_hf;
    l_obj_hf->origin = true;
    return (PyObject*)l_obj_hf;
}

PyObject *dap_chain_mempool_remove_py(PyObject *self, PyObject *args){
    PyDapChainObject *obj_chain;
    char *l_str_hash = NULL;
    if (!PyArg_ParseTuple(args, "Os", &obj_chain, &l_str_hash)){
        return NULL;
    }
    if (!PyDapChain_Check(obj_chain)){
        PyErr_SetString(PyExc_AttributeError, "The first argument was not correctly passed to "
                                              "the function, the first argument must be an instance of an object of "
                                              "type Chain.");
        return NULL;
    }
    if (!obj_chain->chain_t){
        PyErr_SetString(PyExc_AttributeError, "The passed chain arguments are corrupted.");
        return NULL;
    }
    char *l_gdb_group_mempool = dap_chain_net_get_gdb_group_mempool(obj_chain->chain_t);
    uint8_t *l_data_tmp = l_str_hash ? dap_chain_global_db_gr_get(l_str_hash, NULL, l_gdb_group_mempool) : NULL;
    if(l_data_tmp && dap_chain_global_db_gr_del(l_str_hash, l_gdb_group_mempool)) {
        DAP_DELETE(l_gdb_group_mempool);
        DAP_DELETE(l_data_tmp);
        Py_RETURN_TRUE;
    } else {
        DAP_DELETE(l_gdb_group_mempool);
        DAP_DELETE(l_data_tmp);
        Py_RETURN_FALSE;
    }
}

PyObject* pvt_dap_chain_mempool_list(dap_chain_t *a_chain){
    PyObject *obj_dict = PyDict_New();
    char * l_gdb_group_mempool = dap_chain_net_get_gdb_group_mempool(a_chain);
    if (l_gdb_group_mempool){
        size_t l_objs_size = 0;
        dap_global_db_obj_t * l_objs = dap_chain_global_db_gr_load(l_gdb_group_mempool, &l_objs_size);
        for (size_t i = 0; i < l_objs_size; i++){
            dap_chain_datum_t * l_datum = (dap_chain_datum_t*) l_objs[i].value;
            PyDapChainDatumObject *obj_datum = PyObject_New(PyDapChainDatumObject, &DapChainDatumObjectType);
            obj_datum->datum = DAP_DUP_SIZE((dap_chain_datum_t*) l_objs[i].value, l_objs[i].value_len);
            obj_datum->origin = true;
            PyDict_SetItemString(obj_dict, l_objs[i].key, (PyObject*)obj_datum);
            Py_XDECREF((PyObject*)obj_datum);
        }
        dap_chain_global_db_objs_delete(l_objs, l_objs_size);
    }
    DAP_FREE(l_gdb_group_mempool);
    return obj_dict;
}

PyObject *dap_chain_mempool_list_py(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainNetObject *obj_net;
    PyDapChainObject *obj_chain = NULL;
    if (!PyArg_ParseTuple(args, "O|O", &obj_net, &obj_chain)){
        return NULL;
    }
    if (!PyDapChainNet_Check(obj_net)){
        PyErr_SetString(PyExc_AttributeError, "The first argument was passed to the function incorrectly,"
                                              " the first argument must be an instance of an object of type ChainNet.");
        return NULL;
    }
    if (!obj_chain){
        dap_chain_t *l_chain_tmp;
        PyObject *obj_list = PyList_New(0);
        DL_FOREACH(obj_net->chain_net->pub.chains, l_chain_tmp){
            PyObject *obj_list_datum_from_chain = pvt_dap_chain_mempool_list(l_chain_tmp);
            for (int i=0; i < PyList_Size(obj_list_datum_from_chain); i++){
                PyList_Append(obj_list, PyList_GetItem(obj_list_datum_from_chain, i));
            }
        }
        return obj_list;
    }else{
        if (!PyDapChain_Check(obj_chain)){
            PyErr_SetString(PyExc_AttributeError, "The second argument was passed to the function incorrectly, the "
                                                  "second argument must be an instance of an object of type Chain.");
            return NULL;
        }
        PyObject *obj_list = pvt_dap_chain_mempool_list(obj_chain->chain_t);
        return obj_list;
    }
}

PyObject *dap_chain_mempool_add_datum_py(PyObject *self, PyObject *args){
    (void)self;
    PyDapChainDatumObject *obj_datum;
    PyDapChainObject *obj_chain;
    if (!PyArg_ParseTuple(args, "OO", &obj_chain, &obj_datum)){
        return NULL;
    }
    if (!PyDapChain_Check(obj_chain)){
        PyErr_SetString(PyExc_AttributeError, "The first argument was not passed correctly. "
                                              "The first argument must be instance of an object of type Chain.");
        return NULL;
    }
    if (!PyDapChainDatum_Check(obj_datum)){
        PyErr_SetString(PyExc_AttributeError, "The second argument was not passed correctly. "
                                              "The second argument must be instance of an object of type Datum.");
        return NULL;
    }
    char *l_str = dap_chain_mempool_datum_add(obj_datum->datum, obj_chain->chain_t);
    if (!l_str)
        return Py_None;
    obj_datum->origin = false;
    PyObject *l_obj_ret = Py_BuildValue("s", l_str);
    DAP_DELETE(l_str);
    return l_obj_ret;
}
