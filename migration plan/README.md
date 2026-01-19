# Migration Plan

This folder tracks the migration of Cellframe SDK modules into the current codebase.
It is generated from `doc/en/api/cellframe-sdk.md` and should stay in sync with it.

## Principles

- Migrate module-by-module; keep public API stable and avoid breaking changes.
- Prefer additive changes and clear deprecation paths when old behavior must change.
- Keep C wrappers and Python bindings aligned with the SDK headers during each step.
- Update documentation and checklists in the same change set as code moves.

## Testing

- Add unit tests for each migrated method; mark completed in the module checklist.
- Use existing pytest fixtures in `tests/` to avoid external SDK dependencies where possible.
- Add integration tests only after the module is stable and its dependencies are migrated.
- Run targeted tests per module (example): `pytest -m unit -k <module>`.

### Manual build and test (local)

#### 1) Update submodules (if needed)
```bash
git submodule update --init --recursive
```

#### 2) Build cellframe-sdk
```bash
cmake -S cellframe-sdk -B cellframe-sdk/build -DCMAKE_BUILD_TYPE=Release
cmake --build cellframe-sdk/build -j
```

#### 3) Build python-dap
```bash
cmake -S python-dap -B python-dap/build -DCMAKE_BUILD_TYPE=Release
cmake --build python-dap/build -j
```

#### 4) Create venv and install test deps
```bash
python3 -m venv .venv
. .venv/bin/activate
pip install -U pip pytest pytest-asyncio pytest-mock pytest-benchmark
```

#### 5) Build python_cellframe extension (manual cc)
```bash
cat > /tmp/python_cellframe_stubs.c <<'C'
#include "dap_chain.h"
void dap_chain_info_dump_log(dap_chain_t *a_chain) { (void)a_chain; }
C

CF_INC=$(find cellframe-sdk/modules -type d -name include -print0 | xargs -0 -I{} echo -I{})
DAP_INC=$(find cellframe-sdk/dap-sdk/module -type d -name include -print0 | xargs -0 -I{} echo -I{})
UTHASH_INC="-Icellframe-sdk/dap-sdk/3rdparty/uthash/src"
DAP_SRC_INC="-Icellframe-sdk/dap-sdk/module/core/src -Icellframe-sdk/dap-sdk/module/core/src/unix"
JSONC_INC="-Icellframe-sdk/dap-sdk/3rdparty/json-c"
JSONC_BUILD_INC="-Icellframe-sdk/build/dap-sdk/module/core/json-c"
SRC_FILES=$(find src -name '*.c')

cc -shared -fPIC $(python3-config --cflags) -DPY_SSIZE_T_CLEAN -DDAP_OS_LINUX -DDAP_OS_UNIX \
  -Isrc/include -Isrc/common $CF_INC $DAP_INC $UTHASH_INC $DAP_SRC_INC $JSONC_INC $JSONC_BUILD_INC \
  $SRC_FILES /tmp/python_cellframe_stubs.c \
  -Lcellframe-sdk/build -lcellframe_sdk \
  $(python3-config --ldflags) -Wl,--allow-multiple-definition \
  -o python_cellframe$(python3-config --extension-suffix)
```

#### 6) Env for runtime + run tests
```bash
export PYTHONPATH=$PWD:$PWD/python-dap/src
export LD_LIBRARY_PATH=$PWD/python-dap/src/dap:$PWD/python-dap/build/dap-sdk:$PWD/cellframe-sdk/build:$PWD/cellframe-sdk/build/dap-sdk

.venv/bin/pytest -q tests/unit/core/test_chain_wrappers.py
```

Examples of targeted tests:
```bash
.venv/bin/pytest -q tests/unit/core/test_chain_wrappers.py -k "cell"
.venv/bin/pytest -q tests/unit/core/test_chain_wrappers.py::test_chain_basic_getters
```

Notes:
- After any C changes in `src/`, rebuild `python_cellframe` (step 5) to avoid loading an old `.so`.

## Progress

| Module | Migrated | Unit Tests |
| --- | --- | --- |
| `chain` | [x] | [x] |
| `common` | [x] | [x] |
| `consensus` | [x] | [x] |
| `datum` | [x] | [x] |
| `governance` | [ ] | [ ] |
| `ledger` | [ ] | [ ] |
| `mempool` | [ ] | [ ] |
| `net` | [ ] | [ ] |
| `rpc` | [ ] | [ ] |
| `service` | [ ] | [ ] |
| `type` | [ ] | [ ] |
| `wallet` | [ ] | [ ] |
| `wallet-cache` | [ ] | [ ] |
| `wallet-shared` | [ ] | [ ] |

## Files

- `chain.md`
- `common.md`
- `consensus.md`
- `datum.md`
- `governance.md`
- `ledger.md`
- `mempool.md`
- `net.md`
- `rpc.md`
- `service.md`
- `type.md`
- `wallet.md`
- `wallet-cache.md`
- `wallet-shared.md`

## Migration Order and Notes

1) Common/base: callbacks, registries, core types (`src/common`, `src/include/cellframe.h`).
2) Chain + consensus: chain models, consensus hooks and init (`src/chain`).
3) Ledger + TX compose: ledger core, tx composition, events (`src/ledger`, `src/ledger/tx`).
4) Wallet: wallet APIs depend on ledger/tx (`src/wallet`).
5) Network core + node/balancer: net, node, balancer (`src/network`, `src/include/cf_network.h`).
6) Services + mempool + stake: service layer and tx flow (`src/network/services`, `src/network/mempool`, `src/network/stake`).
7) RPC: expose stable surfaces after dependencies are in place (`cellframe-sdk/modules/rpc`).

Checklist tips:
- Mark a method migrated only when its signature, behavior, and error codes match the SDK.
- Mark unit tests only when they exercise the migrated code path, not just mocks.
- Keep module boundaries clean; avoid circular dependencies when moving code.
