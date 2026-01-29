import uuid
import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_FAST_SIZE = 32
HASH_FAST_BYTES = b"\x00" * HASH_FAST_SIZE
HASH_FAST_BAD = b"\x00" * (HASH_FAST_SIZE - 1)
HASH_FAST_STR = "0x" + ("00" * HASH_FAST_SIZE)
UINT256_SIZE = 32
NODE_ADDR_UINT = 0x0102030405060708
NODE_ADDR_STR = "0001::0002::0003::0004"


def is_capsule(obj):
    return type(obj).__name__ == "PyCapsule"


def assert_uint256_bytes(value):
    assert isinstance(value, (bytes, bytearray))
    assert len(value) == UINT256_SIZE


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
def chain_context(sdk_initialized):
    uid = uuid.uuid4().int & 0xFFFFFFFF
    net_id = 10000 + (uid % 10000)
    chain_id = 20000 + (uid % 10000)
    net_name = f"testnet_{uid}"
    chain_name = f"chain_{uid}"
    try:
        chain = cf.dap_chain_create(net_name, chain_name, net_id, chain_id)
    except Exception as exc:
        pytest.skip(f"Chain create failed: {exc}")
    if chain is None:
        pytest.skip("Chain create returned None")

    deleted = {"value": False}

    def _delete():
        if not deleted["value"]:
            cf.dap_chain_delete(chain)
            deleted["value"] = True

    yield chain, net_id

    if not deleted["value"]:
        _delete()


CONSENSUS_METHODS = [
    "dap_chain_type_dag_poa_deinit",
    "dap_chain_type_dag_poa_get_auth_certs",
    "dap_chain_type_dag_poa_init",
    "dap_chain_type_dag_poa_presign_callback_set",
    "dap_chain_cs_esbocs_deinit",
    "dap_chain_cs_esbocs_init",
    "dap_chain_esbocs_add_block_collect",
    "dap_chain_esbocs_add_validator_to_clusters",
    "dap_chain_esbocs_get_autocollect_status",
    "dap_chain_esbocs_get_collecting_level",
    "dap_chain_esbocs_get_fee",
    "dap_chain_esbocs_get_min_validators_count",
    "dap_chain_esbocs_get_sign_key",
    "dap_chain_esbocs_get_sign_pkey",
    "dap_chain_esbocs_hardfork_engaged",
    "dap_chain_esbocs_remove_validator_from_clusters",
    "dap_chain_esbocs_set_custom_metadata_callback",
    "dap_chain_esbocs_set_emergency_validator",
    "dap_chain_esbocs_set_empty_block_every_times",
    "dap_chain_esbocs_set_hardfork_complete",
    "dap_chain_esbocs_set_hardfork_prepare",
    "dap_chain_esbocs_set_hardfork_state",
    "dap_chain_esbocs_set_min_validators_count",
    "dap_chain_esbocs_set_presign_callback",
    "dap_chain_esbocs_set_signs_struct_check",
    "dap_chain_esbocs_start_timer",
    "dap_chain_esbocs_started",
    "dap_chain_esbocs_stop_timer",
    "dap_chain_cs_esbocs_decree_deinit",
    "dap_chain_cs_esbocs_decree_init",
    "dap_chain_cs_add",
    "dap_chain_cs_add_block_collect",
    "dap_chain_cs_add_validator",
    "dap_chain_cs_create",
    "dap_chain_cs_deinit",
    "dap_chain_cs_get_autocollect_status",
    "dap_chain_cs_get_callbacks",
    "dap_chain_cs_get_collecting_level",
    "dap_chain_cs_get_fee",
    "dap_chain_cs_get_fee_group",
    "dap_chain_cs_get_min_validators_count",
    "dap_chain_cs_get_reward_group",
    "dap_chain_cs_get_sign_key",
    "dap_chain_cs_get_sign_pkey",
    "dap_chain_cs_hardfork_engaged",
    "dap_chain_cs_init",
    "dap_chain_cs_is_started",
    "dap_chain_cs_load",
    "dap_chain_cs_mempool_datum_add",
    "dap_chain_cs_purge",
    "dap_chain_cs_remove_validator",
    "dap_chain_cs_set_callbacks",
    "dap_chain_cs_set_hardfork_state",
    "dap_chain_cs_set_min_validators_count",
    "dap_chain_cs_stake_check_pkey_hash",
    "dap_chain_cs_stake_hardfork_data_import",
    "dap_chain_cs_stake_switch_table",
    "dap_chain_cs_start",
    "dap_chain_cs_stop",
    "dap_chain_validator_api_check_key_delegated",
    "dap_chain_validator_api_deinit",
    "dap_chain_validator_api_get_allowed_min_value",
    "dap_chain_validator_api_get_node_addr",
    "dap_chain_validator_api_get_pkey_by_hash",
    "dap_chain_validator_api_get_validators",
    "dap_chain_validator_api_get_value",
    "dap_chain_validator_api_hardfork_tx_update",
    "dap_chain_validator_api_init",
    "dap_chain_validator_api_key_delegate",
    "dap_chain_validator_api_mark_validator_active",
    "dap_chain_validator_api_register",
]


def test_consensus_methods_present():
    missing = [name for name in CONSENSUS_METHODS if not hasattr(cf, name)]
    assert not missing, f"Missing consensus methods: {missing}"


NOARG_INT_METHODS = [
    "dap_chain_type_dag_poa_init",
    "dap_chain_cs_esbocs_init",
    "dap_chain_cs_esbocs_decree_init",
    "dap_chain_cs_init",
    "dap_chain_validator_api_init",
]

NOARG_NONE_METHODS = [
    "dap_chain_type_dag_poa_deinit",
    "dap_chain_cs_esbocs_deinit",
    "dap_chain_cs_esbocs_decree_deinit",
    "dap_chain_cs_deinit",
    "dap_chain_validator_api_deinit",
]


@pytest.mark.parametrize("method", NOARG_INT_METHODS)
def test_noarg_int_methods(sdk_initialized, method):
    result = getattr(cf, method)()
    assert isinstance(result, int)


@pytest.mark.parametrize("method", NOARG_NONE_METHODS)
def test_noarg_none_methods(sdk_initialized, method):
    result = getattr(cf, method)()
    assert result is None


def test_dag_poa_get_auth_certs(chain_context):
    chain, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_type_dag_poa_get_auth_certs(chain)


def test_dag_poa_presign_callback_set(chain_context):
    chain, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_type_dag_poa_presign_callback_set(chain, None, None)


def test_cs_create_load_invalid_config(chain_context):
    chain, _ = chain_context
    with pytest.raises(TypeError):
        cf.dap_chain_cs_create(chain, object())
    with pytest.raises(TypeError):
        cf.dap_chain_cs_load(chain, object())


def test_cs_start_stop_purge(chain_context):
    chain, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_cs_start(chain)
    with pytest.raises(ValueError):
        cf.dap_chain_cs_stop(chain)
    with pytest.raises(ValueError):
        cf.dap_chain_cs_purge(chain)


def test_cs_callbacks_roundtrip(chain_context):
    chain, _ = chain_context
    assert cf.dap_chain_cs_get_callbacks(chain) is None
    assert cf.dap_chain_cs_set_callbacks(chain, None) is None
    assert cf.dap_chain_cs_get_callbacks(chain) is None
    with pytest.raises(TypeError):
        cf.dap_chain_cs_set_callbacks(chain, object())


def test_cs_group_getters(chain_context):
    chain, _ = chain_context
    fee_group = cf.dap_chain_cs_get_fee_group(chain, "testnet")
    reward_group = cf.dap_chain_cs_get_reward_group(chain, "testnet")
    assert fee_group is None or isinstance(fee_group, str)
    assert reward_group is None or isinstance(reward_group, str)


def test_cs_fee_collecting_level(chain_context):
    chain, _ = chain_context
    fee = cf.dap_chain_cs_get_fee(chain)
    level = cf.dap_chain_cs_get_collecting_level(chain)
    assert_uint256_bytes(fee)
    assert_uint256_bytes(level)


def test_cs_sign_getters(chain_context):
    chain, _ = chain_context
    sign_pkey = cf.dap_chain_cs_get_sign_pkey(chain)
    sign_key = cf.dap_chain_cs_get_sign_key(chain)
    assert sign_pkey is None or is_capsule(sign_pkey)
    assert sign_key is None or is_capsule(sign_key)


def test_cs_status_flags(chain_context):
    chain, _ = chain_context
    assert isinstance(cf.dap_chain_cs_get_autocollect_status(chain), bool)
    assert isinstance(cf.dap_chain_cs_hardfork_engaged(chain), bool)
    assert isinstance(cf.dap_chain_cs_is_started(chain), bool)


def test_cs_min_validators_count(chain_context):
    chain, _ = chain_context
    assert isinstance(cf.dap_chain_cs_get_min_validators_count(chain), int)
    assert isinstance(cf.dap_chain_cs_set_min_validators_count(chain, 1), int)
    with pytest.raises(ValueError):
        cf.dap_chain_cs_set_min_validators_count(chain, 70000)


def test_cs_add_remove_validator(chain_context):
    chain, _ = chain_context
    assert isinstance(cf.dap_chain_cs_add_validator(chain, NODE_ADDR_UINT), int)
    assert isinstance(cf.dap_chain_cs_remove_validator(chain, NODE_ADDR_UINT), int)
    assert isinstance(cf.dap_chain_cs_add_validator(chain, NODE_ADDR_STR), int)
    assert isinstance(cf.dap_chain_cs_remove_validator(chain, NODE_ADDR_STR), int)
    with pytest.raises(ValueError):
        cf.dap_chain_cs_add_validator(chain, "bad_addr")


def test_cs_add_block_collect(chain_context):
    chain, _ = chain_context
    assert cf.dap_chain_cs_add_block_collect(chain, None, None, 0) is None
    with pytest.raises(TypeError):
        cf.dap_chain_cs_add_block_collect(object(), None, None, 0)
    with pytest.raises(TypeError):
        cf.dap_chain_cs_add_block_collect(chain, object(), None, 0)


def test_cs_set_hardfork_state(chain_context):
    chain, _ = chain_context
    assert isinstance(cf.dap_chain_cs_set_hardfork_state(chain, True), int)
    assert isinstance(cf.dap_chain_cs_set_hardfork_state(chain, False), int)
    with pytest.raises(TypeError):
        cf.dap_chain_cs_set_hardfork_state(object(), True)


def test_cs_mempool_datum_add_invalid(chain_context):
    chain, _ = chain_context
    with pytest.raises(TypeError):
        cf.dap_chain_cs_mempool_datum_add(object(), object(), "hex")
    with pytest.raises(TypeError):
        cf.dap_chain_cs_mempool_datum_add(chain, object(), "hex")
    with pytest.raises(TypeError):
        cf.dap_chain_cs_mempool_datum_add(chain, object(), 123)


def test_cs_stake_check_pkey_hash(chain_context):
    chain, _ = chain_context
    result = cf.dap_chain_cs_stake_check_pkey_hash(chain, HASH_FAST_BYTES)
    assert isinstance(result, tuple)
    assert len(result) == 3
    code, tax, addr = result
    assert isinstance(code, int)
    assert_uint256_bytes(tax)
    assert is_capsule(addr)

    result = cf.dap_chain_cs_stake_check_pkey_hash(chain, HASH_FAST_STR)
    assert isinstance(result, tuple)
    assert len(result) == 3
    with pytest.raises(ValueError):
        cf.dap_chain_cs_stake_check_pkey_hash(chain, HASH_FAST_BAD)


def test_cs_stake_hardfork_data_import(chain_context):
    chain, _ = chain_context
    assert isinstance(cf.dap_chain_cs_stake_hardfork_data_import(chain, HASH_FAST_BYTES), int)
    with pytest.raises(ValueError):
        cf.dap_chain_cs_stake_hardfork_data_import(chain, HASH_FAST_BAD)


def test_cs_stake_switch_table(chain_context):
    chain, _ = chain_context
    assert isinstance(cf.dap_chain_cs_stake_switch_table(chain, True), int)
    assert isinstance(cf.dap_chain_cs_stake_switch_table(chain, False), int)
    with pytest.raises(TypeError):
        cf.dap_chain_cs_stake_switch_table(object(), True)


def test_cs_add(sdk_initialized):
    name = f"unit_test_{uuid.uuid4().hex}"
    assert cf.dap_chain_cs_add(name, None) is None
    with pytest.raises(TypeError):
        cf.dap_chain_cs_add(name, object())


def test_esbocs_timers_and_status(sdk_initialized):
    assert isinstance(cf.dap_chain_esbocs_started(1), bool)
    assert cf.dap_chain_esbocs_stop_timer(1) is None
    assert cf.dap_chain_esbocs_start_timer(1) is None
    assert isinstance(cf.dap_chain_esbocs_get_autocollect_status(1), bool)


def test_esbocs_net_getters(sdk_initialized):
    pkey = cf.dap_chain_esbocs_get_sign_pkey(1)
    assert pkey is None or is_capsule(pkey)
    fee = cf.dap_chain_esbocs_get_fee(1)
    assert_uint256_bytes(fee)


def test_esbocs_add_remove_validator_clusters(sdk_initialized):
    assert isinstance(cf.dap_chain_esbocs_add_validator_to_clusters(1, NODE_ADDR_UINT), bool)
    assert isinstance(cf.dap_chain_esbocs_remove_validator_from_clusters(1, NODE_ADDR_UINT), bool)
    assert isinstance(cf.dap_chain_esbocs_add_validator_to_clusters(1, NODE_ADDR_STR), bool)
    assert isinstance(cf.dap_chain_esbocs_remove_validator_from_clusters(1, NODE_ADDR_STR), bool)
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_add_validator_to_clusters(1, "bad_addr")


def test_esbocs_chain_getters(chain_context):
    chain, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_get_collecting_level(chain)
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_get_sign_key(chain)


def test_esbocs_min_validators_count(chain_context):
    chain, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_set_min_validators_count(chain, 1)
    assert isinstance(cf.dap_chain_esbocs_get_min_validators_count(1), int)
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_set_min_validators_count(chain, 70000)


def test_esbocs_emergency_validator(chain_context):
    chain, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_set_emergency_validator(chain, True, 0, HASH_FAST_BYTES)


def test_esbocs_signs_struct_check(chain_context):
    chain, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_set_signs_struct_check(chain, True)
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_set_signs_struct_check(chain, False)


def test_esbocs_hardfork_controls(chain_context):
    chain, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_set_hardfork_prepare(chain, 1, 0)
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_set_hardfork_complete(chain)
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_hardfork_engaged(chain)
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_set_hardfork_state(chain, True)


def test_esbocs_custom_metadata(chain_context):
    chain, _ = chain_context
    assert isinstance(cf.dap_chain_esbocs_set_custom_metadata_callback(1, None), int)
    assert isinstance(cf.dap_chain_esbocs_set_presign_callback(1, None), int)
    with pytest.raises(TypeError):
        cf.dap_chain_esbocs_set_custom_metadata_callback(1, object())
    with pytest.raises(TypeError):
        cf.dap_chain_esbocs_set_presign_callback(1, object())


def test_esbocs_set_empty_block_every_times(chain_context):
    chain, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_set_empty_block_every_times(chain, 0)
    with pytest.raises(ValueError):
        cf.dap_chain_esbocs_set_empty_block_every_times(chain, 70000)


def test_esbocs_add_block_collect(sdk_initialized):
    assert cf.dap_chain_esbocs_add_block_collect(None, None, 0) is None
    with pytest.raises(TypeError):
        cf.dap_chain_esbocs_add_block_collect(object(), None, 0)


def test_validator_api_register_invalid(sdk_initialized):
    with pytest.raises(TypeError):
        cf.dap_chain_validator_api_register(object())


def test_validator_api_get_validators(sdk_initialized):
    result = cf.dap_chain_validator_api_get_validators(1, False)
    assert result is None or is_capsule(result)


def test_validator_api_check_key_delegated(sdk_initialized):
    assert isinstance(cf.dap_chain_validator_api_check_key_delegated("null"), int)
    with pytest.raises(ValueError):
        cf.dap_chain_validator_api_check_key_delegated(b"\x00")


def test_validator_api_mark_validator_active(sdk_initialized):
    assert isinstance(cf.dap_chain_validator_api_mark_validator_active("null", True), int)
    with pytest.raises(ValueError):
        cf.dap_chain_validator_api_mark_validator_active(b"\x00", True)


def test_validator_api_get_pkey_by_hash(sdk_initialized):
    result = cf.dap_chain_validator_api_get_pkey_by_hash(1, HASH_FAST_BYTES)
    assert result is None or is_capsule(result)
    with pytest.raises(ValueError):
        cf.dap_chain_validator_api_get_pkey_by_hash(1, HASH_FAST_BAD)


def test_validator_api_key_delegate_invalid(sdk_initialized):
    with pytest.raises(TypeError):
        cf.dap_chain_validator_api_key_delegate(object(), "null", None, None, 0, NODE_ADDR_UINT)


def test_validator_api_get_allowed_min_value(sdk_initialized):
    value = cf.dap_chain_validator_api_get_allowed_min_value(1)
    assert_uint256_bytes(value)


def test_validator_api_hardfork_tx_update_invalid(sdk_initialized):
    with pytest.raises(TypeError):
        cf.dap_chain_validator_api_hardfork_tx_update(object())


def test_validator_api_get_node_addr_invalid(sdk_initialized):
    with pytest.raises(TypeError):
        cf.dap_chain_validator_api_get_node_addr(object())


def test_validator_api_get_value_invalid(sdk_initialized):
    with pytest.raises(TypeError):
        cf.dap_chain_validator_api_get_value(object())
