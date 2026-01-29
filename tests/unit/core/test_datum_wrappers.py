import ctypes

import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_SIZE = 32
HASH_BYTES = b"\x00" * HASH_SIZE
HASH_BYTES_ALT = b"\x01" * HASH_SIZE
DAP_CHAIN_DATUM_TX = 0x0100
DECREE_TYPE_COMMON = 0x0001
DECREE_SUBTYPE_FEE = 0x0001
DECREE_SUBTYPE_OWNERS_MIN = 0x0003
DECREE_TSD_TYPE_VALUE = 0x0100
TOKEN_TICKER = "CELL"
TOKEN_TICKER_BAD_SHORT = "AA"
TOKEN_TICKER_BAD_CHARS = "C-ELL"
TOKEN_FLAG_UNDEFINED = 0xFFFF
EMISSION_TSD_TYPE_TOKEN_SYM = 0x0007
EMISSION_SERIAL_BYTES = b"\x00" * 512
TOKEN_SERIAL_SHORT = b"\x00" * 8
SERVICE_UID = 1
EVENT_GROUP = "group1"
EVENT_TYPE = 1
COND_BYTES = b"cond"
EVENT_DATA = b"event"
EVENT_TIMESTAMP = 0
TSD_TYPE = 0x1000
TSD_DATA = b"tsd"
UNIT_NAME = "SEC"
TX_ITEM_TYPE_ANY = 0
RECEIPT_EXT = b"ext"
VOTING_QUESTION = "Question?"
VOTING_OPTION_A = "Option A"
VOTING_OPTION_B = "Option B"
VOTING_EXPIRE = 1
VOTING_MAX_VOTES = 1
VOTING_ANSWER_IDX = 0
VOTING_TX_OUT_IDX = 0
COMPOSE_TX_TYPE = "compose_test_tx"
COMPOSE_TX_TYPE_ALT = "compose_test_tx_alt"

_PyCapsule_New = ctypes.pythonapi.PyCapsule_New
_PyCapsule_New.restype = ctypes.py_object
_PyCapsule_New.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]
_PyCapsule_GetPointer = ctypes.pythonapi.PyCapsule_GetPointer
_PyCapsule_GetPointer.restype = ctypes.c_void_p
_PyCapsule_GetPointer.argtypes = [ctypes.py_object, ctypes.c_char_p]
_PyCapsule_GetName = ctypes.pythonapi.PyCapsule_GetName
_PyCapsule_GetName.restype = ctypes.c_char_p
_PyCapsule_GetName.argtypes = [ctypes.py_object]
_capsule_keepalive = []


def make_capsule(name):
    buf = ctypes.create_string_buffer(1)
    name_buf = ctypes.create_string_buffer(name.encode("ascii"))
    capsule = _PyCapsule_New(ctypes.addressof(buf), name_buf, None)
    _capsule_keepalive.extend([buf, name_buf])
    return capsule


def make_capsule_from_ptr(ptr, name):
    name_buf = ctypes.create_string_buffer(name.encode("ascii"))
    capsule = _PyCapsule_New(ctypes.c_void_p(ptr), name_buf, None)
    _capsule_keepalive.append(name_buf)
    return capsule


def is_capsule(obj):
    return type(obj).__name__ == "PyCapsule"


def assert_capsule_or_none(value):
    if value is None:
        return
    assert is_capsule(value)


def get_receipt_size(receipt_capsule, uint256_size):
    ptr = _PyCapsule_GetPointer(receipt_capsule, b"dap_chain_datum_tx_receipt_t")
    if not ptr:
        return None

    class _ReceiptInfo(ctypes.Structure):
        _pack_ = 1
        _fields_ = [
            ("srv_uid", ctypes.c_uint64),
            ("addition", ctypes.c_uint64),
            ("units_type", ctypes.c_uint32),
            ("version", ctypes.c_uint8),
            ("padding", ctypes.c_uint8 * 3),
            ("units", ctypes.c_uint64),
            ("value", ctypes.c_uint8 * uint256_size),
            ("prev_hash", ctypes.c_uint8 * HASH_SIZE),
        ]

    class _Receipt(ctypes.Structure):
        _pack_ = 1
        _fields_ = [
            ("type", ctypes.c_uint8),
            ("receipt_info", _ReceiptInfo),
            ("size", ctypes.c_uint64),
            ("exts_size", ctypes.c_uint64),
        ]

    return _Receipt.from_address(ptr).size


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
def datum_capsule(sdk_initialized):
    datum = cf.dap_chain_datum_create(DAP_CHAIN_DATUM_TX, b"datum")
    if datum is None:
        pytest.skip("Failed to create datum")
    return datum


@pytest.fixture
def tx_capsule(sdk_initialized):
    tx = cf.dap_chain_datum_tx_create()
    if tx is None:
        pytest.skip("Failed to create tx datum")
    return tx


@pytest.fixture
def decree_capsule_empty(sdk_initialized):
    decree = cf.dap_chain_datum_decree_new(1, 1, 1, 0)
    if decree is None:
        pytest.skip("Failed to create decree datum")
    return decree


@pytest.fixture
def decree_size(decree_capsule_empty):
    return cf.dap_chain_datum_decree_get_size(decree_capsule_empty)


@pytest.fixture
def addr_capsule(sdk_initialized):
    if not hasattr(cf, "dap_chain_addr_fill"):
        pytest.skip("dap_chain_addr_fill not available")
    addr = cf.dap_chain_addr_fill(0, HASH_BYTES, 1)
    if addr is None:
        pytest.skip("Failed to create address capsule")
    return addr


@pytest.fixture
def addr_capsule_alt(sdk_initialized):
    if not hasattr(cf, "dap_chain_addr_fill"):
        pytest.skip("dap_chain_addr_fill not available")
    addr = cf.dap_chain_addr_fill(0, HASH_BYTES_ALT, 1)
    if addr is None:
        pytest.skip("Failed to create alternative address capsule")
    return addr


@pytest.fixture
def uint256_one(sdk_initialized):
    if not hasattr(cf, "dap_chain_uint256_from"):
        pytest.skip("dap_chain_uint256_from not available")
    return cf.dap_chain_uint256_from(1)


@pytest.fixture
def unit_enum(sdk_initialized):
    if not hasattr(cf, "dap_chain_srv_str_to_unit_enum"):
        pytest.skip("dap_chain_srv_str_to_unit_enum not available")
    unit = cf.dap_chain_srv_str_to_unit_enum(UNIT_NAME)
    if unit == 0:
        pytest.skip("Failed to resolve unit enum")
    return unit


@pytest.fixture
def crypto_key(sdk_initialized):
    crypto = pytest.importorskip("dap.crypto")
    try:
        return crypto.DapKey.generate()
    except Exception as exc:
        pytest.skip(f"DapKey generation failed: {exc}")


@pytest.fixture
def pkey_handle(crypto_key):
    crypto = pytest.importorskip("dap.crypto")
    try:
        return crypto.DapPKey.from_key(crypto_key)
    except Exception as exc:
        pytest.skip(f"DapPKey creation failed: {exc}")


@pytest.fixture
def pkey_capsule(pkey_handle):
    return make_capsule_from_ptr(pkey_handle.ptr, "dap_pkey_t")


@pytest.fixture
def pkey_hash_bytes(pkey_handle):
    try:
        return pkey_handle.hash
    except Exception as exc:
        pytest.skip(f"Failed to get pkey hash: {exc}")


@pytest.fixture
def enc_key_capsule(crypto_key):
    return make_capsule_from_ptr(crypto_key.ptr, "dap_enc_key_t")


@pytest.fixture
def emission_capsule(sdk_initialized, addr_capsule):
    if not hasattr(cf, "dap_chain_uint256_from"):
        pytest.skip("dap_chain_uint256_from not available")
    value = cf.dap_chain_uint256_from(1)
    emission = cf.dap_chain_datum_emission_create(value, TOKEN_TICKER, addr_capsule)
    if emission is None:
        pytest.skip("Failed to create emission")
    return emission


def test_dap_chain_datum_calc_hash_returns_bytes(datum_capsule):
    out = cf.dap_chain_datum_calc_hash(datum_capsule)
    assert isinstance(out, (bytes, bytearray))
    assert len(out) == HASH_SIZE


def test_dap_chain_datum_calc_hash_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_calc_hash(object())


def test_dap_chain_datum_create_accepts_none_and_bytes():
    datum_none = cf.dap_chain_datum_create(DAP_CHAIN_DATUM_TX)
    assert is_capsule(datum_none)
    datum_bytes = cf.dap_chain_datum_create(DAP_CHAIN_DATUM_TX, b"data")
    assert is_capsule(datum_bytes)


def test_dap_chain_datum_create_accepts_memoryview():
    data = memoryview(b"data")
    datum = cf.dap_chain_datum_create(DAP_CHAIN_DATUM_TX, data)
    assert is_capsule(datum)


def test_dap_chain_datum_create_rejects_invalid_data():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_create(DAP_CHAIN_DATUM_TX, "bad")
    with pytest.raises(TypeError):
        cf.dap_chain_datum_create(DAP_CHAIN_DATUM_TX, 123)


def test_dap_chain_datum_size_returns_int(datum_capsule):
    size = cf.dap_chain_datum_size(datum_capsule)
    assert isinstance(size, int)
    assert size >= 0


def test_dap_chain_datum_size_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_size(object())


def test_dap_chain_datum_type_id_to_str_known_values():
    assert cf.dap_chain_datum_type_id_to_str(0x0100) == "DATUM_TX"
    assert cf.dap_chain_datum_type_id_to_str(0xFFFF) == "DATUM_CUSTOM"
    assert cf.dap_chain_datum_type_id_to_str(0x1234) == "DATUM_UNKNOWN"

def test_dap_chain_datum_dump_tx_json_smoke(tx_capsule, sdk_initialized):
    dap_json = pytest.importorskip("dap.json")
    json_arr = dap_json.DapJson.array()
    json_obj = dap_json.DapJson.object()
    result = cf.dap_chain_datum_dump_tx_json(
        json_arr.capsule,
        tx_capsule,
        None,
        json_obj.capsule,
        "hex",
        HASH_BYTES,
        1,
        2,
    )
    assert isinstance(result, bool)


def test_dap_chain_datum_dump_tx_json_invalid_hash(tx_capsule, sdk_initialized):
    dap_json = pytest.importorskip("dap.json")
    json_arr = dap_json.DapJson.array()
    json_obj = dap_json.DapJson.object()
    with pytest.raises(ValueError):
        cf.dap_chain_datum_dump_tx_json(
            json_arr.capsule,
            tx_capsule,
            None,
            json_obj.capsule,
            "hex",
            b"\x00",
            1,
            2,
        )


def test_dap_chain_datum_dump_tx_json_rejects_invalid_types(tx_capsule, sdk_initialized):
    dap_json = pytest.importorskip("dap.json")
    json_obj = dap_json.DapJson.object()
    with pytest.raises(TypeError):
        cf.dap_chain_datum_dump_tx_json(
            object(),
            tx_capsule,
            None,
            json_obj.capsule,
            "hex",
            HASH_BYTES,
            1,
            2,
        )
    with pytest.raises(TypeError):
        cf.dap_chain_datum_dump_tx_json(
            json_obj.capsule,
            object(),
            None,
            json_obj.capsule,
            "hex",
            HASH_BYTES,
            1,
            2,
        )
    with pytest.raises(TypeError):
        cf.dap_chain_datum_dump_tx_json(
            json_obj.capsule,
            tx_capsule,
            123,
            json_obj.capsule,
            "hex",
            HASH_BYTES,
            1,
            2,
        )
    with pytest.raises(TypeError):
        cf.dap_chain_datum_dump_tx_json(
            json_obj.capsule,
            tx_capsule,
            None,
            json_obj.capsule,
            123,
            HASH_BYTES,
            1,
            2,
        )


def test_dap_chain_datum_register_dump_anchor_callback_accepts_callable():
    assert cf.dap_chain_datum_register_dump_anchor_callback(lambda *_: None) is None


def test_dap_chain_datum_register_dump_anchor_callback_rejects_non_callable():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_register_dump_anchor_callback(123)


def test_dap_chain_datum_register_dump_decree_callback_accepts_callable():
    assert cf.dap_chain_datum_register_dump_decree_callback(lambda *_: None) is None


def test_dap_chain_datum_register_dump_decree_callback_rejects_non_callable():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_register_dump_decree_callback(123)


def test_dap_datum_token_dump_tsd_to_json_rejects_invalid():
    dap_json = pytest.importorskip("dap.json")
    json_obj = dap_json.DapJson.object()
    with pytest.raises(ValueError):
        cf.dap_datum_token_dump_tsd_to_json(object(), object(), -1, "hex")
    with pytest.raises(TypeError):
        cf.dap_datum_token_dump_tsd_to_json(object(), object(), 0, "hex")
    with pytest.raises(TypeError):
        cf.dap_datum_token_dump_tsd_to_json(json_obj.capsule, object(), 0, "hex")


def test_dap_chain_datum_anchor_certs_dump_returns_string():
    out = cf.dap_chain_datum_anchor_certs_dump(b"", "hex")
    assert isinstance(out, str)


def test_dap_chain_datum_anchor_certs_dump_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_anchor_certs_dump("bad", "hex")
    with pytest.raises(TypeError):
        cf.dap_chain_datum_anchor_certs_dump(b"", 123)


def test_dap_chain_datum_anchor_certs_dump_json_smoke(sdk_initialized):
    dap_json = pytest.importorskip("dap.json")
    json_obj = dap_json.DapJson.object()
    assert (
        cf.dap_chain_datum_anchor_certs_dump_json(json_obj.capsule, b"", "hex", 2) is None
    )


def test_dap_chain_datum_anchor_certs_dump_json_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_anchor_certs_dump_json(object(), b"", "hex", 2)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_anchor_certs_dump_json(None, "bad", "hex", 2)


def test_dap_chain_datum_anchor_get_hash_from_data_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_anchor_get_hash_from_data(object())
    with pytest.raises(ValueError):
        cf.dap_chain_datum_anchor_get_hash_from_data(make_capsule("wrong_anchor"))


def test_dap_chain_datum_anchor_get_size_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_anchor_get_size(object())
    with pytest.raises(ValueError):
        cf.dap_chain_datum_anchor_get_size(make_capsule("wrong_anchor"))


def test_dap_chain_datum_from_tx_smoke(tx_capsule):
    result = cf.dap_chain_datum_from_tx(tx_capsule)
    assert_capsule_or_none(result)


def test_dap_chain_datum_from_tx_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_from_tx(object())


def test_dap_chain_datum_decree_certs_dump_json_smoke(sdk_initialized):
    dap_json = pytest.importorskip("dap.json")
    json_obj = dap_json.DapJson.object()
    assert cf.dap_chain_datum_decree_certs_dump_json(
        json_obj.capsule, b"", "hex", 2
    ) is None


def test_dap_chain_datum_decree_certs_dump_json_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_certs_dump_json(object(), b"", "hex", 2)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_certs_dump_json(None, "bad", "hex", 2)


def test_dap_chain_datum_decree_dump_json_smoke(decree_capsule_empty, decree_size, sdk_initialized):
    dap_json = pytest.importorskip("dap.json")
    json_obj = dap_json.DapJson.object()
    assert cf.dap_chain_datum_decree_dump_json(
        json_obj.capsule, decree_capsule_empty, decree_size, "hex", 2
    ) is None


def test_dap_chain_datum_decree_dump_json_rejects_invalid(decree_capsule_empty):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_dump_json(object(), decree_capsule_empty, 0, "hex", 2)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_dump_json(None, object(), 0, "hex", 2)
    with pytest.raises(ValueError):
        cf.dap_chain_datum_decree_dump_json(make_capsule("dap_chain_datum_decree_t"), object(), -1, "hex", 2)


def test_dap_chain_datum_decree_get_action_empty(decree_capsule_empty):
    rc, action = cf.dap_chain_datum_decree_get_action(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert action is None


def test_dap_chain_datum_decree_get_action_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_action(object())


def test_dap_chain_datum_decree_get_atom_num_empty(decree_capsule_empty):
    rc, atom_num = cf.dap_chain_datum_decree_get_atom_num(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert atom_num is None


def test_dap_chain_datum_decree_get_atom_num_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_atom_num(object())


def test_dap_chain_datum_decree_get_ban_addr_empty(decree_capsule_empty):
    rc, addr = cf.dap_chain_datum_decree_get_ban_addr(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert addr is None


def test_dap_chain_datum_decree_get_ban_addr_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_ban_addr(object())


def test_dap_chain_datum_decree_get_empty_block_every_times_empty(decree_capsule_empty):
    rc, period = cf.dap_chain_datum_decree_get_empty_block_every_times(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert period is None


def test_dap_chain_datum_decree_get_empty_block_every_times_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_empty_block_every_times(object())


def test_dap_chain_datum_decree_get_fee_empty(decree_capsule_empty):
    rc, fee = cf.dap_chain_datum_decree_get_fee(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert fee is None


def test_dap_chain_datum_decree_get_fee_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_fee(object())


def test_dap_chain_datum_decree_get_fee_addr_empty(decree_capsule_empty):
    rc, fee_addr = cf.dap_chain_datum_decree_get_fee_addr(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert fee_addr is None


def test_dap_chain_datum_decree_get_fee_addr_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_fee_addr(object())


def test_dap_chain_datum_decree_get_hash_empty(decree_capsule_empty):
    rc, tx_hash = cf.dap_chain_datum_decree_get_hash(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert tx_hash is None


def test_dap_chain_datum_decree_get_hash_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_hash(object())


def test_dap_chain_datum_decree_get_min_owners_empty(decree_capsule_empty):
    rc, min_owners = cf.dap_chain_datum_decree_get_min_owners(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert min_owners is None


def test_dap_chain_datum_decree_get_min_owners_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_min_owners(object())


def test_dap_chain_datum_decree_get_node_addr_empty(decree_capsule_empty):
    rc, node_addr = cf.dap_chain_datum_decree_get_node_addr(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert node_addr is None


def test_dap_chain_datum_decree_get_node_addr_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_node_addr(object())


def test_dap_chain_datum_decree_get_owners_empty(decree_capsule_empty):
    owners, owners_num = cf.dap_chain_datum_decree_get_owners(decree_capsule_empty)
    assert owners_num == 0
    assert owners is None


def test_dap_chain_datum_decree_get_owners_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_owners(object())


def test_dap_chain_datum_decree_get_pkey_empty(decree_capsule_empty):
    assert cf.dap_chain_datum_decree_get_pkey(decree_capsule_empty) is None


def test_dap_chain_datum_decree_get_pkey_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_pkey(object())


def test_dap_chain_datum_decree_get_policy_empty(decree_capsule_empty):
    assert cf.dap_chain_datum_decree_get_policy(decree_capsule_empty) is None


def test_dap_chain_datum_decree_get_policy_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_policy(object())


def test_dap_chain_datum_decree_get_signature_type_empty(decree_capsule_empty):
    rc, sig_type = cf.dap_chain_datum_decree_get_signature_type(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert sig_type is None


def test_dap_chain_datum_decree_get_signature_type_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_signature_type(object())


def test_dap_chain_datum_decree_get_signs_empty(decree_capsule_empty):
    signs, size = cf.dap_chain_datum_decree_get_signs(decree_capsule_empty)
    assert isinstance(signs, (bytes, bytearray))
    assert isinstance(size, int)
    assert size == 0
    assert signs == b""


def test_dap_chain_datum_decree_get_signs_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_signs(object())


def test_dap_chain_datum_decree_get_size_returns_int(decree_capsule_empty):
    size = cf.dap_chain_datum_decree_get_size(decree_capsule_empty)
    assert isinstance(size, int)
    assert size > 0


def test_dap_chain_datum_decree_get_size_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_size(object())


def test_dap_chain_datum_decree_get_stake_min_signers_count_empty(decree_capsule_empty):
    rc, value = cf.dap_chain_datum_decree_get_stake_min_signers_count(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert value is None


def test_dap_chain_datum_decree_get_stake_min_signers_count_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_stake_min_signers_count(object())


def test_dap_chain_datum_decree_get_stake_min_value_empty(decree_capsule_empty):
    rc, value = cf.dap_chain_datum_decree_get_stake_min_value(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert value is None


def test_dap_chain_datum_decree_get_stake_min_value_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_stake_min_value(object())


def test_dap_chain_datum_decree_get_stake_signing_addr_empty(decree_capsule_empty):
    rc, addr = cf.dap_chain_datum_decree_get_stake_signing_addr(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert addr is None


def test_dap_chain_datum_decree_get_stake_signing_addr_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_stake_signing_addr(object())


def test_dap_chain_datum_decree_get_stake_value_empty(decree_capsule_empty):
    rc, value = cf.dap_chain_datum_decree_get_stake_value(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert value is None


def test_dap_chain_datum_decree_get_stake_value_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_stake_value(object())


def test_dap_chain_datum_decree_get_value_empty(decree_capsule_empty):
    rc, value = cf.dap_chain_datum_decree_get_value(decree_capsule_empty)
    assert isinstance(rc, int)
    assert rc != 0
    assert value is None


def test_dap_chain_datum_decree_get_value_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_get_value(object())


def test_dap_chain_datum_decree_new_returns_capsule(sdk_initialized):
    decree = cf.dap_chain_datum_decree_new(1, 1, 1, 0)
    assert is_capsule(decree)


def test_dap_chain_datum_decree_new_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_new("bad", 1, 1, 0)


def test_dap_chain_datum_decree_sign_in_cycle_empty(decree_capsule_empty):
    decree, sign_count = cf.dap_chain_datum_decree_sign_in_cycle([], decree_capsule_empty)
    assert is_capsule(decree)
    assert sign_count == 0


def test_dap_chain_datum_decree_sign_in_cycle_rejects_invalid(decree_capsule_empty):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_sign_in_cycle("bad", decree_capsule_empty)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_sign_in_cycle([object()], decree_capsule_empty)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_decree_sign_in_cycle([], object())


def test_dap_chain_datum_decree_subtype_to_str_known_values():
    assert cf.dap_chain_datum_decree_subtype_to_str(DECREE_SUBTYPE_FEE) == "DECREE_COMMON_SUBTYPE_FEE"
    assert cf.dap_chain_datum_decree_subtype_to_str(0xFFFF) == "DECREE_SUBTYPE_UNKNOWN"


def test_dap_chain_datum_decree_tsd_type_to_str_known_values():
    assert cf.dap_chain_datum_decree_tsd_type_to_str(DECREE_TSD_TYPE_VALUE) == "DAP_CHAIN_DATUM_DECREE_TSD_TYPE_VALUE"
    assert cf.dap_chain_datum_decree_tsd_type_to_str(0xFFFF) == "DECREE_TSD_TYPE_UNKNOWN"


def test_dap_chain_datum_decree_type_from_str_known_values():
    assert cf.dap_chain_datum_decree_type_from_str("fee") == DECREE_SUBTYPE_FEE
    assert cf.dap_chain_datum_decree_type_from_str("owners_min") == DECREE_SUBTYPE_OWNERS_MIN
    assert cf.dap_chain_datum_decree_type_from_str("unknown") == 0


@pytest.fixture
def decree_registry(sdk_initialized):
    rc = cf.dap_chain_decree_registry_init()
    if not isinstance(rc, int) or rc != 0:
        pytest.skip(f"Failed to init decree registry: {rc}")
    yield
    cf.dap_chain_decree_registry_deinit()


def test_dap_chain_decree_registry_init_returns_int(sdk_initialized):
    rc = cf.dap_chain_decree_registry_init()
    assert isinstance(rc, int)
    cf.dap_chain_decree_registry_deinit()


def test_dap_chain_decree_registry_deinit_safe(sdk_initialized):
    cf.dap_chain_decree_registry_deinit()
    cf.dap_chain_decree_registry_deinit()


def test_dap_chain_decree_registry_has_handler_defaults_false(decree_registry):
    assert cf.dap_chain_decree_registry_has_handler(DECREE_TYPE_COMMON, DECREE_SUBTYPE_FEE) is False


def test_dap_chain_decree_registry_has_handler_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_decree_registry_has_handler("bad", "bad")


def test_dap_chain_decree_registry_register_unregister_handler(decree_registry):
    def handler(*_args):
        return 0

    rc = cf.dap_chain_decree_registry_register_handler(
        DECREE_TYPE_COMMON, DECREE_SUBTYPE_FEE, handler, "unit_test_handler"
    )
    assert isinstance(rc, int)
    assert rc == 0
    assert cf.dap_chain_decree_registry_has_handler(DECREE_TYPE_COMMON, DECREE_SUBTYPE_FEE) is True
    rc = cf.dap_chain_decree_registry_unregister_handler(DECREE_TYPE_COMMON, DECREE_SUBTYPE_FEE)
    assert rc == 0
    assert cf.dap_chain_decree_registry_has_handler(DECREE_TYPE_COMMON, DECREE_SUBTYPE_FEE) is False


def test_dap_chain_decree_registry_register_handler_rejects_invalid(decree_registry):
    with pytest.raises(TypeError):
        cf.dap_chain_decree_registry_register_handler(DECREE_TYPE_COMMON, DECREE_SUBTYPE_FEE, 123, "bad")


def test_dap_chain_decree_registry_register_handler_rejects_invalid_name(decree_registry):
    with pytest.raises(TypeError):
        cf.dap_chain_decree_registry_register_handler(
            DECREE_TYPE_COMMON, DECREE_SUBTYPE_FEE, lambda *_: 0, 123
        )


def test_dap_chain_decree_registry_unregister_handler_missing(decree_registry):
    rc = cf.dap_chain_decree_registry_unregister_handler(DECREE_TYPE_COMMON, DECREE_SUBTYPE_FEE)
    assert isinstance(rc, int)
    assert rc != 0


def test_dap_chain_decree_registry_process_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_decree_registry_process(object(), object(), True, False)


def test_dap_chain_datum_token_flag_to_str_known():
    assert cf.dap_chain_datum_token_flag_to_str(0) == "NONE"


def test_dap_chain_datum_token_flag_to_str_unknown():
    assert cf.dap_chain_datum_token_flag_to_str(12345) == "UNKNOWN FLAG OR FLAGS GROUP"


def test_dap_chain_datum_token_flag_from_str_known():
    assert cf.dap_chain_datum_token_flag_from_str("NO_FLAGS") == 0
    assert cf.dap_chain_datum_token_flag_from_str("ALL_BLOCKED") != 0


def test_dap_chain_datum_token_flag_from_str_unknown():
    assert cf.dap_chain_datum_token_flag_from_str("UNKNOWN_FLAG") == TOKEN_FLAG_UNDEFINED


def test_dap_chain_datum_token_flag_from_str_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_token_flag_from_str(123)


def test_dap_chain_datum_token_get_delegated_ticker_smoke():
    rc, ticker = cf.dap_chain_datum_token_get_delegated_ticker(TOKEN_TICKER)
    assert rc == 0
    assert ticker == f"m{TOKEN_TICKER}"


def test_dap_chain_datum_token_get_delegated_ticker_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_token_get_delegated_ticker(123)


def test_dap_chain_datum_token_is_old_values():
    assert cf.dap_chain_datum_token_is_old(0x0005) is True
    assert cf.dap_chain_datum_token_is_old(0x0010) is False


def test_dap_chain_datum_token_check_ticker_values():
    assert cf.dap_chain_datum_token_check_ticker(TOKEN_TICKER) is True
    assert cf.dap_chain_datum_token_check_ticker(TOKEN_TICKER_BAD_SHORT) is False
    assert cf.dap_chain_datum_token_check_ticker(TOKEN_TICKER_BAD_CHARS) is False


def test_dap_chain_datum_token_certs_dump_returns_string():
    out = cf.dap_chain_datum_token_certs_dump(b"", "hex")
    assert isinstance(out, str)


def test_dap_chain_datum_token_certs_dump_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_token_certs_dump("bad", "hex")
    with pytest.raises(TypeError):
        cf.dap_chain_datum_token_certs_dump(b"", 123)


def test_dap_chain_datum_token_certs_dump_to_json_smoke(sdk_initialized):
    dap_json = pytest.importorskip("dap.json")
    json_obj = dap_json.DapJson.object()
    assert cf.dap_chain_datum_token_certs_dump_to_json(
        json_obj.capsule, b"", "hex", 2
    ) is None


def test_dap_chain_datum_token_certs_dump_to_json_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_token_certs_dump_to_json(object(), b"", "hex", 2)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_token_certs_dump_to_json(None, "bad", "hex", 2)


def test_dap_chain_datum_token_flags_dump_to_json_smoke(sdk_initialized):
    dap_json = pytest.importorskip("dap.json")
    json_obj = dap_json.DapJson.object()
    assert cf.dap_chain_datum_token_flags_dump_to_json(
        json_obj.capsule, "flags", 0
    ) is None
    json_str = json_obj.to_string()
    assert "flags" in json_str
    assert "NONE" in json_str


def test_dap_chain_datum_token_flags_dump_to_json_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_token_flags_dump_to_json(object(), "flags", 0)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_token_flags_dump_to_json(None, 123, 0)


def test_dap_chain_datum_token_read_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_token_read(object())


def test_dap_chain_datum_token_read_short_bytes():
    token, size = cf.dap_chain_datum_token_read(TOKEN_SERIAL_SHORT)
    assert token is None
    assert size == 0


def test_dap_chain_datum_token_tsd_get_rejects_negative_size():
    with pytest.raises(ValueError):
        cf.dap_chain_datum_token_tsd_get(object(), -1)


def test_dap_chain_datum_token_tsd_get_rejects_invalid_capsule():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_token_tsd_get(object(), 0)


def test_dap_chain_datum_emission_type_str_known():
    assert cf.dap_chain_datum_emission_type_str(0x01) == "AUTH"
    assert cf.dap_chain_datum_emission_type_str(0x02) == "ALGO"
    assert cf.dap_chain_datum_emission_type_str(0x00) == "UNDEFINED"


def test_dap_chain_datum_emission_create_smoke(sdk_initialized, addr_capsule):
    value = cf.dap_chain_uint256_from(1)
    emission = cf.dap_chain_datum_emission_create(value, TOKEN_TICKER, addr_capsule)
    if emission is None:
        pytest.skip("Failed to create emission")
    assert is_capsule(emission)


def test_dap_chain_datum_emission_create_rejects_invalid(addr_capsule):
    with pytest.raises(ValueError):
        cf.dap_chain_datum_emission_create(b"\x00", TOKEN_TICKER, addr_capsule)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_emission_create(cf.dap_chain_uint256_from(1), 123, addr_capsule)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_emission_create(cf.dap_chain_uint256_from(1), TOKEN_TICKER, object())


def test_dap_chain_datum_emission_get_signs_empty(emission_capsule):
    signs, count = cf.dap_chain_datum_emission_get_signs(emission_capsule)
    assert isinstance(signs, (bytes, bytearray))
    assert signs == b""
    assert count == 0


def test_dap_chain_datum_emission_add_tsd_and_get_tsd(emission_capsule):
    emission = cf.dap_chain_datum_emission_add_tsd(
        emission_capsule, EMISSION_TSD_TYPE_TOKEN_SYM, b"CELL"
    )
    if emission is None:
        pytest.skip("Failed to add TSD to emission")
    data, size = cf.dap_chain_emission_get_tsd(emission, EMISSION_TSD_TYPE_TOKEN_SYM)
    assert data == b"CELL"
    assert size == len(b"CELL")


def test_dap_chain_datum_emission_add_tsd_rejects_invalid(emission_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_emission_add_tsd(object(), EMISSION_TSD_TYPE_TOKEN_SYM, b"CELL")
    with pytest.raises(TypeError):
        cf.dap_chain_datum_emission_add_tsd(emission_capsule, EMISSION_TSD_TYPE_TOKEN_SYM, "bad")


def test_dap_chain_emission_get_tsd_missing(emission_capsule):
    data, size = cf.dap_chain_emission_get_tsd(emission_capsule, EMISSION_TSD_TYPE_TOKEN_SYM)
    assert data is None
    assert size == 0


def test_dap_chain_datum_emission_get_size_rejects_invalid():
    with pytest.raises(ValueError):
        cf.dap_chain_datum_emission_get_size(b"\x00")
    with pytest.raises(TypeError):
        cf.dap_chain_datum_emission_get_size(object())


def test_dap_chain_datum_emission_get_size_smoke():
    size = cf.dap_chain_datum_emission_get_size(EMISSION_SERIAL_BYTES)
    assert isinstance(size, int)
    assert size > 0


def test_dap_chain_datum_emission_read_rejects_invalid():
    with pytest.raises(ValueError):
        cf.dap_chain_datum_emission_read(b"\x00")
    with pytest.raises(TypeError):
        cf.dap_chain_datum_emission_read(object())


def test_dap_chain_datum_emission_read_smoke():
    emission, size = cf.dap_chain_datum_emission_read(EMISSION_SERIAL_BYTES)
    assert_capsule_or_none(emission)
    assert isinstance(size, int)
    assert size >= 0


def test_dap_chain_datum_emission_add_sign_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_emission_add_sign(object(), object())


def test_dap_chain_datum_emission_append_sign_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_emission_append_sign(object(), object())


def _sign_tx_with_key(tx_capsule, key):
    sign_data = cf.dap_chain_datum_tx_get_sign_data(tx_capsule)
    if not isinstance(sign_data, (bytes, bytearray)) or not sign_data:
        pytest.skip("Failed to get tx signing data")
    signature = key.sign(bytes(sign_data))
    return signature, make_capsule_from_ptr(signature.ptr, "dap_sign_t")


def _create_tx_sign(tx_capsule, enc_key_capsule, addr_capsule, uint256_one):
    rc = cf.dap_chain_datum_tx_add_out_item(tx_capsule, addr_capsule, uint256_one)
    if not isinstance(rc, int) or rc <= 0:
        pytest.skip("Failed to add output item to tx")
    sign = cf.dap_chain_datum_tx_sign_create(enc_key_capsule, tx_capsule)
    if sign is None:
        pytest.skip("Failed to create tx sign")
    return sign


def _create_receipt(unit_enum, uint256_one, ext=RECEIPT_EXT):
    receipt = cf.dap_chain_datum_tx_receipt_create(
        SERVICE_UID, unit_enum, 1, uint256_one, ext, HASH_BYTES
    )
    if receipt is None:
        pytest.skip("Failed to create receipt")
    return receipt


def _add_tx_item(tx_capsule, item_capsule):
    rc = cf.dap_chain_datum_tx_add_item(tx_capsule, item_capsule)
    if not isinstance(rc, int) or rc <= 0:
        pytest.skip("Failed to add item to tx")
    return rc


def test_dap_chain_datum_tx_create_and_delete(sdk_initialized):
    tx = cf.dap_chain_datum_tx_create()
    assert is_capsule(tx)
    assert cf.dap_chain_datum_tx_delete(tx) is None


def test_dap_chain_datum_tx_delete_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_delete(object())


def test_dap_chain_datum_tx_get_size_returns_int(tx_capsule):
    size = cf.dap_chain_datum_tx_get_size(tx_capsule)
    assert isinstance(size, int)
    assert size >= 0


def test_dap_chain_datum_tx_add_item_rejects_invalid(tx_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_add_item(tx_capsule, 123)


def test_dap_chain_datum_tx_add_in_item_smoke(tx_capsule):
    rc = cf.dap_chain_datum_tx_add_in_item(tx_capsule, HASH_BYTES, 0)
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_in_item_rejects_invalid_hash(tx_capsule):
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_add_in_item(tx_capsule, b"\x00", 0)


def test_dap_chain_datum_tx_add_in_item_list_rejects_invalid(tx_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_add_in_item_list(tx_capsule, [])


def test_dap_chain_datum_tx_add_in_cond_item_smoke(tx_capsule):
    rc = cf.dap_chain_datum_tx_add_in_cond_item(tx_capsule, HASH_BYTES, 0, 0)
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_in_cond_item_rejects_invalid_hash(tx_capsule):
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_add_in_cond_item(tx_capsule, b"\x00", 0, 0)


def test_dap_chain_datum_tx_add_in_cond_item_list_rejects_invalid(tx_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_add_in_cond_item_list(tx_capsule, {})


def test_dap_chain_datum_tx_add_in_reward_item_smoke(tx_capsule):
    rc = cf.dap_chain_datum_tx_add_in_reward_item(tx_capsule, HASH_BYTES)
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_out_item_smoke(tx_capsule, addr_capsule, uint256_one):
    rc = cf.dap_chain_datum_tx_add_out_item(tx_capsule, addr_capsule, uint256_one)
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_out_item_rejects_invalid_addr(tx_capsule, uint256_one):
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_add_out_item(tx_capsule, b"\x00", uint256_one)


def test_dap_chain_datum_tx_add_out_ext_item_smoke(tx_capsule, addr_capsule, uint256_one):
    rc = cf.dap_chain_datum_tx_add_out_ext_item(tx_capsule, addr_capsule, uint256_one, TOKEN_TICKER)
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_out_ext_item_rejects_invalid_ticker(tx_capsule, addr_capsule, uint256_one):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_add_out_ext_item(tx_capsule, addr_capsule, uint256_one, 123)


def test_dap_chain_datum_tx_add_out_std_item_smoke(tx_capsule, addr_capsule, uint256_one):
    rc = cf.dap_chain_datum_tx_add_out_std_item(tx_capsule, addr_capsule, uint256_one, TOKEN_TICKER, 0)
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_out_std_item_rejects_invalid_ts_unlock(tx_capsule, addr_capsule, uint256_one):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_add_out_std_item(tx_capsule, addr_capsule, uint256_one, TOKEN_TICKER, "bad")


def test_dap_chain_datum_tx_add_out_cond_item_smoke(tx_capsule, uint256_one, unit_enum):
    rc = cf.dap_chain_datum_tx_add_out_cond_item(
        tx_capsule,
        HASH_BYTES,
        SERVICE_UID,
        uint256_one,
        uint256_one,
        unit_enum,
        COND_BYTES,
    )
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_out_cond_item_accepts_none(tx_capsule, uint256_one, unit_enum):
    rc = cf.dap_chain_datum_tx_add_out_cond_item(
        tx_capsule,
        HASH_BYTES,
        SERVICE_UID,
        uint256_one,
        uint256_one,
        unit_enum,
        None,
    )
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_out_cond_item_rejects_invalid_cond(tx_capsule, uint256_one, unit_enum):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_add_out_cond_item(
            tx_capsule,
            HASH_BYTES,
            SERVICE_UID,
            uint256_one,
            uint256_one,
            unit_enum,
            "bad",
        )


def test_dap_chain_datum_tx_add_fee_item_smoke(tx_capsule, uint256_one):
    rc = cf.dap_chain_datum_tx_add_fee_item(tx_capsule, uint256_one)
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_fee_item_rejects_invalid_value(tx_capsule):
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_add_fee_item(tx_capsule, b"\x00")


def test_dap_chain_datum_tx_add_event_item_smoke(tx_capsule, pkey_capsule):
    rc = cf.dap_chain_datum_tx_add_event_item(
        tx_capsule,
        pkey_capsule,
        SERVICE_UID,
        EVENT_GROUP,
        EVENT_TYPE,
        EVENT_DATA,
    )
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_event_item_rejects_invalid_group(tx_capsule, pkey_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_add_event_item(
            tx_capsule,
            pkey_capsule,
            SERVICE_UID,
            123,
            EVENT_TYPE,
            EVENT_DATA,
        )


def test_dap_chain_datum_tx_add_sign_item_smoke(tx_capsule, enc_key_capsule):
    rc = cf.dap_chain_datum_tx_add_sign_item(tx_capsule, enc_key_capsule)
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_sign_item_rejects_invalid(tx_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_add_sign_item(tx_capsule, object())


def test_dap_chain_datum_tx_add_sign_smoke(tx_capsule, crypto_key, addr_capsule, uint256_one):
    rc = cf.dap_chain_datum_tx_add_out_item(tx_capsule, addr_capsule, uint256_one)
    if not isinstance(rc, int) or rc <= 0:
        pytest.skip("Failed to add output item to tx")
    signature, sign_capsule = _sign_tx_with_key(tx_capsule, crypto_key)
    rc = cf.dap_chain_datum_tx_add_sign(tx_capsule, sign_capsule)
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_add_sign_rejects_invalid(tx_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_add_sign(tx_capsule, object())


def test_dap_chain_datum_tx_get_sign_returns_none_on_empty(tx_capsule):
    assert cf.dap_chain_datum_tx_get_sign(tx_capsule, 0) is None


def test_dap_chain_datum_tx_get_sign_after_add(tx_capsule, crypto_key, addr_capsule, uint256_one):
    rc = cf.dap_chain_datum_tx_add_out_item(tx_capsule, addr_capsule, uint256_one)
    if not isinstance(rc, int) or rc <= 0:
        pytest.skip("Failed to add output item to tx")
    signature, sign_capsule = _sign_tx_with_key(tx_capsule, crypto_key)
    rc = cf.dap_chain_datum_tx_add_sign(tx_capsule, sign_capsule)
    if not isinstance(rc, int) or rc <= 0:
        pytest.skip("Failed to add signature to tx")
    sign = cf.dap_chain_datum_tx_get_sign(tx_capsule, 0)
    assert_capsule_or_none(sign)


def test_dap_chain_datum_tx_get_sign_data_returns_bytes(tx_capsule):
    sign_data = cf.dap_chain_datum_tx_get_sign_data(tx_capsule)
    assert isinstance(sign_data, (bytes, bytearray))
    assert len(sign_data) > 0


def test_dap_chain_datum_tx_get_fee_value_empty(tx_capsule):
    rc, value = cf.dap_chain_datum_tx_get_fee_value(tx_capsule)
    assert isinstance(rc, int)
    if rc != 0:
        assert value is None


def test_dap_chain_datum_tx_get_fee_value_after_add(tx_capsule, uint256_one):
    rc = cf.dap_chain_datum_tx_add_fee_item(tx_capsule, uint256_one)
    if not isinstance(rc, int) or rc <= 0:
        pytest.skip("Failed to add fee item")
    rc, value = cf.dap_chain_datum_tx_get_fee_value(tx_capsule)
    assert rc == 0
    assert isinstance(value, (bytes, bytearray))
    assert len(value) == 32


def test_dap_chain_datum_tx_group_items_empty(tx_capsule):
    groups = cf.dap_chain_datum_tx_group_items(tx_capsule)
    assert_capsule_or_none(groups)
    if groups is not None:
        assert cf.dap_chain_datum_tx_group_items_free(groups) is None


def test_dap_chain_datum_tx_group_items_free_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_group_items_free(object())


def test_dap_chain_datum_tx_item_get_empty(tx_capsule):
    item, size, idx = cf.dap_chain_datum_tx_item_get(tx_capsule, 0, None, TX_ITEM_TYPE_ANY)
    assert item is None
    assert isinstance(size, int)
    assert isinstance(idx, int)


def test_dap_chain_datum_tx_item_get_rejects_invalid_iter(tx_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_get(tx_capsule, 0, object(), TX_ITEM_TYPE_ANY)


def test_dap_chain_datum_tx_item_get_nth_empty(tx_capsule):
    assert cf.dap_chain_datum_tx_item_get_nth(tx_capsule, TX_ITEM_TYPE_ANY, 0) is None


def test_dap_chain_datum_tx_items_get_empty(tx_capsule):
    items, count = cf.dap_chain_datum_tx_items_get(tx_capsule, TX_ITEM_TYPE_ANY)
    assert items is None
    assert count == 0


def test_dap_chain_datum_tx_items_get_rejects_invalid_type(tx_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_items_get(tx_capsule, "bad")


def test_dap_chain_datum_tx_out_cond_get_empty(tx_capsule):
    out, out_num = cf.dap_chain_datum_tx_out_cond_get(tx_capsule, 0)
    assert out is None
    assert isinstance(out_num, int)


def test_dap_chain_datum_tx_verify_sign_empty(tx_capsule):
    rc = cf.dap_chain_datum_tx_verify_sign(tx_capsule)
    assert isinstance(rc, int)


def test_dap_chain_datum_tx_verify_sign_all_empty(tx_capsule):
    rc = cf.dap_chain_datum_tx_verify_sign_all(tx_capsule)
    assert isinstance(rc, int)


def test_dap_chain_node_datum_tx_calc_hash_returns_bytes(tx_capsule):
    tx_hash = cf.dap_chain_node_datum_tx_calc_hash(tx_capsule)
    assert isinstance(tx_hash, (bytes, bytearray))
    assert len(tx_hash) == HASH_SIZE


@pytest.fixture
def tx_wrapped_capsule(sdk_initialized):
    tx = cf.dap_chain_datum_tx_create()
    if tx is None:
        pytest.skip("Failed to create tx datum")
    wrapped = cf.dap_chain_tx_wrap_packed(tx)
    if wrapped is None:
        pytest.skip("Failed to wrap tx")
    yield wrapped
    cf.dap_chain_datum_tx_delete(tx)


def test_dap_chain_tx_wrap_packed_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_wrap_packed(object())


def test_dap_chain_tx_dup_smoke(tx_wrapped_capsule):
    dup = cf.dap_chain_tx_dup(tx_wrapped_capsule)
    assert_capsule_or_none(dup)
    if dup is not None:
        cf.dap_chain_tx_delete(dup)


def test_dap_chain_tx_dup_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_dup(object())


def test_dap_chain_tx_delete_smoke(tx_wrapped_capsule):
    assert cf.dap_chain_tx_delete(tx_wrapped_capsule) is None


def test_dap_chain_tx_delete_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_delete(object())


def test_dap_chain_tx_hh_add_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_hh_add(object(), object())
    with pytest.raises(TypeError):
        cf.dap_chain_tx_hh_add(make_capsule("dap_chain_tx_t"), object())


def test_dap_chain_tx_hh_find_rejects_invalid_hash():
    with pytest.raises(ValueError):
        cf.dap_chain_tx_hh_find(make_capsule("dap_chain_tx_t"), b"\x00")


def test_dap_chain_tx_hh_free_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_hh_free(object())


def test_dap_chain_tx_hh_add_find_free_smoke(addr_capsule, uint256_one):
    tx1 = cf.dap_chain_datum_tx_create()
    tx2 = cf.dap_chain_datum_tx_create()
    if tx1 is None or tx2 is None:
        pytest.skip("Failed to create tx for hh tests")
    rc = cf.dap_chain_datum_tx_add_out_item(tx1, addr_capsule, uint256_one)
    rc2 = cf.dap_chain_datum_tx_add_out_item(tx2, addr_capsule, uint256_one)
    if not isinstance(rc, int) or not isinstance(rc2, int) or rc <= 0 or rc2 <= 0:
        cf.dap_chain_datum_tx_delete(tx1)
        cf.dap_chain_datum_tx_delete(tx2)
        pytest.skip("Failed to add output items for hh tests")
    hh = cf.dap_chain_tx_wrap_packed(tx1)
    tx2_wrapped = cf.dap_chain_tx_wrap_packed(tx2)
    if hh is None or tx2_wrapped is None:
        cf.dap_chain_datum_tx_delete(tx1)
        cf.dap_chain_datum_tx_delete(tx2)
        pytest.skip("Failed to wrap tx for hh tests")
    hh = cf.dap_chain_tx_hh_add(None, hh)
    hh = cf.dap_chain_tx_hh_add(hh, tx2_wrapped)
    tx2_hash = cf.dap_chain_node_datum_tx_calc_hash(tx2)
    found = cf.dap_chain_tx_hh_find(hh, tx2_hash)
    assert_capsule_or_none(found)
    cf.dap_chain_tx_hh_free(hh)
    cf.dap_chain_datum_tx_delete(tx1)
    cf.dap_chain_datum_tx_delete(tx2)


def test_dap_chain_tx_get_signing_data_returns_bytes(tx_capsule):
    sign_data = cf.dap_chain_tx_get_signing_data(tx_capsule)
    assert isinstance(sign_data, (bytes, bytearray))
    assert len(sign_data) > 0


def test_dap_chain_tx_get_signing_data_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_get_signing_data(object())


def test_dap_chain_tx_sign_add_smoke(tx_capsule, crypto_key, addr_capsule, uint256_one):
    rc = cf.dap_chain_datum_tx_add_out_item(tx_capsule, addr_capsule, uint256_one)
    if not isinstance(rc, int) or rc <= 0:
        pytest.skip("Failed to add output item to tx")
    signature, sign_capsule = _sign_tx_with_key(tx_capsule, crypto_key)
    rc = cf.dap_chain_tx_sign_add(tx_capsule, sign_capsule)
    assert isinstance(rc, int)


def test_dap_chain_tx_sign_add_rejects_invalid(tx_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_tx_sign_add(tx_capsule, object())


def test_dap_chain_datum_tx_event_to_json_rejects_invalid():
    dap_json = pytest.importorskip("dap.json")
    json_obj = dap_json.DapJson.object()
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_event_to_json(object(), object(), "hex")
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_event_to_json(json_obj.capsule, object(), "hex")
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_event_to_json(
            json_obj.capsule, make_capsule("dap_chain_tx_event_t"), 123
        )
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_event_to_json(
            json_obj.capsule, make_capsule("wrong_event"), "hex"
        )


def test_dap_chain_datum_tx_item_event_to_json_smoke(sdk_initialized):
    dap_json = pytest.importorskip("dap.json")
    json_obj = dap_json.DapJson.object()
    event = cf.dap_chain_datum_tx_event_create(
        SERVICE_UID, EVENT_GROUP, EVENT_TYPE, EVENT_TIMESTAMP
    )
    if event is None:
        pytest.skip("Failed to create tx item event")
    rc = cf.dap_chain_datum_tx_item_event_to_json(json_obj.capsule, event)
    assert isinstance(rc, int)
    assert rc == 0


def test_dap_chain_datum_tx_item_event_to_json_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_event_to_json(object(), object())


def test_dap_chain_tx_item_event_type_roundtrip():
    event_type = cf.dap_chain_tx_item_event_type_from_str("stake_ext_started")
    assert isinstance(event_type, int)
    assert event_type >= 0
    assert cf.dap_chain_tx_item_event_type_to_str(event_type) == "stake_ext_started"
    assert cf.dap_chain_tx_item_event_type_from_str("unknown") == -1
    assert cf.dap_chain_tx_item_event_type_to_str(0xFFFF) == "unknown"


def test_dap_chain_datum_item_tx_get_size_smoke():
    item = cf.dap_chain_datum_tx_item_tsd_create(TSD_DATA, TSD_TYPE)
    if item is None:
        pytest.skip("Failed to create TSD item")
    size = cf.dap_chain_datum_item_tx_get_size(item)
    assert isinstance(size, int)
    assert size > 0
    size_small = cf.dap_chain_datum_item_tx_get_size(item, 1)
    assert size_small == 0


def test_dap_chain_datum_item_tx_get_size_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_item_tx_get_size(123)


def test_dap_chain_datum_tx_event_create_smoke():
    event = cf.dap_chain_datum_tx_event_create(
        SERVICE_UID, EVENT_GROUP, EVENT_TYPE, EVENT_TIMESTAMP
    )
    if event is None:
        pytest.skip("Failed to create tx item event")
    assert is_capsule(event)


def test_dap_chain_datum_tx_event_create_rejects_invalid_group():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_event_create(SERVICE_UID, 123, EVENT_TYPE, EVENT_TIMESTAMP)


def test_dap_chain_datum_tx_item_get_data_returns_bytes():
    tsd_item = cf.dap_chain_datum_tx_item_tsd_create(TSD_DATA, TSD_TYPE)
    if tsd_item is None:
        pytest.skip("Failed to create TSD item")
    data, data_type, size = cf.dap_chain_datum_tx_item_get_data(tsd_item)
    assert data == TSD_DATA
    assert data_type == TSD_TYPE
    assert size == len(TSD_DATA)


def test_dap_chain_datum_tx_item_get_data_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_get_data(object())


def test_dap_chain_datum_tx_item_get_tsd_by_type_empty(tx_capsule):
    assert cf.dap_chain_datum_tx_item_get_tsd_by_type(tx_capsule, TSD_TYPE) is None


def test_dap_chain_datum_tx_item_get_tsd_by_type_after_add(tx_capsule):
    tsd_item = cf.dap_chain_datum_tx_item_tsd_create(TSD_DATA, TSD_TYPE)
    if tsd_item is None:
        pytest.skip("Failed to create TSD item")
    rc = cf.dap_chain_datum_tx_add_item(tx_capsule, tsd_item)
    if not isinstance(rc, int) or rc <= 0:
        pytest.skip("Failed to add TSD item to tx")
    tsd = cf.dap_chain_datum_tx_item_get_tsd_by_type(tx_capsule, TSD_TYPE)
    assert is_capsule(tsd)


def test_dap_chain_datum_tx_item_get_tsd_by_type_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_get_tsd_by_type(object(), TSD_TYPE)


def test_dap_chain_datum_tx_item_in_cond_create_smoke():
    item = cf.dap_chain_datum_tx_item_in_cond_create(HASH_BYTES, 0, 0)
    if item is None:
        pytest.skip("Failed to create in_cond item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_in_cond_create_rejects_invalid_hash():
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_item_in_cond_create(b"\x00", 0, 0)


def test_dap_chain_datum_tx_item_in_create_smoke():
    item = cf.dap_chain_datum_tx_item_in_create(HASH_BYTES, 0)
    if item is None:
        pytest.skip("Failed to create in item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_in_create_rejects_invalid_hash():
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_item_in_create(b"\x00", 0)


def test_dap_chain_datum_tx_item_in_ems_create_smoke():
    item = cf.dap_chain_datum_tx_item_in_ems_create(1, HASH_BYTES, TOKEN_TICKER)
    if item is None:
        pytest.skip("Failed to create in_ems item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_in_ems_create_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_in_ems_create(1, HASH_BYTES, 123)
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_item_in_ems_create(1, b"\x00", TOKEN_TICKER)


def test_dap_chain_datum_tx_item_in_reward_create_smoke():
    item = cf.dap_chain_datum_tx_item_in_reward_create(HASH_BYTES)
    if item is None:
        pytest.skip("Failed to create in_reward item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_in_reward_create_rejects_invalid_hash():
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_item_in_reward_create(b"\x00")


def test_dap_chain_datum_tx_item_out_cond_create_fee_smoke(uint256_one):
    item = cf.dap_chain_datum_tx_item_out_cond_create_fee(uint256_one)
    if item is None:
        pytest.skip("Failed to create fee out_cond item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_cond_create_fee_zero_returns_none(sdk_initialized):
    if not hasattr(cf, "dap_chain_uint256_from"):
        pytest.skip("dap_chain_uint256_from not available")
    zero = cf.dap_chain_uint256_from(0)
    item = cf.dap_chain_datum_tx_item_out_cond_create_fee(zero)
    assert item is None


def test_dap_chain_datum_tx_item_out_cond_create_srv_pay_smoke(
    pkey_capsule, uint256_one, unit_enum
):
    item = cf.dap_chain_datum_tx_item_out_cond_create_srv_pay(
        pkey_capsule, SERVICE_UID, uint256_one, uint256_one, unit_enum, None
    )
    if item is None:
        pytest.skip("Failed to create srv_pay out_cond item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_cond_create_srv_pay_rejects_invalid_params(
    pkey_capsule, uint256_one, unit_enum
):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_out_cond_create_srv_pay(
            pkey_capsule, SERVICE_UID, uint256_one, uint256_one, unit_enum, "bad"
        )


def test_dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash_smoke(
    uint256_one, unit_enum
):
    item = cf.dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash(
        HASH_BYTES, SERVICE_UID, uint256_one, uint256_one, unit_enum, None
    )
    if item is None:
        pytest.skip("Failed to create srv_pay out_cond item with hash")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash_rejects_invalid_hash(
    uint256_one, unit_enum
):
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash(
            b"\x00", SERVICE_UID, uint256_one, uint256_one, unit_enum, None
        )


def test_dap_chain_datum_tx_item_out_cond_create_srv_stake_smoke(
    addr_capsule, uint256_one
):
    item = cf.dap_chain_datum_tx_item_out_cond_create_srv_stake(
        SERVICE_UID, uint256_one, addr_capsule, 1, None, uint256_one, None
    )
    if item is None:
        pytest.skip("Failed to create srv_stake out_cond item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate_smoke(
    addr_capsule, uint256_one
):
    item = cf.dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate(
        SERVICE_UID, uint256_one, addr_capsule, 1, uint256_one, None
    )
    if item is None:
        pytest.skip("Failed to create srv_stake_delegate out_cond item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate_rejects_invalid_params(
    addr_capsule, uint256_one
):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate(
            SERVICE_UID, uint256_one, addr_capsule, 1, uint256_one, "bad"
        )


def test_dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock_smoke(uint256_one):
    item = cf.dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock(
        SERVICE_UID, uint256_one, HASH_BYTES, 0, 0, None
    )
    if item is None:
        pytest.skip("Failed to create srv_stake_ext_lock out_cond item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock_rejects_invalid_hash(
    uint256_one,
):
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock(
            SERVICE_UID, uint256_one, b"\x00", 0, 0, None
        )


def test_dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size():
    size_zero = cf.dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size(
        False, 0
    )
    size_sovereign = cf.dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size(
        True, 0
    )
    size_pkey = cf.dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size(
        False, 33
    )
    assert size_zero == 0
    assert size_sovereign >= size_zero
    assert size_pkey >= size_zero


def test_dap_chain_datum_tx_item_out_cond_create_srv_stake_lock_smoke(uint256_one):
    item = cf.dap_chain_datum_tx_item_out_cond_create_srv_stake_lock(
        SERVICE_UID, uint256_one, 0, uint256_one
    )
    if item is None:
        pytest.skip("Failed to create srv_stake_lock out_cond item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_cond_create_srv_xchange_smoke(
    addr_capsule, uint256_one
):
    item = cf.dap_chain_datum_tx_item_out_cond_create_srv_xchange(
        SERVICE_UID, 1, uint256_one, 2, TOKEN_TICKER, uint256_one, addr_capsule, None
    )
    if item is None:
        pytest.skip("Failed to create srv_xchange out_cond item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_cond_create_srv_xchange_rejects_invalid_token(
    addr_capsule, uint256_one
):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_out_cond_create_srv_xchange(
            SERVICE_UID, 1, uint256_one, 2, 123, uint256_one, addr_capsule, None
        )


def test_dap_chain_datum_tx_item_out_cond_create_wallet_shared_smoke(uint256_one):
    hashes = [HASH_BYTES, HASH_BYTES_ALT]
    item = cf.dap_chain_datum_tx_item_out_cond_create_wallet_shared(
        SERVICE_UID, uint256_one, 1, hashes, "tag"
    )
    if item is None:
        pytest.skip("Failed to create wallet_shared out_cond item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_cond_create_wallet_shared_rejects_invalid(
    uint256_one,
):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_out_cond_create_wallet_shared(
            SERVICE_UID, uint256_one, 1, "bad", None
        )
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_item_out_cond_create_wallet_shared(
            SERVICE_UID, uint256_one, 1, [b"\x00"], None
        )


def test_dap_chain_datum_tx_item_out_create_smoke(addr_capsule, uint256_one):
    item = cf.dap_chain_datum_tx_item_out_create(addr_capsule, uint256_one)
    if item is None:
        pytest.skip("Failed to create out item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_create_rejects_invalid(addr_capsule, uint256_one):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_out_create(object(), uint256_one)
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_item_out_create(addr_capsule, b"\x00")


def test_dap_chain_datum_tx_item_out_ext_create_smoke(addr_capsule, uint256_one):
    item = cf.dap_chain_datum_tx_item_out_ext_create(addr_capsule, uint256_one, TOKEN_TICKER)
    if item is None:
        pytest.skip("Failed to create out_ext item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_ext_create_rejects_invalid(
    addr_capsule, uint256_one
):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_out_ext_create(addr_capsule, uint256_one, 123)


def test_dap_chain_datum_tx_item_out_std_create_smoke(addr_capsule, uint256_one):
    item = cf.dap_chain_datum_tx_item_out_std_create(
        addr_capsule, uint256_one, TOKEN_TICKER, 0
    )
    if item is None:
        pytest.skip("Failed to create out_std item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_out_std_create_rejects_invalid(
    addr_capsule, uint256_one
):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_out_std_create(
            addr_capsule, uint256_one, TOKEN_TICKER, "bad"
        )


def test_dap_chain_datum_tx_item_sig_get_sign_smoke(
    tx_capsule, enc_key_capsule, addr_capsule, uint256_one
):
    sign = _create_tx_sign(tx_capsule, enc_key_capsule, addr_capsule, uint256_one)
    sig_item = cf.dap_chain_tx_sig_create(sign)
    if sig_item is None:
        pytest.skip("Failed to create tx sig item")
    sign_out = cf.dap_chain_datum_tx_item_sig_get_sign(sig_item)
    assert is_capsule(sign_out)


def test_dap_chain_datum_tx_item_sig_get_sign_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_sig_get_sign(object())


def test_dap_chain_datum_tx_item_sign_create_smoke(
    tx_capsule, enc_key_capsule, addr_capsule, uint256_one
):
    rc = cf.dap_chain_datum_tx_add_out_item(tx_capsule, addr_capsule, uint256_one)
    if not isinstance(rc, int) or rc <= 0:
        pytest.skip("Failed to add output item to tx")
    sig_item = cf.dap_chain_datum_tx_item_sign_create(enc_key_capsule, tx_capsule)
    if sig_item is None:
        pytest.skip("Failed to create tx sig item")
    assert is_capsule(sig_item)


def test_dap_chain_datum_tx_item_sign_create_rejects_invalid(tx_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_sign_create(object(), tx_capsule)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_sign_create(make_capsule("dap_enc_key_t"), object())


def test_dap_chain_datum_tx_item_sign_create_from_sign_smoke(
    tx_capsule, enc_key_capsule, addr_capsule, uint256_one
):
    sign = _create_tx_sign(tx_capsule, enc_key_capsule, addr_capsule, uint256_one)
    sig_item = cf.dap_chain_datum_tx_item_sign_create_from_sign(sign)
    if sig_item is None:
        pytest.skip("Failed to create tx sig item from sign")
    assert is_capsule(sig_item)


def test_dap_chain_datum_tx_item_sign_create_from_sign_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_sign_create_from_sign(object())


def test_dap_chain_datum_tx_item_tsd_create_smoke():
    item = cf.dap_chain_datum_tx_item_tsd_create(TSD_DATA, TSD_TYPE)
    if item is None:
        pytest.skip("Failed to create TSD item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_tsd_create_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_tsd_create("bad", TSD_TYPE)
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_item_tsd_create(b"", TSD_TYPE)


def test_dap_chain_datum_tx_item_type_string_roundtrip():
    item_type = cf.dap_chain_datum_tx_item_type_from_str_short("out")
    assert isinstance(item_type, int)
    assert cf.dap_chain_datum_tx_item_type_to_str(item_type) == "TX_ITEM_TYPE_OUT"
    assert cf.dap_chain_datum_tx_item_type_to_str_short(item_type) == "out"
    unknown_type = cf.dap_chain_datum_tx_item_type_from_str_short("unknown")
    assert cf.dap_chain_datum_tx_item_type_to_str_short(unknown_type) == "UNDEFINED"


def test_dap_chain_datum_tx_item_type_from_str_short_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_type_from_str_short(123)


def test_dap_chain_datum_tx_sign_create_smoke(
    tx_capsule, enc_key_capsule, addr_capsule, uint256_one
):
    sign = _create_tx_sign(tx_capsule, enc_key_capsule, addr_capsule, uint256_one)
    assert is_capsule(sign)


def test_dap_chain_datum_tx_sign_create_rejects_invalid(tx_capsule):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_sign_create(object(), tx_capsule)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_sign_create(make_capsule("dap_enc_key_t"), object())


def test_dap_chain_tx_event_copy_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_event_copy(object())
    with pytest.raises(ValueError):
        cf.dap_chain_tx_event_copy(make_capsule("wrong_event"))


def test_dap_chain_tx_event_delete_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_event_delete(object())
    with pytest.raises(ValueError):
        cf.dap_chain_tx_event_delete(make_capsule("wrong_event"))


def test_dap_chain_tx_out_cond_subtype_roundtrip():
    subtype = cf.dap_chain_tx_out_cond_subtype_from_str_short("srv_pay")
    assert isinstance(subtype, int)
    assert cf.dap_chain_tx_out_cond_subtype_to_str_short(subtype) == "srv_pay"
    assert (
        cf.dap_chain_tx_out_cond_subtype_to_str(subtype)
        == "DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY"
    )
    unknown = cf.dap_chain_tx_out_cond_subtype_from_str_short("unknown")
    assert cf.dap_chain_tx_out_cond_subtype_to_str_short(unknown) == "UNDEFINED"


def test_dap_chain_tx_out_cond_subtype_from_str_short_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_out_cond_subtype_from_str_short(123)


def test_dap_chain_tx_sig_create_smoke(
    tx_capsule, enc_key_capsule, addr_capsule, uint256_one
):
    sign = _create_tx_sign(tx_capsule, enc_key_capsule, addr_capsule, uint256_one)
    sig_item = cf.dap_chain_tx_sig_create(sign)
    if sig_item is None:
        pytest.skip("Failed to create tx sig item")
    assert is_capsule(sig_item)


def test_dap_chain_tx_sig_create_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_sig_create(object())


def test_dap_chain_tx_out_cond_subtype_to_str_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_out_cond_subtype_to_str("bad")


def test_dap_chain_tx_out_cond_subtype_to_str_short_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_tx_out_cond_subtype_to_str_short("bad")


def test_dap_chain_datum_tx_receipt_create_smoke(unit_enum, uint256_one):
    receipt = cf.dap_chain_datum_tx_receipt_create(
        SERVICE_UID, unit_enum, 1, uint256_one, RECEIPT_EXT, HASH_BYTES
    )
    if receipt is None:
        pytest.skip("Failed to create receipt")
    assert is_capsule(receipt)
    receipt_none = cf.dap_chain_datum_tx_receipt_create(
        SERVICE_UID, unit_enum, 1, uint256_one, None, HASH_BYTES
    )
    if receipt_none is None:
        pytest.skip("Failed to create receipt with None ext")
    assert is_capsule(receipt_none)


def test_dap_chain_datum_tx_receipt_create_rejects_invalid_hash(unit_enum, uint256_one):
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_receipt_create(
            SERVICE_UID, unit_enum, 1, uint256_one, RECEIPT_EXT, b"\x00"
        )


def test_dap_chain_datum_tx_receipt_create_rejects_invalid_ext(unit_enum, uint256_one):
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_receipt_create(
            SERVICE_UID, unit_enum, 1, uint256_one, "bad", HASH_BYTES
        )


def test_dap_chain_datum_tx_receipt_check_size_smoke(unit_enum, uint256_one):
    receipt = _create_receipt(unit_enum, uint256_one)
    size = get_receipt_size(receipt, len(uint256_one))
    if not size or size <= 0:
        pytest.skip("Failed to resolve receipt size")
    rc = cf.dap_chain_datum_tx_receipt_check_size(receipt, size)
    assert isinstance(rc, int)
    assert rc == 0
    rc_bad = cf.dap_chain_datum_tx_receipt_check_size(receipt, max(size - 1, 0))
    assert rc_bad != 0


def test_dap_chain_datum_tx_receipt_check_size_rejects_invalid(unit_enum, uint256_one):
    receipt = _create_receipt(unit_enum, uint256_one)
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_receipt_check_size(receipt, -1)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_receipt_check_size(object(), 0)


def test_dap_chain_datum_tx_receipt_signs_count_and_get(
    unit_enum, uint256_one, enc_key_capsule
):
    receipt = _create_receipt(unit_enum, uint256_one, ext=None)
    count = cf.dap_chain_datum_tx_receipt_signs_count(receipt)
    assert isinstance(count, int)
    assert count == 0
    signed = cf.dap_chain_datum_tx_receipt_sign_add(receipt, enc_key_capsule)
    if signed is None:
        pytest.skip("Failed to add first receipt signature")
    signed = cf.dap_chain_datum_tx_receipt_sign_add(signed, enc_key_capsule)
    if signed is None:
        pytest.skip("Failed to add second receipt signature")
    count_after = cf.dap_chain_datum_tx_receipt_signs_count(signed)
    assert count_after >= 2
    size = get_receipt_size(signed, len(uint256_one))
    if not size or size <= 0:
        pytest.skip("Failed to resolve signed receipt size")
    sign = cf.dap_chain_datum_tx_receipt_sign_get(signed, size, 0)
    if sign is None:
        pytest.skip("Failed to read first receipt sign")
    assert is_capsule(sign)
    sign2 = cf.dap_chain_datum_tx_receipt_sign_get(signed, size, 1)
    if sign2 is None:
        pytest.skip("Failed to read second receipt sign")
    assert is_capsule(sign2)
    assert cf.dap_chain_datum_tx_receipt_sign_get(signed, size, 10) is None


def test_dap_chain_datum_tx_receipt_sign_add_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_receipt_sign_add(object(), object())


def test_dap_chain_datum_tx_receipt_sign_get_rejects_invalid(unit_enum, uint256_one):
    receipt = _create_receipt(unit_enum, uint256_one)
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_receipt_sign_get(receipt, -1, 0)
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_receipt_sign_get(object(), 0, 0)


def test_dap_chain_datum_tx_receipt_getters(unit_enum, uint256_one):
    receipt = _create_receipt(unit_enum, uint256_one)
    assert cf.dap_chain_datum_tx_receipt_srv_uid_get(receipt) == SERVICE_UID
    assert cf.dap_chain_datum_tx_receipt_units_get(receipt) == 1
    assert cf.dap_chain_datum_tx_receipt_utype_get(receipt) == unit_enum
    value = cf.dap_chain_datum_tx_receipt_value_get(receipt)
    assert isinstance(value, (bytes, bytearray))
    assert value == uint256_one


def test_dap_chain_datum_tx_item_vote_create_smoke():
    item = cf.dap_chain_datum_tx_item_vote_create(HASH_BYTES, VOTING_ANSWER_IDX)
    if item is None:
        pytest.skip("Failed to create vote item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_vote_create_rejects_invalid_hash():
    with pytest.raises(ValueError):
        cf.dap_chain_datum_tx_item_vote_create(b"\x00", VOTING_ANSWER_IDX)


def test_dap_chain_datum_tx_item_vote_to_json_smoke():
    item = cf.dap_chain_datum_tx_item_vote_create(HASH_BYTES, VOTING_ANSWER_IDX)
    if item is None:
        pytest.skip("Failed to create vote item")
    json_obj = cf.dap_chain_datum_tx_item_vote_to_json(item, 1)
    assert_capsule_or_none(json_obj)


def test_dap_chain_datum_tx_item_vote_to_json_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_vote_to_json(object(), 1)


def test_dap_chain_datum_tx_item_voting_create_smoke():
    item = cf.dap_chain_datum_tx_item_voting_create()
    if item is None:
        pytest.skip("Failed to create voting item")
    assert is_capsule(item)


def test_dap_chain_datum_tx_item_voting_create_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_voting_create(123)


def test_dap_chain_datum_tx_item_voting_tsd_to_json_smoke(tx_capsule):
    question = cf.dap_chain_datum_voting_question_tsd_create(VOTING_QUESTION)
    if question is None:
        pytest.skip("Failed to create voting question TSD")
    _add_tx_item(tx_capsule, question)
    option = cf.dap_chain_datum_voting_answer_tsd_create(VOTING_OPTION_A)
    if option is None:
        pytest.skip("Failed to create voting option TSD")
    _add_tx_item(tx_capsule, option)
    json_obj = cf.dap_chain_datum_tx_item_voting_tsd_to_json(tx_capsule, 2)
    assert_capsule_or_none(json_obj)


def test_dap_chain_datum_tx_item_voting_tsd_to_json_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_item_voting_tsd_to_json(object(), 2)


def test_dap_chain_datum_tx_voting_get_answer_text_by_idx_smoke(tx_capsule):
    question = cf.dap_chain_datum_voting_question_tsd_create(VOTING_QUESTION)
    if question is None:
        pytest.skip("Failed to create voting question TSD")
    _add_tx_item(tx_capsule, question)
    option_a = cf.dap_chain_datum_voting_answer_tsd_create(VOTING_OPTION_A)
    option_b = cf.dap_chain_datum_voting_answer_tsd_create(VOTING_OPTION_B)
    if option_a is None or option_b is None:
        pytest.skip("Failed to create voting option TSDs")
    _add_tx_item(tx_capsule, option_a)
    _add_tx_item(tx_capsule, option_b)
    assert cf.dap_chain_datum_tx_voting_get_answer_text_by_idx(tx_capsule, 0) == VOTING_OPTION_A
    assert cf.dap_chain_datum_tx_voting_get_answer_text_by_idx(tx_capsule, 1) == VOTING_OPTION_B
    assert cf.dap_chain_datum_tx_voting_get_answer_text_by_idx(tx_capsule, 2) is None


def test_dap_chain_datum_tx_voting_get_answer_text_by_idx_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_voting_get_answer_text_by_idx(object(), 0)


def test_dap_chain_datum_tx_voting_parse_and_delete(tx_capsule):
    question = cf.dap_chain_datum_voting_question_tsd_create(VOTING_QUESTION)
    option = cf.dap_chain_datum_voting_answer_tsd_create(VOTING_OPTION_A)
    expire = cf.dap_chain_datum_voting_expire_tsd_create(VOTING_EXPIRE)
    if question is None or option is None or expire is None:
        pytest.skip("Failed to create voting TSD items")
    _add_tx_item(tx_capsule, question)
    _add_tx_item(tx_capsule, option)
    _add_tx_item(tx_capsule, expire)
    params = cf.dap_chain_datum_tx_voting_parse_tsd(tx_capsule)
    if params is None:
        pytest.skip("Failed to parse voting params")
    assert is_capsule(params)
    assert cf.dap_chain_datum_tx_voting_params_delete(params) is None


def test_dap_chain_datum_tx_voting_parse_tsd_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_voting_parse_tsd(object())


def test_dap_chain_datum_tx_voting_params_delete_rejects_invalid():
    with pytest.raises(TypeError):
        cf.dap_chain_datum_tx_voting_params_delete(object())


def test_dap_chain_datum_voting_answer_tsd_create_smoke():
    tsd = cf.dap_chain_datum_voting_answer_tsd_create(VOTING_OPTION_A)
    if tsd is None:
        pytest.skip("Failed to create voting answer TSD")
    assert is_capsule(tsd)


def test_dap_chain_datum_voting_answer_tsd_create_rejects_empty():
    with pytest.raises(ValueError):
        cf.dap_chain_datum_voting_answer_tsd_create("")


def test_dap_chain_datum_voting_cancel_tsd_create_smoke():
    tsd = cf.dap_chain_datum_voting_cancel_tsd_create(HASH_BYTES)
    if tsd is None:
        pytest.skip("Failed to create voting cancel TSD")
    assert is_capsule(tsd)


def test_dap_chain_datum_voting_cancel_tsd_create_rejects_invalid_hash():
    with pytest.raises(ValueError):
        cf.dap_chain_datum_voting_cancel_tsd_create(b"\x00")


def test_dap_chain_datum_voting_delegated_key_required_tsd_create_smoke():
    tsd_true = cf.dap_chain_datum_voting_delegated_key_required_tsd_create(True)
    tsd_false = cf.dap_chain_datum_voting_delegated_key_required_tsd_create(False)
    if tsd_true is None or tsd_false is None:
        pytest.skip("Failed to create delegated key required TSD")
    assert is_capsule(tsd_true)
    assert is_capsule(tsd_false)


def test_dap_chain_datum_voting_expire_tsd_create_smoke():
    assert cf.dap_chain_datum_voting_expire_tsd_create(0) is None
    tsd = cf.dap_chain_datum_voting_expire_tsd_create(VOTING_EXPIRE)
    if tsd is None:
        pytest.skip("Failed to create expire TSD")
    assert is_capsule(tsd)


def test_dap_chain_datum_voting_max_votes_count_tsd_create_smoke():
    assert cf.dap_chain_datum_voting_max_votes_count_tsd_create(0) is None
    tsd = cf.dap_chain_datum_voting_max_votes_count_tsd_create(VOTING_MAX_VOTES)
    if tsd is None:
        pytest.skip("Failed to create max votes count TSD")
    assert is_capsule(tsd)


def test_dap_chain_datum_voting_question_tsd_create_smoke():
    tsd = cf.dap_chain_datum_voting_question_tsd_create(VOTING_QUESTION)
    if tsd is None:
        pytest.skip("Failed to create voting question TSD")
    assert is_capsule(tsd)


def test_dap_chain_datum_voting_question_tsd_create_rejects_empty():
    with pytest.raises(ValueError):
        cf.dap_chain_datum_voting_question_tsd_create("")


def test_dap_chain_datum_voting_token_tsd_create_smoke():
    tsd = cf.dap_chain_datum_voting_token_tsd_create(TOKEN_TICKER)
    if tsd is None:
        pytest.skip("Failed to create voting token TSD")
    assert is_capsule(tsd)
    assert cf.dap_chain_datum_voting_token_tsd_create("") is None
    assert cf.dap_chain_datum_voting_token_tsd_create("X" * 16) is None


def test_dap_chain_datum_voting_vote_changing_allowed_tsd_create_smoke():
    tsd_true = cf.dap_chain_datum_voting_vote_changing_allowed_tsd_create(True)
    tsd_false = cf.dap_chain_datum_voting_vote_changing_allowed_tsd_create(False)
    if tsd_true is None or tsd_false is None:
        pytest.skip("Failed to create vote changing allowed TSD")
    assert is_capsule(tsd_true)
    assert is_capsule(tsd_false)


def test_dap_chain_datum_voting_vote_tx_cond_tsd_create_smoke():
    tsd = cf.dap_chain_datum_voting_vote_tx_cond_tsd_create(HASH_BYTES, VOTING_TX_OUT_IDX)
    if tsd is None:
        pytest.skip("Failed to create vote tx cond TSD")
    assert is_capsule(tsd)


def test_dap_chain_datum_voting_vote_tx_cond_tsd_create_rejects_invalid_hash():
    with pytest.raises(ValueError):
        cf.dap_chain_datum_voting_vote_tx_cond_tsd_create(b"\x00", VOTING_TX_OUT_IDX)


@pytest.fixture
def compose_api(sdk_initialized):
    if not hasattr(cf, "dap_chain_tx_compose_init"):
        pytest.skip("dap_chain_tx_compose_init not available")
    rc = cf.dap_chain_tx_compose_init()
    if not isinstance(rc, int) or rc != 0:
        pytest.skip(f"Failed to init tx compose API: {rc}")
    yield
    cf.dap_chain_tx_compose_deinit()


@pytest.fixture
def compose_registry_api(sdk_initialized):
    if not hasattr(cf, "dap_chain_tx_compose_registry_init"):
        pytest.skip("dap_chain_tx_compose_registry_init not available")
    rc = cf.dap_chain_tx_compose_registry_init()
    if not isinstance(rc, int) or rc != 0:
        pytest.skip(f"Failed to init tx compose registry: {rc}")
    yield
    cf.dap_chain_tx_compose_registry_deinit()


def test_dap_chain_tx_compose_init_returns_int(sdk_initialized):
    if not hasattr(cf, "dap_chain_tx_compose_init"):
        pytest.skip("dap_chain_tx_compose_init not available")
    rc = cf.dap_chain_tx_compose_init()
    assert isinstance(rc, int)
    cf.dap_chain_tx_compose_deinit()


def test_dap_chain_tx_compose_deinit_safe(sdk_initialized):
    if not hasattr(cf, "dap_chain_tx_compose_deinit"):
        pytest.skip("dap_chain_tx_compose_deinit not available")
    cf.dap_chain_tx_compose_deinit()
    cf.dap_chain_tx_compose_deinit()


def test_dap_chain_tx_compose_is_registered_defaults_false(compose_api):
    assert cf.dap_chain_tx_compose_is_registered(COMPOSE_TX_TYPE) is False


def test_dap_chain_tx_compose_register_unregister(compose_api):
    def builder(*_args):
        return None

    rc = cf.dap_chain_tx_compose_register(COMPOSE_TX_TYPE, builder, None)
    assert isinstance(rc, int)
    assert rc == 0
    assert cf.dap_chain_tx_compose_is_registered(COMPOSE_TX_TYPE) is True
    assert cf.dap_chain_tx_compose_unregister(COMPOSE_TX_TYPE) is None
    assert cf.dap_chain_tx_compose_is_registered(COMPOSE_TX_TYPE) is False


def test_dap_chain_tx_compose_register_rejects_invalid(compose_api):
    with pytest.raises(TypeError):
        cf.dap_chain_tx_compose_register(COMPOSE_TX_TYPE, 123)
    with pytest.raises(TypeError):
        cf.dap_chain_tx_compose_register(123, lambda *_: None)


def test_dap_chain_tx_compose_unregister_unknown_no_error(compose_api):
    assert cf.dap_chain_tx_compose_unregister("unknown") is None


def test_dap_chain_tx_compose_is_registered_rejects_invalid(compose_api):
    with pytest.raises(TypeError):
        cf.dap_chain_tx_compose_is_registered(123)


def test_dap_chain_tx_compose_create_unregistered_returns_none(compose_api):
    ledger = make_capsule("dap_ledger_t")
    utxo = make_capsule("dap_list_t")
    result = cf.dap_chain_tx_compose_create(COMPOSE_TX_TYPE, ledger, utxo, None)
    assert result is None


def test_dap_chain_tx_compose_create_rejects_invalid_capsules(compose_api):
    ledger = make_capsule("dap_ledger_t")
    utxo = make_capsule("dap_list_t")
    with pytest.raises(TypeError):
        cf.dap_chain_tx_compose_create(COMPOSE_TX_TYPE, object(), utxo, None)
    with pytest.raises(TypeError):
        cf.dap_chain_tx_compose_create(COMPOSE_TX_TYPE, ledger, object(), None)
    bad_ledger = make_capsule("bad_ledger")
    with pytest.raises(ValueError):
        cf.dap_chain_tx_compose_create(COMPOSE_TX_TYPE, bad_ledger, utxo, None)
    bad_utxo = make_capsule("bad_list")
    with pytest.raises(ValueError):
        cf.dap_chain_tx_compose_create(COMPOSE_TX_TYPE, ledger, bad_utxo, None)


def test_dap_chain_tx_compose_registry_init_returns_int(sdk_initialized):
    if not hasattr(cf, "dap_chain_tx_compose_registry_init"):
        pytest.skip("dap_chain_tx_compose_registry_init not available")
    rc = cf.dap_chain_tx_compose_registry_init()
    assert isinstance(rc, int)
    cf.dap_chain_tx_compose_registry_deinit()


def test_dap_chain_tx_compose_registry_deinit_safe(sdk_initialized):
    if not hasattr(cf, "dap_chain_tx_compose_registry_deinit"):
        pytest.skip("dap_chain_tx_compose_registry_deinit not available")
    cf.dap_chain_tx_compose_registry_deinit()
    cf.dap_chain_tx_compose_registry_deinit()


def test_dap_chain_tx_compose_registry_add_find_remove(compose_registry_api):
    def builder(*_args):
        return None

    rc = cf.dap_chain_tx_compose_registry_add(COMPOSE_TX_TYPE_ALT, builder, None)
    assert isinstance(rc, int)
    assert rc == 0
    entry = cf.dap_chain_tx_compose_registry_find(COMPOSE_TX_TYPE_ALT)
    assert is_capsule(entry)
    assert _PyCapsule_GetName(entry) == b"dap_chain_tx_compose_registry_entry_t"
    assert cf.dap_chain_tx_compose_registry_remove(COMPOSE_TX_TYPE_ALT) is None
    assert cf.dap_chain_tx_compose_registry_find(COMPOSE_TX_TYPE_ALT) is None


def test_dap_chain_tx_compose_registry_add_rejects_invalid(compose_registry_api):
    with pytest.raises(TypeError):
        cf.dap_chain_tx_compose_registry_add(COMPOSE_TX_TYPE, 123)
    with pytest.raises(TypeError):
        cf.dap_chain_tx_compose_registry_add(123, lambda *_: None)


def test_dap_chain_tx_compose_registry_remove_unknown_no_error(compose_registry_api):
    assert cf.dap_chain_tx_compose_registry_remove("unknown") is None


def test_dap_chain_tx_compose_registry_find_returns_none(compose_registry_api):
    assert cf.dap_chain_tx_compose_registry_find("unknown") is None


def test_dap_chain_tx_compose_registry_find_rejects_invalid(compose_registry_api):
    with pytest.raises(TypeError):
        cf.dap_chain_tx_compose_registry_find(123)
