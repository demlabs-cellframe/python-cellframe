import ctypes

import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_SIZE = 32
HASH_BYTES = b"\x00" * HASH_SIZE
DATUM_SMALL = b"\x00"

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


NODE_REQUIRED = [
    "node_alias_register",
    "node_alias_find",
    "node_alias_delete",
    "node_info_save",
    "node_info_del",
    "node_info_read",
    "node_info_get_size",
    "node_get_states_list_sort",
    "node_cli_cmd_values_parse_net_chain_for_json",
    "node_mempool_need_process",
    "node_mempool_process",
    "node_mempool_process_all",
    "node_hardfork_process",
    "node_hardfork_check",
    "node_hardfork_confirm",
    "node_hardfork_data_cleanup",
    "node_sync_client_connect",
    "node_sync_handshake",
    "node_sync_request",
    "node_sync_request_ex",
    "node_sync_client_close",
    "node_sync_client_is_connected",
    "node_sync_error_str",
]


@pytest.mark.parametrize("name", NODE_REQUIRED)
def test_node_methods_require_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


def test_node_init_smoke(sdk_initialized):
    assert callable(cf.node_init)
    assert callable(cf.node_list_clean_init)


def test_node_mempool_autoproc_smoke(sdk_initialized):
    assert isinstance(cf.node_mempool_autoproc_init(), bool)
    assert cf.node_mempool_autoproc_deinit() is None


def test_node_sync_client_init_deinit_smoke(sdk_initialized):
    assert isinstance(cf.node_sync_client_init(), int)
    assert cf.node_sync_client_deinit() is None


def test_node_sync_error_str():
    assert isinstance(cf.node_sync_error_str(0), str)
    with pytest.raises(TypeError):
        cf.node_sync_error_str("bad")


def test_node_cli_cmd_values_parse_net_chain_for_json_invalid():
    with pytest.raises(TypeError):
        cf.node_cli_cmd_values_parse_net_chain_for_json("net", 0)
    with pytest.raises(TypeError):
        cf.node_cli_cmd_values_parse_net_chain_for_json([object()], 0)
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_json")
        cf.node_cli_cmd_values_parse_net_chain_for_json(["net"], 0, 0, bad_capsule)


def test_node_mempool_need_process_rejects_small_datum():
    chain_capsule = make_capsule("dap_chain_t")
    with pytest.raises(ValueError):
        cf.node_mempool_need_process(chain_capsule, DATUM_SMALL)


def test_node_mempool_process_rejects_small_datum():
    chain_capsule = make_capsule("dap_chain_t")
    with pytest.raises(ValueError):
        cf.node_mempool_process(chain_capsule, DATUM_SMALL, "hash")


def test_node_hardfork_rejects_small_datum():
    chain_capsule = make_capsule("dap_chain_t")
    with pytest.raises(ValueError):
        cf.node_hardfork_check(chain_capsule, DATUM_SMALL)
    with pytest.raises(ValueError):
        cf.node_hardfork_confirm(chain_capsule, DATUM_SMALL)


def test_node_states_info_read_rejects_invalid_args():
    with pytest.raises(TypeError):
        cf.node_states_info_read(object())
    with pytest.raises(TypeError):
        net_capsule = make_capsule("dap_chain_net_t")
        cf.node_states_info_read(net_capsule, object())


def test_node_info_get_size_rejects_invalid_capsule():
    with pytest.raises(TypeError):
        cf.node_info_get_size(object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_info")
        cf.node_info_get_size(bad_capsule)
