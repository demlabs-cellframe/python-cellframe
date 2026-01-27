#include "cf_chain_api.h"

// =========================================
// MODULE INITIALIZATION
// =========================================
// This function registers all chain functions with the Python module
int cellframe_chain_init(PyObject *module) {
    // Define chain method definitions
    static PyMethodDef chain_methods[] = {
        // Chain address functions
        {"dap_chain_addr_get_net_id", py_dap_chain_addr_get_net_id, METH_VARARGS,
         "Get network ID from chain address"},
        {"dap_chain_addr_to_str", py_dap_chain_addr_to_str, METH_VARARGS,
         "Convert chain address to string"},

        // Chain operations
        {"dap_chain_atom_create", py_dap_chain_atom_create, METH_VARARGS,
         "Create chain atom"},
        {"dap_chain_atom_get_data", py_dap_chain_atom_get_data, METH_VARARGS,
         "Get atom data from atom pointer"},

        // Mempool operations
        {"dap_chain_mempool_by_chain_name", py_dap_chain_mempool_by_chain_name, METH_VARARGS,
         "Get mempool by chain name"},
        {"dap_chain_mempool_tx_get_by_hash", py_dap_chain_mempool_tx_get_by_hash, METH_VARARGS,
         "Get transaction from mempool by hash"},

        // Chain atom operations
        {"chain_get_atom_by_hash", (PyCFunction)dap_chain_get_atom_by_hash_py, METH_VARARGS,
         "Get atom from chain by hash"},
        {"chain_get_atom_last", (PyCFunction)dap_chain_get_atom_last_py, METH_VARARGS,
         "Get last atom hash, number and timestamp"},

        // Chain management operations
        {"chain_load_all", (PyCFunction)dap_chain_load_all_py, METH_VARARGS,
         "Load all chain data from storage"},
        {"chain_has_file_store", (PyCFunction)dap_chain_has_file_store_py, METH_VARARGS,
         "Check if chain has file store"},
        {"chain_purge", (PyCFunction)dap_chain_purge_py, METH_VARARGS,
         "Purge chain data"},
        {"chain_atom_save", (PyCFunction)dap_chain_atom_save_py, METH_VARARGS,
         "Save atom to chain"},
        {"chain_add_callback_notify", (PyCFunction)dap_chain_add_callback_notify_py, METH_VARARGS,
         "Add callback notify for chain"},

        // Block operations
        {"chain_block_new", py_dap_chain_block_new, METH_VARARGS,
         "Create new block (capsule-based)"},
        {"chain_block_datum_add", py_dap_chain_block_datum_add, METH_VARARGS,
         "Add datum to block (capsule-based). Returns {'block': capsule, 'size': int}"},
        {"chain_block_get_datums", py_dap_chain_block_get_datums, METH_VARARGS,
         "Get datums from block (capsule-based)"},
        {"chain_block_meta_add", py_dap_chain_block_meta_add, METH_VARARGS,
         "Add metadata to block (capsule-based). Returns {'block': capsule, 'size': int}"},
        {"chain_block_sign_add", py_dap_chain_block_sign_add, METH_VARARGS,
         "Add signature to block (capsule-based). Returns {'block': capsule, 'size': int}"},

        // Chain lifecycle operations
        {"chain_create", (PyCFunction)dap_chain_create_py, METH_VARARGS,
         "Create a new chain"},
        {"chain_delete", (PyCFunction)dap_chain_delete_py, METH_VARARGS,
         "Delete a chain"},

        // Chain configuration
        {"chain_set_cs_type", (PyCFunction)dap_chain_set_cs_type_py, METH_VARARGS,
         "Set consensus type for chain"},
        {"chain_set_cs_name", (PyCFunction)dap_chain_set_cs_name_py, METH_VARARGS,
         "Set consensus name for chain"},

        // Chain advanced operations
        {"chain_atom_add_from_threshold", (PyCFunction)dap_chain_atom_add_from_threshold_py, METH_VARARGS,
         "Add atom from threshold to chain"},

        // Chain lookup and utility
        {"chain_find_by_id", (PyCFunction)dap_chain_find_by_id_py, METH_VARARGS,
         "Find chain by network ID and chain ID"},
        {"chain_datum_type_supported_by_chain", (PyCFunction)dap_chain_datum_type_supported_by_chain_py, METH_VARARGS,
         "Check if datum type is supported by chain"},

        // Chain generation management
        {"chain_generation_banned", (PyCFunction)dap_chain_generation_banned_py, METH_VARARGS,
         "Check if chain generation is banned"},
        {"chain_generation_ban", (PyCFunction)dap_chain_generation_ban_py, METH_VARARGS,
         "Ban a chain generation"},

        // Chain extended operations
        {"chain_get_atom_last_hash_num_ts", (PyCFunction)dap_chain_get_atom_last_hash_num_ts_py, METH_VARARGS,
         "Get last atom hash, number and timestamp with full details"},
        {"chain_add_callback_datum_index_notify", (PyCFunction)dap_chain_add_callback_datum_index_notify_py, METH_VARARGS,
         "Add callback for datum index notifications"},
        {"chain_add_callback_datum_removed_from_index_notify", (PyCFunction)dap_chain_add_callback_datum_removed_from_index_notify_py, METH_VARARGS,
         "Add callback for datum removed from index notifications"},
        {"chain_atom_confirmed_notify_add", (PyCFunction)dap_chain_atom_confirmed_notify_add_py, METH_VARARGS,
         "Add callback for atom confirmed notifications"},
        {"chain_add_callback_timer", (PyCFunction)dap_chain_add_callback_timer_py, METH_VARARGS,
         "Add timer callback for blockchain"},

        // Chain SDK core operations
        {"dap_chain_init", (PyCFunction)dap_chain_init_py, METH_NOARGS,
         "Initialize chain subsystem"},
        {"dap_chain_deinit", (PyCFunction)dap_chain_deinit_py, METH_NOARGS,
         "Deinitialize chain subsystem"},
        {"dap_chain_id_parse", (PyCFunction)dap_chain_id_parse_py, METH_VARARGS,
         "Parse chain ID from string"},
        {"dap_chain_load_from_cfg", (PyCFunction)dap_chain_load_from_cfg_py, METH_VARARGS,
         "Load chain from config"},
        {"dap_chain_info_dump_log", (PyCFunction)dap_chain_info_dump_log_py, METH_VARARGS,
         "Dump chain info to log"},
        {"dap_chain_get_path", (PyCFunction)dap_chain_get_path_py, METH_VARARGS,
         "Get chain storage path"},
        {"dap_chain_get_cs_type", (PyCFunction)dap_chain_get_cs_type_py, METH_VARARGS,
         "Get chain consensus type"},
        {"dap_chain_get_blockhain_time", (PyCFunction)dap_chain_get_blockhain_time_py, METH_VARARGS,
         "Get blockchain time for chain or cell"},
        {"dap_chain_get_atom_last_hash", (PyCFunction)dap_chain_get_atom_last_hash_py, METH_VARARGS,
         "Get last atom hash"},
        {"dap_chain_generation_next", (PyCFunction)dap_chain_generation_next_py, METH_VARARGS,
         "Get next chain generation"},
        {"dap_cert_chain_file_save", (PyCFunction)dap_cert_chain_file_save_py, METH_VARARGS,
         "Save certificate chain file"},

        // Chain SDK notify triggers
        {"dap_chain_atom_notify", (PyCFunction)dap_chain_atom_notify_py, METH_VARARGS,
         "Notify atom addition"},
        {"dap_chain_atom_remove_notify", (PyCFunction)dap_chain_atom_remove_notify_py, METH_VARARGS,
         "Notify atom removal"},
        {"dap_chain_datum_notify", (PyCFunction)dap_chain_datum_notify_py, METH_VARARGS,
         "Notify datum addition to index"},
        {"dap_chain_datum_removed_notify", (PyCFunction)dap_chain_datum_removed_notify_py, METH_VARARGS,
         "Notify datum removal from index"},

        // Chain SDK aliases for legacy chain_* names
        {"dap_chain_get_atom_by_hash", (PyCFunction)dap_chain_get_atom_by_hash_py, METH_VARARGS,
         "Get atom from chain by hash"},
        {"dap_chain_get_atom_last_hash_num_ts", (PyCFunction)dap_chain_get_atom_last_hash_num_ts_py, METH_VARARGS,
         "Get last atom hash, number and timestamp"},
        {"dap_chain_load_all", (PyCFunction)dap_chain_load_all_py, METH_VARARGS,
         "Load all chain data from storage"},
        {"dap_chain_has_file_store", (PyCFunction)dap_chain_has_file_store_py, METH_VARARGS,
         "Check if chain has file store"},
        {"dap_chain_purge", (PyCFunction)dap_chain_purge_py, METH_VARARGS,
         "Purge chain data"},
        {"dap_chain_atom_save", (PyCFunction)dap_chain_atom_save_py, METH_VARARGS,
         "Save atom to chain"},
        {"dap_chain_add_callback_notify", (PyCFunction)dap_chain_add_callback_notify_py, METH_VARARGS,
         "Add callback notify for chain"},
        {"dap_chain_create", (PyCFunction)dap_chain_create_py, METH_VARARGS,
         "Create a new chain"},
        {"dap_chain_delete", (PyCFunction)dap_chain_delete_py, METH_VARARGS,
         "Delete a chain"},
        {"dap_chain_set_cs_type", (PyCFunction)dap_chain_set_cs_type_py, METH_VARARGS,
         "Set consensus type for chain"},
        {"dap_chain_set_cs_name", (PyCFunction)dap_chain_set_cs_name_py, METH_VARARGS,
         "Set consensus name for chain"},
        {"dap_chain_atom_add_from_threshold", (PyCFunction)dap_chain_atom_add_from_threshold_py, METH_VARARGS,
         "Add atom from threshold to chain"},
        {"dap_chain_find_by_id", (PyCFunction)dap_chain_find_by_id_py, METH_VARARGS,
         "Find chain by network ID and chain ID"},
        {"dap_chain_datum_type_supported_by_chain", (PyCFunction)dap_chain_datum_type_supported_by_chain_py, METH_VARARGS,
         "Check if datum type is supported by chain"},
        {"dap_chain_generation_banned", (PyCFunction)dap_chain_generation_banned_py, METH_VARARGS,
         "Check if chain generation is banned"},
        {"dap_chain_generation_ban", (PyCFunction)dap_chain_generation_ban_py, METH_VARARGS,
         "Ban a chain generation"},
        {"dap_chain_add_callback_datum_index_notify", (PyCFunction)dap_chain_add_callback_datum_index_notify_py, METH_VARARGS,
         "Add callback for datum index notifications"},
        {"dap_chain_add_callback_datum_removed_from_index_notify", (PyCFunction)dap_chain_add_callback_datum_removed_from_index_notify_py, METH_VARARGS,
         "Add callback for datum removed from index notifications"},
        {"dap_chain_atom_confirmed_notify_add", (PyCFunction)dap_chain_atom_confirmed_notify_add_py, METH_VARARGS,
         "Add callback for atom confirmed notifications"},
        {"dap_chain_add_callback_timer", (PyCFunction)dap_chain_add_callback_timer_py, METH_VARARGS,
         "Add timer callback for blockchain"},

        // Chain cell operations
        {"dap_chain_cell_init", (PyCFunction)dap_chain_cell_init_py, METH_NOARGS,
         "Initialize chain cells"},
        {"dap_chain_cell_open", (PyCFunction)dap_chain_cell_open_py, METH_VARARGS,
         "Open chain cell"},
        {"dap_chain_cell_create", (PyCFunction)dap_chain_cell_create_py, METH_VARARGS,
         "Create chain cell"},
        {"dap_chain_cell_find_by_id", (PyCFunction)dap_chain_cell_find_by_id_py, METH_VARARGS,
         "Find chain cell by ID"},
        {"dap_chain_cell_capture_by_id", (PyCFunction)dap_chain_cell_capture_by_id_py, METH_VARARGS,
         "Capture chain cell by ID"},
        {"dap_chain_cell_remit", (PyCFunction)dap_chain_cell_remit_py, METH_VARARGS,
         "Remit chain cell lock"},
        {"dap_chain_cell_close", (PyCFunction)dap_chain_cell_close_py, METH_VARARGS,
         "Close chain cell"},
        {"dap_chain_cell_close_all", (PyCFunction)dap_chain_cell_close_all_py, METH_VARARGS,
         "Close all chain cells"},
        {"dap_chain_cell_file_append", (PyCFunction)dap_chain_cell_file_append_py, METH_VARARGS,
         "Append atom to chain cell file"},
        {"dap_chain_cell_remove", (PyCFunction)dap_chain_cell_remove_py, METH_VARARGS,
         "Remove chain cell"},
        {"dap_chain_cell_truncate", (PyCFunction)dap_chain_cell_truncate_py, METH_VARARGS,
         "Truncate chain cell file"},
        {"dap_chain_cell_set_load_skip", (PyCFunction)dap_chain_cell_set_load_skip_py, METH_NOARGS,
         "Set chain cell load skip"},

        // Chain ch operations
        {"dap_chain_ch_init", (PyCFunction)dap_chain_ch_init_py, METH_NOARGS,
         "Initialize chain channel"},
        {"dap_chain_ch_deinit", (PyCFunction)dap_chain_ch_deinit_py, METH_NOARGS,
         "Deinitialize chain channel"},
        {"dap_stream_ch_write_error_unsafe", (PyCFunction)dap_stream_ch_write_error_unsafe_py, METH_VARARGS,
         "Write chain channel error (unsafe)"},

        // Chain ch packet operations
        {"dap_chain_ch_pkt_get_size", (PyCFunction)dap_chain_ch_pkt_get_size_py, METH_VARARGS,
         "Get chain channel packet size"},
        {"dap_chain_ch_pkt_new", (PyCFunction)dap_chain_ch_pkt_new_py, METH_VARARGS,
         "Create chain channel packet"},
        {"dap_chain_ch_pkt_type_to_str", (PyCFunction)dap_chain_ch_pkt_type_to_str_py, METH_VARARGS,
         "Get chain channel packet type string"},
        {"dap_chain_ch_pkt_write", (PyCFunction)dap_chain_ch_pkt_write_py, METH_VARARGS,
         "Write chain channel packet"},
        {"dap_chain_ch_pkt_write_unsafe", (PyCFunction)dap_chain_ch_pkt_write_unsafe_py, METH_VARARGS,
         "Write chain channel packet (unsafe)"},

        // Chain net utils
        {"dap_chain_net_get_default_chain_by_chain_type", (PyCFunction)dap_chain_net_get_default_chain_by_chain_type_py, METH_VARARGS,
         "Get default chain by type"},

        // Chain policy operations
        {"dap_chain_policy_init", (PyCFunction)dap_chain_policy_init_py, METH_NOARGS,
         "Initialize chain policy subsystem"},
        {"dap_chain_policy_deinit", (PyCFunction)dap_chain_policy_deinit_py, METH_NOARGS,
         "Deinitialize chain policy subsystem"},
        {"dap_chain_policy_create_activate", (PyCFunction)dap_chain_policy_create_activate_py, METH_VARARGS,
         "Create activate policy"},
        {"dap_chain_policy_create_deactivate", (PyCFunction)dap_chain_policy_create_deactivate_py, METH_VARARGS,
         "Create deactivate policy"},
        {"dap_chain_policy_net_add", (PyCFunction)dap_chain_policy_net_add_py, METH_VARARGS,
         "Add policy net"},
        {"dap_chain_policy_net_purge", (PyCFunction)dap_chain_policy_net_purge_py, METH_VARARGS,
         "Purge policy net"},
        {"dap_chain_policy_net_remove", (PyCFunction)dap_chain_policy_net_remove_py, METH_VARARGS,
         "Remove policy net"},
        {"dap_chain_policy_apply", (PyCFunction)dap_chain_policy_apply_py, METH_VARARGS,
         "Apply policy"},
        {"dap_chain_policy_update_last_num", (PyCFunction)dap_chain_policy_update_last_num_py, METH_VARARGS,
         "Update policy last number"},
        {"dap_chain_policy_get_last_num", (PyCFunction)dap_chain_policy_get_last_num_py, METH_VARARGS,
         "Get policy last number"},
        {"dap_chain_policy_activate_json_collect", (PyCFunction)dap_chain_policy_activate_json_collect_py, METH_VARARGS,
         "Collect activate policy JSON"},
        {"dap_chain_policy_json_collect", (PyCFunction)dap_chain_policy_json_collect_py, METH_VARARGS,
         "Collect policy JSON"},
        {"dap_chain_policy_list", (PyCFunction)dap_chain_policy_list_py, METH_VARARGS,
         "List policies as JSON"},
        {"dap_chain_policy_is_exist", (PyCFunction)dap_chain_policy_is_exist_py, METH_VARARGS,
         "Check if policy exists"},
        {"dap_chain_policy_is_activated", (PyCFunction)dap_chain_policy_is_activated_py, METH_VARARGS,
         "Check if policy is activated"},
        {"dap_chain_policy_get_size", (PyCFunction)dap_chain_policy_get_size_py, METH_VARARGS,
         "Get policy size"},
        {"dap_chain_policy_to_str", (PyCFunction)dap_chain_policy_to_str_py, METH_VARARGS,
         "Get policy type string"},

        // Chain service operations
        {"dap_chain_srv_init", (PyCFunction)dap_chain_srv_init_py, METH_NOARGS,
         "Initialize chain services"},
        {"dap_chain_srv_deinit", (PyCFunction)dap_chain_srv_deinit_py, METH_NOARGS,
         "Deinitialize chain services"},
        {"dap_chain_srv_add", (PyCFunction)dap_chain_srv_add_py, METH_VARARGS,
         "Add chain service"},
        {"dap_chain_srv_start", (PyCFunction)dap_chain_srv_start_py, METH_VARARGS,
         "Start chain service"},
        {"dap_chain_srv_start_all", (PyCFunction)dap_chain_srv_start_all_py, METH_VARARGS,
         "Start all chain services"},
        {"dap_chain_srv_delete", (PyCFunction)dap_chain_srv_delete_py, METH_VARARGS,
         "Delete chain service"},
        {"dap_chain_srv_get_internal", (PyCFunction)dap_chain_srv_get_internal_py, METH_VARARGS,
         "Get chain service internal pointer"},
        {"dap_chain_srv_get_uid_by_name", (PyCFunction)dap_chain_srv_get_uid_by_name_py, METH_VARARGS,
         "Get chain service UID by name"},
        {"dap_chain_srv_count", (PyCFunction)dap_chain_srv_count_py, METH_VARARGS,
         "Get chain services count"},
        {"dap_chain_srv_list", (PyCFunction)dap_chain_srv_list_py, METH_VARARGS,
         "List chain services"},
        {"dap_chain_srv_purge", (PyCFunction)dap_chain_srv_purge_py, METH_VARARGS,
         "Purge chain service"},
        {"dap_chain_srv_purge_all", (PyCFunction)dap_chain_srv_purge_all_py, METH_VARARGS,
         "Purge all chain services"},
        {"dap_chain_srv_hardfork_all", (PyCFunction)dap_chain_srv_hardfork_all_py, METH_VARARGS,
         "Prepare hardfork state for all services"},
        {"dap_chain_srv_load_state", (PyCFunction)dap_chain_srv_load_state_py, METH_VARARGS,
         "Load hardfork state for service"},
        {"dap_chain_srv_hardfork_complete_all", (PyCFunction)dap_chain_srv_hardfork_complete_all_py, METH_VARARGS,
         "Complete hardfork for all services"},
        {"dap_chain_srv_event_verify", (PyCFunction)dap_chain_srv_event_verify_py, METH_VARARGS,
         "Verify chain service event"},
        {"dap_chain_srv_decree", (PyCFunction)dap_chain_srv_decree_py, METH_VARARGS,
         "Apply chain service decree"},
        {"dap_chain_srv_get_fees", (PyCFunction)dap_chain_srv_get_fees_py, METH_VARARGS,
         "Get chain services fees"},
        {"dap_chain_srv_fee_type_to_str", (PyCFunction)dap_chain_srv_fee_type_to_str_py, METH_VARARGS,
         "Get chain service fee type string"},

        {NULL, NULL, 0, NULL}  // Sentinel
    };

    // Add all methods to the module
    for (PyMethodDef *method = chain_methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
        if (!func) {
            return -1;
        }

        if (PyModule_AddObject(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            return -1;
        }
    }

    return 0;
}
