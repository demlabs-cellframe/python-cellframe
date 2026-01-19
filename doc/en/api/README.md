# Python Cellframe SDK API Documentation

This section documents the APIs that are implemented in the current codebase.
The high-level `CellframeNode`/`auto_create_node` layer is not implemented yet;
use the module-level APIs described below.

## Modules covered

- [Core context system](./core.md)
- [Wallet](./wallet.md)
- [Ledger](./ledger.md)
- [Transactions](./tx.md)

## DAP integration

Low-level DAP bindings are provided by the separate `python-dap` project.
Refer to its documentation for `dap.*` modules.

## Minimal usage example

```python
import CellFrame
from CellFrame.chain import Wallet

ctx = CellFrame.initialize_context()
wallet = Wallet.open("my_wallet", "/path/to/wallets", password="secret")
address = wallet.get_address_for_network("mainnet")

print(address)

CellFrame.shutdown_context()
```
