# Token Manager - Python Bindings

## Overview

Простые C биндинги для доступа к токенам в ledger.

## Python API

### 1. Получение токена по ticker

```python
import python_cellframe as cf

# Get ledger
ledger = cf.get_ledger_for_network("Backbone")

# Get token by ticker
token = cf.ledger_token_get(ledger, "KEL")
if token:
    ticker = cf.token_get_ticker(token)
    token_type = cf.token_get_type(token)
    print(f"Token: {ticker}, Type: {token_type}")
```

### 2. Получение всех токенов (JSON)

```python
# Get all tokens
json_tokens = cf.ledger_token_get_all(ledger, limit=100, offset=0, version=0)

# Convert to Python dict (using dap_json bindings)
tokens_dict = cf.json_to_dict(json_tokens)
print(tokens_dict)
```

### 3. Получение информации о токене (JSON)

```python
# Get specific token info
json_info = cf.ledger_token_get_info(ledger, "KEL", version=0)

if json_info:
    info = cf.json_to_dict(json_info)
    print(f"Token info: {info}")
```

### 4. Получение emission по hash

```python
# Get emission by hash (32 bytes)
emission_hash = bytes.fromhex("...")  # 32 bytes
emission = cf.ledger_token_emission_get(ledger, emission_hash)

if emission:
    # Work with emission datum
    pass
```

## Constants

```python
# Token types
cf.TOKEN_TYPE_SIMPLE           # Simple token
cf.TOKEN_TYPE_PRIVATE_DECL     # Private token declaration
cf.TOKEN_TYPE_PRIVATE_UPDATE   # Private token update
cf.TOKEN_TYPE_NATIVE_DECL      # Native token declaration  
cf.TOKEN_TYPE_NATIVE_UPDATE    # Native token update
```

## Architecture

```
Python API:
  ├─ ledger_token_get(ledger, ticker) → token capsule
  ├─ ledger_token_get_all(ledger, limit, offset) → JSON capsule
  ├─ ledger_token_get_info(ledger, ticker) → JSON capsule
  ├─ ledger_token_emission_get(ledger, hash) → emission capsule
  ├─ token_get_ticker(token) → str
  └─ token_get_type(token) → int
     │
     └─> C API:
         ├─ dap_ledger_token_ticker_check()
         ├─ dap_ledger_token_info()
         ├─ dap_ledger_token_info_by_name()
         └─ dap_ledger_token_emission_find()
```

## Complete Example

```python
import python_cellframe as cf

class TokenManager:
    def __init__(self, network_name):
        self.ledger = cf.get_ledger_for_network(network_name)
    
    def get_token(self, ticker):
        """Get token by ticker"""
        token = cf.ledger_token_get(self.ledger, ticker)
        if not token:
            return None
        
        return {
            'ticker': cf.token_get_ticker(token),
            'type': cf.token_get_type(token),
            'capsule': token
        }
    
    def get_all_tokens(self, limit=0):
        """Get all tokens as dict"""
        json_tokens = cf.ledger_token_get_all(self.ledger, limit, 0, 0)
        if not json_tokens:
            return []
        
        return cf.json_to_dict(json_tokens)
    
    def get_token_info(self, ticker):
        """Get detailed token info"""
        json_info = cf.ledger_token_get_info(self.ledger, ticker, 0)
        if not json_info:
            return None
        
        return cf.json_to_dict(json_info)

# Usage
mgr = TokenManager("Backbone")

# Get specific token
kel_token = mgr.get_token("KEL")
print(f"KEL Token: {kel_token}")

# Get all tokens
all_tokens = mgr.get_all_tokens(limit=10)
for token in all_tokens:
    print(f"Token: {token}")

# Get detailed info
kel_info = mgr.get_token_info("KEL")
print(f"KEL Info: {kel_info}")
```

## Notes

- **Capsules**: Token/emission возвращаются как `PyCapsule` (указатели на C структуры)
- **JSON**: Token info возвращается как `dap_json_t` capsule (используй `json_to_dict()` для конверсии)
- **Memory**: Capsules не требуют ручного освобождения (managed by ledger)
- **Thread Safety**: Ledger API thread-safe (rwlock protection)

## Next Steps

- **Phase B**: Ledger Manager - превратить ledger capsule в PyLedgerObject
- **Phase C**: Anchor Manager - аналогичный API для anchors
- **Phase D**: Stake Manager - API для stakes
