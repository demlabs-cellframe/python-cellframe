# Transactions

Creating transactions with the current API.

## Simple Transfer (Composer)

The `Composer` module builds and submits transactions using native
`dap_compose_*` bindings.

```python
from decimal import Decimal
from CellFrame.chain import Wallet
from CellFrame.composer import Composer

wallet = Wallet.open("sender_wallet", "/path/to/wallets", password="secret")
recipient = wallet.get_address_for_network("mainnet")

with Composer(net_name="mainnet", wallet=wallet) as composer:
    tx_hash = composer.create_tx(
        to_address=recipient,
        amount=Decimal("1.0"),
        token_ticker="CELL",
        fee=Decimal("0.01")
    )

print(tx_hash)
```

Note: the composer requires the native `python_cellframe` extension with
transaction composition functions exported.
