#include "wrapping_dap_chain_cs_blocks.h"

PyMethodDef DapChainCsBlocksMethod[]  = {
        {"getBlockByhHash", (PyCFunction)wrapping_dap_chain_cs_block_get_block, METH_VARARGS | METH_STATIC, ""},
        {"getBlocks", (PyCFunction)wrapping_dap_chain_cs_block_get_list_blocks, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainCsBlocksType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Consensus.ChainCSBlocks",                                            /* tp_name */
        sizeof(PyDapChainCSBlocksObject),                                     /* tp_basicsize */
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
        "Chain cs blocks objects",                                              /* tp_doc */
        0,                                                              /* tp_traverse */
        0,                                                              /* tp_clear */
        0,                                                              /* tp_richcompare */
        0,                                                              /* tp_weaklistoffset */
        0,                                                              /* tp_iter */
        0,                                                            /* tp_iternext */
        DapChainCsBlocksMethod,                                        /* tp_methods */
        0,                                                          /* tp_members */
        0,                                                           /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */
};

PyObject *wrapping_dap_chain_cs_block_get_block(PyObject *self, PyObject *args){
    //TODO: Get a block by its hash.
    Py_RETURN_NONE;
}
PyObject *wrapping_dap_chain_cs_block_get_list_blocks(PyObject *self, PyObject *args){
    (void)args;
    //TODO: Make a wrapper for getting a list of blocks. Limited in number and offset on the page.
//    dap_chain_cs_blocks_t *l_blocks = ((PyDapChainCSBlocksObject*)self)->blocks;
//    pthread_rwlock_rdlock((dap_chain_cs_blocks_pvt_t *))
//    pthread_rwlock_rdlock(&PVT(l_blocks)->rwlock);
//    dap_string_t * l_str_tmp = dap_string_new(NULL);
//    dap_string_append_printf(l_str_tmp,"%s.%s: Have %"DAP_UINT64_FORMAT_U" blocks :\n",
//                             l_net->pub.name,l_chain->name,PVT(l_blocks)->blocks_count);
//    dap_chain_block_cache_t * l_block_cache = NULL,*l_block_cache_tmp = NULL;
//
//    HASH_ITER(hh,PVT(l_blocks)->block_cache_first,l_block_cache, l_block_cache_tmp ) {
//        char l_buf[50];
//        ctime_r((time_t *)&l_block_cache->block->hdr.ts_created, l_buf);
//        dap_string_append_printf(l_str_tmp,"\t%s: ts_create=%s",
//                                 l_block_cache->block_hash_str, l_buf);
//    }
//    pthread_rwlock_unlock(&PVT(l_blocks)->rwlock);

//    l_blocks->
    Py_RETURN_NONE;
}
