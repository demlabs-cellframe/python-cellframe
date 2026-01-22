# Global Database (GDB) Usage

GDB is a distributed key-value storage.

## Groups

Data is organized into groups. A group is synchronized across nodes.

## Reading and Writing

```python
import dap.global_db as gdb

group_name = "my.app.data"
key = "user_1"
value = b"some_binary_data"

# Write data
gdb.set(group_name, key, value)

# Read data
data = gdb.get(group_name, key)
if data:
    print(f"Read: {data}")

# Delete data
gdb.delete(group_name, key)
```

## Iterating

```python
# Get all items in a group
items = gdb.get_all(group_name)
for key, value in items.items():
    print(f"{key}: {value}")
```

