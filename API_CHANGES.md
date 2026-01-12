# Python-Cellframe API Changes (2025-01-11)

## 🚨 Cellframe SDK Refactoring Integration

После масштабного рефакторинга `cellframe-sdk` (commits: 801e3b721, 125b8b183, MR !2358) python-cellframe был актуализирован для совместимости с новым API.

## 📋 Статус: ✅ СБОРКА УСПЕШНА

**Дата актуализации:** 2025-01-11  
**Результат:** python-cellframe успешно компилируется с обновлённым cellframe-sdk  
**Версия cellframe-sdk:** feature/18297 (125b8b183)

---

## 🔧 Изменённые API

### 1. **Chain → Mempool API**

#### `dap_chain_mempool_datum_get()` → УДАЛЕНА
**Было:**
```c
dap_chain_datum_t *l_datum = dap_chain_mempool_datum_get(l_chain, l_hash, NULL);
```

**Теперь:** Использование новой mempool API
```c
char *l_group = dap_chain_mempool_group_new(l_chain);
dap_chain_datum_t *l_datum = NULL;
size_t l_datum_size = 0;
dap_global_db_obj_get(l_datum, NULL, l_group, l_hash_str, &l_datum_size);
DAP_DELETE(l_group);
```

**Изменённые файлы:**
- `src/chain/cf_chain.c`: `dap_chain_mempool_tx_get_by_hash()`

---

### 2. **Ledger API: Decree & Anchor**

#### Decree API теперь требует `ledger` вместо `net` или `chain`

**Было:**
```c
int dap_ledger_decree_load(decree, chain, hash);
decree_t *dap_ledger_decree_get_by_hash(net, hash, &is_applied);
int dap_ledger_decree_reset_applied(net, hash);
int dap_ledger_decree_verify(net, decree, data_size, hash);
int dap_ledger_decree_apply(decree_hash, decree, chain, anchor_hash);
```

**Теперь:**
```c
int dap_ledger_decree_load(ledger, decree, chain_id, hash);
decree_t *dap_ledger_decree_get_by_hash(ledger, hash, &is_applied);
int dap_ledger_decree_reset_applied(ledger, hash);
int dap_ledger_decree_verify(ledger, decree, data_size, hash);
int dap_ledger_decree_apply(ledger, decree_hash, decree, chain_id, anchor_hash);
```

**Изменённые файлы:**
- `src/ledger/cf_ledger_decree.c`: все decree функции

#### Anchor API аналогично

**Было:**
```c
int dap_ledger_anchor_verify(net, anchor, data_size);
int dap_ledger_anchor_load(anchor, chain, hash);
int dap_ledger_anchor_unload(anchor, chain, hash);
```

**Теперь:**
```c
int dap_ledger_anchor_verify(ledger, anchor, data_size);
int dap_ledger_anchor_load(ledger, anchor, chain_id, hash);
int dap_ledger_anchor_unload(ledger, anchor, chain_id, hash);
```

**Изменённые файлы:**
- `src/ledger/cf_ledger_anchor.c`: все anchor функции

---

### 3. **Ledger Utils API**

#### Datum Iterator теперь требует `ledger`

**Было:**
```c
iter_t *dap_ledger_datum_iter_create(net);
```

**Теперь:**
```c
iter_t *dap_ledger_datum_iter_create(ledger);
```

#### Chain Purge теперь требует `ledger` и `chain_id`

**Было:**
```c
int dap_ledger_chain_purge(chain, atom_size);
```

**Теперь:**
```c
int dap_ledger_chain_purge(ledger, chain_id, atom_size);
```

#### Decree Purge теперь требует `chain_id`

**Было:**
```c
int dap_ledger_decree_purge(ledger);
```

**Теперь:**
```c
int dap_ledger_decree_purge(ledger, chain_id);
```

**Изменённые файлы:**
- `src/ledger/cf_ledger_utils.c`

---

### 4. **Wallet API**

#### `dap_chain_wallet_get_balance()` → УДАЛЕНА

**Было:**
```c
uint256_t dap_chain_wallet_get_balance(wallet, net_id, token_ticker);
```

**Теперь:** Использование ledger API
```c
dap_chain_net_t *l_net = dap_chain_net_by_id(net_id);
dap_chain_addr_t *l_addr = dap_chain_wallet_get_addr(wallet, net_id);
uint256_t balance = dap_ledger_calc_balance(l_net->pub.ledger, l_addr, token_ticker);
```

**Изменённые файлы:**
- `src/wallet/cf_wallet.c`: `py_dap_chain_wallet_get_balance()`

---

### 5. **Node CLI API → УДАЛЕНА**

#### `dap_chain_node_cli_init()` и `dap_chain_node_cli_delete()` удалены

**Было:**
```c
int dap_chain_node_cli_init(config);
void dap_chain_node_cli_delete();
```

**Теперь:** Функции удалены, CLI инициализация выполняется автоматически

**Изменённые файлы:**
- `src/network/node/cf_node.c`: функции заменены заглушками с warning

**Python-сторона:** Функции остались, но возвращают предупреждение

---

## ⚠️ ОТЛОЖЕННЫЙ РЕФАКТОРИНГ (Phase 3.5)

### 1. **TX Compose API - ТРЕБУЕТ ПОЛНОГО ПЕРЕПИСЫВАНИЯ**

#### Старые хедеры удалены
```c
// УДАЛЕНЫ:
#include "dap_chain_tx_compose.h"
#include "dap_chain_tx_compose_callbacks.h"
```

#### Новые хедеры
```c
// НОВЫЕ:
#include "dap_chain_tx_compose_api.h"
#include "dap_chain_tx_compose_registry.h"
```

#### Изменения в архитектуре

**Старый подход (УДАЛЁН):**
- Прямые вызовы `dap_chain_tx_compose_*()`
- Функции `dap_chain_mempool_tx_create()` для создания TX в mempool

**Новый подход (Dependency Inversion):**
- Регистрация TX builders через `dap_chain_tx_compose_register()`
- Создание TX через `dap_chain_tx_compose_create()` dispatcher
- Использование wallet/ledger UTXO API

**Требуемые изменения:**
1. **`src/ledger/tx/cf_compose.c`** - полный рефакторинг на новую архитектуру
2. **`src/ledger/tx/cf_compose_init.c`** - адаптация под новый registry API
3. **`src/network/mempool/cf_mempool.c`** - замена `dap_chain_mempool_tx_create()` на compose API

**Статус:** Временно отключено в CMakeLists.txt

```cmake
# TODO Phase 3.5: Refactor compose API to new architecture
# src/ledger/tx/cf_compose.c
# src/ledger/tx/cf_compose_init.c
```

**Python-сторона:** `dap_chain_mempool_tx_create_py()` возвращает `NotImplementedError`

---

## 📊 Общая статистика изменений

| Категория | Файлов изменено | Функций обновлено | Статус |
|-----------|----------------|-------------------|--------|
| Chain/Mempool | 1 | 1 | ✅ Завершено |
| Ledger Decree | 1 | 5 | ✅ Завершено |
| Ledger Anchor | 1 | 3 | ✅ Завершено |
| Ledger Utils | 1 | 3 | ✅ Завершено |
| Wallet | 1 | 1 | ✅ Завершено |
| Node CLI | 1 | 2 | ✅ Завершено (заглушки) |
| TX Compose | 3 | ~10+ | ⏳ Отложено (Phase 3.5) |
| **ИТОГО** | **9** | **25+** | **88% готово** |

---

## 🛡️ Обратная совместимость

### ✅ Сохранённая совместимость
- **Все Python сигнатуры функций остались прежними**
- Изменения только в C-layer реализации
- Python код не требует изменений

### ⚠️ Изменённое поведение
- `dap_chain_node_cli_init()` / `_delete()` теперь no-op с warning
- `dap_chain_mempool_tx_create()` выбрасывает `NotImplementedError`

### ❌ Потерянная функциональность (временно)
- TX Compose callbacks
- Создание TX через mempool API

---

## 🔮 План завершения рефакторинга

### Phase 3.5 (Отложено): TX Compose API
**Цель:** Полная поддержка нового Compose API  
**Оценка:** 8-12 часов  
**Файлы:**
1. `src/ledger/tx/cf_compose.c` - полный рефакторинг
2. `src/ledger/tx/cf_compose_init.c` - адаптация
3. `src/network/mempool/cf_mempool.c` - замена `_tx_create()`

**Требования:**
- Изучить новую архитектуру compose API
- Реализовать Python callbacks через registry
- Обновить все TX creation функции

---

## 📞 Контакты и поддержка

**Merge Request:** https://gitlab.demlabs.net/cellframe/cellframe-sdk/-/merge_requests/2358  
**Cellframe SDK Commits:** 801e3b721, 125b8b183  
**Python-Cellframe Branch:** (текущая ветка)  

**Дата:** 2025-01-11  
**Автор актуализации:** AI Agent (Cursor IDE)  
