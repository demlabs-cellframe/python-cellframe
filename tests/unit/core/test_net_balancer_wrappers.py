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


BALANCER_REQUIRED = [
    "net_balancer_type_to_str",
    "net_balancer_http_issue_link",
    "net_balancer_dns_issue_link",
    "net_balancer_handshake",
    "net_balancer_get_node_str",
    "net_balancer_request",
    "net_node_list_get_gdb_group",
    "net_node_check_http_issue_link",
    "net_node_list_request",
    "net_node_list_ip_check",
]


@pytest.mark.parametrize("name", BALANCER_REQUIRED)
def test_balancer_methods_require_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


def test_net_balancer_deinit_smoke(sdk_initialized):
    assert cf.net_balancer_deinit() is None


def test_net_node_list_init_smoke(sdk_initialized):
    assert isinstance(cf.net_node_list_init(), int)


def test_net_balancer_type_to_str():
    assert isinstance(cf.net_balancer_type_to_str(0), str)
    with pytest.raises(TypeError):
        cf.net_balancer_type_to_str("bad")


def test_net_balancer_dns_issue_link_rejects_invalid_type():
    with pytest.raises(TypeError):
        cf.net_balancer_dns_issue_link(123)


def test_net_node_list_request_rejects_invalid_cmd():
    with pytest.raises(TypeError):
        cf.net_node_list_request(make_capsule("dap_chain_net_t"), 1, 0, 123)


def test_net_balancer_http_issue_link_rejects_invalid_capsule():
    with pytest.raises(TypeError):
        cf.net_balancer_http_issue_link(object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_http")
        cf.net_balancer_http_issue_link(bad_capsule)


def test_net_node_check_http_issue_link_rejects_invalid_capsule():
    with pytest.raises(TypeError):
        cf.net_node_check_http_issue_link(object())
    with pytest.raises(ValueError):
        bad_capsule = make_capsule("wrong_http")
        cf.net_node_check_http_issue_link(bad_capsule)


def test_net_node_list_ip_check_rejects_invalid_capsule():
    with pytest.raises(TypeError):
        cf.net_node_list_ip_check(object(), object())
    with pytest.raises(ValueError):
        bad_info = make_capsule("wrong_info")
        bad_net = make_capsule("wrong_net")
        cf.net_node_list_ip_check(bad_info, bad_net)
