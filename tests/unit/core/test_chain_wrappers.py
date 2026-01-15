import ctypes
import uuid
import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_SIZE = 32
CELL_ID_BYTES_SIZE = 8
VALID_HASH = b"\x00" * HASH_SIZE
BAD_HASH = b"\x00" * (HASH_SIZE - 1)
VALID_ATOM = b"atom"
VALID_DATUM = b"datum"
VALID_CELL_ID_BYTES = b"\x01" + b"\x00" * (CELL_ID_BYTES_SIZE - 1)

_PyCapsule_New = ctypes.pythonapi.PyCapsule_New
_PyCapsule_New.restype = ctypes.py_object
_PyCapsule_New.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]
_capsule_keepalive = []


def make_capsule(name):
    buf = ctypes.create_string_buffer(1)
    capsule = _PyCapsule_New(ctypes.addressof(buf), name.encode("ascii"), None)
    _capsule_keepalive.append(buf)
    return capsule


def is_capsule(obj):
    return type(obj).__name__ == "PyCapsule"


def assert_capsule_or_none(value):
    if value is None:
        return
    assert is_capsule(value)


@pytest.fixture(scope="session")
def sdk_initialized(tmp_path_factory):
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

    yield chain, net_id, chain_id, _delete

    if not deleted["value"]:
        _delete()


ALL_CHAIN_METHODS = [
    "dap_cert_chain_file_save",
    "dap_chain_add_callback_datum_index_notify",
    "dap_chain_add_callback_datum_removed_from_index_notify",
    "dap_chain_add_callback_notify",
    "dap_chain_add_callback_timer",
    "dap_chain_atom_add_from_threshold",
    "dap_chain_atom_confirmed_notify_add",
    "dap_chain_atom_notify",
    "dap_chain_atom_remove_notify",
    "dap_chain_atom_save",
    "dap_chain_create",
    "dap_chain_datum_notify",
    "dap_chain_datum_removed_notify",
    "dap_chain_datum_type_supported_by_chain",
    "dap_chain_deinit",
    "dap_chain_delete",
    "dap_chain_find_by_id",
    "dap_chain_generation_ban",
    "dap_chain_generation_banned",
    "dap_chain_generation_next",
    "dap_chain_get_atom_by_hash",
    "dap_chain_get_atom_last_hash",
    "dap_chain_get_atom_last_hash_num_ts",
    "dap_chain_get_blockhain_time",
    "dap_chain_get_cs_type",
    "dap_chain_get_path",
    "dap_chain_has_file_store",
    "dap_chain_id_parse",
    "dap_chain_info_dump_log",
    "dap_chain_init",
    "dap_chain_load_all",
    "dap_chain_load_from_cfg",
    "dap_chain_purge",
    "dap_chain_set_cs_name",
    "dap_chain_set_cs_type",
    "dap_chain_cell_capture_by_id",
    "dap_chain_cell_close",
    "dap_chain_cell_close_all",
    "dap_chain_cell_create",
    "dap_chain_cell_file_append",
    "dap_chain_cell_find_by_id",
    "dap_chain_cell_init",
    "dap_chain_cell_open",
    "dap_chain_cell_remit",
    "dap_chain_cell_remove",
    "dap_chain_cell_set_load_skip",
    "dap_chain_cell_truncate",
    "dap_chain_ch_deinit",
    "dap_chain_ch_init",
    "dap_stream_ch_write_error_unsafe",
    "dap_chain_ch_pkt_get_size",
    "dap_chain_ch_pkt_new",
    "dap_chain_ch_pkt_type_to_str",
    "dap_chain_ch_pkt_write",
    "dap_chain_ch_pkt_write_unsafe",
    "dap_chain_net_get_default_chain_by_chain_type",
    "dap_chain_policy_activate_json_collect",
    "dap_chain_policy_apply",
    "dap_chain_policy_create_activate",
    "dap_chain_policy_create_deactivate",
    "dap_chain_policy_deinit",
    "dap_chain_policy_get_last_num",
    "dap_chain_policy_get_size",
    "dap_chain_policy_init",
    "dap_chain_policy_is_activated",
    "dap_chain_policy_is_exist",
    "dap_chain_policy_json_collect",
    "dap_chain_policy_list",
    "dap_chain_policy_net_add",
    "dap_chain_policy_net_purge",
    "dap_chain_policy_net_remove",
    "dap_chain_policy_to_str",
    "dap_chain_policy_update_last_num",
    "dap_chain_srv_add",
    "dap_chain_srv_count",
    "dap_chain_srv_decree",
    "dap_chain_srv_deinit",
    "dap_chain_srv_delete",
    "dap_chain_srv_event_verify",
    "dap_chain_srv_fee_type_to_str",
    "dap_chain_srv_get_fees",
    "dap_chain_srv_get_internal",
    "dap_chain_srv_get_uid_by_name",
    "dap_chain_srv_hardfork_all",
    "dap_chain_srv_hardfork_complete_all",
    "dap_chain_srv_init",
    "dap_chain_srv_list",
    "dap_chain_srv_load_state",
    "dap_chain_srv_purge",
    "dap_chain_srv_purge_all",
    "dap_chain_srv_start",
    "dap_chain_srv_start_all",
]

NOARG_METHODS = {
    "dap_chain_init",
    "dap_chain_deinit",
    "dap_chain_cell_init",
    "dap_chain_cell_set_load_skip",
    "dap_chain_ch_init",
    "dap_chain_ch_deinit",
    "dap_chain_policy_init",
    "dap_chain_policy_deinit",
    "dap_chain_srv_init",
    "dap_chain_srv_deinit",
}

PURE_METHODS = {
    "dap_chain_id_parse",
    "dap_chain_policy_get_size",
    "dap_chain_policy_to_str",
    "dap_chain_ch_pkt_type_to_str",
    "dap_chain_srv_fee_type_to_str",
    "dap_chain_policy_create_activate",
    "dap_chain_policy_create_deactivate",
}

REQUIRES_ARGS = [
    name for name in ALL_CHAIN_METHODS if name not in NOARG_METHODS
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


@pytest.mark.parametrize("name", sorted(PURE_METHODS))
def test_pure_methods_exist(name):
    assert hasattr(cf, name)


def test_chain_init_deinit_roundtrip(sdk_initialized):
    assert isinstance(cf.dap_chain_init(), int)
    assert cf.dap_chain_deinit() is None
    assert isinstance(cf.dap_chain_init(), int)


def test_chain_policy_init_deinit_roundtrip(sdk_initialized):
    assert isinstance(cf.dap_chain_policy_init(), int)
    assert cf.dap_chain_policy_deinit() is None
    assert isinstance(cf.dap_chain_policy_init(), int)


def test_chain_srv_init_deinit_roundtrip(sdk_initialized):
    assert isinstance(cf.dap_chain_srv_init(), int)
    assert cf.dap_chain_srv_deinit() is None
    assert isinstance(cf.dap_chain_srv_init(), int)


def test_chain_ch_init_deinit_roundtrip(sdk_initialized):
    assert isinstance(cf.dap_chain_ch_init(), int)
    assert cf.dap_chain_ch_deinit() is None
    assert isinstance(cf.dap_chain_ch_init(), int)


def test_chain_cell_init_and_load_skip(sdk_initialized):
    assert isinstance(cf.dap_chain_cell_init(), int)
    assert cf.dap_chain_cell_set_load_skip() is None


def test_chain_id_parse_valid():
    assert cf.dap_chain_id_parse("1") == 1
    assert cf.dap_chain_id_parse("0x1") == 1


def test_chain_id_parse_invalid():
    with pytest.raises(ValueError):
        cf.dap_chain_id_parse("not-an-id")


def test_chain_create_find_and_delete(chain_context):
    chain, net_id, chain_id, _ = chain_context
    assert is_capsule(chain)
    found = cf.dap_chain_find_by_id(net_id, chain_id)
    assert is_capsule(found)


def test_chain_delete_invalidates_capsule(chain_context):
    chain, _, _, delete_chain = chain_context
    delete_chain()
    with pytest.raises(ValueError):
        cf.dap_chain_get_cs_type(chain)


def test_chain_cs_type_roundtrip(chain_context):
    chain, _, _, _ = chain_context
    assert cf.dap_chain_set_cs_type(chain, "test") is None
    assert cf.dap_chain_get_cs_type(chain) == "test"


def test_chain_set_cs_name_smoke(chain_context):
    chain, _, _, _ = chain_context
    assert cf.dap_chain_set_cs_name(chain, "consensus") is None


def test_chain_generation_controls(chain_context):
    chain, _, _, _ = chain_context
    assert isinstance(cf.dap_chain_generation_next(chain), int)
    assert cf.dap_chain_generation_banned(chain, 1) in (True, False)
    result = cf.dap_chain_generation_ban(chain, 1)
    assert isinstance(result, int)
    assert cf.dap_chain_generation_banned(chain, 1) is True


def test_chain_basic_getters(chain_context):
    chain, _, _, _ = chain_context
    assert isinstance(cf.dap_chain_has_file_store(chain), bool)
    path = cf.dap_chain_get_path(chain)
    assert path is None or isinstance(path, str)
    assert isinstance(cf.dap_chain_get_blockhain_time(chain), int)
    assert cf.dap_chain_info_dump_log(chain) is None
    assert isinstance(cf.dap_chain_load_all(chain), int)
    assert isinstance(cf.dap_chain_purge(chain), int)
    assert isinstance(cf.dap_chain_datum_type_supported_by_chain(chain, 0), bool)


def test_chain_atom_lookup_returns_none(chain_context):
    chain, _, _, _ = chain_context
    assert cf.dap_chain_get_atom_by_hash(chain, VALID_HASH) is None
    assert cf.dap_chain_get_atom_last_hash(chain) is None
    assert cf.dap_chain_get_atom_last_hash_num_ts(chain) is None


def test_chain_atom_save_returns_hash_and_code(chain_context):
    chain, _, _, _ = chain_context
    result = cf.dap_chain_atom_save(chain, 0, VALID_ATOM)
    assert isinstance(result, dict)
    assert set(result.keys()) == {"code", "hash"}
    assert isinstance(result["code"], int)
    assert isinstance(result["hash"], (bytes, bytearray))
    assert len(result["hash"]) == HASH_SIZE


def test_chain_atom_add_from_threshold_smoke(chain_context):
    chain, _, _, _ = chain_context
    assert cf.dap_chain_atom_add_from_threshold(chain) is None


def test_chain_atom_notify_callback(chain_context):
    chain, _, _, _ = chain_context
    called = {}

    def cb(chain_capsule, cell_id, atom_hash, atom, atom_size, atom_time, user_data):
        called["args"] = (
            chain_capsule,
            cell_id,
            atom_hash,
            atom,
            atom_size,
            atom_time,
            user_data,
        )

    cf.dap_chain_add_callback_notify(chain, cb, "ctx")
    cf.dap_chain_atom_notify(chain, 1, VALID_HASH, VALID_ATOM, 123)

    assert "args" in called
    args = called["args"]
    assert is_capsule(args[0])
    assert isinstance(args[1], int)
    assert isinstance(args[2], (bytes, bytearray))
    assert isinstance(args[3], (bytes, bytearray))
    assert isinstance(args[4], int)
    assert isinstance(args[5], int)
    assert args[6] == "ctx"


def test_chain_datum_notify_callback(chain_context):
    chain, _, _, _ = chain_context
    called = {}

    def cb(datum_hash, atom_hash, datum, datum_size, ret_code, action, uid, user_data):
        called["args"] = (
            datum_hash,
            atom_hash,
            datum,
            datum_size,
            ret_code,
            action,
            uid,
            user_data,
        )

    cf.dap_chain_add_callback_datum_index_notify(chain, cb, "ctx")
    cf.dap_chain_datum_notify(chain, 1, VALID_HASH, VALID_HASH, VALID_DATUM, 0, 0, 0)

    assert "args" in called
    args = called["args"]
    assert isinstance(args[0], (bytes, bytearray))
    assert isinstance(args[1], (bytes, bytearray))
    assert isinstance(args[2], (bytes, bytearray))
    assert isinstance(args[3], int)
    assert isinstance(args[4], int)
    assert isinstance(args[5], int)
    assert isinstance(args[6], int)
    assert args[7] == "ctx"


def test_chain_datum_removed_notify_callback(chain_context):
    chain, _, _, _ = chain_context
    called = {}

    def cb(datum_hash, datum_capsule, user_data):
        called["args"] = (datum_hash, datum_capsule, user_data)

    cf.dap_chain_add_callback_datum_removed_from_index_notify(chain, cb, "ctx")
    cf.dap_chain_datum_removed_notify(chain, 1, VALID_HASH, None)

    assert "args" in called
    args = called["args"]
    assert isinstance(args[0], (bytes, bytearray))
    assert args[2] == "ctx"


def test_chain_timer_callback_registration(chain_context):
    chain, _, _, _ = chain_context
    result = cf.dap_chain_add_callback_timer(chain, lambda *args: None)
    assert result == 0


def test_chain_atom_confirmed_notify_add_smoke(chain_context):
    chain, _, _, _ = chain_context
    assert cf.dap_chain_atom_confirmed_notify_add(chain, lambda *args: None, None, 1) is None


def test_chain_atom_remove_notify_smoke(chain_context):
    chain, _, _, _ = chain_context
    assert cf.dap_chain_atom_remove_notify(chain, 1) is None


def test_chain_datum_removed_notify_accepts_none(chain_context):
    chain, _, _, _ = chain_context
    assert cf.dap_chain_datum_removed_notify(chain, 1, VALID_HASH, None) is None


def test_chain_cell_find_capture_none(chain_context):
    chain, _, _, _ = chain_context
    assert cf.dap_chain_cell_find_by_id(chain, 9999) is None
    assert cf.dap_chain_cell_capture_by_id(chain, 9999) is None


def test_chain_cell_open_mode_validation(chain_context):
    chain, _, _, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_cell_open(chain, 1, "rw")


def test_chain_cell_truncate_negative_delta(chain_context):
    chain, _, _, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_cell_truncate(chain, 1, -1)


def test_chain_cell_smoke_operations(chain_context):
    chain, _, _, _ = chain_context
    assert isinstance(cf.dap_chain_cell_create(chain, 1), int)
    assert isinstance(cf.dap_chain_cell_open(chain, 1, "r"), int)
    assert cf.dap_chain_cell_close(chain, 1) is None
    assert cf.dap_chain_cell_close_all(chain) is None
    assert cf.dap_chain_cell_remit(chain) is None
    assert isinstance(cf.dap_chain_cell_file_append(chain, 1, b"a"), int)
    assert isinstance(cf.dap_chain_cell_remove(chain, 1, False), int)
    assert isinstance(cf.dap_chain_cell_truncate(chain, 1, 0), int)


def test_chain_ch_pkt_new_and_get_size():
    pkt = cf.dap_chain_ch_pkt_new(1, 2, 1, b"data", 1)
    assert is_capsule(pkt)
    size = cf.dap_chain_ch_pkt_get_size(pkt)
    assert isinstance(size, int)
    assert size > 0


def test_chain_ch_pkt_new_rejects_invalid_cell_id():
    with pytest.raises((TypeError, ValueError)):
        cf.dap_chain_ch_pkt_new(1, 2, "bad", b"data", 1)


def test_chain_ch_pkt_type_to_str_returns_string():
    assert isinstance(cf.dap_chain_ch_pkt_type_to_str(0x80), str)


def test_chain_ch_pkt_write_rejects_invalid_capsule():
    with pytest.raises(TypeError):
        cf.dap_chain_ch_pkt_write(object(), 1, 1, 1, 1, 1, b"data", 1)
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_worker")
        cf.dap_chain_ch_pkt_write(bad_capsule, 1, 1, 1, 1, 1, b"data", 1)


def test_chain_ch_pkt_write_unsafe_rejects_invalid_capsule():
    with pytest.raises(TypeError):
        cf.dap_chain_ch_pkt_write_unsafe(object(), 1, 1, 1, 1, 1, b"data", 1)
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_ch")
        cf.dap_chain_ch_pkt_write_unsafe(bad_capsule, 1, 1, 1, 1, 1, b"data", 1)


def test_stream_ch_write_error_unsafe_rejects_invalid_capsule():
    with pytest.raises(TypeError):
        cf.dap_stream_ch_write_error_unsafe(object(), 1, 1, 1, 1)
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_stream")
        cf.dap_stream_ch_write_error_unsafe(bad_capsule, 1, 1, 1, 1)


def test_chain_net_get_default_chain_invalid_capsule():
    with pytest.raises(TypeError):
        cf.dap_chain_net_get_default_chain_by_chain_type(object(), 0)
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_net")
        cf.dap_chain_net_get_default_chain_by_chain_type(bad_capsule, 0)


def test_chain_policy_create_activate_and_size():
    policy = cf.dap_chain_policy_create_activate(1, 0, 0, 1, 1)
    assert is_capsule(policy)
    size = cf.dap_chain_policy_get_size(policy)
    assert isinstance(size, int)
    assert size > 0
    assert isinstance(cf.dap_chain_policy_to_str(policy), str)


def test_chain_policy_create_deactivate_validation():
    with pytest.raises(TypeError):
        cf.dap_chain_policy_create_deactivate(1)
    with pytest.raises(ValueError):
        cf.dap_chain_policy_create_deactivate([])
    policy = cf.dap_chain_policy_create_deactivate(["1", "2"])
    assert is_capsule(policy)


def test_chain_policy_to_str_none():
    assert cf.dap_chain_policy_to_str(None) == "<null>"


def test_chain_policy_get_size_none():
    assert cf.dap_chain_policy_get_size(None) == 0


def test_chain_policy_apply_and_json_collect(chain_context):
    _, net_id, _, _ = chain_context
    policy = cf.dap_chain_policy_create_activate(1, 0, 0, 1, 1)
    assert isinstance(cf.dap_chain_policy_apply(policy, net_id), int)
    assert_capsule_or_none(cf.dap_chain_policy_json_collect(policy))


def test_chain_policy_net_add_and_list(chain_context):
    _, net_id, _, _ = chain_context
    assert isinstance(cf.dap_chain_policy_net_add(net_id), int)
    assert_capsule_or_none(cf.dap_chain_policy_list(net_id, 0))
    assert_capsule_or_none(cf.dap_chain_policy_activate_json_collect(net_id, 1))
    assert isinstance(cf.dap_chain_policy_is_exist(net_id, 1), bool)
    assert isinstance(cf.dap_chain_policy_is_activated(net_id, 1), bool)
    assert cf.dap_chain_policy_update_last_num(net_id, 5) is None
    assert isinstance(cf.dap_chain_policy_get_last_num(net_id), int)
    assert cf.dap_chain_policy_net_purge(net_id) is None
    assert cf.dap_chain_policy_net_remove(net_id) is None


def test_chain_srv_misc_returns_types(chain_context):
    _, net_id, _, _ = chain_context
    assert isinstance(cf.dap_chain_srv_add(1, "service"), int)
    assert isinstance(cf.dap_chain_srv_get_uid_by_name("service"), int)
    assert isinstance(cf.dap_chain_srv_count(net_id), int)
    assert_capsule_or_none(cf.dap_chain_srv_list(net_id))
    assert_capsule_or_none(cf.dap_chain_srv_get_internal(net_id, 1))
    assert_capsule_or_none(cf.dap_chain_srv_get_fees(net_id))
    assert_capsule_or_none(cf.dap_chain_srv_hardfork_all(net_id))
    assert isinstance(cf.dap_chain_srv_start(net_id, "service"), int)
    assert isinstance(cf.dap_chain_srv_start_all(net_id), int)
    assert isinstance(cf.dap_chain_srv_delete(1), int)
    assert isinstance(cf.dap_chain_srv_purge(net_id, 1), int)
    assert isinstance(cf.dap_chain_srv_purge_all(net_id), int)
    assert cf.dap_chain_srv_hardfork_complete_all(net_id) is None
    assert isinstance(cf.dap_chain_srv_fee_type_to_str(0x9999), str)


def test_chain_srv_load_state_returns_int(chain_context):
    _, net_id, _, _ = chain_context
    assert isinstance(cf.dap_chain_srv_load_state(net_id, 1, b"", 0), int)


def test_chain_srv_event_verify_hash_validation(chain_context):
    _, net_id, _, _ = chain_context
    assert isinstance(
        cf.dap_chain_srv_event_verify(net_id, 1, "group", 0, b"data", None),
        int,
    )
    with pytest.raises(TypeError):
        cf.dap_chain_srv_event_verify(net_id, 1, "group", 0, b"data", "bad")
    with pytest.raises(ValueError):
        cf.dap_chain_srv_event_verify(net_id, 1, "group", 0, b"data", BAD_HASH)


def test_chain_srv_decree_validation(chain_context):
    _, net_id, _, _ = chain_context
    assert isinstance(cf.dap_chain_srv_decree(net_id, 1, True, None, 0), int)
    with pytest.raises(ValueError):
        cf.dap_chain_srv_decree(net_id, 1, True, None, 1)
    with pytest.raises(ValueError):
        cf.dap_chain_srv_decree(net_id, 1, True, None, -1)
    with pytest.raises(TypeError):
        cf.dap_chain_srv_decree(net_id, 1, True, "bad", 0)


def test_chain_srv_add_validation():
    with pytest.raises(TypeError):
        cf.dap_chain_srv_add(1, 123)
    with pytest.raises(TypeError):
        cf.dap_chain_srv_add(1, "service", "bad")
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_callbacks")
        cf.dap_chain_srv_add(1, "service", bad_capsule)


def test_chain_srv_start_validation():
    with pytest.raises(TypeError):
        cf.dap_chain_srv_start(1, "service", "bad")
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_config")
        cf.dap_chain_srv_start(1, "service", bad_capsule)


def test_chain_policy_net_add_validation():
    with pytest.raises(TypeError):
        cf.dap_chain_policy_net_add(1, "bad")
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_config")
        cf.dap_chain_policy_net_add(1, bad_capsule)


def test_chain_load_from_cfg_validation():
    with pytest.raises(TypeError):
        cf.dap_chain_load_from_cfg("net", 1, "bad")
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_config")
        cf.dap_chain_load_from_cfg("net", 1, bad_capsule)


def test_cert_chain_file_save_validation():
    with pytest.raises(TypeError):
        cf.dap_cert_chain_file_save(object(), "net")
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_datum")
        cf.dap_cert_chain_file_save(bad_capsule, "net")


INVALID_CHAIN = object()

CHAIN_CAPSULE_TYPE_ERROR_CASES = [
    ("dap_chain_get_atom_by_hash", (INVALID_CHAIN, VALID_HASH)),
    ("dap_chain_get_atom_last_hash", (INVALID_CHAIN,)),
    ("dap_chain_get_atom_last_hash_num_ts", (INVALID_CHAIN,)),
    ("dap_chain_get_blockhain_time", (INVALID_CHAIN,)),
    ("dap_chain_get_cs_type", (INVALID_CHAIN,)),
    ("dap_chain_get_path", (INVALID_CHAIN,)),
    ("dap_chain_has_file_store", (INVALID_CHAIN,)),
    ("dap_chain_info_dump_log", (INVALID_CHAIN,)),
    ("dap_chain_load_all", (INVALID_CHAIN,)),
    ("dap_chain_purge", (INVALID_CHAIN,)),
    ("dap_chain_atom_save", (INVALID_CHAIN, 1, VALID_ATOM)),
    ("dap_chain_add_callback_notify", (INVALID_CHAIN, lambda *_: None)),
    ("dap_chain_add_callback_datum_index_notify", (INVALID_CHAIN, lambda *_: None)),
    ("dap_chain_add_callback_datum_removed_from_index_notify", (INVALID_CHAIN, lambda *_: None)),
    ("dap_chain_atom_confirmed_notify_add", (INVALID_CHAIN, lambda *_: None)),
    ("dap_chain_add_callback_timer", (INVALID_CHAIN, lambda *_: None)),
    ("dap_chain_atom_add_from_threshold", (INVALID_CHAIN,)),
    (
        "dap_chain_datum_notify",
        (INVALID_CHAIN, 1, VALID_HASH, VALID_HASH, VALID_DATUM, 0, 0, 0),
    ),
    ("dap_chain_datum_removed_notify", (INVALID_CHAIN, 1, VALID_HASH, None)),
    ("dap_chain_datum_type_supported_by_chain", (INVALID_CHAIN, 0)),
    ("dap_chain_delete", (INVALID_CHAIN,)),
    ("dap_chain_generation_ban", (INVALID_CHAIN, 1)),
    ("dap_chain_generation_banned", (INVALID_CHAIN, 1)),
    ("dap_chain_generation_next", (INVALID_CHAIN,)),
    ("dap_chain_set_cs_type", (INVALID_CHAIN, "pow")),
    ("dap_chain_set_cs_name", (INVALID_CHAIN, "consensus")),
    ("dap_chain_atom_notify", (INVALID_CHAIN, 1, VALID_HASH, VALID_ATOM)),
    ("dap_chain_atom_remove_notify", (INVALID_CHAIN, 1)),
    ("dap_chain_cell_open", (INVALID_CHAIN, 1, "r")),
    ("dap_chain_cell_create", (INVALID_CHAIN, 1)),
    ("dap_chain_cell_find_by_id", (INVALID_CHAIN, 1)),
    ("dap_chain_cell_capture_by_id", (INVALID_CHAIN, 1)),
    ("dap_chain_cell_remit", (INVALID_CHAIN,)),
    ("dap_chain_cell_close", (INVALID_CHAIN, 1)),
    ("dap_chain_cell_close_all", (INVALID_CHAIN,)),
    ("dap_chain_cell_file_append", (INVALID_CHAIN, 1, b"a")),
    ("dap_chain_cell_remove", (INVALID_CHAIN, 1, False)),
    ("dap_chain_cell_truncate", (INVALID_CHAIN, 1, 0)),
]


@pytest.mark.parametrize("name,args", CHAIN_CAPSULE_TYPE_ERROR_CASES)
def test_chain_methods_reject_non_capsule(name, args):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func(*args)


CALLBACK_METHODS = [
    "dap_chain_add_callback_notify",
    "dap_chain_add_callback_datum_index_notify",
    "dap_chain_add_callback_datum_removed_from_index_notify",
    "dap_chain_atom_confirmed_notify_add",
    "dap_chain_add_callback_timer",
]


@pytest.mark.parametrize("name", CALLBACK_METHODS)
def test_chain_callback_requires_callable(chain_context, name):
    chain, _, _, _ = chain_context
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func(chain, 123)


INVALID_CELL_IDS = [
    object(),
    "bad",
    {"raw": b"x"},
    {"raw": "bad"},
    {"uint64": "bad"},
    {"bad": 1},
    b"\x00",
]

CELL_ID_CALLS = [
    ("dap_chain_atom_save", lambda chain, cid: cf.dap_chain_atom_save(chain, cid, VALID_ATOM)),
    ("dap_chain_atom_notify", lambda chain, cid: cf.dap_chain_atom_notify(chain, cid, VALID_HASH, VALID_ATOM)),
    ("dap_chain_atom_remove_notify", lambda chain, cid: cf.dap_chain_atom_remove_notify(chain, cid)),
    (
        "dap_chain_datum_notify",
        lambda chain, cid: cf.dap_chain_datum_notify(chain, cid, VALID_HASH, VALID_HASH, VALID_DATUM, 0, 0, 0),
    ),
    (
        "dap_chain_datum_removed_notify",
        lambda chain, cid: cf.dap_chain_datum_removed_notify(chain, cid, VALID_HASH, None),
    ),
    ("dap_chain_get_atom_last_hash", lambda chain, cid: cf.dap_chain_get_atom_last_hash(chain, cid)),
    (
        "dap_chain_get_atom_last_hash_num_ts",
        lambda chain, cid: cf.dap_chain_get_atom_last_hash_num_ts(chain, cid),
    ),
    ("dap_chain_get_blockhain_time", lambda chain, cid: cf.dap_chain_get_blockhain_time(chain, cid)),
    ("dap_chain_cell_open", lambda chain, cid: cf.dap_chain_cell_open(chain, cid, "r")),
    ("dap_chain_cell_create", lambda chain, cid: cf.dap_chain_cell_create(chain, cid)),
    ("dap_chain_cell_find_by_id", lambda chain, cid: cf.dap_chain_cell_find_by_id(chain, cid)),
    ("dap_chain_cell_capture_by_id", lambda chain, cid: cf.dap_chain_cell_capture_by_id(chain, cid)),
    ("dap_chain_cell_close", lambda chain, cid: cf.dap_chain_cell_close(chain, cid)),
    ("dap_chain_cell_file_append", lambda chain, cid: cf.dap_chain_cell_file_append(chain, cid, b"a")),
    ("dap_chain_cell_remove", lambda chain, cid: cf.dap_chain_cell_remove(chain, cid, False)),
    ("dap_chain_cell_truncate", lambda chain, cid: cf.dap_chain_cell_truncate(chain, cid, 0)),
]


@pytest.mark.parametrize("cell_id", INVALID_CELL_IDS)
@pytest.mark.parametrize("name,call", CELL_ID_CALLS)
def test_chain_methods_reject_invalid_cell_id(chain_context, cell_id, name, call):
    chain, _, _, _ = chain_context
    with pytest.raises((TypeError, ValueError)):
        call(chain, cell_id)


def test_chain_get_atom_by_hash_rejects_bad_hash(chain_context):
    chain, _, _, _ = chain_context
    with pytest.raises(ValueError):
        cf.dap_chain_get_atom_by_hash(chain, BAD_HASH)


def test_chain_atom_notify_hash_and_atom_validation(chain_context):
    chain, _, _, _ = chain_context
    with pytest.raises(TypeError):
        cf.dap_chain_atom_notify(chain, 1, None, VALID_ATOM)
    with pytest.raises(ValueError):
        cf.dap_chain_atom_notify(chain, 1, BAD_HASH, VALID_ATOM)
    with pytest.raises(TypeError):
        cf.dap_chain_atom_notify(chain, 1, VALID_HASH, None)
    with pytest.raises(TypeError):
        cf.dap_chain_atom_notify(chain, 1, VALID_HASH, "atom")


def test_chain_datum_notify_hash_and_datum_validation(chain_context):
    chain, _, _, _ = chain_context
    with pytest.raises(TypeError):
        cf.dap_chain_datum_notify(chain, 1, None, VALID_HASH, VALID_DATUM, 0, 0, 0)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_notify(chain, 1, VALID_HASH, None, VALID_DATUM, 0, 0, 0)
    with pytest.raises(ValueError):
        cf.dap_chain_datum_notify(chain, 1, BAD_HASH, VALID_HASH, VALID_DATUM, 0, 0, 0)
    with pytest.raises(ValueError):
        cf.dap_chain_datum_notify(chain, 1, VALID_HASH, BAD_HASH, VALID_DATUM, 0, 0, 0)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_notify(chain, 1, VALID_HASH, VALID_HASH, "datum", 0, 0, 0)


def test_chain_datum_removed_notify_hash_validation(chain_context):
    chain, _, _, _ = chain_context
    with pytest.raises(TypeError):
        cf.dap_chain_datum_removed_notify(chain, 1, None, None)
    with pytest.raises(ValueError):
        cf.dap_chain_datum_removed_notify(chain, 1, BAD_HASH, None)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_removed_notify(chain, 1, VALID_HASH, "bad")


def test_chain_cell_file_append_rejects_non_bytes(chain_context):
    chain, _, _, _ = chain_context
    with pytest.raises(TypeError):
        cf.dap_chain_cell_file_append(chain, 1, "bad")


def test_chain_srv_fee_type_to_str_returns_string():
    assert isinstance(cf.dap_chain_srv_fee_type_to_str(0x9999), str)
