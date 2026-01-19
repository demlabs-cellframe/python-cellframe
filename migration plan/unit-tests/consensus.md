# Unit Test Plan: consensus

This document describes unit test coverage for the consensus module wrappers.
Focus is wrapper behavior (argument validation, return types) and basic smoke
checks; integration-style checks are noted where SDK state is required.

## Common setup and fixtures

- `sdk_available`: skip tests if `python_cellframe.is_sdk_available()` is false.
- `init_sdk`: call `python_cellframe.initialize()` or DAP init once per session.
- `net_id_int`: use `1` (uint64).
- `chain_fixture`: create a chain via `dap_chain_init()` + `dap_chain_create()`.
- `config_fixture`: minimal config capsule for chain creation (reuse from chain tests).
- `net_fixture`: create a net capsule if required (reuse from network tests if available).
- `hash_bytes`: `b"\x00" * HASH_SIZE` (match `sizeof(dap_hash_fast_t)`; confirm size).
- `hash_hex_str`: `"00" * 64` (hex string for hash parsing).
- `uint256_bytes`: `b"\x00" * 32` (match `sizeof(uint256_t)`; confirm size).
- `node_addr_uint`: `0x0102030405060708`.
- `node_addr_str`: `"0001::0002::0003::0004"`.

Note: Many consensus APIs depend on registered callbacks or runtime consensus
state. Unit tests should focus on input validation and return types, and mark
behavioral checks as integration tests for later.

---

## [ ] dap_chain_cs_block_pow_init
- What to test: returns int result code.
- Edge cases: call twice; call without SDK init.
- Test ideas: call init twice and assert int return each time; deinit in teardown.

## [ ] dap_chain_cs_block_pow_deinit
- What to test: returns None and does not crash.
- Edge cases: deinit before init; multiple calls.
- Test ideas: call with no prior init; call after init; assert no exception.

## [x] dap_chain_type_dag_poa_init
- What to test: returns int result code.
- Edge cases: call twice; call without SDK init.
- Test ideas: init twice; assert int return; deinit in teardown.

## [x] dap_chain_type_dag_poa_deinit
- What to test: returns None and does not crash.
- Edge cases: deinit before init; multiple calls.
- Test ideas: call twice; assert no exception.

## [x] dap_chain_type_dag_poa_get_auth_certs
- What to test: returns tuple (list capsule, count, verify_count) or None.
- Edge cases: invalid chain capsule; chain not dag_poa.
- Test ideas: call on non-dag_poa chain and assert None; integration test with dag_poa chain to validate counts.

## [x] dap_chain_type_dag_poa_presign_callback_set
- What to test: accepts None or callback capsule; returns None.
- Edge cases: invalid chain capsule; invalid callback capsule; invalid arg capsule.
- Test ideas: pass None to clear; invalid type raises TypeError; integration test with real callback capsule later.

## [x] dap_chain_cs_esbocs_init
- What to test: returns int result code.
- Edge cases: call twice; call without SDK init.
- Test ideas: init twice; assert int return; deinit in teardown.

## [x] dap_chain_cs_esbocs_deinit
- What to test: returns None and does not crash.
- Edge cases: deinit before init; multiple calls.
- Test ideas: call twice; assert no exception.

## [x] dap_chain_esbocs_started
- What to test: returns bool.
- Edge cases: invalid net_id type; negative net_id.
- Test ideas: call with `net_id_int` and assert bool; negative net_id raises OverflowError.

## [x] dap_chain_esbocs_stop_timer
- What to test: returns None.
- Edge cases: invalid net_id type; negative net_id.
- Test ideas: call with `net_id_int`; negative net_id raises OverflowError.

## [x] dap_chain_esbocs_start_timer
- What to test: returns None.
- Edge cases: invalid net_id type; negative net_id.
- Test ideas: call with `net_id_int`; negative net_id raises OverflowError.

## [x] dap_chain_esbocs_get_sign_pkey
- What to test: returns pkey capsule or None.
- Edge cases: invalid net_id type.
- Test ideas: call with `net_id_int` and assert None or capsule; integration test for actual pkey.

## [x] dap_chain_esbocs_get_fee
- What to test: returns bytes of uint256.
- Edge cases: invalid net_id type.
- Test ideas: call with `net_id_int`; assert bytes length equals `sizeof(uint256_t)`.

## [x] dap_chain_esbocs_get_autocollect_status
- What to test: returns bool.
- Edge cases: invalid net_id type.
- Test ideas: call with `net_id_int` and assert bool.

## [x] dap_chain_esbocs_add_block_collect
- What to test: accepts block_cache and params capsules (or None) and returns None.
- Edge cases: invalid capsule types; non-int type.
- Test ideas: call with None capsules and valid type; invalid capsule raises TypeError.

## [x] dap_chain_esbocs_add_validator_to_clusters
- What to test: returns bool.
- Edge cases: invalid net_id type; invalid node address format.
- Test ideas: call with `node_addr_uint` and `node_addr_str`; invalid string raises ValueError.

## [x] dap_chain_esbocs_remove_validator_from_clusters
- What to test: returns bool.
- Edge cases: invalid net_id type; invalid node address format.
- Test ideas: call with `node_addr_uint` and `node_addr_str`; invalid string raises ValueError.

## [x] dap_chain_esbocs_get_collecting_level
- What to test: returns bytes of uint256.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture and assert bytes length.

## [x] dap_chain_esbocs_get_sign_key
- What to test: returns sign key capsule or None.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture; assert None or capsule.

## [x] dap_chain_esbocs_set_min_validators_count
- What to test: returns int result code.
- Edge cases: invalid chain capsule; count > uint16.
- Test ideas: pass count 1 and assert int; pass 70000 and expect ValueError.

## [x] dap_chain_esbocs_get_min_validators_count
- What to test: returns int count.
- Edge cases: invalid net_id type.
- Test ideas: call with `net_id_int` and assert int.

## [x] dap_chain_esbocs_set_emergency_validator
- What to test: returns int result code.
- Edge cases: invalid chain capsule; invalid hash; sign_type > uint32.
- Test ideas: use `hash_bytes` and sign_type 0; invalid hash length raises ValueError.

## [x] dap_chain_esbocs_set_signs_struct_check
- What to test: returns int result code.
- Edge cases: invalid chain capsule; non-bool enable.
- Test ideas: call with enable True/False and assert int.

## [x] dap_chain_esbocs_set_hardfork_prepare
- What to test: returns int result code.
- Edge cases: invalid chain capsule; generation > uint16; invalid list/json capsules.
- Test ideas: call with None trusted/changed capsules; invalid capsule raises TypeError.

## [x] dap_chain_esbocs_set_hardfork_complete
- What to test: returns int result code.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture and assert int.

## [x] dap_chain_esbocs_hardfork_engaged
- What to test: returns bool.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture and assert bool.

## [x] dap_chain_esbocs_set_hardfork_state
- What to test: returns int result code.
- Edge cases: invalid chain capsule; non-bool state.
- Test ideas: call with state True/False and assert int.

## [ ] dap_chain_esbocs_get_custom_metadata
- What to test: returns bytes or None.
- Edge cases: invalid session capsule.
- Test ideas: invalid capsule raises TypeError; integration test with real session capsule later.

## [x] dap_chain_esbocs_set_custom_metadata_callback
- What to test: accepts callback capsule or None; returns int.
- Edge cases: invalid callback capsule type.
- Test ideas: pass None and assert int; invalid type raises TypeError; integration test with real callback capsule later.

## [x] dap_chain_esbocs_set_presign_callback
- What to test: accepts callback capsule or None; returns int.
- Edge cases: invalid callback capsule type.
- Test ideas: pass None and assert int; invalid type raises TypeError; integration test with real callback capsule later.

## [x] dap_chain_esbocs_set_empty_block_every_times
- What to test: returns int result code.
- Edge cases: invalid chain capsule; blockgen_period > uint16.
- Test ideas: call with period 0/1; invalid value raises ValueError.

## [x] dap_chain_cs_esbocs_decree_init
- What to test: returns int result code.
- Edge cases: call twice; call without SDK init.
- Test ideas: init twice and assert int return.

## [x] dap_chain_cs_esbocs_decree_deinit
- What to test: returns None and does not crash.
- Edge cases: deinit before init; multiple calls.
- Test ideas: call twice; assert no exception.

## [x] dap_chain_cs_add
- What to test: registers consensus implementation; returns None.
- Edge cases: empty name; invalid callbacks capsule.
- Test ideas: call with name and None callbacks; invalid capsule raises TypeError.

## [x] dap_chain_cs_create
- What to test: returns int result code.
- Edge cases: invalid chain/config capsules.
- Test ideas: call with valid chain/config capsules; invalid type raises TypeError.

## [x] dap_chain_cs_load
- What to test: returns int result code.
- Edge cases: invalid chain/config capsules.
- Test ideas: call with valid chain/config capsules; invalid type raises TypeError.

## [x] dap_chain_cs_start
- What to test: returns int result code.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture and assert int return.

## [x] dap_chain_cs_stop
- What to test: returns int result code.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture and assert int return.

## [x] dap_chain_cs_purge
- What to test: returns int result code.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture and assert int return.

## [x] dap_chain_cs_set_callbacks
- What to test: accepts callbacks capsule or None; returns None.
- Edge cases: invalid chain capsule; invalid callbacks capsule.
- Test ideas: pass None callbacks; invalid type raises TypeError; integration test with real callbacks capsule later.

## [x] dap_chain_cs_get_callbacks
- What to test: returns callbacks capsule or None.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain without callbacks and assert None; integration test with callbacks set.

## [x] dap_chain_cs_get_fee_group
- What to test: returns string or None.
- Edge cases: invalid chain capsule; empty net name.
- Test ideas: call with empty net name and assert None or string; integration test with callbacks.

## [x] dap_chain_cs_get_reward_group
- What to test: returns string or None.
- Edge cases: invalid chain capsule; empty net name.
- Test ideas: call with empty net name and assert None or string; integration test with callbacks.

## [x] dap_chain_cs_get_fee
- What to test: returns bytes of uint256.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture and assert bytes length equals `sizeof(uint256_t)`.

## [x] dap_chain_cs_get_sign_pkey
- What to test: returns pkey capsule or None.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture; assert None or capsule.

## [x] dap_chain_cs_get_sign_key
- What to test: returns sign key capsule or None.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture; assert None or capsule.

## [x] dap_chain_cs_get_collecting_level
- What to test: returns bytes of uint256.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture; assert bytes length.

## [x] dap_chain_cs_get_autocollect_status
- What to test: returns bool.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture and assert bool.

## [x] dap_chain_cs_add_block_collect
- What to test: accepts block_cache/params capsules (or None) and returns None.
- Edge cases: invalid chain capsule; invalid block_cache/params capsules.
- Test ideas: call with None block_cache/params; invalid capsule raises TypeError.

## [x] dap_chain_cs_set_hardfork_state
- What to test: returns int result code.
- Edge cases: invalid chain capsule; non-bool state.
- Test ideas: call with True/False; assert int return.

## [x] dap_chain_cs_hardfork_engaged
- What to test: returns bool.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture and assert bool.

## [x] dap_chain_cs_is_started
- What to test: returns bool.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture and assert bool.

## [x] dap_chain_cs_get_min_validators_count
- What to test: returns int count.
- Edge cases: invalid chain capsule.
- Test ideas: call on chain fixture and assert int.

## [x] dap_chain_cs_set_min_validators_count
- What to test: returns int result code.
- Edge cases: invalid chain capsule; count > uint16.
- Test ideas: pass count 1 and assert int; pass 70000 and expect ValueError.

## [x] dap_chain_cs_add_validator
- What to test: returns int result code.
- Edge cases: invalid chain capsule; invalid node address format.
- Test ideas: pass `node_addr_uint` and `node_addr_str`; invalid string raises ValueError.

## [x] dap_chain_cs_remove_validator
- What to test: returns int result code.
- Edge cases: invalid chain capsule; invalid node address format.
- Test ideas: pass `node_addr_uint` and `node_addr_str`; invalid string raises ValueError.

## [x] dap_chain_cs_mempool_datum_add
- What to test: returns hash string or None.
- Edge cases: invalid chain capsule; invalid datum capsule; invalid hash_out_type.
- Test ideas: invalid types raise TypeError; integration test with real datum capsule to verify hash string.

## [x] dap_chain_cs_stake_check_pkey_hash
- What to test: returns tuple (int, uint256 bytes, addr capsule).
- Edge cases: invalid chain capsule; invalid hash size; hash as hex string.
- Test ideas: use `hash_bytes` and `hash_hex_str`; assert tuple types and sizes.

## [x] dap_chain_cs_stake_hardfork_data_import
- What to test: returns int result code.
- Edge cases: invalid chain capsule; invalid hash size.
- Test ideas: call with `hash_bytes`; invalid size raises ValueError.

## [x] dap_chain_cs_stake_switch_table
- What to test: returns int result code.
- Edge cases: invalid chain capsule; non-bool argument.
- Test ideas: call with True/False; assert int return.

## [x] dap_chain_cs_init
- What to test: returns int result code; idempotent call.
- Edge cases: call multiple times.
- Test ideas: init twice and assert int each time.

## [x] dap_chain_cs_deinit
- What to test: returns None; safe to call multiple times.
- Edge cases: deinit before init.
- Test ideas: call twice; assert no exception.

## [x] dap_chain_validator_api_init
- What to test: returns int result code.
- Edge cases: call multiple times.
- Test ideas: init twice and assert int return.

## [x] dap_chain_validator_api_deinit
- What to test: returns None; safe to call multiple times.
- Edge cases: deinit before init.
- Test ideas: call twice; assert no exception.

## [x] dap_chain_validator_api_register
- What to test: accepts registry capsule and returns None.
- Edge cases: invalid capsule type.
- Test ideas: invalid type raises TypeError; integration test with real registry capsule.

## [x] dap_chain_validator_api_get_validators
- What to test: returns list capsule or None.
- Edge cases: invalid net_id type.
- Test ideas: call with `net_id_int` and assert None when API not registered.

## [x] dap_chain_validator_api_check_key_delegated
- What to test: returns int result code.
- Edge cases: invalid address type; invalid address length.
- Test ideas: pass address bytes or string; invalid length raises ValueError.

## [x] dap_chain_validator_api_mark_validator_active
- What to test: returns int result code.
- Edge cases: invalid address type; non-bool active.
- Test ideas: pass address and True/False; invalid type raises TypeError.

## [x] dap_chain_validator_api_get_pkey_by_hash
- What to test: returns pkey capsule or None.
- Edge cases: invalid hash size; invalid net_id type.
- Test ideas: pass `hash_bytes` and `net_id_int`; assert None when API not registered.

## [x] dap_chain_validator_api_key_delegate
- What to test: returns None; validates input types.
- Edge cases: invalid net capsule; invalid address/hash/value/node_addr types.
- Test ideas: invalid types raise TypeError; integration test with real net and validator setup.

## [x] dap_chain_validator_api_get_allowed_min_value
- What to test: returns bytes of uint256.
- Edge cases: invalid net_id type.
- Test ideas: call with `net_id_int`; assert bytes length.

## [x] dap_chain_validator_api_hardfork_tx_update
- What to test: returns None.
- Edge cases: invalid net capsule.
- Test ideas: invalid type raises TypeError; integration test with real net capsule.

## [x] dap_chain_validator_api_get_node_addr
- What to test: returns uint64 or None.
- Edge cases: invalid validator item capsule.
- Test ideas: invalid type raises TypeError; integration test with real validator item capsule.

## [x] dap_chain_validator_api_get_value
- What to test: returns bytes of uint256.
- Edge cases: invalid validator item capsule.
- Test ideas: invalid type raises TypeError; integration test with real validator item capsule.
