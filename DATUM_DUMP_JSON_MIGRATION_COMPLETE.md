# ✅ Migration Complete: dap_chain_datum_dump_json → ledger module

## Проблема
Функция `dap_chain_datum_dump_json` находилась в модуле `datum`, но требовала доступ к `ledger` для получения TX ticker. Это создавало **скрытую кольцевую зависимость** через callback-паттерн.

## Решение
Функция **полностью перенесена** из `modules/datum/dap_chain_datum.c` в `modules/ledger/dap_chain_ledger_json.c`.

## Что было сделано

### 1. ✅ Перенос функции в ledger модуль
- **Файл:** `cellframe-sdk/modules/ledger/dap_chain_ledger_json.c`
- **Функция:** `dap_chain_datum_dump_json()` - полная реализация
- **Вспомогательные:** `s_token_dump_decl_json()`, `s_token_dump_update_json()`
- **Все includes:** Добавлены все необходимые заголовки для datum, token, decree, anchor

### 2. ✅ Удаление из datum модуля
- **Файл:** `cellframe-sdk/modules/datum/dap_chain_datum.c`
- **Удалено:** Строки 703-865 (функция `dap_chain_datum_dump_json` и комментарий)
- **Сохранено:** Функции-помощники `s_token_dump_decl_json` и `s_token_dump_update_json`
  - **Причина:** Они используются в `dap_chain_datum_dump_tx_json()` которая остаётся в datum

### 3. ✅ Очистка зависимостей
- **Удалено из datum.c:**
  - Callback `s_get_ledger_callback`
  - Функция регистрации `dap_chain_datum_register_get_ledger_callback()`
  - Forward declaration `typedef struct dap_ledger dap_ledger_t`
  - extern для `dap_ledger_tx_get_token_ticker_by_hash`

- **Удалено из datum.h:**
  - `typedef dap_chain_datum_get_ledger_callback_t`
  - `void dap_chain_datum_register_get_ledger_callback()`

- **Удалено из net.c:**
  - Регистрация callback в `dap_chain_net_init()`

## Архитектурные улучшения

### ❌ Было (ПЛОХО)
```
datum (низкий уровень)
  ↓ (callback для ledger)
  ↑ (использует ledger API)
ledger (высокий уровень)

= СКРЫТАЯ КОЛЬЦЕВАЯ ЗАВИСИМОСТЬ!
```

### ✅ Стало (ХОРОШО)
```
datum (низкий уровень)
  ↑ (использует только datum API)
ledger (высокий уровень)
  ↓ (вызывает datum dump_tx_json напрямую)
  
= ЧИСТАЯ ИЕРАРХИЯ ЗАВИСИМОСТЕЙ!
```

## Функциональность

### TX Ticker в JSON дампе
- **Было:** NULL (отсутствовал из-за отказа от callback)
- **Стало:** Полный ticker через `dap_ledger_find_by_net_id()` → `dap_ledger_tx_get_token_ticker_by_hash()`

### Вызовы из CLI
Все CLI команды (`ledger list`, `mempool list`, chain dumps) теперь вызывают:
```c
// Напрямую из ledger модуля - правильное место!
dap_chain_datum_dump_json(...)  // теперь в ledger/dap_chain_ledger_json.c
```

## Компиляция

### ✅ cellframe-sdk
```bash
make
# [100%] Built target cellframe_sdk
# ✅ SUCCESS
```

### ✅ python-cellframe
```bash
make
# [100%] Built target dap_core_unix
# ✅ SUCCESS
```

## Размеры файлов

| Файл | Было | Стало | Изменение |
|------|------|-------|-----------|
| `datum/dap_chain_datum.c` | 865 строк | 702 строки | **-163 строки** |
| `ledger/dap_chain_ledger_json.c` | 109 строк | 337 строк | **+228 строк** |

## Итог

✅ **Кольцевая зависимость устранена**  
✅ **Функциональность восстановлена** (TX ticker в JSON)  
✅ **Архитектура исправлена** (datum → ledger, не наоборот)  
✅ **Всё компилируется** без ошибок и предупреждений  
✅ **Код чище** (никаких callback-костылей)

**FAIL-FAST стратегия соблюдена:** Если нет ledger → FAIL, не NULL!
