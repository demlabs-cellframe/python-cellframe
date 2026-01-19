# Getting Started with Python-Cellframe

This guide walks through a minimal setup using the current API.

## Prerequisites

- Python 3.8+.
- Built and installed native `python_cellframe` and `python_dap` extensions.

See `doc/en/quickstart/installation.md` for build and package options.

## Minimal Example

```python
import CellFrame
from CellFrame.chain import Wallet

# Initialize global context (auto-detects plugin vs library)
ctx = CellFrame.initialize_context()

wallet = Wallet.open("my_wallet", "/path/to/wallets", password="secret")
address = wallet.get_address_for_network("mainnet")
print(address)

CellFrame.shutdown_context()
```

## Checking Execution Mode

```python
import CellFrame
from CellFrame.common.context import ExecutionMode

mode = CellFrame.get_mode()
if mode == ExecutionMode.PLUGIN:
    print("Plugin mode")
elif mode == ExecutionMode.LIBRARY:
    print("Library mode")
```

## Next Steps

- Wallet operations: `doc/en/cookbook/wallet_operations.md`
- Transactions: `doc/en/cookbook/transactions.md`
- Core context API: `doc/en/api/core.md`
