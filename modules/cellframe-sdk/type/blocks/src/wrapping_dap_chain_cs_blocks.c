#include "libdap-python.h"
#include "wrapping_dap_chain_cs_blocks.h"
#include "python-cellframe_common.h"
#include "wrapping_dap_hash.h"

static PyMethodDef DapChainCsBlocksMethod[]  = {
        {"getBlockByhHash", (PyCFunction)wrapping_dap_chain_cs_block_get_block, METH_VARARGS | METH_STATIC, ""},
        {"getBlocks", (PyCFunction)wrapping_dap_chain_cs_block_get_list_blocks, METH_VARARGS | METH_STATIC, ""},
        {"getBlockSignersRewards", (PyCFunction)wrapping_dap_chain_cs_block_get_block_signers_rewards, METH_VARARGS | METH_STATIC, ""},
};

PyTypeObject DapChainCsBlocksType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Consensus.ChainCSBlocks", sizeof(PyDapChainCSBlocksObject),
        "Chain cs blocks objects",
        .tp_methods = DapChainCsBlocksMethod);

PyTypeObject DapChainCSBlockRewardsType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Consensus.ChainCSBlockRewards", sizeof(PyDapChainCSBlockRewardsObject),
        "Chain cs block rewards objects");

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

PyObject *wrapping_dap_chain_cs_block_get_block_signers_rewards(PyObject *self, PyObject *args){
    
    PyDapHashFastObject *obj_block_hash = NULL;

    if (!PyArg_ParseTuple(args, "0", &obj_block_hash)){
        return NULL;
    }
    if (!PyDapHashFast_Check(obj_block_hash)) {
        PyErr_SetString(PyExc_Exception, "The first argument must be an instance of the DAP.HashFast object");
        return NULL;
    }

    dap_hash_fast_t l_block_hash = *(obj_block_hash->hash_fast);

    dap_list_t *l_rewards_list = dap_chain_cs_blocks_get_block_signers_rewards(((PyDapChainCSBlocksObject*)self)->blocks->chain, &l_block_hash);

    size_t l_rewards_list_count = dap_list_length(l_rewards_list);
    dap_list_t *tmp = l_rewards_list;
    PyObject *obj_rewards_list = PyList_New((Py_ssize_t)l_rewards_list_count);
    for (size_t i = 0; i < l_rewards_list_count; i++){
        dap_chain_cs_block_rewards_t *l_reward = (dap_chain_cs_block_rewards_t*)tmp->data;
        PyDapChainCSBlockRewardsObject *l_obj_reward = PyObject_New(PyDapChainCSBlockRewardsObject,
                                                                       &DapChainCSBlockRewardsType);
        l_obj_reward->rewards = DAP_NEW(dap_chain_cs_block_rewards_t);
        memcpy(l_obj_reward->rewards, l_reward, sizeof(dap_chain_cs_block_rewards_t));
        PyList_Append(obj_rewards_list, (PyObject*)l_obj_reward);
        Py_XDECREF(l_obj_reward);
        tmp = tmp->next;
    }
    dap_list_free(l_rewards_list);
    return obj_rewards_list;

}