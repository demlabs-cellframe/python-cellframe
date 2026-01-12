# Python-Cellframe Refactoring Roadmap (2025-01-11)

## 📊 Текущий статус: 30% завершено

**Phases 1-3, 6:** ✅ ЗАВЕРШЕНЫ - python-cellframe компилируется!  
**Phases 7-12:** ⏳ ЗАПЛАНИРОВАНЫ - глубокий рефакторинг и новые модули

---

## ✅ Завершённые Phases (30%)

### Phase 1: API Analysis ✅
- Все deprecated API идентифицированы
- Создан mapping старый → новый
- **Deliverable:** `API_CHANGES.md`

### Phase 2: Mempool API Fix ✅
- `dap_chain_mempool_datum_get()` заменён на новый API
- Использование `dap_chain_mempool_group_new()` + GDB

### Phase 3: Deprecated APIs Update ✅
- 25+ функций обновлено
- **Результат:** python-cellframe успешно компилируется!

### Phase 6: Documentation ✅
- `API_CHANGES.md` - детальные изменения
- `MIGRATION_GUIDE.md` - гайд для разработчиков
- `CHANGELOG.md` - история изменений

---

## ⏳ Запланированные Phases (70%)

### 🔴 Phase 3.5: TX Compose Refactoring (ВЫСОКИЙ ПРИОРИТЕТ)
**Время:** 10-12 часов  
**Статус:** Блокирует `Mempool.tx_create()`

#### Что нужно сделать:
1. **Изучить новый Registry-based API**
   - `dap_chain_tx_compose_api.h` - plugin system
   - `dap_chain_tx_compose_registry.c` - registry implementation

2. **Создать Python TX builders как plugins**
   - Python callback → C wrapper → register в compose registry
   - GIL management для безопасного вызова

3. **Полностью переписать:**
   - `src/ledger/tx/cf_compose.c` - COMPLETE REWRITE
   - `src/ledger/tx/cf_compose_init.c` - адаптация под registry
   - `src/network/mempool/cf_mempool.c` - использовать dispatcher

#### Новая архитектура:
```c
// Старое (удалено):
dap_chain_mempool_tx_create(chain, key, addr, ...)

// Новое (registry-based):
dap_chain_tx_compose_register("transfer", python_tx_builder, NULL);
datum = dap_chain_tx_compose_create("transfer", ledger, utxo, params);
```

---

### 🟡 Phase 4: New API Exposure (СРЕДНИЙ ПРИОРИТЕТ)
**Время:** 4-6 часов

#### Новые Python модули:
- **`CellFrame.StakeExt.Cache`** - stake-ext cache API
  - `cache_create()`, `cache_find()`, `cache_add()`
- **`CellFrame.Chain.Metrics`** - TPS metrics
  - `get_tps()`, `get_transaction_stats()`

---

### 🟢 Phase 5: Testing & Validation
**Время:** 4-6 часов

- Unit tests (>80% coverage)
- Integration tests
- **КРИТИЧНО:** Regression tests (backward compatibility)

---

### 🟡 Phase 7: Governance Module Bindings
**Время:** 6-8 часов  
**Новый функционал!**

#### Что такое Governance?
Low-level decree management с callback registry (Dependency Inversion)

#### Создаём новый модуль:
- **`CellFrame.Governance.Decree`** - decree operations
- **`CellFrame.Governance.Handlers`** - handler registration
- **`CellFrame.Governance.Callbacks`** - callback management

#### Новые файлы:
```
src/governance/ (NEW DIRECTORY)
├── cf_governance_decree.c
├── cf_governance_handlers.c
└── include/cf_governance.h
```

#### Ключевые функции:
```python
# Register decree handler from Python
def my_decree_handler(decree, ledger, chain, apply):
    # Process decree in Python
    return 0

CellFrame.Governance.Handlers.register(
    decree_type=COMMON,
    decree_subtype=OWNERS,
    handler=my_decree_handler
)
```

---

### 🟡 Phase 8: Policy Module Bindings
**Время:** 4-6 часов  
**Новый функционал!**

#### Что такое Policy?
Policy decree dispatching + anchor handlers (двухуровневая диспетчеризация)

#### Создаём новый модуль:
- **`CellFrame.Policy.Decree`** - policy decrees
- **`CellFrame.Policy.Anchor`** - anchor handlers
- **`CellFrame.Policy.Dispatcher`** - dispatching

#### Новые файлы:
```
src/policy/ (NEW DIRECTORY)
├── cf_policy_decree.c
├── cf_policy_anchor.c
└── include/cf_policy.h
```

---

### 🟡 Phase 9: Wallet-Shared Module Bindings
**Время:** 5-7 часов  
**Новый функционал!**

#### Что такое Wallet-Shared?
Shared funds operations, отделён от wallet для modularity

#### Создаём новый модуль:
- **`CellFrame.WalletShared`**

#### Ключевые функции:
```python
# Create shared wallet TX
tx = CellFrame.WalletShared.create_taking_tx(net, key, addr_to, value, fee, ...)
tx_signed = CellFrame.WalletShared.sign_taking_tx(net, key, tx)
hashes = CellFrame.WalletShared.get_tx_hashes(pkey_hash, net_name)
```

#### Новые файлы:
```
src/wallet/
├── cf_wallet_shared.c (NEW)
└── include/cf_wallet_shared.h (NEW)
```

---

### 🔴 Phase 10: Реализация всех callback stubs (ВЫСОКИЙ ПРИОРИТЕТ)
**Время:** 8-10 часов  
**Блокирует полную функциональность!**

#### Проблема:
9 stub functions требуют реализации (~18-20 callbacks total):

```c
// src/ledger/cf_ledger_utils.c:369
// TODO: Implement full Python callback wrapper with GIL management

// src/chain/cf_chain.c:1113, 1146, 1179, 1212
// TODO: 4 notify callbacks - stubs

// src/ledger/cf_ledger_cond.c:238, 259
// TODO: 10 verificator callbacks - stubs
```

#### Что нужно сделать:
1. **Cache TX check callback**
   - `set_cache_tx_check_callback_py()` - полная реализация

2. **4 Chain notify callbacks**
   - `add_datumindex_notify_callback_py()`
   - `add_datumremoved_notify_callback_py()`
   - `add_atom_confirmed_notify_callback_py()`
   - `add_timer_callback_py()`

3. **10 Verificator callbacks** 
   - `add_verificator_py()` - 6 callbacks
   - `add_voting_verificator_py()` - 4 callbacks

#### Архитектура callback'ов:
```c
// Pattern: Python callback → C wrapper (GIL management) → SDK callback
static int python_callback_wrapper(void *a_arg, ...) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    
    PyObject *l_callback = (PyObject*)a_arg;
    PyObject *l_result = PyObject_CallObject(l_callback, ...);
    
    // Handle result, errors
    
    PyGILState_Release(gstate);
    return result;
}
```

#### Новый файл:
```
src/include/cf_callbacks.h (NEW) - общие callback utilities
```

---

### 🟡 Phase 11: Python Layer Refactoring
**Время:** 6-8 часов  
**Устранение избыточных оберток!**

#### Проблемы:

1. **`CellFrame/composer/*` - дублирует архитектуру C SDK**
   ```python
   # Старое (ПЛОХО):
   class ComposeConfig:
       # Сложная логика в Python, дублирует C SDK
   
   # Новое (ХОРОШО):
   def compose_transfer(ledger, utxo, params):
       # Тонкая обёртка над dap_chain_tx_compose_create
       return cf_native.compose_create("transfer", ledger, utxo, params)
   ```

2. **`CellFrame/legacy/*` - избыточный overhead**
   - Упростить migration layer
   - Убрать лишние врапперы

3. **`CellFrame/services/*` - некоторые избыточные врапперы**
   - Direct delegation к C API где возможно
   - Оставить только: type hints, async/await, context managers

#### Принципы:
- ✅ Python layer = **thin wrappers** + **Pythonic convenience**
- ❌ NO business logic in Python (должен быть в C SDK)
- ❌ NO duplication of C SDK architecture
- ✅ KEEP: type hints, async/await, context managers, error handling

---

### 🟢 Phase 12: Architectural Improvements & SOLID
**Время:** 8-10 часов  
**Качество кода и архитектура!**

#### Проверки:

1. **SOLID Принципы**
   - ✅ Single Responsibility
   - ✅ Open/Closed
   - ✅ Liskov Substitution
   - ✅ Interface Segregation
   - ✅ Dependency Inversion

2. **God Objects**
   - Поиск классов/модулей с >10 ответственностями
   - Разделение на focused modules

3. **Tight Coupling**
   - Прямые зависимости между высоко- и низкоуровневыми модулями
   - Ввести абстракции/интерфейсы

4. **Broken Encapsulation**
   - Прямой доступ к внутренним структурам C SDK
   - Использовать только публичный API

5. **Error Hierarchy**
   - Добавить для новых модулей:
     - `GovernanceError`
     - `PolicyError`
     - `WalletSharedError`
     - `ComposeError`

6. **Circular Dependencies**
   - Проверить import cycles в Python modules
   - Устранить если найдены

---

## 📈 Roadmap Timeline

### Sprint 1 (3-4 дня)
- ✅ Phase 1: API Analysis
- ✅ Phase 2: Mempool API Fix
- ✅ Phase 3: Deprecated APIs Update
- ✅ Phase 6: Documentation

### Sprint 2 (4-5 дней) - ТЕКУЩИЙ
- 🔴 Phase 3.5: TX Compose Refactoring (10-12h)
- 🔴 Phase 10: Callback Implementations (8-10h)
- 🟡 Phase 4: New API Exposure (4-6h)

### Sprint 3 (3-4 дня)
- 🟡 Phase 7: Governance Bindings (6-8h)
- 🟡 Phase 8: Policy Bindings (4-6h)
- 🟡 Phase 9: Wallet-Shared Bindings (5-7h)

### Sprint 4 (2-3 дня)
- 🟡 Phase 11: Python Layer Refactoring (6-8h)
- 🟢 Phase 12: SOLID Improvements (8-10h)
- 🟢 Phase 5: Final Testing (4-6h)

---

## 🎯 Success Criteria

| Критерий | Статус |
|----------|--------|
| python-cellframe компилируется | ✅ Completed |
| Документация создана | ✅ Completed |
| TX Compose refactored | ⏳ Phase 3.5 |
| Новые API exposed | ⏳ Phase 4, 7-9 |
| Все callbacks реализованы | ⏳ Phase 10 |
| Python layer отрефакторен | ⏳ Phase 11 |
| SOLID compliance | ⏳ Phase 12 |
| Тесты проходят (>80%) | ⏳ Phase 5 |
| Regression tests pass | ⏳ Phase 5 |

---

## 🚀 Quick Start Guide

### Начать работу прямо сейчас:

**Вариант A: TX Compose (критично для tx_create)**
```bash
cd /home/naeper/work/python-cellframe
# Изучить новый API:
cat cellframe-sdk/modules/datum/tx/include/dap_chain_tx_compose_api.h
# Начать Phase 3.5
```

**Вариант B: Callback implementations (критично для функционала)**
```bash
# Реализовать первый callback:
vim src/ledger/cf_ledger_utils.c +369
# Заменить TODO на полную реализацию с GIL management
```

**Вариант C: Новые модули (новая функциональность)**
```bash
# Начать Phase 7 - Governance:
mkdir -p src/governance/include
vim src/governance/cf_governance_decree.c
```

---

## 📞 Документация

- **Детали изменений:** `API_CHANGES.md`
- **Руководство по миграции:** `MIGRATION_GUIDE.md`
- **История изменений:** `CHANGELOG.md`
- **Текущий статус:** `PYTHON_CELLFRAME_UPDATE_STATUS.md`
- **SLC задача:** `.context/tasks/python_cellframe_api_update_20250111.json`

---

**Дата:** 2025-01-11  
**Версия roadmap:** 1.0  
**Общий прогресс:** 30% (Phases 1-3, 6 завершены)  
**Следующий шаг:** Phase 3.5 (TX Compose) ИЛИ Phase 10 (Callbacks) ИЛИ Phase 7 (Governance)
