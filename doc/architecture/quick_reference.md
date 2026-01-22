# Быстрая справка: Модули DAP SDK и Cellframe SDK

Краткая справочная таблица для быстрого поиска информации о модулях.

## DAP SDK Модули

| Модуль | Слой | Назначение | Основные файлы |
|--------|------|------------|----------------|
| **dap_core** | 0 | Базовые примитивы, события, потоки | `dap_common.h`, `dap_events.h`, `dap_proc_thread.h` |
| **dap_crypto** | 1 | Криптография: ECDSA, Dilithium, Falcon, Kyber512, SPHINCS+ | `dap_enc.h`, `dap_sign.h`, `dap_hash.h` |
| **dap_plugin** | 1 | Система плагинов | `dap_plugin.h` |
| **dap_io** | 2 | Асинхронный I/O (epoll/kqueue/wepoll) | `dap_events.h`, `dap_worker.h` |
| **dap_net_common** | 2 | Общие сетевые типы и HTTP коды | `dap_net_common.h`, `dap_http_status_code.h` |
| **dap_stream** | 3 | Потоковая передача данных | `dap_stream.h`, `dap_stream_worker.h`, `dap_stream_pkt.h` |
| **dap_stream_ch** | 3 | Каналы: gossip, pkt, proc | `dap_stream_ch.h`, `dap_stream_ch_gossip.h` |
| **dap_stream_event** | 3 | События потоков | `dap_stream_event.h` |
| **dap_stream_session** | 3 | Сессии клиентов | `dap_stream_session.h` |
| **dap_client** | 4 | Клиент для подключения к серверам | `dap_client.h` |
| **dap_server** | 4 | Серверы: enc, http, json_rpc, cli, dns, notify | `dap_http_server.h`, `dap_json_rpc.h` |
| **dap_client_http** | 5 | HTTP клиент (без stream зависимостей) | `dap_client_http.h` |
| **dap_link_manager** | 6 | Управление соединениями | `dap_link_manager.h` |
| **dap_global_db** | 7 | Глобальная БД (MDBX/SQLite/PostgreSQL) | `dap_global_db.h` |
| **dap_app_cli** | 8 | CLI интерфейс для приложений | `dap_cli.h`, `dap_cli_server.h` |

## Cellframe SDK Модули

| Модуль | Слой | Назначение | Основные файлы |
|--------|------|------------|----------------|
| **common** | 0 | Базовые типы: uint256_t, hash_t, tsd_t | `dap_common_types.h` |
| **datum** | 1 | Структуры данных: token, decree, anchor, emission | `dap_chain_datum.h`, `dap_chain_datum_token.h` |
| **datum_tx** | 1 | Транзакции и их компоненты | `dap_chain_datum_tx.h`, `dap_chain_datum_tx_items.h` |
| **wallet** | 2 | Кошельки: ключи, подписи, операции | `dap_chain_wallet.h`, `dap_chain_coin.h` |
| **chain** | 3 | Базовый чейн: атомы, цепи, хранилище | `dap_chain.h`, `dap_chain_atom.h` |
| **chain_type_dag** | 3 | DAG структура с событиями | `dap_chain_type_dag.h`, `dap_chain_type_event.h` |
| **chain_type_blocks** | 3 | Block структура с кешем | `dap_chain_type_blocks.h`, `dap_chain_block.h` |
| **chain_type_none** | 3 | Чейн без консенсуса | `dap_chain_type_none.h` |
| **ledger** | 4 | UTXO ledger: балансы, валидация TX | `dap_chain_ledger.h`, `dap_ledger.h` |
| **wallet_cache** | 5 | Кеш адресов | `dap_chain_wallet_cache.h` |
| **wallet_shared** | 5 | Кеш общих транзакций | `dap_chain_wallet_shared.h` |
| **mempool** | 6 | Пул транзакций: очередь, валидация | `dap_chain_mempool.h` |
| **dap_chain_cs** | 7 | Регистр консенсусов: callbacks, validator API | `dap_chain_cs.h`, `dap_chain_validator_api.h` |
| **cs_dag_poa** | 7 | DAG + PoA консенсус | `dap_chain_cs_dag_poa.h` |
| **cs_esbocs** | 7 | Enhanced SBOCS консенсус | `dap_chain_cs_esbocs.h` |
| **cs_block_pow** | 7 | Blocks + PoW консенсус | `dap_chain_cs_block_pow.h` |
| **governance (decree)** | 8 | Указы: decree datum, callbacks | `dap_chain_ledger_decree.h`, `dap_chain_decree_callbacks.h` |
| **policy** | 8 | Политики: anchors, decree handlers | `dap_chain_policy.h`, `dap_chain_policy_anchor.h` |
| **chain_net** | 9 | Сетевой слой: синхронизация, gossip | `dap_chain_net.h`, `dap_chain_net_sync.h` |
| **chain_net_tx** | 9 | TX broadcast | `dap_chain_net_tx.h` |
| **chain_net_decree** | 9 | Сетевые указы | `dap_chain_net_decree.h` |
| **chain_net_srv** | 9 | Базовый класс для сервисов | `dap_chain_net_srv.h` |
| **srv_stake** | 10 | Стейкинг и PoS | `dap_chain_net_srv_stake.h` |
| **srv_xchange** | 10 | Обмен токенов | `dap_chain_net_srv_xchange.h` |
| **srv_voting** | 10 | Голосование | `dap_chain_net_srv_voting.h` |
| **srv_bridge** | 10 | Кросс-чейн мост | `dap_chain_net_srv_bridge.h` |
| **srv_datum** | 10 | Публикация данных | `dap_chain_net_srv_datum.h` |
| **srv_app** | 10 | Приложения | `dap_chain_net_srv_app.h` |
| **srv_app_db** | 10 | БД приложений | `dap_chain_net_srv_app_db.h` |
| **srv_stake_ext** | 10 | Расширенный стейкинг | `dap_chain_net_srv_stake_ext.h` |
| **tx_compose** | 11 | Построение транзакций: создание, подпись, receipt | `dap_chain_tx_compose.h` |

## Ключевые зависимости между слоями

```
DAP SDK:
  Core (0) → Crypto/Plugin (1) → I/O (2) → Stream (3) → Client/Server (4)
           → Global DB (7)

Cellframe SDK:
  Common (0) → Datum (1) → Wallet (2) → Chain (3) → Ledger (4) 
  → Mempool (6) → Consensus (7) → Governance (8) → Network (9) 
  → Services (10) → TX Compose (11)
```

## Разрешенные циклы зависимостей

| Цикл | Решение |
|------|---------|
| **ledger ↔ mempool** | Forward declarations, интерфейсы |
| **chain_net ↔ ledger** | Callbacks |
| **srv_stake → esbocs → srv_stake** | Интерфейс `dap_chain_esbocs_interface.h` |
| **decree ↔ policy** | Односторонняя зависимость |

## Быстрый поиск по функционалу

### Криптография
- **Подписи:** `dap_crypto` → `dap_sign.h`
- **Хеширование:** `dap_crypto` → `dap_hash.h`
- **Шифрование:** `dap_crypto` → `dap_enc.h`

### Транзакции
- **Создание TX:** `tx_compose` → `dap_chain_tx_compose.h`
- **Валидация TX:** `ledger` → `dap_chain_ledger.h`
- **Очередь TX:** `mempool` → `dap_chain_mempool.h`
- **Broadcast TX:** `chain_net_tx` → `dap_chain_net_tx.h`

### Консенсус
- **DAG PoA:** `cs_dag_poa` → `dap_chain_cs_dag_poa.h`
- **ESBOCS:** `cs_esbocs` → `dap_chain_cs_esbocs.h`
- **Block PoW:** `cs_block_pow` → `dap_chain_cs_block_pow.h`

### Сетевое взаимодействие
- **Синхронизация:** `chain_net` → `dap_chain_net_sync.h`
- **Потоки:** `dap_stream` → `dap_stream.h`
- **Сервер:** `dap_server` → HTTP, JSON-RPC, CLI

### Хранение данных
- **Глобальная БД:** `dap_global_db` → MDBX/SQLite
- **Ledger UTXO:** `ledger` → `dap_chain_ledger.h`
- **Блоки/События:** `chain` + типы (DAG/Blocks/None)

## Внешние зависимости

### DAP SDK
- **uthash** - хеш-таблицы
- **json-c** - JSON парсинг
- **shishua** - быстрый PRNG
- **secp256k1** - ECDSA криптография
- **libmdbx** - встраиваемая БД
- **XKCP** - Keccak/SHA3
- **kyber512** - Post-quantum KEM

### Cellframe SDK
- Зависит от всех компонентов DAP SDK
- **libzip** (опционально) - работа с архивами

## Полезные команды

```bash
# Поиск определения модуля
find . -name "CMakeLists.txt" -exec grep -l "module_name" {} \;

# Поиск зависимостей модуля
grep -r "target_link_libraries.*module_name" --include="CMakeLists.txt"

# Генерация графа зависимостей
cmake --graphviz=deps.dot . && dot -Tpng deps.dot -o deps.png
```

---

**Для подробной информации см.:** [module_dependencies.md](module_dependencies.md)
