# Привет, CellFrame

Этот урок поможет вам создать первый скрипт на Python с использованием CellFrame SDK.

## 1. Базовый скрипт

Создайте файл `hello_cellframe.py`:

```python
import CellFrame
import dap.core

def main():
    # Инициализация SDK
    # Настраивает управление памятью, логирование и подсистемы ядра
    if not CellFrame.init():
        print("Ошибка инициализации CellFrame")
        return

    print("Привет, CellFrame!")
    
    # Получение версий
    print(f"Версия CellFrame: {CellFrame.get_version()}")
    print(f"Версия DAP SDK: {dap.core.get_version()}")

    # Очистка ресурсов
    CellFrame.deinit()

if __name__ == "__main__":
    main()
```

## 2. Запуск

Запустите скрипт через python3:

```bash
python3 hello_cellframe.py
```

## 3. Пояснение

*   `import CellFrame`: Импортирует основной модуль CellFrame SDK.
*   `CellFrame.init()`: Обязательный шаг. Инициализирует C-библиотеки (пулы памяти, логгеры). Должен быть вызван до использования других функций.
*   `CellFrame.deinit()`: Освобождает ресурсы. Хорошая практика вызывать при завершении.

