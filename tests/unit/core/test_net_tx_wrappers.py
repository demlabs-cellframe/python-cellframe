import ctypes

import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_SIZE = 32
HASH_BYTES = b"\x00" * HASH_SIZE
BAD_HASH_BYTES = b"\x00" * (HASH_SIZE - 1)
TX_BYTES_SMALL = b"\x00"
UINT256_BYTES = b"\x00" * 32

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


NET_TX_REQUIRED = [
    "net_get_tx_all",
    "net_get_tx_by_hash",
    "net_get_tx_cond_chain",
    "net_get_tx_cond_all_by_srv_uid",
    "net_get_tx_cond_all_for_addr",
    "net_get_tx_cond_all_with_spends_by_srv_uid",
    "datum_tx_spends_item_free",
    "datum_tx_spends_items_free",
    "net_tx_create_by_json",
    "tx_datum_from_json",
    "net_tx_to_json",
    "wallet_tx_create_transfer",
    "wallet_tx_create_multi_transfer",
]


@pytest.mark.parametrize("name", NET_TX_REQUIRED)
def test_net_tx_methods_require_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


def test_net_tx_init_deinit_smoke(sdk_initialized):
    assert isinstance(cf.net_tx_init(), int)
    assert cf.net_tx_deinit() is None


def test_wallet_tx_builders_register_unregister_smoke(sdk_initialized):
    assert isinstance(cf.wallet_tx_builders_register(), int)
    assert cf.wallet_tx_builders_unregister() is None


def test_token_cli_init_deinit_smoke(sdk_initialized):
    assert isinstance(cf.token_cli_init(), int)
    assert cf.token_cli_deinit() is None
    assert isinstance(cf.token_cli_error_codes_init(), int)
    assert cf.token_cli_error_codes_deinit() is None


def test_net_tx_to_json_rejects_invalid_types():
    with pytest.raises(TypeError):
        cf.net_tx_to_json(123)
    with pytest.raises(ValueError):
        cf.net_tx_to_json(TX_BYTES_SMALL)


def test_net_tx_create_by_json_rejects_invalid_json():
    with pytest.raises(TypeError):
        cf.net_tx_create_by_json(object())


def test_tx_datum_from_json_rejects_invalid_json():
    with pytest.raises(TypeError):
        cf.tx_datum_from_json(object())


NET_METHODS_INVALID_NET = [
    ("net_get_tx_all", ()),
    ("net_get_tx_by_hash", (HASH_BYTES,)),
    ("net_get_tx_cond_chain", (HASH_BYTES, 1)),
    ("net_get_tx_cond_all_by_srv_uid", (1,)),
    ("net_get_tx_cond_all_for_addr", (make_capsule("dap_chain_addr_t"), 1)),
    ("net_get_tx_cond_all_with_spends_by_srv_uid", (1,)),
]


@pytest.mark.parametrize("name, extra_args", NET_METHODS_INVALID_NET)
def test_net_tx_methods_reject_invalid_net(name, extra_args):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func(object(), *extra_args)
    with pytest.raises(ValueError):
        bad_net = make_capsule("wrong_net")
        func(bad_net, *extra_args)


def test_datum_tx_spends_free_rejects_invalid_capsule():
    with pytest.raises(TypeError):
        cf.datum_tx_spends_item_free(object())
    with pytest.raises(TypeError):
        cf.datum_tx_spends_items_free(object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_spends")
        cf.datum_tx_spends_item_free(bad_capsule)
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_spends")
        cf.datum_tx_spends_items_free(bad_capsule)


def test_wallet_tx_create_transfer_rejects_invalid_args():
    with pytest.raises(TypeError):
        cf.wallet_tx_create_transfer("bad", object(), "TOK", UINT256_BYTES, UINT256_BYTES)
    with pytest.raises(TypeError):
        cf.wallet_tx_create_transfer(None, object(), "TOK", UINT256_BYTES, UINT256_BYTES)
    with pytest.raises(TypeError):
        cf.wallet_tx_create_transfer(None, object(), 123, UINT256_BYTES, UINT256_BYTES)


def test_wallet_tx_create_multi_transfer_rejects_invalid_args():
    with pytest.raises(TypeError):
        cf.wallet_tx_create_multi_transfer(None, object(), [], "TOK", UINT256_BYTES)
    with pytest.raises(ValueError):
        cf.wallet_tx_create_multi_transfer(None, [], [], "TOK", UINT256_BYTES)
    with pytest.raises(TypeError):
        cf.wallet_tx_create_multi_transfer(None, [object()], [UINT256_BYTES], "TOK", UINT256_BYTES)
