# Cellframe-SDK C API Index

This document lists public C API functions declared in cellframe-sdk module headers.
Scope: `cellframe-sdk/modules/**/include/*.h` (excluding dap-sdk and 3rdparty).

## Module: chain

### cellframe-sdk/modules/chain/include/dap_chain.h
- dap_cert_chain_file_save (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_add_callback_datum_index_notify (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_add_callback_datum_removed_from_index_notify (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_add_callback_notify (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_add_callback_timer (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_atom_add_from_threshold (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_atom_confirmed_notify_add (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_atom_notify (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_atom_remove_notify (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_atom_save (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_create (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_datum_notify (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_datum_removed_notify (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_datum_type_supported_by_chain (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_deinit (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_delete (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_find_by_id (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_generation_ban (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_generation_banned (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_generation_next (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_get_atom_by_hash (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_get_atom_last_hash (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_get_atom_last_hash_num_ts (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_get_blockhain_time (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_get_cs_type (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_get_path (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_has_file_store (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_id_parse (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_info_dump_log (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_init (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_load_all (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_load_from_cfg (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_purge (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_set_cs_name (cellframe-sdk/modules/chain/include/dap_chain.h)
- dap_chain_set_cs_type (cellframe-sdk/modules/chain/include/dap_chain.h)

### cellframe-sdk/modules/chain/include/dap_chain_cell.h
- dap_chain_cell_capture_by_id (cellframe-sdk/modules/chain/include/dap_chain_cell.h)
- dap_chain_cell_close (cellframe-sdk/modules/chain/include/dap_chain_cell.h)
- dap_chain_cell_close_all (cellframe-sdk/modules/chain/include/dap_chain_cell.h)
- dap_chain_cell_create (cellframe-sdk/modules/chain/include/dap_chain_cell.h)
- dap_chain_cell_file_append (cellframe-sdk/modules/chain/include/dap_chain_cell.h)
- dap_chain_cell_find_by_id (cellframe-sdk/modules/chain/include/dap_chain_cell.h)
- dap_chain_cell_init (cellframe-sdk/modules/chain/include/dap_chain_cell.h)
- dap_chain_cell_open (cellframe-sdk/modules/chain/include/dap_chain_cell.h)
- dap_chain_cell_remit (cellframe-sdk/modules/chain/include/dap_chain_cell.h)
- dap_chain_cell_remove (cellframe-sdk/modules/chain/include/dap_chain_cell.h)
- dap_chain_cell_set_load_skip (cellframe-sdk/modules/chain/include/dap_chain_cell.h)
- dap_chain_cell_truncate (cellframe-sdk/modules/chain/include/dap_chain_cell.h)

### cellframe-sdk/modules/chain/include/dap_chain_ch.h
- dap_chain_ch_deinit (cellframe-sdk/modules/chain/include/dap_chain_ch.h)
- dap_chain_ch_init (cellframe-sdk/modules/chain/include/dap_chain_ch.h)
- dap_stream_ch_write_error_unsafe (cellframe-sdk/modules/chain/include/dap_chain_ch.h)

### cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h
- dap_chain_ch_pkt_get_size (cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h)
- dap_chain_ch_pkt_new (cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h)
- dap_chain_ch_pkt_type_to_str (cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h)
- dap_chain_ch_pkt_write (cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h)
- dap_chain_ch_pkt_write_unsafe (cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h)

### cellframe-sdk/modules/chain/include/dap_chain_net_utils.h
- dap_chain_net_get_default_chain_by_chain_type (cellframe-sdk/modules/chain/include/dap_chain_net_utils.h)

### cellframe-sdk/modules/chain/include/dap_chain_policy.h
- dap_chain_policy_activate_json_collect (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_apply (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_create_activate (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_create_deactivate (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_deinit (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_get_last_num (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_get_size (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_init (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_is_activated (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_is_exist (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_json_collect (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_list (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_net_add (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_net_purge (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_net_remove (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_to_str (cellframe-sdk/modules/chain/include/dap_chain_policy.h)
- dap_chain_policy_update_last_num (cellframe-sdk/modules/chain/include/dap_chain_policy.h)

### cellframe-sdk/modules/chain/include/dap_chain_srv.h
- dap_chain_srv_add (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_count (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_decree (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_deinit (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_delete (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_event_verify (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_fee_type_to_str (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_get_fees (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_get_internal (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_get_uid_by_name (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_hardfork_all (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_hardfork_complete_all (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_init (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_list (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_load_state (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_purge (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_purge_all (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_start (cellframe-sdk/modules/chain/include/dap_chain_srv.h)
- dap_chain_srv_start_all (cellframe-sdk/modules/chain/include/dap_chain_srv.h)

## Module: common

### cellframe-sdk/modules/common/include/dap_chain_atom.h
- dap_chain_atom_verify_res_to_str (cellframe-sdk/modules/common/include/dap_chain_atom.h)

### cellframe-sdk/modules/common/include/dap_chain_block_callbacks.h
- dap_chain_block_callbacks_deinit (cellframe-sdk/modules/common/include/dap_chain_block_callbacks.h)
- dap_chain_block_callbacks_get_sovereign_tax (cellframe-sdk/modules/common/include/dap_chain_block_callbacks.h)
- dap_chain_block_callbacks_init (cellframe-sdk/modules/common/include/dap_chain_block_callbacks.h)
- dap_chain_block_callbacks_register_sovereign_tax (cellframe-sdk/modules/common/include/dap_chain_block_callbacks.h)

### cellframe-sdk/modules/common/include/dap_chain_callback.h
- (No public functions detected)

### cellframe-sdk/modules/common/include/dap_chain_common.h
- dap_chain_addr_check_sum (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_addr_compare (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_addr_fill (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_addr_fill_from_key (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_addr_fill_from_sign (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_addr_from_str (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_addr_from_str_array (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_addr_is_blank (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_addr_to_str_static_ (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_balance_scan (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_balance_to_coins_uint64 (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_hash_slow_kind_check (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_hash_slow_to_str (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_hash_slow_to_str_new (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_mempool_group_name (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_node_role_to_str (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_set_offset_limit_json (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_srv_str_to_unit_enum (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_srv_uid_from_str (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_srv_unit_enum_to_str (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_uint128_from (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_uint128_from_uint256 (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_uint256_from (cellframe-sdk/modules/common/include/dap_chain_common.h)
- dap_chain_uint256_from_uint128 (cellframe-sdk/modules/common/include/dap_chain_common.h)

### cellframe-sdk/modules/common/include/dap_chain_net_api.h
- dap_chain_net_api_add_reward (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_by_id (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_by_name (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_datum_add_to_mempool (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_deinit (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_get_chain_by_name (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_get_chain_by_type (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_get_cur_cell (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_get_default_chain_by_type (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_get_load_mode (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_get_reward (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_init (cellframe-sdk/modules/common/include/dap_chain_net_api.h)
- dap_chain_net_api_register (cellframe-sdk/modules/common/include/dap_chain_net_api.h)

### cellframe-sdk/modules/common/include/dap_chain_net_types.h
- (No public functions detected)

### cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h
- dap_chain_rpc_callbacks_deinit (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)
- dap_chain_rpc_callbacks_init (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)
- dap_chain_rpc_callbacks_notify_consensus (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)
- dap_chain_rpc_callbacks_notify_service (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)
- dap_chain_rpc_callbacks_notify_storage (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)
- dap_chain_rpc_callbacks_notify_tx (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)
- dap_chain_rpc_callbacks_notify_wallet (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)
- dap_chain_rpc_callbacks_register_consensus (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)
- dap_chain_rpc_callbacks_register_service (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)
- dap_chain_rpc_callbacks_register_storage (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)
- dap_chain_rpc_callbacks_register_tx_notify (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)
- dap_chain_rpc_callbacks_register_wallet (cellframe-sdk/modules/common/include/dap_chain_rpc_callbacks.h)

### cellframe-sdk/modules/common/include/dap_chain_types.h
- dap_chain_type_from_str (cellframe-sdk/modules/common/include/dap_chain_types.h)
- dap_chain_type_to_datum_type (cellframe-sdk/modules/common/include/dap_chain_types.h)
- dap_chain_type_to_str (cellframe-sdk/modules/common/include/dap_chain_types.h)
- dap_datum_type_to_chain_type (cellframe-sdk/modules/common/include/dap_chain_types.h)
- dap_datum_type_to_str (cellframe-sdk/modules/common/include/dap_chain_types.h)

### cellframe-sdk/modules/common/include/dap_cli_error_codes.h
- dap_cli_error_code_get (cellframe-sdk/modules/common/include/dap_cli_error_codes.h)
- dap_cli_error_code_register (cellframe-sdk/modules/common/include/dap_cli_error_codes.h)
- dap_cli_error_codes_deinit (cellframe-sdk/modules/common/include/dap_cli_error_codes.h)
- dap_cli_error_codes_init (cellframe-sdk/modules/common/include/dap_cli_error_codes.h)

## Module: consensus

### cellframe-sdk/modules/consensus/block-pow/include/dap_chain_cs_block_pow.h
- dap_chain_cs_block_pow_deinit (cellframe-sdk/modules/consensus/block-pow/include/dap_chain_cs_block_pow.h)
- dap_chain_cs_block_pow_init (cellframe-sdk/modules/consensus/block-pow/include/dap_chain_cs_block_pow.h)

### cellframe-sdk/modules/consensus/dag-poa/include/dap_chain_type_dag_poa.h
- dap_chain_type_dag_poa_deinit (cellframe-sdk/modules/consensus/dag-poa/include/dap_chain_type_dag_poa.h)
- dap_chain_type_dag_poa_get_auth_certs (cellframe-sdk/modules/consensus/dag-poa/include/dap_chain_type_dag_poa.h)
- dap_chain_type_dag_poa_init (cellframe-sdk/modules/consensus/dag-poa/include/dap_chain_type_dag_poa.h)
- dap_chain_type_dag_poa_presign_callback_set (cellframe-sdk/modules/consensus/dag-poa/include/dap_chain_type_dag_poa.h)

### cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h
- dap_chain_cs_esbocs_deinit (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_cs_esbocs_init (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_add_block_collect (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_add_validator_to_clusters (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_get_autocollect_status (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_get_collecting_level (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_get_custom_metadata (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_get_fee (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_get_min_validators_count (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_get_sign_key (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_get_sign_pkey (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_hardfork_engaged (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_remove_validator_from_clusters (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_set_custom_metadata_callback (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_set_emergency_validator (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_set_empty_block_every_times (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_set_hardfork_complete (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_set_hardfork_prepare (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_set_hardfork_state (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_set_min_validators_count (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_set_presign_callback (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_set_signs_struct_check (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_start_timer (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_started (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)
- dap_chain_esbocs_stop_timer (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h)

### cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs_decree.h
- dap_chain_cs_esbocs_decree_deinit (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs_decree.h)
- dap_chain_cs_esbocs_decree_init (cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs_decree.h)

### cellframe-sdk/modules/consensus/include/dap_chain_block_collect.h
- (No public functions detected)

### cellframe-sdk/modules/consensus/include/dap_chain_cs.h
- dap_chain_cs_add (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_add_block_collect (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_add_validator (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_create (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_deinit (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_get_autocollect_status (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_get_callbacks (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_get_collecting_level (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_get_fee (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_get_fee_group (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_get_min_validators_count (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_get_reward_group (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_get_sign_key (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_get_sign_pkey (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_hardfork_engaged (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_init (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_is_started (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_load (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_mempool_datum_add (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_purge (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_remove_validator (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_set_callbacks (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_set_hardfork_state (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_set_min_validators_count (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_stake_check_pkey_hash (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_stake_hardfork_data_import (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_stake_switch_table (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_start (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)
- dap_chain_cs_stop (cellframe-sdk/modules/consensus/include/dap_chain_cs.h)

### cellframe-sdk/modules/consensus/include/dap_chain_cs_callbacks.h
- (No public functions detected)

### cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h
- dap_chain_validator_api_check_key_delegated (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)
- dap_chain_validator_api_deinit (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)
- dap_chain_validator_api_get_allowed_min_value (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)
- dap_chain_validator_api_get_node_addr (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)
- dap_chain_validator_api_get_pkey_by_hash (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)
- dap_chain_validator_api_get_validators (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)
- dap_chain_validator_api_get_value (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)
- dap_chain_validator_api_hardfork_tx_update (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)
- dap_chain_validator_api_init (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)
- dap_chain_validator_api_key_delegate (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)
- dap_chain_validator_api_mark_validator_active (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)
- dap_chain_validator_api_register (cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h)

## Module: datum

### cellframe-sdk/modules/datum/include/dap_chain_datum.h
- dap_chain_datum_calc_hash (cellframe-sdk/modules/datum/include/dap_chain_datum.h)
- dap_chain_datum_create (cellframe-sdk/modules/datum/include/dap_chain_datum.h)
- dap_chain_datum_dump_tx_json (cellframe-sdk/modules/datum/include/dap_chain_datum.h)
- dap_chain_datum_register_dump_anchor_callback (cellframe-sdk/modules/datum/include/dap_chain_datum.h)
- dap_chain_datum_register_dump_decree_callback (cellframe-sdk/modules/datum/include/dap_chain_datum.h)
- dap_chain_datum_size (cellframe-sdk/modules/datum/include/dap_chain_datum.h)
- dap_chain_datum_to_json (cellframe-sdk/modules/datum/include/dap_chain_datum.h)
- dap_chain_datum_type_id_to_str (cellframe-sdk/modules/datum/include/dap_chain_datum.h)
- dap_datum_token_dump_tsd_to_json (cellframe-sdk/modules/datum/include/dap_chain_datum.h)

### cellframe-sdk/modules/datum/include/dap_chain_datum_anchor.h
- dap_chain_datum_anchor_certs_dump (cellframe-sdk/modules/datum/include/dap_chain_datum_anchor.h)
- dap_chain_datum_anchor_certs_dump_json (cellframe-sdk/modules/datum/include/dap_chain_datum_anchor.h)
- dap_chain_datum_anchor_get_hash_from_data (cellframe-sdk/modules/datum/include/dap_chain_datum_anchor.h)
- dap_chain_datum_anchor_get_size (cellframe-sdk/modules/datum/include/dap_chain_datum_anchor.h)

### cellframe-sdk/modules/datum/include/dap_chain_datum_converter.h
- dap_chain_datum_from_tx (cellframe-sdk/modules/datum/include/dap_chain_datum_converter.h)

### cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h
- dap_chain_datum_decree_certs_dump (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_certs_dump_json (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_dump (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_dump_json (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_action (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_atom_num (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_ban_addr (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_empty_block_every_times (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_fee (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_fee_addr (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_hash (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_min_owners (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_node_addr (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_owners (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_pkey (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_policy (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_signature_type (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_signs (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_size (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_stake_min_signers_count (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_stake_min_value (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_stake_signing_addr (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_stake_value (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_get_value (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_new (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_sign_in_cycle (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_subtype_to_str (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_tsd_type_to_str (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)
- dap_chain_datum_decree_type_from_str (cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h)

### cellframe-sdk/modules/datum/include/dap_chain_datum_hashtree_roots.h
- (No public functions detected)

### cellframe-sdk/modules/datum/include/dap_chain_datum_service_state.h
- (No public functions detected)

### cellframe-sdk/modules/datum/include/dap_chain_datum_token.h
- dap_chain_datum_emission_add_sign (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_emission_add_tsd (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_emission_append_sign (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_emission_create (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_emission_get_signs (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_emission_get_size (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_emission_read (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_emission_type_str (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_token_certs_dump (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_token_certs_dump_to_json (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_token_check_ticker (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_token_flag_from_str (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_token_flag_to_str (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_token_flags_dump_to_json (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_token_get_delegated_ticker (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_token_is_old (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_token_read (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_datum_token_tsd_get (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)
- dap_chain_emission_get_tsd (cellframe-sdk/modules/datum/include/dap_chain_datum_token.h)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h
- dap_chain_datum_tx_add_event_item (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_fee_item (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_in_cond_item (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_in_cond_item_list (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_in_item (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_in_item_list (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_in_reward_item (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_item (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_out_cond_item (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_out_ext_item (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_out_item (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_out_std_item (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_sign (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_add_sign_item (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_delete (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_get_fee_value (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_get_sign (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_get_sign_data (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_get_size (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_group_items (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_group_items_free (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_item_get (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_item_get_nth (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_items_get (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_out_cond_get (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_verify_sign (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_datum_tx_verify_sign_all (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)
- dap_chain_node_datum_tx_calc_hash (cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h
- dap_chain_datum_tx_add_sign (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h)
- dap_chain_datum_tx_create_cond_output (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h)
- dap_chain_datum_tx_create_event (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h)
- dap_chain_datum_tx_create_from_emission (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h)
- dap_chain_datum_tx_create_multi_transfer (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h)
- dap_chain_datum_tx_create_transfer (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h)
- dap_chain_datum_tx_get_sign_data (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_event.h
- dap_chain_datum_tx_event_to_json (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_event.h)
- dap_chain_datum_tx_item_event_to_json (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_event.h)
- dap_chain_tx_item_event_type_from_str (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_event.h)
- dap_chain_tx_item_event_type_to_str (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_event.h)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_in.h
- (No public functions detected)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_in_cond.h
- (No public functions detected)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_in_ems.h
- (No public functions detected)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_in_reward.h
- (No public functions detected)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h
- dap_chain_datum_item_tx_get_size (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_event_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_get_data (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_get_tsd_by_type (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_in_cond_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_in_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_in_ems_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_in_reward_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_cond_create_fee (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_cond_create_srv_pay (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_cond_create_srv_stake (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_cond_create_srv_stake_lock (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_cond_create_srv_xchange (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_cond_create_wallet_shared (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_ext_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_out_std_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_sig_get_sign (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_sign_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_sign_create_from_sign (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_tsd_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_type_from_str_short (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_type_to_str (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_item_type_to_str_short (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_datum_tx_sign_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_tx_event_copy (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_tx_event_delete (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_tx_out_cond_subtype_from_str_short (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)
- dap_chain_tx_sig_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_out.h
- (No public functions detected)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_out_cond.h
- dap_chain_tx_out_cond_subtype_to_str (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_out_cond.h)
- dap_chain_tx_out_cond_subtype_to_str_short (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_out_cond.h)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_pkey.h
- (No public functions detected)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h
- dap_chain_datum_tx_receipt_check_size (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h)
- dap_chain_datum_tx_receipt_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h)
- dap_chain_datum_tx_receipt_sign_add (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h)
- dap_chain_datum_tx_receipt_sign_get (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h)
- dap_chain_datum_tx_receipt_signs_count (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h)
- dap_chain_datum_tx_receipt_srv_uid_get (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h)
- dap_chain_datum_tx_receipt_units_get (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h)
- dap_chain_datum_tx_receipt_utype_get (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h)
- dap_chain_datum_tx_receipt_value_get (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_sig.h
- (No public functions detected)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_tsd.h
- (No public functions detected)

### cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h
- dap_chain_datum_tx_item_vote_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_tx_item_vote_to_json (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_tx_item_voting_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_tx_item_voting_tsd_to_json (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_tx_voting_get_answer_text_by_idx (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_tx_voting_params_delete (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_tx_voting_parse_tsd (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_voting_answer_tsd_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_voting_cancel_tsd_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_voting_delegated_key_required_tsd_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_voting_expire_tsd_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_voting_max_votes_count_tsd_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_voting_question_tsd_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_voting_token_tsd_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_voting_vote_changing_allowed_tsd_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)
- dap_chain_datum_voting_vote_tx_cond_tsd_create (cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h)

### cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h
- dap_chain_decree_registry_deinit (cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h)
- dap_chain_decree_registry_has_handler (cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h)
- dap_chain_decree_registry_init (cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h)
- dap_chain_decree_registry_process (cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h)
- dap_chain_decree_registry_register_handler (cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h)
- dap_chain_decree_registry_unregister_handler (cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h)

### cellframe-sdk/modules/datum/include/dap_chain_tx.h
- dap_chain_tx_delete (cellframe-sdk/modules/datum/include/dap_chain_tx.h)
- dap_chain_tx_dup (cellframe-sdk/modules/datum/include/dap_chain_tx.h)
- dap_chain_tx_hh_add (cellframe-sdk/modules/datum/include/dap_chain_tx.h)
- dap_chain_tx_hh_find (cellframe-sdk/modules/datum/include/dap_chain_tx.h)
- dap_chain_tx_hh_free (cellframe-sdk/modules/datum/include/dap_chain_tx.h)
- dap_chain_tx_wrap_packed (cellframe-sdk/modules/datum/include/dap_chain_tx.h)

### cellframe-sdk/modules/datum/include/dap_chain_tx_sign.h
- dap_chain_tx_get_signing_data (cellframe-sdk/modules/datum/include/dap_chain_tx_sign.h)
- dap_chain_tx_sign_add (cellframe-sdk/modules/datum/include/dap_chain_tx_sign.h)

### cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h
- dap_chain_tx_compose_create (cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h)
- dap_chain_tx_compose_deinit (cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h)
- dap_chain_tx_compose_init (cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h)
- dap_chain_tx_compose_is_registered (cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h)
- dap_chain_tx_compose_register (cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h)
- dap_chain_tx_compose_unregister (cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h)

### cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_registry.h
- dap_chain_tx_compose_registry_add (cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_registry.h)
- dap_chain_tx_compose_registry_deinit (cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_registry.h)
- dap_chain_tx_compose_registry_find (cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_registry.h)
- dap_chain_tx_compose_registry_init (cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_registry.h)
- dap_chain_tx_compose_registry_remove (cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_registry.h)

## Module: governance

### cellframe-sdk/modules/governance/include/dap_chain_decree_callbacks.h
- dap_chain_decree_handler_call (cellframe-sdk/modules/governance/include/dap_chain_decree_callbacks.h)
- dap_chain_decree_handler_register (cellframe-sdk/modules/governance/include/dap_chain_decree_callbacks.h)

### cellframe-sdk/modules/governance/include/dap_chain_ledger_decree.h
- dap_chain_ledger_decree_deinit (cellframe-sdk/modules/governance/include/dap_chain_ledger_decree.h)
- dap_chain_ledger_decree_init (cellframe-sdk/modules/governance/include/dap_chain_ledger_decree.h)

## Module: ledger

### cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_cmd_registry.h
- dap_ledger_cli_cmd_execute (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_cmd_registry.h)
- dap_ledger_cli_cmd_get_categories (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_cmd_registry.h)
- dap_ledger_cli_cmd_get_commands (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_cmd_registry.h)
- dap_ledger_cli_cmd_is_registered (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_cmd_registry.h)
- dap_ledger_cli_cmd_register (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_cmd_registry.h)
- dap_ledger_cli_cmd_registry_deinit (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_cmd_registry.h)
- dap_ledger_cli_cmd_registry_init (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_cmd_registry.h)
- dap_ledger_cli_cmd_unregister (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_cmd_registry.h)

### cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_internal.h
- cli_get_ledger_by_net_name (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_internal.h)
- cli_parse_hash_out_type (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_internal.h)
- cli_parse_pagination (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_internal.h)

### cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_tx.h
- dap_chain_ledger_cli_tx_deinit (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_tx.h)
- dap_chain_ledger_cli_tx_init (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_tx.h)
- ledger_cli_tx_create (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_tx.h)
- ledger_cli_tx_create_json (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_tx.h)
- ledger_cli_tx_history (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_tx.h)
- ledger_cli_tx_verify (cellframe-sdk/modules/ledger/cli/include/dap_chain_ledger_cli_tx.h)

### cellframe-sdk/modules/ledger/include/dap_chain_ledger.h
- dap_chain_datum_dump_json (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_chain_ledger_get_tx_out_cond_linked_to_tx_in_cond (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_addr_get_token_ticker_all (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_addr_get_token_ticker_all_deprecated (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_anchor_find (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_anchor_load (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_anchor_purge (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_anchor_unload (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_anchor_unload_set_callback (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_anchor_verify (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_anchors_aggregate (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_balance_change_notifier_register (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_balance_change_notifier_unregister (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_balance_info (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_bridged_tx_notify_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_cache_enabled (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_calc_balance (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_calc_balance_full (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_chain_purge (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_check_condition_owner (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_check_error_str (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_coin_get_uncoloured_value (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_colour_clear_callback (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_count (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_count_from_to (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_create (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_create_options_default (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_datum_is_enforced (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_datum_iter_create (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_datum_iter_delete (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_datum_iter_get_first (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_datum_iter_get_last (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_datum_iter_get_next (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decree_apply (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decree_get_by_hash (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decree_get_min_num_of_signers (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decree_get_num_of_owners (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decree_get_owners_pkeys (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decree_init (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decree_load (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decree_purge (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decree_reset_applied (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decree_set_callback (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decree_verify (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_decrees_get_by_type (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_deduct_tx_tag (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_deinit (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_emission_for_stake_lock_item_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_event_find (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_event_get_list (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_event_get_list_ex (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_event_notify_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_event_pkey_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_event_pkey_check (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_event_pkey_list (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_event_pkey_rm (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_event_purge (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_events_aggregate (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_find_by_name (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_find_by_net_id (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_blockchain_time (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_chain_info (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_chain_info_by_name (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_description_by_ticker (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_final_chain_tx_hash (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_first_chain_tx_hash (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_gdb_group (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_list_tx_cond_outs (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_list_tx_outs_from_json (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_locked_values (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_name (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_net_id (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_pkey (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_pkey_hash (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_tx_cond_out (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_txs (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_get_utxo_for_value (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_handle_free (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_has_chain_id (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_init (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_is_used_reward (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_load_cache (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_load_end (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_out_cond_unspent_find_by_addr (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_purge (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_register_chain (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_service_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_blockchain_time (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_blockchain_timer (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_cache_tx_check_callback (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_decree_generic_callback (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_decree_set_fee_callback (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_fee_params (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_get_cur_cell_callback (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_mempool_callbacks (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_native_ticker (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_net_id (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_poa_keys (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_reward_removed_callback (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_syncing_state (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_wallet_cache_callbacks (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_set_wallet_callbacks (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_sign_data (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_states_aggregate (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tax_callback_set (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_threshold_info (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_add_check (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_decl_all (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_emission_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_emission_add_check (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_emission_find (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_emission_load (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_emissions_mark_hardfork (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_get_auth_pkeys_hashes (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_get_auth_signs_total (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_get_auth_signs_valid (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_get_emission_rate (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_info (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_info_by_name (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_load (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_purge (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_ticker_check (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_token_tx_item_list (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_action_str (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_action_str_to_action_t (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_add_check (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_add_notify (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_cache_find_out_cond_all (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_calculate_main_ticker_ (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_check_recipient (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_clear_colour (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_find_by_addr (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_find_by_hash (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_get_token_ticker_by_hash (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_get_trackers (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_hash_is_used_out_item (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_load (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_poa_signed (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_purge (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_remove (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_service_info (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_tag_str_by_uid (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_tx_unspent_find_by_hash (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_unregister_chain (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_verificator_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)
- dap_ledger_voting_verificator_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger.h)

### cellframe-sdk/modules/ledger/include/dap_chain_ledger_callbacks.h
- dap_ledger_callbacks_get (cellframe-sdk/modules/ledger/include/dap_chain_ledger_callbacks.h)
- dap_ledger_callbacks_register (cellframe-sdk/modules/ledger/include/dap_chain_ledger_callbacks.h)

### cellframe-sdk/modules/ledger/include/dap_chain_ledger_cli.h
- dap_chain_ledger_cli_deinit (cellframe-sdk/modules/ledger/include/dap_chain_ledger_cli.h)
- dap_chain_ledger_cli_init (cellframe-sdk/modules/ledger/include/dap_chain_ledger_cli.h)

### cellframe-sdk/modules/ledger/include/dap_chain_ledger_cli_compat.h
- (No public functions detected)

### cellframe-sdk/modules/ledger/include/dap_chain_ledger_cli_error_codes.h
- dap_chain_ledger_cli_error_codes_init (cellframe-sdk/modules/ledger/include/dap_chain_ledger_cli_error_codes.h)

### cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h
- dap_ledger_pvt_addr_check (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_balance_update_for_addr (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_cache_gdb_load_balances_callback (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_cache_gdb_load_stake_lock_callback (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_cache_gdb_load_tokens_callback (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_emission_cache_update (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_emission_item_find (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_event_remove (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_event_verify_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_find_token (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_threshold_txs_add (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_threshold_txs_proc (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_token_supply_check (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)
- dap_ledger_pvt_token_supply_check_update (cellframe-sdk/modules/ledger/include/dap_chain_ledger_pvt.h)

### cellframe-sdk/modules/ledger/include/dap_chain_ledger_tx.h
- dap_ledger_tx_builders_register (cellframe-sdk/modules/ledger/include/dap_chain_ledger_tx.h)
- dap_ledger_tx_builders_unregister (cellframe-sdk/modules/ledger/include/dap_chain_ledger_tx.h)
- dap_ledger_tx_create_event (cellframe-sdk/modules/ledger/include/dap_chain_ledger_tx.h)
- dap_ledger_tx_create_from_emission (cellframe-sdk/modules/ledger/include/dap_chain_ledger_tx.h)

### cellframe-sdk/modules/ledger/include/dap_chain_utxo.h
- (No public functions detected)

## Module: mempool

### cellframe-sdk/modules/mempool/include/dap_chain_mempool.h
- dap_chain_mempool_add_proc (cellframe-sdk/modules/mempool/include/dap_chain_mempool.h)
- dap_chain_mempool_datum_add (cellframe-sdk/modules/mempool/include/dap_chain_mempool.h)
- dap_chain_mempool_delete_callback_init (cellframe-sdk/modules/mempool/include/dap_chain_mempool.h)
- dap_chain_mempool_filter (cellframe-sdk/modules/mempool/include/dap_chain_mempool.h)
- dap_chain_mempool_group_new (cellframe-sdk/modules/mempool/include/dap_chain_mempool.h)
- dap_chain_mempool_out_is_used (cellframe-sdk/modules/mempool/include/dap_chain_mempool.h)
- dap_chain_mempool_set_fee_callback (cellframe-sdk/modules/mempool/include/dap_chain_mempool.h)
- dap_datum_mempool_init (cellframe-sdk/modules/mempool/include/dap_chain_mempool.h)

### cellframe-sdk/modules/mempool/include/dap_chain_mempool_cli.h
- com_mempool (cellframe-sdk/modules/mempool/include/dap_chain_mempool_cli.h)
- dap_chain_mempool_cli_deinit (cellframe-sdk/modules/mempool/include/dap_chain_mempool_cli.h)
- dap_chain_mempool_cli_init (cellframe-sdk/modules/mempool/include/dap_chain_mempool_cli.h)

### cellframe-sdk/modules/mempool/include/dap_chain_mempool_rpc.h
- dap_chain_mempool_rpc_handler_list (cellframe-sdk/modules/mempool/include/dap_chain_mempool_rpc.h)
- dap_chain_mempool_rpc_handler_test (cellframe-sdk/modules/mempool/include/dap_chain_mempool_rpc.h)
- dap_chain_mempool_rpc_init (cellframe-sdk/modules/mempool/include/dap_chain_mempool_rpc.h)

## Module: net

### cellframe-sdk/modules/net/core/include/dap_chain_net_core.h
- dap_chain_net_by_id (cellframe-sdk/modules/net/core/include/dap_chain_net_core.h)
- dap_chain_net_by_name (cellframe-sdk/modules/net/core/include/dap_chain_net_core.h)
- dap_chain_net_core_init (cellframe-sdk/modules/net/core/include/dap_chain_net_core.h)
- dap_chain_net_parse_net_chain (cellframe-sdk/modules/net/core/include/dap_chain_net_core.h)
- dap_chain_net_register (cellframe-sdk/modules/net/core/include/dap_chain_net_core.h)
- dap_chain_net_unregister (cellframe-sdk/modules/net/core/include/dap_chain_net_core.h)

### cellframe-sdk/modules/net/include/dap_chain_net.h
- dap_chain_add_mempool_notify_callback (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_datum_add (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_datum_list (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_datum_remove (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_add_auth_nodes_to_cluster (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_add_nodelist_notify_callback (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_add_reward (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_add_validator_to_clusters (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_announce_addr (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_announce_addr_all (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_by_id (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_by_name (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_count (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_deinit (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_delete (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_acl_idx (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_authorized_nodes (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_chain_by_chain_type (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_chain_by_id (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_chain_by_name (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_cur_addr_int (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_cur_cell (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_decrees (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_flag_sync_from_zero (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_gdb_group_acl (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_gdb_group_mempool_by_chain_type (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_gdb_group_nochain_new (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_load_mode (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_mempool_cluster (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_my_node_info (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_net_decree (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_reward (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_role (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_state (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_state_name (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_get_target_state (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_id_by_name (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_id_parse (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_init (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_is_bridged (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_is_my_node_authorized (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_iter_next (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_iter_start (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_iterate (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_link_add (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_links_dump (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_links_establish (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_list_json_collect (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_load_all (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_proc_mempool (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_purge (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_remove_last_reward (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_remove_validator_from_clusters (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_set_flag_sync_from_zero (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_set_load_skip (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_set_net_decree (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_srv_order_add_notify_callback (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_start (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_state_go_to (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_state_to_str (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_state_to_str_user (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_states_json_collect (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_stop (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_sync (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_try_online_all (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_verify_datum_err_code_to_str (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_net_verify_datum_for_add (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_chain_nets_info_json_collect (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_cli_list_net (cellframe-sdk/modules/net/include/dap_chain_net.h)
- dap_ledger_by_net_name (cellframe-sdk/modules/net/include/dap_chain_net.h)

### cellframe-sdk/modules/net/include/dap_chain_net_balancer.h
- dap_chain_net_balancer_deinit (cellframe-sdk/modules/net/include/dap_chain_net_balancer.h)
- dap_chain_net_balancer_dns_issue_link (cellframe-sdk/modules/net/include/dap_chain_net_balancer.h)
- dap_chain_net_balancer_get_node_str (cellframe-sdk/modules/net/include/dap_chain_net_balancer.h)
- dap_chain_net_balancer_handshake (cellframe-sdk/modules/net/include/dap_chain_net_balancer.h)
- dap_chain_net_balancer_http_issue_link (cellframe-sdk/modules/net/include/dap_chain_net_balancer.h)
- dap_chain_net_balancer_request (cellframe-sdk/modules/net/include/dap_chain_net_balancer.h)
- dap_chain_net_balancer_type_to_str (cellframe-sdk/modules/net/include/dap_chain_net_balancer.h)

### cellframe-sdk/modules/net/include/dap_chain_net_ch.h
- dap_chain_net_ch_deinit (cellframe-sdk/modules/net/include/dap_chain_net_ch.h)
- dap_chain_net_ch_from_session_data_extract_node_addr (cellframe-sdk/modules/net/include/dap_chain_net_ch.h)
- dap_chain_net_ch_init (cellframe-sdk/modules/net/include/dap_chain_net_ch.h)

### cellframe-sdk/modules/net/include/dap_chain_net_ch_pkt.h
- dap_chain_net_ch_pkt_write (cellframe-sdk/modules/net/include/dap_chain_net_ch_pkt.h)
- dap_chain_net_ch_pkt_write_f (cellframe-sdk/modules/net/include/dap_chain_net_ch_pkt.h)

### cellframe-sdk/modules/net/include/dap_chain_net_cli.h
- dap_chain_net_cli_deinit (cellframe-sdk/modules/net/include/dap_chain_net_cli.h)
- dap_chain_net_cli_init (cellframe-sdk/modules/net/include/dap_chain_net_cli.h)

### cellframe-sdk/modules/net/include/dap_chain_net_cli_error_codes.h
- dap_chain_net_cli_error_codes_init (cellframe-sdk/modules/net/include/dap_chain_net_cli_error_codes.h)

### cellframe-sdk/modules/net/include/dap_chain_net_decree.h
- dap_chain_net_decree_apply (cellframe-sdk/modules/net/include/dap_chain_net_decree.h)
- dap_chain_net_decree_deinit (cellframe-sdk/modules/net/include/dap_chain_net_decree.h)
- dap_chain_net_decree_get_by_hash (cellframe-sdk/modules/net/include/dap_chain_net_decree.h)
- dap_chain_net_decree_init (cellframe-sdk/modules/net/include/dap_chain_net_decree.h)
- dap_chain_net_decree_load (cellframe-sdk/modules/net/include/dap_chain_net_decree.h)
- dap_chain_net_decree_purge (cellframe-sdk/modules/net/include/dap_chain_net_decree.h)
- dap_chain_net_decree_reset_applied (cellframe-sdk/modules/net/include/dap_chain_net_decree.h)
- dap_chain_net_decree_verify (cellframe-sdk/modules/net/include/dap_chain_net_decree.h)

### cellframe-sdk/modules/net/include/dap_chain_net_fee.h
- dap_chain_net_tx_get_fee (cellframe-sdk/modules/net/include/dap_chain_net_fee.h)
- dap_chain_net_tx_set_fee (cellframe-sdk/modules/net/include/dap_chain_net_fee.h)

### cellframe-sdk/modules/net/include/dap_chain_net_node_list.h
- dap_chain_net_node_check_http_issue_link (cellframe-sdk/modules/net/include/dap_chain_net_node_list.h)
- dap_chain_net_node_list_get_gdb_group (cellframe-sdk/modules/net/include/dap_chain_net_node_list.h)
- dap_chain_net_node_list_init (cellframe-sdk/modules/net/include/dap_chain_net_node_list.h)
- dap_chain_net_node_list_request (cellframe-sdk/modules/net/include/dap_chain_net_node_list.h)
- dap_chain_node_list_ip_check (cellframe-sdk/modules/net/include/dap_chain_net_node_list.h)

### cellframe-sdk/modules/net/include/dap_chain_net_utils.h
- dap_chain_net_get_default_chain_by_chain_type (cellframe-sdk/modules/net/include/dap_chain_net_utils.h)
- dap_chain_net_tx_set_fee (cellframe-sdk/modules/net/include/dap_chain_net_utils.h)

### cellframe-sdk/modules/net/include/dap_chain_node.h
- dap_chain_node_alias_delete (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_alias_find (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_alias_register (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_cli_cmd_values_parse_net_chain_for_json (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_get_states_list_sort (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_hardfork_check (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_hardfork_confirm (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_hardfork_data_cleanup (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_hardfork_process (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_info_del (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_info_get_size (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_info_read (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_info_save (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_init (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_list_clean_init (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_mempool_autoproc_deinit (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_mempool_autoproc_init (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_mempool_need_process (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_mempool_process (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_mempool_process_all (cellframe-sdk/modules/net/include/dap_chain_node.h)
- dap_chain_node_states_info_read (cellframe-sdk/modules/net/include/dap_chain_node.h)

### cellframe-sdk/modules/net/include/dap_chain_node_rpc.h
- dap_chain_node_rpc_deinit (cellframe-sdk/modules/net/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_get_states_info_size (cellframe-sdk/modules/net/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_get_states_sort (cellframe-sdk/modules/net/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_info_del (cellframe-sdk/modules/net/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_info_save (cellframe-sdk/modules/net/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_init (cellframe-sdk/modules/net/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_is_balancer (cellframe-sdk/modules/net/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_is_my_node_authorized (cellframe-sdk/modules/net/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_is_root (cellframe-sdk/modules/net/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_list (cellframe-sdk/modules/net/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_states_info_read (cellframe-sdk/modules/net/include/dap_chain_node_rpc.h)

### cellframe-sdk/modules/net/include/dap_chain_node_sync_client.h
- dap_chain_node_sync_client_close (cellframe-sdk/modules/net/include/dap_chain_node_sync_client.h)
- dap_chain_node_sync_client_connect (cellframe-sdk/modules/net/include/dap_chain_node_sync_client.h)
- dap_chain_node_sync_client_connect_addr (cellframe-sdk/modules/net/include/dap_chain_node_sync_client.h)
- dap_chain_node_sync_client_deinit (cellframe-sdk/modules/net/include/dap_chain_node_sync_client.h)
- dap_chain_node_sync_client_init (cellframe-sdk/modules/net/include/dap_chain_node_sync_client.h)
- dap_chain_node_sync_client_is_connected (cellframe-sdk/modules/net/include/dap_chain_node_sync_client.h)
- dap_chain_node_sync_error_str (cellframe-sdk/modules/net/include/dap_chain_node_sync_client.h)
- dap_chain_node_sync_handshake (cellframe-sdk/modules/net/include/dap_chain_node_sync_client.h)
- dap_chain_node_sync_request (cellframe-sdk/modules/net/include/dap_chain_node_sync_client.h)
- dap_chain_node_sync_request_ex (cellframe-sdk/modules/net/include/dap_chain_node_sync_client.h)

### cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h
- dap_chain_net_srv_create (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)
- dap_chain_net_srv_deinit (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)
- dap_chain_net_srv_del (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)
- dap_chain_net_srv_get_price_from_order (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)
- dap_chain_net_srv_init (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)
- dap_chain_net_srv_issue_receipt (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)
- dap_chain_net_srv_parse_pricelist (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)
- dap_chain_net_srv_price_apply_from_my_order (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)
- dap_chain_net_srv_price_unit_uid_from_str (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)
- dap_chain_net_srv_price_unit_uid_to_str (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)
- dap_chain_net_srv_uid_compare (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)
- dap_chain_net_srv_uid_compare_scalar (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv.h)

### cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_ch.h
- dap_chain_net_srv_ch_create_statistic_report (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_ch.h)
- dap_chain_net_srv_ch_grace_control (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_ch.h)
- dap_chain_net_srv_ch_init (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_ch.h)

### cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_ch_pkt.h
- dap_chain_net_srv_ch_pkt_data_write (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_ch_pkt.h)
- dap_chain_net_srv_ch_pkt_data_write_f (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_ch_pkt.h)

### cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_client.h
- dap_chain_net_srv_client_check (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_client.h)
- dap_chain_net_srv_client_close (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_client.h)
- dap_chain_net_srv_client_connect (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_client.h)
- dap_chain_net_srv_client_error_str (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_client.h)
- dap_chain_net_srv_client_is_connected (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_client.h)
- dap_chain_net_srv_client_request (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_client.h)
- dap_chain_net_srv_client_write (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_client.h)

### cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_common.h
- dap_chain_net_srv_get_chain_id_by_name (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_common.h)
- dap_chain_net_srv_get_chain_id_by_type (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_common.h)
- dap_chain_net_srv_get_net_id_by_name (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_common.h)
- dap_chain_net_srv_set_net_callbacks (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_common.h)

### cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_countries.h
- (No public functions detected)

### cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_geoip.h
- chain_net_geoip_get_ip_info (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_geoip.h)
- chain_net_geoip_init (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_geoip.h)

### cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h
- dap_chain_net_srv_order_add_notify_callback (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_check (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_compose (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_continent_to_num (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_continent_to_str (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_continents_count (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_create (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_deinit (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_delete_by_hash (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_delete_by_hash_str_sync (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_dump_to_json (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_dump_to_string (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_find_all_by (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_find_by_hash (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_find_by_hash_str (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_get_common_group (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_get_continent_region (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_get_country_code (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_get_gdb_group (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_get_sign (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_get_size (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_init (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_save (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_net_srv_order_set_continent_region (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)
- dap_chain_srv_order_pin_init (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order.h)

### cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_order_callbacks.h
- (No public functions detected)

### cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_stream_session.h
- dap_chain_net_srv_stream_session_create (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_stream_session.h)
- dap_chain_net_srv_stream_session_delete (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_stream_session.h)
- dap_chain_net_srv_usage_add (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_stream_session.h)
- dap_chain_net_srv_usage_delete (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_stream_session.h)
- dap_chain_net_srv_usage_find_unsafe (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_stream_session.h)

### cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_tx.h
- dap_net_srv_tx_builders_register (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_tx.h)
- dap_net_srv_tx_builders_unregister (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_tx.h)
- dap_net_srv_tx_create_cond_input (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_tx.h)
- dap_net_srv_tx_create_cond_output (cellframe-sdk/modules/net/srv/base/include/dap_chain_net_srv_tx.h)

### cellframe-sdk/modules/net/srv/stake/include/dap_chain_net_srv_stake_common.h
- (No public functions detected)

### cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h
- dap_chain_datum_tx_spends_item_free (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_datum_tx_spends_items_free (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_get_tx_all (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_get_tx_all_from_tx (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_get_tx_by_hash (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_get_tx_cond_all_by_srv_uid (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_get_tx_cond_all_for_addr (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_get_tx_cond_all_with_spends_by_srv_uid (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_get_tx_cond_chain (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_tx_create_by_json (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_tx_deinit (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_tx_get_fee (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_tx_init (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_tx_set_fee (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_net_tx_to_json (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)
- dap_chain_tx_datum_from_json (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx.h)

### cellframe-sdk/modules/net/tx/include/dap_chain_net_tx_decree.h
- dap_chain_net_tx_decree_deinit (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx_decree.h)
- dap_chain_net_tx_decree_init (cellframe-sdk/modules/net/tx/include/dap_chain_net_tx_decree.h)

### cellframe-sdk/modules/net/tx/include/dap_chain_token_cli.h
- dap_chain_token_cli_deinit (cellframe-sdk/modules/net/tx/include/dap_chain_token_cli.h)
- dap_chain_token_cli_init (cellframe-sdk/modules/net/tx/include/dap_chain_token_cli.h)

### cellframe-sdk/modules/net/tx/include/dap_chain_token_cli_error_codes.h
- dap_chain_token_cli_error_codes_deinit (cellframe-sdk/modules/net/tx/include/dap_chain_token_cli_error_codes.h)
- dap_chain_token_cli_error_codes_init (cellframe-sdk/modules/net/tx/include/dap_chain_token_cli_error_codes.h)

### cellframe-sdk/modules/net/tx/wallet-tx/include/dap_chain_wallet_tx.h
- dap_wallet_tx_builders_register (cellframe-sdk/modules/net/tx/wallet-tx/include/dap_chain_wallet_tx.h)
- dap_wallet_tx_builders_unregister (cellframe-sdk/modules/net/tx/wallet-tx/include/dap_chain_wallet_tx.h)
- dap_wallet_tx_create_multi_transfer (cellframe-sdk/modules/net/tx/wallet-tx/include/dap_chain_wallet_tx.h)
- dap_wallet_tx_create_transfer (cellframe-sdk/modules/net/tx/wallet-tx/include/dap_chain_wallet_tx.h)

## Module: rpc

### cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h
- dap_chain_node_rpc_deinit (cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_get_states_info_size (cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_get_states_sort (cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_info_del (cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_info_save (cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_init (cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_is_balancer (cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_is_my_node_authorized (cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_is_root (cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_list (cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h)
- dap_chain_node_rpc_states_info_read (cellframe-sdk/modules/rpc/include/dap_chain_node_rpc.h)

## Module: service

### cellframe-sdk/modules/service/app-db/include/dap_chain_net_srv_app_db.h
- dap_chain_net_srv_app_db_deinit (cellframe-sdk/modules/service/app-db/include/dap_chain_net_srv_app_db.h)
- dap_chain_net_srv_app_db_init (cellframe-sdk/modules/service/app-db/include/dap_chain_net_srv_app_db.h)

### cellframe-sdk/modules/service/app/include/dap_chain_net_srv_app.h
- dap_chain_net_srv_app_deinit (cellframe-sdk/modules/service/app/include/dap_chain_net_srv_app.h)
- dap_chain_net_srv_app_init (cellframe-sdk/modules/service/app/include/dap_chain_net_srv_app.h)

### cellframe-sdk/modules/service/bridge/include/dap_chain_net_srv_bridge.h
- dap_chain_net_srv_bridge_deinit (cellframe-sdk/modules/service/bridge/include/dap_chain_net_srv_bridge.h)
- dap_chain_net_srv_bridge_init (cellframe-sdk/modules/service/bridge/include/dap_chain_net_srv_bridge.h)

### cellframe-sdk/modules/service/datum/include/dap_chain_net_srv_datum.h
- dap_chain_net_srv_datum_custom_add (cellframe-sdk/modules/service/datum/include/dap_chain_net_srv_datum.h)
- dap_chain_net_srv_datum_deinit (cellframe-sdk/modules/service/datum/include/dap_chain_net_srv_datum.h)
- dap_chain_net_srv_datum_init (cellframe-sdk/modules/service/datum/include/dap_chain_net_srv_datum.h)
- dap_chain_net_srv_file_datum_data_read (cellframe-sdk/modules/service/datum/include/dap_chain_net_srv_datum.h)

### cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h
- dap_chain_net_srv_stake_ext_deinit (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_net_srv_stake_ext_delete (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_net_srv_stake_ext_find (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_net_srv_stake_ext_get_detailed (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_net_srv_stake_ext_get_list (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_net_srv_stake_ext_get_stats (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_net_srv_stake_ext_init (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_net_srv_stake_ext_lock_create (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_net_srv_stake_ext_unlock_create (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_cache_add_lock (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_cache_add_stake_ext (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_cache_create (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_cache_delete (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_cache_event_callback (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_cache_find_lock (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_cache_find_position (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_cache_find_stake_ext (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_cache_find_stake_ext_by_name (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_cache_unlock_lock (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_cache_update_stake_ext_status (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_ended_tx_event_create (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_started_tx_event_create (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_status_from_event_type (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_srv_stake_ext_status_to_str (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)
- dap_chain_tx_event_data_time_unit_to_str (cellframe-sdk/modules/service/stake-ext/include/dap_chain_net_srv_stake_ext.h)

### cellframe-sdk/modules/service/stake/include/dap_chain_esbocs_interface.h
- dap_chain_esbocs_add_validator_to_clusters (cellframe-sdk/modules/service/stake/include/dap_chain_esbocs_interface.h)
- dap_chain_esbocs_get_min_validators_count (cellframe-sdk/modules/service/stake/include/dap_chain_esbocs_interface.h)
- dap_chain_esbocs_interface_register (cellframe-sdk/modules/service/stake/include/dap_chain_esbocs_interface.h)
- dap_chain_esbocs_interface_unregister (cellframe-sdk/modules/service/stake/include/dap_chain_esbocs_interface.h)
- dap_chain_esbocs_remove_validator_from_clusters (cellframe-sdk/modules/service/stake/include/dap_chain_esbocs_interface.h)
- dap_chain_esbocs_set_min_validators_count (cellframe-sdk/modules/service/stake/include/dap_chain_esbocs_interface.h)
- dap_chain_esbocs_started (cellframe-sdk/modules/service/stake/include/dap_chain_esbocs_interface.h)

### cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake.h
- dap_chain_net_srv_stake_deinit (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake.h)
- dap_chain_net_srv_stake_init (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake.h)

### cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_decree.h
- dap_chain_net_srv_stake_decree_deinit (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_decree.h)
- dap_chain_net_srv_stake_decree_init (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_decree.h)

### cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h
- dap_chain_net_srv_stake_add_approving_decree_info (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_check_pkey_hash (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_check_validator (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_decree_approve (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_get_allowed_min_value (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_get_fee_validators (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_get_percent_max (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_get_pkey_by_hash (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_get_total_keys (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_get_total_weight (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_get_validator_ext (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_get_validators (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_hardfork_data_export (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_hardfork_data_import (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_hardfork_data_verify (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_hardfork_tx_update (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_key_delegate (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_key_delegated (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_key_invalidate (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_key_update (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_load_cache (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_mark_validator_active (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_pkey_update (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_pos_delegate_deinit (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_pos_delegate_init (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_remove_approving_decree_info (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_set_allowed_min_value (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_set_percent_max (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_switch_table (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)
- dap_chain_net_srv_stake_verify_key_and_node (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_pos_delegate.h)

### cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_tx_builder.h
- dap_stake_tx_builders_register (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_tx_builder.h)
- dap_stake_tx_builders_unregister (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_tx_builder.h)
- dap_stake_tx_create_delegate (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_tx_builder.h)
- dap_stake_tx_create_invalidate (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_tx_builder.h)
- dap_stake_tx_create_lock (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_tx_builder.h)
- dap_stake_tx_create_unlock (cellframe-sdk/modules/service/stake/include/dap_chain_net_srv_stake_tx_builder.h)

### cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h
- dap_chain_net_srv_vote_create (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h)
- dap_chain_net_srv_voting_create (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h)
- dap_chain_net_srv_voting_deinit (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h)
- dap_chain_net_srv_voting_get_expiration_time (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h)
- dap_chain_net_srv_voting_get_result (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h)
- dap_chain_net_srv_voting_init (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h)
- dap_chain_net_vote_cancel (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h)
- dap_chain_net_voting_extract_info (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h)
- dap_chain_net_voting_info_free (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h)
- dap_chain_net_voting_list (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h)
- dap_get_options_list_from_str (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting.h)

### cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting_compose.h
- dap_chain_net_srv_voting_compose_deinit (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting_compose.h)
- dap_chain_net_srv_voting_compose_init (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting_compose.h)
- dap_chain_tx_compose_poll_create (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting_compose.h)
- dap_chain_tx_compose_poll_vote (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting_compose.h)
- dap_voting_tx_create_poll (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting_compose.h)
- dap_voting_tx_create_vote (cellframe-sdk/modules/service/voting/include/dap_chain_net_srv_voting_compose.h)

### cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h
- dap_chain_net_srv_xchange_create (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_deinit (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_get_fee (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_get_order_completion_rate (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_get_order_status (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_get_prices (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_get_tx_xchange (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_init (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_print_fee (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_print_fee_json (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_purchase (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_remove (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)
- dap_chain_net_srv_xchange_tx_get_type (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange.h)

### cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange_compose.h
- dap_chain_net_srv_xchange_compose_deinit (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange_compose.h)
- dap_chain_net_srv_xchange_compose_init (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange_compose.h)
- dap_chain_tx_compose_xchange_create (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange_compose.h)
- dap_chain_tx_compose_xchange_order_remove (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange_compose.h)
- dap_chain_tx_compose_xchange_purchase (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange_compose.h)
- dap_xchange_find_last_tx (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange_compose.h)
- dap_xchange_price_from_order (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange_compose.h)
- dap_xchange_tx_create_invalidate (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange_compose.h)
- dap_xchange_tx_create_order (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange_compose.h)
- dap_xchange_tx_create_purchase (cellframe-sdk/modules/service/xchange/include/dap_chain_net_srv_xchange_compose.h)

## Module: type

### cellframe-sdk/modules/type/blocks/include/dap_chain_block.h
- dap_chain_block_datum_add (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_datum_del_by_hash (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_deinit (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_get_datums (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_get_prev_hash (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_get_sign_offset (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_get_signs_count (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_get_size (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_init (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_meta_add (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_meta_extract (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_meta_get (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_new (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_sign_add (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_sign_get (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)
- dap_chain_block_sign_match_pkey (cellframe-sdk/modules/type/blocks/include/dap_chain_block.h)

### cellframe-sdk/modules/type/blocks/include/dap_chain_block_cache.h
- dap_chain_block_cache_deinit (cellframe-sdk/modules/type/blocks/include/dap_chain_block_cache.h)
- dap_chain_block_cache_delete (cellframe-sdk/modules/type/blocks/include/dap_chain_block_cache.h)
- dap_chain_block_cache_dup (cellframe-sdk/modules/type/blocks/include/dap_chain_block_cache.h)
- dap_chain_block_cache_init (cellframe-sdk/modules/type/blocks/include/dap_chain_block_cache.h)
- dap_chain_block_cache_new (cellframe-sdk/modules/type/blocks/include/dap_chain_block_cache.h)
- dap_chain_block_cache_update (cellframe-sdk/modules/type/blocks/include/dap_chain_block_cache.h)
- dap_chain_block_get_list_tx_cond_outs_with_val (cellframe-sdk/modules/type/blocks/include/dap_chain_block_cache.h)

### cellframe-sdk/modules/type/blocks/include/dap_chain_block_tx.h
- dap_chain_block_tx_coll_fee_create (cellframe-sdk/modules/type/blocks/include/dap_chain_block_tx.h)
- dap_chain_block_tx_coll_fee_stack_create (cellframe-sdk/modules/type/blocks/include/dap_chain_block_tx.h)
- dap_chain_block_tx_reward_create (cellframe-sdk/modules/type/blocks/include/dap_chain_block_tx.h)

### cellframe-sdk/modules/type/blocks/include/dap_chain_blocks_decree.h
- dap_chain_blocks_decree_deinit (cellframe-sdk/modules/type/blocks/include/dap_chain_blocks_decree.h)
- dap_chain_blocks_decree_init (cellframe-sdk/modules/type/blocks/include/dap_chain_blocks_decree.h)

### cellframe-sdk/modules/type/blocks/include/dap_chain_type_blocks.h
- dap_chain_block_add_fork_notificator (cellframe-sdk/modules/type/blocks/include/dap_chain_type_blocks.h)
- dap_chain_block_cache_get_by_hash (cellframe-sdk/modules/type/blocks/include/dap_chain_type_blocks.h)
- dap_chain_block_cache_get_by_number (cellframe-sdk/modules/type/blocks/include/dap_chain_type_blocks.h)
- dap_chain_cs_blocks_get_fee_group (cellframe-sdk/modules/type/blocks/include/dap_chain_type_blocks.h)
- dap_chain_cs_blocks_get_reward_group (cellframe-sdk/modules/type/blocks/include/dap_chain_type_blocks.h)
- dap_chain_type_blocks_deinit (cellframe-sdk/modules/type/blocks/include/dap_chain_type_blocks.h)
- dap_chain_type_blocks_fees_aggregate (cellframe-sdk/modules/type/blocks/include/dap_chain_type_blocks.h)
- dap_chain_type_blocks_get_block_signers_rewards (cellframe-sdk/modules/type/blocks/include/dap_chain_type_blocks.h)
- dap_chain_type_blocks_get_pkey_by_hash (cellframe-sdk/modules/type/blocks/include/dap_chain_type_blocks.h)
- dap_chain_type_blocks_init (cellframe-sdk/modules/type/blocks/include/dap_chain_type_blocks.h)

### cellframe-sdk/modules/type/dag/include/dap_chain_type_dag.h
- dap_chain_type_dag_deinit (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag.h)
- dap_chain_type_dag_find_event_by_hash (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag.h)
- dap_chain_type_dag_init (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag.h)
- dap_chain_type_dag_proc_event_round_new (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag.h)
- dap_chain_type_dag_start (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag.h)

### cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h
- dap_chain_type_dag_event_broadcast (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_calc_hash (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_calc_size (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_calc_size_excl_signs (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_gdb_get (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_gdb_set (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_get_datum (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_get_datum_size_maximum (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_get_sign (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_new (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_round_item_get_size (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_round_sign_add (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_round_sign_exists (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_sign_add (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)
- dap_chain_type_dag_event_sign_exists (cellframe-sdk/modules/type/dag/include/dap_chain_type_dag_event.h)

### cellframe-sdk/modules/type/none/include/dap_chain_type_none.h
- dap_nonconsensus_get_group (cellframe-sdk/modules/type/none/include/dap_chain_type_none.h)
- dap_nonconsensus_init (cellframe-sdk/modules/type/none/include/dap_chain_type_none.h)

## Module: wallet

### cellframe-sdk/modules/wallet/include/dap_chain_coin.h
- (No public functions detected)

### cellframe-sdk/modules/wallet/include/dap_chain_wallet.h
- dap_cert_to_addr (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_activate (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_add_wallet_created_notify (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_add_wallet_opened_notify (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_addr_cache_get_name (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_check_sign (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_close (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_create (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_create_with_pass (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_create_with_seed (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_create_with_seed_multi (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_deactivate (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_deinit (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_get_addr (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_get_certs_number (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_get_key (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_get_local_addr (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_get_path (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_get_pkey (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_get_pkey_hash (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_get_pkey_str (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_init (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_open (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_open_ext (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_open_file (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_save (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)
- dap_chain_wallet_save_file (cellframe-sdk/modules/wallet/include/dap_chain_wallet.h)

### cellframe-sdk/modules/wallet/include/dap_chain_wallet_cli_error_codes.h
- dap_chain_wallet_cli_error_codes_deinit (cellframe-sdk/modules/wallet/include/dap_chain_wallet_cli_error_codes.h)
- dap_chain_wallet_cli_error_codes_init (cellframe-sdk/modules/wallet/include/dap_chain_wallet_cli_error_codes.h)

### cellframe-sdk/modules/wallet/include/dap_chain_wallet_internal.h
- (No public functions detected)

### cellframe-sdk/modules/wallet/include/dap_chain_wallet_ops.h
- dap_chain_wallet_op_tx_request (cellframe-sdk/modules/wallet/include/dap_chain_wallet_ops.h)

## Module: wallet-cache

### cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h
- dap_chain_wallet_cache_deinit (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)
- dap_chain_wallet_cache_init (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)
- dap_chain_wallet_cache_iter_create (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)
- dap_chain_wallet_cache_iter_delete (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)
- dap_chain_wallet_cache_iter_get (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)
- dap_chain_wallet_cache_register_chain (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)
- dap_chain_wallet_cache_tx_find (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)
- dap_chain_wallet_cache_tx_find_in_history (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)
- dap_chain_wallet_cache_tx_find_outs (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)
- dap_chain_wallet_cache_tx_find_outs_mempool_check (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)
- dap_chain_wallet_cache_tx_find_outs_with_val (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)
- dap_chain_wallet_cache_tx_find_outs_with_val_mempool_check (cellframe-sdk/modules/wallet-cache/include/dap_chain_wallet_cache.h)

## Module: wallet-shared

### cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h
- dap_chain_shared_tx_find_in_mempool (cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h)
- dap_chain_wallet_shared_cli (cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h)
- dap_chain_wallet_shared_deinit (cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h)
- dap_chain_wallet_shared_get_tx_hashes_json (cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h)
- dap_chain_wallet_shared_hold_tx_add (cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h)
- dap_chain_wallet_shared_init (cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h)
- dap_chain_wallet_shared_notify_init (cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h)
- dap_chain_wallet_shared_refilling_tx_create (cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h)
- dap_chain_wallet_shared_register_chain (cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h)
- dap_chain_wallet_shared_taking_tx_create (cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h)
- dap_chain_wallet_shared_taking_tx_sign (cellframe-sdk/modules/wallet-shared/include/dap_chain_wallet_shared.h)
