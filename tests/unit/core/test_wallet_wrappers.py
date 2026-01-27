import uuid

import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_SIZE = 32
HASH_FAST_BYTES = b"\x00" * HASH_SIZE
WALLET_SEED_BYTES = b"\x00" * 32
LONG_WALLET_NAME = "w" * 65
LONG_WALLET_PASS = "p" * 65
SIGN_TYPE_DILITHIUM = 0x0102


def is_capsule(obj):
    return type(obj).__name__ == "PyCapsule"


def unique_wallet_name(prefix="wallet"):
    return f"{prefix}_{uuid.uuid4().hex[:8]}"


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
def wallet_dir(tmp_path):
    path = tmp_path / "wallets"
    path.mkdir(parents=True, exist_ok=True)
    return path


@pytest.fixture
def addr_null(sdk_initialized):
    if not hasattr(cf, "dap_chain_addr_from_str"):
        pytest.skip("dap_chain_addr_from_str not available")
    return cf.dap_chain_addr_from_str("null")


WALLET_METHODS = [
    "dap_cert_to_addr",
    "dap_chain_wallet_activate",
    "dap_chain_wallet_add_wallet_created_notify",
    "dap_chain_wallet_add_wallet_opened_notify",
    "dap_chain_wallet_addr_cache_get_name",
    "dap_chain_wallet_check_sign",
    "dap_chain_wallet_cli_deinit",
    "dap_chain_wallet_cli_error_codes_deinit",
    "dap_chain_wallet_cli_error_codes_init",
    "dap_chain_wallet_cli_init",
    "dap_chain_wallet_close",
    "dap_chain_wallet_create",
    "dap_chain_wallet_create_with_pass",
    "dap_chain_wallet_create_with_seed",
    "dap_chain_wallet_create_with_seed_multi",
    "dap_chain_wallet_deactivate",
    "dap_chain_wallet_deinit",
    "dap_chain_wallet_get_addr",
    "dap_chain_wallet_get_balance",
    "dap_chain_wallet_get_certs_number",
    "dap_chain_wallet_get_key",
    "dap_chain_wallet_get_local_addr",
    "dap_chain_wallet_get_path",
    "dap_chain_wallet_get_pkey",
    "dap_chain_wallet_get_pkey_hash",
    "dap_chain_wallet_get_pkey_str",
    "dap_chain_wallet_init",
    "dap_chain_wallet_open",
    "dap_chain_wallet_open_ext",
    "dap_chain_wallet_open_file",
    "dap_chain_wallet_op_tx_request",
    "dap_chain_wallet_save",
    "dap_chain_wallet_save_file",
    "dap_chain_wallet_shared_get_tx_hashes_json",
    "dap_chain_wallet_shared_hold_tx_add",
]

REQUIRES_ARGS = [
    "dap_cert_to_addr",
    "dap_chain_wallet_activate",
    "dap_chain_wallet_add_wallet_created_notify",
    "dap_chain_wallet_add_wallet_opened_notify",
    "dap_chain_wallet_addr_cache_get_name",
    "dap_chain_wallet_check_sign",
    "dap_chain_wallet_close",
    "dap_chain_wallet_create",
    "dap_chain_wallet_create_with_pass",
    "dap_chain_wallet_create_with_seed",
    "dap_chain_wallet_create_with_seed_multi",
    "dap_chain_wallet_deactivate",
    "dap_chain_wallet_get_addr",
    "dap_chain_wallet_get_balance",
    "dap_chain_wallet_get_certs_number",
    "dap_chain_wallet_get_key",
    "dap_chain_wallet_get_pkey",
    "dap_chain_wallet_get_pkey_hash",
    "dap_chain_wallet_get_pkey_str",
    "dap_chain_wallet_open",
    "dap_chain_wallet_open_ext",
    "dap_chain_wallet_open_file",
    "dap_chain_wallet_op_tx_request",
    "dap_chain_wallet_save",
    "dap_chain_wallet_save_file",
    "dap_chain_wallet_shared_get_tx_hashes_json",
    "dap_chain_wallet_shared_hold_tx_add",
]

NOARG_INT_METHODS = [
    "dap_chain_wallet_init",
    "dap_chain_wallet_cli_error_codes_init",
    "dap_chain_wallet_cli_init",
]

NOARG_NONE_METHODS = [
    "dap_chain_wallet_deinit",
    "dap_chain_wallet_cli_error_codes_deinit",
    "dap_chain_wallet_cli_deinit",
]

NOARG_LIST_METHODS = [
    "dap_chain_wallet_get_local_addr",
]


def test_wallet_methods_present():
    missing = [name for name in WALLET_METHODS if not hasattr(cf, name)]
    assert not missing, f"Missing wallet wrappers: {', '.join(missing)}"


@pytest.mark.parametrize("name", REQUIRES_ARGS)
def test_wallet_methods_require_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


@pytest.mark.parametrize("name", NOARG_INT_METHODS)
def test_wallet_noarg_int_methods(sdk_initialized, name):
    result = getattr(cf, name)()
    assert isinstance(result, int)


@pytest.mark.parametrize("name", NOARG_NONE_METHODS)
def test_wallet_noarg_none_methods(sdk_initialized, name):
    result = getattr(cf, name)()
    assert result is None


@pytest.mark.parametrize("name", NOARG_LIST_METHODS)
def test_wallet_noarg_list_methods(sdk_initialized, name):
    result = getattr(cf, name)()
    assert isinstance(result, list)
    for item in result:
        assert is_capsule(item)


def test_wallet_get_path(sdk_initialized):
    result = cf.dap_chain_wallet_get_path()
    assert result is None or isinstance(result, str)


def test_wallet_get_path_rejects_bad_config(sdk_initialized):
    with pytest.raises(ValueError):
        cf.dap_chain_wallet_get_path(object())


def test_wallet_create_with_seed_multi_rejects_empty_sig_types(sdk_initialized, wallet_dir):
    with pytest.raises(ValueError):
        cf.dap_chain_wallet_create_with_seed_multi(
            unique_wallet_name(),
            str(wallet_dir),
            [],
            WALLET_SEED_BYTES,
        )


def test_wallet_create_with_seed_multi_rejects_bad_sig_type(sdk_initialized, wallet_dir):
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_create_with_seed_multi(
            unique_wallet_name(),
            str(wallet_dir),
            ["bad"],
            WALLET_SEED_BYTES,
        )


def test_wallet_create_with_seed_multi_rejects_bad_seed_hex(sdk_initialized, wallet_dir):
    with pytest.raises(ValueError):
        cf.dap_chain_wallet_create_with_seed_multi(
            unique_wallet_name(),
            str(wallet_dir),
            [SIGN_TYPE_DILITHIUM],
            "0",
        )


def test_wallet_create_with_seed_multi_rejects_bad_seed_type(sdk_initialized, wallet_dir):
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_create_with_seed_multi(
            unique_wallet_name(),
            str(wallet_dir),
            [SIGN_TYPE_DILITHIUM],
            123,
        )


def test_wallet_create_with_seed_multi_rejects_bad_pass_type(sdk_initialized, wallet_dir):
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_create_with_seed_multi(
            unique_wallet_name(),
            str(wallet_dir),
            [SIGN_TYPE_DILITHIUM],
            WALLET_SEED_BYTES,
            object(),
        )


def test_wallet_create_with_seed_rejects_bad_seed_hex(sdk_initialized, wallet_dir):
    with pytest.raises(ValueError):
        cf.dap_chain_wallet_create_with_seed(
            unique_wallet_name("seed_bad"),
            str(wallet_dir),
            SIGN_TYPE_DILITHIUM,
            "0",
        )


def test_wallet_create_with_seed_rejects_bad_sig_type(sdk_initialized, wallet_dir):
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_create_with_seed(
            unique_wallet_name("seed_bad_sig"),
            str(wallet_dir),
            "bad",
            WALLET_SEED_BYTES,
        )


def test_wallet_create_with_pass_rejects_long_pass(sdk_initialized, wallet_dir):
    with pytest.raises(Exception):
        cf.dap_chain_wallet_create_with_pass(
            unique_wallet_name("pass_long"),
            str(wallet_dir),
            LONG_WALLET_PASS,
        )


def test_wallet_create_rejects_long_name(sdk_initialized, wallet_dir):
    with pytest.raises(Exception):
        cf.dap_chain_wallet_create(
            LONG_WALLET_NAME,
            str(wallet_dir),
            SIGN_TYPE_DILITHIUM,
        )


def test_wallet_open_file_rejects_bad_pass_type(sdk_initialized, wallet_dir):
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_open_file(str(wallet_dir / "missing.dwallet"), 123)


def test_wallet_close_invalid_capsule():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_close(object())


def test_wallet_create_with_pass_rejects_bad_pass_type(sdk_initialized, wallet_dir):
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_create_with_pass(
            unique_wallet_name("pass_bad"),
            str(wallet_dir),
            123,
        )


def test_wallet_activate_rejects_long_name_or_pass(sdk_initialized):
    rc = cf.dap_chain_wallet_activate(LONG_WALLET_NAME, "pass")
    assert isinstance(rc, int)
    assert rc < 0

    rc = cf.dap_chain_wallet_activate("name", LONG_WALLET_PASS)
    assert isinstance(rc, int)
    assert rc < 0


def test_wallet_deactivate_rejects_long_name():
    rc = cf.dap_chain_wallet_deactivate(LONG_WALLET_NAME)
    assert isinstance(rc, int)
    assert rc < 0


def test_wallet_addr_cache_get_name(addr_null):
    result = cf.dap_chain_wallet_addr_cache_get_name(addr_null)
    assert result is None or isinstance(result, str)


def test_wallet_addr_cache_get_name_rejects_bad_addr():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_addr_cache_get_name(object())


def test_wallet_add_wallet_opened_notify_rejects_non_callable():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_add_wallet_opened_notify(123)


def test_wallet_add_wallet_created_notify_rejects_non_callable():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_add_wallet_created_notify(123)


def test_wallet_cert_to_addr_rejects_empty_list():
    with pytest.raises(ValueError):
        cf.dap_cert_to_addr([], 0, 1)


def test_wallet_cert_to_addr_rejects_bad_list_type():
    with pytest.raises(TypeError):
        cf.dap_cert_to_addr("bad", 0, 1)


def test_wallet_cert_to_addr_rejects_bad_net_id():
    with pytest.raises(TypeError):
        cf.dap_cert_to_addr([object()], 0, object())


def test_wallet_get_balance_invalid_wallet():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_get_balance(object(), 1, "CELL")


def test_wallet_shared_get_tx_hashes_json_invalid_hash():
    with pytest.raises(ValueError):
        cf.dap_chain_wallet_shared_get_tx_hashes_json(b"\x00", "net")


def test_wallet_shared_get_tx_hashes_json_returns_str_or_none():
    result = cf.dap_chain_wallet_shared_get_tx_hashes_json(HASH_FAST_BYTES, "net")
    assert result is None or isinstance(result, str)


def test_wallet_shared_hold_tx_add_rejects_bad_tx():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_shared_hold_tx_add(object(), "net")


def test_wallet_op_tx_request_invalid_wallet():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_op_tx_request(object(), 0, object(), 0, object(), "null")


def test_wallet_op_tx_request_requires_args():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_op_tx_request()


def test_wallet_invalid_wallet_capsules():
    cases = [
        ("dap_chain_wallet_save", (object(),)),
        ("dap_chain_wallet_save_file", (object(),)),
        ("dap_chain_wallet_get_addr", (object(), 1)),
        ("dap_chain_wallet_get_certs_number", (object(),)),
        ("dap_chain_wallet_get_pkey", (object(), 0)),
        ("dap_chain_wallet_get_key", (object(), 0)),
        ("dap_chain_wallet_check_sign", (object(),)),
        ("dap_chain_wallet_get_pkey_hash", (object(),)),
        ("dap_chain_wallet_get_pkey_str", (object(),)),
    ]
    for name, args in cases:
        func = getattr(cf, name)
        with pytest.raises(TypeError):
            func(*args)
