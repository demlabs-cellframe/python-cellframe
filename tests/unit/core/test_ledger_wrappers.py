import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_SIZE = 32
HASH_BYTES = b"\x00" * HASH_SIZE
BAD_HASH = b"\x00" * (HASH_SIZE - 1)


def is_capsule(obj):
    return type(obj).__name__ == "PyCapsule"


def assert_capsule_or_none(value):
    if value is None:
        return
    assert is_capsule(value)


@pytest.fixture(scope="session")
def sdk_initialized(tmp_path_factory):
    if hasattr(cf, "is_sdk_available") and not cf.is_sdk_available():
        pytest.skip("Cellframe SDK not available")
    base = tmp_path_factory.mktemp("cellframe_sdk")
    config_dir = base / "etc"
    temp_dir = base / "tmp"
    log_dir = base / "var"
    config_dir.mkdir(parents=True, exist_ok=True)
    temp_dir.mkdir(parents=True, exist_ok=True)
    log_dir.mkdir(parents=True, exist_ok=True)
    log_file = log_dir / "cellframe.log"
    try:
        result = cf.initialize(
            "cellframe_tests",
            str(base),
            str(config_dir),
            str(temp_dir),
            str(log_file),
            1,
            1000,
            False,
        )
    except Exception as exc:
        pytest.skip(f"Cellframe SDK init failed: {exc}")
    if result is not True:
        pytest.skip("Cellframe SDK init returned failure")

    cf.dap_chain_init()
    cf.dap_chain_cell_init()
    cf.dap_chain_policy_init()
    cf.dap_chain_srv_init()
    cf.ledger_init()
    return True




LEDGER_METHODS = [
    "chain_datum_token_get_signs_total",
    "chain_datum_token_get_signs_valid",
    "chain_datum_token_get_subtype",
    "chain_datum_token_get_ticker",
    "chain_datum_token_get_total_supply",
    "chain_datum_token_get_type",
    "chain_datum_token_get_version",
    "cli_get_ledger_by_net_name",
    "cli_parse_hash_out_type",
    "cli_parse_pagination",
    "com_ledger",
    "com_mempool_add",
    "com_token",
    "com_tx_cond_create",
    "com_tx_cond_remove",
    "com_tx_cond_unspent_find",
    "com_tx_create",
    "com_tx_create_json",
    "com_tx_history",
    "com_tx_verify",
    "dap_chain_datum_dump_json",
    "ledger_addr_get_token_ticker_all",
    "ledger_addr_get_token_ticker_all_deprecated",
    "ledger_addr_get_token_ticker_all_depricated",
    "ledger_anchor_find",
    "ledger_anchor_load",
    "ledger_anchor_purge",
    "ledger_anchor_unload",
    "ledger_anchor_unload_set_callback",
    "ledger_anchor_verify",
    "ledger_anchors_aggregate",
    "ledger_balance_change_notifier_register",
    "ledger_balance_change_notifier_unregister",
    "ledger_balance_info",
    "ledger_bridged_tx_notify_add",
    "ledger_cache_enabled",
    "ledger_calc_balance",
    "ledger_calc_balance_full",
    "ledger_callbacks_get",
    "ledger_callbacks_register",
    "ledger_chain_purge",
    "ledger_check_condition_owner",
    "ledger_check_error_str",
    "ledger_cli_cmd_execute",
    "ledger_cli_cmd_get_categories",
    "ledger_cli_cmd_get_commands",
    "ledger_cli_cmd_is_registered",
    "ledger_cli_cmd_register",
    "ledger_cli_cmd_registry_deinit",
    "ledger_cli_cmd_registry_init",
    "ledger_cli_cmd_unregister",
    "ledger_cli_dispatcher",
    "ledger_cli_error_codes_init",
    "ledger_cli_module_deinit",
    "ledger_cli_module_init",
    "ledger_cli_tx_create",
    "ledger_cli_tx_create_json",
    "ledger_cli_tx_deinit",
    "ledger_cli_tx_history",
    "ledger_cli_tx_init",
    "ledger_cli_tx_verify",
    "ledger_coin_get_uncoloured_value",
    "ledger_colour_clear_callback",
    "ledger_count",
    "ledger_count_from_to",
    "ledger_create",
    "ledger_create_options_default",
    "ledger_datum_is_enforced",
    "ledger_datum_iter_create",
    "ledger_datum_iter_delete",
    "ledger_datum_iter_get_first",
    "ledger_datum_iter_get_last",
    "ledger_datum_iter_get_next",
    "ledger_decree_apply",
    "ledger_decree_get_by_hash",
    "ledger_decree_get_min_num_of_signers",
    "ledger_decree_get_num_of_owners",
    "ledger_decree_get_owners_pkeys",
    "ledger_decree_init",
    "ledger_decree_load",
    "ledger_decree_purge",
    "ledger_decree_reset_applied",
    "ledger_decree_set_callback",
    "ledger_decree_verify",
    "ledger_decrees_get_by_type",
    "ledger_deduct_tx_tag",
    "ledger_deinit",
    "ledger_emission_for_stake_lock_item_add",
    "ledger_event_find",
    "ledger_event_get_list",
    "ledger_event_get_list_ex",
    "ledger_event_notify_add",
    "ledger_event_pkey_add",
    "ledger_event_pkey_check",
    "ledger_event_pkey_list",
    "ledger_event_pkey_rm",
    "ledger_event_purge",
    "ledger_events_aggregate",
    "ledger_find_by_name",
    "ledger_find_by_net_id",
    "ledger_get_blockchain_time",
    "ledger_get_chain_info",
    "ledger_get_chain_info_by_name",
    "ledger_get_description_by_ticker",
    "ledger_get_final_chain_tx_hash",
    "ledger_get_first_chain_tx_hash",
    "ledger_get_gdb_group",
    "ledger_get_list_tx_cond_outs",
    "ledger_get_list_tx_outs_from_json",
    "ledger_get_locked_values",
    "ledger_get_name",
    "ledger_get_net_id",
    "ledger_get_pkey",
    "ledger_get_pkey_hash",
    "ledger_get_tx_cond_out",
    "ledger_get_tx_out_cond_linked_to_tx_in_cond",
    "ledger_get_txs",
    "ledger_get_utxo_for_value",
    "ledger_handle_free",
    "ledger_has_chain_id",
    "ledger_init",
    "ledger_is_used_reward",
    "ledger_load_cache",
    "ledger_load_end",
    "ledger_out_cond_unspent_find_by_addr",
    "ledger_purge",
    "ledger_pvt_addr_check",
    "ledger_pvt_balance_update_for_addr",
    "ledger_pvt_cache_gdb_load_balances_callback",
    "ledger_pvt_cache_gdb_load_stake_lock_callback",
    "ledger_pvt_cache_gdb_load_tokens_callback",
    "ledger_pvt_emission_cache_update",
    "ledger_pvt_emission_item_find",
    "ledger_pvt_event_remove",
    "ledger_pvt_event_verify_add",
    "ledger_pvt_find_token",
    "ledger_pvt_threshold_txs_add",
    "ledger_pvt_threshold_txs_proc",
    "ledger_pvt_token_supply_check",
    "ledger_pvt_token_supply_check_update",
    "ledger_register_chain",
    "ledger_service_add",
    "ledger_set_blockchain_time",
    "ledger_set_blockchain_timer",
    "ledger_set_cache_tx_check_callback",
    "ledger_set_check_ds",
    "ledger_set_fee_callback",
    "ledger_set_fee_params",
    "ledger_set_get_cur_cell_callback",
    "ledger_set_hal_hrl",
    "ledger_set_load_mode",
    "ledger_set_mempool_callbacks",
    "ledger_set_native_ticker",
    "ledger_set_net_id",
    "ledger_set_poa_keys",
    "ledger_set_syncing_state",
    "ledger_set_wallet_cache_callbacks",
    "ledger_set_wallet_callbacks",
    "ledger_sign_data",
    "ledger_states_aggregate",
    "ledger_tax_callback_set",
    "ledger_threshold_info",
    "ledger_token_add",
    "ledger_token_add_check",
    "ledger_token_decl_all",
    "ledger_token_emission_add",
    "ledger_token_emission_add_check",
    "ledger_token_emission_find",
    "ledger_token_emission_load",
    "ledger_token_emissions_mark_hardfork",
    "ledger_token_get_auth_pkeys_hashes",
    "ledger_token_get_auth_signs_total",
    "ledger_token_get_auth_signs_valid",
    "ledger_token_get_emission_rate",
    "ledger_token_info",
    "ledger_token_info_by_name",
    "ledger_token_load",
    "ledger_token_purge",
    "ledger_token_ticker_check",
    "ledger_token_tx_item_list",
    "ledger_tx_action_str",
    "ledger_tx_action_str_to_action_t",
    "ledger_tx_add",
    "ledger_tx_add_check",
    "ledger_tx_add_notify",
    "ledger_tx_builders_register",
    "ledger_tx_builders_unregister",
    "ledger_tx_cache_find_out_cond_all",
    "ledger_tx_calculate_main_ticker",
    "ledger_tx_check_recipient",
    "ledger_tx_clear_colour",
    "ledger_tx_create_event",
    "ledger_tx_create_from_emission",
    "ledger_tx_find_by_addr",
    "ledger_tx_find_by_hash",
    "ledger_tx_get_token_ticker_by_hash",
    "ledger_tx_get_trackers",
    "ledger_tx_hash_is_used_out_item",
    "ledger_tx_load",
    "ledger_tx_poa_signed",
    "ledger_tx_remove",
    "ledger_tx_service_info",
    "ledger_tx_tag_str_by_uid",
    "ledger_tx_unspent_find_by_hash",
    "ledger_unregister_chain",
    "ledger_verificator_add",
    "ledger_voting_verificator_add",
]

NOARG_METHODS = [
    "ledger_callbacks_get",
    "ledger_cli_cmd_get_categories",
    "ledger_cli_cmd_registry_deinit",
    "ledger_cli_cmd_registry_init",
    "ledger_cli_error_codes_init",
    "ledger_cli_module_deinit",
    "ledger_cli_module_init",
    "ledger_cli_tx_deinit",
    "ledger_cli_tx_init",
    "ledger_create_options_default",
    "ledger_deinit",
    "ledger_init",
    "ledger_tx_builders_register",
    "ledger_tx_builders_unregister",
]

REQUIRES_ARGS = [
    "chain_datum_token_get_signs_total",
    "chain_datum_token_get_signs_valid",
    "chain_datum_token_get_subtype",
    "chain_datum_token_get_ticker",
    "chain_datum_token_get_total_supply",
    "chain_datum_token_get_type",
    "chain_datum_token_get_version",
    "cli_get_ledger_by_net_name",
    "cli_parse_hash_out_type",
    "cli_parse_pagination",
    "com_ledger",
    "com_mempool_add",
    "com_token",
    "com_tx_cond_create",
    "com_tx_cond_remove",
    "com_tx_cond_unspent_find",
    "com_tx_create",
    "com_tx_create_json",
    "com_tx_history",
    "com_tx_verify",
    "dap_chain_datum_dump_json",
    "ledger_addr_get_token_ticker_all",
    "ledger_addr_get_token_ticker_all_deprecated",
    "ledger_addr_get_token_ticker_all_depricated",
    "ledger_anchor_find",
    "ledger_anchor_load",
    "ledger_anchor_purge",
    "ledger_anchor_unload",
    "ledger_anchor_unload_set_callback",
    "ledger_anchor_verify",
    "ledger_anchors_aggregate",
    "ledger_balance_change_notifier_register",
    "ledger_balance_change_notifier_unregister",
    "ledger_balance_info",
    "ledger_bridged_tx_notify_add",
    "ledger_cache_enabled",
    "ledger_calc_balance",
    "ledger_calc_balance_full",
    "ledger_callbacks_register",
    "ledger_chain_purge",
    "ledger_check_condition_owner",
    "ledger_check_error_str",
    "ledger_cli_cmd_execute",
    "ledger_cli_cmd_get_commands",
    "ledger_cli_cmd_is_registered",
    "ledger_cli_cmd_register",
    "ledger_cli_cmd_unregister",
    "ledger_cli_dispatcher",
    "ledger_cli_tx_create",
    "ledger_cli_tx_create_json",
    "ledger_cli_tx_history",
    "ledger_cli_tx_verify",
    "ledger_coin_get_uncoloured_value",
    "ledger_colour_clear_callback",
    "ledger_count",
    "ledger_count_from_to",
    "ledger_create",
    "ledger_datum_is_enforced",
    "ledger_datum_iter_create",
    "ledger_datum_iter_delete",
    "ledger_datum_iter_get_first",
    "ledger_datum_iter_get_last",
    "ledger_datum_iter_get_next",
    "ledger_decree_apply",
    "ledger_decree_get_by_hash",
    "ledger_decree_get_min_num_of_signers",
    "ledger_decree_get_num_of_owners",
    "ledger_decree_get_owners_pkeys",
    "ledger_decree_init",
    "ledger_decree_load",
    "ledger_decree_purge",
    "ledger_decree_reset_applied",
    "ledger_decree_set_callback",
    "ledger_decree_verify",
    "ledger_decrees_get_by_type",
    "ledger_deduct_tx_tag",
    "ledger_emission_for_stake_lock_item_add",
    "ledger_event_find",
    "ledger_event_get_list",
    "ledger_event_get_list_ex",
    "ledger_event_notify_add",
    "ledger_event_pkey_add",
    "ledger_event_pkey_check",
    "ledger_event_pkey_list",
    "ledger_event_pkey_rm",
    "ledger_event_purge",
    "ledger_events_aggregate",
    "ledger_find_by_name",
    "ledger_find_by_net_id",
    "ledger_get_blockchain_time",
    "ledger_get_chain_info",
    "ledger_get_chain_info_by_name",
    "ledger_get_description_by_ticker",
    "ledger_get_final_chain_tx_hash",
    "ledger_get_first_chain_tx_hash",
    "ledger_get_gdb_group",
    "ledger_get_list_tx_cond_outs",
    "ledger_get_list_tx_outs_from_json",
    "ledger_get_locked_values",
    "ledger_get_name",
    "ledger_get_net_id",
    "ledger_get_pkey",
    "ledger_get_pkey_hash",
    "ledger_get_tx_cond_out",
    "ledger_get_tx_out_cond_linked_to_tx_in_cond",
    "ledger_get_txs",
    "ledger_get_utxo_for_value",
    "ledger_handle_free",
    "ledger_has_chain_id",
    "ledger_is_used_reward",
    "ledger_load_cache",
    "ledger_load_end",
    "ledger_out_cond_unspent_find_by_addr",
    "ledger_purge",
    "ledger_pvt_addr_check",
    "ledger_pvt_balance_update_for_addr",
    "ledger_pvt_cache_gdb_load_balances_callback",
    "ledger_pvt_cache_gdb_load_stake_lock_callback",
    "ledger_pvt_cache_gdb_load_tokens_callback",
    "ledger_pvt_emission_cache_update",
    "ledger_pvt_emission_item_find",
    "ledger_pvt_event_remove",
    "ledger_pvt_event_verify_add",
    "ledger_pvt_find_token",
    "ledger_pvt_threshold_txs_add",
    "ledger_pvt_threshold_txs_proc",
    "ledger_pvt_token_supply_check",
    "ledger_pvt_token_supply_check_update",
    "ledger_register_chain",
    "ledger_service_add",
    "ledger_set_blockchain_time",
    "ledger_set_blockchain_timer",
    "ledger_set_cache_tx_check_callback",
    "ledger_set_check_ds",
    "ledger_set_fee_callback",
    "ledger_set_fee_params",
    "ledger_set_get_cur_cell_callback",
    "ledger_set_hal_hrl",
    "ledger_set_load_mode",
    "ledger_set_mempool_callbacks",
    "ledger_set_native_ticker",
    "ledger_set_net_id",
    "ledger_set_poa_keys",
    "ledger_set_syncing_state",
    "ledger_set_wallet_cache_callbacks",
    "ledger_set_wallet_callbacks",
    "ledger_sign_data",
    "ledger_states_aggregate",
    "ledger_tax_callback_set",
    "ledger_threshold_info",
    "ledger_token_add",
    "ledger_token_add_check",
    "ledger_token_decl_all",
    "ledger_token_emission_add",
    "ledger_token_emission_add_check",
    "ledger_token_emission_find",
    "ledger_token_emission_load",
    "ledger_token_emissions_mark_hardfork",
    "ledger_token_get_auth_pkeys_hashes",
    "ledger_token_get_auth_signs_total",
    "ledger_token_get_auth_signs_valid",
    "ledger_token_get_emission_rate",
    "ledger_token_info",
    "ledger_token_info_by_name",
    "ledger_token_load",
    "ledger_token_purge",
    "ledger_token_ticker_check",
    "ledger_token_tx_item_list",
    "ledger_tx_action_str",
    "ledger_tx_action_str_to_action_t",
    "ledger_tx_add",
    "ledger_tx_add_check",
    "ledger_tx_add_notify",
    "ledger_tx_cache_find_out_cond_all",
    "ledger_tx_calculate_main_ticker",
    "ledger_tx_check_recipient",
    "ledger_tx_clear_colour",
    "ledger_tx_create_event",
    "ledger_tx_create_from_emission",
    "ledger_tx_find_by_addr",
    "ledger_tx_find_by_hash",
    "ledger_tx_get_token_ticker_by_hash",
    "ledger_tx_get_trackers",
    "ledger_tx_hash_is_used_out_item",
    "ledger_tx_load",
    "ledger_tx_poa_signed",
    "ledger_tx_remove",
    "ledger_tx_service_info",
    "ledger_tx_tag_str_by_uid",
    "ledger_tx_unspent_find_by_hash",
    "ledger_unregister_chain",
    "ledger_verificator_add",
    "ledger_voting_verificator_add",
]

NOARG_INT_METHODS = [
    "ledger_cli_cmd_registry_init",
    "ledger_cli_module_init",
    "ledger_cli_tx_init",
    "ledger_init",
    "ledger_tx_builders_register",
]

NOARG_NONE_METHODS = [
    "ledger_cli_cmd_registry_deinit",
    "ledger_cli_error_codes_init",
    "ledger_cli_module_deinit",
    "ledger_cli_tx_deinit",
    "ledger_deinit",
    "ledger_tx_builders_unregister",
]

NOARG_LIST_METHODS = [
    "ledger_cli_cmd_get_categories",
]

NOARG_DICT_METHODS = [
    "ledger_callbacks_get",
]

NOARG_CAPSULE_METHODS = [
    "ledger_create_options_default",
]


def test_ledger_methods_present():
    missing = [name for name in LEDGER_METHODS if not hasattr(cf, name)]
    assert not missing, f"Missing ledger wrappers: {', '.join(missing)}"


@pytest.mark.parametrize("name", REQUIRES_ARGS)
def test_ledger_methods_require_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


@pytest.mark.parametrize("name", NOARG_INT_METHODS)
def test_ledger_noarg_int_methods(sdk_initialized, name):
    result = getattr(cf, name)()
    assert isinstance(result, int)


@pytest.mark.parametrize("name", NOARG_NONE_METHODS)
def test_ledger_noarg_none_methods(sdk_initialized, name):
    result = getattr(cf, name)()
    assert result is None


@pytest.mark.parametrize("name", NOARG_LIST_METHODS)
def test_ledger_noarg_list_methods(sdk_initialized, name):
    result = getattr(cf, name)()
    assert isinstance(result, list)
    for item in result:
        assert isinstance(item, str)


@pytest.mark.parametrize("name", NOARG_DICT_METHODS)
def test_ledger_noarg_dict_methods(sdk_initialized, name):
    result = getattr(cf, name)()
    assert isinstance(result, dict)


@pytest.mark.parametrize("name", NOARG_CAPSULE_METHODS)
def test_ledger_noarg_capsule_methods(sdk_initialized, name):
    result = getattr(cf, name)()
    if result is not None:
        assert is_capsule(result)


def test_ledger_callbacks_get_default_keys(sdk_initialized):
    result = cf.ledger_callbacks_get()
    assert isinstance(result, dict)
    assert set(result.keys()) >= {
        "addr_to_wallet_name",
        "wallet_info_to_json",
        "tx_added",
        "user_data",
    }


def test_ledger_helper_strings():
    assert isinstance(cf.ledger_check_error_str(0), str)
    action = cf.ledger_tx_action_str(0)
    assert isinstance(action, str)
    assert isinstance(cf.ledger_tx_action_str_to_action_t(action), int)
    tag = cf.ledger_tx_tag_str_by_uid(0)
    assert tag is None or isinstance(tag, str)


def test_ledger_cli_cmd_get_commands_returns_list(sdk_initialized):
    result = cf.ledger_cli_cmd_get_commands("ledger")
    assert isinstance(result, list)


def test_ledger_cli_parse_helpers(sdk_initialized):
    result = cf.cli_parse_hash_out_type(["hash"], 0)
    assert isinstance(result, tuple)
    assert len(result) == 2
    assert isinstance(result[0], int)
    assert result[1] is None or isinstance(result[1], str)

    result = cf.cli_parse_pagination(["--offset", "1", "--limit", "1"])
    assert isinstance(result, tuple)
    assert len(result) == 3
    assert isinstance(result[0], int)
    assert isinstance(result[1], int)
    assert isinstance(result[2], int)


def test_ledger_cli_cmd_execute_returns_tuple(sdk_initialized):
    cf.ledger_cli_cmd_registry_init()
    try:
        result = cf.ledger_cli_cmd_execute("ledger", "help", ["ledger", "help"])
        assert isinstance(result, tuple)
        assert len(result) == 2
        assert isinstance(result[0], int)
        assert result[1] is None or isinstance(result[1], str)
    finally:
        cf.ledger_cli_cmd_registry_deinit()


def test_ledger_cli_cmd_is_registered_returns_bool(sdk_initialized):
    assert isinstance(cf.ledger_cli_cmd_is_registered("ledger"), bool)
    assert isinstance(cf.ledger_cli_cmd_is_registered("ledger", None), bool)


def test_ledger_cli_cmd_register_rejects_non_capsule(sdk_initialized):
    with pytest.raises(TypeError):
        cf.ledger_cli_cmd_register("ledger", "noop", object())


def test_ledger_tx_action_str_rejects_invalid():
    with pytest.raises(TypeError):
        cf.ledger_tx_action_str("bad")


def test_ledger_tx_tag_str_by_uid_invalid():
    with pytest.raises(TypeError):
        cf.ledger_tx_tag_str_by_uid("bad")
