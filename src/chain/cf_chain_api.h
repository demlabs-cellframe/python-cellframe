#ifndef CF_CHAIN_API_H
#define CF_CHAIN_API_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

PyObject* dap_chain_get_atom_by_hash_py(PyObject *self, PyObject *args);
PyObject* dap_chain_get_atom_last_py(PyObject *self, PyObject *args);
PyObject* dap_chain_load_all_py(PyObject *self, PyObject *args);
PyObject* dap_chain_has_file_store_py(PyObject *self, PyObject *args);
PyObject* dap_chain_purge_py(PyObject *self, PyObject *args);
PyObject* dap_chain_atom_save_py(PyObject *self, PyObject *args);
PyObject* dap_chain_add_callback_notify_py(PyObject *self, PyObject *args);
PyObject* py_dap_chain_addr_get_net_id(PyObject *self, PyObject *args);
PyObject* py_dap_chain_addr_to_str(PyObject *self, PyObject *args);
PyObject* py_dap_chain_atom_create(PyObject *self, PyObject *args);
PyObject* py_dap_chain_atom_get_data(PyObject *self, PyObject *args);
PyObject* py_dap_chain_mempool_by_chain_name(PyObject *self, PyObject *args);
PyObject* py_dap_chain_mempool_tx_get_by_hash(PyObject *self, PyObject *args);
PyObject* dap_chain_create_py(PyObject *self, PyObject *args);
PyObject* dap_chain_delete_py(PyObject *self, PyObject *args);
PyObject* dap_chain_set_cs_type_py(PyObject *self, PyObject *args);
PyObject* dap_chain_set_cs_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_atom_add_from_threshold_py(PyObject *self, PyObject *args);
PyObject* dap_chain_find_by_id_py(PyObject *self, PyObject *args);
PyObject* dap_chain_datum_type_supported_by_chain_py(PyObject *self, PyObject *args);
PyObject* dap_chain_generation_banned_py(PyObject *self, PyObject *args);
PyObject* dap_chain_generation_ban_py(PyObject *self, PyObject *args);
PyObject* dap_chain_get_atom_last_hash_num_ts_py(PyObject *self, PyObject *args);
PyObject* dap_chain_init_py(PyObject *self, PyObject *args);
PyObject* dap_chain_deinit_py(PyObject *self, PyObject *args);
PyObject* dap_chain_id_parse_py(PyObject *self, PyObject *args);
PyObject* dap_chain_get_atom_last_hash_py(PyObject *self, PyObject *args);
PyObject* dap_chain_get_blockhain_time_py(PyObject *self, PyObject *args);
PyObject* dap_chain_get_path_py(PyObject *self, PyObject *args);
PyObject* dap_chain_get_cs_type_py(PyObject *self, PyObject *args);
PyObject* dap_chain_load_from_cfg_py(PyObject *self, PyObject *args);
PyObject* dap_chain_info_dump_log_py(PyObject *self, PyObject *args);
PyObject* dap_chain_generation_next_py(PyObject *self, PyObject *args);
PyObject* dap_cert_chain_file_save_py(PyObject *self, PyObject *args);
PyObject* dap_chain_atom_notify_py(PyObject *self, PyObject *args);
PyObject* dap_chain_atom_remove_notify_py(PyObject *self, PyObject *args);
PyObject* dap_chain_datum_notify_py(PyObject *self, PyObject *args);
PyObject* dap_chain_datum_removed_notify_py(PyObject *self, PyObject *args);
PyObject* dap_chain_add_callback_datum_index_notify_py(PyObject *self, PyObject *args);
PyObject* dap_chain_add_callback_datum_removed_from_index_notify_py(PyObject *self, PyObject *args);
PyObject* dap_chain_atom_confirmed_notify_add_py(PyObject *self, PyObject *args);
PyObject* dap_chain_add_callback_timer_py(PyObject *self, PyObject *args);
PyObject* py_dap_chain_block_new(PyObject *self, PyObject *args);
PyObject* py_dap_chain_block_datum_add(PyObject *self, PyObject *args);
PyObject* py_dap_chain_block_get_datums(PyObject *self, PyObject *args);
PyObject* py_dap_chain_block_meta_add(PyObject *self, PyObject *args);
PyObject* py_dap_chain_block_sign_add(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_init_py(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_open_py(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_create_py(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_find_by_id_py(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_capture_by_id_py(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_remit_py(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_close_py(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_close_all_py(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_file_append_py(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_remove_py(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_truncate_py(PyObject *self, PyObject *args);
PyObject* dap_chain_cell_set_load_skip_py(PyObject *self, PyObject *args);
PyObject* dap_chain_ch_init_py(PyObject *self, PyObject *args);
PyObject* dap_chain_ch_deinit_py(PyObject *self, PyObject *args);
PyObject* dap_stream_ch_write_error_unsafe_py(PyObject *self, PyObject *args);
PyObject* dap_chain_ch_pkt_get_size_py(PyObject *self, PyObject *args);
PyObject* dap_chain_ch_pkt_new_py(PyObject *self, PyObject *args);
PyObject* dap_chain_ch_pkt_write_unsafe_py(PyObject *self, PyObject *args);
PyObject* dap_chain_ch_pkt_write_py(PyObject *self, PyObject *args);
PyObject* dap_chain_ch_pkt_type_to_str_py(PyObject *self, PyObject *args);
PyObject* dap_chain_net_get_default_chain_by_chain_type_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_init_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_deinit_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_create_activate_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_create_deactivate_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_net_add_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_net_purge_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_net_remove_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_apply_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_update_last_num_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_get_last_num_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_activate_json_collect_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_json_collect_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_list_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_is_exist_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_is_activated_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_get_size_py(PyObject *self, PyObject *args);
PyObject* dap_chain_policy_to_str_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_init_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_deinit_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_add_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_start_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_start_all_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_delete_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_get_internal_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_get_uid_by_name_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_count_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_list_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_purge_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_purge_all_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_hardfork_all_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_load_state_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_hardfork_complete_all_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_event_verify_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_decree_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_get_fees_py(PyObject *self, PyObject *args);
PyObject* dap_chain_srv_fee_type_to_str_py(PyObject *self, PyObject *args);

#ifdef __cplusplus
}
#endif

#endif  // CF_CHAIN_API_H
