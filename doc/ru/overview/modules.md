# Основные модули

SDK разделен на несколько основных модулей. Python-слой является тонкой
оберткой над нативными `python_cellframe` и `python-dap` биндингами.

## Пакет CellFrame

### CellFrame.common
- Система контекстов (`ExecutionMode`, `AppContext`, `ContextFactory`).
- Общие исключения и типы.

### CellFrame.chain
- Обертки для кошельков, леджера и транзакций.
- Утилиты для адресов и mempool.
- Переэкспорт части классов для удобства.

### CellFrame.wallet
- Управление кошельками: создание/открытие, адреса, балансы, ключи.

### CellFrame.composer
- Компоновка транзакций (нужны нативные `dap_compose_*` функции).

### CellFrame.network
- Сетевые структуры и утилиты (зависят от нативных биндингов).

### CellFrame.services
- Сервисы (staking, exchange, voting, orders), завязаны на нативный SDK.

### CellFrame.legacy
- Слой совместимости со старым API.

## Модуль DAP (`python-dap`)

Низкоуровневый доступ к сети, криптографии, конфигурации и Global DB.
Смотрите документацию `python-dap` для API `dap.*`.
