# Wallet Operations

Common wallet operations using the current API.

## Creating a Wallet

```python
from CellFrame.chain import Wallet

wallet_name = "my_wallet"
wallet_path = "/path/to/wallets"

wallet = Wallet.create(wallet_name, wallet_path, password="secret")
print(f"Wallet '{wallet_name}' created")
```

## Opening a Wallet

```python
wallet = Wallet.open("my_wallet", "/path/to/wallets", password="secret")
print(f"Opened wallet: {wallet.name}")
```

## Getting an Address and Balance

```python
address = wallet.get_address_for_network("mainnet")
balance = wallet.get_balance_by_network("mainnet", "CELL")
print(address)
print(balance)
```
