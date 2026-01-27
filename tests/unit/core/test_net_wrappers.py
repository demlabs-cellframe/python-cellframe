import ctypes
import uuid

import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_SIZE = 32
HASH_BYTES = b"\x00" * HASH_SIZE
BAD_HASH_BYTES = b"\x00" * (HASH_SIZE - 1)
DATUM_SMALL = b"\x00"
DATUM_BYTES = b"\x00" * 256
UINT256_BYTES = b"\x00" * 32

CHAIN_TYPE_INVALID = -1
NET_STATE_INVALID = 999

_PyCapsule_New = ctypes.pythonapi.PyCapsule_New
_PyCapsule_New.restype = ctypes.py_object
_PyCapsule_New.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]
_capsule_keepalive = []


def make_capsule(name):
    buf = ctypes.create_string_buffer(1)
    name_buf = ctypes.create_string_buffer(name.encode("ascii"))
    capsule = _PyCapsule_New(ctypes.addressof(buf), name_buf, None)
    _capsule_keepalive.extend([buf, name_buf])
    return capsule


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

    yield chain, net_name, chain_name

    if not deleted["value"]:
        _delete()


NET_METHODS = [
    "net_core_init",
    "net_register",
    "net_unregister",
    "net_by_name",
    "net_by_id",
    "net_parse_net_chain",
    "net_id_by_name",
    "net_id_parse",
    "net_count",
    "net_iter_start",
    "net_iter_next",
    "net_iterate",
    "net_get_name",
    "net_get_id",
    "net_get_state",
    "net_state_to_str",
    "net_state_to_str_user",
    "net_get_state_name",
    "net_state_go_to",
    "net_start",
    "net_stop",
    "net_get_native_ticker",
    "net_init",
    "net_deinit",
    "net_load_all",
    "net_try_online_all",
    "net_get_chain_by_name",
    "net_get_chain_by_id",
    "net_get_chain_by_type",
    "net_get_default_chain_by_type",
    "chain_get_name",
    "chain_get_id",
    "net_get_role",
    "net_get_acl_idx",
    "net_get_cur_addr_int",
    "net_get_cur_cell",
    "net_get_flag_sync_from_zero",
    "net_set_flag_sync_from_zero",
    "net_get_load_mode",
    "net_set_load_skip",
    "net_is_bridged",
    "net_get_target_state",
    "net_get_gdb_group_acl",
    "net_get_gdb_group_nochain_new",
    "net_get_gdb_group_mempool_by_chain_type",
    "net_list_json_collect",
    "net_states_json_collect",
    "nets_info_json_collect",
    "cli_list_net",
    "ledger_by_net_name",
    "net_link_add",
    "net_sync",
    "net_links_establish",
    "net_purge",
    "net_announce_addr",
    "net_announce_addr_all",
    "net_tx_get_fee",
    "net_get_my_node_info",
    "net_get_authorized_nodes",
    "net_is_my_node_authorized",
    "net_verify_datum_for_add",
    "net_verify_datum_err_code_to_str",
    "net_datum_list",
    "net_datum_add",
    "net_datum_remove",
    "net_proc_mempool",
    "net_get_mempool_cluster",
    "net_add_auth_nodes_to_cluster",
    "net_add_validator_to_clusters",
    "net_remove_validator_from_clusters",
    "net_add_reward",
    "net_remove_last_reward",
    "net_get_reward",
    "chain_add_mempool_notify_callback",
    "net_add_nodelist_notify_callback",
    "net_srv_order_add_notify_callback",
    "net_delete",
    "net_tx_set_fee",
]

NOARG_METHODS = {
    "net_core_init",
    "net_init",
    "net_deinit",
    "net_load_all",
    "net_try_online_all",
    "net_count",
    "net_iter_start",
    "net_iterate",
    "net_set_load_skip",
    "net_announce_addr_all",
    "cli_list_net",
}

REQUIRES_ARGS = [
    name for name in NET_METHODS if name not in NOARG_METHODS
]


@pytest.mark.parametrize("name", REQUIRES_ARGS)
def test_methods_require_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


@pytest.mark.parametrize("name", sorted(NOARG_METHODS))
def test_noarg_methods_reject_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func(1)


def test_net_core_init_returns_int(sdk_initialized):
    assert isinstance(cf.net_core_init(), int)


def test_net_init_deinit_roundtrip(sdk_initialized):
    assert isinstance(cf.net_init(), int)
    assert cf.net_deinit() is None


def test_net_id_parse_valid_invalid():
    assert cf.net_id_parse("1") == 1
    assert cf.net_id_parse("0x1") == 1
    with pytest.raises(ValueError):
        cf.net_id_parse("not-an-id")


def test_net_state_to_str_and_state_name():
    assert isinstance(cf.net_state_to_str(0), str)
    assert cf.net_state_to_str(NET_STATE_INVALID) == "NET_STATE_INVALID"
    assert cf.net_get_state_name(NET_STATE_INVALID) == "UNKNOWN"


def test_net_by_name_invalid():
    with pytest.raises(ValueError):
        cf.net_by_name("")
    with pytest.raises(ValueError):
        cf.net_by_name(f"missing_{uuid.uuid4().hex}")


def test_net_by_id_invalid():
    with pytest.raises(ValueError):
        cf.net_by_id(0)


def test_net_id_by_name_invalid():
    with pytest.raises(ValueError):
        cf.net_id_by_name("")
    with pytest.raises(ValueError):
        cf.net_id_by_name(f"missing_{uuid.uuid4().hex}")


def test_net_count_returns_int(sdk_initialized):
    assert isinstance(cf.net_count(), int)


def test_net_iter_start_and_iterate(sdk_initialized):
    first = cf.net_iter_start()
    assert_capsule_or_none(first)
    iter_first = cf.net_iterate()
    assert_capsule_or_none(iter_first)
    if first is None or iter_first is None:
        assert first is None
        assert iter_first is None
        assert cf.net_count() == 0
        return

    assert cf.net_get_id(iter_first) == cf.net_get_id(first)
    next_net = cf.net_iter_next(first)
    assert_capsule_or_none(next_net)


def test_net_parse_net_chain_invalid_args():
    with pytest.raises(TypeError):
        cf.net_parse_net_chain("net")
    with pytest.raises(TypeError):
        cf.net_parse_net_chain([object()])
    with pytest.raises(TypeError):
        cf.net_parse_net_chain(["net"], object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_json")
        cf.net_parse_net_chain(["net"], bad_capsule)


def test_net_parse_net_chain_missing_net(sdk_initialized):
    result = cf.net_parse_net_chain(["net"])
    assert isinstance(result, tuple)
    assert len(result) == 5
    rc, arg_index, chain, net, json_str = result
    assert isinstance(rc, int)
    assert isinstance(arg_index, int)
    assert_capsule_or_none(chain)
    assert_capsule_or_none(net)
    assert json_str is None or isinstance(json_str, str)
    assert rc < 0


def test_chain_get_name_and_id(chain_context):
    chain, _, chain_name = chain_context
    assert cf.chain_get_name(chain) == chain_name
    assert isinstance(cf.chain_get_id(chain), int)


def test_chain_get_name_invalid_capsule():
    with pytest.raises(TypeError):
        cf.chain_get_name(object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_chain")
        cf.chain_get_name(bad_capsule)


def test_chain_get_id_invalid_capsule():
    with pytest.raises(TypeError):
        cf.chain_get_id(object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_chain")
        cf.chain_get_id(bad_capsule)


NET_CAPSULE_METHODS = [
    ("net_register", ()),
    ("net_unregister", ()),
    ("net_get_name", ()),
    ("net_get_id", ()),
    ("net_get_state", ()),
    ("net_get_target_state", ()),
    ("net_get_native_ticker", ()),
    ("net_state_to_str_user", ()),
    ("net_state_go_to", (0,)),
    ("net_start", ()),
    ("net_stop", ()),
    ("net_get_role", ()),
    ("net_get_acl_idx", ()),
    ("net_get_cur_addr_int", ()),
    ("net_get_cur_cell", ()),
    ("net_get_flag_sync_from_zero", ()),
    ("net_set_flag_sync_from_zero", (True,)),
    ("net_get_load_mode", ()),
    ("net_is_bridged", (1,)),
    ("net_get_chain_by_name", ("chain",)),
    ("net_get_chain_by_id", (1,)),
    ("net_get_chain_by_type", (CHAIN_TYPE_INVALID,)),
    ("net_get_default_chain_by_type", (CHAIN_TYPE_INVALID,)),
    ("net_get_gdb_group_acl", ()),
    ("net_get_gdb_group_mempool_by_chain_type", (CHAIN_TYPE_INVALID,)),
    ("net_states_json_collect", (1,)),
    ("net_iter_next", ()),
    ("net_link_add", (1, "127.0.0.1", 1)),
    ("net_sync", ()),
    ("net_links_establish", ()),
    ("net_purge", ()),
    ("net_announce_addr", ()),
    ("net_tx_get_fee", ()),
    ("net_get_my_node_info", ()),
    ("net_get_authorized_nodes", ()),
    ("net_is_my_node_authorized", ()),
    ("net_datum_list", ()),
    ("net_proc_mempool", ()),
    ("net_add_auth_nodes_to_cluster", (make_capsule("dap_global_db_cluster_t"),)),
    ("net_add_reward", (UINT256_BYTES, 1)),
    ("net_remove_last_reward", ()),
    ("net_get_reward", (1,)),
    ("net_delete", ()),
    ("net_tx_set_fee", (UINT256_BYTES, make_capsule("dap_chain_addr_t"))),
]


@pytest.mark.parametrize("name, extra_args", NET_CAPSULE_METHODS)
def test_net_methods_reject_invalid_capsule(name, extra_args):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func(object(), *extra_args)
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_net")
        func(bad_capsule, *extra_args)


def test_net_state_go_to_rejects_invalid_state():
    bad_capsule = make_capsule("wrong_net")
    with pytest.raises(ValueError):
        cf.net_state_go_to(bad_capsule, NET_STATE_INVALID)


def test_net_list_json_collect_and_nets_info(sdk_initialized):
    assert_capsule_or_none(cf.net_list_json_collect(1))
    assert_capsule_or_none(cf.nets_info_json_collect(1))


def test_net_list_json_collect_invalid():
    with pytest.raises(TypeError):
        cf.net_list_json_collect("bad")


def test_nets_info_json_collect_invalid():
    with pytest.raises(TypeError):
        cf.nets_info_json_collect("bad")


def test_cli_list_net_returns_string(sdk_initialized):
    result = cf.cli_list_net()
    assert result is None or isinstance(result, str)


def test_ledger_by_net_name_invalid():
    with pytest.raises(TypeError):
        cf.ledger_by_net_name(123)
    result = cf.ledger_by_net_name(f"missing_{uuid.uuid4().hex}")
    assert result is None or is_capsule(result)


def test_net_get_gdb_group_nochain_new_invalid_capsule():
    with pytest.raises(TypeError):
        cf.net_get_gdb_group_nochain_new(object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_chain")
        cf.net_get_gdb_group_nochain_new(bad_capsule)


def test_net_get_gdb_group_nochain_new_smoke(chain_context):
    chain, _, _ = chain_context
    result = cf.net_get_gdb_group_nochain_new(chain)
    assert result is None or isinstance(result, str)


def test_net_verify_datum_for_add_invalid_capsule():
    with pytest.raises(TypeError):
        cf.net_verify_datum_for_add(object(), DATUM_SMALL, HASH_BYTES)
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_chain")
        cf.net_verify_datum_for_add(bad_capsule, DATUM_SMALL, HASH_BYTES)


def test_net_verify_datum_for_add_invalid_hash(chain_context):
    chain, _, _ = chain_context
    with pytest.raises(ValueError):
        cf.net_verify_datum_for_add(chain, DATUM_SMALL, BAD_HASH_BYTES)


def test_net_verify_datum_for_add_small_datum(chain_context):
    chain, _, _ = chain_context
    with pytest.raises(ValueError):
        cf.net_verify_datum_for_add(chain, DATUM_SMALL, HASH_BYTES)


def test_chain_add_mempool_notify_callback_rejects_invalid():
    with pytest.raises(TypeError):
        cf.chain_add_mempool_notify_callback(object(), lambda *args: None)
    with pytest.raises(TypeError):
        bad_capsule = make_capsule("wrong_chain")
        cf.chain_add_mempool_notify_callback(bad_capsule, object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_chain")
        cf.chain_add_mempool_notify_callback(bad_capsule, lambda *args: None)


def test_net_add_nodelist_notify_callback_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_add_nodelist_notify_callback(object(), lambda *args: None)
    with pytest.raises(TypeError):
        bad_capsule = make_capsule("wrong_net")
        cf.net_add_nodelist_notify_callback(bad_capsule, object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_net")
        cf.net_add_nodelist_notify_callback(bad_capsule, lambda *args: None)


def test_net_srv_order_add_notify_callback_rejects_invalid():
    with pytest.raises(TypeError):
        cf.net_srv_order_add_notify_callback(object(), lambda *args: None)
    with pytest.raises(TypeError):
        bad_capsule = make_capsule("wrong_net")
        cf.net_srv_order_add_notify_callback(bad_capsule, object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_net")
        cf.net_srv_order_add_notify_callback(bad_capsule, lambda *args: None)


def test_net_verify_datum_err_code_to_str():
    assert isinstance(cf.net_verify_datum_err_code_to_str(DATUM_BYTES, 0), str)
    with pytest.raises(TypeError):
        cf.net_verify_datum_err_code_to_str("bad")


def test_net_datum_list_rejects_invalid_chain_and_filter():
    net_capsule = make_capsule("dap_chain_net_t")
    with pytest.raises(TypeError):
        cf.net_datum_list(net_capsule, object())
    with pytest.raises(ValueError):
        bad_chain = make_capsule("wrong_chain")
        cf.net_datum_list(net_capsule, bad_chain)
    with pytest.raises(TypeError):
        cf.net_datum_list(net_capsule, None, object())


def test_net_datum_add_rejects_invalid_sizes():
    chain_capsule = make_capsule("dap_chain_t")
    with pytest.raises(ValueError):
        cf.net_datum_add(chain_capsule, DATUM_SMALL, BAD_HASH_BYTES)
    with pytest.raises(ValueError):
        cf.net_datum_add(chain_capsule, DATUM_SMALL, HASH_BYTES)


def test_net_datum_remove_rejects_invalid_sizes():
    chain_capsule = make_capsule("dap_chain_t")
    with pytest.raises(ValueError):
        cf.net_datum_remove(chain_capsule, DATUM_SMALL, BAD_HASH_BYTES)
    with pytest.raises(ValueError):
        cf.net_datum_remove(chain_capsule, DATUM_SMALL, HASH_BYTES)


def test_net_get_mempool_cluster_rejects_invalid_chain():
    with pytest.raises(TypeError):
        cf.net_get_mempool_cluster(object())
    with pytest.raises(ValueError):
        bad_chain = make_capsule("wrong_chain")
        cf.net_get_mempool_cluster(bad_chain)


def test_net_add_auth_nodes_to_cluster_rejects_invalid_cluster():
    net_capsule = make_capsule("dap_chain_net_t")
    with pytest.raises(TypeError):
        cf.net_add_auth_nodes_to_cluster(net_capsule, object())


def test_net_add_validator_to_clusters_rejects_invalid_chain():
    with pytest.raises(TypeError):
        cf.net_add_validator_to_clusters(object(), 1)
    with pytest.raises(ValueError):
        bad_chain = make_capsule("wrong_chain")
        cf.net_add_validator_to_clusters(bad_chain, 1)


def test_net_remove_validator_from_clusters_rejects_invalid_chain():
    with pytest.raises(TypeError):
        cf.net_remove_validator_from_clusters(object(), 1)
    with pytest.raises(ValueError):
        bad_chain = make_capsule("wrong_chain")
        cf.net_remove_validator_from_clusters(bad_chain, 1)


def test_net_tx_set_fee_rejects_invalid_args():
    net_capsule = make_capsule("dap_chain_net_t")
    addr_capsule = make_capsule("dap_chain_addr_t")
    with pytest.raises(TypeError):
        cf.net_tx_set_fee(net_capsule, UINT256_BYTES, object())
    with pytest.raises(TypeError):
        cf.net_tx_set_fee(object(), UINT256_BYTES, addr_capsule)
    with pytest.raises(TypeError):
        cf.net_tx_set_fee(net_capsule, object(), addr_capsule)


def test_net_get_reward_rejects_invalid_block_num():
    with pytest.raises(TypeError):
        cf.net_get_reward(object(), "bad")
