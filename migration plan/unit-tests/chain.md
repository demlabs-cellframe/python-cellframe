# Unit Test Plan: chain

This document describes unit test coverage for the chain module wrappers.
Focus is wrapper behavior (argument validation, return types) and basic smoke
checks; integration-style checks are noted where SDK state is required.

## Common setup and fixtures

- `sdk_available`: skip tests if `python_cellframe.is_sdk_available()` is false.
- `init_sdk`: call `python_cellframe.initialize()` or DAP init once per session.
- `chain_fixture`: create a chain via `dap_chain_init()` + `dap_chain_create()`.
- `cleanup_chain`: call `dap_chain_delete()` for chains created in tests.
- `cell_id_int`: use `1` (uint64).
- `cell_id_dict`: `{"uint64": 1}`.
- `cell_id_bytes`: `b"\x01\x00\x00\x00\x00\x00\x00\x00"` (8 bytes).
- `hash_bytes`: `b"\x00" * HASH_SIZE` (match `sizeof(dap_hash_fast_t)`; confirm size in SDK).
- `atom_bytes`: small bytes payload (e.g., `b"atom"`).
- `datum_bytes`: small bytes payload (e.g., `b"datum"`).
- `tmp_storage_dir`: temporary directory for cell file operations (requires config).

Note: Some methods require a real net config or stream worker/channel objects.
For those, include validation-only unit tests and mark integration tests for later.

---

## dap_cert_chain_file_save
- What to test: accepts datum capsule + net name and returns int result.
- Edge cases: invalid capsule type; empty net name; datum capsule is NULL.
- Test ideas: create a dummy datum (if available) and assert int return; invalid args raise TypeError/ValueError.

## dap_chain_add_callback_datum_index_notify
- What to test: registers Python callback; returns None.
- Edge cases: non-callable callback; invalid chain capsule; user_data None.
- Test ideas: register callback; trigger via `dap_chain_datum_notify` and assert callback called with expected args.

## dap_chain_add_callback_datum_removed_from_index_notify
- What to test: registers Python callback; returns None.
- Edge cases: non-callable callback; invalid chain capsule.
- Test ideas: register callback; trigger via `dap_chain_datum_removed_notify` and assert callback called.

## dap_chain_add_callback_notify
- What to test: registers atom notify callback; returns None.
- Edge cases: non-callable callback; invalid chain capsule.
- Test ideas: register callback; trigger via `dap_chain_atom_notify` and assert callback called.

## dap_chain_add_callback_timer
- What to test: registers timer callback; returns int (0 on success).
- Edge cases: non-callable callback; invalid chain capsule.
- Test ideas: register callback; trigger via `dap_chain_atom_notify`/`dap_chain_atom_remove_notify` with cell_id 0 and assert callback called.

## dap_chain_atom_add_from_threshold
- What to test: no-crash when callback is not set; returns None.
- Edge cases: invalid chain capsule.
- Test ideas: call on a new chain with no threshold callback and assert no exception.

## dap_chain_atom_confirmed_notify_add
- What to test: registers callback; returns None.
- Edge cases: non-callable callback; invalid chain capsule; conf_cnt 0.
- Test ideas: smoke test registration; integration test for actual confirmation notifications later.

## dap_chain_atom_notify
- What to test: passes hash/atom and updates timers; returns None.
- Edge cases: hash None/invalid length; atom None; invalid chain capsule.
- Test ideas: register atom notify callback and timer callback; call `dap_chain_atom_notify` and assert both callbacks called.

## dap_chain_atom_remove_notify
- What to test: triggers timer callbacks; returns None.
- Edge cases: invalid chain capsule; invalid cell_id type.
- Test ideas: register timer callback; call `dap_chain_atom_remove_notify` with cell_id 0; assert callback called with reverse=True.

## dap_chain_atom_save
- What to test: returns dict with `code` and `hash` bytes.
- Edge cases: invalid chain capsule; invalid cell_id; atom bytes empty.
- Test ideas: save small atom; assert dict keys and hash size; verify code is int.

## dap_chain_create
- What to test: returns chain capsule; subsequent delete works.
- Edge cases: empty net/chain names; invalid ids.
- Test ideas: create chain with net/chain ids; assert capsule type; delete in teardown.

## dap_chain_datum_notify
- What to test: passes hash/atom_hash/datum to callbacks; returns None.
- Edge cases: hash/atom_hash wrong size or None; datum not bytes; invalid chain capsule.
- Test ideas: register datum notify callback; call and assert args match and callback invoked.

## dap_chain_datum_removed_notify
- What to test: passes hash and datum capsule to callbacks; returns None.
- Edge cases: hash None/invalid size; datum capsule invalid type.
- Test ideas: register datum removed callback; call and assert callback invoked.

## dap_chain_datum_type_supported_by_chain
- What to test: returns bool.
- Edge cases: invalid chain capsule; non-int datum type.
- Test ideas: call with known datum type (if available) and assert bool result.

## dap_chain_deinit
- What to test: returns None; safe to call multiple times.
- Edge cases: call without init.
- Test ideas: call after init and again; assert no exception.

## dap_chain_delete
- What to test: deletes chain; invalidates capsule.
- Edge cases: invalid chain capsule.
- Test ideas: create chain, delete, then ensure subsequent use errors or returns None.

## dap_chain_find_by_id
- What to test: returns chain capsule or None.
- Edge cases: invalid net/chain ids.
- Test ideas: create chain, find by ids and assert same name/id.

## dap_chain_generation_ban
- What to test: returns int code; affects banned state.
- Edge cases: invalid chain capsule; generation > uint16.
- Test ideas: ban generation, then call `dap_chain_generation_banned` and assert True.

## dap_chain_generation_banned
- What to test: returns bool.
- Edge cases: invalid chain capsule; generation > uint16.
- Test ideas: check before/after ban.

## dap_chain_generation_next
- What to test: returns uint16.
- Edge cases: invalid chain capsule.
- Test ideas: call and assert int in range.

## dap_chain_get_atom_by_hash
- What to test: returns bytes or None.
- Edge cases: invalid chain capsule; hash wrong size.
- Test ideas: save atom and then fetch by hash; compare bytes.

## dap_chain_get_atom_last_hash
- What to test: returns hash bytes or None.
- Edge cases: invalid chain capsule; invalid cell_id.
- Test ideas: save atom and then request last hash; verify size.

## dap_chain_get_atom_last_hash_num_ts
- What to test: returns dict with hash/num/timestamp or None.
- Edge cases: invalid chain capsule; invalid cell_id.
- Test ideas: save atom and verify dict keys and types.

## dap_chain_get_blockhain_time
- What to test: returns int timestamp.
- Edge cases: invalid chain capsule; invalid cell_id.
- Test ideas: call `dap_chain_atom_notify` with cell_id 0 and a timestamp, then assert this function returns that timestamp.

## dap_chain_get_cs_type
- What to test: returns string or None.
- Edge cases: invalid chain capsule.
- Test ideas: set cs_type then get and compare.

## dap_chain_get_path
- What to test: returns string or None.
- Edge cases: invalid chain capsule.
- Test ideas: load chain from config with storage_dir and assert returned path.

## dap_chain_has_file_store
- What to test: returns bool.
- Edge cases: invalid chain capsule.
- Test ideas: chain with storage_dir should return True; without storage_dir should return False.

## dap_chain_id_parse
- What to test: valid hex/dec strings parse to uint64.
- Edge cases: invalid string raises ValueError.
- Test ideas: parse "1" and "0x1"; invalid "xyz".

## dap_chain_info_dump_log
- What to test: returns None, no crash.
- Edge cases: invalid chain capsule.
- Test ideas: call on valid chain; verify no exception.

## dap_chain_init
- What to test: returns int code; idempotent call.
- Edge cases: call multiple times.
- Test ideas: call twice and assert int return each time.

## dap_chain_load_all
- What to test: returns int code.
- Edge cases: invalid chain capsule.
- Test ideas: call on newly created chain and assert int.

## dap_chain_load_from_cfg
- What to test: returns chain capsule or raises error.
- Edge cases: invalid config capsule; missing required config keys.
- Test ideas: build minimal config file (name/id/consensus) and assert chain created.

## dap_chain_purge
- What to test: returns int code.
- Edge cases: invalid chain capsule.
- Test ideas: call after creating chain; assert int.

## dap_chain_set_cs_name
- What to test: sets consensus name.
- Edge cases: invalid chain capsule; empty string.
- Test ideas: set name then read via chain config (or subsequent logic).

## dap_chain_set_cs_type
- What to test: sets consensus type.
- Edge cases: invalid chain capsule; empty string.
- Test ideas: set type then get via `dap_chain_get_cs_type`.

## dap_chain_cell_capture_by_id
- What to test: returns cell capsule or None; no deadlock.
- Edge cases: invalid chain capsule; invalid cell_id.
- Test ideas: call capture on missing cell then call `dap_chain_cell_remit` and ensure subsequent calls succeed.

## dap_chain_cell_close
- What to test: closes cell; returns None.
- Edge cases: invalid chain capsule; invalid cell_id.
- Test ideas: create/open cell, close, then re-open.

## dap_chain_cell_close_all
- What to test: closes all cells; returns None.
- Edge cases: invalid chain capsule.
- Test ideas: open multiple cells then close all and ensure no exceptions.

## dap_chain_cell_create
- What to test: returns int code.
- Edge cases: invalid chain capsule; invalid cell_id.
- Test ideas: create cell in tmp storage; assert return code.

## dap_chain_cell_file_append
- What to test: returns int code; writes data.
- Edge cases: invalid chain capsule; invalid cell_id; empty atom bytes.
- Test ideas: create/open cell, append atom; assert code is int and file grows.

## dap_chain_cell_find_by_id
- What to test: returns cell capsule or None.
- Edge cases: invalid chain capsule; invalid cell_id.
- Test ideas: create cell then find it.

## dap_chain_cell_init
- What to test: returns int code.
- Edge cases: call multiple times.
- Test ideas: call twice and assert int return.

## dap_chain_cell_open
- What to test: returns int code; accepts mode char.
- Edge cases: invalid mode string length; invalid chain capsule.
- Test ideas: open with mode "w" then "r" and assert int return.

## dap_chain_cell_remit
- What to test: releases cell lock; returns None.
- Edge cases: invalid chain capsule.
- Test ideas: capture cell then remit; ensure no deadlock by performing another capture.

## dap_chain_cell_remove
- What to test: returns int code; respects archivate flag.
- Edge cases: invalid chain capsule; invalid cell_id.
- Test ideas: create cell then remove with archivate False/True and assert int.

## dap_chain_cell_set_load_skip
- What to test: returns None; toggles global load skip.
- Edge cases: call multiple times.
- Test ideas: call and ensure no exception.

## dap_chain_cell_truncate
- What to test: returns int code.
- Edge cases: negative delta; invalid chain capsule.
- Test ideas: append data then truncate and verify size decreases.

## dap_chain_ch_deinit
- What to test: returns None; safe to call multiple times.
- Edge cases: call without init.
- Test ideas: call after `dap_chain_ch_init` and again; assert no exception.

## dap_chain_ch_init
- What to test: returns int code.
- Edge cases: call multiple times.
- Test ideas: call twice and assert int.

## dap_stream_ch_write_error_unsafe
- What to test: accepts stream channel capsule and ids; returns None.
- Edge cases: invalid capsule; invalid cell_id.
- Test ideas: validation-only test for TypeError; integration test later with real stream channel.

## dap_chain_ch_pkt_get_size
- What to test: returns size for packet capsule.
- Edge cases: invalid capsule.
- Test ideas: create packet via `dap_chain_ch_pkt_new`, then get size and assert > 0.

## dap_chain_ch_pkt_new
- What to test: returns packet capsule.
- Edge cases: invalid cell_id; data size mismatch.
- Test ideas: create packet with small payload and assert capsule; check get_size.

## dap_chain_ch_pkt_type_to_str
- What to test: returns string for known/unknown types.
- Edge cases: type outside known range.
- Test ideas: call with known type and unknown type; assert string starts with expected prefix.

## dap_chain_ch_pkt_write
- What to test: returns size written.
- Edge cases: invalid worker capsule; invalid cell_id.
- Test ideas: validation-only; integration test later with real stream worker.

## dap_chain_ch_pkt_write_unsafe
- What to test: returns size written.
- Edge cases: invalid channel capsule; invalid cell_id.
- Test ideas: validation-only; integration test later with real stream channel.

## dap_chain_net_get_default_chain_by_chain_type
- What to test: returns chain capsule or None.
- Edge cases: invalid net capsule; invalid chain type.
- Test ideas: load net config, call for known chain type and assert capsule.

## dap_chain_policy_activate_json_collect
- What to test: returns json capsule or None.
- Edge cases: invalid net id; invalid policy num.
- Test ideas: after `dap_chain_policy_net_add`, call and assert capsule or None.

## dap_chain_policy_apply
- What to test: returns int code.
- Edge cases: invalid policy capsule; invalid net id.
- Test ideas: create policy, apply on net, assert int.

## dap_chain_policy_create_activate
- What to test: returns policy capsule.
- Edge cases: invalid args; zero policy num.
- Test ideas: create policy and assert capsule, then `dap_chain_policy_get_size` > 0.

## dap_chain_policy_create_deactivate
- What to test: returns policy capsule from list of numbers.
- Edge cases: empty list; non-numeric items.
- Test ideas: pass list ["1", "2"]; assert capsule.

## dap_chain_policy_deinit
- What to test: returns None; safe to call multiple times.
- Edge cases: call without init.
- Test ideas: call after init and again.

## dap_chain_policy_get_last_num
- What to test: returns uint32.
- Edge cases: invalid net id.
- Test ideas: set last num then get and compare.

## dap_chain_policy_get_size
- What to test: returns size for policy capsule.
- Edge cases: None policy.
- Test ideas: create policy then assert size > 0; pass None and expect 0.

## dap_chain_policy_init
- What to test: returns int code.
- Edge cases: call multiple times.
- Test ideas: call twice.

## dap_chain_policy_is_activated
- What to test: returns bool.
- Edge cases: invalid net id.
- Test ideas: set policy activation and check true/false.

## dap_chain_policy_is_exist
- What to test: returns bool.
- Edge cases: invalid net id.
- Test ideas: query before and after adding policy.

## dap_chain_policy_json_collect
- What to test: returns json capsule or None.
- Edge cases: invalid policy capsule.
- Test ideas: create policy and collect JSON, assert capsule.

## dap_chain_policy_list
- What to test: returns json capsule or None.
- Edge cases: invalid net id.
- Test ideas: after policy add, list and assert capsule.

## dap_chain_policy_net_add
- What to test: returns int code.
- Edge cases: invalid config capsule; duplicate net id.
- Test ideas: add net with config; repeat add and assert error.

## dap_chain_policy_net_purge
- What to test: returns None.
- Edge cases: invalid net id.
- Test ideas: add net, purge, then list returns None.

## dap_chain_policy_net_remove
- What to test: returns None.
- Edge cases: invalid net id.
- Test ideas: add net, remove, then is_exist returns False.

## dap_chain_policy_to_str
- What to test: returns string for policy type.
- Edge cases: None policy.
- Test ideas: create activate/deactivate policy and assert returned string.

## dap_chain_policy_update_last_num
- What to test: returns None; updates last num.
- Edge cases: invalid net id.
- Test ideas: update then get last num and compare.

## dap_chain_srv_add
- What to test: returns int code.
- Edge cases: duplicate uid; invalid callbacks capsule.
- Test ideas: add with uid+name, then `dap_chain_srv_get_uid_by_name` returns uid.

## dap_chain_srv_count
- What to test: returns size.
- Edge cases: invalid net id.
- Test ideas: add service and network entry, then count >= 1.

## dap_chain_srv_decree
- What to test: returns int code.
- Edge cases: params None with non-zero size; invalid capsule.
- Test ideas: call with None params and size 0, assert int; invalid size raises ValueError.

## dap_chain_srv_deinit
- What to test: returns None; safe to call multiple times.
- Edge cases: call without init.
- Test ideas: call after init and again.

## dap_chain_srv_delete
- What to test: returns int code.
- Edge cases: unknown uid.
- Test ideas: add service, delete, then get_uid_by_name returns 0.

## dap_chain_srv_event_verify
- What to test: returns int code.
- Edge cases: invalid hash length; invalid event data type.
- Test ideas: validation-only; integration test later with real service callbacks.

## dap_chain_srv_fee_type_to_str
- What to test: returns string for known/unknown fee types.
- Edge cases: value outside enum.
- Test ideas: call with known and unknown values.

## dap_chain_srv_get_fees
- What to test: returns json capsule or None.
- Edge cases: invalid net id.
- Test ideas: call on net without services and expect None; integration test when services configured.

## dap_chain_srv_get_internal
- What to test: returns capsule or None.
- Edge cases: invalid net id or uid.
- Test ideas: after service start, get internal and assert capsule; otherwise None.

## dap_chain_srv_get_uid_by_name
- What to test: returns uint64.
- Edge cases: unknown name.
- Test ideas: add service with name, then query by name.

## dap_chain_srv_hardfork_all
- What to test: returns capsule or None.
- Edge cases: invalid net id.
- Test ideas: call without services (None) and after services (capsule).

## dap_chain_srv_hardfork_complete_all
- What to test: returns None.
- Edge cases: invalid net id.
- Test ideas: call after hardfork prep; ensure no exception.

## dap_chain_srv_init
- What to test: returns int code.
- Edge cases: call multiple times.
- Test ideas: call twice and assert int.

## dap_chain_srv_list
- What to test: returns list capsule or None.
- Edge cases: invalid net id.
- Test ideas: add service to net, list and assert capsule.

## dap_chain_srv_load_state
- What to test: returns int code.
- Edge cases: invalid state bytes; invalid uid.
- Test ideas: pass empty state and assert code; invalid args raise TypeError.

## dap_chain_srv_purge
- What to test: returns int code.
- Edge cases: invalid net id or uid.
- Test ideas: add service, purge, check code.

## dap_chain_srv_purge_all
- What to test: returns int code.
- Edge cases: invalid net id.
- Test ideas: add services, purge_all, count becomes 0.

## dap_chain_srv_start
- What to test: returns int code.
- Edge cases: invalid config capsule; unknown service name.
- Test ideas: validation-only; integration test once services are wired.

## dap_chain_srv_start_all
- What to test: returns int code.
- Edge cases: invalid net id.
- Test ideas: validation-only; integration test later with multiple services.
