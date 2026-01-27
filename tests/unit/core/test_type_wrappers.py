import ctypes
import struct

import pytest


cf = pytest.importorskip("python_cellframe")
pytestmark = pytest.mark.unit

HASH_SIZE = 32
HASH_BYTES = b"\x00" * HASH_SIZE
BAD_HASH = b"\x00" * (HASH_SIZE - 1)
UINT256_BYTES = b"\x00" * 32

META_GENESIS = 0x01
META_GENERATION = 0x02
META_PREV = 0x10
META_ANCHOR = 0x11
META_LINK = 0x12
META_NONCE = 0x20
META_NONCE2 = 0x21
META_MERKLE = 0x30
META_EMERGENCY = 0x80
META_SYNC_ATTEMPT = 0x81
META_ROUND_ATTEMPT = 0x82
META_EXCLUDED_KEYS = 0x83
META_EVM_DATA = 0x84
META_BLOCKGEN = 0x85

_PyCapsule_New = ctypes.pythonapi.PyCapsule_New
_PyCapsule_New.restype = ctypes.py_object
_PyCapsule_New.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]
_PyCapsule_GetPointer = ctypes.pythonapi.PyCapsule_GetPointer
_PyCapsule_GetPointer.restype = ctypes.c_void_p
_PyCapsule_GetPointer.argtypes = [ctypes.py_object, ctypes.c_char_p]
_capsule_keepalive = []


def make_capsule_from_ptr(ptr, name):
    name_buf = ctypes.create_string_buffer(name.encode("ascii"))
    capsule = _PyCapsule_New(ctypes.c_void_p(ptr), name_buf, None)
    _capsule_keepalive.append(name_buf)
    return capsule


def build_round_item_bytes(event_bytes, datum_hash=None):
    if datum_hash is None:
        datum_hash = HASH_BYTES

    class _RoundInfo(ctypes.Structure):
        _pack_ = 1
        _fields_ = [
            ("reject_count", ctypes.c_uint16),
            ("ts_update", ctypes.c_uint64),
            ("datum_hash", ctypes.c_ubyte * HASH_SIZE),
        ]

    class _RoundItem(ctypes.Structure):
        _pack_ = 1
        _fields_ = [
            ("round_info", _RoundInfo),
            ("event_size", ctypes.c_uint32),
            ("data_size", ctypes.c_uint32),
        ]

    datum_hash_arr = (ctypes.c_ubyte * HASH_SIZE).from_buffer_copy(datum_hash)
    round_info = _RoundInfo(0, 0, datum_hash_arr)
    event_size = len(event_bytes)
    item = _RoundItem(round_info, event_size, event_size)
    return bytes(item) + event_bytes


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
def block_module(sdk_initialized):
    cf.dap_chain_block_init()
    yield
    cf.dap_chain_block_deinit()


@pytest.fixture
def block_cache_module(sdk_initialized):
    cf.dap_chain_block_cache_init()
    yield
    cf.dap_chain_block_cache_deinit()


@pytest.fixture
def dag_module(sdk_initialized):
    cf.dap_chain_type_dag_init()
    yield
    cf.dap_chain_type_dag_deinit()


@pytest.fixture
def datum_capsule(sdk_initialized):
    return cf.dap_chain_datum_create(1, b"datum")


@pytest.fixture
def datum_bytes(datum_capsule):
    size = cf.dap_chain_datum_size(datum_capsule)
    ptr = _PyCapsule_GetPointer(datum_capsule, b"dap_chain_datum_t")
    assert ptr
    return ctypes.string_at(ptr, size)


@pytest.fixture
def datum_hash_bytes(datum_capsule):
    return cf.dap_chain_datum_calc_hash(datum_capsule)


@pytest.fixture
def block_bytes(block_module):
    return cf.dap_chain_block_new()


@pytest.fixture
def event_dict(dag_module, datum_bytes):
    return cf.dap_chain_type_dag_event_new(1, 1, datum_bytes)


@pytest.fixture
def event_bytes(event_dict):
    return event_dict["event"]


@pytest.fixture
def round_item_bytes(event_bytes, datum_hash_bytes):
    return build_round_item_bytes(event_bytes, datum_hash_bytes)


@pytest.fixture
def key_bundle(sdk_initialized):
    crypto = pytest.importorskip("dap.crypto")
    try:
        key = crypto.DapKey.generate()
        pkey = crypto.DapPKey.from_key(key)
    except Exception as exc:
        pytest.skip(f"DapKey generation failed: {exc}")

    pkey_str = None
    try:
        pkey_str = pkey.to_hex()
    except Exception:
        pkey_str = None
    key_capsule = make_capsule_from_ptr(key.ptr, "dap_enc_key_t")
    pkey_capsule = make_capsule_from_ptr(pkey.ptr, "dap_pkey_t")
    pkey_bytes = ctypes.string_at(pkey.ptr, pkey.size)
    yield {
        "key": key,
        "pkey": pkey,
        "key_capsule": key_capsule,
        "pkey_capsule": pkey_capsule,
        "pkey_bytes": pkey_bytes,
        "pkey_str": pkey_str,
    }


def test_dap_chain_block_init_returns_int(sdk_initialized):
    rc = cf.dap_chain_block_init()
    assert isinstance(rc, int)
    rc2 = cf.dap_chain_block_init()
    assert isinstance(rc2, int)


def test_dap_chain_block_deinit_safe(sdk_initialized):
    cf.dap_chain_block_deinit()
    cf.dap_chain_block_deinit()


def test_dap_chain_block_new_returns_bytes(block_module):
    block = cf.dap_chain_block_new()
    assert isinstance(block, (bytes, bytearray))
    assert len(block) > 0


def test_dap_chain_block_new_accepts_prev_hash(block_module):
    block = cf.dap_chain_block_new(HASH_BYTES)
    assert isinstance(block, (bytes, bytearray))
    assert len(block) > 0


def test_dap_chain_block_new_rejects_bad_hash(block_module):
    with pytest.raises(ValueError):
        cf.dap_chain_block_new(BAD_HASH)


def test_dap_chain_block_get_size_accepts_bytes_and_dict(block_bytes):
    size = cf.dap_chain_block_get_size(block_bytes)
    assert size == len(block_bytes)
    size_dict = cf.dap_chain_block_get_size({"block": block_bytes})
    assert size_dict == len(block_bytes)


def test_dap_chain_block_get_size_rejects_short_bytes():
    with pytest.raises(ValueError):
        cf.dap_chain_block_get_size(b"\x00")


def test_dap_chain_block_get_size_rejects_missing_dict_key(block_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_block_get_size({"bad": block_bytes})


def test_dap_chain_block_meta_add_adds_meta(block_bytes):
    new_block = cf.dap_chain_block_meta_add(block_bytes, META_PREV, HASH_BYTES)
    assert len(new_block) > len(block_bytes)


def test_dap_chain_block_meta_add_allows_empty_data(block_bytes):
    new_block = cf.dap_chain_block_meta_add(block_bytes, META_GENESIS, None)
    assert len(new_block) > len(block_bytes)


def test_dap_chain_block_meta_add_rejects_non_bytes(block_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_block_meta_add(block_bytes, META_PREV, object())


def test_dap_chain_block_meta_add_rejects_bad_block():
    with pytest.raises(ValueError):
        cf.dap_chain_block_meta_add(b"\x00", META_PREV, HASH_BYTES)


def test_dap_chain_block_meta_get_returns_expected_types(block_module):
    genesis_block = cf.dap_chain_block_new()
    assert cf.dap_chain_block_meta_get(genesis_block, META_GENESIS) is True

    prev_block = cf.dap_chain_block_new(HASH_BYTES)
    prev = cf.dap_chain_block_meta_get(prev_block, META_PREV)
    assert prev == HASH_BYTES

    nonce_block = cf.dap_chain_block_meta_add(genesis_block, META_NONCE, struct.pack("<Q", 42))
    assert cf.dap_chain_block_meta_get(nonce_block, META_NONCE) == 42

    gen_block = cf.dap_chain_block_meta_add(genesis_block, META_GENERATION, struct.pack("<H", 7))
    assert cf.dap_chain_block_meta_get(gen_block, META_GENERATION) == 7

    round_block = cf.dap_chain_block_meta_add(genesis_block, META_ROUND_ATTEMPT, struct.pack("<B", 1))
    assert cf.dap_chain_block_meta_get(round_block, META_ROUND_ATTEMPT) == 1

    evm_block = cf.dap_chain_block_meta_add(genesis_block, META_EVM_DATA, b"evm")
    assert cf.dap_chain_block_meta_get(evm_block, META_EVM_DATA) == b"evm"

    excluded = struct.pack("<HHH", 2, 5, 7)
    excluded_block = cf.dap_chain_block_meta_add(genesis_block, META_EXCLUDED_KEYS, excluded)
    assert cf.dap_chain_block_meta_get(excluded_block, META_EXCLUDED_KEYS) == [5, 7]

    assert cf.dap_chain_block_meta_get(genesis_block, 0x99) is None


def test_dap_chain_block_meta_extract_reports_prev_hash(block_module):
    block = cf.dap_chain_block_new(HASH_BYTES)
    info = cf.dap_chain_block_meta_extract(block)
    assert isinstance(info, dict)
    assert info["prev_hash"] == HASH_BYTES
    assert isinstance(info["rc"], int)
    assert isinstance(info["links"], list)


def test_dap_chain_block_get_prev_hash_handles_genesis_and_prev(block_module):
    genesis = cf.dap_chain_block_new()
    assert cf.dap_chain_block_get_prev_hash(genesis) is None

    prev_block = cf.dap_chain_block_new(HASH_BYTES)
    assert cf.dap_chain_block_get_prev_hash(prev_block) == HASH_BYTES


def test_dap_chain_block_get_prev_hash_rejects_bad_block():
    with pytest.raises(ValueError):
        cf.dap_chain_block_get_prev_hash(b"\x00")


def test_dap_chain_block_get_datums_empty(block_bytes):
    assert cf.dap_chain_block_get_datums(block_bytes) == []


def test_dap_chain_block_datum_add_and_get_datums(block_bytes, datum_bytes):
    new_block = cf.dap_chain_block_datum_add(block_bytes, datum_bytes)
    datums = cf.dap_chain_block_get_datums(new_block)
    assert isinstance(datums, list)
    assert len(datums) == 1
    assert datums[0] == datum_bytes


def test_dap_chain_block_datum_add_rejects_bad_datum(block_bytes):
    with pytest.raises(ValueError):
        cf.dap_chain_block_datum_add(block_bytes, b"\x00")


def test_dap_chain_block_datum_add_rejects_bad_block(datum_bytes):
    with pytest.raises(ValueError):
        cf.dap_chain_block_datum_add(b"\x00", datum_bytes)


def test_dap_chain_block_datum_del_by_hash_removes(block_bytes, datum_bytes, datum_hash_bytes):
    new_block = cf.dap_chain_block_datum_add(block_bytes, datum_bytes)
    cleared = cf.dap_chain_block_datum_del_by_hash(new_block, datum_hash_bytes)
    datums = cf.dap_chain_block_get_datums(cleared)
    assert datums == []


def test_dap_chain_block_datum_del_by_hash_rejects_bad_hash(block_bytes, datum_bytes):
    new_block = cf.dap_chain_block_datum_add(block_bytes, datum_bytes)
    with pytest.raises(ValueError):
        cf.dap_chain_block_datum_del_by_hash(new_block, BAD_HASH)


def test_dap_chain_block_datum_del_by_hash_rejects_bad_block(datum_hash_bytes):
    with pytest.raises(ValueError):
        cf.dap_chain_block_datum_del_by_hash(b"\x00", datum_hash_bytes)


def test_dap_chain_block_datum_del_by_hash_missing_keeps_block(block_bytes, datum_hash_bytes):
    updated = cf.dap_chain_block_datum_del_by_hash(block_bytes, datum_hash_bytes)
    assert len(updated) == len(block_bytes)
    assert cf.dap_chain_block_get_datums(updated) == []


def test_dap_chain_block_sign_get_none_for_unsigned(block_bytes):
    assert cf.dap_chain_block_sign_get(block_bytes, 0) is None


def test_dap_chain_block_sign_add_and_get_signs(block_bytes, key_bundle):
    key_capsule = key_bundle["key_capsule"]
    signed = cf.dap_chain_block_sign_add(block_bytes, key_capsule)
    assert len(signed) > len(block_bytes)
    assert cf.dap_chain_block_get_signs_count(signed) == 1

    sign = cf.dap_chain_block_sign_get(signed, 0)
    assert isinstance(sign, (bytes, bytearray))
    assert len(sign) > 0

    assert cf.dap_chain_block_sign_get(signed, 1) is None


def test_dap_chain_block_get_signs_count_zero(block_bytes):
    assert cf.dap_chain_block_get_signs_count(block_bytes) == 0


def test_dap_chain_block_sign_match_pkey(block_bytes, key_bundle):
    key_capsule = key_bundle["key_capsule"]
    pkey_capsule = key_bundle["pkey_capsule"]
    signed = cf.dap_chain_block_sign_add(block_bytes, key_capsule)
    assert cf.dap_chain_block_sign_match_pkey(signed, pkey_capsule) is True


def test_dap_chain_block_sign_match_pkey_rejects_bad_pkey(block_bytes, key_bundle):
    key_capsule = key_bundle["key_capsule"]
    signed = cf.dap_chain_block_sign_add(block_bytes, key_capsule)
    with pytest.raises(TypeError):
        cf.dap_chain_block_sign_match_pkey(signed, object())


def test_dap_chain_block_sign_match_pkey_accepts_bytes(block_bytes, key_bundle):
    key_capsule = key_bundle["key_capsule"]
    signed = cf.dap_chain_block_sign_add(block_bytes, key_capsule)
    assert cf.dap_chain_block_sign_match_pkey(signed, key_bundle["pkey_bytes"]) is True


def test_dap_chain_block_sign_match_pkey_accepts_str(block_bytes, key_bundle):
    pkey_str = key_bundle["pkey_str"]
    if not pkey_str:
        pytest.skip("pkey string not available")
    key_capsule = key_bundle["key_capsule"]
    signed = cf.dap_chain_block_sign_add(block_bytes, key_capsule)
    assert cf.dap_chain_block_sign_match_pkey(signed, pkey_str) is True


def test_dap_chain_block_sign_add_rejects_bad_key(block_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_block_sign_add(block_bytes, object())


def test_dap_chain_block_sign_add_rejects_bad_block(key_bundle):
    with pytest.raises(ValueError):
        cf.dap_chain_block_sign_add(b"\x00", key_bundle["key_capsule"])


def test_dap_chain_block_get_sign_offset_returns_int(block_bytes):
    offset = cf.dap_chain_block_get_sign_offset(block_bytes)
    assert isinstance(offset, int)
    assert 0 <= offset <= len(block_bytes)


def test_dap_chain_block_get_sign_offset_rejects_bad_block():
    with pytest.raises(ValueError):
        cf.dap_chain_block_get_sign_offset(b"\x00")


def test_dap_chain_block_cache_init_returns_int(sdk_initialized):
    rc = cf.dap_chain_block_cache_init()
    assert isinstance(rc, int)


def test_dap_chain_block_cache_deinit_safe(sdk_initialized):
    cf.dap_chain_block_cache_deinit()
    cf.dap_chain_block_cache_deinit()


def test_dap_chain_block_cache_new_returns_dict(block_bytes, block_cache_module):
    cache = cf.dap_chain_block_cache_new(block_bytes)
    assert isinstance(cache, dict)
    assert cache["block_size"] == len(block_bytes)
    assert isinstance(cache["block_hash"], (bytes, bytearray))
    assert len(cache["block_hash"]) == HASH_SIZE
    assert isinstance(cache["datums"], list)


def test_dap_chain_block_cache_dup_returns_dict(block_bytes, block_cache_module):
    cache = cf.dap_chain_block_cache_dup(block_bytes)
    assert isinstance(cache, dict)
    assert isinstance(cache["block_hash"], (bytes, bytearray))


def test_dap_chain_block_cache_update_returns_rc(block_bytes, block_cache_module):
    cache = cf.dap_chain_block_cache_update(block_bytes)
    assert isinstance(cache, dict)
    assert isinstance(cache.get("rc"), int)


def test_dap_chain_block_cache_delete_returns_none(block_bytes, block_cache_module):
    assert cf.dap_chain_block_cache_delete(block_bytes) is None


def test_dap_chain_block_get_list_tx_cond_outs_with_val_rejects_bad_ledger(block_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_block_get_list_tx_cond_outs_with_val(object(), block_bytes)


def test_dap_chain_block_tx_coll_fee_create_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_block_tx_coll_fee_create(
            object(),
            object(),
            object(),
            [HASH_BYTES],
            object(),
            "CELL",
            1,
            UINT256_BYTES,
        )


def test_dap_chain_block_tx_reward_create_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_block_tx_reward_create(
            object(),
            object(),
            object(),
            [HASH_BYTES],
            object(),
            "CELL",
            1,
            UINT256_BYTES,
        )


def test_dap_chain_block_tx_coll_fee_stack_create_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_block_tx_coll_fee_stack_create(
            object(),
            object(),
            object(),
            object(),
            "CELL",
            1,
            UINT256_BYTES,
        )


def test_dap_chain_blocks_decree_init_deinit(sdk_initialized):
    assert cf.dap_chain_blocks_decree_init() is None
    assert cf.dap_chain_blocks_decree_deinit() is None


def test_dap_chain_block_add_fork_notificator_registers(block_module):
    def callback(chain, before_hash, reverted, reverted_cnt, main_cnt, user_data=None):
        return None

    rc = cf.dap_chain_block_add_fork_notificator(callback)
    assert isinstance(rc, int)


def test_dap_chain_block_add_fork_notificator_rejects_bad_callback():
    with pytest.raises(TypeError):
        cf.dap_chain_block_add_fork_notificator(object())


def test_dap_chain_block_cache_get_by_hash_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_block_cache_get_by_hash(object(), HASH_BYTES)


def test_dap_chain_block_cache_get_by_number_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_block_cache_get_by_number(object(), 0)


def test_dap_chain_cs_blocks_get_fee_group_returns_str():
    group = cf.dap_chain_cs_blocks_get_fee_group("net")
    assert isinstance(group, str)
    assert "net" in group
    assert "fees" in group


def test_dap_chain_cs_blocks_get_reward_group_returns_str():
    group = cf.dap_chain_cs_blocks_get_reward_group("net")
    assert isinstance(group, str)
    assert "net" in group
    assert "rewards" in group


def test_dap_chain_type_blocks_init_returns_int(sdk_initialized):
    rc = cf.dap_chain_type_blocks_init()
    assert isinstance(rc, int)


def test_dap_chain_type_blocks_deinit_safe(sdk_initialized):
    assert cf.dap_chain_type_blocks_deinit() is None


def test_dap_chain_type_blocks_fees_aggregate_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_type_blocks_fees_aggregate(object())


def test_dap_chain_type_blocks_get_block_signers_rewards_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_type_blocks_get_block_signers_rewards(object(), HASH_BYTES)


def test_dap_chain_type_blocks_get_pkey_by_hash_rejects_bad_net():
    with pytest.raises(TypeError):
        cf.dap_chain_type_blocks_get_pkey_by_hash(object(), HASH_BYTES)


def test_dap_chain_type_dag_init_returns_int(sdk_initialized):
    rc = cf.dap_chain_type_dag_init()
    assert isinstance(rc, int)


def test_dap_chain_type_dag_deinit_safe(sdk_initialized):
    assert cf.dap_chain_type_dag_deinit() is None


def test_dap_chain_type_dag_start_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_start(object())


def test_dap_chain_type_dag_proc_event_round_new_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_proc_event_round_new(object())


def test_dap_chain_type_dag_find_event_by_hash_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_find_event_by_hash(object(), HASH_BYTES)


def test_dap_chain_type_dag_event_new_returns_dict(datum_bytes):
    event = cf.dap_chain_type_dag_event_new(1, 1, datum_bytes)
    assert isinstance(event, dict)
    assert isinstance(event["event"], (bytes, bytearray))
    assert event["size"] == len(event["event"])


def test_dap_chain_type_dag_event_new_rejects_bad_cell_id(datum_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_event_new(1, object(), datum_bytes)


def test_dap_chain_type_dag_event_new_rejects_bad_datum():
    with pytest.raises(ValueError):
        cf.dap_chain_type_dag_event_new(1, 1, b"\x00")


def test_dap_chain_type_dag_event_calc_hash_accepts_bytes_and_dict(event_bytes):
    hash_bytes = cf.dap_chain_type_dag_event_calc_hash(event_bytes)
    assert isinstance(hash_bytes, (bytes, bytearray))
    assert len(hash_bytes) == HASH_SIZE

    hash_dict = cf.dap_chain_type_dag_event_calc_hash({"event": event_bytes})
    assert hash_dict == hash_bytes


def test_dap_chain_type_dag_event_calc_hash_rejects_missing_dict_key(event_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_event_calc_hash({"bad": event_bytes})


def test_dap_chain_type_dag_event_calc_hash_rejects_short_bytes():
    with pytest.raises(ValueError):
        cf.dap_chain_type_dag_event_calc_hash(b"\x00")


def test_dap_chain_type_dag_event_calc_size_returns_int(event_bytes):
    size = cf.dap_chain_type_dag_event_calc_size(event_bytes)
    assert isinstance(size, int)
    assert size == len(event_bytes)


def test_dap_chain_type_dag_event_calc_size_limit_too_small(event_bytes):
    with pytest.raises(ValueError):
        cf.dap_chain_type_dag_event_calc_size(event_bytes, 1)


def test_dap_chain_type_dag_event_calc_size_excl_signs_returns_int(event_bytes):
    size = cf.dap_chain_type_dag_event_calc_size_excl_signs(event_bytes)
    assert isinstance(size, int)
    assert size > 0


def test_dap_chain_type_dag_event_calc_size_excl_signs_limit_too_small(event_bytes):
    with pytest.raises(ValueError):
        cf.dap_chain_type_dag_event_calc_size_excl_signs(event_bytes, 1)


def test_dap_chain_type_dag_event_get_datum_returns_bytes(event_bytes, datum_bytes):
    datum = cf.dap_chain_type_dag_event_get_datum(event_bytes)
    assert isinstance(datum, (bytes, bytearray))
    assert datum == datum_bytes


def test_dap_chain_type_dag_event_get_datum_size_maximum(event_bytes, datum_bytes):
    max_size = cf.dap_chain_type_dag_event_get_datum_size_maximum(event_bytes)
    assert isinstance(max_size, int)
    assert max_size >= len(datum_bytes)


def test_dap_chain_type_dag_event_get_sign_none_before_sign(event_bytes):
    assert cf.dap_chain_type_dag_event_get_sign(event_bytes, 0) is None


def test_dap_chain_type_dag_event_get_sign_after_sign(event_bytes, key_bundle):
    key_capsule = key_bundle["key_capsule"]
    signed = cf.dap_chain_type_dag_event_sign_add(event_bytes, key_capsule)
    sign = cf.dap_chain_type_dag_event_get_sign(signed, 0)
    assert isinstance(sign, (bytes, bytearray))
    assert len(sign) > 0


def test_dap_chain_type_dag_event_sign_add_and_exists(event_bytes, key_bundle):
    key_capsule = key_bundle["key_capsule"]
    assert cf.dap_chain_type_dag_event_sign_exists(event_bytes, key_capsule) is False
    signed = cf.dap_chain_type_dag_event_sign_add(event_bytes, key_capsule)
    assert len(signed) >= len(event_bytes)
    assert cf.dap_chain_type_dag_event_sign_exists(signed, key_capsule) is True


def test_dap_chain_type_dag_event_sign_add_rejects_bad_key(event_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_event_sign_add(event_bytes, object())


def test_dap_chain_type_dag_event_sign_exists_rejects_bad_key(event_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_event_sign_exists(event_bytes, object())


def test_dap_chain_type_dag_event_round_item_get_size(round_item_bytes):
    size = cf.dap_chain_type_dag_event_round_item_get_size(round_item_bytes)
    assert size == len(round_item_bytes)

    size_dict = cf.dap_chain_type_dag_event_round_item_get_size({"round_item": round_item_bytes})
    assert size_dict == len(round_item_bytes)


def test_dap_chain_type_dag_event_round_item_get_size_rejects_short_bytes():
    with pytest.raises(ValueError):
        cf.dap_chain_type_dag_event_round_item_get_size(b"\x00")


def test_dap_chain_type_dag_event_round_item_get_size_rejects_missing_key(round_item_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_event_round_item_get_size({"bad": round_item_bytes})


def test_dap_chain_type_dag_event_round_sign_add_and_exists(round_item_bytes, key_bundle):
    key_capsule = key_bundle["key_capsule"]
    assert cf.dap_chain_type_dag_event_round_sign_exists(round_item_bytes, key_capsule) is False
    signed = cf.dap_chain_type_dag_event_round_sign_add(round_item_bytes, key_capsule)
    assert len(signed) > len(round_item_bytes)
    assert cf.dap_chain_type_dag_event_round_sign_exists(signed, key_capsule) is True


def test_dap_chain_type_dag_event_round_sign_add_rejects_bad_key(round_item_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_event_round_sign_add(round_item_bytes, object())


def test_dap_chain_type_dag_event_round_sign_exists_rejects_bad_key(round_item_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_event_round_sign_exists(round_item_bytes, object())


def test_dap_chain_type_dag_event_gdb_set_rejects_bad_chain(event_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_event_gdb_set(object(), "hash", event_bytes)


def test_dap_chain_type_dag_event_gdb_set_rejects_bad_hash(event_bytes):
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_event_gdb_set(None, 123, event_bytes)


def test_dap_chain_type_dag_event_gdb_get_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_chain_type_dag_event_gdb_get(object(), "hash")


def test_dap_chain_type_dag_event_broadcast_not_implemented():
    with pytest.raises(NotImplementedError):
        cf.dap_chain_type_dag_event_broadcast(None, None, None)


def test_dap_nonconsensus_init_returns_int(sdk_initialized):
    rc = cf.dap_nonconsensus_init()
    assert isinstance(rc, int)


def test_dap_nonconsensus_get_group_rejects_bad_chain():
    with pytest.raises(TypeError):
        cf.dap_nonconsensus_get_group(object())
