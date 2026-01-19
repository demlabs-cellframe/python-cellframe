# Операции с кошельком

Примеры основных операций с кошельком.

## Создание кошелька

```python
from CellFrame.chain import Wallet

wallet_name = "my_wallet"
wallet_path = "/path/to/wallets"

wallet = Wallet.create(wallet_name, wallet_path, password="secret")
print(f"Кошелек '{wallet_name}' создан")
```

## Открытие кошелька

```python
wallet = Wallet.open("my_wallet", "/path/to/wallets", password="secret")
print(f"Открыт кошелек: {wallet.name}")
```

## Адрес и баланс

```python
address = wallet.get_address_for_network("mainnet")
balance = wallet.get_balance_by_network("mainnet", "CELL")
print(address)
print(balance)
```
