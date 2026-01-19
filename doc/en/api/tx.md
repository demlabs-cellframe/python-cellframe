# Transaction API Reference

API reference for the `TX` class and transaction operations.

## Class: `TX`

Low-level transaction operations for working with `dap_chain_tx_t` structures.

### Constructor

```python
TX(tx_handle: Any, owns_handle: bool = True)
```

**Parameters:**
- `tx_handle` (Any): Native C transaction handle
- `owns_handle` (bool): Whether this instance owns the handle

### Class Methods

#### `from_raw(raw_tx_data: bytes) -> TX`

Create a `TX` instance from raw transaction bytes.

### Instance Methods

#### `add_input(tx_hash: str, out_index: int, value: int, token_ticker: str) -> None`

Add a transaction input.

#### `add_output(address: str, value: int, token_ticker: str) -> None`

Add a transaction output.

#### `sign(signature: Any) -> None`

Attach a signature. The object must either be raw signature bytes or expose
`get_signature_data()`.

#### `verify() -> bool`

Verify all signatures using native bindings.

#### `get_size() -> int`

Return serialized transaction size.

#### `broadcast(chain_handle: Any, hash_out_type: str = "hex") -> str`

Send the transaction to the mempool. `chain_handle` is a native chain pointer
from the network/chain layer.

#### `to_dict() -> Dict[str, Any]`

Return a dictionary view of the TX object.

#### `finalize() -> None`

Finalize and release native resources.

### Context Manager

```python
with TX.from_raw(b"") as tx:
    tx.add_output("addr...", 1000, "CELL")
    # tx.sign(signature_data)
    # tx.broadcast(chain_handle)
```

### Enums

#### `TxType`

- `TRANSFER`
- `STAKE_ORDER`
- `VOTE`
- `DECREE`
- `CONDITIONAL`
- `SHARED_WALLET`
- `EXCHANGE`
- `BRIDGE`

#### `TxStatus`

- `PENDING`
- `CONFIRMED`
- `FAILED`
- `REJECTED`
- `EXPIRED`

### Exceptions

- `TxError`

## Helper Functions

- `get_tx_by_hash(tx_hash: str) -> Optional[TX]`
- `broadcast_tx(tx: TX, chain_handle: Any) -> str`

## Notes

- `token_ticker` is required for inputs and outputs.
- Use the `Composer` module for higher-level transaction creation when available.
