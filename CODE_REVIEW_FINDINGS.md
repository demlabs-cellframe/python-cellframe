# Python-Cellframe Code Review Findings (2025-01-11)

## 🚨 КРИТИЧЕСКИЕ ПРОБЛЕМЫ

### 1. MEMORY LEAKS в callback регистрациях (CRITICAL!)

#### Проблема:
В `src/network/cf_network.c` три функции выделяют память и делают `Py_INCREF` БЕЗ cleanup:

```c
// src/network/cf_network.c:1490-1506
python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
l_ctx->callback = l_callback;
l_ctx->user_data = l_user_data;

Py_INCREF(l_callback);   // ❌ NO CLEANUP!
Py_INCREF(l_user_data);  // ❌ NO CLEANUP!

// Context registered, но НИКОГДА не освободится!
dap_chain_add_mempool_notify_callback(l_chain, s_python_callback_wrapper, l_ctx);
```

#### Затронутые функции:
1. `dap_chain_add_mempool_notify_callback_py()` - line 1461
2. `dap_chain_net_add_nodelist_notify_callback_py()` - line 1519
3. `dap_chain_net_srv_order_add_notify_callback_py()` - line 1574

#### Последствия:
- Memory leak при каждой регистрации callback
- Python objects никогда не освободятся (reference count не упадёт до 0)
- При долгой работе приложения - accumulation of leaks

#### Решение:
```c
// НУЖНО ДОБАВИТЬ cleanup destructor:
static void python_callback_ctx_cleanup(void *a_ctx) {
    python_callback_ctx_t *l_ctx = (python_callback_ctx_t*)a_ctx;
    if (l_ctx) {
        Py_XDECREF(l_ctx->callback);
        Py_XDECREF(l_ctx->user_data);
        DAP_DELETE(l_ctx);
    }
}

// И зарегистрировать его:
// Но проблема - dap_chain_add_mempool_notify_callback() НЕ принимает cleanup!
// Нужно либо:
// 1. Track callbacks в глобальном registry с cleanup при module unload
// 2. Или добавить unregister функцию в SDK
```

#### Статус: REQUIRES Phase 13 - Memory Management Fix

---

### 2. АНАЛОГИЧНАЯ ПРОБЛЕМА в `cf_ledger_callbacks.c`

#### Код выглядит лучше:
```c
// src/ledger/cf_ledger_callbacks.c:263
static void py_ledger_callback_data_cleanup(PyObject *a_capsule) {
    py_ledger_callback_data_t *l_data = ...;
    if (l_data) {
        Py_XDECREF(l_data->callback);  // ✅ ПРАВИЛЬНО!
        Py_XDECREF(l_data->ledger_capsule);
        DAP_DELETE(l_data);
    }
}
```

#### Но cleanup функция НЕ ВЫЗЫВАЕТСЯ!

Проверил функции:
- `dap_ledger_tx_add_notify_py()` - line 278
- `dap_ledger_event_notify_add_py()` - line 325
- `dap_ledger_bridged_tx_notify_add_py()` - line 382

Все делают `Py_INCREF` но НЕ регистрируют cleanup!

#### Статус: REQUIRES Phase 13 - Memory Management Fix

---

### 3. `cf_compose.c` - потенциальная утечка

```c
// src/ledger/tx/cf_compose.c:122
Py_INCREF(l_callback);
s_python_compose_callbacks[slot] = l_callback;
// ❌ Callback хранится в global array, но NO cleanup при unregister!
```

При освобождении slot нужно делать `Py_DECREF`.

---

## ⚠️ СРЕДНИЕ ПРОБЛЕМЫ

### 4. Stub functions вместо реализаций

**Точный список (9 stubs):**

| Файл | Функция | Line | Callbacks требуется |
|------|---------|------|---------------------|
| `cf_ledger_utils.c` | `set_cache_tx_check_callback_py` | 369 | 1 |
| `cf_chain.c` | `add_callback_datum_index_notify_py` | 1113 | 1 |
| `cf_chain.c` | `add_callback_datum_removed_notify_py` | 1146 | 1 |
| `cf_chain.c` | `atom_confirmed_notify_add_py` | 1179 | 1 |
| `cf_chain.c` | `add_callback_timer_py` | 1212 | 1 |
| `cf_ledger_cond.c` | `verificator_add_py` | 238 | 6 |
| `cf_ledger_cond.c` | `voting_verificator_add_py` | 259 | 4 |
| `cf_ledger_callbacks.c` | `service_add_py` | 511 | 1-2 |
| `cf_ledger_callbacks.c` | `tax_callback_set_py` | 529 | 1 |

**ИТОГО:** 9 stub functions, ~18-20 callbacks требуется реализовать

---

### 5. Комментарий "will be freed by Python GC" - ЛОЖЬ

```c
// src/network/cf_network.c:1490
// Allocate callback context (will be freed by Python GC when callback is removed)
python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
```

❌ **НЕПРАВДА:** Python GC НЕ знает о C-allocated структурах!  
Структура выделена через `DAP_NEW_Z` (malloc), не через Python allocator.

#### Исправление:
```c
// Allocate callback context (MUST BE FREED manually - see Phase 13)
// TODO Phase 13: Add cleanup mechanism for callback contexts
```

---

### 6. TODO в cf_compose.c - устаревшая функция

```c
// src/ledger/tx/cf_compose.c использует СТАРЫЙ API:
dap_chain_tx_compose_service_callback_register(...)
```

Эта функция больше НЕ существует! Весь `cf_compose.c` отключён, требует рефакторинга.

---

## ✅ ХОРОШИЕ ПРАКТИКИ (что работает правильно)

### 1. GIL Management в существующих callbacks

```c
// src/network/cf_network.c:1431
PyGILState_STATE l_gstate = PyGILState_Ensure();
// ... Python calls ...
PyGILState_Release(l_gstate);
```

✅ **ПРАВИЛЬНО!** Acquire GIL перед Python вызовами, release после.

---

### 2. Error handling в ledger callbacks

```c
// src/ledger/cf_ledger_callbacks.c:250-254
if (PyErr_Occurred()) {
    PyErr_Print();
    PyErr_Clear();
}
```

✅ **ПРАВИЛЬНО!** Обработка Python exceptions в C callbacks.

---

### 3. Capsule проверки

```c
if (!PyCapsule_CheckExact(l_obj)) {
    PyErr_SetString(PyExc_TypeError, "Expected capsule");
    return NULL;
}
```

✅ **ПРАВИЛЬНО!** Валидация типов перед использованием.

---

## 🔧 РЕКОМЕНДАЦИИ ПО ИСПРАВЛЕНИЮ

### PHASE 13 (НОВАЯ!): Memory Management Fixes

**Приоритет:** 🔴 **CRITICAL**  
**Время:** 4-6 часов

#### Задачи:

1. **Создать глобальный callback registry**
   ```c
   // src/include/cf_callbacks_registry.h
   typedef struct {
       PyObject *callback;
       PyObject *user_data;
       void *sdk_context;  // Для unregister
   } registered_callback_t;
   
   void cf_callbacks_registry_add(PyObject *callback, PyObject *data, void *ctx);
   void cf_callbacks_registry_cleanup_all();  // При module unload
   ```

2. **Добавить cleanup при module finalization**
   ```c
   // src/cellframe.c - в module cleanup:
   cf_callbacks_registry_cleanup_all();
   ```

3. **Или альтернатива:** Использовать capsule destructors где возможно
   ```c
   PyObject *l_ctx_capsule = PyCapsule_New(l_ctx, "python_callback_ctx_t",
                                            python_callback_ctx_cleanup);
   // Сохранить capsule в global list для cleanup
   ```

---

### PHASE 10: Callback Stubs Implementation

Использовать паттерн из `cf_network.c:1423-1453` для всех stubs:

```c
// Template для всех callbacks:
static void s_my_callback_wrapper(SDK_ARGS..., void *a_arg) {
    python_callback_ctx_t *l_ctx = (python_callback_ctx_t*)a_arg;
    if (!l_ctx || !l_ctx->callback) return;
    
    PyGILState_STATE gstate = PyGILState_Ensure();
    
    // Wrap SDK arguments в capsules
    PyObject *args = Py_BuildValue("(...)", ...);
    PyObject *result = PyObject_CallObject(l_ctx->callback, args);
    
    Py_XDECREF(args);
    Py_XDECREF(result);
    if (PyErr_Occurred()) {
        PyErr_Print();
        PyErr_Clear();
    }
    
    PyGILState_Release(gstate);
}
```

---

## 📊 Статистика проблем

| Категория | Количество | Приоритет | Phase |
|-----------|------------|-----------|-------|
| **Memory leaks** | 3+ места | 🔴 CRITICAL | 13 (NEW) |
| **Stub callbacks** | 9 functions (~18-20 callbacks) | 🔴 HIGH | 10 |
| **Отсутствующие модули** | 3 (governance, policy, wallet-shared) | 🟡 MEDIUM | 7-9 |
| **TX Compose refactor** | 3 files | 🔴 HIGH | 3.5 |
| **Python layer bloat** | ~10 files | 🟡 MEDIUM | 11 |
| **SOLID violations** | TBD | 🟢 LOW | 12 |

---

## 🎯 Приоритизация исправлений

### Sprint 1 (Критичные проблемы)
1. **Phase 13** - Memory Management Fixes (4-6h) - 🔴 CRITICAL
2. **Phase 10** - Callback Implementations (8-10h) - 🔴 HIGH
3. **Phase 3.5** - TX Compose Refactoring (10-12h) - 🔴 HIGH

### Sprint 2 (Новая функциональность)
4. **Phase 7** - Governance Bindings (6-8h)
5. **Phase 8** - Policy Bindings (4-6h)
6. **Phase 9** - Wallet-Shared Bindings (5-7h)
7. **Phase 4** - New API Exposure (4-6h)

### Sprint 3 (Качество кода)
8. **Phase 11** - Python Layer Refactoring (6-8h)
9. **Phase 12** - SOLID Improvements (8-10h)
10. **Phase 5** - Testing & Validation (4-6h)

---

## ✅ Что работает ПРАВИЛЬНО

### Хорошие реализации для использования как примеры:

1. **`cf_network.c:1423`** - `s_python_callback_wrapper()`
   - ✅ Правильный GIL management
   - ✅ Error handling
   - ❌ Но нет cleanup для context!

2. **`cf_ledger_callbacks.c:263`** - `py_ledger_callback_data_cleanup()`
   - ✅ Правильный cleanup с Py_XDECREF
   - ❌ Но НЕ используется как capsule destructor!

3. **`cf_ledger_callbacks.c:210-258`** - полные callback реализации
   - ✅ Правильно wrapping аргументов
   - ✅ Правильный error handling
   - ⚠️ Но cleanup не вызывается!

---

## 📋 Actionable Items

### Immediate (Sprint 1)
- [ ] **CRITICAL:** Fix memory leaks в 6+ callback registrations (cf_network.c + cf_ledger_callbacks.c)
- [ ] **CRITICAL:** Implement callback registry с cleanup при module unload
- [ ] **HIGH:** Реализовать 9 stub functions (~18-20 callbacks) используя pattern из cf_network.c

### Short-term (Sprint 2)
- [ ] Создать governance bindings
- [ ] Создать policy bindings
- [ ] Создать wallet-shared bindings
- [ ] Expose stake-ext cache API
- [ ] Expose TPS metrics API

### Long-term (Sprint 3)
- [ ] Рефакторинг Python Composer на thin wrappers
- [ ] SOLID compliance review
- [ ] Comprehensive testing

---

**Дата:** 2025-01-11  
**Reviewer:** AI Agent (Cursor IDE)  
**Severity Level:** 🔴 CRITICAL (memory leaks)  
**Estimated Fix Time:** 4-6 hours (Phase 13)
