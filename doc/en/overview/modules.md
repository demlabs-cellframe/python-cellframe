# Core Modules

The SDK is divided into several main modules. The Python layer is a thin wrapper
around the native `python_cellframe` extension and the `python-dap` bindings.

## CellFrame Package

### CellFrame.common
- Context system (`ExecutionMode`, `AppContext`, `ContextFactory`).
- Core exceptions and shared types.

### CellFrame.chain
- Wallet, ledger, and transaction wrappers.
- Address and mempool helpers.
- Re-exports selected wallet/ledger/tx classes for convenience.

### CellFrame.wallet
- Wallet management: create/open, addresses, balances, key access.

### CellFrame.composer
- Transaction composition utilities (requires native `dap_compose_*` bindings).

### CellFrame.network
- Network helpers and data structures (depends on native network bindings).

### CellFrame.services
- Service wrappers (staking, exchange, voting, orders) backed by native SDK.

### CellFrame.legacy
- Compatibility layer for legacy API consumers.

## DAP Module (`python-dap`)

Low-level system access (networking, cryptography, configuration, Global DB).
Refer to `python-dap` documentation for the `dap.*` APIs.
