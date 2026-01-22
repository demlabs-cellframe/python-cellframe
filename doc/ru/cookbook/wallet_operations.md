# Операции с кошельком

Этот рецепт описывает основные операции с кошельками.

## Создание кошелька

```python
from CellFrame.chain import Wallet, WalletType

# Создание нового кошелька
wallet_name = "my_wallet"
wallet_type = WalletType.SIG_DILITHIUM  # Современная квантово-устойчивая подпись
wallet = Wallet.create(wallet_name, wallet_type)

if wallet:
    print(f"Кошелек '{wallet_name}' создан.")
    print(f"Адрес: {wallet.get_address()}")
else:
    print("Не удалось создать кошелек.")
```

## Открытие кошелька

```python
wallet = Wallet.open("my_wallet")
if wallet:
    print(f"Открыт кошелек: {wallet.name}")
```

## Проверка баланса

```python
# Получить баланс для конкретного токена (например, "CELL")
balance = wallet.get_balance("CELL")
print(f"Баланс: {balance} CELL")
```

