#ifndef CELLFRAME_NETWORK_H
#define CELLFRAME_NETWORK_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

// =========================================
// NETWORK FUNCTION DECLARATIONS
// =========================================

// Modern Network API functions (DAP SDK Binding Standards compliant)
// Rule DAP_NAMING_002: All Python binding functions have _py suffix
PyObject* dap_chain_net_core_init_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_register_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_unregister_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_by_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_by_id_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_parse_net_chain_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_id_by_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_id_parse_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_count_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_iter_start_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_iter_next_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_iterate_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_id_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_state_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_state_to_str_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_state_to_str_user_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_state_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_state_go_to_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_start_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_stop_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_native_ticker_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_init_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_deinit_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_delete_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_load_all_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_try_online_all_py(PyObject *self, PyObject *args);

// Chain access functions 
PyObject* dap_chain_net_get_chain_by_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_chain_by_id_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_chain_by_chain_type_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_default_chain_by_chain_type_py(PyObject *self, PyObject *args);
PyObject* dap_chain_get_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_get_id_py(PyObject *self, PyObject *args);

// Node info functions
PyObject* dap_chain_net_get_role_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_is_my_node_authorized_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_cur_addr_int_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_acl_idx_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_flag_sync_from_zero_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_set_flag_sync_from_zero_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_load_mode_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_set_load_skip_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_is_bridged_py(PyObject *self, PyObject *args);

// Network operations functions
PyObject* dap_chain_net_link_add_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_target_state_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_sync_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_links_establish_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_purge_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_announce_addr_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_announce_addr_all_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_tx_get_fee_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_tx_set_fee_py(PyObject *self, PyObject *args);

// GDB/JSON helper functions
PyObject* dap_chain_net_get_gdb_group_acl_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_gdb_group_nochain_new_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_gdb_group_mempool_by_chain_type_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_list_json_collect_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_states_json_collect_py(PyObject *self, PyObject *args);
PyObject* dap_chain_nets_info_json_collect_py(PyObject *self, PyObject *args);
PyObject* dap_cli_list_net_py(PyObject *self, PyObject *args);
PyObject* dap_ledger_by_net_name_py(PyObject *self, PyObject *args);

// Advanced functions with complex structures
PyObject* dap_chain_net_get_cur_cell_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_my_node_info_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_authorized_nodes_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_verify_datum_for_add_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_verify_datum_err_code_to_str_py(PyObject *self, PyObject *args);
PyObject* dap_chain_datum_list_py(PyObject *self, PyObject *args);
PyObject* dap_chain_datum_add_py(PyObject *self, PyObject *args);
PyObject* dap_chain_datum_remove_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_proc_mempool_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_mempool_cluster_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_add_auth_nodes_to_cluster_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_add_validator_to_clusters_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_remove_validator_from_clusters_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_add_reward_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_remove_last_reward_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_reward_py(PyObject *self, PyObject *args);

// Callback functions
PyObject* dap_chain_add_mempool_notify_callback_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_add_nodelist_notify_callback_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_srv_order_add_notify_callback_py(PyObject *self, PyObject *args);

// Module initialization
int cellframe_network_init(PyObject *module);

#ifdef __cplusplus
}
#endif


#endif // CELLFRAME_NETWORK_H 
