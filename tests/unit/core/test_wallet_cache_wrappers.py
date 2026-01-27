import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

TOKEN_TICKER = "CELL"


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
def wallet_cache_ready(sdk_initialized):
    if not hasattr(cf, "dap_chain_wallet_cache_init"):
        pytest.skip("dap_chain_wallet_cache_init not available")
    try:
        rc = cf.dap_chain_wallet_cache_init()
    except Exception as exc:
        pytest.skip(f"wallet-cache init failed: {exc}")
    if not isinstance(rc, int):
        pytest.skip("wallet-cache init returned non-int")
    if rc < 0:
        pytest.skip("wallet-cache init returned error")
    yield rc
    try:
        cf.dap_chain_wallet_cache_deinit()
    except Exception:
        pass


@pytest.fixture
def addr_null(sdk_initialized):
    return cf.dap_chain_addr_from_str("null")


def test_wallet_cache_init_returns_int():
    if not hasattr(cf, "dap_chain_wallet_cache_init"):
        pytest.skip("dap_chain_wallet_cache_init not available")
    rc = cf.dap_chain_wallet_cache_init()
    assert isinstance(rc, int)


def test_wallet_cache_deinit_returns_int():
    if not hasattr(cf, "dap_chain_wallet_cache_deinit"):
        pytest.skip("dap_chain_wallet_cache_deinit not available")
    rc = cf.dap_chain_wallet_cache_deinit()
    assert isinstance(rc, int)


def test_wallet_cache_register_chain_rejects_bad_capsules():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_cache_register_chain(object(), object())


def test_wallet_cache_tx_find_rejects_bad_addr():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_cache_tx_find(object())


def test_wallet_cache_tx_find_rejects_bad_token(addr_null, wallet_cache_ready):
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_cache_tx_find(addr_null, 123)


def test_wallet_cache_tx_find_rejects_bad_hash(addr_null, wallet_cache_ready):
    with pytest.raises(ValueError):
        cf.dap_chain_wallet_cache_tx_find(addr_null, None, b"\x00")


def test_wallet_cache_tx_find_in_history_rejects_bad_hash(addr_null, wallet_cache_ready):
    with pytest.raises(ValueError):
        cf.dap_chain_wallet_cache_tx_find_in_history(addr_null, b"\x00")


def test_wallet_cache_tx_find_outs_mempool_check_rejects_bad_net(addr_null, wallet_cache_ready):
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_cache_tx_find_outs_mempool_check(object(), TOKEN_TICKER, addr_null)


def test_wallet_cache_iter_create_rejects_bad_addr():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_cache_iter_create(object())


def test_wallet_cache_iter_delete_rejects_bad_iter():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_cache_iter_delete(object())


def test_wallet_cache_iter_get_rejects_bad_iter():
    with pytest.raises(TypeError):
        cf.dap_chain_wallet_cache_iter_get(object(), 0)


def test_wallet_cache_iter_get_rejects_bad_type(addr_null, wallet_cache_ready):
    with pytest.raises(ValueError):
        cf.dap_chain_wallet_cache_iter_get(addr_null, 4)

