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


CHANNEL_REQUIRED = [
    "net_ch_pkt_write",
    "net_ch_pkt_write_f",
]


@pytest.mark.parametrize("name", CHANNEL_REQUIRED)
def test_net_channel_methods_require_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


def test_net_ch_init_deinit_smoke(sdk_initialized):
    assert isinstance(cf.net_ch_init(), int)
    assert cf.net_ch_deinit() is None


def test_net_cli_init_deinit_smoke(sdk_initialized):
    assert isinstance(cf.net_cli_init(), int)
    assert cf.net_cli_deinit() is None
    assert cf.net_cli_error_codes_init() is None


def test_net_decree_init_deinit_smoke(sdk_initialized):
    assert isinstance(cf.net_decree_init(), int)
    assert cf.net_decree_deinit() is None


def test_net_ch_pkt_write_rejects_invalid_args():
    ch_capsule = make_capsule("dap_stream_ch_t")
    with pytest.raises(TypeError):
        cf.net_ch_pkt_write(ch_capsule, 0, 1, "bad")
    with pytest.raises(ValueError):
        cf.net_ch_pkt_write(ch_capsule, 256, 1, b"")


def test_net_ch_pkt_write_f_rejects_invalid_args():
    ch_capsule = make_capsule("dap_stream_ch_t")
    with pytest.raises(ValueError):
        cf.net_ch_pkt_write_f(ch_capsule, 256, 1, "%s", None)
    with pytest.raises(TypeError):
        cf.net_ch_pkt_write_f(ch_capsule, 0, 1, "%s", object())
