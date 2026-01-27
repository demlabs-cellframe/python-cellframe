import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_SIZE = 32
HASH_BYTES = b"\x00" * HASH_SIZE
BAD_HASH_BYTES = b"\x00" * (HASH_SIZE - 1)
HASH_STR = "0" * (HASH_SIZE * 2)
UINT256_BYTES = b"\x00" * 32
TOKEN_TICKER = "CELL"
NODE_ADDR_INT = 1


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
    return True


@pytest.fixture
def addr_null(sdk_initialized):
    if not hasattr(cf, "dap_chain_addr_from_str"):
        pytest.skip("dap_chain_addr_from_str not available")
    return cf.dap_chain_addr_from_str("null")


BASE_NOARG_METHODS = [
    "net_srv_app_db_init",
    "net_srv_app_db_deinit",
    "net_srv_app_init",
    "net_srv_app_deinit",
    "net_srv_bridge_init",
    "net_srv_bridge_deinit",
    "net_srv_datum_init",
    "net_srv_datum_deinit",
]

BASE_ARG_METHODS = [
    "net_srv_file_datum_data_read",
    "net_srv_datum_custom_add",
]


@pytest.mark.parametrize("name", BASE_NOARG_METHODS)
def test_net_srv_base_noarg_methods_reject_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func(1)


@pytest.mark.parametrize("name", BASE_ARG_METHODS)
def test_net_srv_base_arg_methods_require_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


def test_net_srv_base_init_deinit_smoke(sdk_initialized):
    assert isinstance(cf.net_srv_app_db_init(), int)
    assert cf.net_srv_app_db_deinit() is None
    assert isinstance(cf.net_srv_app_init(), int)
    assert cf.net_srv_app_deinit() is None
    assert isinstance(cf.net_srv_bridge_init(), int)
    assert cf.net_srv_bridge_deinit() is None
    assert isinstance(cf.net_srv_datum_init(), int)
    assert cf.net_srv_datum_deinit() is None


def test_net_srv_file_datum_data_read_roundtrip(tmp_path):
    payload = b"datum-bytes"
    file_path = tmp_path / "datum.bin"
    file_path.write_bytes(payload)

    data = cf.net_srv_file_datum_data_read(str(file_path))
    assert data == payload


def test_net_srv_file_datum_data_read_rejects_invalid_type():
    with pytest.raises(TypeError):
        cf.net_srv_file_datum_data_read(123)


def test_net_srv_datum_custom_add_rejects_invalid_chain():
    with pytest.raises(TypeError):
        cf.net_srv_datum_custom_add(object(), b"data")


EXT_METHODS = [
    "net_srv_stake_ext_init",
    "net_srv_stake_ext_deinit",
    "net_srv_stake_ext_find",
    "net_srv_stake_ext_get_list",
    "net_srv_stake_ext_get_detailed",
    "net_srv_stake_ext_get_stats",
    "net_srv_stake_ext_lock_create",
    "net_srv_stake_ext_unlock_create",
    "net_srv_stake_ext_delete",
    "net_srv_stake_ext_status_to_str",
    "net_srv_stake_ext_status_from_event_type",
    "net_srv_tx_event_data_time_unit_to_str",
    "net_srv_stake_ext_started_tx_event_create",
    "net_srv_stake_ext_ended_tx_event_create",
    "net_srv_stake_init",
    "net_srv_stake_deinit",
    "net_srv_stake_pos_delegate_init",
    "net_srv_stake_pos_delegate_deinit",
    "net_srv_stake_decree_init",
    "net_srv_stake_decree_deinit",
    "net_srv_stake_key_delegate",
    "net_srv_stake_key_invalidate",
    "net_srv_stake_key_update",
    "net_srv_stake_pkey_update",
    "net_srv_stake_set_allowed_min_value",
    "net_srv_stake_get_allowed_min_value",
    "net_srv_stake_set_percent_max",
    "net_srv_stake_get_percent_max",
    "net_srv_stake_key_delegated",
    "net_srv_stake_verify_key_and_node",
    "net_srv_stake_get_validators",
    "net_srv_stake_get_fee_validators",
    "net_srv_stake_load_cache",
    "net_srv_stake_check_validator",
    "net_srv_stake_decree_approve",
    "net_srv_stake_mark_validator_active",
    "net_srv_stake_check_pkey_hash",
    "net_srv_stake_get_total_weight",
    "net_srv_stake_get_total_keys",
    "net_srv_stake_add_approving_decree_info",
    "net_srv_stake_remove_approving_decree_info",
    "net_srv_stake_hardfork_data_export",
    "net_srv_stake_hardfork_data_import",
    "net_srv_stake_hardfork_data_verify",
    "net_srv_stake_switch_table",
    "net_srv_stake_get_pkey_by_hash",
    "net_srv_stake_hardfork_tx_update",
    "net_srv_stake_get_validator_ext",
    "net_srv_stake_tx_builders_register",
    "net_srv_stake_tx_builders_unregister",
    "net_srv_stake_tx_create_lock",
    "net_srv_stake_tx_create_unlock",
    "net_srv_stake_tx_create_delegate",
    "net_srv_stake_tx_create_invalidate",
    "net_srv_xchange_init",
    "net_srv_xchange_deinit",
    "net_srv_xchange_create",
    "net_srv_xchange_remove",
    "net_srv_xchange_get_fee",
    "net_srv_xchange_get_order_completion_rate",
    "net_srv_xchange_purchase",
    "net_srv_xchange_get_tx_xchange",
    "net_srv_xchange_get_prices",
    "net_srv_xchange_get_order_status",
    "net_srv_xchange_print_fee",
    "net_srv_xchange_print_fee_json",
    "net_srv_xchange_tx_get_type",
    "net_srv_xchange_compose_init",
    "net_srv_xchange_compose_deinit",
    "net_srv_tx_compose_xchange_create",
    "net_srv_tx_compose_xchange_order_remove",
    "net_srv_tx_compose_xchange_purchase",
    "net_srv_xchange_find_last_tx",
    "net_srv_xchange_price_from_order",
    "net_srv_xchange_tx_create_order",
    "net_srv_xchange_tx_create_invalidate",
    "net_srv_xchange_tx_create_purchase",
    "net_srv_voting_init",
    "net_srv_voting_deinit",
    "net_srv_voting_create",
    "net_srv_voting_get_expiration_time",
    "net_srv_voting_get_result",
    "net_srv_vote_create",
    "net_srv_voting_list",
    "net_srv_voting_extract_info",
    "net_srv_voting_info_free",
    "net_srv_get_options_list_from_str",
    "net_srv_vote_cancel",
    "net_srv_voting_compose_init",
    "net_srv_voting_compose_deinit",
    "net_srv_tx_compose_poll_create",
    "net_srv_tx_compose_poll_vote",
    "net_srv_voting_tx_create_poll",
    "net_srv_voting_tx_create_vote",
]

STAKE_EXT_ARG_METHODS = [
    "net_srv_stake_ext_find",
    "net_srv_stake_ext_get_list",
    "net_srv_stake_ext_get_detailed",
    "net_srv_stake_ext_get_stats",
    "net_srv_stake_ext_lock_create",
    "net_srv_stake_ext_unlock_create",
    "net_srv_stake_ext_delete",
    "net_srv_stake_ext_status_to_str",
    "net_srv_stake_ext_status_from_event_type",
    "net_srv_tx_event_data_time_unit_to_str",
    "net_srv_stake_ext_started_tx_event_create",
    "net_srv_stake_ext_ended_tx_event_create",
]

STAKE_CORE_ARG_METHODS = [
    "net_srv_stake_key_delegate",
    "net_srv_stake_key_invalidate",
    "net_srv_stake_key_update",
    "net_srv_stake_pkey_update",
    "net_srv_stake_set_allowed_min_value",
    "net_srv_stake_get_allowed_min_value",
    "net_srv_stake_set_percent_max",
    "net_srv_stake_get_percent_max",
    "net_srv_stake_key_delegated",
    "net_srv_stake_verify_key_and_node",
    "net_srv_stake_get_validators",
    "net_srv_stake_get_fee_validators",
    "net_srv_stake_load_cache",
    "net_srv_stake_check_validator",
    "net_srv_stake_decree_approve",
    "net_srv_stake_mark_validator_active",
    "net_srv_stake_check_pkey_hash",
    "net_srv_stake_get_total_weight",
    "net_srv_stake_get_total_keys",
    "net_srv_stake_add_approving_decree_info",
    "net_srv_stake_remove_approving_decree_info",
    "net_srv_stake_hardfork_data_export",
    "net_srv_stake_hardfork_data_import",
    "net_srv_stake_hardfork_data_verify",
    "net_srv_stake_switch_table",
    "net_srv_stake_get_pkey_by_hash",
    "net_srv_stake_hardfork_tx_update",
    "net_srv_stake_get_validator_ext",
    "net_srv_stake_tx_create_lock",
    "net_srv_stake_tx_create_unlock",
    "net_srv_stake_tx_create_delegate",
    "net_srv_stake_tx_create_invalidate",
]

VOTING_ARG_METHODS = [
    "net_srv_voting_create",
    "net_srv_voting_get_expiration_time",
    "net_srv_voting_get_result",
    "net_srv_vote_create",
    "net_srv_voting_list",
    "net_srv_voting_extract_info",
    "net_srv_voting_info_free",
    "net_srv_get_options_list_from_str",
    "net_srv_vote_cancel",
    "net_srv_tx_compose_poll_create",
    "net_srv_tx_compose_poll_vote",
    "net_srv_voting_tx_create_poll",
    "net_srv_voting_tx_create_vote",
]

XCHANGE_ARG_METHODS = [
    "net_srv_xchange_create",
    "net_srv_xchange_remove",
    "net_srv_xchange_get_fee",
    "net_srv_xchange_get_order_completion_rate",
    "net_srv_xchange_get_order_status",
    "net_srv_xchange_tx_get_type",
    "net_srv_xchange_find_last_tx",
    "net_srv_xchange_price_from_order",
    "net_srv_xchange_tx_create_order",
    "net_srv_xchange_tx_create_invalidate",
    "net_srv_xchange_tx_create_purchase",
]

EXT_ARG_METHODS = STAKE_EXT_ARG_METHODS + STAKE_CORE_ARG_METHODS + VOTING_ARG_METHODS + XCHANGE_ARG_METHODS

EXT_NOARG_INT_METHODS = [
    "net_srv_stake_ext_init",
    "net_srv_stake_init",
    "net_srv_stake_pos_delegate_init",
    "net_srv_stake_decree_init",
    "net_srv_stake_tx_builders_register",
    "net_srv_voting_init",
    "net_srv_voting_compose_init",
    "net_srv_xchange_init",
    "net_srv_xchange_compose_init",
]

EXT_NOARG_NONE_METHODS = [
    "net_srv_stake_ext_deinit",
    "net_srv_stake_deinit",
    "net_srv_stake_pos_delegate_deinit",
    "net_srv_stake_decree_deinit",
    "net_srv_stake_tx_builders_unregister",
    "net_srv_voting_deinit",
    "net_srv_voting_compose_deinit",
    "net_srv_xchange_deinit",
    "net_srv_xchange_compose_deinit",
]

NOT_IMPLEMENTED_METHODS = [
    "net_srv_xchange_purchase",
    "net_srv_xchange_get_tx_xchange",
    "net_srv_xchange_get_prices",
    "net_srv_xchange_print_fee",
    "net_srv_xchange_print_fee_json",
    "net_srv_tx_compose_xchange_create",
    "net_srv_tx_compose_xchange_order_remove",
    "net_srv_tx_compose_xchange_purchase",
]


def test_net_srv_ext_methods_present():
    missing = [name for name in EXT_METHODS if not hasattr(cf, name)]
    assert not missing, f"Missing service ext wrappers: {', '.join(missing)}"


@pytest.mark.parametrize("name", EXT_ARG_METHODS)
def test_net_srv_ext_methods_require_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


@pytest.mark.parametrize("name", EXT_NOARG_INT_METHODS)
def test_net_srv_ext_noarg_int_methods(sdk_initialized, name):
    result = getattr(cf, name)()
    assert isinstance(result, int)


@pytest.mark.parametrize("name", EXT_NOARG_NONE_METHODS)
def test_net_srv_ext_noarg_none_methods(sdk_initialized, name):
    result = getattr(cf, name)()
    assert result is None


@pytest.mark.parametrize("name", NOT_IMPLEMENTED_METHODS)
def test_net_srv_xchange_not_implemented_methods(name):
    func = getattr(cf, name)
    with pytest.raises(NotImplementedError):
        func()


def test_net_srv_tx_event_data_time_unit_to_str():
    assert isinstance(cf.net_srv_tx_event_data_time_unit_to_str(0), str)
    assert isinstance(cf.net_srv_tx_event_data_time_unit_to_str(999), str)


def test_net_srv_stake_ext_status_helpers():
    assert isinstance(cf.net_srv_stake_ext_status_to_str(0), str)
    assert isinstance(cf.net_srv_stake_ext_status_from_event_type(0), int)


def test_net_srv_stake_ext_started_event_create_smoke():
    data = cf.net_srv_stake_ext_started_tx_event_create(1, 60, 0, 0, 1, [1])
    assert data is None or isinstance(data, bytes)


def test_net_srv_stake_ext_started_event_create_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_stake_ext_started_tx_event_create(1, 0, 0, 0, 0, "bad")
    with pytest.raises(ValueError):
        cf.net_srv_stake_ext_started_tx_event_create(1, 0, 0, 0, 1, [])


def test_net_srv_stake_ext_ended_event_create_smoke():
    data = cf.net_srv_stake_ext_ended_tx_event_create(0, 1, [1])
    assert data is None or isinstance(data, bytes)


def test_net_srv_stake_ext_ended_event_create_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_stake_ext_ended_tx_event_create(0, 0, "bad")
    with pytest.raises(ValueError):
        cf.net_srv_stake_ext_ended_tx_event_create(0, 1, [])


def test_net_srv_stake_ext_find_rejects_invalid_net():
    with pytest.raises(TypeError):
        cf.net_srv_stake_ext_find(object(), HASH_STR)


def test_net_srv_stake_ext_get_list_rejects_invalid_net():
    with pytest.raises(TypeError):
        cf.net_srv_stake_ext_get_list(object())


def test_net_srv_stake_ext_get_detailed_rejects_invalid_net():
    with pytest.raises(TypeError):
        cf.net_srv_stake_ext_get_detailed(object(), HASH_STR)


def test_net_srv_stake_ext_get_stats_rejects_invalid_net():
    with pytest.raises(TypeError):
        cf.net_srv_stake_ext_get_stats(object())


def test_net_srv_stake_ext_delete_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_stake_ext_delete(object())


def test_net_srv_stake_ext_lock_unlock_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_stake_ext_lock_create(object(), object(), HASH_STR, 0, 0, 0, 0)
    with pytest.raises(TypeError):
        cf.net_srv_stake_ext_unlock_create(object(), object(), HASH_STR, 0)


def test_net_srv_stake_key_invalidate_smoke(addr_null):
    assert cf.net_srv_stake_key_invalidate(addr_null) is None


def test_net_srv_stake_key_update_rejects_invalid_hash(addr_null):
    with pytest.raises(ValueError):
        cf.net_srv_stake_key_update(addr_null, UINT256_BYTES, BAD_HASH_BYTES)


def test_net_srv_stake_key_update_smoke(addr_null):
    assert cf.net_srv_stake_key_update(addr_null, UINT256_BYTES, HASH_BYTES) is None


def test_net_srv_stake_key_delegated_smoke(addr_null):
    assert isinstance(cf.net_srv_stake_key_delegated(addr_null), int)


def test_net_srv_stake_verify_key_and_node_smoke(addr_null):
    assert isinstance(cf.net_srv_stake_verify_key_and_node(addr_null, NODE_ADDR_INT), int)


def test_net_srv_stake_mark_validator_active_smoke(addr_null):
    assert isinstance(cf.net_srv_stake_mark_validator_active(addr_null, True), int)


def test_net_srv_stake_check_validator_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_stake_check_validator(object(), HASH_BYTES, 0, 0)


def test_net_srv_stake_add_approving_decree_info_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_stake_add_approving_decree_info(object(), object())


def test_net_srv_stake_get_validator_ext_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_stake_get_validator_ext(object())


def test_net_srv_stake_tx_create_lock_rejects_invalid_used_outs(addr_null):
    with pytest.raises(TypeError):
        cf.net_srv_stake_tx_create_lock(
            object(), addr_null, TOKEN_TICKER, UINT256_BYTES, UINT256_BYTES, 0, UINT256_BYTES, None, None, 1, 1
        )


def test_net_srv_stake_tx_create_unlock_rejects_invalid_used_outs():
    with pytest.raises(TypeError):
        cf.net_srv_stake_tx_create_unlock(
            object(), HASH_BYTES, 0, TOKEN_TICKER, UINT256_BYTES, UINT256_BYTES, None, None
        )


def test_net_srv_stake_tx_create_delegate_rejects_invalid_used_outs(addr_null):
    with pytest.raises(TypeError):
        cf.net_srv_stake_tx_create_delegate(
            object(), UINT256_BYTES, UINT256_BYTES, addr_null, NODE_ADDR_INT, None, UINT256_BYTES, None, 1
        )


def test_net_srv_stake_tx_create_invalidate_rejects_invalid_used_outs():
    with pytest.raises(TypeError):
        cf.net_srv_stake_tx_create_invalidate(object(), HASH_BYTES, UINT256_BYTES)


def test_net_srv_stake_tx_create_unlock_rejects_invalid_hash():
    with pytest.raises(ValueError):
        cf.net_srv_stake_tx_create_unlock(
            None, BAD_HASH_BYTES, 0, TOKEN_TICKER, UINT256_BYTES, UINT256_BYTES, None, None
        )


def test_net_srv_stake_tx_create_invalidate_rejects_invalid_hash():
    with pytest.raises(ValueError):
        cf.net_srv_stake_tx_create_invalidate(None, BAD_HASH_BYTES, UINT256_BYTES)


def test_net_srv_stake_tx_create_lock_smoke(addr_null):
    tx = cf.net_srv_stake_tx_create_lock(
        None, addr_null, TOKEN_TICKER, UINT256_BYTES, UINT256_BYTES, 0, UINT256_BYTES, None, None, 1, 1
    )
    assert_capsule_or_none(tx)


def test_net_srv_stake_tx_create_unlock_smoke():
    tx = cf.net_srv_stake_tx_create_unlock(
        None, HASH_BYTES, 0, TOKEN_TICKER, UINT256_BYTES, UINT256_BYTES, None, None
    )
    assert_capsule_or_none(tx)


def test_net_srv_stake_tx_create_delegate_smoke(addr_null):
    tx = cf.net_srv_stake_tx_create_delegate(
        None, UINT256_BYTES, UINT256_BYTES, addr_null, NODE_ADDR_INT, None, UINT256_BYTES, None, 1
    )
    assert_capsule_or_none(tx)


def test_net_srv_stake_tx_create_invalidate_smoke():
    tx = cf.net_srv_stake_tx_create_invalidate(None, HASH_BYTES, UINT256_BYTES)
    assert_capsule_or_none(tx)


def test_net_srv_voting_create_rejects_invalid_options():
    with pytest.raises(TypeError):
        cf.net_srv_voting_create("Question", "bad", 0, 0, 0, 0, 0, object(), object(), TOKEN_TICKER)
    with pytest.raises(TypeError):
        cf.net_srv_voting_create("Question", [1], 0, 0, 0, 0, 0, object(), object(), TOKEN_TICKER)


def test_net_srv_vote_create_rejects_invalid_capsules():
    with pytest.raises(TypeError):
        cf.net_srv_vote_create(None, 0, object(), HASH_STR, 0, object())


def test_net_srv_voting_list_rejects_invalid_net():
    with pytest.raises(TypeError):
        cf.net_srv_voting_list(object())




def test_net_srv_voting_extract_info_rejects_invalid_net():
    with pytest.raises(TypeError):
        cf.net_srv_voting_extract_info(object(), HASH_STR)




def test_net_srv_voting_info_free_accepts_none():
    assert cf.net_srv_voting_info_free(None) is None


def test_net_srv_voting_info_free_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_voting_info_free(object())


def test_net_srv_get_options_list_from_str_smoke():
    result = cf.net_srv_get_options_list_from_str("yes,no")
    assert result == ["yes", "no"]


def test_net_srv_voting_get_expiration_time_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_voting_get_expiration_time(object(), HASH_STR)




def test_net_srv_voting_get_result_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_voting_get_result(object(), HASH_STR)




def test_net_srv_vote_cancel_rejects_invalid_capsules():
    with pytest.raises(TypeError):
        cf.net_srv_vote_cancel(0, object(), HASH_STR, object())




def test_net_srv_tx_compose_poll_create_smoke(addr_null):
    result = cf.net_srv_tx_compose_poll_create(
        0,
        "Question",
        "yes,no",
        "0",
        "0",
        "0",
        0,
        0,
        addr_null,
        TOKEN_TICKER,
    )
    assert result is None or isinstance(result, str)


def test_net_srv_tx_compose_poll_vote_smoke(addr_null):
    result = cf.net_srv_tx_compose_poll_vote(0, HASH_STR, "", "0", addr_null, "0")
    assert result is None or isinstance(result, str)


def test_net_srv_voting_tx_create_poll_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_voting_tx_create_poll(object(), "Question", ["yes"], 0, 0, 0, 0, 0, "bad", TOKEN_TICKER)
    with pytest.raises(TypeError):
        cf.net_srv_voting_tx_create_poll(object(), "Question", "bad", 0, 0, 0, 0, 0, "bad", TOKEN_TICKER)


def test_net_srv_voting_tx_create_vote_rejects_invalid_ledger():
    with pytest.raises(TypeError):
        cf.net_srv_voting_tx_create_vote(object(), HASH_STR, 0, 0, "bad")


def test_net_srv_xchange_create_rejects_invalid_capsules():
    with pytest.raises(TypeError):
        cf.net_srv_xchange_create(object(), TOKEN_TICKER, TOKEN_TICKER, 0, 0, 0, object())




def test_net_srv_xchange_remove_rejects_invalid_capsules():
    with pytest.raises(TypeError):
        cf.net_srv_xchange_remove(object(), HASH_STR, 0, object())




def test_net_srv_xchange_get_fee_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_xchange_get_fee(object())




def test_net_srv_xchange_get_order_completion_rate_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_xchange_get_order_completion_rate(object(), HASH_STR)




def test_net_srv_xchange_get_order_status_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_xchange_get_order_status(object(), HASH_STR)




def test_net_srv_xchange_tx_get_type_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_xchange_tx_get_type(object(), object())




def test_net_srv_xchange_tx_create_order_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_xchange_tx_create_order(object(), TOKEN_TICKER, TOKEN_TICKER, UINT256_BYTES, UINT256_BYTES, UINT256_BYTES, "bad")




def test_net_srv_xchange_tx_create_invalidate_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_xchange_tx_create_invalidate(object(), HASH_BYTES, UINT256_BYTES, "bad")




def test_net_srv_xchange_tx_create_purchase_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_xchange_tx_create_purchase(object(), HASH_BYTES, UINT256_BYTES, UINT256_BYTES, "bad")




def test_net_srv_xchange_find_last_tx_rejects_invalid_ledger(addr_null):
    with pytest.raises(TypeError):
        cf.net_srv_xchange_find_last_tx(object(), HASH_BYTES, addr_null)




def test_net_srv_xchange_price_from_order_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_xchange_price_from_order(object(), object(), 0, HASH_BYTES, HASH_BYTES, TOKEN_TICKER)
    with pytest.raises(TypeError):
        cf.net_srv_xchange_price_from_order(object(), object(), 0, HASH_BYTES, HASH_BYTES, TOKEN_TICKER, None, True)
