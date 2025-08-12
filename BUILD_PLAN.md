# 🏗️ ПЛАН ИСПРАВЛЕНИЯ СБОРКИ PYTHON CELLFRAME SDK

## 🔍 **Анализ текущих проблем:**

### Проблема 1: Конфликт подмодулей dap-sdk
- `cellframe-sdk/dap-sdk/` - полный подмодуль dap-sdk ✅
- `python-dap/dap-sdk/` - пустая директория ❌
- `dap-sdk/` - основной подмодуль на верхнем уровне ✅

### Проблема 2: Циклические зависимости и дублирование
Текущая архитектура пытается собрать DAP SDK 3 раза:
1. В основном `dap-sdk/` 
2. Внутри `cellframe-sdk/dap-sdk/`
3. Внутри `python-dap/dap-sdk/` (пустой)

### Проблема 3: Неправильная последовательность сборки
CMake пытается собрать все сразу вместо поэтапной сборки.

---

## 🎯 **РЕШЕНИЕ: Поэтапная сборка (Phase 2 Architecture)**

### Этап 1: Подготовка подмодулей
```bash
# Исправляем python-dap/dap-sdk -> должен ссылаться на основной dap-sdk
rm -rf python-dap/dap-sdk
ln -sf ../dap-sdk python-dap/dap-sdk

# Проверяем что cellframe-sdk/dap-sdk существует
ls -la cellframe-sdk/dap-sdk/CMakeLists.txt  # должен существовать
```

### Этап 2: Сборка DAP SDK (shared library)
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=OFF
make dap_sdk -j$(nproc)
```

### Этап 3: Сборка Cellframe SDK (static, using shared DAP SDK)
```bash
# Cellframe SDK собирается автоматически с правильными зависимостями
make cellframe_sdk -j$(nproc) 
```

### Этап 4: Сборка python-dap (shared, using shared DAP SDK)
```bash
make python_dap -j$(nproc)
```

### Этап 5: Сборка python-cellframe (using all previous components)
```bash
make python_cellframe -j$(nproc)
```

### Этап 6: Создание пакетов и установка в тестовое окружение
```bash
make python_package  # создает wheel пакеты
make install_venv    # устанавливает в тестовое окружение
```

---

## 📋 **ДЕТАЛЬНЫЙ ПЛАН РЕАЛИЗАЦИИ:**

### 1. Исправление файловой структуры
- ✅ Создать `CTestCustom.cmake.in`
- 🔄 Исправить symlink для `python-dap/dap-sdk`
- 🔄 Проверить целостность подмодулей

### 2. Модификация CMakeLists.txt
**НЕ ТРЕБУЕТСЯ** - архитектура уже правильная в коде:
- DAP SDK собирается как shared library первым
- Cellframe SDK использует этот shared DAP SDK  
- python-dap настроен на использование внешнего shared DAP SDK
- python-cellframe корректно зависит от обоих

### 3. Последовательность сборки (строго по порядку!)
```bash
# Шаг 1: Конфигурация
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON

# Шаг 2: DAP SDK (foundation)
make dap_sdk -j$(nproc)

# Шаг 3: Cellframe SDK (depends on dap_sdk)
make cellframe_sdk -j$(nproc)

# Шаг 4: python-dap (depends on dap_sdk) 
make python_dap -j$(nproc)

# Шаг 5: python-cellframe (depends on all above)
make python_cellframe -j$(nproc)

# Шаг 6: Создание пакетов
make python_package

# Шаг 7: Тестирование
ctest --output-on-failure
```

---

## 🚀 **ПРЕИМУЩЕСТВА ЭТОГО ПОДХОДА:**

1. **Единственный экземпляр DAP SDK** - нет дублирования в памяти
2. **Четкие зависимости** - каждый компонент знает что использовать
3. **Масштабируемость** - легко добавлять новые Python модули
4. **Отладка** - все native библиотеки собираются в Debug режиме
5. **Тестирование** - все компоненты доступны для CTest

---

## ⚠️ **КРИТИЧЕСКИ ВАЖНО:**

1. **НЕ запускать `git submodule update --init --recursive`** - это может сломать настроенную структуру
2. **Соблюдать порядок сборки** - нельзя собирать компоненты параллельно
3. **Использовать одну сборочную директорию** - все в `build/`
4. **Debug режим** - обязательно для отладки нативных библиотек

---

## 🎉 **ОЖИДАЕМЫЙ РЕЗУЛЬТАТ:**

После выполнения всех этапов:
- ✅ Все нативные библиотеки собраны и доступны
- ✅ Python пакеты установлены в venv_test
- ✅ Все тесты проходят (включая те, что требуют нативных модулей)
- ✅ `import CellFrame` работает без ошибок
- ✅ `ctest` выполняется успешно