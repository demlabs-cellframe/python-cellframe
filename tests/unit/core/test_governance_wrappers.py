import ctypes

import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

_PyCapsule_New = ctypes.pythonapi.PyCapsule_New
_PyCapsule_New.restype = ctypes.py_object
_PyCapsule_New.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]
_capsule_keepalive = []


def make_capsule(name):
    buf = ctypes.create_string_buffer(64)
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
    return True


def test_dap_chain_decree_handler_register_rejects_invalid(sdk_initialized):
    with pytest.raises(TypeError):
        cf.dap_chain_decree_handler_register(1, 2, 123)


def test_dap_chain_decree_handler_register_returns_int(sdk_initialized):
    def handler(*_args):
        return 0

    rc = cf.dap_chain_decree_handler_register(0xFFFF, 0xFFFE, handler)
    assert isinstance(rc, int)
    assert rc == 0


def test_dap_chain_decree_handler_call_rejects_invalid(sdk_initialized):
    with pytest.raises(TypeError):
        cf.dap_chain_decree_handler_call(1, 2, object(), object(), None, False)


def test_dap_chain_decree_handler_call_no_handler_returns_int(sdk_initialized):
    decree = make_capsule("dap_chain_datum_decree_t")
    ledger = make_capsule("dap_ledger_t")
    rc = cf.dap_chain_decree_handler_call(0xFFFD, 0xFFFC, decree, ledger, None, False)
    assert isinstance(rc, int)
    assert rc == -1
