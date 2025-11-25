# Транзакции

Создание и отправка транзакций.

## Простой перевод

```python
from CellFrame.chain import TX, Wallet

# 1. Открыть кошелек отправителя
sender_wallet = Wallet.open("sender_wallet")

# 2. Создать объект транзакции
tx = TX.create()

# 3. Добавить вход (от отправителя)
# 'value' - сумма в datoshi (минимальная единица)
value = 10 * 10**18  # 10.0 токенов
token_ticker = "CELL"
tx.add_in(sender_wallet.get_address(), value, token_ticker)

# 4. Добавить выход (получателю)
recipient_addr = "..." # Строка адреса получателя
tx.add_out(recipient_addr, value, token_ticker)

# 5. Подписать транзакцию
tx.sign(sender_wallet)

# 6. Отправить в сеть
tx.broadcast()
```

