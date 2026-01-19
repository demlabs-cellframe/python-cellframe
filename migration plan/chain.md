# Migration Checklist: chain

Method checklist:

| Method | Header | Migrated | Unit Tests | SDK Impl | Python Wrapper |
| --- | --- | --- | --- | --- | --- |
| `dap_cert_chain_file_save` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_add_callback_datum_index_notify` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_callbacks.c` |
| `dap_chain_add_callback_datum_removed_from_index_notify` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_callbacks.c` |
| `dap_chain_add_callback_notify` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_callbacks.c` |
| `dap_chain_add_callback_timer` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_callbacks.c` |
| `dap_chain_atom_add_from_threshold` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_atom_confirmed_notify_add` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_callbacks.c` |
| `dap_chain_atom_notify` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_callbacks.c` |
| `dap_chain_atom_remove_notify` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_callbacks.c` |
| `dap_chain_atom_save` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_create` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_datum_notify` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_callbacks.c` |
| `dap_chain_datum_removed_notify` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_callbacks.c` |
| `dap_chain_datum_type_supported_by_chain` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_deinit` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_delete` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_find_by_id` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_generation_ban` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_generation_banned` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_generation_next` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_get_atom_by_hash` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_get_atom_last_hash` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain.h (inline)` | `src/chain/cf_chain_core.c` |
| `dap_chain_get_atom_last_hash_num_ts` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_get_blockhain_time` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain.h (inline)` | `src/chain/cf_chain_core.c` |
| `dap_chain_get_cs_type` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_get_path` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_has_file_store` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_id_parse` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain.h (inline)` | `src/chain/cf_chain_core.c` |
| `dap_chain_info_dump_log` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain.h (decl only)` | `src/chain/cf_chain_core.c` |
| `dap_chain_init` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_load_all` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_load_from_cfg` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_purge` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_set_cs_name` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_set_cs_type` | `cellframe-sdk/modules/chain/include/dap_chain.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain.c` | `src/chain/cf_chain_core.c` |
| `dap_chain_cell_capture_by_id` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain_cell.h (inline)` | `src/chain/cf_chain_cell.c` |
| `dap_chain_cell_close` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_cell.c` | `src/chain/cf_chain_cell.c` |
| `dap_chain_cell_close_all` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_cell.c` | `src/chain/cf_chain_cell.c` |
| `dap_chain_cell_create` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain_cell.h (inline)` | `src/chain/cf_chain_cell.c` |
| `dap_chain_cell_file_append` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_cell.c` | `src/chain/cf_chain_cell.c` |
| `dap_chain_cell_find_by_id` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain_cell.h (inline)` | `src/chain/cf_chain_cell.c` |
| `dap_chain_cell_init` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_cell.c` | `src/chain/cf_chain_cell.c` |
| `dap_chain_cell_open` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_cell.c` | `src/chain/cf_chain_cell.c` |
| `dap_chain_cell_remit` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain_cell.h (inline)` | `src/chain/cf_chain_cell.c` |
| `dap_chain_cell_remove` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_cell.c` | `src/chain/cf_chain_cell.c` |
| `dap_chain_cell_set_load_skip` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_cell.c` | `src/chain/cf_chain_cell.c` |
| `dap_chain_cell_truncate` | `cellframe-sdk/modules/chain/include/dap_chain_cell.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_cell.c` | `src/chain/cf_chain_cell.c` |
| `dap_chain_ch_deinit` | `cellframe-sdk/modules/chain/include/dap_chain_ch.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_ch.c` | `src/chain/cf_chain_ch.c` |
| `dap_chain_ch_init` | `cellframe-sdk/modules/chain/include/dap_chain_ch.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_ch.c` | `src/chain/cf_chain_ch.c` |
| `dap_stream_ch_write_error_unsafe` | `cellframe-sdk/modules/chain/include/dap_chain_ch.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_ch.c` | `src/chain/cf_chain_ch.c` |
| `dap_chain_ch_pkt_get_size` | `cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h (inline)` | `src/chain/cf_chain_ch_pkt.c` |
| `dap_chain_ch_pkt_new` | `cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_ch_pkt.c` | `src/chain/cf_chain_ch_pkt.c` |
| `dap_chain_ch_pkt_type_to_str` | `cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h (inline)` | `src/chain/cf_chain_ch_pkt.c` |
| `dap_chain_ch_pkt_write` | `cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_ch_pkt.c` | `src/chain/cf_chain_ch_pkt.c` |
| `dap_chain_ch_pkt_write_unsafe` | `cellframe-sdk/modules/chain/include/dap_chain_ch_pkt.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_ch_pkt.c` | `src/chain/cf_chain_ch_pkt.c` |
| `dap_chain_net_get_default_chain_by_chain_type` | `cellframe-sdk/modules/chain/include/dap_chain_net_utils.h` | [x] | [x] | `cellframe-sdk/modules/net/dap_chain_net_utils.c` | `src/chain/cf_chain_net_utils.c` |
| `dap_chain_policy_activate_json_collect` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_apply` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_create_activate` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_create_deactivate` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_deinit` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_get_last_num` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_get_size` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain_policy.h (inline)` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_init` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_is_activated` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_is_exist` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_json_collect` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_list` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_net_add` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_net_purge` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_net_remove` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_to_str` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain_policy.h (inline)` | `src/chain/cf_chain_policy.c` |
| `dap_chain_policy_update_last_num` | `cellframe-sdk/modules/chain/include/dap_chain_policy.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_policy.c` | `src/chain/cf_chain_policy.c` |
| `dap_chain_srv_add` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_count` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_decree` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_deinit` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_delete` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_event_verify` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_fee_type_to_str` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain_srv.h (inline)` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_get_fees` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_get_internal` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_get_uid_by_name` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_hardfork_all` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_hardfork_complete_all` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_init` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/include/dap_chain_srv.h (inline)` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_list` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_load_state` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_purge` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_purge_all` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_start` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
| `dap_chain_srv_start_all` | `cellframe-sdk/modules/chain/include/dap_chain_srv.h` | [x] | [x] | `cellframe-sdk/modules/chain/dap_chain_srv.c` | `src/chain/cf_chain_srv.c` |
