#include "wrapping_dap_chain_cs_block.h"

PyGetSetDef DapChainCsBlockGetsSetsDef[] = {
        {"hash", (getter)wrapping_dap_chain_block_get_hash, NULL, NULL, NULL},
        {"version", (getter)wrapping_dap_chain_block_get_version, NULL, NULL, NULL},
        {"cellId", (getter)wrapping_dap_chain_block_get_cell_id, NULL, NULL, NULL},
        {"chainId", (getter)wrapping_dap_chain_block_get_chain_id, NULL, NULL, NULL},
        {"created", (getter)wrapping_dap_chain_block_get_ts_created, NULL, NULL, NULL},
        {"metaData", (getter)wrapping_dap_chain_block_get_meta_data, NULL, NULL, NULL},
        {"datums", (getter)wrapping_dap_chain_block_get_datums, NULL, NULL, NULL},
        {"signs", (getter)wrapping_dap_chain_block_get_signs, NULL, NULL, NULL},
//        {"blockCache", (getter)wrapping_dap_chain_block_get_block_cache, NULL, NULL, NULL},
        {NULL}
};

PyMethodDef DapChainCsBlockMethods[] = {
        {"fromAtom", dap_chain_cs_block_get_atom_ptr, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainCsBlockType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Consensus.ChainCSBlock",                                            /* tp_name */
        sizeof(PyDapChainCSBlockObject),                                     /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        0,                         /* tp_dealloc */
        0,                                                            /* tp_print */
        0,                                                            /* tp_getattr */
        0,                                                            /* tp_setattr */
        0,                                                            /* tp_reserved */
        0,                                                            /* tp_repr */
        0,                                                            /* tp_as_number */
        0,                                                            /* tp_as_sequence */
        0,                                                            /* tp_as_mapping */
        0,                                                            /* tp_hash  */
        0,                                                            /* tp_call */
        0,                                                            /* tp_str */
        0,                                                            /* tp_getattro */
        0,                                                            /* tp_setattro */
        0,                                                            /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                                      /* tp_flags */
        "Chain cs block objects",                                              /* tp_doc */
        0,                                                              /* tp_traverse */
        0,                                                              /* tp_clear */
        0,                                                              /* tp_richcompare */
        0,                                                              /* tp_weaklistoffset */
        0,                                                              /* tp_iter */
        0,                                                            /* tp_iternext */
        DapChainCsBlockMethods,                                        /* tp_methods */
        0,                                                          /* tp_members */
        DapChainCsBlockGetsSetsDef,                                   /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */
};

PyObject *wrapping_dap_chain_block_get_version(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("i", ((PyDapChainCSBlockObject*)self)->block->hdr.version);
}
PyObject *wrapping_dap_chain_block_get_cell_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainCellIDObject *obj_cell_id = PyObject_New(PyDapChainCellIDObject, &DapChainCellIdObjectType);
    obj_cell_id->cell_id = ((PyDapChainCSBlockObject*)self)->block->hdr.cell_id;
    return (PyObject*)obj_cell_id;
}
PyObject *wrapping_dap_chain_block_get_chain_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainIDObject *obj_chain_id = PyObject_New(PyDapChainIDObject, &DapChainIdObjectType);
    obj_chain_id->chain_id = &((PyDapChainCSBlockObject*)self)->block->hdr.chain_id;
    return (PyObject*)obj_chain_id;
}
PyObject *wrapping_dap_chain_block_get_ts_created(PyObject *self, void *closure){
    (void)closure;
    PyDateTime_IMPORT;
    PyObject *l_obj_long_ts = PyLong_FromLong((long)((PyDapChainCSBlockObject*)self)->block->hdr.ts_created);
    PyObject *l_obj_tuple = Py_BuildValue("(O)", l_obj_long_ts);
    PyObject *l_obj_dateTime = PyDateTime_FromTimestamp(l_obj_tuple);
    Py_XDECREF(l_obj_tuple);
    return l_obj_dateTime;
}
PyObject *wrapping_dap_chain_block_get_meta_data(PyObject *self, void *closure){
    (void)closure;
    size_t l_count = 0;
    dap_chain_block_meta_t **l_meta = dap_chain_block_get_meta(((PyDapChainCSBlockObject*)self)->block,
            ((PyDapChainCSBlockObject*)self)->block_size,
            &l_count);
    dap_chain_hash_fast_t l_block_prev_hash = {0};
    dap_chain_hash_fast_t l_block_anchor_hash = {0};
    dap_chain_hash_fast_t l_merkle = {0};
    dap_chain_hash_fast_t *l_block_links = NULL;
    size_t l_block_links_count = 0;
    bool l_is_genesis = false;
    uint64_t l_nonce = {0}, l_nonce2 = {0};
    dap_chain_block_meta_extract(
            l_meta, l_count,
            &l_block_prev_hash, &l_block_anchor_hash,
            &l_merkle, &l_block_links,
            &l_block_links_count, &l_is_genesis, &l_nonce, &l_nonce2);
    PyObject *obj_dict = PyDict_New();
    PyDapHashFastObject *l_obj_prev_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    l_obj_prev_hash->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    memcpy(l_obj_prev_hash->hash_fast, &l_block_prev_hash, sizeof(dap_chain_hash_fast_t));
    l_obj_prev_hash->origin = true;
    PyDict_SetItemString(obj_dict, "blockPrevHash", (PyObject*)l_obj_prev_hash);
    PyDapHashFastObject *l_obj_anchor_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    l_obj_anchor_hash->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    memcpy(l_obj_anchor_hash->hash_fast, &l_block_anchor_hash, sizeof(dap_chain_hash_fast_t));
    l_obj_anchor_hash->origin = true;
    PyDict_SetItemString(obj_dict, "blockAnchorHash", (PyObject*)l_obj_anchor_hash);
    PyDapHashFastObject *l_obj_merkle = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    l_obj_merkle->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    memcpy(l_obj_merkle->hash_fast, &l_merkle, sizeof(dap_chain_hash_fast_t));
    l_obj_merkle->origin = true;
    PyDict_SetItemString(obj_dict, "merkle", (PyObject*)l_obj_merkle);
    // Get List links
    PyObject *obj_block_links = PyList_New((Py_ssize_t)l_block_links_count);
    for (size_t i = 0; i < l_block_links_count; i++){
        PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
        obj_hf->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
        memcpy(obj_hf->hash_fast, &l_block_links[i], sizeof(dap_chain_hash_fast_t));
        obj_hf->origin = true;
//        obj_hf->hash_fast = &(l_block_links[i]);
    }
    PyDict_SetItemString(obj_dict, "links", obj_block_links);
    if (l_is_genesis){
        PyDict_SetItemString(obj_dict, "isGenesis", Py_True);
    } else {
        PyDict_SetItemString(obj_dict, "isGenesis", Py_False);
    }
    PyObject *obj_nonce = Py_BuildValue("k", l_nonce);
    PyDict_SetItemString(obj_dict, "nonce", obj_nonce);
    PyObject *obj_nonce2 = Py_BuildValue("k", l_nonce2);
    PyDict_SetItemString(obj_dict, "nonce2", obj_nonce2);
    return obj_dict;
}
PyObject *wrapping_dap_chain_block_get_datums(PyObject *self, void *closure){
    (void)closure;
    size_t l_count = 0;
    dap_chain_datum_t **l_datums = dap_chain_block_get_datums(
            ((PyDapChainCSBlockObject*)self)->block,
            ((PyDapChainCSBlockObject*)self)->block_size, &l_count);
    if (l_count == 0){
        Py_RETURN_NONE;
    }
    PyObject *obj_datums = PyList_New(l_count);
    for (size_t i = 0; i < l_count; i++) {
        PyDapChainDatumObject *obj_datum = PyObject_New(PyDapChainDatumObject, &DapChainDatumObjectType);
        obj_datum->datum = l_datums[i];
        obj_datum->origin = false;
        PyList_SetItem(obj_datums, (Py_ssize_t) i, (PyObject *)obj_datum);
    }
    return obj_datums;
}
PyObject *wrapping_dap_chain_block_get_signs(PyObject *self, void *closure){
    (void)closure;
    size_t l_count = dap_chain_block_get_signs_count(((PyDapChainCSBlockObject*)self)->block,
                                                     ((PyDapChainCSBlockObject*)self)->block_size);
    PyObject *obj_list = PyList_New((Py_ssize_t)l_count);
    for (size_t i =0; i < l_count; i++){
        PyDapSignObject *obj_sign = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
        obj_sign->sign = dap_chain_block_sign_get(((PyDapChainCSBlockObject*)self)->block,
                                                  ((PyDapChainCSBlockObject*)self)->block_size, i);
        PyList_SetItem(obj_list, (Py_ssize_t)i, (PyObject*)obj_sign);
    }
    return obj_list;
}

PyObject *wrapping_dap_chain_block_get_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hf->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    dap_hash_fast(((PyDapChainCSBlockObject*)self)->block,
                  ((PyDapChainCSBlockObject*)self)->block_size, obj_hf->hash_fast);
    obj_hf->origin = true;
    return (PyObject*)obj_hf;
}

PyObject* dap_chain_cs_block_get_atom_ptr(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_atom_ptr;
    size_t l_atom_size;
    if (!PyArg_ParseTuple(args, "On", &obj_atom_ptr, &l_atom_size)){
        return NULL;
    }
    PyDapChainCSBlockObject *obj_block = PyObject_New(PyDapChainCSBlockObject, &DapChainCsBlockType);
    obj_block->block = (dap_chain_block_t *)((PyChainAtomPtrObject*)obj_atom_ptr)->ptr;
    obj_block->block_size = l_atom_size;
    return (PyObject*)obj_block;
}
