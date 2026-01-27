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

// Base services: app/app-db/bridge/datum
PyObject* dap_chain_net_srv_app_db_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_app_db_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_app_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_app_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_bridge_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_bridge_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_datum_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_datum_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_file_datum_data_read_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_datum_custom_add_py(PyObject *a_self, PyObject *a_args);

// Stake-Ext
PyObject* dap_chain_net_srv_stake_ext_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_find_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_delete_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_get_list_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_get_detailed_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_get_stats_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_lock_create_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_ext_unlock_create_py(PyObject *a_self, PyObject *a_args);

// Stake-Ext helpers + cache
PyObject* dap_chain_srv_stake_ext_status_to_str_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_srv_stake_ext_status_from_event_type_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_tx_event_data_time_unit_to_str_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_srv_stake_ext_started_tx_event_create_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_srv_stake_ext_ended_tx_event_create_py(PyObject *a_self, PyObject *a_args);

// Stake core + pos delegate + tx builders
PyObject* dap_chain_net_srv_stake_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_pos_delegate_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_pos_delegate_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_decree_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_decree_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_key_delegate_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_key_invalidate_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_key_update_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_pkey_update_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_set_allowed_min_value_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_get_allowed_min_value_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_set_percent_max_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_get_percent_max_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_key_delegated_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_verify_key_and_node_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_get_validators_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_get_fee_validators_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_load_cache_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_check_validator_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_decree_approve_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_mark_validator_active_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_check_pkey_hash_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_get_total_weight_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_get_total_keys_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_add_approving_decree_info_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_remove_approving_decree_info_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_hardfork_data_export_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_hardfork_data_import_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_hardfork_data_verify_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_switch_table_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_get_pkey_by_hash_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_hardfork_tx_update_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_get_validator_ext_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_tx_builders_register_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_tx_builders_unregister_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_tx_create_lock_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_tx_create_unlock_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_tx_create_delegate_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_stake_tx_create_invalidate_py(PyObject *a_self, PyObject *a_args);

// VPN
PyObject* dap_chain_net_srv_vpn_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_vpn_deinit_py(PyObject *a_self, PyObject *a_args);

// Xchange
PyObject* dap_chain_net_srv_xchange_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_create_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_remove_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_get_fee_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_get_order_completion_rate_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_purchase_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_get_tx_xchange_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_get_prices_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_get_order_status_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_print_fee_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_print_fee_json_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_tx_get_type_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_compose_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_xchange_compose_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_tx_compose_xchange_create_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_tx_compose_xchange_order_remove_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_tx_compose_xchange_purchase_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_xchange_find_last_tx_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_xchange_price_from_order_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_xchange_tx_create_invalidate_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_xchange_tx_create_order_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_xchange_tx_create_purchase_py(PyObject *a_self, PyObject *a_args);

// Voting
PyObject* dap_chain_net_srv_voting_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_voting_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_voting_create_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_voting_get_expiration_time_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_voting_get_result_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_vote_create_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_voting_list_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_voting_extract_info_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_voting_info_free_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_get_options_list_from_str_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_vote_cancel_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_voting_compose_init_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_net_srv_voting_compose_deinit_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_tx_compose_poll_create_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_chain_tx_compose_poll_vote_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_voting_tx_create_poll_py(PyObject *a_self, PyObject *a_args);
PyObject* dap_voting_tx_create_vote_py(PyObject *a_self, PyObject *a_args);

#ifdef __cplusplus
}
#endif

#endif /* CELLFRAME_SERVICES_EXT_H */
