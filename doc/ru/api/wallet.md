# Справочник API Кошелька

Полный справочник API для класса `Wallet` и операций с кошельками.

## Класс: `Wallet`

Высокоуровневое управление кошельками для операций блокчейна CellFrame.

### Конструктор

```python
Wallet(name: str, wallet_dir: str = None, wallet_handle: Any = None)
```

**Параметры:**
- `name` (str): Имя кошелька
- `wallet_dir` (str, опционально): Директория для файлов кошелька. По умолчанию используется системная директория.
- `wallet_handle` (Any, опционально): Нативный C handle кошелька (внутреннее использование)

**Вызывает:**
- `WalletError`: Если создание кошелька не удалось

### Методы класса

#### `create(name: str, wallet_dir: str = None, wallet_type: WalletType = None) -> Wallet`

Создать новый кошелек.

**Параметры:**
- `name` (str): Имя кошелька
- `wallet_dir` (str, опционально): Директория для файлов кошелька
- `wallet_type` (WalletType, опционально): Тип подписи (по умолчанию: SIG_DILITHIUM)

**Возвращает:**
- `Wallet`: Новый экземпляр кошелька

**Вызывает:**
- `WalletError`: Если создание не удалось

**Пример:**
```python
from CellFrame.chain import Wallet, WalletType

wallet = Wallet.create("my_wallet", wallet_type=WalletType.SIG_DILITHIUM)
```

#### `create_with_seed(name: str, seed: str, wallet_dir: str = None, wallet_type: WalletType = None) -> Wallet`

Создать кошелек из seed фразы.

**Параметры:**
- `name` (str): Имя кошелька
- `seed` (str): Seed фраза
- `wallet_dir` (str, опционально): Директория для файлов кошелька
- `wallet_type` (WalletType, опционально): Тип подписи

**Возвращает:**
- `Wallet`: Новый экземпляр кошелька

#### `create_multi(name: str, seed: str, required_signatures: int, wallet_dir: str = None, wallet_type: WalletType = None) -> Wallet`

Создать мультиподписной кошелек.

**Параметры:**
- `name` (str): Имя кошелька
- `seed` (str): Seed фраза
- `required_signatures` (int): Количество требуемых подписей
- `wallet_dir` (str, опционально): Директория для файлов кошелька
- `wallet_type` (WalletType, опционально): Тип подписи

**Возвращает:**
- `Wallet`: Новый экземпляр мультиподписного кошелька

#### `open(name: str, wallet_dir: str = None) -> Wallet`

Открыть существующий кошелек.

**Параметры:**
- `name` (str): Имя кошелька
- `wallet_dir` (str, опционально): Директория для файлов кошелька

**Возвращает:**
- `Wallet`: Открытый экземпляр кошелька

**Вызывает:**
- `WalletError`: Если кошелек не найден или не может быть открыт

### Методы экземпляра

#### `get_address() -> WalletAddress`

Получить адрес кошелька для сети по умолчанию.

**Возвращает:**
- `WalletAddress`: Объект адреса кошелька

#### `get_address_for_network(network_name: str, key_index: int = 0) -> WalletAddress`

Получить адрес кошелька для конкретной сети.

**Параметры:**
- `network_name` (str): Имя сети (например, "mainnet", "testnet")
- `key_index` (int): Индекс ключа (по умолчанию: 0)

**Возвращает:**
- `WalletAddress`: Адрес кошелька для сети

**Вызывает:**
- `WalletError`: Если сеть не найдена или адрес не может быть получен

#### `get_balance(token_ticker: str = "CELL") -> Decimal`

Получить баланс кошелька для сети по умолчанию.

**Параметры:**
- `token_ticker` (str): Тикер токена (по умолчанию: "CELL")

**Возвращает:**
- `Decimal`: Баланс кошелька

#### `get_balance_by_network(network_name: str, token_ticker: str = "CELL") -> Decimal`

Получить баланс кошелька для конкретной сети и токена.

**Параметры:**
- `network_name` (str): Имя сети
- `token_ticker` (str): Тикер токена (по умолчанию: "CELL")

**Возвращает:**
- `Decimal`: Баланс кошелька

**Вызывает:**
- `WalletError`: Если сеть не найдена или баланс не может быть получен

#### `get_pkey_hash() -> str`

Получить хэш публичного ключа кошелька в виде шестнадцатеричной строки.

**Возвращает:**
- `str`: Хэш публичного ключа (hex формат с префиксом "0x")

**Вызывает:**
- `WalletError`: Если операция не удалась

#### `get_shared_tx_hashes(pkey_hash: str, network_name: str) -> Optional[Dict[str, Any]]`

Получить хэши транзакций shared кошелька по хэшу публичного ключа.

**Параметры:**
- `pkey_hash` (str): Строка хэша публичного ключа
- `network_name` (str): Имя сети

**Возвращает:**
- `Optional[Dict[str, Any]]`: Словарь с хэшами транзакций или None

**Вызывает:**
- `WalletError`: Если операция не удалась

#### `hold_shared_tx(tx_hash: str, network_name: str) -> bool`

Добавить hold транзакцию в shared кошелек.

**Параметры:**
- `tx_hash` (str): Хэш транзакции
- `network_name` (str): Имя сети

**Возвращает:**
- `bool`: True если успешно

**Вызывает:**
- `WalletError`: Если операция не удалась

#### `activate() -> None`

Активировать кошелек (сделать его доступным для операций).

**Вызывает:**
- `WalletError`: Если активация не удалась

#### `deactivate() -> None`

Деактивировать кошелек.

**Вызывает:**
- `WalletError`: Если деактивация не удалась

#### `save() -> None`

Сохранить кошелек в файл.

**Вызывает:**
- `WalletError`: Если сохранение не удалось

#### `close() -> None`

Закрыть кошелек и освободить ресурсы.

#### `is_closed() -> bool`

Проверить, закрыт ли кошелек.

**Возвращает:**
- `bool`: True если кошелек закрыт

### Свойства

- `name` (str): Имя кошелька (только чтение)
- `wallet_dir` (str): Директория кошелька (только чтение)
- `access_type` (WalletAccessType): Тип доступа (только чтение)
- `wallet_type` (WalletType): Тип подписи (только чтение)

## Класс: `WalletAddress`

Представляет адрес кошелька.

### Конструктор

```python
WalletAddress(address: str, net_id: int)
```

**Параметры:**
- `address` (str): Строка адреса
- `net_id` (int): ID сети

### Методы

#### `__str__() -> str`

Вернуть адрес как строку.

#### `__repr__() -> str`

Вернуть представление адреса.

### Свойства

- `address` (str): Строка адреса
- `net_id` (int): ID сети

## Класс: `WalletManager`

Управляет несколькими кошельками.

### Методы

#### `get_wallet(name: str) -> Optional[Wallet]`

Получить кошелек по имени.

**Параметры:**
- `name` (str): Имя кошелька

**Возвращает:**
- `Optional[Wallet]`: Экземпляр кошелька или None

#### `list_wallets() -> List[str]`

Список всех доступных имен кошельков.

**Возвращает:**
- `List[str]`: Список имен кошельков

## Перечисления

### `WalletType`

Типы алгоритмов подписи:
- `SIG_DILITHIUM`: Dilithium (квантово-безопасный)
- `SIG_FALCON`: Falcon (квантово-безопасный)
- `SIG_ED25519`: Ed25519 (классический)

### `WalletAccessType`

Типы доступа к кошельку:
- `READ_ONLY`: Только чтение
- `READ_WRITE`: Чтение и запись

## Исключения

### `WalletError`

Базовое исключение для операций с кошельком.

### `InsufficientFundsError`

Вызывается когда у кошелька недостаточно средств.

### `InvalidAddressError`

Вызывается когда адрес недействителен.

## Примеры

### Базовые операции с кошельком

```python
from CellFrame.chain import Wallet, WalletType
from decimal import Decimal

# Создать кошелек
wallet = Wallet.create("my_wallet", wallet_type=WalletType.SIG_DILITHIUM)

# Получить адрес
address = wallet.get_address()
print(f"Адрес: {address}")

# Проверить баланс
balance = wallet.get_balance("CELL")
print(f"Баланс: {balance} CELL")

# Сохранить и закрыть
wallet.save()
wallet.close()
```

### Мультиподписной кошелек

```python
from CellFrame.chain import Wallet

# Создать мультиподписной кошелек
seed = "ваша seed фраза здесь"
wallet = Wallet.create_multi(
    "multisig_wallet",
    seed=seed,
    required_signatures=3
)
```

### Операции для конкретной сети

```python
# Получить адрес для конкретной сети
mainnet_address = wallet.get_address_for_network("mainnet")
testnet_address = wallet.get_address_for_network("testnet")

# Получить баланс для конкретной сети
mainnet_balance = wallet.get_balance_by_network("mainnet", "CELL")
```

### Операции с shared кошельком

```python
# Получить хэш публичного ключа
pkey_hash = wallet.get_pkey_hash()

# Получить хэши транзакций shared кошелька
tx_hashes = wallet.get_shared_tx_hashes(pkey_hash, "mainnet")

# Добавить hold транзакцию
if tx_hashes:
    tx_hash = list(tx_hashes.keys())[0]
    wallet.hold_shared_tx(tx_hash, "mainnet")
```

