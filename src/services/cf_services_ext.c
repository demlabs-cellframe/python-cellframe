#include "cellframe.h"
#include "cf_services_ext.h"

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

PyMethodDef* cellframe_services_ext_get_methods(void) {
    static PyMethodDef methods[] = {
        // Base services
        {"net_srv_app_db_init", (PyCFunction)dap_chain_net_srv_app_db_init_py, METH_VARARGS, "Init app-db service"},
        {"net_srv_app_db_deinit", (PyCFunction)dap_chain_net_srv_app_db_deinit_py, METH_VARARGS, "Deinit app-db service"},
        {"net_srv_app_init", (PyCFunction)dap_chain_net_srv_app_init_py, METH_VARARGS, "Init app service"},
        {"net_srv_app_deinit", (PyCFunction)dap_chain_net_srv_app_deinit_py, METH_VARARGS, "Deinit app service"},
        {"net_srv_bridge_init", (PyCFunction)dap_chain_net_srv_bridge_init_py, METH_VARARGS, "Init bridge service"},
        {"net_srv_bridge_deinit", (PyCFunction)dap_chain_net_srv_bridge_deinit_py, METH_VARARGS, "Deinit bridge service"},
        {"net_srv_datum_init", (PyCFunction)dap_chain_net_srv_datum_init_py, METH_VARARGS, "Init datum service"},
        {"net_srv_datum_deinit", (PyCFunction)dap_chain_net_srv_datum_deinit_py, METH_VARARGS, "Deinit datum service"},
        {"net_srv_file_datum_data_read", (PyCFunction)dap_chain_net_srv_file_datum_data_read_py, METH_VARARGS, "Read datum data from file"},
        {"net_srv_datum_custom_add", (PyCFunction)dap_chain_net_srv_datum_custom_add_py, METH_VARARGS, "Add custom datum to mempool"},

        // Stake-Ext
        {"net_srv_stake_ext_init", (PyCFunction)dap_chain_net_srv_stake_ext_init_py, METH_VARARGS, "Init stake_ext service"},
        {"net_srv_stake_ext_deinit", (PyCFunction)dap_chain_net_srv_stake_ext_deinit_py, METH_VARARGS, "Deinit stake_ext service"},
        {"net_srv_stake_ext_find", (PyCFunction)dap_chain_net_srv_stake_ext_find_py, METH_VARARGS, "Find stake_ext item"},
        {"net_srv_stake_ext_get_list", (PyCFunction)dap_chain_net_srv_stake_ext_get_list_py, METH_VARARGS, "Get stake_ext list"},
        {"net_srv_stake_ext_get_detailed", (PyCFunction)dap_chain_net_srv_stake_ext_get_detailed_py, METH_VARARGS, "Get detailed stake_ext info"},
        {"net_srv_stake_ext_get_stats", (PyCFunction)dap_chain_net_srv_stake_ext_get_stats_py, METH_VARARGS, "Get stake_ext statistics"},
        {"net_srv_stake_ext_lock_create", (PyCFunction)dap_chain_net_srv_stake_ext_lock_create_py, METH_VARARGS, "Create lock tx for stake_ext"},
        {"net_srv_stake_ext_unlock_create", (PyCFunction)dap_chain_net_srv_stake_ext_unlock_create_py, METH_VARARGS, "Create unlock tx for stake_ext"},
        {"net_srv_stake_ext_delete", (PyCFunction)dap_chain_net_srv_stake_ext_delete_py, METH_VARARGS, "Delete stake_ext item"},

        // Stake-Ext helpers + cache
        {"net_srv_stake_ext_status_to_str", (PyCFunction)dap_chain_srv_stake_ext_status_to_str_py, METH_VARARGS, "Stake_ext status to string"},
        {"net_srv_stake_ext_status_from_event_type", (PyCFunction)dap_chain_srv_stake_ext_status_from_event_type_py, METH_VARARGS, "Stake_ext status from event type"},
        {"net_srv_tx_event_data_time_unit_to_str", (PyCFunction)dap_chain_tx_event_data_time_unit_to_str_py, METH_VARARGS, "Time unit to string"},
        {"net_srv_stake_ext_started_tx_event_create", (PyCFunction)dap_chain_srv_stake_ext_started_tx_event_create_py, METH_VARARGS, "Create stake_ext started event data"},
        {"net_srv_stake_ext_ended_tx_event_create", (PyCFunction)dap_chain_srv_stake_ext_ended_tx_event_create_py, METH_VARARGS, "Create stake_ext ended event data"},

        // Stake core + pos delegate + tx builders
        {"net_srv_stake_init", (PyCFunction)dap_chain_net_srv_stake_init_py, METH_VARARGS, "Init stake service"},
        {"net_srv_stake_deinit", (PyCFunction)dap_chain_net_srv_stake_deinit_py, METH_VARARGS, "Deinit stake service"},
        {"net_srv_stake_pos_delegate_init", (PyCFunction)dap_chain_net_srv_stake_pos_delegate_init_py, METH_VARARGS, "Init stake pos delegate"},
        {"net_srv_stake_pos_delegate_deinit", (PyCFunction)dap_chain_net_srv_stake_pos_delegate_deinit_py, METH_VARARGS, "Deinit stake pos delegate"},
        {"net_srv_stake_decree_init", (PyCFunction)dap_chain_net_srv_stake_decree_init_py, METH_VARARGS, "Init stake decree handlers"},
        {"net_srv_stake_decree_deinit", (PyCFunction)dap_chain_net_srv_stake_decree_deinit_py, METH_VARARGS, "Deinit stake decree handlers"},
        {"net_srv_stake_key_delegate", (PyCFunction)dap_chain_net_srv_stake_key_delegate_py, METH_VARARGS, "Delegate stake key"},
        {"net_srv_stake_key_invalidate", (PyCFunction)dap_chain_net_srv_stake_key_invalidate_py, METH_VARARGS, "Invalidate stake key"},
        {"net_srv_stake_key_update", (PyCFunction)dap_chain_net_srv_stake_key_update_py, METH_VARARGS, "Update stake key"},
        {"net_srv_stake_pkey_update", (PyCFunction)dap_chain_net_srv_stake_pkey_update_py, METH_VARARGS, "Update stake pkey"},
        {"net_srv_stake_set_allowed_min_value", (PyCFunction)dap_chain_net_srv_stake_set_allowed_min_value_py, METH_VARARGS, "Set stake min value"},
        {"net_srv_stake_get_allowed_min_value", (PyCFunction)dap_chain_net_srv_stake_get_allowed_min_value_py, METH_VARARGS, "Get stake min value"},
        {"net_srv_stake_set_percent_max", (PyCFunction)dap_chain_net_srv_stake_set_percent_max_py, METH_VARARGS, "Set stake percent max"},
        {"net_srv_stake_get_percent_max", (PyCFunction)dap_chain_net_srv_stake_get_percent_max_py, METH_VARARGS, "Get stake percent max"},
        {"net_srv_stake_key_delegated", (PyCFunction)dap_chain_net_srv_stake_key_delegated_py, METH_VARARGS, "Check if stake key delegated"},
        {"net_srv_stake_verify_key_and_node", (PyCFunction)dap_chain_net_srv_stake_verify_key_and_node_py, METH_VARARGS, "Verify stake key and node"},
        {"net_srv_stake_get_validators", (PyCFunction)dap_chain_net_srv_stake_get_validators_py, METH_VARARGS, "Get stake validators"},
        {"net_srv_stake_get_fee_validators", (PyCFunction)dap_chain_net_srv_stake_get_fee_validators_py, METH_VARARGS, "Get stake validators fee stats"},
        {"net_srv_stake_load_cache", (PyCFunction)dap_chain_net_srv_stake_load_cache_py, METH_VARARGS, "Load stake cache"},
        {"net_srv_stake_check_validator", (PyCFunction)dap_chain_net_srv_stake_check_validator_py, METH_VARARGS, "Check stake validator"},
        {"net_srv_stake_decree_approve", (PyCFunction)dap_chain_net_srv_stake_decree_approve_py, METH_VARARGS, "Approve stake decree"},
        {"net_srv_stake_mark_validator_active", (PyCFunction)dap_chain_net_srv_stake_mark_validator_active_py, METH_VARARGS, "Set validator active flag"},
        {"net_srv_stake_check_pkey_hash", (PyCFunction)dap_chain_net_srv_stake_check_pkey_hash_py, METH_VARARGS, "Check stake pkey hash"},
        {"net_srv_stake_get_total_weight", (PyCFunction)dap_chain_net_srv_stake_get_total_weight_py, METH_VARARGS, "Get total stake weight"},
        {"net_srv_stake_get_total_keys", (PyCFunction)dap_chain_net_srv_stake_get_total_keys_py, METH_VARARGS, "Get total stake keys"},
        {"net_srv_stake_add_approving_decree_info", (PyCFunction)dap_chain_net_srv_stake_add_approving_decree_info_py, METH_VARARGS, "Add approving decree info"},
        {"net_srv_stake_remove_approving_decree_info", (PyCFunction)dap_chain_net_srv_stake_remove_approving_decree_info_py, METH_VARARGS, "Remove approving decree info"},
        {"net_srv_stake_hardfork_data_export", (PyCFunction)dap_chain_net_srv_stake_hardfork_data_export_py, METH_VARARGS, "Export stake hardfork data"},
        {"net_srv_stake_hardfork_data_import", (PyCFunction)dap_chain_net_srv_stake_hardfork_data_import_py, METH_VARARGS, "Import stake hardfork data"},
        {"net_srv_stake_hardfork_data_verify", (PyCFunction)dap_chain_net_srv_stake_hardfork_data_verify_py, METH_VARARGS, "Verify stake hardfork data"},
        {"net_srv_stake_switch_table", (PyCFunction)dap_chain_net_srv_stake_switch_table_py, METH_VARARGS, "Switch stake table"},
        {"net_srv_stake_get_pkey_by_hash", (PyCFunction)dap_chain_net_srv_stake_get_pkey_by_hash_py, METH_VARARGS, "Get stake pkey by hash"},
        {"net_srv_stake_hardfork_tx_update", (PyCFunction)dap_chain_net_srv_stake_hardfork_tx_update_py, METH_VARARGS, "Update stake hardfork tx"},
        {"net_srv_stake_get_validator_ext", (PyCFunction)dap_chain_net_srv_stake_get_validator_ext_py, METH_VARARGS, "Get validator ext data"},
        {"net_srv_stake_tx_builders_register", (PyCFunction)dap_chain_net_srv_stake_tx_builders_register_py, METH_VARARGS, "Register stake tx builders"},
        {"net_srv_stake_tx_builders_unregister", (PyCFunction)dap_chain_net_srv_stake_tx_builders_unregister_py, METH_VARARGS, "Unregister stake tx builders"},
        {"net_srv_stake_tx_create_lock", (PyCFunction)dap_chain_net_srv_stake_tx_create_lock_py, METH_VARARGS, "Create stake lock tx"},
        {"net_srv_stake_tx_create_unlock", (PyCFunction)dap_chain_net_srv_stake_tx_create_unlock_py, METH_VARARGS, "Create stake unlock tx"},
        {"net_srv_stake_tx_create_delegate", (PyCFunction)dap_chain_net_srv_stake_tx_create_delegate_py, METH_VARARGS, "Create stake delegate tx"},
        {"net_srv_stake_tx_create_invalidate", (PyCFunction)dap_chain_net_srv_stake_tx_create_invalidate_py, METH_VARARGS, "Create stake invalidate tx"},
        
        // Xchange
        {"net_srv_xchange_init", (PyCFunction)dap_chain_net_srv_xchange_init_py, METH_VARARGS, "Init Xchange service"},
        {"net_srv_xchange_deinit", (PyCFunction)dap_chain_net_srv_xchange_deinit_py, METH_VARARGS, "Deinit Xchange service"},
        {"net_srv_xchange_create", (PyCFunction)dap_chain_net_srv_xchange_create_py, METH_VARARGS, "Create exchange order"},
        {"net_srv_xchange_remove", (PyCFunction)dap_chain_net_srv_xchange_remove_py, METH_VARARGS, "Remove exchange order"},
        {"net_srv_xchange_get_fee", (PyCFunction)dap_chain_net_srv_xchange_get_fee_py, METH_VARARGS, "Get exchange fee"},
        {"net_srv_xchange_get_order_completion_rate", (PyCFunction)dap_chain_net_srv_xchange_get_order_completion_rate_py, METH_VARARGS, "Get order completion rate"},
        {"net_srv_xchange_purchase", (PyCFunction)dap_chain_net_srv_xchange_purchase_py, METH_VARARGS, "Purchase exchange order"},
        {"net_srv_xchange_get_tx_xchange", (PyCFunction)dap_chain_net_srv_xchange_get_tx_xchange_py, METH_VARARGS, "Get exchange transactions"},
        {"net_srv_xchange_get_prices", (PyCFunction)dap_chain_net_srv_xchange_get_prices_py, METH_VARARGS, "Get exchange prices"},
        {"net_srv_xchange_get_order_status", (PyCFunction)dap_chain_net_srv_xchange_get_order_status_py, METH_VARARGS, "Get order status"},
        {"net_srv_xchange_print_fee", (PyCFunction)dap_chain_net_srv_xchange_print_fee_py, METH_VARARGS, "Print exchange fee"},
        {"net_srv_xchange_print_fee_json", (PyCFunction)dap_chain_net_srv_xchange_print_fee_json_py, METH_VARARGS, "Print exchange fee JSON"},
        {"net_srv_xchange_tx_get_type", (PyCFunction)dap_chain_net_srv_xchange_tx_get_type_py, METH_VARARGS, "Get exchange tx type"},
        {"net_srv_xchange_compose_init", (PyCFunction)dap_chain_net_srv_xchange_compose_init_py, METH_VARARGS, "Init xchange compose service"},
        {"net_srv_xchange_compose_deinit", (PyCFunction)dap_chain_net_srv_xchange_compose_deinit_py, METH_VARARGS, "Deinit xchange compose service"},
        {"net_srv_tx_compose_xchange_create", (PyCFunction)dap_chain_tx_compose_xchange_create_py, METH_VARARGS, "Compose xchange order create"},
        {"net_srv_tx_compose_xchange_order_remove", (PyCFunction)dap_chain_tx_compose_xchange_order_remove_py, METH_VARARGS, "Compose xchange order remove"},
        {"net_srv_tx_compose_xchange_purchase", (PyCFunction)dap_chain_tx_compose_xchange_purchase_py, METH_VARARGS, "Compose xchange purchase"},
        {"net_srv_xchange_find_last_tx", (PyCFunction)dap_xchange_find_last_tx_py, METH_VARARGS, "Find last xchange tx"},
        {"net_srv_xchange_price_from_order", (PyCFunction)dap_xchange_price_from_order_py, METH_VARARGS, "Create xchange price from order"},
        {"net_srv_xchange_tx_create_order", (PyCFunction)dap_xchange_tx_create_order_py, METH_VARARGS, "Create xchange order tx (unsigned)"},
        {"net_srv_xchange_tx_create_invalidate", (PyCFunction)dap_xchange_tx_create_invalidate_py, METH_VARARGS, "Create xchange invalidate tx (unsigned)"},
        {"net_srv_xchange_tx_create_purchase", (PyCFunction)dap_xchange_tx_create_purchase_py, METH_VARARGS, "Create xchange purchase tx (unsigned)"},
        
        // Voting
        {"net_srv_voting_init", (PyCFunction)dap_chain_net_srv_voting_init_py, METH_VARARGS, "Init voting service"},
        {"net_srv_voting_deinit", (PyCFunction)dap_chain_net_srv_voting_deinit_py, METH_VARARGS, "Deinit voting service"},
        {"net_srv_voting_create", (PyCFunction)dap_chain_net_srv_voting_create_py, METH_VARARGS, "Create voting"},
        {"net_srv_voting_get_expiration_time", (PyCFunction)dap_chain_net_srv_voting_get_expiration_time_py, METH_VARARGS, "Get voting expiration time"},
        {"net_srv_voting_get_result", (PyCFunction)dap_chain_net_srv_voting_get_result_py, METH_VARARGS, "Get voting results"},
        {"net_srv_vote_create", (PyCFunction)dap_chain_net_srv_vote_create_py, METH_VARARGS, "Create vote"},
        {"net_srv_voting_list", (PyCFunction)dap_chain_net_voting_list_py, METH_VARARGS, "List votings"},
        {"net_srv_voting_extract_info", (PyCFunction)dap_chain_net_voting_extract_info_py, METH_VARARGS, "Get voting info"},
        {"net_srv_voting_info_free", (PyCFunction)dap_chain_net_voting_info_free_py, METH_VARARGS, "Free voting info"},
        {"net_srv_get_options_list_from_str", (PyCFunction)dap_get_options_list_from_str_py, METH_VARARGS, "Parse voting options list"},
        {"net_srv_vote_cancel", (PyCFunction)dap_chain_net_vote_cancel_py, METH_VARARGS, "Cancel vote"},
        {"net_srv_voting_compose_init", (PyCFunction)dap_chain_net_srv_voting_compose_init_py, METH_VARARGS, "Init voting compose service"},
        {"net_srv_voting_compose_deinit", (PyCFunction)dap_chain_net_srv_voting_compose_deinit_py, METH_VARARGS, "Deinit voting compose service"},
        {"net_srv_tx_compose_poll_create", (PyCFunction)dap_chain_tx_compose_poll_create_py, METH_VARARGS, "Compose voting poll create"},
        {"net_srv_tx_compose_poll_vote", (PyCFunction)dap_chain_tx_compose_poll_vote_py, METH_VARARGS, "Compose voting poll vote"},
        {"net_srv_voting_tx_create_poll", (PyCFunction)dap_voting_tx_create_poll_py, METH_VARARGS, "Create poll tx (unsigned)"},
        {"net_srv_voting_tx_create_vote", (PyCFunction)dap_voting_tx_create_vote_py, METH_VARARGS, "Create vote tx (unsigned)"},

        {NULL, NULL, 0, NULL}
    };
    return methods;
}
