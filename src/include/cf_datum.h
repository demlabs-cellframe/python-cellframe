#ifndef CELLFRAME_DATUM_H
#define CELLFRAME_DATUM_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

// Datum core API
PyObject *dap_chain_datum_calc_hash_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_dump_tx_json_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_register_dump_anchor_callback_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_register_dump_decree_callback_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_size_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_type_id_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_datum_token_dump_tsd_to_json_py(PyObject *self, PyObject *args);

// Datum anchors
PyObject *dap_chain_datum_anchor_certs_dump_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_anchor_certs_dump_json_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_anchor_get_hash_from_data_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_anchor_get_size_py(PyObject *self, PyObject *args);

// Datum converter
PyObject *dap_chain_datum_from_tx_py(PyObject *self, PyObject *args);

// Datum decree
PyObject *dap_chain_datum_decree_certs_dump_json_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_dump_json_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_action_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_atom_num_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_ban_addr_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_empty_block_every_times_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_fee_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_fee_addr_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_hash_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_min_owners_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_node_addr_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_owners_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_pkey_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_policy_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_signature_type_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_signs_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_size_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_stake_min_signers_count_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_stake_min_value_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_stake_signing_addr_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_stake_value_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_get_value_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_new_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_sign_in_cycle_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_subtype_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_tsd_type_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_decree_type_from_str_py(PyObject *self, PyObject *args);

// Datum token + emission
PyObject *dap_chain_datum_token_flag_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_token_flag_from_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_token_get_delegated_ticker_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_token_is_old_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_token_check_ticker_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_token_flags_dump_to_json_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_token_certs_dump_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_token_certs_dump_to_json_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_token_read_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_token_tsd_get_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_emission_type_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_emission_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_emission_add_tsd_py(PyObject *self, PyObject *args);
PyObject *dap_chain_emission_get_tsd_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_emission_get_size_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_emission_read_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_emission_add_sign_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_emission_append_sign_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_emission_get_signs_py(PyObject *self, PyObject *args);

// Decree registry
PyObject *dap_chain_decree_registry_deinit_py(PyObject *self, PyObject *args);
PyObject *dap_chain_decree_registry_has_handler_py(PyObject *self, PyObject *args);
PyObject *dap_chain_decree_registry_init_py(PyObject *self, PyObject *args);
PyObject *dap_chain_decree_registry_process_py(PyObject *self, PyObject *args);
PyObject *dap_chain_decree_registry_register_handler_py(PyObject *self, PyObject *args);
PyObject *dap_chain_decree_registry_unregister_handler_py(PyObject *self, PyObject *args);

// Datum tx items, events, out_cond
PyObject *dap_chain_datum_tx_event_to_json_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_event_to_json_py(PyObject *self, PyObject *args);
PyObject *dap_chain_tx_item_event_type_from_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_tx_item_event_type_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_item_tx_get_size_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_event_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_get_data_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_get_tsd_by_type_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_in_cond_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_in_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_in_ems_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_in_reward_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_cond_create_fee_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_cond_create_srv_pay_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_cond_create_srv_stake_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_cond_create_srv_stake_lock_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_cond_create_srv_xchange_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_cond_create_wallet_shared_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_ext_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_out_std_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_sig_get_sign_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_sign_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_sign_create_from_sign_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_tsd_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_type_from_str_short_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_type_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_type_to_str_short_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_sign_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_tx_event_copy_py(PyObject *self, PyObject *args);
PyObject *dap_chain_tx_event_delete_py(PyObject *self, PyObject *args);
PyObject *dap_chain_tx_out_cond_subtype_from_str_short_py(PyObject *self, PyObject *args);
PyObject *dap_chain_tx_sig_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_tx_out_cond_subtype_to_str_py(PyObject *self, PyObject *args);
PyObject *dap_chain_tx_out_cond_subtype_to_str_short_py(PyObject *self, PyObject *args);

// Datum tx receipts + voting
PyObject *dap_chain_datum_tx_receipt_check_size_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_receipt_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_receipt_sign_add_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_receipt_sign_get_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_receipt_signs_count_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_receipt_srv_uid_get_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_receipt_units_get_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_receipt_utype_get_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_receipt_value_get_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_vote_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_vote_to_json_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_voting_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_item_voting_tsd_to_json_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_voting_get_answer_text_by_idx_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_voting_params_delete_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_tx_voting_parse_tsd_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_voting_answer_tsd_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_voting_cancel_tsd_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_voting_delegated_key_required_tsd_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_voting_expire_tsd_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_voting_max_votes_count_tsd_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_voting_question_tsd_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_voting_token_tsd_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_voting_vote_changing_allowed_tsd_create_py(PyObject *self, PyObject *args);
PyObject *dap_chain_datum_voting_vote_tx_cond_tsd_create_py(PyObject *self, PyObject *args);

// Module initialization
int cellframe_datum_init(PyObject *module);

#ifdef __cplusplus
}
#endif

#endif /* CELLFRAME_DATUM_H */
