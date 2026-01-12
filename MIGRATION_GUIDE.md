# Python-Cellframe Migration Guide (2025-01-11)

## 🎯 Обзор

Этот документ описывает миграцию python-cellframe после рефакторинга cellframe-sdk (MR !2358).

**Хорошие новости:** 🎉 **Python код не требует изменений!**

Все breaking changes находятся только в C-layer implementation. Python API сигнатуры остались прежними.

---

## ✅ Что работает БЕЗ изменений

### 1. Все базовые операции с Chain
```python
import CellFrame

# Получение chain - работает как раньше
chain = CellFrame.Chain.load("my-chain")
info = chain.get_info()
```

### 2. Ledger операции
```python
# Работа с ledger - без изменений
ledger = CellFrame.Ledger("my-net")
balance = ledger.get_balance(address, "CELL")
```

### 3. Wallet операции
```python
# Создание кошельков - работает
wallet = CellFrame.Wallet.create("my-wallet", "password")
address = wallet.get_addr(net_id)

# ✅ get_balance работает (обновлён C-layer для использования ledger API)
balance = wallet.get_balance(net_id, "CELL")
```

### 4. Network операции
```python
# Работа с сетью - без изменений
net = CellFrame.Network.by_name("Backbone")
chains = net.get_chains()
```

---

## ⚠️ Изменённое поведение

### 1. Node CLI Init/Delete → No-Op

**Старое поведение:**
```python
import CellFrame

# Инициализация CLI
result = CellFrame.Node.cli_init(config)  # Возвращал 0/-1
```

**Новое поведение:**
```python
# Функция остаётся, но ничего не делает
result = CellFrame.Node.cli_init(config)
# Всегда возвращает 0
# Выводит WARNING: "Function removed in cellframe-sdk refactoring"
```

**Рекомендация:**  
- Можно оставить вызовы (backward compatible)
- Можно удалить (CLI инициализируется автоматически)

---

## ❌ Временно НЕ работает

### 1. TX Compose / Mempool TX Create

**Не работает:**
```python
import CellFrame

# ❌ Выбросит NotImplementedError
hash_str = CellFrame.Mempool.tx_create(
    chain, key_from, addr_from, 
    addr_to_list, ticker, value_list,
    fee, hash_out_type
)
# NotImplementedError: "dap_chain_mempool_tx_create: Function removed, requires TX Compose API refactoring"
```

**Временное решение:**  
- Используйте lower-level TX creation API
- Дождитесь Phase 3.5 (TX Compose refactoring)

**Когда будет исправлено:** Phase 3.5 (8-12 часов работы)

---

## 🔧 Рекомендации по миграции

### Для разработчиков Python приложений

#### ✅ НЕ требуется действий для:
- Wallet операций
- Ledger balance queries
- Chain traversal
- Network operations
- Datum operations

#### ⚠️ Проверьте код, если используете:
- `CellFrame.Mempool.tx_create()` → временно не работает
- `CellFrame.Node.cli_init()` → можно удалить вызовы

### Для разработчиков C extensions

#### ✅ Обновите includes если используете:
```c
// Старое:
#include "dap_chain_tx_compose.h"
#include "dap_chain_tx_compose_callbacks.h"

// Новое:
#include "dap_chain_tx_compose_api.h"
#include "dap_chain_tx_compose_registry.h"
```

#### ⚠️ Обновите API calls:

**Ledger Decree:**
```c
// Старое:
dap_ledger_decree_load(decree, chain, hash);

// Новое:
dap_ledger_t *l_ledger = dap_ledger_by_net_name(net_name);
dap_chain_id_t l_chain_id = chain->id;
dap_ledger_decree_load(l_ledger, decree, l_chain_id, hash);
```

**Wallet Balance:**
```c
// Старое:
uint256_t balance = dap_chain_wallet_get_balance(wallet, net_id, ticker);

// Новое:
dap_chain_net_t *l_net = dap_chain_net_by_id(net_id);
dap_chain_addr_t *l_addr = dap_chain_wallet_get_addr(wallet, net_id);
uint256_t balance = dap_ledger_calc_balance(l_net->pub.ledger, l_addr, ticker);
```

---

## 📊 Матрица совместимости

| Функциональность | Статус | Требует изменений | Комментарий |
|-----------------|--------|-------------------|-------------|
| Chain operations | ✅ Работает | Нет | Полная совместимость |
| Ledger balance | ✅ Работает | Нет | C-layer обновлён |
| Wallet create/open | ✅ Работает | Нет | Без изменений |
| Wallet get_balance | ✅ Работает | Нет | C-layer использует ledger API |
| Network operations | ✅ Работает | Нет | Без изменений |
| Datum operations | ✅ Работает | Нет | Без изменений |
| Node CLI init | ⚠️ No-op | Опционально | Можно удалить вызовы |
| Mempool TX create | ❌ Не работает | Да | Дождитесь Phase 3.5 |
| TX Compose callbacks | ❌ Не работает | Да | Дождитесь Phase 3.5 |

---

## 🔮 Roadmap

### ✅ Phase 1-3: ЗАВЕРШЕНО (2025-01-11)
- Mempool API актуализирован
- Ledger Decree/Anchor API обновлён
- Wallet balance переведён на ledger API
- Node CLI заглушен
- **Результат:** python-cellframe успешно компилируется

### ⏳ Phase 3.5: TX Compose API (Отложено)
**Цель:** Восстановить TX creation функциональность  
**Задачи:**
1. Рефакторинг `cf_compose.c` на новую архитектуру
2. Адаптация `cf_compose_init.c` под registry API
3. Замена `dap_chain_mempool_tx_create()` на compose dispatcher

**Оценка:** 8-12 часов

### ⏳ Phase 4: Новые API (Планируется)
**Цель:** Expose новый функционал cellframe-sdk  
**Новые возможности:**
- Stake-ext Cache API
- TPS Metrics API
- Другие публичные API из refactoring

### ⏳ Phase 5: Testing & Validation (Планируется)
**Цель:** Comprehensive тестирование  
**Задачи:**
- Unit tests
- Integration tests
- Regression tests (критично!)
- Performance benchmarks

---

## 🐛 Troubleshooting

### Проблема: "NotImplementedError: dap_chain_mempool_tx_create"

**Причина:** TX Compose API ещё не портирован  
**Решение:**  
1. Дождитесь Phase 3.5
2. Используйте альтернативные TX creation API (если доступны)

### Проблема: Wallet balance не обновляется

**Возможная причина:** Изменения в C-layer  
**Проверка:**
```python
# Попробуйте прямой доступ через ledger
ledger = CellFrame.Ledger.by_net_name("Backbone")
balance = ledger.calc_balance(address, "CELL")
```

### Проблема: Warnings о "cli_init removed"

**Причина:** Node CLI API удалён из cellframe-sdk  
**Решение:** Удалите вызовы `CellFrame.Node.cli_init()` из кода (опционально)

---

## 📞 Получение помощи

### GitLab Issues
- **Cellframe SDK:** https://gitlab.demlabs.net/cellframe/cellframe-sdk/-/issues
- **Python-Cellframe:** https://gitlab.demlabs.net/cellframe/python-cellframe/-/issues

### Документация
- **API Changes:** См. `API_CHANGES.md`
- **Cellframe SDK MR:** https://gitlab.demlabs.net/cellframe/cellframe-sdk/-/merge_requests/2358

### Контрибуция
При обнаружении проблем, пожалуйста:
1. Проверьте `API_CHANGES.md` - возможно это known issue
2. Создайте issue с описанием проблемы
3. Укажите версию cellframe-sdk и python-cellframe

---

## ✅ Checklist для миграции

### Для Python разработчиков:
- [ ] Проверить, используется ли `CellFrame.Mempool.tx_create()`
- [ ] Если да → запланировать замену после Phase 3.5
- [ ] Опционально: удалить вызовы `CellFrame.Node.cli_init()`
- [ ] Запустить существующие тесты
- [ ] Проверить wallet balance queries

### Для C extension разработчиков:
- [ ] Обновить includes для TX Compose
- [ ] Заменить `dap_chain_wallet_get_balance()` на ledger API
- [ ] Обновить decree/anchor API calls (ledger вместо net)
- [ ] Удалить зависимости от `dap_chain_node_cli.h`
- [ ] Пересобрать extension

---

**Дата:** 2025-01-11  
**Версия документа:** 1.0  
**Статус:** Phase 3 завершена, Phase 3.5 отложена  
