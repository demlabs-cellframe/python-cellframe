#ifndef CELLFRAME_SERVICES_EXT_H
#define CELLFRAME_SERVICES_EXT_H

#include <Python.h>
#include "dap_chain_net.h"
#include "dap_chain_wallet.h"

#ifdef __cplusplus
extern "C" {
#endif

// Module initialization
PyMethodDef* cellframe_services_ext_get_methods(void);

// Stake-Ext
PyObject* dap_chain_net_srv_stake_ext_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_find_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_get_list_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_get_detailed_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_get_stats_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_lock_create_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_unlock_create_py(PyObject *a_self, PyObject *a_args);

// VPN
PyObject* dap_chain_net_srv_vpn_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_vpn_deinit_py(PyObject *a_self, PyObject *a_args);

// Xchange
PyObject* dap_chain_net_srv_xchange_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_create_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_remove_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_purchase_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_get_tx_xchange_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_get_prices_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_get_order_status_py(PyObject *a_self, PyObject *a_args);

// Voting
PyObject* dap_chain_net_srv_voting_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_voting_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_voting_create_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_vote_create_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_voting_list_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_voting_extract_info_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_vote_cancel_py(PyObject *a_self, PyObject *a_args);

#ifdef __cplusplus
}
#endif

#endif /* CELLFRAME_SERVICES_EXT_H */

