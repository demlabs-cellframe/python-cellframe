import ctypes

import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_SIZE = 32
HASH_BYTES = b"\x00" * HASH_SIZE
BAD_HASH_BYTES = b"\x00" * (HASH_SIZE - 1)
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


NET_SRV_REQUIRED = [
    "net_srv_create",
    "net_srv_del",
    "net_srv_issue_receipt",
    "net_srv_get_price_from_order",
    "net_srv_price_unit_uid_from_str",
    "net_srv_price_unit_uid_to_str",
    "net_srv_uid_compare",
    "net_srv_uid_compare_scalar",
    "net_srv_order_check",
    "net_srv_order_compose",
    "net_srv_order_create",
    "net_srv_order_save",
    "net_srv_order_find_by_hash",
    "net_srv_order_find_by_hash_str",
    "net_srv_order_delete_by_hash_str",
    "net_srv_order_delete_by_hash",
    "net_srv_order_get_size",
    "net_srv_order_find_all_by",
    "net_srv_order_dump_to_string",
    "net_srv_order_dump_to_json",
    "net_srv_order_get_sign",
    "net_srv_order_get_gdb_group",
    "net_srv_order_get_common_group",
    "net_srv_order_set_continent_region",
    "net_srv_order_get_continent_region",
    "net_srv_order_get_country_code",
    "net_srv_order_continent_to_str",
    "net_srv_order_continent_to_num",
    "net_srv_client_connect",
    "net_srv_client_close",
    "net_srv_client_is_connected",
    "net_srv_client_error_str",
    "net_srv_client_check",
    "net_srv_client_request",
    "net_srv_client_write",
    "net_srv_ch_grace_control",
    "net_srv_ch_pkt_data_write",
    "net_srv_ch_pkt_data_write_f",
    "net_srv_stream_session_create",
    "net_srv_stream_session_delete",
    "net_srv_usage_add",
    "net_srv_usage_delete",
    "net_srv_usage_find_unsafe",
    "net_srv_get_chain_id_by_name",
    "net_srv_get_chain_id_by_type",
    "net_srv_get_net_id_by_name",
    "net_srv_tx_create_cond_input",
    "net_srv_tx_create_cond_output",
    "chain_net_geoip_init",
    "chain_net_geoip_get_ip_info",
]


@pytest.mark.parametrize("name", NET_SRV_REQUIRED)
def test_net_srv_methods_require_args(name):
    func = getattr(cf, name)
    with pytest.raises(TypeError):
        func()


def test_net_srv_init_deinit_smoke(sdk_initialized):
    assert isinstance(cf.net_srv_init(), int)
    assert cf.net_srv_deinit() is None


def test_net_srv_order_init_deinit_smoke(sdk_initialized):
    assert isinstance(cf.net_srv_order_init(), int)
    assert cf.net_srv_order_deinit() is None


def test_net_srv_order_pin_init_smoke(sdk_initialized):
    assert isinstance(cf.net_srv_order_pin_init(), int)


def test_net_srv_ch_init_smoke(sdk_initialized):
    assert isinstance(cf.net_srv_ch_init(), int)


def test_net_srv_ch_create_statistic_report_smoke(sdk_initialized):
    report = cf.net_srv_ch_create_statistic_report()
    assert report is None or isinstance(report, str)


def test_net_srv_set_net_callbacks_smoke(sdk_initialized):
    assert cf.net_srv_set_net_callbacks() is None


def test_net_srv_tx_builders_register_unregister_smoke(sdk_initialized):
    assert isinstance(cf.net_srv_tx_builders_register(), int)
    assert cf.net_srv_tx_builders_unregister() is None


def test_net_srv_price_unit_uid_conversions():
    assert isinstance(cf.net_srv_price_unit_uid_from_str("sec"), int)
    assert isinstance(cf.net_srv_price_unit_uid_to_str(0), str)
    with pytest.raises(TypeError):
        cf.net_srv_price_unit_uid_from_str(123)
    with pytest.raises(TypeError):
        cf.net_srv_price_unit_uid_to_str("bad")


def test_net_srv_uid_compare_helpers():
    assert cf.net_srv_uid_compare(1, 1) is True
    assert cf.net_srv_uid_compare(1, 2) is False
    assert cf.net_srv_uid_compare_scalar(1, 1) is True
    assert cf.net_srv_uid_compare_scalar(1, 2) is False


def test_net_srv_order_continent_helpers():
    assert isinstance(cf.net_srv_order_continents_count(), int)
    assert isinstance(cf.net_srv_order_continent_to_str(0), str)
    assert isinstance(cf.net_srv_order_continent_to_num("EU"), int)


def test_net_srv_client_error_str():
    assert isinstance(cf.net_srv_client_error_str(0), str)
    with pytest.raises(TypeError):
        cf.net_srv_client_error_str("bad")


def test_net_srv_get_ids_helpers():
    assert isinstance(cf.net_srv_get_chain_id_by_name(0, "chain"), int)
    assert isinstance(cf.net_srv_get_chain_id_by_type(0, 0), int)
    assert isinstance(cf.net_srv_get_net_id_by_name("net"), int)
    with pytest.raises(TypeError):
        cf.net_srv_get_chain_id_by_name("bad", "chain")
    with pytest.raises(TypeError):
        cf.net_srv_get_chain_id_by_type("bad", 0)
    with pytest.raises(TypeError):
        cf.net_srv_get_net_id_by_name(123)


def test_chain_net_geoip_rejects_invalid_args():
    with pytest.raises(TypeError):
        cf.chain_net_geoip_init(object())
    with pytest.raises(TypeError):
        cf.chain_net_geoip_get_ip_info(123)


def test_net_srv_order_check_rejects_invalid_order():
    with pytest.raises(TypeError):
        cf.net_srv_order_check(123, b"\x00")
    with pytest.raises(TypeError):
        cf.net_srv_order_check("hash", "bad")


def test_net_srv_ch_grace_control_rejects_invalid_hash():
    srv_capsule = make_capsule("dap_chain_net_srv_t")
    with pytest.raises(ValueError):
        cf.net_srv_ch_grace_control(srv_capsule, BAD_HASH_BYTES)


def test_net_srv_ch_pkt_data_write_rejects_invalid_data():
    ch_capsule = make_capsule("dap_stream_ch_t")
    with pytest.raises(TypeError):
        cf.net_srv_ch_pkt_data_write(ch_capsule, 1, 1, object())


def test_net_srv_tx_create_cond_input_rejects_invalid_hash():
    receipt_capsule = make_capsule("dap_chain_datum_tx_receipt_t")
    with pytest.raises(ValueError):
        cf.net_srv_tx_create_cond_input(BAD_HASH_BYTES, 0, receipt_capsule, object(), UINT256_BYTES, "TOK")


def test_net_srv_tx_create_cond_output_rejects_invalid_hash():
    with pytest.raises(ValueError):
        cf.net_srv_tx_create_cond_output(None, BAD_HASH_BYTES, "TOK", UINT256_BYTES, UINT256_BYTES, 0, 1, UINT256_BYTES)
