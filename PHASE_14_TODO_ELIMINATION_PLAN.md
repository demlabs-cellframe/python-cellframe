# 🎯 Phase 14: TODO Elimination - Full Implementation Plan

## Философия: "Любой TODO - заглушка"

**Стратегия:** TDD (Test-Driven Development)
1. Написать unit тест с мокированием зависимостей
2. Реализовать функционал до прохождения теста
3. Интеграционное тестирование
4. Удалить TODO комментарий

---

## 📊 Категоризация TODO (14 критичных)

### Категория A: Voting Service (PRIORITY: HIGH)
- **Файл:** `modules/service/voting/dap_chain_net_srv_voting_compose.c`
- **Строки:** 325, 337
- **TODO:**
  1. `Register voting_vote when fully implemented`
  2. `Unregister vote when implemented`

### Категория B: Light Node Mode (PRIORITY: MEDIUM)
- **Файл:** `modules/net/dap_chain_net.c`
- **Строка:** 1931
- **TODO:** `implement light mode`

### Категория C: Blocks Threshold (PRIORITY: MEDIUM)
- **Файл:** `modules/type/blocks/dap_chain_type_blocks.c`
- **Строка:** 2019
- **TODO:** `reimplement and enable threshold for blocks`

### Категория D: Performance Optimization (PRIORITY: LOW)
- **Файл:** `modules/net/srv/base/dap_chain_net_srv_ch.c`
- **Строка:** 802
- **TODO:** `change it to less CPU consuming algorithm`

### Категория E: Net Balancer Refactoring (PRIORITY: MEDIUM)
- **Файл:** `modules/net/dap_chain_net_balancer.c`
- **Строка:** 609
- **TODO:** `change signature and implementation`

### Категория F: Xchange Service (PRIORITY: HIGH)
- **Файл:** `modules/service/xchange/dap_chain_net_srv_xchange.c`
- **Строки:** 177, 187, 279, 1306, 1361, 2185
- **TODO:**
  1. `Implement ledger transaction add notification`
  2. `Implement ledger transaction remove notification`
  3. `Temporarily disabled sorting (migration needed)`
  4. `add filters to list` (3 места)

### Категория G: Stake Service (PRIORITY: LOW)
- **Файл:** `modules/service/stake/dap_chain_net_srv_stake_tx_builder.c`
- **Строка:** 323
- **TODO:** `Unregister other types`

### Категория H: CLI JSON wrappers (PRIORITY: LOW)
- **Файл:** `modules/service/xchange/dap_chain_net_srv_xchange_compose.c`
- **Строка:** 932
- **TODO:** `Implement CLI wrappers that return JSON`

---

## 🗂️ Фазы и субфазы

### PHASE 14.1: Voting Service - Full Implementation
**Приоритет:** HIGH  
**Время:** 4-6 часов

#### Субфаза 14.1.1: Анализ архитектуры voting
- Изучить `dap_chain_net_srv_voting.h/c`
- Определить интерфейс `voting_vote` TX builder
- Проверить существующие тесты voting

#### Субфаза 14.1.2: Unit тесты для voting_vote
- Создать `tests/integration/service/test_voting_vote.c`
- Мокировать ledger callbacks
- Написать тесты для:
  - Создание vote TX
  - Валидация vote TX
  - Проверка proof-of-stake для голосования

#### Субфаза 14.1.3: Реализация voting_vote TX builder
- Реализовать `s_voting_vote_tx_builder()` в `dap_chain_net_srv_voting_compose.c`
- Зарегистрировать через `dap_chain_tx_compose_register()`
- Удалить TODO на строке 325

#### Субфаза 14.1.4: Реализация voting_vote unregister
- Добавить `dap_chain_tx_compose_unregister("voting_vote")` в deinit
- Удалить TODO на строке 337

#### Субфаза 14.1.5: Интеграционное тестирование
- Запустить тесты voting service
- Проверить регистрацию/дерегистрацию
- Финальная валидация

**Deliverables:**
- ✅ `tests/integration/service/test_voting_vote.c` (100+ LOC)
- ✅ Полная реализация `voting_vote` TX builder
- ✅ 2 TODO удалены

---

### PHASE 14.2: Xchange Service - Ledger Notifications
**Приоритет:** HIGH  
**Время:** 6-8 часов

#### Субфаза 14.2.1: Анализ ledger callback API
- Изучить `dap_chain_ledger_callbacks.h`
- Определить структуру `tx_added` callback
- Проверить существующие реализации

#### Субфаза 14.2.2: Unit тесты для xchange notifications
- Создать `tests/integration/service/test_xchange_notifications.c`
- Мокировать ledger tx add/remove
- Тестировать:
  - Обработка TX добавления
  - Обработка TX удаления
  - Обновление состояния xchange orders

#### Субфаза 14.2.3: Реализация tx_added notification
- Реализовать `s_xchange_ledger_tx_added_callback()`
- Зарегистрировать через `dap_ledger_callbacks_register()`
- Обновить состояние orders при TX add
- Удалить TODO на строке 177

#### Субфаза 14.2.4: Реализация tx_removed notification
- Реализовать `s_xchange_ledger_tx_removed_callback()`
- Обработка revert orders при TX remove
- Удалить TODO на строке 187

#### Субфаза 14.2.5: Интеграционное тестирование
- Тест полного цикла: create order → TX add → match → TX remove
- Проверка консистентности состояния

**Deliverables:**
- ✅ `tests/integration/service/test_xchange_notifications.c` (150+ LOC)
- ✅ 2 callback реализации
- ✅ 2 TODO удалены

---

### PHASE 14.3: Xchange Service - Filters & Sorting
**Приоритет:** MEDIUM  
**Время:** 4-6 часов

#### Субфаза 14.3.1: JSON-C API migration (sorting fix)
- Изучить новый JSON-C API
- Портировать старый код сортировки
- Удалить TODO на строке 279

#### Субфаза 14.3.2: Unit тесты для фильтров
- Создать `tests/unit/service/test_xchange_filters.c`
- Тестировать фильтрацию по:
  - Token ticker
  - Network ID
  - Order status
  - Price range

#### Субфаза 14.3.3: Реализация фильтров для orders list
- Добавить параметры фильтрации в CLI команды
- Реализовать фильтрацию в `s_srv_xchange_order()` (строка 1306)
- Реализовать фильтрацию в `s_srv_xchange_orders()` (строка 1361)
- Реализовать фильтрацию в `s_srv_xchange_tx_list()` (строка 2185)
- Удалить 3 TODO

#### Субфаза 14.3.4: Документация и примеры
- Обновить help текст CLI команд
- Добавить примеры использования фильтров

**Deliverables:**
- ✅ `tests/unit/service/test_xchange_filters.c` (100+ LOC)
- ✅ Фильтрация в 3 функциях
- ✅ 4 TODO удалены (sorting + 3 filters)

---

### PHASE 14.4: Light Node Mode Implementation
**Приоритет:** MEDIUM  
**Время:** 8-10 часов

#### Субфаза 14.4.1: Архитектурный дизайн light mode
- Определить отличия LIGHT vs FULL node
- Спроектировать конфигурацию
- Определить отключаемые модули

#### Субфаза 14.4.2: Unit тесты для light mode
- Создать `tests/unit/net/test_light_mode.c`
- Тестировать:
  - Переключение режимов
  - Disabled features в light mode
  - Memory footprint reduction

#### Субфаза 14.4.3: Реализация light mode switch
- Добавить конфиг параметр `node_role`
- Реализовать `dap_chain_net_set_node_role()`
- Условно отключать тяжёлые модули (consensus, services)
- Удалить TODO на строке 1931

#### Субфаза 14.4.4: Интеграционное тестирование
- Тест запуска в LIGHT mode
- Проверка отключения consensus
- Проверка минимальной функциональности

**Deliverables:**
- ✅ `tests/unit/net/test_light_mode.c` (80+ LOC)
- ✅ Light mode configuration
- ✅ 1 TODO удалён

---

### PHASE 14.5: Blocks Threshold Re-implementation
**Приоритет:** MEDIUM  
**Время:** 4-5 часов

#### Субфаза 14.5.1: Анализ старой реализации
- Git blame для понимания причины отключения
- Изучить performance impact
- Определить threshold алгоритм

#### Субфаза 14.5.2: Unit тесты для threshold
- Создать `tests/unit/type/test_blocks_threshold.c`
- Тестировать:
  - Threshold calculation
  - Block rejection при превышении
  - Performance benchmarks

#### Субфаза 14.5.3: Реализация threshold
- Реимплементировать threshold проверку
- Оптимизировать алгоритм
- Включить проверку
- Удалить TODO на строке 2019

#### Субфаза 14.5.4: Performance валидация
- Benchmark до/после
- Проверка memory leaks
- Stress testing

**Deliverables:**
- ✅ `tests/unit/type/test_blocks_threshold.c` (60+ LOC)
- ✅ Threshold реализация
- ✅ 1 TODO удалён

---

### PHASE 14.6: Performance & Cleanup (LOW PRIORITY)
**Приоритет:** LOW  
**Время:** 3-4 часа

#### Субфаза 14.6.1: Hash algorithm optimization (srv_ch.c:802)
- Benchmark текущего алгоритма
- Выбрать менее CPU-intensive хэш
- Реализовать замену
- Удалить TODO

#### Субфаза 14.6.2: Stake unregister cleanup (stake_tx_builder.c:323)
- Добавить unregister для всех stake TX types
- Удалить TODO

#### Субфаза 14.6.3: Xchange CLI JSON wrappers (xchange_compose.c:932)
- Реализовать JSON wrappers для CLI
- Удалить TODO

#### Субфаза 14.6.4: Net balancer refactoring (net_balancer.c:609)
- Изменить signature функции
- Обновить вызовы
- Удалить TODO

**Deliverables:**
- ✅ 4 TODO удалены
- ✅ Performance improvements

---

## 📊 Общая статистика

| Категория | TODO Count | Priority | Est. Time |
|-----------|------------|----------|-----------|
| Voting | 2 | HIGH | 4-6h |
| Xchange Notifications | 2 | HIGH | 6-8h |
| Xchange Filters | 4 | MEDIUM | 4-6h |
| Light Mode | 1 | MEDIUM | 8-10h |
| Blocks Threshold | 1 | MEDIUM | 4-5h |
| Performance | 4 | LOW | 3-4h |
| **ИТОГО** | **14** | - | **29-39h** |

---

## 🧪 Тестовая инфраструктура

### Новые тестовые файлы:
1. `tests/integration/service/test_voting_vote.c`
2. `tests/integration/service/test_xchange_notifications.c`
3. `tests/unit/service/test_xchange_filters.c`
4. `tests/unit/net/test_light_mode.c`
5. `tests/unit/type/test_blocks_threshold.c`

### Fixtures:
- `tests/fixtures/service_fixtures.h/c` - mock services
- `tests/fixtures/ledger_mock.h/c` - mock ledger callbacks
- `tests/fixtures/net_mock.h/c` - mock network

---

## 🎯 Acceptance Criteria

### Для каждой задачи:
- ✅ Unit тест с мокированием написан и проходит
- ✅ Реализация функционала завершена
- ✅ Интеграционный тест проходит
- ✅ TODO комментарий удалён
- ✅ Код review passed
- ✅ Нет memory leaks (valgrind clean)

### Общие критерии фазы 14:
- ✅ **14 TODO удалены**
- ✅ **100% test coverage** новых реализаций
- ✅ **Нет регрессий** (existing tests pass)
- ✅ **Документация обновлена**
- ✅ **Компиляция без warnings**

---

## 🚀 Execution Strategy

1. **Начать с HIGH priority** (Voting, Xchange)
2. **Параллельно писать тесты и реализацию**
3. **Использовать dap_mock framework**
4. **Commit после каждой субфазы**
5. **Финальный review после всей фазы**

---

## 📦 Dependencies

- dap_mock (для unit тестов)
- test_fixtures (для интеграционных тестов)
- CMake тестовая инфраструктура (уже настроена)

**Готово к старту!** 🚀
