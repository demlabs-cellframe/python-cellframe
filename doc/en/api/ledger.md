# Ledger API Reference

Complete API reference for the `CfLedger` class and ledger operations.

## Class: `CfLedger`

High-level ledger management for CellFrame blockchain operations.

### Constructor

```python
CfLedger(network_name: str, ledger_type: CfLedgerType = None)
```

**Parameters:**
- `network_name` (str): Network name (e.g., "mainnet", "testnet")
- `ledger_type` (CfLedgerType, optional): Ledger type (default: UTXO)

**Raises:**
- `CfLedgerError`: If ledger creation fails

### Class Methods

#### `create(network_name: str, ledger_type: CfLedgerType = None) -> CfLedger`

Create a new ledger for a network.

**Parameters:**
- `network_name` (str): Network name
- `ledger_type` (CfLedgerType, optional): Ledger type

**Returns:**
- `CfLedger`: New ledger instance

**Raises:**
- `CfLedgerError`: If creation fails

**Example:**
```python
from CellFrame.chain.ledger import CfLedger, CfLedgerType

ledger = CfLedger.create("mainnet", ledger_type=CfLedgerType.UTXO)
```

#### `open(network_name: str, ledger_type: CfLedgerType = None) -> CfLedger`

Open existing ledger for a network.

**Parameters:**
- `network_name` (str): Network name
- `ledger_type` (CfLedgerType, optional): Ledger type

**Returns:**
- `CfLedger`: Opened ledger instance

**Raises:**
- `CfLedgerError`: If ledger not found or cannot be opened

### Instance Methods

#### `get_balance(address: str, token_ticker: str) -> Decimal`

Get balance for an address and token.

**Parameters:**
- `address` (str): Address string
- `token_ticker` (str): Token ticker (REQUIRED)

**Returns:**
- `Decimal`: Balance (converted from uint256_t)

**Raises:**
- `CfLedgerError`: If token_ticker is empty or operation fails

**Example:**
```python
balance = ledger.get_balance("addr123...", "CELL")
print(f"Balance: {balance} CELL")
```

**Note:** The function accepts address as string and converts it internally to `dap_chain_addr_t` capsule.

#### `get_balance_full(address: str, token_ticker: str, with_unconfirmed: bool = False) -> Dict[str, int]`

Get full balance information including all states (pending, confirmed, etc.).

**Parameters:**
- `address` (str): Address string
- `token_ticker` (str): Token ticker (REQUIRED)
- `with_unconfirmed` (bool): Include unconfirmed transactions (default: False)

**Returns:**
- `Dict[str, int]`: Dictionary with balance information

**Raises:**
- `CfLedgerError`: If operation fails

#### `find_transaction(tx_hash: str) -> Optional[Dict[str, Any]]`

Find transaction by hash.

**Parameters:**
- `tx_hash` (str): Transaction hash

**Returns:**
- `Optional[Dict[str, Any]]`: Transaction data or None if not found

**Raises:**
- `CfLedgerError`: If operation fails

#### `get_token_tickers(address: str) -> List[str]`

Get all token tickers for an address.

**Parameters:**
- `address` (str): Address string

**Returns:**
- `List[str]`: List of token tickers

**Raises:**
- `CfLedgerError`: If operation fails

#### `add_token(token_info: DapTokenInfo) -> bool`

Add token to ledger.

**Parameters:**
- `token_info` (DapTokenInfo): Token information object

**Returns:**
- `bool`: True if successful

**Raises:**
- `CfLedgerError`: If operation fails

**Example:**
```python
from CellFrame.chain.ledger import DapTokenInfo

token_info = DapTokenInfo(
    ticker="MYTOKEN",
    name="My Token",
    total_supply=1000000,
    decimals=18
)
ledger.add_token(token_info)
```

#### `remove_token(ticker: str) -> bool`

Remove token from ledger.

**Parameters:**
- `ticker` (str): Token ticker

**Returns:**
- `bool`: True if successful

**Raises:**
- `CfLedgerError`: If operation fails

#### `get_token_info(ticker: str) -> Optional[DapTokenInfo]`

Get token information by ticker.

**Parameters:**
- `ticker` (str): Token ticker (REQUIRED)

**Returns:**
- `Optional[DapTokenInfo]`: Token info or None if not found

**Raises:**
- `CfLedgerError`: If ticker is empty or operation fails

#### `get_all_tokens() -> List[DapTokenInfo]`

Get all tokens from ledger.

**Returns:**
- `List[DapTokenInfo]`: List of token info

**Raises:**
- `CfLedgerError`: If operation fails

#### `add_transaction(tx_hash: str, tx_data: bytes) -> bool`

Add transaction to ledger.

**Parameters:**
- `tx_hash` (str): Transaction hash
- `tx_data` (bytes): Transaction data

**Returns:**
- `bool`: True if successful

**Raises:**
- `CfLedgerError`: If operation fails

**Note:** This method uses `dap_ledger_tx_add` internally.

#### `remove_transaction(tx_hash: str) -> bool`

Remove transaction from ledger.

**Parameters:**
- `tx_hash` (str): Transaction hash

**Returns:**
- `bool`: True if successful

**Raises:**
- `CfLedgerError`: If operation fails

#### `find_transactions_by_address(address: str) -> List[Dict[str, Any]]`

Find transactions by address.

**Parameters:**
- `address` (str): Address string

**Returns:**
- `List[Dict[str, Any]]`: List of transaction data

**Raises:**
- `CfLedgerError`: If operation fails

#### `get_tx_outputs_with_value(address: str, token_ticker: str = "CELL") -> List[Dict[str, Any]]`

Get list of transaction outputs with value for an address.

**Parameters:**
- `address` (str): Address string
- `token_ticker` (str): Token ticker (default: "CELL")

**Returns:**
- `List[Dict[str, Any]]`: List of transaction outputs

**Raises:**
- `CfLedgerError`: If operation fails

#### `add_token_emission(ticker: str, emission_hash: str, emission_data: bytes) -> bool`

Add token emission to ledger.

**Parameters:**
- `ticker` (str): Token ticker
- `emission_hash` (str): Emission hash
- `emission_data` (bytes): Emission data

**Returns:**
- `bool`: True if successful

**Raises:**
- `CfLedgerError`: If operation fails

#### `load_token_emission(ticker: str, emission_hash: str) -> Optional[bytes]`

Load token emission from ledger.

**Parameters:**
- `ticker` (str): Token ticker
- `emission_hash` (str): Emission hash

**Returns:**
- `Optional[bytes]`: Emission data or None if not found

**Raises:**
- `CfLedgerError`: If operation fails

#### `check_token_emission(ticker: str, emission_hash: str, emission_data: bytes) -> bool`

Check/validate token emission before adding.

**Parameters:**
- `ticker` (str): Token ticker
- `emission_hash` (str): Emission hash
- `emission_data` (bytes): Emission data

**Returns:**
- `bool`: True if valid

**Raises:**
- `CfLedgerError`: If operation fails

#### `close() -> None`

Close ledger and release resources.

**Raises:**
- `CfLedgerError`: If operation fails

### Context Manager

CfLedger supports context manager protocol:

```python
with CfLedger.open("mainnet") as ledger:
    balance = ledger.get_balance("addr...", "CELL")
    # Automatically closed on exit
```

### Properties

- `network_name` (str): Network name (read-only)
- `ledger_type` (CfLedgerType): Ledger type (read-only)
- `is_closed` (bool): Whether ledger is closed (read-only)

## Class: `DapTokenInfo`

Represents token information.

### Constructor

```python
DapTokenInfo(ticker: str, name: str, total_supply: int = 0, 
             circulating_supply: int = 0, decimals: int = 18)
```

**Parameters:**
- `ticker` (str): Token ticker
- `name` (str): Token name
- `total_supply` (int): Total supply
- `circulating_supply` (int): Circulating supply
- `decimals` (int): Decimal places

### Properties

- `ticker` (str): Token ticker
- `name` (str): Token name
- `total_supply` (int): Total supply
- `circulating_supply` (int): Circulating supply
- `decimals` (int): Decimal places

## Enums

### `CfLedgerType`

Ledger types:
- `UTXO`: UTXO model
- `ACCOUNT`: Account model
- `MIXED`: Mixed model
- `TOKEN`: Token ledger
- `STAKE`: Staking ledger

### `CfLedgerTxState`

Transaction states:
- `NOT_FOUND`: Transaction not found
- `ACCEPTED`: Transaction accepted
- `INVALID`: Transaction invalid

## Exceptions

### `CfLedgerError`

Base exception for ledger operations.

## Examples

### Basic Ledger Operations

```python
from CellFrame.chain.ledger import CfLedger, CfLedgerType
from decimal import Decimal

# Open ledger
ledger = CfLedger.open("mainnet", ledger_type=CfLedgerType.UTXO)

# Get balance
balance = ledger.get_balance("addr123...", "CELL")
print(f"Balance: {balance} CELL")

# Get all token tickers for address
tickers = ledger.get_token_tickers("addr123...")
print(f"Tokens: {tickers}")

# Close ledger
ledger.close()
```

### Token Management

```python
# Add token
ledger.add_token(
    ticker="MYTOKEN",
    name="My Token",
    total_supply=1000000,
    decimals=18
)

# Find token
token_info = ledger.find_token("MYTOKEN")
if token_info:
    print(f"Token: {token_info.name}, Supply: {token_info.total_supply}")

# Get all tokens
all_tokens = ledger.get_all_tokens()
for token in all_tokens:
    print(f"{token.ticker}: {token.name}")
```

### Transaction Queries

```python
# Find transaction by hash
tx = ledger.find_transaction("0x1234...")
if tx:
    print(f"Transaction found: {tx}")

# Find transactions by address
txs = ledger.find_transactions_by_address("addr123...")
print(f"Found {len(txs)} transactions")

# Get transaction outputs with value
outputs = ledger.get_tx_outputs_with_value("addr123...", "CELL")
for output in outputs:
    print(f"Output: {output}")
```

### Token Emission

```python
# Check emission before adding
emission_data = b"emission data..."
emission_hash = "0xhash..."
if ledger.check_token_emission("MYTOKEN", emission_hash, emission_data):
    # Add emission
    ledger.add_token_emission("MYTOKEN", emission_hash, emission_data)

# Load emission
loaded_data = ledger.load_token_emission("MYTOKEN", emission_hash)
if loaded_data:
    print(f"Emission loaded: {len(loaded_data)} bytes")
```

### Using Context Manager

```python
with CfLedger.open("mainnet") as ledger:
    balance = ledger.get_balance("addr...", "CELL")
    tickers = ledger.get_token_tickers("addr...")
    # Automatically closed
```

