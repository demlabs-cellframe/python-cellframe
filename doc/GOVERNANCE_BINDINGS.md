# Governance Module - Python Bindings

## Overview

Тонкие C биндинги вокруг системы управления decree (указ) в Cellframe SDK.

## C API

**Core Functions:**
- `dap_chain_decree_handler_register()` - регистрация обработчиков decree
- `dap_chain_decree_handler_call()` - вызов зарегистрированных обработчиков  
- `PyDapDecree` - Python тип для работы с decree datum

## Python API

### 1. Регистрация обработчиков

```python
import python_cellframe as cf

def my_fee_handler(decree, ledger, chain, apply, user_data):
    """
    Handler for Fee decrees
    
    Args:
        decree: PyDapDecree object
        ledger: dap_ledger_t capsule
        chain: dap_chain_t capsule or None
        apply: bool - True to apply changes, False to verify only
        user_data: Optional user data passed during registration
        
    Returns:
        int: 0 on success, negative on error
    """
    print(f"Fee decree received: {decree.get_fee()}")
    if apply:
        # Apply fee changes
        pass
    return 0

# Register handler
cf.register_handler(
    type=cf.DECREE_TYPE_COMMON,         # 0x0001
    subtype=cf.DECREE_SUBTYPE_FEE,       # 0x0001
    callback=my_fee_handler,
    user_data={"my": "data"}             # Optional
)
```

### 2. Unregister обработчиков

```python
cf.unregister_handler(
    type=cf.DECREE_TYPE_COMMON,
    subtype=cf.DECREE_SUBTYPE_FEE
)
```

### 3. Ручной вызов обработчиков (для тестирования)

```python
result = cf.call_handler(
    type=cf.DECREE_TYPE_COMMON,
    subtype=cf.DECREE_SUBTYPE_FEE,
    decree=decree_obj,
    ledger=ledger_capsule,
    chain=chain_capsule,  # or None
    apply=True
)
print(f"Handler returned: {result}")
```

### 4. Работа с PyDapDecree

```python
# PyDapDecree уже реализован в cf_governance_decree.c
# Предоставляет 27 getter методов:

decree = ...  # получен из ledger или callback

# Basic info
decree_type = decree.get_type()           # uint16_t
subtype = decree.get_subtype()            # uint16_t
timestamp = decree.get_timestamp()        # uint64_t
net_id = decree.get_net_id()              # uint64_t
chain_id = decree.get_chain_id()          # uint64_t

# Financial data
fee = decree.get_fee()                     # uint256 or None
fee_addr = decree.get_fee_addr()           # str or None
stake_value = decree.get_stake_value()     # uint256 or None

# Governance data
owners = decree.get_owners()               # list[bytes]
min_owners = decree.get_min_owners()       # int or None
signs = decree.get_signs()                 # list[bytes]

# Serialization
json_str = decree.to_json("hex")           # JSON string
raw_bytes = decree.to_bytes()              # bytes
dump_str = decree.dump("hex")              # human-readable
```

## Constants

```python
# Decree types
cf.DECREE_TYPE_COMMON   = 0x0001
cf.DECREE_TYPE_SERVICE  = 0x0002

# Common decree subtypes
cf.DECREE_SUBTYPE_FEE                     = 0x0001
cf.DECREE_SUBTYPE_OWNERS                  = 0x0002
cf.DECREE_SUBTYPE_OWNERS_MIN              = 0x0003
cf.DECREE_SUBTYPE_STAKE_APPROVE           = 0x0005
cf.DECREE_SUBTYPE_STAKE_INVALIDATE        = 0x0006
cf.DECREE_SUBTYPE_STAKE_MIN_VALUE         = 0x0007
cf.DECREE_SUBTYPE_STAKE_MIN_VALIDATORS    = 0x0008
cf.DECREE_SUBTYPE_BAN                     = 0x0009
cf.DECREE_SUBTYPE_UNBAN                   = 0x000A
cf.DECREE_SUBTYPE_HARDFORK                = 0x000F
cf.DECREE_SUBTYPE_POLICY                  = 0x0012
```

## Architecture

```
Python Layer:
  ├─ register_handler(type, subtype, callback, user_data)
  ├─ unregister_handler(type, subtype)
  └─ call_handler(type, subtype, decree, ledger, chain, apply)
     │
     └─> C Wrapper with GIL management
         └─> dap_chain_decree_handler_register()
             └─> UTHASH registry

Callback Flow:
  C SDK triggers decree → s_py_decree_handler_wrapper()
                       → Acquire GIL
                       → Wrap C objects (decree, ledger, chain)
                       → Call Python callback
                       → Release GIL
                       → Return result
```

## Thread Safety

- **GIL Management**: Автоматическое управление Python GIL в callbacks
- **Mutex Protection**: `pthread_mutex` защита для registry
- **Reference Counting**: Правильный INCREF/DECREF для Python objects

## Memory Management

- **Decree Objects**: `is_owned=false` в callbacks (borrowed reference)
- **Capsules**: Auto-cleanup через PyCapsule destructors
- **Python Callbacks**: INCREF при регистрации, DECREF при cleanup

## Example: Complete Handler Registration

```python
import python_cellframe as cf

class GovernanceManager:
    def __init__(self):
        self.handlers = {}
    
    def register_fee_handler(self):
        def handler(decree, ledger, chain, apply, user_data):
            fee = decree.get_fee()
            print(f"Fee decree: {fee}")
            return 0
        
        cf.register_handler(
            type=cf.DECREE_TYPE_COMMON,
            subtype=cf.DECREE_SUBTYPE_FEE,
            callback=handler
        )
        self.handlers[(cf.DECREE_TYPE_COMMON, cf.DECREE_SUBTYPE_FEE)] = handler
    
    def cleanup(self):
        for (dtype, subtype), _ in self.handlers.items():
            cf.unregister_handler(dtype, subtype)

# Usage
mgr = GovernanceManager()
mgr.register_fee_handler()

# ... work with decrees ...

mgr.cleanup()
```

## Next Steps

Для высокоуровневого Pythonic API см. **Phase A-F** (Token/Ledger/Anchor/Stake Managers).
