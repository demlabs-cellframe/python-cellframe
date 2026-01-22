# Использование Global Database (GDB)

GDB — это распределенное хранилище типа ключ-значение.

## Группы

Данные организованы в группы. Группа синхронизируется между узлами.

## Чтение и Запись

```python
import dap.global_db as gdb

group_name = "my.app.data"
key = "user_1"
value = b"some_binary_data"

# Запись данных
gdb.set(group_name, key, value)

# Чтение данных
data = gdb.get(group_name, key)
if data:
    print(f"Прочитано: {data}")

# Удаление данных
gdb.delete(group_name, key)
```

## Итерация

```python
# Получить все элементы группы
items = gdb.get_all(group_name)
for key, value in items.items():
    print(f"{key}: {value}")
```

