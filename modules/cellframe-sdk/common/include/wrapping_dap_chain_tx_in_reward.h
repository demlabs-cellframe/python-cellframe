/*
 * Authors:
 * DeM Labs Inc.   https://demlabs.net
 * Copyright  (c) 2025, All rights reserved.
 */

#ifndef _WRAPPING_DAP_CHAIN_TX_IN_REWARD_
#define _WRAPPING_DAP_CHAIN_TX_IN_REWARD_

#include <Python.h>
#include "dap_chain_datum_tx_in_reward.h"
#include "wrapping_dap_hash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapChainTxInReward {
    PyObject_HEAD
    dap_chain_tx_in_reward_t *tx_in_reward;
} PyDapChainTxInRewardObject;

PyObject *wrapping_dap_chain_tx_in_reward_get_block_hash(PyObject *self, void *closure);

extern PyTypeObject DapChainTxInRewardObjectType;

#ifdef __cplusplus
}
#endif

#endif /* _WRAPPING_DAP_CHAIN_TX_IN_REWARD_ */
