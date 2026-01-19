# Unit Test Plan: datum

This document describes unit test coverage for the datum module wrappers.
Focus is wrapper behavior (argument validation, return types) and basic smoke
checks; integration-style checks are noted where SDK state is required.

## Common setup and fixtures

- `sdk_available`: skip tests if `python_cellframe.is_sdk_available()` is false.
- `init_sdk`: call `python_cellframe.initialize()` once per session (same pattern as chain tests).
- `hash_fast_bytes`: `b"\x00" * HASH_SIZE` (match `sizeof(dap_hash_fast_t)`; confirm size in SDK).
- `hash_fast_str`: hex/base58 string for a known hash (use SDK helper if available).
- `hash_out_type`: `"hex"` (also test `"base58"` where accepted).
- `net_id`: `0` or `1` (uint64).
- `datum_type_tx`: use `0x0100` (DAP_CHAIN_DATUM_TX) for tests.
- `datum_data_bytes`: small bytes payload (e.g., `b"datum"`).
- `datum_capsule`: create via `dap_chain_datum_create(datum_type_tx, datum_data_bytes)`.
- `tx_capsule`: create via `dap_chain_datum_tx_create()` from tx module.
- `tx_wrapped_capsule`: create via `dap_chain_tx_wrap_packed(tx_capsule)` (for tx wrappers).
- `pkey_capsule`: `dap_pkey_t` capsule from `DapCert` or `DapKey` (integration-only).
- `pkey_service_capsule`: `dap_pkey_t` capsule for service/event signing (integration-only).
- `uint256_bytes`: use `dap_chain_uint256_from(1)` for tx values/fees.
- `srv_uid`: `1` (uint64).
- `unit_enm`: `SERV_UNIT_B` or `SERV_UNIT_SEC` (enum int).
- `cond_bytes`: small bytes payload (e.g., `b"cond"`), optional for out_cond.
- `event_group`: `"group1"` (non-empty).
- `event_type`: `1` (uint16).
- `event_data_bytes`: small bytes payload (e.g., `b"event"`).
- `event_timestamp`: `0` (uint64).
- `node_addr_int`: `1` (uint64) for `dap_chain_node_addr_t`.
- `tx_item_tsd_type`: `0x1000` (TSD type for item data).
- `tx_item_tsd_data`: small bytes payload (e.g., `b"tsd"`).
- `hashes_list`: list of 2 `hash_fast_bytes` (for wallet_shared).
- `signs_min`: `1` (minimum signatures for wallet_shared).
- `sell_net_id`: `net_id` (uint64).
- `buy_net_id`: `net_id + 1` (uint64).
- `stake_lock_time`: `0` (uint64).
- `position_id`: `0` (uint32).
- `addr_list`: list of 2 addresses (capsules/bytes/strings) for multi-transfer.
- `values_list`: list of 2 uint256 values (ints/strings/bytes) matching `addr_list` length.
- `time_unlocks`: list of 2 uint64 timestamps (optional for multi-transfer).
- `tx_item_type`: valid `DAP_CHAIN_TX_ITEM_TYPE_*` enum (e.g., OUT).
- `out_cond_subtype`: valid `DAP_CHAIN_TX_OUT_COND_SUBTYPE_*` enum (e.g., SRV_PAY).
- `receipt_ext_bytes`: `b"ext"` (optional extension payload for receipt).
- `vote_answer_idx`: `0` (uint64).
- `voting_question`: `"Question?"` (non-empty).
- `voting_option`: `"Option A"` (non-empty).
- `voting_expire`: `1` (dap_time_t).
- `voting_max_votes`: `1` (uint64).
- `voting_delegate_required`: `True`.
- `voting_vote_changing_allowed`: `True`.
- `voting_tx_cond_out_idx`: `0` (int).
- `compose_tx_type`: `"test_tx"` (short ascii string).
- `compose_user_data`: `{"k": "v"}` (any python object).
- `dummy_ledger_capsule`: `dummy_capsule("dap_ledger_t")` (non-null pointer).
- `dummy_utxo_capsule`: `dummy_capsule("dap_list_t")` (non-null pointer).
- `compose_params`: `{"k": "v"}` (any python object).
- `compose_builder_ok`: callback returning datum capsule via `dap_chain_datum_create`.
- `compose_builder_none`: callback returning `None` to simulate builder failure.
- `json_obj`: `dap.json.DapJson.object()`; use `json_obj.capsule`.
- `json_array`: `dap.json.DapJson.array()`; use `json_array.capsule`.
- `signs_bytes_empty`: `b""` (for anchor certs dump).
- `chain_id`: `0` or `1` (uint64).
- `cell_id`: `0` or `1` (uint64).
- `decree_capsule_empty`: create via `dap_chain_datum_decree_new(net_id, chain_id, cell_id, 0)`.
- `decree_size`: `dap_chain_datum_decree_get_size(decree_capsule_empty)`.
- `decree_signs_empty`: `b""` (for decree certs dump).
- `cert_capsule`: wrap `DapCertificate.ptr` in `PyCapsule_New(ptr, "dap_cert_t", NULL)`; for sign tests.
- `net_capsule`: use real net capsule from network module when available; otherwise `dummy_capsule("dap_chain_net_t")` for validation-only tests.
- `dummy_capsule(name)`: helper using `ctypes.pythonapi.PyCapsule_New` (as in chain tests)
  for validation-only tests where the pointer is never dereferenced.
- `token_ticker`: `"CELL"` (valid ticker).
- `token_ticker_bad_short`: `"AA"` (too short).
- `token_ticker_bad_chars`: `"C-ELL"` (non-alnum).
- `token_flags_zero`: `0` (no flags).
- `token_flag_str_valid`: `"ALL_BLOCKED"` or `"NO_FLAGS"`.
- `token_flag_str_invalid`: `"UNKNOWN_FLAG"`.
- `token_serial_short`: `b"\x00" * 8` (intentionally too small).
- `token_serial_min`: real token bytes from ledger or token creation flow (integration-only).
- `addr_capsule`: address capsule from wallet or `dap_chain_addr_from_str` (integration-only).
- `addr_bytes`: bytes of length `sizeof(dap_chain_addr_t)` from `addr_capsule` (if a helper exists).
- `emission_value_uint256`: bytes of length `sizeof(uint256_t)` representing value (e.g., from `dap_chain_uint256_from`).
- `emission_tsd_type_token_sym`: `0x0007` (DAP_CHAIN_DATUM_EMISSION_TSD_TYPE_TOKEN_SYM).
- `emission_tsd_payload`: `b"CELL"` (or other bytes payload).
- `emission_serial_short`: `b"\x00" * 8` (intentionally too small).
- `emission_serial_min`: bytes of length `sizeof(dap_chain_emission_header_v0)` with zeros (smoke only).
- `emission_capsule`: create via `dap_chain_datum_emission_create` (integration-only).
- `enc_key_capsule`: `dap_enc_key_t` capsule from wallet key or `DapCert` (integration-only).
- `sign_capsule`: `dap_sign_t` capsule from `dap_sign_create` (if wrapped) or other signing API (integration-only).

Note: Some functions require fully formed SDK structures (token/anchor/tx).
For those, include validation-only unit tests and mark integration tests for later.

---

## dap_chain_datum_calc_hash
- What to test: accepts datum capsule and returns hash bytes of correct size.
- Edge cases: invalid capsule type; None input.
- Test ideas: create datum via `dap_chain_datum_create`, call, assert `len == HASH_SIZE`;
  invalid args raise TypeError/ValueError.

## dap_chain_datum_create
- What to test: creates datum capsule from type id and optional data.
- Edge cases: data is None; data is bytes-like (bytearray/memoryview); invalid data type.
- Test ideas: call with no data, with bytes, and with memoryview; assert capsule returned;
  invalid data (e.g., int) raises TypeError.

## dap_chain_datum_dump_tx_json
- What to test: returns bool; accepts json array (or None), tx capsule, ticker (str/bytes/None),
  json object, hash_out_type (str/bytes), tx_hash (bytes/str/capsule), net_id, version.
- Edge cases: wrong capsule types; wrong hash length; ticker type not str/bytes/None.
- Test ideas: build `tx_capsule` via `dap_chain_datum_tx_create`, `json_obj`, `json_array`;
  call with hash bytes and `hash_out_type="hex"`, expect bool; invalid args raise errors.
  Integration: verify actual JSON content after tx fields are populated.

## dap_chain_datum_register_dump_anchor_callback
- What to test: accepts callable and returns None.
- Edge cases: non-callable callback.
- Test ideas: register lambda and assert no exception; invalid arg raises TypeError.
  Integration: trigger via code path that dumps anchor JSON and assert callback invoked.

## dap_chain_datum_register_dump_decree_callback
- What to test: accepts callable and returns None.
- Edge cases: non-callable callback.
- Test ideas: register lambda and assert no exception; invalid arg raises TypeError.
  Integration: trigger via code path that dumps decree JSON and assert callback invoked.

## dap_chain_datum_size
- What to test: returns integer size for datum capsule.
- Edge cases: invalid capsule type.
- Test ideas: create datum with `datum_data_bytes`, call size and assert `size >= len(data)`;
  if header size is known, assert `size == header_size + len(data)`.

## dap_chain_datum_type_id_to_str
- What to test: returns correct string for known type ids.
- Edge cases: unknown type id.
- Test ideas: `0x0100 -> "DATUM_TX"`, `0xffff -> "DATUM_CUSTOM"`, unknown -> "DATUM_UNKNOWN".

## dap_datum_token_dump_tsd_to_json
- What to test: accepts json capsule, token capsule, token_size, hash_out_type; returns None.
- Edge cases: invalid token capsule; negative size; invalid hash_out_type type.
- Test ideas: validation-only unit tests (TypeError/ValueError).
  Integration: create token datum in Part 3 and assert JSON fields are populated.

## dap_chain_datum_anchor_certs_dump
- What to test: returns string; accepts signs bytes and hash_out_type.
- Edge cases: signs not bytes; hash_out_type not str/bytes.
- Test ideas: call with empty `signs_bytes_empty` and `"hex"`, assert string returned (possibly empty).

## dap_chain_datum_anchor_certs_dump_json
- What to test: accepts json capsule, signs bytes, hash_out_type, version; returns None.
- Edge cases: invalid json capsule; signs not bytes; invalid version type.
- Test ideas: use `json_obj` and empty signs, assert no exception; invalid args raise TypeError.

## dap_chain_datum_anchor_get_hash_from_data
- What to test: returns tuple `(rc, hash_bytes|None)`.
- Edge cases: invalid anchor capsule type.
- Test ideas: validation-only unit tests for TypeError.
  Integration: construct a real anchor datum and verify `rc == 0` and hash length.

## dap_chain_datum_anchor_get_size
- What to test: returns size as int.
- Edge cases: invalid anchor capsule type.
- Test ideas: validation-only unit tests for TypeError.
  Integration: build anchor datum and compare to expected size.

## dap_chain_datum_from_tx
- What to test: converts tx datum capsule to datum capsule.
- Edge cases: invalid capsule type.
- Test ideas: call with `tx_capsule` and assert capsule or None (depending on SDK requirements);
  invalid type raises TypeError. Integration: use signed tx and assert datum returned.

---

## Part 2: Decree datums + registry

## dap_chain_datum_decree_certs_dump
- What to test: returns string; accepts signs bytes and hash_out_type.
- Edge cases: signs not bytes; hash_out_type not str/bytes.
- Test ideas: call with `decree_signs_empty` and `"hex"`; assert string returned (possibly empty);
  invalid arg types raise TypeError.

## dap_chain_datum_decree_certs_dump_json
- What to test: accepts json capsule, signs bytes, hash_out_type, version; returns None.
- Edge cases: invalid json capsule; signs not bytes; invalid version type.
- Test ideas: use `json_obj` and empty signs; assert no exception; invalid args raise TypeError.

## dap_chain_datum_decree_dump
- What to test: returns string dump; accepts decree capsule, decree_size, hash_out_type.
- Edge cases: invalid decree capsule; negative decree_size; hash_out_type not str/bytes.
- Test ideas: call with `decree_capsule_empty`, `decree_size`, `"hex"` and assert string;
  negative size raises ValueError; invalid capsule raises TypeError.

## dap_chain_datum_decree_dump_json
- What to test: fills JSON object; accepts json capsule, decree capsule, decree_size, hash_out_type, version.
- Edge cases: invalid json/decree capsule; negative decree_size; invalid version type.
- Test ideas: use `json_obj` + empty decree; assert no exception; invalid args raise TypeError/ValueError.
  Integration: populate decree TSDs and assert JSON fields exist.

## dap_chain_datum_decree_get_action
- What to test: returns tuple `(rc, action|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: call with `decree_capsule_empty` and expect `rc != 0` and `None`;
  invalid capsule raises TypeError. Integration: decree with ACTION TSD returns `rc == 0`.

## dap_chain_datum_decree_get_atom_num
- What to test: returns tuple `(rc, atom_num|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0`; invalid capsule raises TypeError.
  Integration: decree with BLOCK_NUM TSD returns `rc == 0` and uint64.

## dap_chain_datum_decree_get_ban_addr
- What to test: returns tuple `(rc, addr_str|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0` and None; invalid capsule raises TypeError.
  Integration: decree with HOST/STRING TSD returns address string.

## dap_chain_datum_decree_get_empty_block_every_times
- What to test: returns tuple `(rc, period|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0`; invalid capsule raises TypeError.
  Integration: decree with BLOCKGEN_PERIOD TSD returns `rc == 0`.

## dap_chain_datum_decree_get_fee
- What to test: returns tuple `(rc, uint256_bytes|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0`; invalid capsule raises TypeError.
  Integration: decree with FEE TSD returns bytes length `sizeof(uint256_t)`.

## dap_chain_datum_decree_get_fee_addr
- What to test: returns tuple `(rc, dap_chain_addr_t capsule|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0` and None; invalid capsule raises TypeError.
  Integration: decree with FEE_WALLET TSD returns capsule.

## dap_chain_datum_decree_get_hash
- What to test: returns tuple `(rc, hash_bytes|None)` with correct hash size.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0`; invalid capsule raises TypeError.
  Integration: decree with HASH TSD returns `rc == 0` and `len == HASH_SIZE`.

## dap_chain_datum_decree_get_min_owners
- What to test: returns tuple `(rc, uint256_bytes|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0`; invalid capsule raises TypeError.
  Integration: decree with MIN_OWNER TSD returns bytes size `sizeof(uint256_t)`.

## dap_chain_datum_decree_get_node_addr
- What to test: returns tuple `(rc, dap_chain_node_addr_t capsule|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0` and None; invalid capsule raises TypeError.
  Integration: decree with NODE_ADDR TSD returns capsule.

## dap_chain_datum_decree_get_owners
- What to test: returns tuple `(dap_list_t capsule|None, owners_num)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `(None, 0)` or `(capsule, 0)` depending on SDK; accept either.
  Integration: decree with OWNER TSDs returns list capsule and nonzero count.

## dap_chain_datum_decree_get_pkey
- What to test: returns dap_pkey_t capsule or None.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns None; invalid capsule raises TypeError.
  Integration: decree with STAKE_PKEY TSD returns capsule.

## dap_chain_datum_decree_get_policy
- What to test: returns dap_chain_policy_t capsule or None.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns None; invalid capsule raises TypeError.
  Integration: decree with POLICY_EXECUTE TSD returns capsule (policy created via chain module).

## dap_chain_datum_decree_get_signature_type
- What to test: returns tuple `(rc, signature_type|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0`; invalid capsule raises TypeError.
  Integration: decree with SIGNATURE_TYPE TSD returns `rc == 0` and uint32.

## dap_chain_datum_decree_get_signs
- What to test: returns tuple `(signs_bytes, signs_size)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `(b"", 0)`; invalid capsule raises TypeError.
  Integration: after signing, `signs_size > 0` and `len(signs_bytes) == signs_size`.

## dap_chain_datum_decree_get_size
- What to test: returns size as int.
- Edge cases: invalid decree capsule.
- Test ideas: create decree with `total_tsd_size=0`, assert size > 0; invalid capsule raises TypeError.

## dap_chain_datum_decree_get_stake_min_signers_count
- What to test: returns tuple `(rc, uint256_bytes|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0`; invalid capsule raises TypeError.
  Integration: decree with STAKE_MIN_SIGNERS_COUNT TSD returns bytes size `sizeof(uint256_t)`.

## dap_chain_datum_decree_get_stake_min_value
- What to test: returns tuple `(rc, uint256_bytes|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0`; invalid capsule raises TypeError.
  Integration: decree with STAKE_MIN_VALUE TSD returns bytes size `sizeof(uint256_t)`.

## dap_chain_datum_decree_get_stake_signing_addr
- What to test: returns tuple `(rc, dap_chain_addr_t capsule|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0` and None; invalid capsule raises TypeError.
  Integration: decree with STAKE_SIGNING_ADDR TSD returns capsule.

## dap_chain_datum_decree_get_stake_value
- What to test: returns tuple `(rc, uint256_bytes|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0`; invalid capsule raises TypeError.
  Integration: decree with STAKE_VALUE TSD returns bytes size `sizeof(uint256_t)`.

## dap_chain_datum_decree_get_value
- What to test: returns tuple `(rc, uint256_bytes|None)`.
- Edge cases: invalid decree capsule.
- Test ideas: empty decree returns `rc != 0`; invalid capsule raises TypeError.
  Integration: decree with VALUE TSD returns bytes size `sizeof(uint256_t)`.

## dap_chain_datum_decree_new
- What to test: returns decree capsule for valid ids/size.
- Edge cases: invalid arg types; extremely large `total_tsd_size`.
- Test ideas: create with small size; assert capsule and size >= size of header; invalid types raise TypeError.

## dap_chain_datum_decree_sign_in_cycle
- What to test: returns tuple `(decree_capsule|None, sign_count)`.
- Edge cases: first arg not list/tuple; list item not cert capsule; invalid decree capsule.
- Test ideas: empty cert list returns same decree and `sign_count == 0`; invalid list item raises TypeError.
  Integration: create `DapCertificate`, wrap into `dap_cert_t` capsule, sign and assert `sign_count > 0`.

## dap_chain_datum_decree_subtype_to_str
- What to test: returns string for known subtype.
- Edge cases: unknown subtype.
- Test ideas: `0x0001 -> "DECREE_COMMON_SUBTYPE_FEE"`, unknown -> "DECREE_SUBTYPE_UNKNOWN".

## dap_chain_datum_decree_tsd_type_to_str
- What to test: returns string for known TSD type.
- Edge cases: unknown TSD type.
- Test ideas: `0x0100 -> "DAP_CHAIN_DATUM_DECREE_TSD_TYPE_VALUE"`, unknown -> "DECREE_TSD_TYPE_UNKNOWN".

## dap_chain_datum_decree_type_from_str
- What to test: returns subtype id for known strings.
- Edge cases: unknown string returns 0.
- Test ideas: "fee" -> fee subtype; "owners_min" -> min owners subtype; unknown -> 0.

## dap_chain_decree_registry_deinit
- What to test: returns None; safe to call after init.
- Edge cases: call without init; call twice.
- Test ideas: call init then deinit; call deinit twice and assert no exception.

## dap_chain_decree_registry_has_handler
- What to test: returns bool.
- Edge cases: invalid types (non-int).
- Test ideas: call before registration -> False; register handler then True; unregister then False.

## dap_chain_decree_registry_init
- What to test: returns int code; idempotent.
- Edge cases: call multiple times.
- Test ideas: call twice and assert int return.

## dap_chain_decree_registry_process
- What to test: returns int rc from handler or -404 if none.
- Edge cases: invalid decree/net capsule types; non-bool apply/anchored.
- Test ideas: validation-only for wrong types; integration: register handler that returns 0
  and call with real decree+net to assert rc == 0; without handler expect -404.

## dap_chain_decree_registry_register_handler
- What to test: registers handler; returns int code.
- Edge cases: non-callable handler; handler_name not str.
- Test ideas: register lambda, assert rc == 0; has_handler returns True; non-callable raises TypeError.

## dap_chain_decree_registry_unregister_handler
- What to test: unregisters handler; returns int code.
- Edge cases: unregister missing handler.
- Test ideas: register then unregister -> rc == 0; has_handler False afterward; unregister again returns nonzero.

---

## Part 3: Token + emission datums

## dap_chain_datum_emission_add_sign
- What to test: accepts `dap_enc_key_t` capsule + emission capsule; returns emission capsule or None.
- Edge cases: invalid key capsule; invalid emission capsule; emission type not AUTH; key is None.
- Test ideas: integration-only: create emission via `dap_chain_datum_emission_create`, sign with real key
  (wallet key or `DapCert`), then `dap_chain_datum_emission_get_signs` shows `sign_count > 0`.
  Validation: wrong types raise TypeError/ValueError.

## dap_chain_datum_emission_add_tsd
- What to test: appends TSD; accepts bytes-like or None; returns emission capsule or None.
- Edge cases: invalid emission capsule; data not bytes-like; emission type not AUTH.
- Test ideas: integration-only: add TSD with `emission_tsd_type_token_sym` + `emission_tsd_payload`,
  then read back via `dap_chain_emission_get_tsd`. Validation-only: data type errors raise TypeError.

## dap_chain_datum_emission_append_sign
- What to test: accepts `dap_sign_t` capsule + emission capsule; returns emission capsule or None.
- Edge cases: invalid sign capsule; invalid emission capsule; emission type not AUTH.
- Test ideas: integration-only: create `dap_sign_t` (from `dap_sign_create` or similar), append it,
  then `get_signs` shows count incremented. Validation: wrong types raise TypeError/ValueError.

## dap_chain_datum_emission_create
- What to test: accepts uint256 value, ticker, address; returns emission capsule or None.
- Edge cases: invalid uint256 (wrong byte length); invalid address type; ticker not str/bytes.
- Test ideas: integration-only: build `addr_capsule`, use `emission_value_uint256` and `token_ticker`,
  assert capsule returned. Validation: bad uint256 length raises ValueError; bad address raises TypeError/ValueError.

## dap_chain_datum_emission_get_signs
- What to test: returns `(signs_bytes, sign_count)`.
- Edge cases: invalid emission capsule; emission with no signs.
- Test ideas: for emission with no signs, expect `sign_count == 0` and `signs_bytes == b""`.
  After signing, expect `sign_count > 0` and `len(signs_bytes) > 0`.

## dap_chain_datum_emission_get_size
- What to test: accepts bytes and returns size as int.
- Edge cases: bytes shorter than `sizeof(dap_chain_emission_header_v0)` (ValueError); non-bytes (TypeError).
- Test ideas: pass `emission_serial_min` (zeroed header bytes) and assert `size > 0`.
  Short bytes raise ValueError.

## dap_chain_datum_emission_read
- What to test: returns `(emission_capsule|None, emission_size)`.
- Edge cases: bytes shorter than header (ValueError); non-bytes (TypeError).
- Test ideas: pass `emission_serial_min` and expect capsule + size (or None if SDK rejects).
  Short bytes raise ValueError.

## dap_chain_datum_emission_type_str
- What to test: maps known emission types to strings; unknown -> "UNDEFINED".
- Edge cases: unknown numeric type.
- Test ideas: `0x01 -> "AUTH"`, `0x02 -> "ALGO"`, unknown -> "UNDEFINED".

## dap_chain_datum_token_certs_dump
- What to test: returns string dump from certs bytes + hash_out_type.
- Edge cases: non-bytes certs; hash_out_type not str/bytes.
- Test ideas: call with `signs_bytes_empty` and `"hex"`, expect string (contains "signatures").
  Invalid arg types raise TypeError.

## dap_chain_datum_token_certs_dump_to_json
- What to test: writes cert dump into JSON; returns None.
- Edge cases: invalid json capsule; non-bytes certs; invalid version type.
- Test ideas: use `json_obj` + empty certs + `"hex"` + version `1`, assert no exception;
  invalid types raise TypeError.

## dap_chain_datum_token_check_ticker
- What to test: returns bool for ticker validity.
- Edge cases: too short/too long; non-alnum chars; bytes input.
- Test ideas: `"CELL"` -> True, `"AA"` -> False, `"C-ELL"` -> False, `"CELL123"` -> True.

## dap_chain_datum_token_flag_from_str
- What to test: maps flag name to integer; unknown -> `DAP_CHAIN_DATUM_TOKEN_FLAG_UNDEFINED` (0xFFFF).
- Edge cases: non-string/bytes input.
- Test ideas: `"NO_FLAGS"` -> `0`, `"ALL_BLOCKED"` -> combined flags; unknown -> `0xFFFF`;
  invalid type raises TypeError.

## dap_chain_datum_token_flag_to_str
- What to test: maps flag value to string; unknown -> "UNKNOWN FLAG OR FLAGS GROUP".
- Edge cases: invalid type (non-int).
- Test ideas: `0` -> `"NONE"`; known flag -> expected string; unknown flag -> `"UNKNOWN FLAG OR FLAGS GROUP"`.

## dap_chain_datum_token_flags_dump_to_json
- What to test: writes flags into JSON under a key; returns None.
- Edge cases: invalid json capsule; key not str/bytes; invalid flags type.
- Test ideas: `flags=0` should write string `"NONE"` under key; nonzero flags should create array.
  Validate JSON via `to_string()` contains the key.

## dap_chain_datum_token_get_delegated_ticker
- What to test: returns tuple `(rc, delegated_ticker|None)`.
- Edge cases: non-string/bytes input; empty ticker.
- Test ideas: `"CELL"` -> `rc == 0` and `"mCELL"`. Invalid type raises TypeError.

## dap_chain_datum_token_is_old
- What to test: returns bool for old token type ids.
- Edge cases: unknown type id.
- Test ideas: old types (0x0005..0x000A) -> True, new types (0x0010/0x0011) -> False.

## dap_chain_datum_token_read
- What to test: returns `(token_capsule|None, token_size_out)`.
- Edge cases: non-bytes input; too short bytes -> `(None, 0)`.
- Test ideas: `token_serial_short` returns `(None, 0)`; integration-only: use real token bytes
  and assert capsule + size >= input size.

## dap_chain_datum_token_tsd_get
- What to test: returns `dap_tsd_t` capsule or None.
- Edge cases: invalid token capsule; negative token_size.
- Test ideas: negative size raises ValueError; invalid capsule raises TypeError.
  Integration-only: get token via `dap_chain_datum_token_read`, pass size, expect capsule (even if no TSD data).

## dap_chain_emission_get_tsd
- What to test: returns `(data_bytes|None, size)`.
- Edge cases: invalid emission capsule; TSD not found -> `(None, 0)`.
- Test ideas: after `dap_chain_datum_emission_add_tsd`, read back and verify bytes match;
  missing TSD returns `(None, 0)`.

---

## Part 4: Transaction core, creation, signing

## dap_chain_datum_tx_add_event_item
- What to test: accepts tx capsule, `dap_pkey_t` capsule, srv_uid, group name, event_type, event_data; returns int.
- Edge cases: invalid tx/pkey capsule; group not str/bytes; event_data not bytes-like/None; event_type out of range.
- Test ideas: integration-only: add event with `pkey_capsule`, `srv_uid`, `event_group`, `event_type`,
  `event_data_bytes` and assert int rc; `event_data` None accepted; invalid types raise TypeError/ValueError.

## dap_chain_datum_tx_add_fee_item
- What to test: accepts tx capsule and uint256 value (bytes/str/int/capsule); returns int.
- Edge cases: invalid tx capsule; uint256 wrong length or None.
- Test ideas: add fee with `uint256_bytes`, then `dap_chain_datum_tx_get_fee_value` returns `rc == 0`;
  invalid value length raises ValueError.

## dap_chain_datum_tx_add_in_cond_item
- What to test: accepts tx capsule, hash (bytes/str/capsule), prev_out_idx, receipt_idx; returns int.
- Edge cases: invalid hash type/length; negative receipt_idx; invalid tx capsule.
- Test ideas: call with `hash_fast_bytes`, prev idx 0, receipt_idx 0 and assert int rc; invalid hash raises ValueError.

## dap_chain_datum_tx_add_in_cond_item_list
- What to test: accepts tx capsule and `dap_list_t` capsule; returns uint256 bytes.
- Edge cases: invalid list capsule; empty list.
- Test ideas: integration-only: build a list of `tx_in_cond` items, call and assert length == 32 bytes;
  invalid capsule raises TypeError.

## dap_chain_datum_tx_add_in_item
- What to test: accepts tx capsule, hash (bytes/str/capsule), prev_out_idx; returns int.
- Edge cases: invalid hash type/length; prev_out_idx out of range.
- Test ideas: call with `hash_fast_bytes`, prev idx 0 and assert int rc; invalid hash raises ValueError.

## dap_chain_datum_tx_add_in_item_list
- What to test: accepts tx capsule and `dap_list_t` capsule; returns uint256 bytes.
- Edge cases: invalid list capsule; empty list.
- Test ideas: integration-only: build a list of `tx_in` items and assert result length == 32 bytes;
  invalid capsule raises TypeError.

## dap_chain_datum_tx_add_in_reward_item
- What to test: accepts tx capsule and hash (bytes/str/capsule); returns int.
- Edge cases: invalid hash type/length; invalid tx capsule.
- Test ideas: call with `hash_fast_bytes` and assert int rc; invalid hash raises ValueError.

## dap_chain_datum_tx_add_item
- What to test: accepts raw item bytes or item capsule; returns int.
- Edge cases: non-bytes and non-capsule item; invalid capsule pointer.
- Test ideas: integration-only: add a real item bytes (from Part 5 helpers) and assert rc; validation-only:
  int or list input raises TypeError.

## dap_chain_datum_tx_add_out_cond_item
- What to test: accepts tx capsule, key_hash, srv_uid, value, value_max, unit_enm, optional cond bytes; returns int.
- Edge cases: invalid hash/value types; unit_enm not int; cond not bytes-like/None.
- Test ideas: call with `hash_fast_bytes`, `srv_uid`, `uint256_bytes`, `unit_enm`, `cond_bytes` and `None`;
  invalid cond type raises TypeError.

## dap_chain_datum_tx_add_out_ext_item
- What to test: accepts tx capsule, address (bytes/str/capsule), value, ticker; returns int.
- Edge cases: invalid address length/type; ticker not str/bytes; value invalid length.
- Test ideas: use `addr_capsule`, `uint256_bytes`, `token_ticker`; invalid ticker raises TypeError.

## dap_chain_datum_tx_add_out_item
- What to test: accepts tx capsule, address (bytes/str/capsule), value; returns int.
- Edge cases: invalid address length/type; value invalid length/type.
- Test ideas: use `addr_capsule` and `uint256_bytes`; invalid address length raises ValueError.

## dap_chain_datum_tx_add_out_std_item
- What to test: accepts tx capsule, address, value, ticker, ts_unlock; returns int.
- Edge cases: invalid ts_unlock type; ticker not str/bytes; address/value invalid.
- Test ideas: use `addr_capsule`, `uint256_bytes`, `token_ticker`, `0`; non-int ts_unlock raises TypeError.

## dap_chain_datum_tx_add_sign
- What to test: accepts tx capsule and `dap_sign_t` capsule; returns int; covers create.h export too.
- Edge cases: invalid sign capsule; invalid tx capsule.
- Test ideas: integration-only: add sign and then `dap_chain_datum_tx_get_sign(0)` returns capsule;
  invalid sign raises TypeError/ValueError.

## dap_chain_datum_tx_add_sign_item
- What to test: accepts tx capsule and `dap_enc_key_t` capsule; returns int.
- Edge cases: invalid key capsule; invalid tx capsule.
- Test ideas: integration-only: add sign item with `enc_key_capsule`; then verify signs or sign data;
  invalid key raises TypeError/ValueError.

## dap_chain_datum_tx_create
- What to test: creates tx capsule; returns `dap_chain_datum_tx_t`.
- Edge cases: allocation failure (should raise CellframeNetworkError).
- Test ideas: create tx, assert capsule; call `dap_chain_datum_tx_get_size` and expect `> 0`.

## dap_chain_datum_tx_delete
- What to test: deletes tx capsule; returns None.
- Edge cases: invalid capsule type.
- Test ideas: create tx, delete, and avoid reuse; invalid type raises TypeError/ValueError.

## dap_chain_datum_tx_get_fee_value
- What to test: returns `(rc, uint256_bytes|None)`; rc==0 when fee item exists.
- Edge cases: invalid tx capsule; no fee item.
- Test ideas: new tx -> rc != 0 and None; after `dap_chain_datum_tx_add_fee_item` -> rc == 0 and len == 32.

## dap_chain_datum_tx_get_sign
- What to test: returns `dap_sign_t` capsule or None by index.
- Edge cases: sign_num out of range; negative sign_num.
- Test ideas: after adding a sign, `sign_num=0` returns capsule; large index returns None.

## dap_chain_datum_tx_get_sign_data
- What to test: returns signing bytes or None; covers create.h export too.
- Edge cases: invalid tx capsule; tx with no signing data.
- Test ideas: empty tx -> None; after adding items/signs -> bytes with `len > 0`.

## dap_chain_datum_tx_get_size
- What to test: returns size as int.
- Edge cases: invalid tx capsule.
- Test ideas: size increases after adding an item (compare before/after).

## dap_chain_datum_tx_group_items
- What to test: returns `dap_chain_datum_tx_item_groups_t` capsule or None.
- Edge cases: empty tx -> None; invalid tx capsule.
- Test ideas: with no items expect None; after adding items expect groups capsule.

## dap_chain_datum_tx_group_items_free
- What to test: frees groups capsule; returns None.
- Edge cases: invalid groups capsule.
- Test ideas: call after `dap_chain_datum_tx_group_items`; invalid capsule raises TypeError/ValueError.

## dap_chain_datum_tx_item_get
- What to test: returns `(item_capsule|None, item_size, item_idx)` for iterator search.
- Edge cases: invalid iter capsule; item_type not int; negative start index.
- Test ideas: empty tx -> item None and size 0; after adding items, iterate by passing None
  then passing returned item capsule as iter; ensure item_idx increases.

## dap_chain_datum_tx_item_get_nth
- What to test: returns item capsule or None for nth item.
- Edge cases: item_idx out of range; invalid item_type.
- Test ideas: after adding outputs, `item_idx=0` returns capsule; out-of-range returns None.

## dap_chain_datum_tx_items_get
- What to test: returns `(dap_list_t capsule|None, count)`.
- Edge cases: invalid item_type; empty tx -> None and count 0.
- Test ideas: after adding items, count > 0 and capsule type is `dap_list_t`.

## dap_chain_datum_tx_out_cond_get
- What to test: returns `(dap_chain_tx_out_cond_t capsule|None, out_num)`.
- Edge cases: invalid cond_subtype; out_num negative; no conditional outputs.
- Test ideas: empty tx -> None and out_num unchanged or -1; after adding out_cond item, returns capsule
  and out_num updated.

## dap_chain_datum_tx_verify_sign
- What to test: returns int; with sign_num omitted or negative, verifies all.
- Edge cases: sign_num out of range; tx without signs.
- Test ideas: unsigned tx -> rc != 0; after signing -> rc == 0; verify specific sign with sign_num.

## dap_chain_datum_tx_verify_sign_all
- What to test: returns int for full signature verification.
- Edge cases: tx without signs.
- Test ideas: unsigned tx -> rc != 0; signed tx -> rc == 0.

## dap_chain_node_datum_tx_calc_hash
- What to test: returns hash bytes of correct size.
- Edge cases: invalid tx capsule.
- Test ideas: hash length == HASH_SIZE; hash changes after adding item (integration).

## dap_chain_datum_tx_create_cond_output
- What to test: creates conditional output tx; returns tx capsule.
- Edge cases: invalid pkey/addr/hash; value/fee invalid; cond not bytes-like/None; unit_enm not int.
- Test ideas: integration-only: use real keys and address, call with `cond_bytes` and None; invalid args raise errors.

## dap_chain_datum_tx_create_event
- What to test: creates event tx; returns tx capsule.
- Edge cases: invalid pkey capsules; group not str/bytes; event_data not bytes-like/None; fee invalid.
- Test ideas: integration-only: use `pkey_capsule` and `pkey_service_capsule`, `event_group`,
  `event_type`, `event_data_bytes` or None; invalid args raise TypeError/ValueError.

## dap_chain_datum_tx_create_from_emission
- What to test: creates tx from emission; returns tx capsule.
- Edge cases: emission hash invalid; chain_id invalid; emission_value invalid; addr invalid; ticker invalid.
- Test ideas: integration-only: create emission and use its hash; invalid hash length raises ValueError.

## dap_chain_datum_tx_create_multi_transfer
- What to test: creates multi-transfer tx from address/value lists; optional time_unlock list.
- Edge cases: empty addr_list; values length mismatch; time_unlocks length mismatch; invalid address/value element.
- Test ideas: integration-only: use `addr_list` + `values_list` and optional `time_unlocks`; mismatched sizes raise ValueError.

## dap_chain_datum_tx_create_transfer
- What to test: creates transfer tx; returns tx capsule.
- Edge cases: invalid pkey; invalid addresses; ticker not str/bytes; value/fee invalid.
- Test ideas: integration-only: call with `pkey_capsule`, addresses, `token_ticker`, `uint256_bytes`, `uint256_bytes`;
  invalid args raise TypeError/ValueError.

## dap_chain_tx_delete
- What to test: deletes `dap_chain_tx_t` capsule; returns None.
- Edge cases: invalid tx capsule.
- Test ideas: create `tx_wrapped_capsule`, delete, ensure no exception; invalid type raises TypeError/ValueError.

## dap_chain_tx_dup
- What to test: duplicates `dap_chain_tx_t`; returns new capsule or None.
- Edge cases: invalid tx capsule.
- Test ideas: wrap a packed tx, dup it, and assert duplicate capsule is returned; original delete should not break dup.

## dap_chain_tx_hh_add
- What to test: adds tx to hashtable; updates head pointer; returns None.
- Edge cases: invalid capsule types; head not initialized.
- Test ideas: integration-only: add a second tx to head and then verify `dap_chain_tx_hh_find`;
  invalid types raise TypeError/ValueError.

## dap_chain_tx_hh_find
- What to test: finds tx by hash; returns tx capsule or None.
- Edge cases: invalid hash type/length; empty table.
- Test ideas: integration-only: compute hash for a known tx and assert it is found; invalid hash raises ValueError.

## dap_chain_tx_hh_free
- What to test: frees hashtable; returns None.
- Edge cases: invalid tx_hh capsule.
- Test ideas: integration-only: free after hh_add; invalid capsule raises TypeError/ValueError.

## dap_chain_tx_wrap_packed
- What to test: wraps packed tx into `dap_chain_tx_t`; returns capsule or None.
- Edge cases: invalid tx capsule.
- Test ideas: call with `tx_capsule` and assert capsule returned; invalid type raises TypeError/ValueError.

## dap_chain_tx_get_signing_data
- What to test: returns signing bytes or None for tx_sign flow.
- Edge cases: invalid tx capsule; empty tx has no signing data.
- Test ideas: empty tx -> None; after adding items -> bytes with `len > 0`.

## dap_chain_tx_sign_add
- What to test: adds precomputed sign to tx; returns int.
- Edge cases: invalid sign capsule; invalid tx capsule.
- Test ideas: integration-only: add sign and then verify with `dap_chain_datum_tx_verify_sign_all`;
  invalid sign raises TypeError/ValueError.

---

## Part 5: Transaction items, events, out_cond

## dap_chain_datum_tx_event_to_json
- What to test: accepts json capsule, tx event capsule, hash_out_type; returns int rc.
- Edge cases: invalid json/event capsule; hash_out_type not str/bytes.
- Test ideas: integration-only: obtain `dap_chain_tx_event_t` via ledger event API, call with `json_obj`
  and `"hex"`, assert `rc == 0` and JSON has expected keys; invalid args raise TypeError/ValueError.

## dap_chain_datum_tx_item_event_to_json
- What to test: accepts json capsule and tx item event capsule; returns int rc.
- Edge cases: invalid json/event capsule.
- Test ideas: create event item via `dap_chain_datum_tx_event_create`, call and assert `rc == 0`
  and JSON fields (`timestamp`, `srv_uid`, `event_type`, `event_version`, `event_group`).

## dap_chain_tx_item_event_type_from_str
- What to test: converts event type string to int.
- Edge cases: unknown string -> `-1`; non-str/bytes -> TypeError.
- Test ideas: `"stake_ext_started"` returns non-negative; `"unknown"` returns `-1`.

## dap_chain_tx_item_event_type_to_str
- What to test: converts event type int to string.
- Edge cases: unknown value -> `"unknown"`; non-int -> TypeError.
- Test ideas: pass known constant and assert string; `0xffff` -> `"unknown"`.

## dap_chain_datum_item_tx_get_size
- What to test: returns size for item capsule/bytes; honors `max_size` when provided.
- Edge cases: non-bytes and non-capsule item; invalid capsule pointer; negative max_size.
- Test ideas: create an item capsule (e.g., out or tsd) and assert size > 0; call with `max_size=1`
  and assert size == 0; invalid item type raises TypeError/ValueError.

## dap_chain_datum_tx_event_create
- What to test: creates `dap_chain_tx_item_event_t` capsule from srv_uid, group, type, timestamp.
- Edge cases: group not str/bytes; group too long; srv_uid/timestamp not int.
- Test ideas: use `srv_uid`, `event_group`, `event_type`, `event_timestamp` -> capsule; invalid group
  type raises TypeError; extremely long group returns None (integration-only).

## dap_chain_datum_tx_item_get_data
- What to test: returns `(bytes|None, type, size)` from `dap_chain_tx_tsd_t`.
- Edge cases: invalid capsule; tsd with empty data.
- Test ideas: create tsd via `dap_chain_datum_tx_item_tsd_create(tx_item_tsd_data, tx_item_tsd_type)`,
  call and assert data bytes and size; invalid capsule raises TypeError.

## dap_chain_datum_tx_item_get_tsd_by_type
- What to test: returns `dap_chain_tx_tsd_t` capsule or None by type.
- Edge cases: invalid tx capsule; type not int; no matching TSD.
- Test ideas: integration-only: add TSD item to tx using `dap_chain_datum_tx_add_item`, then fetch by type;
  empty tx returns None; invalid tx raises TypeError.

## dap_chain_datum_tx_item_in_cond_create
- What to test: creates `dap_chain_tx_in_cond_t` capsule from prev hash and indices.
- Edge cases: invalid hash type/length; negative indices (overflow error).
- Test ideas: use `hash_fast_bytes`, `prev_out_idx=0`, `receipt_idx=0` -> capsule; invalid hash length
  raises ValueError.

## dap_chain_datum_tx_item_in_create
- What to test: creates `dap_chain_tx_in_t` capsule from prev hash and index.
- Edge cases: invalid hash type/length; negative index (overflow error).
- Test ideas: use `hash_fast_bytes`, `prev_out_idx=0` -> capsule; invalid hash raises ValueError.

## dap_chain_datum_tx_item_in_ems_create
- What to test: creates `dap_chain_tx_in_ems_t` capsule from chain_id, token hash, ticker.
- Edge cases: invalid hash length; ticker not str/bytes; chain_id not int.
- Test ideas: `chain_id=0`, `hash_fast_bytes`, `token_ticker` -> capsule; invalid ticker type raises TypeError.

## dap_chain_datum_tx_item_in_reward_create
- What to test: creates `dap_chain_tx_in_reward_t` capsule from block hash.
- Edge cases: invalid hash type/length.
- Test ideas: `hash_fast_bytes` -> capsule; invalid hash length raises ValueError.

## dap_chain_datum_tx_item_out_cond_create_fee
- What to test: creates fee out_cond item; returns capsule or None.
- Edge cases: zero value -> None; invalid uint256 type/size.
- Test ideas: `uint256_bytes` -> capsule; zero value returns None; invalid length raises ValueError.

## dap_chain_datum_tx_item_out_cond_create_srv_pay
- What to test: creates srv_pay out_cond with pkey, srv_uid, value, value_max, unit, params.
- Edge cases: invalid pkey capsule; value/value_max invalid; unit not int; params not bytes-like/None.
- Test ideas: integration-only: real `pkey_capsule` with params None and bytes; invalid params type
  raises TypeError.

## dap_chain_datum_tx_item_out_cond_create_srv_pay_with_hash
- What to test: creates srv_pay out_cond from pkey hash.
- Edge cases: invalid hash type/length; value zero -> None; params not bytes-like/None.
- Test ideas: `hash_fast_bytes`, `uint256_bytes`, `unit_enm`, params None/bytes -> capsule;
  zero value returns None.

## dap_chain_datum_tx_item_out_cond_create_srv_stake
- What to test: creates stake out_cond with signing addr, node addr, sovereign addr/tax, pkey.
- Edge cases: invalid address types; invalid node addr; pkey not capsule; value zero -> None.
- Test ideas: integration-only: use `addr_capsule`, `node_addr_int`, `uint256_bytes`, sovereign addr None,
  pkey None; invalid addr type raises TypeError.

## dap_chain_datum_tx_item_out_cond_create_srv_stake_delegate
- What to test: creates stake delegate out_cond with params.
- Edge cases: invalid addr/node addr; params not bytes-like/None; value zero -> None.
- Test ideas: integration-only: use real addresses and params None/bytes; invalid params type raises TypeError.

## dap_chain_datum_tx_item_out_cond_create_srv_stake_ext_lock
- What to test: creates stake ext lock out_cond with hash, lock_time, position_id, params.
- Edge cases: invalid hash type/length; value zero -> None; params not bytes-like/None.
- Test ideas: `hash_fast_bytes`, `uint256_bytes`, `stake_lock_time`, `position_id`, params None/bytes -> capsule;
  invalid params type raises TypeError.

## dap_chain_datum_tx_item_out_cond_create_srv_stake_get_tsd_size
- What to test: returns computed TSD size for stake out_cond.
- Edge cases: negative pkey_size (overflow error); non-bool `has_sovereign_addr`.
- Test ideas: `(False, 0)` -> `0`; `(True, 0)` -> `> 0`; `(False, 33)` -> `> 0`; size increases with
  larger `pkey_size`.

## dap_chain_datum_tx_item_out_cond_create_srv_stake_lock
- What to test: creates stake lock out_cond; returns capsule or None.
- Edge cases: value zero -> None; reinvest_percent invalid type/size.
- Test ideas: `uint256_bytes` for value and reinvest, `stake_lock_time` -> capsule; invalid reinvest
  raises ValueError.

## dap_chain_datum_tx_item_out_cond_create_srv_xchange
- What to test: creates xchange out_cond with net ids, token, value_rate, seller addr, params.
- Edge cases: token not str/bytes; value_sell/value_rate zero -> None; invalid seller addr; params invalid.
- Test ideas: integration-only: use `addr_capsule` and params None/bytes; invalid token type raises TypeError.

## dap_chain_datum_tx_item_out_cond_create_wallet_shared
- What to test: creates wallet_shared out_cond with hashes list and tag.
- Edge cases: hashes not list/tuple; hash elements invalid length; tag not str/bytes/None.
- Test ideas: empty `hashes_list` with `tag=None` -> capsule; list with two hashes -> capsule; invalid hash
  length raises ValueError; invalid tag type raises TypeError.

## dap_chain_datum_tx_item_out_create
- What to test: creates out item; returns capsule or None.
- Edge cases: invalid address; value zero -> None; value invalid type/size.
- Test ideas: `addr_capsule` + `uint256_bytes` -> capsule; invalid addr type raises TypeError.

## dap_chain_datum_tx_item_out_ext_create
- What to test: creates out_ext item with token.
- Edge cases: token not str/bytes; invalid address; value zero -> None.
- Test ideas: `addr_capsule`, `uint256_bytes`, `token_ticker` -> capsule; invalid token type raises TypeError.

## dap_chain_datum_tx_item_out_std_create
- What to test: creates out_std item with unlock time.
- Edge cases: ts_unlock not int; token not str/bytes; value zero -> None.
- Test ideas: `addr_capsule`, `uint256_bytes`, `token_ticker`, `ts_unlock=0` -> capsule; invalid ts_unlock
  raises TypeError.

## dap_chain_datum_tx_item_sig_get_sign
- What to test: returns `dap_sign_t` capsule or None from tx sig item.
- Edge cases: invalid sig capsule; sig with invalid size.
- Test ideas: integration-only: create sign and tx sig, then call and assert sign capsule; invalid capsule
  raises TypeError.

## dap_chain_datum_tx_item_sign_create
- What to test: creates tx sig item from enc_key and tx.
- Edge cases: invalid key capsule; invalid tx capsule.
- Test ideas: integration-only: create tx with items, sign, assert sig capsule; invalid key raises TypeError.

## dap_chain_datum_tx_item_sign_create_from_sign
- What to test: creates tx sig item from `dap_sign_t`.
- Edge cases: invalid sign capsule.
- Test ideas: integration-only: create sign via `dap_chain_datum_tx_sign_create` and wrap; invalid sign raises TypeError.

## dap_chain_datum_tx_item_tsd_create
- What to test: creates tsd item from bytes and type.
- Edge cases: data not bytes-like; empty data -> ValueError.
- Test ideas: `tx_item_tsd_data`, `tx_item_tsd_type` -> capsule; empty bytes raises ValueError.

## dap_chain_datum_tx_item_type_from_str_short
- What to test: converts short type string to enum.
- Edge cases: unknown string -> `TX_ITEM_TYPE_UNKNOWN`; non-str/bytes -> TypeError.
- Test ideas: `"out"` -> `TX_ITEM_TYPE_OUT`; `"unknown"` -> `TX_ITEM_TYPE_UNKNOWN`.

## dap_chain_datum_tx_item_type_to_str
- What to test: converts type enum to full string.
- Edge cases: unknown type -> `"UNDEFINED"`; non-int -> TypeError.
- Test ideas: `TX_ITEM_TYPE_OUT` -> `"TX_ITEM_TYPE_OUT"`; unknown -> `"UNDEFINED"`.

## dap_chain_datum_tx_item_type_to_str_short
- What to test: converts type enum to short string.
- Edge cases: unknown type -> `"UNDEFINED"`; non-int -> TypeError.
- Test ideas: `TX_ITEM_TYPE_OUT` -> `"out"`; unknown -> `"UNDEFINED"`.

## dap_chain_datum_tx_sign_create
- What to test: creates `dap_sign_t` from enc_key and tx.
- Edge cases: invalid key capsule; invalid tx capsule.
- Test ideas: integration-only: create tx with items and sign; invalid key raises TypeError.

## dap_chain_tx_event_copy
- What to test: returns copied `dap_chain_tx_event_t` capsule.
- Edge cases: invalid event capsule.
- Test ideas: integration-only: obtain real event, copy, and compare via `dap_chain_datum_tx_event_to_json`;
  invalid capsule raises TypeError.

## dap_chain_tx_event_delete
- What to test: frees `dap_chain_tx_event_t`; returns None.
- Edge cases: invalid event capsule.
- Test ideas: integration-only: delete real event and ensure no crash; invalid capsule raises TypeError.

## dap_chain_tx_out_cond_subtype_from_str_short
- What to test: converts short subtype string to enum.
- Edge cases: unknown string -> `DAP_CHAIN_TX_OUT_COND_SUBTYPE_UNDEFINED`; non-str/bytes -> TypeError.
- Test ideas: `"srv_pay"` -> subtype enum; `"unknown"` -> undefined.

## dap_chain_tx_sig_create
- What to test: creates tx sig item from `dap_sign_t`.
- Edge cases: invalid sign capsule.
- Test ideas: integration-only: create sign and call; invalid sign raises TypeError.

## dap_chain_tx_out_cond_subtype_to_str
- What to test: converts subtype enum to full string.
- Edge cases: unknown subtype -> `"UNDEFINED"`; non-int -> TypeError.
- Test ideas: `DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY` -> full string; unknown -> `"UNDEFINED"`.

## dap_chain_tx_out_cond_subtype_to_str_short
- What to test: converts subtype enum to short string.
- Edge cases: unknown subtype -> `"UNDEFINED"`; non-int -> TypeError.
- Test ideas: `DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY` -> `"srv_pay"`; unknown -> `"UNDEFINED"`.

---

## Part 6: Transaction receipts + voting

## dap_chain_datum_tx_receipt_check_size
- What to test: returns int rc for `(receipt_capsule, control_size)`.
- Edge cases: invalid receipt capsule; negative control_size; size mismatch.
- Test ideas: create receipt, read `receipt->size` via `ctypes` from capsule and expect rc == 0;
  call with `size - 1` and expect rc != 0; invalid capsule raises TypeError.

## dap_chain_datum_tx_receipt_create
- What to test: creates receipt capsule from srv_uid/unit/units/value/ext/prev_tx_hash.
- Edge cases: invalid hash (wrong size/format); invalid uint256 value; ext not bytes-like.
- Test ideas: call with `receipt_ext_bytes` and `hash_fast_bytes`, expect capsule;
  call with `ext=None` and expect capsule; invalid hash bytes length raises ValueError.

## dap_chain_datum_tx_receipt_sign_add
- What to test: returns updated receipt capsule after adding signature.
- Edge cases: invalid receipt capsule; invalid key capsule; key not suitable -> None.
- Test ideas: integration-only: create receipt and add sign with real `enc_key_capsule`,
  then `dap_chain_datum_tx_receipt_signs_count` increments; invalid types raise TypeError.

## dap_chain_datum_tx_receipt_sign_get
- What to test: returns `dap_sign_t` capsule for given position and size.
- Edge cases: invalid receipt capsule; negative size; out-of-range position; wrong size -> None.
- Test ideas: integration-only: create receipt, sign once, use `receipt->size` and position `0`
  to get sign capsule; position `1` returns None; invalid size returns None.

## dap_chain_datum_tx_receipt_signs_count
- What to test: returns count as int.
- Edge cases: invalid receipt capsule.
- Test ideas: new receipt returns `0`; after `sign_add` returns `1`.

## dap_chain_datum_tx_receipt_srv_uid_get
- What to test: returns service uid as uint64.
- Edge cases: invalid receipt capsule.
- Test ideas: create receipt with `srv_uid` and assert value matches.

## dap_chain_datum_tx_receipt_units_get
- What to test: returns units as uint64.
- Edge cases: invalid receipt capsule.
- Test ideas: create receipt with `units` and assert value matches.

## dap_chain_datum_tx_receipt_utype_get
- What to test: returns unit type enum as uint32.
- Edge cases: invalid receipt capsule.
- Test ideas: create receipt with `unit_enm` and assert value matches.

## dap_chain_datum_tx_receipt_value_get
- What to test: returns uint256 bytes.
- Edge cases: invalid receipt capsule.
- Test ideas: create receipt with `uint256_bytes`, assert `len == sizeof(uint256_t)` and bytes equal.

## dap_chain_datum_tx_item_vote_create
- What to test: creates vote item capsule from voting hash + answer idx.
- Edge cases: invalid hash type/size; None hash.
- Test ideas: call with `hash_fast_bytes` + `vote_answer_idx`, expect capsule; invalid hash length raises ValueError.

## dap_chain_datum_tx_item_vote_to_json
- What to test: returns `dap_json_t` capsule for vote item.
- Edge cases: invalid vote capsule; invalid version type.
- Test ideas: create vote item and call with version `1` and `2`, assert capsule; invalid type raises TypeError.

## dap_chain_datum_tx_item_voting_create
- What to test: returns voting item capsule with no args.
- Edge cases: extra args.
- Test ideas: call without args, expect capsule; call with arg -> TypeError.

## dap_chain_datum_tx_item_voting_tsd_to_json
- What to test: returns JSON capsule from tx TSD items for voting.
- Edge cases: invalid tx capsule; invalid version type.
- Test ideas: integration-only: create tx, add question/option TSD items, call and assert JSON capsule;
  tx without voting TSDs returns JSON with empty answer list.

## dap_chain_datum_tx_voting_get_answer_text_by_idx
- What to test: returns answer text by index or None.
- Edge cases: invalid tx capsule; out-of-range index.
- Test ideas: integration-only: create tx with two option TSDs and assert idx `0/1` returns expected string;
  idx `2` returns None.

## dap_chain_datum_tx_voting_params_delete
- What to test: frees params capsule; returns None.
- Edge cases: invalid capsule type.
- Test ideas: parse params from tx, delete, ensure no crash; invalid capsule raises TypeError.

## dap_chain_datum_tx_voting_parse_tsd
- What to test: returns voting params capsule or None.
- Edge cases: invalid tx capsule; tx without voting TSD items.
- Test ideas: tx without voting TSD -> None; tx with question/option/expire returns capsule.
  Optional: use `ctypes` to inspect fields (question/options/expire) in the params struct.

## dap_chain_datum_voting_answer_tsd_create
- What to test: creates answer TSD from string.
- Edge cases: empty string; non-str/bytes.
- Test ideas: `"Option A"` returns capsule; `""`/`b""` raises ValueError.

## dap_chain_datum_voting_cancel_tsd_create
- What to test: creates cancel TSD from voting hash.
- Edge cases: invalid hash size/type.
- Test ideas: valid `hash_fast_bytes` returns capsule; invalid length raises ValueError.

## dap_chain_datum_voting_delegated_key_required_tsd_create
- What to test: creates delegated-key-required TSD.
- Edge cases: non-bool input (truthy/falsy ints).
- Test ideas: True/False return capsule; int `1/0` also accepted.

## dap_chain_datum_voting_expire_tsd_create
- What to test: creates expire TSD from dap_time_t.
- Edge cases: expire == 0 returns None.
- Test ideas: expire `0` -> None; expire `voting_expire` -> capsule.

## dap_chain_datum_voting_max_votes_count_tsd_create
- What to test: creates max-votes-count TSD.
- Edge cases: max_count == 0 returns None.
- Test ideas: `0` -> None; `voting_max_votes` -> capsule.

## dap_chain_datum_voting_question_tsd_create
- What to test: creates question TSD from string.
- Edge cases: empty string; non-str/bytes.
- Test ideas: `voting_question` returns capsule; `""`/`b""` raises ValueError.

## dap_chain_datum_voting_token_tsd_create
- What to test: creates token ticker TSD.
- Edge cases: empty ticker; ticker too long; invalid type.
- Test ideas: `token_ticker` returns capsule; empty string returns None; too-long string returns None.

## dap_chain_datum_voting_vote_changing_allowed_tsd_create
- What to test: creates vote-changing-allowed TSD.
- Edge cases: non-bool input (truthy/falsy ints).
- Test ideas: True/False return capsule.

## dap_chain_datum_voting_vote_tx_cond_tsd_create
- What to test: creates vote-tx-cond TSD from tx hash + out_idx.
- Edge cases: invalid hash size/type; negative out_idx (allowed but should not crash).
- Test ideas: valid hash + `voting_tx_cond_out_idx` returns capsule; invalid hash length raises ValueError.

---

## Part 7: Transaction compose registry

## dap_chain_tx_compose_init
- What to test: returns int rc, initializes registry state.
- Edge cases: extra args; repeated init calls.
- Test ideas: call once and assert int result; call twice and ensure no exception.

## dap_chain_tx_compose_deinit
- What to test: returns None, clears registry and python callbacks.
- Edge cases: deinit without init; repeated deinit calls; extra args.
- Test ideas: register builder, deinit, assert `dap_chain_tx_compose_is_registered` is False;
  call deinit twice to ensure no crash.

## dap_chain_tx_compose_register
- What to test: registers python callback and returns rc (0 on success).
- Edge cases: non-callable callback; tx_type not str/bytes; missing args; duplicate tx_type replaces.
- Test ideas: register `compose_builder_none`, assert rc == 0 and `is_registered` True;
  re-register same tx_type with another callback and still registered; invalid callback raises TypeError.

## dap_chain_tx_compose_unregister
- What to test: removes registration; returns None.
- Edge cases: unknown tx_type; tx_type not str/bytes.
- Test ideas: register then unregister and assert `is_registered` False; unregister unknown tx_type no error.

## dap_chain_tx_compose_is_registered
- What to test: returns bool.
- Edge cases: tx_type not str/bytes.
- Test ideas: False before register, True after, False after unregister; invalid type raises TypeError.

## dap_chain_tx_compose_create
- What to test: dispatches to registered builder; returns datum capsule or None.
- Edge cases: missing args; ledger/utxo not capsules; invalid capsule names; unregistered tx_type;
  builder returns None.
- Test ideas: register `compose_builder_ok`, call with `dummy_ledger_capsule`,
  `dummy_utxo_capsule`, `compose_params`, assert returned capsule name is "dap_chain_datum_t";
  register `compose_builder_none` and expect None; unregistered tx_type returns None;
  invalid ledger/utxo capsules raise TypeError/ValueError.

## dap_chain_tx_compose_registry_init
- What to test: returns int rc for registry init.
- Edge cases: extra args; repeated init calls.
- Test ideas: call once and assert int result; call twice to ensure no exception.

## dap_chain_tx_compose_registry_deinit
- What to test: returns None; clears registry and python callbacks.
- Edge cases: deinit without init; repeated deinit calls; extra args.
- Test ideas: add entry, deinit, `registry_find` returns None; deinit twice no crash.

## dap_chain_tx_compose_registry_add
- What to test: adds entry with python callback; returns rc (0 on success).
- Edge cases: non-callable callback; tx_type not str/bytes; missing args; duplicate tx_type replaces.
- Test ideas: add with `compose_builder_none`, assert rc == 0 and `registry_find` returns capsule;
  re-add same tx_type and still findable; invalid callback raises TypeError.

## dap_chain_tx_compose_registry_remove
- What to test: removes entry; returns None.
- Edge cases: unknown tx_type; tx_type not str/bytes.
- Test ideas: add then remove and assert `registry_find` returns None; remove unknown tx_type no error.

## dap_chain_tx_compose_registry_find
- What to test: returns registry entry capsule or None.
- Edge cases: tx_type not str/bytes.
- Test ideas: find before add returns None; after add returns capsule with name
  "dap_chain_tx_compose_registry_entry_t" (use `ctypes.pythonapi.PyCapsule_GetName` to assert).
