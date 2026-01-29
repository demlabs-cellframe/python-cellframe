import ctypes

import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

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


RPC_REQUIRED = [
    "node_rpc_init",
    "node_rpc_info_save",
    "node_rpc_info_del",
    "node_rpc_get_states_info_size",
]


@pytest.mark.parametrize("name", RPC_REQUIRED)
def test_rpc_methods_require_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


def test_rpc_noarg_smoke(sdk_initialized):
    assert callable(cf.node_rpc_deinit)
    assert callable(cf.node_rpc_list)
    assert callable(cf.node_rpc_get_states_sort)
    assert callable(cf.node_rpc_is_my_node_authorized)
    assert callable(cf.node_rpc_is_balancer)
    assert callable(cf.node_rpc_is_root)


def test_rpc_init_rejects_invalid_config():
    with pytest.raises(TypeError):
        cf.node_rpc_init(object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_config")
        cf.node_rpc_init(bad_capsule)


def test_rpc_states_info_read_rejects_invalid_addr():
    with pytest.raises(TypeError):
        cf.node_rpc_states_info_read(object())


def test_rpc_info_save_rejects_invalid_info():
    with pytest.raises(TypeError):
        cf.node_rpc_info_save(object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_info")
        cf.node_rpc_info_save(bad_capsule)


def test_rpc_info_del_rejects_invalid_addr():
    with pytest.raises(TypeError):
        cf.node_rpc_info_del(object())


def test_rpc_get_states_info_size_rejects_invalid_capsule():
    with pytest.raises(TypeError):
        cf.node_rpc_get_states_info_size(object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_states")
        cf.node_rpc_get_states_info_size(bad_capsule)
