# Wallet API Reference

API reference for the `Wallet` class and related wallet operations.

## Class: `Wallet`

High-level wallet management backed by native `python_cellframe` bindings.

### Constructor

```python
Wallet(name: str, wallet_handle: Any = None, access_type: WalletAccessType = WalletAccessType.LOCAL)
```

Direct construction is uncommon; prefer `Wallet.create()` / `Wallet.open()`.

### Class Methods

#### `create(name: str, wallet_path: str, password: Optional[str] = None, seed: Optional[bytes] = None, signature_type: int = 0x0102) -> Wallet`

Create a new wallet.

**Parameters:**
- `name` (str): Wallet name
- `wallet_path` (str): Path to wallet directory
- `password` (str, optional): Wallet password
- `seed` (bytes, optional): Seed for recovery
- `signature_type` (int, optional): Signature algorithm type (native constant)

#### `open(name: str, wallet_path: str, password: Optional[str] = None) -> Wallet`

Open an existing wallet.

**Parameters:**
- `name` (str): Wallet name
- `wallet_path` (str): Path to wallet directory
- `password` (str, optional): Wallet password

#### `create_with_network(name: str, network_name: str, wallet_path: Optional[str] = None, signature_type: int = 0x0102, seed: Optional[bytes] = None) -> Wallet`

Convenience wrapper that auto-detects wallet path via DAP configuration.

### Instance Methods

#### `get_address(net_id: int) -> WalletAddress`

Get a wallet address for a specific network id.

#### `get_address_for_network(network_name: str, key_index: int = 0) -> WalletAddress`

Resolve network by name and return an address.

#### `get_balance(net_id: int, token_ticker: str) -> Decimal`

Get balance for a network id and token.

#### `get_balance_by_network(network_name: str, token_ticker: str = "CELL") -> Decimal`

Get balance by network name.

#### `get_key(key_type: int = 0x0102)` / `get_pkey(key_type: int = 0x0102)`

Return key or public key handles from the native SDK.

#### `get_pkey_hash() -> str`

Return the public key hash as a hex string.

#### `get_shared_tx_hashes(pkey_hash: str, network_name: str) -> Optional[Dict[str, Any]]`

Fetch shared wallet transaction hashes.

#### `hold_shared_tx(tx_handle: Any, network_name: str) -> bool`

Add a shared-wallet hold transaction (expects a native tx handle).

#### `activate() -> bool` / `deactivate() -> bool`

Activate or deactivate the wallet.

#### `save() -> bool`

Persist wallet data using native bindings.

#### `close() -> None` / `is_closed() -> bool`

Close wallet and release resources.

### Enums

#### `WalletType`

Wallet classification:
- `SIMPLE`
- `MULTISIG`
- `SHARED`
- `HARDWARE`

#### `WalletAccessType`

Access type:
- `LOCAL`
- `REMOTE`

### Exceptions

- `WalletError`
- `InsufficientFundsError`
- `InvalidAddressError`

## WalletManager and Convenience Functions

`WalletManager` provides `create`, `open`, `get`, `close`, `list`, and `close_all`.
Module-level helpers in `CellFrame.wallet.wallet` proxy to the global manager:
`create`, `open`, `get`, `close`, `list`, `close_all`.

## Examples

```python
from CellFrame.chain import Wallet

wallet = Wallet.open("my_wallet", "/path/to/wallets", password="secret")
address = wallet.get_address_for_network("mainnet")
balance = wallet.get_balance_by_network("mainnet", "CELL")

print(address)
print(balance)

wallet.close()
```
