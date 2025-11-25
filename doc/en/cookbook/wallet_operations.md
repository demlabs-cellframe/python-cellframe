# Wallet Operations

This recipe covers common wallet operations.

## Creating a Wallet

```python
from CellFrame.chain import Wallet, WalletType

# Create a new wallet
wallet_name = "my_wallet"
wallet_type = WalletType.SIG_DILITHIUM  # Modern quantum-safe signature
wallet = Wallet.create(wallet_name, wallet_type)

if wallet:
    print(f"Wallet '{wallet_name}' created.")
    print(f"Address: {wallet.get_address()}")
else:
    print("Failed to create wallet.")
```

## Opening a Wallet

```python
wallet = Wallet.open("my_wallet")
if wallet:
    print(f"Opened wallet: {wallet.name}")
```

## Checking Balance

```python
# Get balance for a specific token (e.g., "CELL")
balance = wallet.get_balance("CELL")
print(f"Balance: {balance} CELL")
```

