# Migration Checklist: datum

Parts:

1) Datum core, anchors, converters (`dap_chain_datum.h`, `dap_chain_datum_anchor.h`, `dap_chain_datum_converter.h`)
2) Decree datums + registry (`dap_chain_datum_decree.h`, `dap_chain_decree_registry.h`)
3) Token + emission datums (`dap_chain_datum_token.h`)
4) Transaction core, creation, signing (`dap_chain_datum_tx.h`, `dap_chain_datum_tx_create.h`, `dap_chain_tx.h`, `dap_chain_tx_sign.h`)
5) Transaction items, events, out_cond (`dap_chain_datum_tx_items.h`, `dap_chain_datum_tx_event.h`, `dap_chain_datum_tx_out_cond.h`)
6) Transaction receipts + voting (`dap_chain_datum_tx_receipt.h`, `dap_chain_datum_tx_voting.h`)
7) Transaction compose registry (`dap_chain_tx_compose_api.h`, `dap_chain_tx_compose_registry.h`)

## Part 1: Datum core, anchors, converters

| Method | Header | Migrated | Unit Tests |
| --- | --- | --- | --- |
| `dap_chain_datum_calc_hash` | `cellframe-sdk/modules/datum/include/dap_chain_datum.h` | [x] | [x] |
| `dap_chain_datum_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum.h` | [x] | [x] |
| `dap_chain_datum_dump_tx_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum.h` | [x] | [x] |
| `dap_chain_datum_register_dump_anchor_callback` | `cellframe-sdk/modules/datum/include/dap_chain_datum.h` | [x] | [x] |
| `dap_chain_datum_register_dump_decree_callback` | `cellframe-sdk/modules/datum/include/dap_chain_datum.h` | [x] | [x] |
| `dap_chain_datum_size` | `cellframe-sdk/modules/datum/include/dap_chain_datum.h` | [x] | [x] |
| `dap_chain_datum_to_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum.h` | [ ] | [ ] |
| `dap_chain_datum_type_id_to_str` | `cellframe-sdk/modules/datum/include/dap_chain_datum.h` | [x] | [x] |
| `dap_datum_token_dump_tsd_to_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum.h` | [x] | [x] |
| `dap_chain_datum_anchor_certs_dump` | `cellframe-sdk/modules/datum/include/dap_chain_datum_anchor.h` | [x] | [x] |
| `dap_chain_datum_anchor_certs_dump_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum_anchor.h` | [x] | [x] |
| `dap_chain_datum_anchor_get_hash_from_data` | `cellframe-sdk/modules/datum/include/dap_chain_datum_anchor.h` | [x] | [x] |
| `dap_chain_datum_anchor_get_size` | `cellframe-sdk/modules/datum/include/dap_chain_datum_anchor.h` | [x] | [x] |
| `dap_chain_datum_from_tx` | `cellframe-sdk/modules/datum/include/dap_chain_datum_converter.h` | [x] | [x] |

## Part 2: Decree datums + registry

| Method | Header | Migrated | Unit Tests |
| --- | --- | --- | --- |
| `dap_chain_datum_decree_certs_dump` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [ ] | [ ] |
| `dap_chain_datum_decree_certs_dump_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_dump` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [ ] | [ ] |
| `dap_chain_datum_decree_dump_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_action` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_atom_num` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_ban_addr` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_empty_block_every_times` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_fee` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_fee_addr` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_hash` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_min_owners` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_node_addr` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_owners` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_pkey` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_policy` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_signature_type` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_signs` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_size` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_stake_min_signers_count` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_stake_min_value` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_stake_signing_addr` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_stake_value` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_get_value` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_new` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_sign_in_cycle` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_subtype_to_str` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_tsd_type_to_str` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_datum_decree_type_from_str` | `cellframe-sdk/modules/datum/include/dap_chain_datum_decree.h` | [x] | [x] |
| `dap_chain_decree_registry_deinit` | `cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h` | [x] | [x] |
| `dap_chain_decree_registry_has_handler` | `cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h` | [x] | [x] |
| `dap_chain_decree_registry_init` | `cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h` | [x] | [x] |
| `dap_chain_decree_registry_process` | `cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h` | [x] | [x] |
| `dap_chain_decree_registry_register_handler` | `cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h` | [x] | [x] |
| `dap_chain_decree_registry_unregister_handler` | `cellframe-sdk/modules/datum/include/dap_chain_decree_registry.h` | [x] | [x] |

## Part 3: Token + emission datums

| Method | Header | Migrated | Unit Tests |
| --- | --- | --- | --- |
| `dap_chain_datum_emission_add_sign` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_emission_add_tsd` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_emission_append_sign` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_emission_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_emission_get_signs` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_emission_get_size` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_emission_read` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_emission_type_str` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_token_certs_dump` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_token_certs_dump_to_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_token_check_ticker` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_token_flag_from_str` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_token_flag_to_str` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_token_flags_dump_to_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_token_get_delegated_ticker` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_token_is_old` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_token_read` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_datum_token_tsd_get` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |
| `dap_chain_emission_get_tsd` | `cellframe-sdk/modules/datum/include/dap_chain_datum_token.h` | [x] | [x] |

## Part 4: Transaction core, creation, signing

| Method | Header | Migrated | Unit Tests |
| --- | --- | --- | --- |
| `dap_chain_datum_tx_add_event_item` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_fee_item` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_in_cond_item` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_in_cond_item_list` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_in_item` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_in_item_list` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_in_reward_item` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_item` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_out_cond_item` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_out_ext_item` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_out_item` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_out_std_item` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_sign` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_sign_item` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_delete` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_get_fee_value` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_get_sign` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_get_sign_data` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_get_size` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_group_items` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_group_items_free` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_item_get` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_item_get_nth` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_items_get` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_out_cond_get` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_verify_sign` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_verify_sign_all` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_node_datum_tx_calc_hash` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx.h` | [x] | [x] |
| `dap_chain_datum_tx_add_sign` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h` | [x] | [x] |
| `dap_chain_datum_tx_create_cond_output` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h` | [x] | [ ] |
| `dap_chain_datum_tx_create_event` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h` | [x] | [ ] |
| `dap_chain_datum_tx_create_from_emission` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h` | [x] | [ ] |
| `dap_chain_datum_tx_create_multi_transfer` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h` | [x] | [ ] |
| `dap_chain_datum_tx_create_transfer` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h` | [x] | [ ] |
| `dap_chain_datum_tx_get_sign_data` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_create.h` | [x] | [x] |
| `dap_chain_tx_delete` | `cellframe-sdk/modules/datum/include/dap_chain_tx.h` | [x] | [x] |
| `dap_chain_tx_dup` | `cellframe-sdk/modules/datum/include/dap_chain_tx.h` | [x] | [x] |
| `dap_chain_tx_hh_add` | `cellframe-sdk/modules/datum/include/dap_chain_tx.h` | [x] | [x] |
| `dap_chain_tx_hh_find` | `cellframe-sdk/modules/datum/include/dap_chain_tx.h` | [x] | [x] |
| `dap_chain_tx_hh_free` | `cellframe-sdk/modules/datum/include/dap_chain_tx.h` | [x] | [x] |
| `dap_chain_tx_wrap_packed` | `cellframe-sdk/modules/datum/include/dap_chain_tx.h` | [x] | [x] |
| `dap_chain_tx_get_signing_data` | `cellframe-sdk/modules/datum/include/dap_chain_tx_sign.h` | [x] | [x] |
| `dap_chain_tx_sign_add` | `cellframe-sdk/modules/datum/include/dap_chain_tx_sign.h` | [x] | [x] |

## Part 5: Transaction items, events, out_cond

| Method | Header | Migrated | Unit Tests |
| --- | --- | --- | --- |
| `dap_chain_datum_tx_event_to_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_event.h` | [x] | [x] |
| `dap_chain_datum_tx_item_event_to_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_event.h` | [x] | [x] |
| `dap_chain_tx_item_event_type_from_str` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_event.h` | [x] | [x] |
| `dap_chain_tx_item_event_type_to_str` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_event.h` | [x] | [x] |
| `dap_chain_datum_item_tx_get_size` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_event_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_get_data` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_get_tsd_by_type` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_in_cond_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_in_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_in_ems_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_in_reward_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_cond_create_fee` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_cond_create_srv_pay` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_cond_create_srv_stake` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_cond_create_srv_stake_lock` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_cond_create_srv_xchange` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_cond_create_wallet_shared` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_ext_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_out_std_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_sig_get_sign` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_sign_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_sign_create_from_sign` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_tsd_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_type_from_str_short` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_type_to_str` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_item_type_to_str_short` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_datum_tx_sign_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_tx_event_copy` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_tx_event_delete` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_tx_out_cond_subtype_from_str_short` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_tx_sig_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_items.h` | [x] | [x] |
| `dap_chain_tx_out_cond_subtype_to_str` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_out_cond.h` | [x] | [x] |
| `dap_chain_tx_out_cond_subtype_to_str_short` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_out_cond.h` | [x] | [x] |

## Part 6: Transaction receipts + voting

| Method | Header | Migrated | Unit Tests |
| --- | --- | --- | --- |
| `dap_chain_datum_tx_receipt_check_size` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h` | [x] | [x] |
| `dap_chain_datum_tx_receipt_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h` | [x] | [x] |
| `dap_chain_datum_tx_receipt_sign_add` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h` | [x] | [x] |
| `dap_chain_datum_tx_receipt_sign_get` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h` | [x] | [x] |
| `dap_chain_datum_tx_receipt_signs_count` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h` | [x] | [x] |
| `dap_chain_datum_tx_receipt_srv_uid_get` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h` | [x] | [x] |
| `dap_chain_datum_tx_receipt_units_get` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h` | [x] | [x] |
| `dap_chain_datum_tx_receipt_utype_get` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h` | [x] | [x] |
| `dap_chain_datum_tx_receipt_value_get` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_receipt.h` | [x] | [x] |
| `dap_chain_datum_tx_item_vote_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_tx_item_vote_to_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_tx_item_voting_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_tx_item_voting_tsd_to_json` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_tx_voting_get_answer_text_by_idx` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_tx_voting_params_delete` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_tx_voting_parse_tsd` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_voting_answer_tsd_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_voting_cancel_tsd_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_voting_delegated_key_required_tsd_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_voting_expire_tsd_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_voting_max_votes_count_tsd_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_voting_question_tsd_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_voting_token_tsd_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_voting_vote_changing_allowed_tsd_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |
| `dap_chain_datum_voting_vote_tx_cond_tsd_create` | `cellframe-sdk/modules/datum/include/dap_chain_datum_tx_voting.h` | [x] | [x] |

## Part 7: Transaction compose registry

| Method | Header | Migrated | Unit Tests |
| --- | --- | --- | --- |
| `dap_chain_tx_compose_create` | `cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h` | [x] | [x] |
| `dap_chain_tx_compose_deinit` | `cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h` | [x] | [x] |
| `dap_chain_tx_compose_init` | `cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h` | [x] | [x] |
| `dap_chain_tx_compose_is_registered` | `cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h` | [x] | [x] |
| `dap_chain_tx_compose_register` | `cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h` | [x] | [x] |
| `dap_chain_tx_compose_unregister` | `cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h` | [x] | [x] |
| `dap_chain_tx_compose_registry_add` | `cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_registry.h` | [x] | [x] |
| `dap_chain_tx_compose_registry_deinit` | `cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_registry.h` | [x] | [x] |
| `dap_chain_tx_compose_registry_find` | `cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_registry.h` | [x] | [x] |
| `dap_chain_tx_compose_registry_init` | `cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_registry.h` | [x] | [x] |
| `dap_chain_tx_compose_registry_remove` | `cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_registry.h` | [x] | [x] |
