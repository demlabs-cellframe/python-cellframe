# Ledger API Reference

API reference for the `CfLedger` class and ledger operations.

## Class: `CfLedger`

Low-level ledger wrapper backed by native `python_cellframe` functions.

### Constructor

```python
CfLedger(ledger_handle: Any, owns_handle: bool = True)
```

Direct construction is uncommon; prefer `CfLedger.create()` or `CfLedger.open()`.

### Class Methods

#### `create(net_name: str, ledger_type: CfLedgerType = CfLedgerType.MIXED) -> CfLedger`

Create a new ledger for a network.

#### `open(net_name: str) -> CfLedger`

Open an existing ledger for a network.

### Instance Methods

#### `get_balance(address: str, token_ticker: str) -> Decimal`

Get balance for an address and token.

#### `get_balance_full(address: str, token_ticker: str, with_unconfirmed: bool = False) -> Dict[str, int]`

Get full balance information.

#### `add_token(token_info: DapTokenInfo) -> bool`

Add a token to the ledger.

#### `remove_token(token_ticker: str) -> bool`

Remove a token from the ledger.

#### `get_token_info(token_ticker: str) -> Optional[DapTokenInfo]`

Fetch token information.

#### `get_all_tokens() -> List[DapTokenInfo]`

Return all known tokens.

#### `add_transaction(tx_hash: str, tx_data: Any) -> bool`

Add a transaction to the ledger.

#### `remove_transaction(tx_hash: str) -> bool`

Remove a transaction from the ledger.

#### `find_transaction_by_hash(tx_hash: str) -> Optional[Any]`

Look up a transaction by hash.

#### `find_transactions_by_address(address: str, token_ticker: str, limit: int = 100) -> List[Any]`

Find transactions by address and token.

#### `get_outputs_with_value(address: str, token_ticker: str, value_needed: int) -> List[Any]`

Find outputs with a specific value.

#### `close() -> None`

Close the ledger and release resources.

### Context Manager

```python
with CfLedger.open("mainnet") as ledger:
    balance = ledger.get_balance("addr...", "CELL")
```

## Class: `DapTokenInfo`

Represents token information.

```python
DapTokenInfo(ticker: str, name: str, total_supply: int = 0,
             circulating_supply: int = 0, decimals: int = 18)
```

## Enums

### `CfLedgerType`

- `UTXO`
- `ACCOUNT`
- `MIXED`
- `TOKEN`
- `STAKE`

### `CfLedgerTxState`

- `NOT_FOUND`
- `ACCEPTED`
- `INVALID`

## Exceptions

- `CfLedgerError`

## Ledger Manager and Helpers

`CfLedgerManager` manages multiple ledgers. Module-level helpers:
- `create_ledger(net_name, ledger_type=CfLedgerType.MIXED)`
- `open_ledger(net_name)`
- `get_account_balance(ledger, address, token_ticker)`
