import uuid

import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_FAST_SIZE = 32
HASH_FAST_BYTES = b"\x00" * HASH_FAST_SIZE
HASH_FAST_STR = "0x" + ("00" * HASH_FAST_SIZE)
DAP_CHAIN_DATUM_TX = 0x0100


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

    yield chain, net_name, chain_name, _delete

    if not deleted["value"]:
        _delete()


@pytest.fixture
def datum_tx_capsule(sdk_initialized):
    if not hasattr(cf, "dap_chain_datum_create"):
        pytest.skip("dap_chain_datum_create not available")
    datum = cf.dap_chain_datum_create(DAP_CHAIN_DATUM_TX, b"tx")
    if datum is None:
        pytest.skip("Failed to create datum")
    return datum


MEMPOOL_METHODS = [
    "chain_cs_mempool_datum_add",
    "chain_mempool_tx_create",
    "chain_mempool_datum_get",
    "dap_chain_mempool_add_proc",
    "dap_chain_mempool_group_new",
    "dap_chain_mempool_delete_callback_init",
    "dap_datum_mempool_init",
    "dap_chain_mempool_filter",
    "dap_chain_mempool_out_is_used",
    "dap_chain_mempool_cli_init",
    "dap_chain_mempool_cli_deinit",
    "com_mempool",
    "com_mempool_add",
]

NOARG_METHODS = {
    "dap_chain_mempool_delete_callback_init",
    "dap_datum_mempool_init",
    "dap_chain_mempool_cli_init",
    "dap_chain_mempool_cli_deinit",
}

REQUIRES_ARGS = [
    name for name in MEMPOOL_METHODS if name not in NOARG_METHODS
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


def test_chain_cs_mempool_datum_add_rejects_invalid(chain_context, datum_tx_capsule):
    chain, _, _, _ = chain_context
    with pytest.raises(TypeError):
        cf.chain_cs_mempool_datum_add(object(), datum_tx_capsule, "hex")
    with pytest.raises(TypeError):
        cf.chain_cs_mempool_datum_add(chain, object(), "hex")
    with pytest.raises(TypeError):
        cf.chain_cs_mempool_datum_add(chain, datum_tx_capsule, 123)


def test_chain_mempool_tx_create_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.chain_mempool_tx_create(object(), object(), object(), [], "TKN", "1", 0)


def test_chain_mempool_datum_get_rejects_invalid(chain_context):
    chain, _, _, _ = chain_context
    with pytest.raises(TypeError):
        cf.chain_mempool_datum_get(object(), HASH_FAST_STR)
    with pytest.raises(TypeError):
        cf.chain_mempool_datum_get(chain, 123)


def test_dap_chain_mempool_datum_add_rejects_invalid(chain_context, datum_tx_capsule):
    chain, _, _, _ = chain_context
    with pytest.raises(TypeError):
        cf.dap_chain_mempool_datum_add(object(), chain, "hex")
    with pytest.raises(TypeError):
        cf.dap_chain_mempool_datum_add(datum_tx_capsule, object(), "hex")


def test_dap_chain_mempool_add_proc_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_mempool_add_proc(object(), "/mempool")
    with pytest.raises(TypeError):
        cf.dap_chain_mempool_add_proc(object(), 123)


def test_dap_chain_mempool_group_new_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_mempool_group_new(object())


def test_dap_chain_mempool_group_new_returns_str(chain_context):
    chain, _, _, _ = chain_context
    result = cf.dap_chain_mempool_group_new(chain)
    assert result is None or isinstance(result, str)


def test_dap_chain_mempool_delete_callback_init_returns_int(sdk_initialized):
    result = cf.dap_chain_mempool_delete_callback_init()
    assert isinstance(result, int)


def test_dap_datum_mempool_init_returns_int(sdk_initialized):
    result = cf.dap_datum_mempool_init()
    assert isinstance(result, int)


def test_dap_chain_mempool_filter_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_mempool_filter(object())


def test_dap_chain_mempool_filter_returns_int(chain_context):
    chain, _, _, _ = chain_context
    result = cf.dap_chain_mempool_filter(chain)
    assert isinstance(result, int)


def test_dap_chain_mempool_out_is_used_rejects_bad_net():
    with pytest.raises(TypeError):
        cf.dap_chain_mempool_out_is_used(object(), HASH_FAST_BYTES, 0)


def test_mempool_cli_init_deinit_roundtrip(sdk_initialized):
    assert isinstance(cf.dap_chain_mempool_cli_init(), int)
    assert cf.dap_chain_mempool_cli_deinit() is None


def test_com_mempool_rejects_invalid_args():
    with pytest.raises(TypeError):
        cf.com_mempool("mempool")
    with pytest.raises(TypeError):
        cf.com_mempool([object()])
    with pytest.raises(TypeError):
        cf.com_mempool(["mempool"], object())


def test_com_mempool_returns_tuple(sdk_initialized):
    cf.dap_chain_mempool_cli_init()
    result = cf.com_mempool(["mempool"])
    assert isinstance(result, tuple)
    assert len(result) == 2
    code, payload = result
    assert isinstance(code, int)
    assert payload is None or isinstance(payload, str)


def test_com_mempool_add_rejects_invalid_args():
    with pytest.raises(TypeError):
        cf.com_mempool_add("mempool")
    with pytest.raises(TypeError):
        cf.com_mempool_add([object()])
    with pytest.raises(TypeError):
        cf.com_mempool_add(["mempool"], object())


def test_com_mempool_add_returns_tuple(sdk_initialized):
    cf.dap_chain_mempool_cli_init()
    result = cf.com_mempool_add(["mempool"])
    assert isinstance(result, tuple)
    assert len(result) == 2
    code, payload = result
    assert isinstance(code, int)
    assert payload is None or isinstance(payload, str)
