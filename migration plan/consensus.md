# Migration Checklist: consensus

Method checklist:

| Method | Header | Migrated | Unit Tests |
| --- | --- | --- | --- |
| `dap_chain_cs_block_pow_deinit` | `cellframe-sdk/modules/consensus/block-pow/include/dap_chain_cs_block_pow.h` | [ ] | [ ] |
| `dap_chain_cs_block_pow_init` | `cellframe-sdk/modules/consensus/block-pow/include/dap_chain_cs_block_pow.h` | [ ] | [ ] |
| `dap_chain_type_dag_poa_deinit` | `cellframe-sdk/modules/consensus/dag-poa/include/dap_chain_type_dag_poa.h` | [x] | [x] |
| `dap_chain_type_dag_poa_get_auth_certs` | `cellframe-sdk/modules/consensus/dag-poa/include/dap_chain_type_dag_poa.h` | [x] | [x] |
| `dap_chain_type_dag_poa_init` | `cellframe-sdk/modules/consensus/dag-poa/include/dap_chain_type_dag_poa.h` | [x] | [x] |
| `dap_chain_type_dag_poa_presign_callback_set` | `cellframe-sdk/modules/consensus/dag-poa/include/dap_chain_type_dag_poa.h` | [x] | [x] |
| `dap_chain_cs_esbocs_deinit` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_cs_esbocs_init` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_add_block_collect` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_add_validator_to_clusters` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_get_autocollect_status` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_get_collecting_level` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_get_custom_metadata` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [ ] | [ ] |
| `dap_chain_esbocs_get_fee` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_get_min_validators_count` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_get_sign_key` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_get_sign_pkey` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_hardfork_engaged` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_remove_validator_from_clusters` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_set_custom_metadata_callback` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_set_emergency_validator` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_set_empty_block_every_times` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_set_hardfork_complete` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_set_hardfork_prepare` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_set_hardfork_state` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_set_min_validators_count` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_set_presign_callback` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_set_signs_struct_check` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_start_timer` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_started` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_esbocs_stop_timer` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs.h` | [x] | [x] |
| `dap_chain_cs_esbocs_decree_deinit` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs_decree.h` | [x] | [x] |
| `dap_chain_cs_esbocs_decree_init` | `cellframe-sdk/modules/consensus/esbocs/include/dap_chain_cs_esbocs_decree.h` | [x] | [x] |
| `dap_chain_cs_add` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_add_block_collect` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_add_validator` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_create` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_deinit` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_get_autocollect_status` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_get_callbacks` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_get_collecting_level` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_get_fee` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_get_fee_group` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_get_min_validators_count` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_get_reward_group` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_get_sign_key` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_get_sign_pkey` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_hardfork_engaged` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_init` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_is_started` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_load` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_mempool_datum_add` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_purge` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_remove_validator` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_set_callbacks` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_set_hardfork_state` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_set_min_validators_count` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_stake_check_pkey_hash` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_stake_hardfork_data_import` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_stake_switch_table` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_start` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_cs_stop` | `cellframe-sdk/modules/consensus/include/dap_chain_cs.h` | [x] | [x] |
| `dap_chain_validator_api_check_key_delegated` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
| `dap_chain_validator_api_deinit` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
| `dap_chain_validator_api_get_allowed_min_value` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
| `dap_chain_validator_api_get_node_addr` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
| `dap_chain_validator_api_get_pkey_by_hash` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
| `dap_chain_validator_api_get_validators` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
| `dap_chain_validator_api_get_value` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
| `dap_chain_validator_api_hardfork_tx_update` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
| `dap_chain_validator_api_init` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
| `dap_chain_validator_api_key_delegate` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
| `dap_chain_validator_api_mark_validator_active` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
| `dap_chain_validator_api_register` | `cellframe-sdk/modules/consensus/include/dap_chain_validator_api.h` | [x] | [x] |
