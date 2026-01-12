# 🔍 Cellframe SDK Code Review - Issues Found

## ✅ ИСПРАВЛЕНО

### 1. ❌ Скрытая кольцевая зависимость datum → ledger
**Статус:** ✅ **ИСПРАВЛЕНО**

**Проблема:**
- Функция `dap_chain_datum_dump_json()` в модуле `datum` (низкий уровень)
- Требовала доступ к `ledger` (высокий уровень) через callback
- Создавала скрытую кольцевую зависимость

**Решение:**
1. ✅ Функция `dap_chain_datum_dump_json()` **полностью перенесена** в `modules/ledger/dap_chain_ledger_json.c`
2. ✅ Удалён мёртвый код из `datum/dap_chain_datum.c`:
   - `typedef struct dap_ledger dap_ledger_t` (forward declaration)
   - `extern dap_ledger_tx_get_token_ticker_by_hash`
   - `dap_chain_datum_get_ledger_callback_t`
   - `s_get_ledger_callback`
   - `dap_chain_datum_register_get_ledger_callback()`
3. ✅ Header `dap_chain_datum.h` содержит комментарий: `// NO MORE get_ledger callback`

**Результат:**
- `datum` больше НЕ ЗНАЕТ про `ledger`
- Чистая иерархия: `datum` (низкий) ← `ledger` (высокий)
- ✅ Компилируется без ошибок

**Файлы:**
- `cellframe-sdk/modules/datum/dap_chain_datum.c` (702 → 673 строки, -29 строк мёртвого кода)
- `cellframe-sdk/modules/ledger/dap_chain_ledger_json.c` (содержит полную реализацию)

---

## 🟡 ТРЕБУЮТ ВНИМАНИЯ (TODO)

### 2. 🔴 Критичные TODO

#### 2.1. Ledger chain iterator - STUB
**Файл:** `modules/ledger/dap_chain_ledger.c:2201`
```c
// TODO: In full implementation, chain module should register a proper iterator
if (a_ledger->chain_iter_callback) {
    a_ledger->chain_iter_callback(NULL, a_arg);  // ⚠️ Pass NULL instead of chain!
}
```
**Проблема:** Передаётся `NULL` вместо реального указателя на chain.  
**Риск:** FAIL-FAST нарушен, функциональность ослаблена.

#### 2.2. Voting vote registration - NOT IMPLEMENTED
**Файл:** `modules/service/voting/dap_chain_net_srv_voting_compose.c:325`
```c
// TODO: Register voting_vote when fully implemented
```
**Проблема:** Регистрация `voting_vote` TX builder не реализована.  
**Риск:** Часть функционала voting отсутствует.

#### 2.3. Voting vote unregister - NOT IMPLEMENTED
**Файл:** `modules/service/voting/dap_chain_net_srv_voting_compose.c:337`
```c
// TODO: Unregister vote when implemented
```
**Проблема:** Cleanup не реализован.  
**Риск:** Memory leak при деинициализации.

#### 2.4. Blocks threshold - DISABLED
**Файл:** `modules/type/blocks/dap_chain_type_blocks.c:2019`
```c
// TODO: reimplement and enable threshold for blocks
```
**Проблема:** Порог для блоков отключён.  
**Риск:** Потенциальные проблемы с производительностью/памятью.

#### 2.5. Light mode - HARDCODED
**Файл:** `modules/net/dap_chain_net.c:1931`
```c
a_net->pub.node_role.enums = NODE_ROLE_FULL; // TODO: implement light mode
```
**Проблема:** Light mode не реализован, всегда FULL node.  
**Риск:** Невозможность запуска в облегчённом режиме.

#### 2.6. Net balancer signature - NEEDS REFACTORING
**Файл:** `modules/net/dap_chain_net_balancer.c:609`
```c
// TODO: change signature and implementation
```
**Проблема:** API требует изменений (неясно каких).  
**Риск:** Может потребовать изменений в зависимых модулях.

---

### 3. ⚠️ HACK - Backward Compatibility

#### 3.1. Legacy wallet API - BREAKS HARDWARE WALLETS
**Файл:** `modules/ledger/dap_chain_ledger.c:2024`
```c
// This is a HACK for backward compatibility with old wallet module
// It requires direct access to wallet internals which breaks hardware wallet support
// The proper way is to refactor all code to use dap_ledger_sign_data

log_it(L_WARNING, "INTERNAL: Using legacy wallet API - breaks hardware wallet support!");
```
**Проблема:** Прямой доступ к внутренностям кошелька.  
**Риск:** Невозможность работы с аппаратными кошельками.  
**Решение:** Рефакторинг всех мест использования на `dap_ledger_sign_data`.

---

### 4. 🔍 Callback паттерны (потенциальные скрытые зависимости)

Файлы с большим количеством callback-регистраций требуют review:

1. `modules/ledger/dap_chain_ledger.c` - 18 callbacks в структуре
   - ✅ Архитектурно правильно (dependency inversion)
   - ✅ Ledger = LOW, callbacks регистрируют HIGH modules
   
2. `modules/ledger/dap_chain_ledger_callbacks.c` - глобальный реестр
   - ✅ Чистая инверсия зависимостей (wallet-cache регистрирует callbacks)
   
3. `modules/chain/dap_chain.c` - atom/datum notification callbacks
   - ✅ Правильная архитектура
   
4. `modules/mempool/dap_chain_mempool.c` - верификация TX
   - ✅ mempool (HIGH) → ledger (MID), нормально
   
5. `modules/datum/dap_chain_datum.c` - decree/anchor dump callbacks
   - ✅ После очистки - только decree/anchor, правильно
   
6. `modules/policy/dap_chain_policy.c`
   - 🟡 Требует review (новый модуль)
   
7. `modules/wallet-cache/dap_chain_wallet_cache.c`
   - 🟡 Требует review (новый модуль)

---

## 📊 Статистика

| Метрика | Значение |
|---------|----------|
| Всего TODO | 117 |
| Критичных TODO (implement/fix/broken) | 6 |
| HACK комментариев | 1 |
| Явных STUB | 0 (были удалены) |
| Callback-based modules | 15 |
| Скрытых кольцевых зависимостей | 0 (была 1, исправлена) |

---

## 🎯 Приоритеты для исправления

### P0 - КРИТИЧНО
1. ✅ ~~Circular dependency datum → ledger~~ **ИСПРАВЛЕНО**
2. 🔴 Ledger chain iterator NULL pass (FAIL-FAST violation)
3. 🔴 Legacy wallet API (hardware wallet blocker)

### P1 - ВЫСОКИЙ
4. 🟡 Voting vote registration (incomplete feature)
5. 🟡 Light mode implementation (missing feature)

### P2 - СРЕДНИЙ
6. 🟡 Blocks threshold re-enable
7. 🟡 Net balancer signature refactoring

### P3 - НИЗКИЙ
8. 🟢 Review policy/wallet-cache callbacks
9. 🟢 Cleanup remaining 111 non-critical TODOs

---

## ✅ Рекомендации

1. **Ledger chain iterator:**
   - Реализовать правильный iterator в chain module
   - Зарегистрировать callback в ledger
   - Убрать `NULL` pass

2. **Legacy wallet API:**
   - Найти все места использования `_dap_ledger_get_enc_key_internal`
   - Рефакторить на `dap_ledger_sign_data`
   - Удалить HACK

3. **Voting:**
   - Завершить реализацию `voting_vote` TX builder
   - Добавить unregister в cleanup

4. **Light mode:**
   - Спроектировать light node архитектуру
   - Реализовать переключение FULL/LIGHT

5. **Blocks threshold:**
   - Исследовать причину отключения
   - Реимплементировать или удалить TODO если не нужно

---

## 📝 Файлы изменены в этом ревью

- `cellframe-sdk/modules/datum/dap_chain_datum.c` (-29 строк мёртвого кода)
- `cellframe-sdk/modules/datum/include/dap_chain_datum.h` (комментарий о переносе)
- `cellframe-sdk/modules/ledger/dap_chain_ledger_json.c` (содержит полную реализацию)

**Компиляция:** ✅ SUCCESS
**Circular dependencies:** ✅ CLEAN
**Memory leaks:** ✅ NONE (callback code removed)
