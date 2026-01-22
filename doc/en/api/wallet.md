# Wallet API Reference

Complete API reference for the `Wallet` class and related wallet operations.

## Class: `Wallet`

High-level wallet management for CellFrame blockchain operations.

### Constructor

```python
Wallet(name: str, wallet_dir: str = None, wallet_handle: Any = None)
```

**Parameters:**
- `name` (str): Wallet name
- `wallet_dir` (str, optional): Directory for wallet files. Defaults to system wallet directory.
- `wallet_handle` (Any, optional): Native C wallet handle (internal use)

**Raises:**
- `WalletError`: If wallet creation fails

### Class Methods

#### `create(name: str, wallet_dir: str = None, wallet_type: WalletType = None) -> Wallet`

Create a new wallet.

**Parameters:**
- `name` (str): Wallet name
- `wallet_dir` (str, optional): Directory for wallet files
- `wallet_type` (WalletType, optional): Signature type (default: SIG_DILITHIUM)

**Returns:**
- `Wallet`: New wallet instance

**Raises:**
- `WalletError`: If creation fails

**Example:**
```python
from CellFrame.chain import Wallet, WalletType

wallet = Wallet.create("my_wallet", wallet_type=WalletType.SIG_DILITHIUM)
```

#### `create_with_seed(name: str, seed: str, wallet_dir: str = None, wallet_type: WalletType = None) -> Wallet`

Create wallet from seed phrase.

**Parameters:**
- `name` (str): Wallet name
- `seed` (str): Seed phrase
- `wallet_dir` (str, optional): Directory for wallet files
- `wallet_type` (WalletType, optional): Signature type

**Returns:**
- `Wallet`: New wallet instance

#### `create_multi(name: str, seed: str, required_signatures: int, wallet_dir: str = None, wallet_type: WalletType = None) -> Wallet`

Create multi-signature wallet.

**Parameters:**
- `name` (str): Wallet name
- `seed` (str): Seed phrase
- `required_signatures` (int): Number of required signatures
- `wallet_dir` (str, optional): Directory for wallet files
- `wallet_type` (WalletType, optional): Signature type

**Returns:**
- `Wallet`: New multi-signature wallet instance

#### `open(name: str, wallet_dir: str = None) -> Wallet`

Open existing wallet.

**Parameters:**
- `name` (str): Wallet name
- `wallet_dir` (str, optional): Directory for wallet files

**Returns:**
- `Wallet`: Opened wallet instance

**Raises:**
- `WalletError`: If wallet not found or cannot be opened

### Instance Methods

#### `get_address() -> WalletAddress`

Get wallet address for default network.

**Returns:**
- `WalletAddress`: Wallet address object

#### `get_address_for_network(network_name: str, key_index: int = 0) -> WalletAddress`

Get wallet address for specific network.

**Parameters:**
- `network_name` (str): Network name (e.g., "mainnet", "testnet")
- `key_index` (int): Key index (default: 0)

**Returns:**
- `WalletAddress`: Wallet address for the network

**Raises:**
- `WalletError`: If network not found or address cannot be retrieved

#### `get_balance(token_ticker: str = "CELL") -> Decimal`

Get wallet balance for default network.

**Parameters:**
- `token_ticker` (str): Token ticker (default: "CELL")

**Returns:**
- `Decimal`: Wallet balance

#### `get_balance_by_network(network_name: str, token_ticker: str = "CELL") -> Decimal`

Get wallet balance for specific network and token.

**Parameters:**
- `network_name` (str): Network name
- `token_ticker` (str): Token ticker (default: "CELL")

**Returns:**
- `Decimal`: Wallet balance

**Raises:**
- `WalletError`: If network not found or balance cannot be retrieved

#### `get_pkey_hash() -> str`

Get wallet public key hash as hexadecimal string.

**Returns:**
- `str`: Public key hash (hex format with "0x" prefix)

**Raises:**
- `WalletError`: If operation fails

#### `get_shared_tx_hashes(pkey_hash: str, network_name: str) -> Optional[Dict[str, Any]]`

Get shared wallet transaction hashes by public key hash.

**Parameters:**
- `pkey_hash` (str): Public key hash string
- `network_name` (str): Network name

**Returns:**
- `Optional[Dict[str, Any]]`: Dictionary with transaction hashes or None

**Raises:**
- `WalletError`: If operation fails

#### `hold_shared_tx(tx_handle: Any, network_name: str) -> bool`

Add hold transaction to shared wallet.

**Parameters:**
- `tx_handle` (Any): Transaction handle (native C transaction object)
- `network_name` (str): Network name

**Returns:**
- `bool`: True if successful

**Raises:**
- `WalletError`: If operation fails

#### `activate() -> bool`

Activate wallet (make it available for operations).

**Returns:**
- `bool`: True if successful

**Raises:**
- `WalletError`: If activation fails

#### `deactivate() -> bool`

Deactivate wallet.

**Returns:**
- `bool`: True if successful

**Raises:**
- `WalletError`: If deactivation fails

#### `close() -> None`

Close wallet and release resources.

#### `is_closed() -> bool`

Check if wallet is closed.

**Returns:**
- `bool`: True if wallet is closed

### Properties

- `name` (str): Wallet name (read-only)
- `wallet_dir` (str): Wallet directory (read-only)
- `access_type` (WalletAccessType): Access type (read-only)
- `wallet_type` (WalletType): Signature type (read-only)

## Class: `WalletAddress`

Represents a wallet address.

### Constructor

```python
WalletAddress(address: str, net_id: int)
```

**Parameters:**
- `address` (str): Address string
- `net_id` (int): Network ID

### Methods

#### `__str__() -> str`

Return address as string.

#### `__repr__() -> str`

Return address representation.

### Properties

- `address` (str): Address string
- `net_id` (int): Network ID

## Class: `WalletManager`

Manages multiple wallets.

### Methods

#### `get_wallet(name: str) -> Optional[Wallet]`

Get wallet by name.

**Parameters:**
- `name` (str): Wallet name

**Returns:**
- `Optional[Wallet]`: Wallet instance or None

#### `list_wallets() -> List[str]`

List all available wallet names.

**Returns:**
- `List[str]`: List of wallet names

## Enums

### `WalletType`

Signature algorithm types:
- `SIG_DILITHIUM`: Dilithium (quantum-safe)
- `SIG_FALCON`: Falcon (quantum-safe)
- `SIG_ED25519`: Ed25519 (classical)

### `WalletAccessType`

Wallet access types:
- `READ_ONLY`: Read-only access
- `READ_WRITE`: Read-write access

## Exceptions

### `WalletError`

Base exception for wallet operations.

### `InsufficientFundsError`

Raised when wallet has insufficient funds.

### `InvalidAddressError`

Raised when address is invalid.

## Examples

### Basic Wallet Operations

```python
from CellFrame.chain import Wallet, WalletType
from decimal import Decimal

# Create wallet
wallet = Wallet.create("my_wallet", wallet_type=WalletType.SIG_DILITHIUM)

# Get address
address = wallet.get_address()
print(f"Address: {address}")

# Check balance
balance = wallet.get_balance("CELL")
print(f"Balance: {balance} CELL")

# Save and close
wallet.save()
wallet.close()
```

### Multi-Signature Wallet

```python
from CellFrame.chain import Wallet

# Create multi-signature wallet
seed = "your seed phrase here"
wallet = Wallet.create_multi(
    "multisig_wallet",
    seed=seed,
    required_signatures=3
)
```

### Network-Specific Operations

```python
# Get address for specific network
mainnet_address = wallet.get_address_for_network("mainnet")
testnet_address = wallet.get_address_for_network("testnet")

# Get balance for specific network
mainnet_balance = wallet.get_balance_by_network("mainnet", "CELL")
```

### Shared Wallet Operations

```python
# Get public key hash
pkey_hash = wallet.get_pkey_hash()

# Get shared transaction hashes
tx_hashes = wallet.get_shared_tx_hashes(pkey_hash, "mainnet")

# Add hold transaction
if tx_hashes:
    tx_hash = list(tx_hashes.keys())[0]
    wallet.hold_shared_tx(tx_hash, "mainnet")
```

