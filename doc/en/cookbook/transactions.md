# Transactions

Creating and sending transactions.

## Simple Transfer

```python
from CellFrame.chain import TX, Wallet

# 1. Open sender wallet
sender_wallet = Wallet.open("sender_wallet")

# 2. Create transaction object
tx = TX.create()

# 3. Add input (from sender)
# 'value' is amount in datoshi (minimal unit)
value = 10 * 10**18  # 10.0 tokens
token_ticker = "CELL"
tx.add_in(sender_wallet.get_address(), value, token_ticker)

# 4. Add output (to recipient)
recipient_addr = "..." # Recipient address string
tx.add_out(recipient_addr, value, token_ticker)

# 5. Sign transaction
tx.sign(sender_wallet)

# 6. Broadcast to network
tx.broadcast()
```

