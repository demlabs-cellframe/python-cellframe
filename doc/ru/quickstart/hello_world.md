# Hello World

Минимальный скрипт для проверки установки Python-Cellframe.

## 1. Скрипт

Создайте файл `hello_cellframe.py`:

```python
import CellFrame

def main():
    # Инициализация глобального контекста
    CellFrame.initialize_context()

    print("Hello, CellFrame!")
    print(f"Версия CellFrame: {CellFrame.get_version()}")

    # Освобождение ресурсов
    CellFrame.shutdown_context()

if __name__ == "__main__":
    main()
```

## 2. Запуск

```bash
python3 hello_cellframe.py
```
