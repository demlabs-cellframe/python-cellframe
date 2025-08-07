# 🧪 Python Cellframe SDK - Testing Guide

## 🚀 Новый Workflow для Тестирования

Начиная с версии после исправления проблем с CI/CD, тестирование использует двухэтапный подход:

### 📋 Быстрый запуск (рекомендуется)

```bash
# 1. Инициализация тестового окружения (один раз)
./tests/init_test_environment.sh

# 2. Запуск всех тестов
./run_tests.sh
```

### 📋 Ручной запуск

```bash
# 1. Сборка проекта
mkdir -p build && cd build
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)

# 2. Инициализация тестового окружения
cd ..
./tests/init_test_environment.sh

# 3. Запуск тестов
cd build
ctest --output-on-failure
```

## 🔧 Что делает инициализация

Скрипт `./tests/init_test_environment.sh` выполняет полную настройку:

### 🐍 Python окружение
- Создает виртуальное окружение `venv_test/`
- Устанавливает последние версии pip, setuptools, wheel
- Устанавливает все тестовые зависимости (pytest, coverage, etc.)
- Устанавливает инструменты качества кода (mypy, black, flake8)

### 📦 Пакеты проекта
- Собирает и устанавливает `python-dap` (зависимость)
- Собирает и устанавливает `python-cellframe`
- Копирует нативные библиотеки (.so файлы)
- Настраивает пути для доступа к библиотекам

### ⚙️ Конфигурация
- Создает `venv_test/bin/activate_test` с настроенными путями
- Устанавливает переменные окружения для тестов
- Создает удобный скрипт `run_tests.sh` для быстрого запуска

## 🎛️ Режимы работы

### 🔍 Verbose режим (для отладки)
```bash
VERBOSE=true ./tests/init_test_environment.sh
```
Показывает подробные логи pip и venv операций.

### 🤖 CI режим
```bash
CI=true ./tests/init_test_environment.sh
```
Автоматически включает verbose режим и оптимизирован для CI/CD.

### 🔄 Принудительная пересборка
```bash
FORCE_REBUILD=true ./tests/init_test_environment.sh
```
Принудительно пересоздает окружение и пересобирает все пакеты.

## 📊 Типы тестов

После инициализации доступны следующие тесты:

### Unit тесты
```bash
# Запуск конкретного unit теста
cd build && ctest -R test_unit

# Все unit тесты
cd build && ctest -R "test_unit.*"
```

### Функциональные тесты
```bash
# Быстрые функциональные тесты
cd build && ctest -R test_quick_functionality

# Полные функциональные тесты  
cd build && ctest -R test_all_python
```

### Legacy тесты
```bash
# Тесты совместимости
cd build && ctest -R test_unit_legacy
```

### Composer тесты
```bash
# Тесты composer компонентов
cd build && ctest -R test_unit_composer
```

## 🐛 Отладка проблем

### Проблемы с виртуальным окружением
```bash
# Пересоздать окружение
FORCE_REBUILD=true ./tests/init_test_environment.sh

# Проверить активацию
source venv_test/bin/activate_test
python -c "import CellFrame; print('OK')"
```

### Проблемы с нативными библиотеками
```bash
# Проверить наличие библиотек
ls -la build/libpython_cellframe.so*
ls -la build/dap-sdk-build/libdap_sdk.so*

# Проверить пути
source venv_test/bin/activate_test
echo $LD_LIBRARY_PATH
```

### Verbose диагностика
```bash
# Запуск с максимальной информацией
VERBOSE=true ./tests/init_test_environment.sh

# Проверка установленных пакетов
source venv_test/bin/activate_test
pip list | grep -E "(cellframe|dap)"
```

## ❌ Что НЕ работает (старый подход)

### ❌ НЕ используйте
```bash
# Старый подход через CTest fixture (удален)
ctest  # без инициализации - будет падать

# Прямой запуск без инициализации
cd build && ctest  # падает с ошибками импорта
```

Старый `cellframe_environment_setup` тест был удален как ненадежный.

## ✅ Преимущества нового подхода

1. **🚀 Надежность** - инициализация происходит вне CTest
2. **📊 Видимость** - подробные логи показывают что происходит  
3. **🔧 Гибкость** - можно запускать инициализацию отдельно
4. **⚡ Скорость** - инициализация один раз, тесты много раз
5. **🐛 Отладка** - легко диагностировать проблемы с VERBOSE=true

## 🔄 CI/CD интеграция

Для CI/CD пайплайнов:

```yaml
# Пример для GitLab CI
test_python:
  script:
    - mkdir -p build && cd build
    - cmake .. -DBUILD_TESTS=ON
    - make -j$(nproc)
    - cd ..
    - CI=true ./tests/init_test_environment.sh
    - ./run_tests.sh
```

```bash
# Пример для GitHub Actions  
- name: Setup test environment
  run: CI=true ./tests/init_test_environment.sh
  
- name: Run tests
  run: ./run_tests.sh
```

## 📞 Поддержка

При проблемах с тестами:

1. Убедитесь что CMake сборка прошла успешно
2. Запустите инициализацию с `VERBOSE=true`
3. Проверьте логи в секции "Verifying test environment"
4. Убедитесь что все нативные библиотеки собраны

---

**Важно:** Инициализация должна запускаться ПОСЛЕ сборки проекта, но ДО запуска тестов.