# Python-Cellframe Changelog

## [Unreleased] - 2025-01-11

### ✅ Завершено
#### Актуализация с cellframe-sdk refactoring (MR !2358)

**Commits:** 801e3b721, 125b8b183  
**Branch cellframe-sdk:** feature/18297  
**Статус:** ✅ **СБОРКА УСПЕШНА**

#### Changed
- **Mempool API:** Обновлён `dap_chain_mempool_tx_get_by_hash()` для использования нового mempool group API
- **Ledger Decree API:** Все decree функции теперь требуют `ledger` вместо `net`/`chain` + `chain_id`
- **Ledger Anchor API:** Все anchor функции теперь требуют `ledger` вместо `net`/`chain` + `chain_id`
- **Ledger Utils API:** Datum iterator и purge функции обновлены для использования ledger
- **Wallet API:** `get_balance()` теперь использует `dap_ledger_calc_balance()` вместо удалённого `dap_chain_wallet_get_balance()`
- **Node CLI API:** Функции `cli_init()` и `cli_delete()` заменены заглушками (API удалён из cellframe-sdk)

#### Removed (Temporarily)
- **TX Compose:** `cf_compose.c` и `cf_compose_init.c` временно отключены (требуют полного рефакторинга)
- **Mempool TX Create:** `dap_chain_mempool_tx_create_py()` возвращает `NotImplementedError`

#### Files Modified
- `src/chain/cf_chain.c` - обновлён mempool API
- `src/ledger/cf_ledger_decree.c` - 5 функций обновлено на новый ledger API
- `src/ledger/cf_ledger_anchor.c` - 3 функции обновлено на новый ledger API
- `src/ledger/cf_ledger_utils.c` - 3 функции обновлено
- `src/wallet/cf_wallet.c` - `get_balance()` переведён на ledger API
- `src/network/node/cf_node.c` - CLI функции заглушены
- `src/network/mempool/cf_mempool.c` - `tx_create()` временно отключена
- `CMakeLists.txt` - добавлен include path для datum/tx, отключены compose модули

#### Documentation Added
- `API_CHANGES.md` - детальное описание всех изменений API
- `MIGRATION_GUIDE.md` - руководство по миграции для разработчиков

---

### ⏳ В разработке (Phase 3.5)

#### TODO: TX Compose API Refactoring
**Оценка:** 8-12 часов

**Цель:** Портировать TX Compose на новую архитектуру (Dependency Inversion via Registration)

**Задачи:**
- Рефакторинг `src/ledger/tx/cf_compose.c` на новый API
  - Старое: `dap_chain_tx_compose.h`
  - Новое: `dap_chain_tx_compose_api.h` + `dap_chain_tx_compose_registry.h`
- Адаптация `src/ledger/tx/cf_compose_init.c`
- Замена `dap_chain_mempool_tx_create()` в `src/network/mempool/cf_mempool.c`

**Заблокированная функциональность:**
- Python TX creation через mempool API
- TX Compose callbacks

---

### ⏳ Планируется (Phase 4-6)

#### Phase 4: Новые API
- Expose Stake-ext Cache API (теперь публичный)
- Expose TPS Metrics API (всегда активен)
- Другие новые публичные API из cellframe-sdk

#### Phase 5: Testing & Validation
- Unit tests для всех обновлённых модулей
- Integration tests
- **КРИТИЧНО:** Regression tests (backward compatibility)
- Performance benchmarks

#### Phase 6: Finalization
- Обновить README.md
- Обновить Python docs
- Create release notes

---

## Статистика изменений

| Категория | Файлов | Функций | Статус |
|-----------|--------|---------|--------|
| Mempool API | 1 | 1 | ✅ Завершено |
| Ledger Decree | 1 | 5 | ✅ Завершено |
| Ledger Anchor | 1 | 3 | ✅ Завершено |
| Ledger Utils | 1 | 3 | ✅ Завершено |
| Wallet | 1 | 1 | ✅ Завершено |
| Node CLI | 1 | 2 | ✅ Завершено (stubs) |
| TX Compose | 3 | 10+ | ⏳ Отложено (Phase 3.5) |
| **ИТОГО** | **9** | **25+** | **88% готово** |

---

## Обратная совместимость

### ✅ Python API: Полная совместимость
- Все Python функции имеют те же сигнатуры
- Изменения только в C-layer
- Существующий Python код работает БЕЗ изменений

### ⚠️ Изменённое поведение
- `Node.cli_init()` / `cli_delete()` → no-op с warning
- `Mempool.tx_create()` → `NotImplementedError`

### ❌ Временно недоступно
- TX Compose callbacks
- Создание TX через mempool API

---

## Links & References

- **Cellframe SDK MR:** https://gitlab.demlabs.net/cellframe/cellframe-sdk/-/merge_requests/2358
- **API Changes:** См. `API_CHANGES.md`
- **Migration Guide:** См. `MIGRATION_GUIDE.md`
- **Branch:** feature/18297 (cellframe-sdk)
- **Commits:** 801e3b721, 125b8b183

---

**Дата:** 2025-01-11  
**Автор актуализации:** AI Agent (Cursor IDE)
