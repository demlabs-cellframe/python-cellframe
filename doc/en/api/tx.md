# Transaction API Reference

Complete API reference for the `TX` class and transaction operations.

## Class: `TX`

Low-level transaction operations for working with `dap_chain_tx_t` structures.

### Constructor

```python
TX(tx_handle: Any, owns_handle: bool = True)
```

**Parameters:**
- `tx_handle` (Any): Native C transaction handle
- `owns_handle` (bool): Whether this instance owns the handle (default: True)

**Raises:**
- `TxError`: If transaction handle is None

### Class Methods

#### `from_raw(raw_tx_data: bytes) -> TX`

Create TX from raw transaction data.

**Parameters:**
- `raw_tx_data` (bytes): Raw transaction bytes

**Returns:**
- `TX`: Transaction instance

**Raises:**
- `TxError`: If parsing fails

### Instance Methods

#### `add_input(tx_hash: str, out_index: int, value: int, token_ticker: str) -> None`

Add input to transaction.

**Parameters:**
- `tx_hash` (str): Previous transaction hash
- `out_index` (int): Output index in previous transaction
- `value` (int): Value in datoshi (minimal unit)
- `token_ticker` (str): Token ticker (REQUIRED)

**Raises:**
- `TxError`: If token_ticker is empty or operation fails

**Example:**
```python
tx.add_input("0x1234...", 0, 1000000000, "CELL")
```

#### `add_output(address: str, value: int, token_ticker: str) -> None`

Add output to transaction.

**Parameters:**
- `address` (str): Recipient address
- `value` (int): Value in datoshi (minimal unit)
- `token_ticker` (str): Token ticker (REQUIRED)

**Raises:**
- `TxError`: If token_ticker is empty or operation fails

**Example:**
```python
tx.add_output("addr123...", 1000000000, "CELL")
```

#### `sign(signature: Any) -> None`

Sign transaction.

**Parameters:**
- `signature`: Signature object or signature data

**Raises:**
- `TxError`: If signature is None or signing fails

**Example:**
```python
# Sign with wallet
wallet.sign_transaction(tx)

# Or add signature directly
tx.sign(signature_data)
```

#### `verify() -> bool`

Verify transaction signatures.

**Returns:**
- `bool`: True if transaction is valid

**Raises:**
- `TxError`: If verification fails

#### `get_size() -> int`

Get transaction size in bytes.

**Returns:**
- `int`: Transaction size

**Raises:**
- `TxError`: If operation fails

#### `broadcast(chain_handle: Any, hash_out_type: str = "hex") -> str`

Send transaction to mempool.

**Parameters:**
- `chain_handle` (Any): Chain handle (from network)
- `hash_out_type` (str): Hash output format (default: "hex")

**Returns:**
- `str`: Transaction hash

**Raises:**
- `TxError`: If transaction is finalized or broadcast fails

#### `to_dict() -> Dict[str, Any]`

Convert transaction to dictionary.

**Returns:**
- `Dict[str, Any]`: Transaction data as dictionary

#### `finalize() -> None`

Finalize transaction and release resources.

### Context Manager

TX supports context manager protocol:

```python
with TX.from_raw(raw_data) as tx:
    tx.add_output("addr...", 1000, "CELL")
    tx.sign(wallet)
    tx.broadcast(chain)
    # Automatically finalized on exit
```

### Properties

- `hash` (str): Transaction hash
- `type` (TxType): Transaction type
- `status` (TxStatus): Transaction status
- `inputs` (List[TxInput]): Transaction inputs
- `outputs` (List[TxOutput]): Transaction outputs
- `fee` (int): Transaction fee
- `token_ticker` (str): Token ticker
- `created_at` (datetime): Creation timestamp
- `confirmed_at` (Optional[datetime]): Confirmation timestamp
- `signatures` (List): Transaction signatures

## Class: `TxInput`

Represents a transaction input.

### Constructor

```python
TxInput(tx_hash: str, out_index: int, value: int, token_ticker: str)
```

**Parameters:**
- `tx_hash` (str): Previous transaction hash
- `out_index` (int): Output index
- `value` (int): Value in datoshi
- `token_ticker` (str): Token ticker

### Methods

#### `to_dict() -> Dict[str, Any]`

Convert to dictionary.

## Class: `TxOutput`

Represents a transaction output.

### Constructor

```python
TxOutput(address: str, value: int, token_ticker: str)
```

**Parameters:**
- `address` (str): Recipient address
- `value` (int): Value in datoshi
- `token_ticker` (str): Token ticker

### Methods

#### `to_dict() -> Dict[str, Any]`

Convert to dictionary.

## Enums

### `TxType`

Transaction types:
- `TRANSFER`: Standard transfer
- `EMISSION`: Token emission
- `ANCHOR`: Anchor transaction
- `DECREE`: Decree transaction

### `TxStatus`

Transaction status:
- `PENDING`: Pending confirmation
- `CONFIRMED`: Confirmed
- `REJECTED`: Rejected
- `FAILED`: Failed

## Exceptions

### `TxError`

Base exception for transaction operations.

## Examples

### Creating a Simple Transfer

```python
from CellFrame.chain import TX, Wallet

# Open wallet
wallet = Wallet.open("sender_wallet")

# Create transaction
tx = TX.from_raw(b"")  # Or use Composer for high-level creation

# Add input (spending from previous transaction)
tx.add_input(
    tx_hash="0x1234...",
    out_index=0,
    value=1000000000,  # 1.0 token (assuming 9 decimals)
    token_ticker="CELL"
)

# Add output (sending to recipient)
tx.add_output(
    address="addr123...",
    value=1000000000,
    token_ticker="CELL"
)

# Sign transaction
wallet.sign_transaction(tx)

# Verify
if tx.verify():
    # Broadcast to network
    chain_handle = get_chain_handle("mainnet")
    tx_hash = tx.broadcast(chain_handle)
    print(f"Transaction sent: {tx_hash}")
```

### Using Context Manager

```python
with TX.from_raw(raw_data) as tx:
    tx.add_output("addr...", 1000, "CELL")
    tx.sign(wallet)
    
    if tx.verify():
        tx_hash = tx.broadcast(chain_handle)
        print(f"TX hash: {tx_hash}")
```

### Transaction Information

```python
# Get transaction details
tx_dict = tx.to_dict()
print(f"Hash: {tx_dict['hash']}")
print(f"Type: {tx_dict['type']}")
print(f"Status: {tx_dict['status']}")
print(f"Size: {tx.get_size()} bytes")
print(f"Inputs: {len(tx.inputs)}")
print(f"Outputs: {len(tx.outputs)}")
```

### Multi-Token Transaction

```python
# Add inputs/outputs for different tokens
tx.add_input("tx1", 0, 1000, "CELL")
tx.add_input("tx2", 0, 500, "TOKEN")

tx.add_output("addr1", 1000, "CELL")
tx.add_output("addr2", 500, "TOKEN")
```

## Notes

- All values are in **datoshi** (minimal unit). For tokens with 9 decimals, 1 token = 10^9 datoshi.
- `token_ticker` is **REQUIRED** for all inputs and outputs.
- Transactions must be signed before broadcasting.
- Use `Composer` class for high-level transaction creation (recommended for most use cases).

