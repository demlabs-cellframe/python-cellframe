# Python-Cellframe

Python bindings for the Cellframe SDK. This repository ships a native C extension
(`python_cellframe`) plus the `CellFrame` Python package with thin wrappers over
Cellframe/DAP SDK APIs.

## What is included

- Native extension modules for wallet, chain, ledger, tx, network, services, etc.
- Python wrappers in `CellFrame` (and the `cellframe` alias module).
- Legacy compatibility helpers under `CellFrame.legacy`.

## Status

- Core bindings are available via the native `python_cellframe` extension.
- The context system (plugin/library detection) is implemented in
  `CellFrame.common.context`.
- The high-level `CellframeNode`/`auto_create_node` API and async helpers
  described in older drafts are not implemented yet.

## Requirements

- Built and installed `python_dap` and `python_cellframe` extensions.
- Cellframe SDK and DAP SDK libraries available on the system.

## Quick example

```python
import CellFrame
from CellFrame.chain import Wallet

# Initialize context (auto-detects plugin vs library)
ctx = CellFrame.initialize_context()

# Open wallet and query balance
wallet = Wallet.open("my_wallet", "/path/to/wallets", password="secret")
address = wallet.get_address_for_network("mainnet")
balance = wallet.get_balance_by_network("mainnet", "CELL")

print(f"Address: {address}")
print(f"Balance: {balance} CELL")

CellFrame.shutdown_context()
```

## Documentation

See `doc/en/` and `doc/ru/` for API references and guides. The documentation is
kept in sync with the current code; if something is missing, check the module
sources in `src/CellFrame/`.
