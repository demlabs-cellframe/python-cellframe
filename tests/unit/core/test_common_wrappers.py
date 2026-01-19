import ctypes
import json
import uuid

import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_FAST_SIZE = 32
HASH_SLOW_SIZE = 32
HASH_FAST_BYTES = b"\x00" * HASH_FAST_SIZE
HASH_FAST_BYTES_ALT = b"\x01" * HASH_FAST_SIZE
HASH_SLOW_ZERO = b"\x00" * HASH_SLOW_SIZE
HASH_SLOW_ONE = b"\x01" * HASH_SLOW_SIZE
HASH_SLOW_MIXED = b"\x01" + b"\x02" + b"\x01" * (HASH_SLOW_SIZE - 2)

CHAIN_TYPE_INVALID = -1
CHAIN_TYPE_TOKEN = 1
CHAIN_TYPE_EMISSION = 2
CHAIN_TYPE_TX = 3
CHAIN_TYPE_CA = 4
CHAIN_TYPE_SIGNER = 5
CHAIN_TYPE_DECREE = 7
CHAIN_TYPE_ANCHOR = 8

DAP_CHAIN_DATUM_TX = 0x0100
DAP_CHAIN_DATUM_DECREE = 0x0200
DAP_CHAIN_DATUM_ANCHOR = 0x0A00
DAP_CHAIN_DATUM_CA = 0x0C00
DAP_CHAIN_DATUM_SIGNER = 0x0C01
DAP_CHAIN_DATUM_TOKEN = 0xF000
DAP_CHAIN_DATUM_TOKEN_EMISSION = 0xF100
DAP_CHAIN_DATUM_CUSTOM = 0xFFFF

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


def make_capsule_from_ptr(ptr, name):
    name_buf = ctypes.create_string_buffer(name.encode("ascii"))
    capsule = _PyCapsule_New(ctypes.c_void_p(ptr), name_buf, None)
    _capsule_keepalive.append(name_buf)
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
    return True


@pytest.fixture
def addr_null(sdk_initialized):
    return cf.dap_chain_addr_from_str("null")


@pytest.fixture
def addr_filled(sdk_initialized):
    return cf.dap_chain_addr_fill(0, HASH_FAST_BYTES_ALT, 1)


@pytest.fixture
def block_callbacks_initialized(sdk_initialized):
    cf.dap_chain_block_callbacks_init()
    yield
    cf.dap_chain_block_callbacks_deinit()


@pytest.fixture
def rpc_callbacks_initialized(sdk_initialized):
    cf.dap_chain_rpc_callbacks_init()
    yield
    cf.dap_chain_rpc_callbacks_deinit()


@pytest.fixture
def net_api_unregistered(sdk_initialized):
    cf.dap_chain_net_api_deinit()
    yield
    cf.dap_chain_net_api_deinit()


COMMON_METHODS = [
    "dap_chain_atom_verify_res_to_str",
    "dap_chain_block_callbacks_init",
    "dap_chain_block_callbacks_deinit",
    "dap_chain_block_callbacks_register_sovereign_tax",
    "dap_chain_block_callbacks_get_sovereign_tax",
    "dap_chain_addr_compare",
    "dap_chain_addr_fill",
    "dap_chain_addr_fill_from_key",
    "dap_chain_addr_fill_from_sign",
    "dap_chain_addr_from_str_array",
    "dap_chain_addr_to_str_static_",
    "dap_chain_balance_scan",
    "dap_chain_balance_to_coins_uint64",
    "dap_chain_hash_slow_kind_check",
    "dap_chain_hash_slow_to_str",
    "dap_chain_hash_slow_to_str_new",
    "dap_chain_mempool_group_name",
    "dap_chain_node_role_to_str",
    "dap_chain_set_offset_limit_json",
    "dap_chain_srv_str_to_unit_enum",
    "dap_chain_srv_uid_from_str",
    "dap_chain_srv_unit_enum_to_str",
    "dap_chain_uint128_from",
    "dap_chain_uint256_from",
    "dap_chain_uint256_from_uint128",
    "dap_chain_net_api_init",
    "dap_chain_net_api_deinit",
    "dap_chain_net_api_register",
    "dap_chain_net_api_by_id",
    "dap_chain_net_api_by_name",
    "dap_chain_net_api_get_chain_by_name",
    "dap_chain_net_api_get_chain_by_type",
    "dap_chain_net_api_get_default_chain_by_type",
    "dap_chain_net_api_get_cur_cell",
    "dap_chain_net_api_get_load_mode",
    "dap_chain_net_api_get_reward",
    "dap_chain_net_api_add_reward",
    "dap_chain_net_api_datum_add_to_mempool",
    "dap_chain_rpc_callbacks_init",
    "dap_chain_rpc_callbacks_deinit",
    "dap_chain_rpc_callbacks_register_consensus",
    "dap_chain_rpc_callbacks_register_storage",
    "dap_chain_rpc_callbacks_register_service",
    "dap_chain_rpc_callbacks_register_wallet",
    "dap_chain_rpc_callbacks_register_tx_notify",
    "dap_chain_rpc_callbacks_notify_consensus",
    "dap_chain_rpc_callbacks_notify_storage",
    "dap_chain_rpc_callbacks_notify_service",
    "dap_chain_rpc_callbacks_notify_wallet",
    "dap_chain_rpc_callbacks_notify_tx",
    "dap_chain_type_from_str",
    "dap_chain_type_to_datum_type",
    "dap_chain_type_to_str",
    "dap_datum_type_to_chain_type",
    "dap_datum_type_to_str",
    "dap_cli_error_codes_init",
    "dap_cli_error_codes_deinit",
    "dap_cli_error_code_register",
    "dap_cli_error_code_get",
    "dap_chain_addr_from_str",
    "dap_chain_addr_is_blank",
    "dap_chain_addr_check_sum",
]

NOARG_METHODS = {
    "dap_chain_block_callbacks_init",
    "dap_chain_block_callbacks_deinit",
    "dap_chain_net_api_init",
    "dap_chain_net_api_deinit",
    "dap_chain_rpc_callbacks_init",
    "dap_chain_rpc_callbacks_deinit",
    "dap_cli_error_codes_init",
    "dap_cli_error_codes_deinit",
}

REQUIRES_ARGS = [name for name in COMMON_METHODS if name not in NOARG_METHODS]


@pytest.mark.parametrize("name", REQUIRES_ARGS)
def test_methods_require_args(name, sdk_initialized):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


@pytest.mark.parametrize("name", sorted(NOARG_METHODS))
def test_noarg_methods_reject_args(name, sdk_initialized):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func(1)


def test_atom_verify_res_to_str():
    assert cf.dap_chain_atom_verify_res_to_str(0) == "accepted"
    assert cf.dap_chain_atom_verify_res_to_str(-1) == "rejected"
    assert cf.dap_chain_atom_verify_res_to_str(1) == "skipped"
    assert cf.dap_chain_atom_verify_res_to_str(-2) == "thresholded"
    assert cf.dap_chain_atom_verify_res_to_str(999) == "unknown"


def test_block_callbacks_init_deinit_roundtrip(block_callbacks_initialized):
    assert isinstance(cf.dap_chain_block_callbacks_init(), int)
    assert cf.dap_chain_block_callbacks_deinit() is None
    assert isinstance(cf.dap_chain_block_callbacks_init(), int)


def test_block_callbacks_register_requires_callable(block_callbacks_initialized):
    with pytest.raises(TypeError):
        cf.dap_chain_block_callbacks_register_sovereign_tax(123)


def test_block_callbacks_get_sovereign_tax(block_callbacks_initialized):
    def cb(net_id, pkey_hash, user_data):
        return {"has_tax": True, "sovereign_addr": "null", "sovereign_tax": "1"}

    cf.dap_chain_block_callbacks_register_sovereign_tax(cb, "ctx")
    result = cf.dap_chain_block_callbacks_get_sovereign_tax(1, HASH_FAST_BYTES)

    assert isinstance(result, dict)
    assert set(result.keys()) == {"has_tax", "sovereign_addr", "sovereign_tax"}
    assert result["has_tax"] is True
    assert is_capsule(result["sovereign_addr"])
    assert cf.dap_chain_addr_is_blank(result["sovereign_addr"]) is True
    assert isinstance(result["sovereign_tax"], (bytes, bytearray))
    assert len(result["sovereign_tax"]) == 32


def test_block_callbacks_get_sovereign_tax_returns_none(block_callbacks_initialized):
    def cb(net_id, pkey_hash, user_data):
        return None

    cf.dap_chain_block_callbacks_register_sovereign_tax(cb, None)
    result = cf.dap_chain_block_callbacks_get_sovereign_tax(1, HASH_FAST_BYTES)
    assert result is None


def test_block_callbacks_get_sovereign_tax_rejects_bad_hash(block_callbacks_initialized):
    def cb(net_id, pkey_hash, user_data):
        return None

    cf.dap_chain_block_callbacks_register_sovereign_tax(cb, None)
    with pytest.raises(ValueError):
        cf.dap_chain_block_callbacks_get_sovereign_tax(1, b"\x00")


def test_chain_addr_from_str_invalid():
    with pytest.raises(ValueError):
        cf.dap_chain_addr_from_str("not-an-address")


def test_chain_addr_from_str_array():
    result = cf.dap_chain_addr_from_str_array("null,null")
    assert isinstance(result, list)
    assert len(result) == 2
    assert is_capsule(result[0])
    assert is_capsule(result[1])
    with pytest.raises(ValueError):
        cf.dap_chain_addr_from_str_array("null,bad")


def test_chain_addr_compare(addr_null, addr_filled):
    assert cf.dap_chain_addr_compare(addr_null, addr_null) is True
    assert cf.dap_chain_addr_compare(addr_null, addr_filled) is False
    with pytest.raises(TypeError):
        cf.dap_chain_addr_compare(object(), addr_null)


def test_chain_addr_check_sum(addr_null, addr_filled):
    assert cf.dap_chain_addr_check_sum(addr_null) == 0
    assert cf.dap_chain_addr_check_sum(addr_filled) == 0
    with pytest.raises(ValueError):
        cf.dap_chain_addr_check_sum(object())


def test_chain_addr_is_blank(addr_null, addr_filled):
    assert cf.dap_chain_addr_is_blank(addr_null) is True
    assert cf.dap_chain_addr_is_blank(addr_filled) is False


def test_chain_addr_to_str_static(addr_null, addr_filled):
    assert cf.dap_chain_addr_to_str_static_(addr_null) == "null"
    filled_str = cf.dap_chain_addr_to_str_static_(addr_filled)
    assert isinstance(filled_str, str)
    assert filled_str and filled_str != "null"


def test_chain_addr_fill_rejects_bad_hash():
    with pytest.raises(ValueError):
        cf.dap_chain_addr_fill(0, b"\x00", 1)


def test_chain_addr_fill_from_key(sdk_initialized):
    crypto = pytest.importorskip("dap.crypto")
    try:
        key = crypto.DapKey.generate()
    except Exception as exc:
        pytest.skip(f"DapKey generation failed: {exc}")
    key_capsule = make_capsule_from_ptr(key.ptr, "dap_enc_key_t")
    rc, addr = cf.dap_chain_addr_fill_from_key(key_capsule, 1)
    assert rc == 0
    assert is_capsule(addr)
    assert cf.dap_chain_addr_check_sum(addr) == 0


def test_chain_addr_fill_from_sign(sdk_initialized):
    crypto = pytest.importorskip("dap.crypto")
    try:
        key = crypto.DapKey.generate()
        signature = key.sign(b"data")
    except Exception as exc:
        pytest.skip(f"Signature generation failed: {exc}")
    sign_capsule = make_capsule_from_ptr(signature.ptr, "dap_sign_t")
    rc, addr = cf.dap_chain_addr_fill_from_sign(sign_capsule, 1)
    assert rc == 0
    assert is_capsule(addr)
    assert cf.dap_chain_addr_check_sum(addr) == 0


def test_chain_balance_scan_and_to_coins():
    balance = cf.dap_chain_balance_scan("1.0")
    assert isinstance(balance, (bytes, bytearray))
    assert len(balance) == 32
    assert cf.dap_chain_balance_to_coins_uint64(balance) == 1


def test_chain_balance_scan_invalid_returns_zero():
    zero = cf.dap_chain_uint256_from(0)
    result = cf.dap_chain_balance_scan("not-a-number")
    assert result == zero
    with pytest.raises(ValueError):
        cf.dap_chain_balance_to_coins_uint64(b"\x00")


def test_hash_slow_kind_check():
    assert cf.dap_chain_hash_slow_kind_check(HASH_SLOW_ZERO, 32) == 0
    assert cf.dap_chain_hash_slow_kind_check(HASH_SLOW_ONE, 32) == 1
    assert cf.dap_chain_hash_slow_kind_check(HASH_SLOW_MIXED, 32) == -1
    with pytest.raises(ValueError):
        cf.dap_chain_hash_slow_kind_check(b"\x00", 1)


def test_hash_slow_to_str_roundtrip():
    text = cf.dap_chain_hash_slow_to_str(HASH_SLOW_ZERO)
    text_new = cf.dap_chain_hash_slow_to_str_new(HASH_SLOW_ZERO)
    assert isinstance(text, str)
    assert text.startswith("0x")
    assert len(text) == 66
    assert text_new == text


def test_mempool_group_name():
    assert cf.dap_chain_mempool_group_name("gdb", "main") == "gdb.chain-main.mempool"


def test_node_role_to_str():
    assert cf.dap_chain_node_role_to_str(0x00) == "root master"
    assert cf.dap_chain_node_role_to_str(0xFF) == "liht"
    assert cf.dap_chain_node_role_to_str(0x1234) == "none"


def test_set_offset_limit_json(sdk_initialized):
    dap_json = pytest.importorskip("dap.json")
    arr = dap_json.DapJson.array()
    start, end = cf.dap_chain_set_offset_limit_json(arr.capsule, 0, 0, 5, 2, 10, False)
    assert (start, end) == (2, 7)
    payload = json.loads(arr.to_string())
    assert isinstance(payload, list)
    assert payload
    assert payload[0]["offset"] == 2
    assert payload[0]["limit"] == 5


def test_chain_srv_unit_enum_roundtrip():
    assert cf.dap_chain_srv_str_to_unit_enum("SEC") == 0x2
    assert cf.dap_chain_srv_str_to_unit_enum("B") == 0x11
    assert cf.dap_chain_srv_str_to_unit_enum("PCS") == 0x22
    assert cf.dap_chain_srv_str_to_unit_enum("bad") == 0
    assert cf.dap_chain_srv_unit_enum_to_str(0x2) == "SEC"
    assert cf.dap_chain_srv_unit_enum_to_str(0x11) == "B"
    assert cf.dap_chain_srv_unit_enum_to_str(0x22) == "PCS"
    assert cf.dap_chain_srv_unit_enum_to_str(0x9999) == "UNDEFINED"


def test_chain_srv_uid_from_str():
    assert cf.dap_chain_srv_uid_from_str("1") == 1
    assert cf.dap_chain_srv_uid_from_str("0x10") == 16
    assert cf.dap_chain_srv_uid_from_str("bad") == 0


def test_uint128_uint256_conversions():
    uint128 = cf.dap_chain_uint128_from(42)
    uint256 = cf.dap_chain_uint256_from(42)
    assert isinstance(uint128, (bytes, bytearray))
    assert isinstance(uint256, (bytes, bytearray))
    assert len(uint128) == 16
    assert len(uint256) == 32
    uint256_from_128 = cf.dap_chain_uint256_from_uint128(uint128)
    assert uint256_from_128 == uint256


def test_net_api_init_deinit_roundtrip(sdk_initialized):
    assert isinstance(cf.dap_chain_net_api_init(), int)
    assert cf.dap_chain_net_api_deinit() is None
    assert isinstance(cf.dap_chain_net_api_init(), int)


def test_net_api_unregistered_returns_defaults(net_api_unregistered):
    net = make_capsule("dap_chain_net_t")
    chain = make_capsule("dap_chain_t")
    datum = make_capsule("dap_chain_datum_t")
    assert cf.dap_chain_net_api_by_id(1) is None
    assert cf.dap_chain_net_api_by_name("net") is None
    assert cf.dap_chain_net_api_get_chain_by_name(net, "chain") is None
    assert cf.dap_chain_net_api_get_chain_by_type(net, CHAIN_TYPE_TX) is None
    assert cf.dap_chain_net_api_get_default_chain_by_type(net, CHAIN_TYPE_TX) is None
    assert cf.dap_chain_net_api_get_cur_cell(net) is None
    assert cf.dap_chain_net_api_get_load_mode(net) is False
    reward = cf.dap_chain_net_api_get_reward(net, 1)
    assert reward == cf.dap_chain_uint256_from(0)
    assert cf.dap_chain_net_api_add_reward(net, reward, 1) == -1
    assert cf.dap_chain_net_api_datum_add_to_mempool(datum, chain) is None


def test_net_api_register_rejects_invalid_capsule(sdk_initialized):
    with pytest.raises(TypeError):
        cf.dap_chain_net_api_register(object())
    bad_capsule = make_capsule("wrong_registry")
    with pytest.raises(ValueError):
        cf.dap_chain_net_api_register(bad_capsule)


NET_API_TYPE_ERROR_CASES = [
    ("dap_chain_net_api_get_chain_by_name", (object(), "chain")),
    ("dap_chain_net_api_get_chain_by_type", (object(), CHAIN_TYPE_TX)),
    ("dap_chain_net_api_get_default_chain_by_type", (object(), CHAIN_TYPE_TX)),
    ("dap_chain_net_api_get_cur_cell", (object(),)),
    ("dap_chain_net_api_get_load_mode", (object(),)),
    ("dap_chain_net_api_get_reward", (object(), 1)),
    ("dap_chain_net_api_add_reward", (object(), b"\x00" * 32, 1)),
    ("dap_chain_net_api_datum_add_to_mempool", (object(), object(), "hex")),
]


@pytest.mark.parametrize("name,args", NET_API_TYPE_ERROR_CASES)
def test_net_api_rejects_non_capsule(name, args, net_api_unregistered):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func(*args)


def test_rpc_callbacks_init_deinit_roundtrip(sdk_initialized):
    assert isinstance(cf.dap_chain_rpc_callbacks_init(), int)
    assert cf.dap_chain_rpc_callbacks_deinit() is None
    assert isinstance(cf.dap_chain_rpc_callbacks_init(), int)


RPC_REGISTER_METHODS = [
    "dap_chain_rpc_callbacks_register_consensus",
    "dap_chain_rpc_callbacks_register_storage",
    "dap_chain_rpc_callbacks_register_service",
    "dap_chain_rpc_callbacks_register_wallet",
    "dap_chain_rpc_callbacks_register_tx_notify",
]


@pytest.mark.parametrize("name", RPC_REGISTER_METHODS)
def test_rpc_register_requires_callable(name, rpc_callbacks_initialized):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func(123)


def test_rpc_notify_consensus(rpc_callbacks_initialized):
    chain = make_capsule("dap_chain_t")
    net = make_capsule("dap_chain_net_t")
    called = {}

    def cb(chain_caps, net_caps, custom_data, user_data):
        called["args"] = (chain_caps, net_caps, custom_data, user_data)
        return 7

    assert cf.dap_chain_rpc_callbacks_register_consensus(cb, "ctx") == 0
    result = cf.dap_chain_rpc_callbacks_notify_consensus(chain, net)
    assert result == 7
    assert "args" in called
    args = called["args"]
    assert is_capsule(args[0])
    assert is_capsule(args[1])
    assert args[2] is None
    assert args[3] == "ctx"


def test_rpc_notify_storage(rpc_callbacks_initialized):
    chain = make_capsule("dap_chain_t")
    net = make_capsule("dap_chain_net_t")
    called = {}

    def cb(chain_caps, net_caps, block_hash, custom_data, user_data):
        called["args"] = (chain_caps, net_caps, block_hash, custom_data, user_data)
        return 3

    assert cf.dap_chain_rpc_callbacks_register_storage(cb, "ctx") == 0
    result = cf.dap_chain_rpc_callbacks_notify_storage(chain, net, HASH_FAST_BYTES)
    assert result == 3
    args = called["args"]
    assert is_capsule(args[0])
    assert is_capsule(args[1])
    assert isinstance(args[2], (bytes, bytearray))
    assert args[3] is None
    assert args[4] == "ctx"


def test_rpc_notify_service(rpc_callbacks_initialized):
    chain = make_capsule("dap_chain_t")
    net = make_capsule("dap_chain_net_t")
    called = {}

    def cb(chain_caps, net_caps, service_name, custom_data, user_data):
        called["args"] = (chain_caps, net_caps, service_name, custom_data, user_data)
        return 1

    assert cf.dap_chain_rpc_callbacks_register_service(cb, "ctx") == 0
    result = cf.dap_chain_rpc_callbacks_notify_service(chain, net, "service")
    assert result == 1
    args = called["args"]
    assert args[2] == "service"
    assert args[3] is None
    assert args[4] == "ctx"


def test_rpc_notify_wallet(rpc_callbacks_initialized):
    net = make_capsule("dap_chain_net_t")
    called = {}

    def cb(net_caps, wallet_name, value, custom_data, user_data):
        called["args"] = (net_caps, wallet_name, value, custom_data, user_data)
        return 5

    assert cf.dap_chain_rpc_callbacks_register_wallet(cb, "ctx") == 0
    value = cf.dap_chain_balance_scan("1.0")
    result = cf.dap_chain_rpc_callbacks_notify_wallet(net, "wallet", value)
    assert result == 5
    args = called["args"]
    assert is_capsule(args[0])
    assert args[1] == "wallet"
    assert isinstance(args[2], (bytes, bytearray))
    assert args[3] is None
    assert args[4] == "ctx"


def test_rpc_notify_tx(rpc_callbacks_initialized):
    net = make_capsule("dap_chain_net_t")
    datum = make_capsule("dap_chain_datum_t")
    ledger = make_capsule("dap_ledger_t")
    called = {}

    def cb(net_caps, tx_datum, tx_hash, ledger_caps, custom_data, user_data):
        called["args"] = (net_caps, tx_datum, tx_hash, ledger_caps, custom_data, user_data)

    assert cf.dap_chain_rpc_callbacks_register_tx_notify(cb, "ctx") == 0
    result = cf.dap_chain_rpc_callbacks_notify_tx(net, datum, HASH_FAST_BYTES, ledger)
    assert result is None
    args = called["args"]
    assert is_capsule(args[0])
    assert is_capsule(args[1])
    assert isinstance(args[2], (bytes, bytearray))
    assert is_capsule(args[3])
    assert args[4] is None
    assert args[5] == "ctx"


def test_rpc_notify_without_init_returns_negative():
    cf.dap_chain_rpc_callbacks_deinit()
    chain = make_capsule("dap_chain_t")
    net = make_capsule("dap_chain_net_t")
    result = cf.dap_chain_rpc_callbacks_notify_consensus(chain, net)
    assert result < 0


def test_chain_type_mappings():
    assert cf.dap_chain_type_from_str("token") == CHAIN_TYPE_TOKEN
    assert cf.dap_chain_type_from_str("emission") == CHAIN_TYPE_EMISSION
    assert cf.dap_chain_type_from_str("transaction") == CHAIN_TYPE_TX
    assert cf.dap_chain_type_from_str("bad") == CHAIN_TYPE_INVALID
    assert cf.dap_chain_type_to_datum_type(CHAIN_TYPE_TOKEN) == DAP_CHAIN_DATUM_TOKEN
    assert cf.dap_chain_type_to_datum_type(CHAIN_TYPE_DECREE) == DAP_CHAIN_DATUM_DECREE
    assert cf.dap_chain_type_to_datum_type(999) == DAP_CHAIN_DATUM_CUSTOM
    assert cf.dap_chain_type_to_str(CHAIN_TYPE_TX) == "transaction"
    assert cf.dap_chain_type_to_str(CHAIN_TYPE_INVALID) == "invalid"
    assert cf.dap_chain_type_to_str(999) == "custom"


def test_datum_type_mappings():
    assert cf.dap_datum_type_to_chain_type(DAP_CHAIN_DATUM_TOKEN) == CHAIN_TYPE_TOKEN
    assert cf.dap_datum_type_to_chain_type(DAP_CHAIN_DATUM_ANCHOR) == CHAIN_TYPE_ANCHOR
    assert cf.dap_datum_type_to_chain_type(DAP_CHAIN_DATUM_CUSTOM) == CHAIN_TYPE_INVALID
    assert cf.dap_datum_type_to_str(DAP_CHAIN_DATUM_TX) == "transaction"
    assert cf.dap_datum_type_to_str(DAP_CHAIN_DATUM_CUSTOM) == "invalid"


def test_cli_error_codes_register_get(sdk_initialized):
    cf.dap_cli_error_codes_init()
    name = f"ERR_TEST_{uuid.uuid4().hex}"
    code = 12345
    assert cf.dap_cli_error_code_register(name, code, "test error") == 0
    assert cf.dap_cli_error_code_get(name) == code
    assert cf.dap_cli_error_code_get("UNKNOWN_CODE") == -1
    assert cf.dap_cli_error_codes_deinit() is None
