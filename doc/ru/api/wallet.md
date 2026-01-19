# Справочник API: Wallet

Описание класса `Wallet` и связанных операций.

## Класс: `Wallet`

Высокоуровневое управление кошельками на базе нативных биндингов
`python_cellframe`.

### Конструктор

```python
Wallet(name: str, wallet_handle: Any = None, access_type: WalletAccessType = WalletAccessType.LOCAL)
```

Обычно используется `Wallet.create()` или `Wallet.open()`.

### Методы класса

#### `create(name: str, wallet_path: str, password: Optional[str] = None, seed: Optional[bytes] = None, signature_type: int = 0x0102) -> Wallet`

Создать новый кошелек.

#### `open(name: str, wallet_path: str, password: Optional[str] = None) -> Wallet`

Открыть существующий кошелек.

#### `create_with_network(name: str, network_name: str, wallet_path: Optional[str] = None, signature_type: int = 0x0102, seed: Optional[bytes] = None) -> Wallet`

Удобный метод с автоопределением пути через конфигурацию DAP.

### Методы экземпляра

#### `get_address(net_id: int) -> WalletAddress`

Получить адрес по идентификатору сети.

#### `get_address_for_network(network_name: str, key_index: int = 0) -> WalletAddress`

Получить адрес по имени сети.

#### `get_balance(net_id: int, token_ticker: str) -> Decimal`

Получить баланс по идентификатору сети и тикеру.

#### `get_balance_by_network(network_name: str, token_ticker: str = "CELL") -> Decimal`

Получить баланс по имени сети.

#### `get_key(key_type: int = 0x0102)` / `get_pkey(key_type: int = 0x0102)`

Вернуть ключ/публичный ключ из нативного SDK.

#### `get_pkey_hash() -> str`

Вернуть хеш публичного ключа.

#### `get_shared_tx_hashes(pkey_hash: str, network_name: str) -> Optional[Dict[str, Any]]`

Получить хеши shared-транзакций.

#### `hold_shared_tx(tx_handle: Any, network_name: str) -> bool`

Добавить транзакцию в hold (ожидается нативный tx handle).

#### `activate() -> bool` / `deactivate() -> bool`

Активировать или деактивировать кошелек.

#### `save() -> bool`

Сохранить кошелек через нативные биндинги.

#### `close() -> None` / `is_closed() -> bool`

Закрыть кошелек и освободить ресурсы.

### Перечисления

#### `WalletType`

- `SIMPLE`
- `MULTISIG`
- `SHARED`
- `HARDWARE`

#### `WalletAccessType`

- `LOCAL`
- `REMOTE`

### Исключения

- `WalletError`
- `InsufficientFundsError`
- `InvalidAddressError`

## WalletManager и удобные функции

`WalletManager` предоставляет `create`, `open`, `get`, `close`, `list`, `close_all`.
Модульные функции (`create`, `open`, `get`, `close`, `list`, `close_all`) проксируют
в глобальный менеджер.

## Пример

```python
from CellFrame.chain import Wallet

wallet = Wallet.open("my_wallet", "/path/to/wallets", password="secret")
address = wallet.get_address_for_network("mainnet")
balance = wallet.get_balance_by_network("mainnet", "CELL")

print(address)
print(balance)

wallet.close()
```
