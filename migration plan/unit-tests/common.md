# Unit Test Plan: common

This document describes unit test coverage for the common module wrappers.
Focus is wrapper behavior (argument validation, return types) and basic smoke
checks; integration-style checks are noted where SDK state is required.

## Common setup and fixtures

- `sdk_available`: skip tests if `python_cellframe.is_sdk_available()` is false.
- `init_sdk`: call `python_cellframe.initialize()` once per session (same pattern as chain tests).
- `hash_fast_bytes`: `b"\x00" * HASH_SIZE` (match `sizeof(dap_hash_fast_t)`; confirm size in SDK).
- `hash_slow_bytes`: `b"\x00" * 32` (match `DAP_CHAIN_HASH_SLOW_SIZE`).
- `addr_null`: use `cf.dap_chain_addr_from_str("null")` to get a blank address capsule.
- `addr_filled`: use `cf.dap_chain_addr_fill(sign_type, hash_fast_bytes, net_id)` to get a non-blank address.
- `uint256_bytes`: use `cf.dap_chain_uint256_from(0)` to get correct size bytes.
- `uint128_bytes`: use `cf.dap_chain_uint128_from(0)` to get correct size bytes.
- `dummy_capsule(name)`: helper using `ctypes.pythonapi.PyCapsule_New` (as in chain tests) for
  `dap_chain_t`, `dap_chain_net_t`, `dap_chain_datum_t`, `dap_ledger_t` when pointer is not dereferenced.
- `json_array`: `dap.json.DapJson.array()`; use `json_array._cap` when a `dap_json_t` capsule is needed.

Note: For net API functions that require a real network/chain/ledger instance,
add a unit test for type validation and a follow-up integration test after
net module init registers its API.

---

## dap_chain_atom_verify_res_to_str
- What to test: enum to string mapping for known values.
- Edge cases: unknown/unsupported value.
- Test ideas: assert `0 -> "accepted"`, `-1 -> "rejected"`, `1 -> "skipped"`, unknown -> "unknown".

## dap_chain_block_callbacks_init
- What to test: returns int (0 on success), idempotent call.
- Edge cases: call multiple times.
- Test ideas: call twice and assert int result each time.

## dap_chain_block_callbacks_deinit
- What to test: returns None, safe to call multiple times.
- Edge cases: call without prior init.
- Test ideas: call after init and again; assert no exception.

## dap_chain_block_callbacks_register_sovereign_tax
- What to test: registers Python callback; returns None.
- Edge cases: non-callable callback; user_data None.
- Test ideas: register callback that returns dict with `has_tax`, `sovereign_addr`, `sovereign_tax`.

## dap_chain_block_callbacks_get_sovereign_tax
- What to test: returns dict with `has_tax`, `sovereign_addr` capsule, `sovereign_tax` bytes.
- Edge cases: callback returns None/False; callback raises; invalid hash size/type.
- Test ideas: register callback returning `{has_tax: True, sovereign_addr: "null", sovereign_tax: "1"}`
  and assert fields; callback returns `None` -> function returns `None`.

## dap_chain_addr_check_sum
- What to test: returns int checksum code (0 for valid/blank address).
- Edge cases: invalid capsule; wrong bytes length.
- Test ideas: use `addr_null` and expect 0; use `addr_filled` and expect 0.

## dap_chain_addr_compare
- What to test: returns bool for equality of two addresses.
- Edge cases: invalid types for either arg.
- Test ideas: compare `addr_null` with itself -> True; compare `addr_null` and `addr_filled` -> False.

## dap_chain_addr_fill
- What to test: returns address capsule; checksum is valid.
- Edge cases: invalid hash type/length; non-int sign type.
- Test ideas: use `hash_fast_bytes` and a sign type (0 or from python-dap); check capsule and checksum 0.

## dap_chain_addr_fill_from_key
- What to test: returns tuple `(rc, addr_capsule_or_None)`.
- Edge cases: invalid key capsule; invalid net_id.
- Test ideas: generate key via `dap.crypto.DapKey.generate`, create capsule for `dap_enc_key_t`,
  call function; expect `rc == 0` and valid capsule.

## dap_chain_addr_fill_from_sign
- What to test: returns tuple `(rc, addr_capsule_or_None)`.
- Edge cases: invalid sign capsule; invalid net_id.
- Test ideas: sign data with `DapKey.sign` to get `dap_sign_t` handle, wrap into capsule,
  call function; expect `rc == 0` and valid capsule.

## dap_chain_addr_from_str
- What to test: returns address capsule for valid strings.
- Edge cases: invalid base58 string; empty string.
- Test ideas: `"null"` returns capsule; invalid string raises ValueError.

## dap_chain_addr_from_str_array
- What to test: returns list of address capsules for comma-separated string.
- Edge cases: invalid address in list; empty string.
- Test ideas: `"null,null"` -> list length 2; invalid element raises ValueError.

## dap_chain_addr_is_blank
- What to test: returns bool for blank address.
- Edge cases: invalid capsule.
- Test ideas: `addr_null` -> True; `addr_filled` -> False.

## dap_chain_addr_to_str_static_
- What to test: converts address to string.
- Edge cases: invalid capsule/bytes length.
- Test ideas: `addr_null` -> "null"; `addr_filled` -> non-empty base58 string.

## dap_chain_balance_scan
- What to test: returns uint256 bytes from string/number input.
- Edge cases: invalid string; None.
- Test ideas: parse "1" and "1.0"; ensure result length is 32 bytes; invalid input -> zero bytes or error.

## dap_chain_balance_to_coins_uint64
- What to test: converts uint256 to uint64 coins.
- Edge cases: invalid value type/length.
- Test ideas: `dap_chain_balance_scan("1.0")` -> 1; `dap_chain_balance_scan("0")` -> 0.

## dap_chain_hash_slow_kind_check
- What to test: returns enum kind for slow hash.
- Edge cases: wrong hash length/type.
- Test ideas: all zeros -> `HASH_GOLD`; all same non-zero -> `HASH_SILVER`; mixed -> `HASH_USELESS`.

## dap_chain_hash_slow_to_str
- What to test: converts slow hash to hex string with 0x prefix.
- Edge cases: wrong hash length/type.
- Test ideas: `hash_slow_bytes` -> string length 66; prefix "0x".

## dap_chain_hash_slow_to_str_new
- What to test: returns string (new allocation) or None.
- Edge cases: wrong hash length/type.
- Test ideas: `hash_slow_bytes` -> non-empty string; match output of `dap_chain_hash_slow_to_str`.

## dap_chain_mempool_group_name
- What to test: returns formatted group name string.
- Edge cases: empty prefix/chain name.
- Test ideas: prefix "gdb" and chain "main" -> "gdb.chain-main.mempool".

## dap_chain_node_role_to_str
- What to test: maps node role enum to string.
- Edge cases: unknown role.
- Test ideas: `0x00` -> "root master"; `0xff` -> "liht"; unknown -> "none".

## dap_chain_set_offset_limit_json
- What to test: updates JSON and returns new `(start, end)` tuple.
- Edge cases: invalid JSON capsule; limit/offset combinations.
- Test ideas: use `DapJson.array()` and call with limit/offset; assert tuple updates and
  JSON contains `limit`/`offset` entries (via JSON string or python-dap helpers).

## dap_chain_srv_str_to_unit_enum
- What to test: string to enum mapping.
- Edge cases: unknown string, None.
- Test ideas: "SEC" -> `SERV_UNIT_SEC`, "B" -> `SERV_UNIT_B`, unknown -> `SERV_UNIT_UNDEFINED`.

## dap_chain_srv_uid_from_str
- What to test: parses UID string to uint64.
- Edge cases: invalid string -> returns 0.
- Test ideas: "1" -> 1; "0x10" -> 16; "bad" -> 0.

## dap_chain_srv_unit_enum_to_str
- What to test: enum to string mapping.
- Edge cases: unknown enum.
- Test ideas: `SERV_UNIT_SEC` -> "SEC"; unknown -> "UNDEFINED".

## dap_chain_uint128_from
- What to test: returns uint128 bytes from uint64.
- Edge cases: negative/overflow input.
- Test ideas: input 0 -> all zero bytes; input 1 -> low bytes reflect 1.

## dap_chain_uint256_from
- What to test: returns uint256 bytes from uint64.
- Edge cases: negative/overflow input.
- Test ideas: input 0 -> zeros; input 1 -> low bytes reflect 1.

## dap_chain_uint256_from_uint128
- What to test: converts uint128 bytes to uint256 bytes.
- Edge cases: invalid bytes length/type.
- Test ideas: start from `dap_chain_uint128_from(42)`; convert to uint256; roundtrip back to uint128 and compare.

## dap_chain_net_api_init
- What to test: returns int code; idempotent.
- Edge cases: call multiple times.
- Test ideas: call twice and assert int return.

## dap_chain_net_api_deinit
- What to test: returns None; safe to call multiple times.
- Edge cases: call without init.
- Test ideas: call after init and again; assert no exception.

## dap_chain_net_api_register
- What to test: validates capsule type; returns int code.
- Edge cases: non-capsule arg.
- Test ideas: TypeError on invalid arg; integration test once a real `dap_chain_net_api_registry_t`
  capsule is available (from net module) and assert return 0.

## dap_chain_net_api_by_id
- What to test: returns network capsule or None.
- Edge cases: not registered -> None; invalid id type.
- Test ideas: call before net registration -> None; integration test after net init returns capsule.

## dap_chain_net_api_by_name
- What to test: returns network capsule or None.
- Edge cases: not registered -> None; invalid name type.
- Test ideas: call before net registration -> None; integration test after net init.

## dap_chain_net_api_get_chain_by_name
- What to test: returns chain capsule or None.
- Edge cases: invalid net capsule; not registered -> None.
- Test ideas: type validation with dummy capsule; integration test after net init.

## dap_chain_net_api_get_chain_by_type
- What to test: returns chain capsule or None.
- Edge cases: invalid net capsule; not registered -> None.
- Test ideas: type validation; integration test after net init.

## dap_chain_net_api_get_default_chain_by_type
- What to test: returns chain capsule or None.
- Edge cases: invalid net capsule; not registered -> None.
- Test ideas: type validation; integration test after net init.

## dap_chain_net_api_get_cur_cell
- What to test: returns dict `{uint64, raw}` or None.
- Edge cases: invalid net capsule; not registered -> None.
- Test ideas: type validation; integration test after net init returns dict with 8-byte raw.

## dap_chain_net_api_get_load_mode
- What to test: returns bool.
- Edge cases: invalid net capsule; not registered -> False.
- Test ideas: call before registration -> False; integration test with real net in load mode.

## dap_chain_net_api_get_reward
- What to test: returns uint256 bytes.
- Edge cases: invalid net capsule; not registered -> zero bytes.
- Test ideas: call before registration -> zero bytes; integration test with real net reward.

## dap_chain_net_api_add_reward
- What to test: returns int code.
- Edge cases: invalid net capsule; invalid reward type.
- Test ideas: call before registration -> -1; integration test after net init returns 0 on success.

## dap_chain_net_api_datum_add_to_mempool
- What to test: returns hash string or None.
- Edge cases: invalid datum/chain capsules; not registered -> None.
- Test ideas: type validation with dummy capsules; integration test with real datum/chain.

## dap_chain_rpc_callbacks_init
- What to test: returns int; idempotent.
- Edge cases: multiple calls.
- Test ideas: call twice and assert int.

## dap_chain_rpc_callbacks_deinit
- What to test: returns None; safe to call multiple times.
- Edge cases: call without init.
- Test ideas: call after init and again; assert no exception.

## dap_chain_rpc_callbacks_register_consensus
- What to test: registers callback, returns int (0 on success).
- Edge cases: non-callable callback.
- Test ideas: register a callback returning 0; then notify and assert return value.

## dap_chain_rpc_callbacks_register_storage
- What to test: registers callback, returns int.
- Edge cases: non-callable callback.
- Test ideas: register callback; notify with `hash_fast_bytes` and assert return value.

## dap_chain_rpc_callbacks_register_service
- What to test: registers callback, returns int.
- Edge cases: non-callable callback.
- Test ideas: register callback; notify with service name; assert return value.

## dap_chain_rpc_callbacks_register_wallet
- What to test: registers callback, returns int.
- Edge cases: non-callable callback.
- Test ideas: register callback; notify with wallet name and value; assert return value.

## dap_chain_rpc_callbacks_register_tx_notify
- What to test: registers callback, returns int.
- Edge cases: non-callable callback.
- Test ideas: register callback; notify and assert callback invoked (return value ignored).

## dap_chain_rpc_callbacks_notify_consensus
- What to test: returns int from callback.
- Edge cases: not initialized -> negative return; callback not registered -> negative return.
- Test ideas: init, register callback returning 7, notify with dummy capsules and assert 7.

## dap_chain_rpc_callbacks_notify_storage
- What to test: returns int from callback.
- Edge cases: invalid hash; not initialized/registered.
- Test ideas: register callback returning 3; notify with `hash_fast_bytes` and assert 3.

## dap_chain_rpc_callbacks_notify_service
- What to test: returns int from callback.
- Edge cases: missing service name; not initialized/registered.
- Test ideas: register callback returning 1; notify with service name and assert 1.

## dap_chain_rpc_callbacks_notify_wallet
- What to test: returns int from callback.
- Edge cases: invalid value type; not initialized/registered.
- Test ideas: register callback returning 5; notify with value from `dap_chain_balance_scan("1")`.

## dap_chain_rpc_callbacks_notify_tx
- What to test: triggers callback with expected args; returns None.
- Edge cases: invalid datum/ledger capsules; not initialized/registered.
- Test ideas: register callback that records args; notify with dummy capsules and hash bytes.

## dap_chain_type_from_str
- What to test: maps string to enum.
- Edge cases: unknown string.
- Test ideas: "token" -> CHAIN_TYPE_TOKEN; "transaction" -> CHAIN_TYPE_TX; unknown -> CHAIN_TYPE_INVALID.

## dap_chain_type_to_datum_type
- What to test: maps chain type enum to datum type.
- Edge cases: invalid enum -> DAP_CHAIN_DATUM_CUSTOM.
- Test ideas: CHAIN_TYPE_TOKEN -> DAP_CHAIN_DATUM_TOKEN; invalid -> DAP_CHAIN_DATUM_CUSTOM.

## dap_chain_type_to_str
- What to test: maps enum to string.
- Edge cases: invalid enum -> "custom".
- Test ideas: CHAIN_TYPE_TX -> "transaction"; CHAIN_TYPE_INVALID -> "invalid".

## dap_datum_type_to_chain_type
- What to test: maps datum type to chain type.
- Edge cases: unknown datum type -> CHAIN_TYPE_INVALID.
- Test ideas: DAP_CHAIN_DATUM_TOKEN -> CHAIN_TYPE_TOKEN; unknown -> CHAIN_TYPE_INVALID.

## dap_datum_type_to_str
- What to test: maps datum type to string.
- Edge cases: unknown datum type -> "invalid" or "custom" (per SDK behavior).
- Test ideas: DAP_CHAIN_DATUM_TOKEN -> "token"; unknown -> "invalid"/"custom" per actual return.

## dap_cli_error_codes_init
- What to test: initializes registry; returns None.
- Edge cases: multiple calls.
- Test ideas: call twice; assert no exception.

## dap_cli_error_codes_deinit
- What to test: deinitializes registry; returns None.
- Edge cases: deinit without init.
- Test ideas: call after init and again; assert no exception.

## dap_cli_error_code_register
- What to test: registers code; returns 0 on success.
- Edge cases: duplicate code name; invalid name; description None.
- Test ideas: register "ERR_TEST" with code 123; assert return 0; register again and expect non-zero or 0 depending on SDK.

## dap_cli_error_code_get
- What to test: returns code by name, -1 if not found.
- Edge cases: unknown name.
- Test ideas: get "ERR_TEST" after register -> 123; unknown -> -1.
