import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_SIZE = 32
HASH_FAST_BYTES = b"\x00" * HASH_SIZE


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


def test_wallet_shared_init_returns_int(sdk_initialized):
    if not hasattr(cf, "dap_chain_wallet_shared_init"):
        pytest.skip("dap_chain_wallet_shared_init not available")
    rc = cf.dap_chain_wallet_shared_init()
    assert isinstance(rc, int)


def test_wallet_shared_notify_init_returns_int(sdk_initialized):
    if not hasattr(cf, "dap_chain_wallet_shared_notify_init"):
        pytest.skip("dap_chain_wallet_shared_notify_init not available")
    rc = cf.dap_chain_wallet_shared_notify_init()
    assert isinstance(rc, int)


def test_wallet_shared_register_chain_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_shared_register_chain(object())


def test_wallet_shared_cli_rejects_bad_argv():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_shared_cli("bad")


def test_wallet_shared_cli_rejects_bad_argv_item():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_shared_cli([object()])


def test_wallet_shared_cli_rejects_bad_json_reply():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_shared_cli(["wallet_shared", "list"], object())


def test_wallet_shared_tx_find_in_mempool_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_shared_tx_find_in_mempool(object(), HASH_FAST_BYTES)


def test_wallet_shared_refilling_tx_create_rejects_bad_net():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_shared_refilling_tx_create(
            object(),
            object(),
            b"\x00" * 32,
            b"\x00" * 32,
            HASH_FAST_BYTES,
        )


def test_wallet_shared_taking_tx_sign_rejects_bad_net():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_shared_taking_tx_sign(object(), object(), object())


def test_wallet_shared_hold_tx_add_rejects_bad_tx():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_shared_hold_tx_add(object(), "net")


def test_wallet_shared_get_tx_hashes_json_rejects_bad_hash():
    with pytest.raises(ValueError):
        cf.dap_chain_wallet_shared_get_tx_hashes_json(b"\x00", "net")


def test_wallet_shared_get_tx_hashes_json_returns_str_or_none():
    result = cf.dap_chain_wallet_shared_get_tx_hashes_json(HASH_FAST_BYTES, "net")
    assert result is None or isinstance(result, str)
