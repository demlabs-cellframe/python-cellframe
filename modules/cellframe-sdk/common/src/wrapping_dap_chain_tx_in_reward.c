#include "wrapping_dap_chain_tx_in_reward.h"

#define LOG_TAG "wrapping_dap_chain_tx_in_reward"

static PyGetSetDef DapChainTxInRewardGetsSetsDef[] = {
    {"blockHash", (getter)wrapping_dap_chain_tx_in_reward_get_block_hash, NULL, NULL, NULL},
    {0}
};

PyTypeObject DapChainTxInRewardObjectType = DAP_PY_TYPE_OBJECT(
    "CellFrame.ChainTxInReward", sizeof(PyDapChainTxInRewardObject),
    "Chain tx in-reward object",
    .tp_getset = DapChainTxInRewardGetsSetsDef);

PyObject *wrapping_dap_chain_tx_in_reward_get_block_hash(PyObject *self, void *closure)
{
    (void)closure;
    PyDapHashFastObject *obj_hash = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    if (!obj_hash) {
        log_it(L_CRITICAL, "Failed to create hash fast object for in_reward");
        return NULL;
    }
    obj_hash->hash_fast = (dap_chain_hash_fast_t *)&((PyDapChainTxInRewardObject *)self)->tx_in_reward->block_hash;
    obj_hash->origin = false;
    return (PyObject *)obj_hash;
}
