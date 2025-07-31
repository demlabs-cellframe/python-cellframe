# 🔧 Automated Error Correction Scripts

Система автоматизированного обнаружения и исправления ошибок для Python Cellframe SDK.

## 📁 Структура скриптов

```
scripts/
├── static_analysis.py      # 🔍 Статический анализ кода
├── error_classifier.py     # 🏷️ Классификация ошибок
├── auto_fixer.py          # 🔧 Автоматическое исправление
├── validation_runner.py   # ✅ Комплексная валидация
├── iterative_fixer.py     # 🔄 Итеративное исправление
└── README.md              # 📚 Эта документация
```

## 🚀 Быстрый старт

### 1. Комплексная валидация проекта
```bash
python3 scripts/validation_runner.py --paths CellFrame/ tests/
```

### 2. Автоматическое исправление проблем
```bash
python3 scripts/auto_fixer.py --paths CellFrame/ tests/
```

### 3. Итеративное исправление до полного устранения ошибок
```bash
python3 scripts/iterative_fixer.py --paths CellFrame/ tests/
```

## 📋 Подробное описание скриптов

### 🔍 static_analysis.py
**Назначение**: Статический анализ кода с использованием множественных инструментов.

**Возможности**:
- Pylint для качества кода
- Flake8 для стиля
- MyPy для типизации
- Bandit для безопасности
- Vulture для мертвого кода
- Radon для сложности

**Использование**:
```bash
# Базовый анализ
python3 scripts/static_analysis.py --paths CellFrame/ tests/

# Только конкретные инструменты
python3 scripts/static_analysis.py --paths CellFrame/ --tools pylint flake8

# Сохранить отчет
python3 scripts/static_analysis.py --paths CellFrame/ --output analysis_report.json
```

### 🏷️ error_classifier.py
**Назначение**: Интеллектуальная классификация и приоритизация ошибок.

**Классификация по**:
- **Приоритету**: Critical, High, Medium, Low
- **Типу**: Security, Reliability, Performance, Style
- **Компоненту**: Core, Chain, Services, Tests
- **Возможности автофикса**: Auto-fixable vs Manual

**Использование**:
```bash
# Классифицировать результаты статического анализа
python3 scripts/error_classifier.py analysis_report.json

# Сохранить классификацию
python3 scripts/error_classifier.py analysis_report.json --output classification.json
```

### 🔧 auto_fixer.py
**Назначение**: Автоматическое исправление типовых проблем.

**Поддерживаемые исправления**:
- ❌ **Неиспользуемые импорты** → Удаление
- 🧹 **Проблемы с пробелами** → Автоформатирование  
- 📚 **Порядок импортов** → Сортировка по PEP 8
- 📏 **Длинные строки** → Разбиение на несколько строк
- 🔤 **Неиспользуемые переменные** → Префикс `_`

**Использование**:
```bash
# Все доступные исправления
python3 scripts/auto_fixer.py --paths CellFrame/ tests/

# Только конкретные типы исправлений
python3 scripts/auto_fixer.py --paths CellFrame/ --fixes unused_imports whitespace

# Восстановить из бэкапа
python3 scripts/auto_fixer.py --restore .backup_20241201_143022
```

### ✅ validation_runner.py
**Назначение**: Комплексная валидация качества кода.

**Типы валидации**:
- **Syntax**: Синтаксические ошибки Python
- **Imports**: Корректность импортов
- **Tests**: Запуск тестового набора
- **Documentation**: Покрытие документацией
- **Security**: Базовые проверки безопасности

**Использование**:
```bash
# Полная валидация
python3 scripts/validation_runner.py --paths CellFrame/ tests/

# Только синтаксис и импорты
python3 scripts/validation_runner.py --paths CellFrame/ --validations syntax imports

# Тихий режим для CI/CD
python3 scripts/validation_runner.py --paths CellFrame/ --quiet
```

### 🔄 iterative_fixer.py
**Назначение**: Итеративный цикл исправления до полного устранения ошибок.

**Алгоритм**:
1. **Обнаружение** → Валидация + статический анализ
2. **Классификация** → Приоритизация найденных проблем
3. **Исправление** → Автоматические фиксы
4. **Валидация** → Проверка результата
5. **Итерация** → Повтор до достижения цели

**Использование**:
```bash
# Стандартный цикл (максимум 5 итераций)
python3 scripts/iterative_fixer.py --paths CellFrame/ tests/

# Увеличить лимит итераций
python3 scripts/iterative_fixer.py --paths CellFrame/ --max-iterations 10

# Сохранить детальный отчет
python3 scripts/iterative_fixer.py --paths CellFrame/ --output fixing_cycle.json
```

## 📊 Выходные форматы

### JSON отчеты
Все скрипты генерируют структурированные JSON отчеты:

```json
{
  "timestamp": "2024-12-01T14:30:22",
  "total_issues": 42,
  "by_severity": {
    "high": 3,
    "medium": 15,
    "low": 24
  },
  "by_category": {
    "security": 3,
    "style": 20,
    "maintainability": 19
  }
}
```

### Коды выхода
- **0**: Успех, критических проблем нет
- **1**: Предупреждения или некритические проблемы
- **2**: Критические ошибки требуют внимания

## 🔧 Интеграция с разработкой

### Pre-commit hook
```bash
#!/bin/bash
python3 scripts/validation_runner.py --paths CellFrame/ --validations syntax imports
exit_code=$?
if [ $exit_code -ne 0 ]; then
    echo "❌ Валидация не прошла. Запустите исправления:"
    echo "python3 scripts/auto_fixer.py --paths CellFrame/"
    exit 1
fi
```

### CI/CD интеграция
```yaml
- name: Code Quality Check
  run: |
    python3 scripts/validation_runner.py --paths CellFrame/ tests/ --quiet
    python3 scripts/iterative_fixer.py --paths CellFrame/ --quiet
```

### VS Code Tasks
```json
{
  "label": "Fix Code Issues",
  "type": "shell",
  "command": "python3",
  "args": ["scripts/iterative_fixer.py", "--paths", "CellFrame/"],
  "group": "build",
  "presentation": {
    "echo": true,
    "reveal": "always"
  }
}
```

## 🎯 Рекомендуемый workflow

### Ежедневная разработка
```bash
# 1. Быстрая проверка перед коммитом
python3 scripts/validation_runner.py --paths CellFrame/ --validations syntax imports

# 2. Автоисправление если нужно
python3 scripts/auto_fixer.py --paths CellFrame/

# 3. Повторная проверка
python3 scripts/validation_runner.py --paths CellFrame/ --validations syntax imports
```

### Еженедельное обслуживание
```bash
# Полный итеративный цикл исправлений
python3 scripts/iterative_fixer.py --paths CellFrame/ tests/
```

### Перед релизом
```bash
# Комплексная валидация всего проекта
python3 scripts/validation_runner.py --paths CellFrame/ tests/ src/
```

## 📈 Метрики качества

### Целевые показатели
- **Синтаксические ошибки**: 0
- **Критические проблемы безопасности**: 0  
- **Покрытие документацией**: > 80%
- **Неиспользуемый код**: < 5%
- **Сложность функций**: < 15 (цикломатическая)

### Отслеживание прогресса
```bash
# Генерация отчета о тенденциях
python3 scripts/validation_runner.py --paths CellFrame/ --output daily_metrics.json
```

## 🛠️ Расширение функциональности

### Добавление новых правил классификации
В `error_classifier.py`, секция `_build_classification_rules()`:

```python
'new_rule_pattern': {
    'priority': Priority.HIGH,
    'type': IssueType.RELIABILITY,
    'strategy': 'Описание исправления',
    'effort': 'medium',
    'auto_fixable': True,
    'breaking': False,
    'api_impact': True
}
```

### Добавление новых автофиксов
В `auto_fixer.py`, метод `apply_auto_fixes()`:

```python
if 'new_fix_type' in fix_types:
    if self.fix_new_issue_type(file_path):
        file_modified = True
        fixes_summary['fixes_applied'].append({
            'file': file_path,
            'fix_type': 'new_fix_type'
        })
```

## 🚨 Устранение неполадок

### Проблема: Скрипты не выполняются
```bash
# Проверить права
ls -la scripts/
# Должно быть: -rwxr-xr-x

# Сделать исполняемыми
chmod +x scripts/*.py
```

### Проблема: Ложные срабатывания
Настроить исключения в соответствующих скриптах или добавить файл `.ignore_patterns`.

### Проблема: Медленная работа
```bash
# Ограничить область анализа
python3 scripts/validation_runner.py --paths CellFrame/core/ --validations syntax imports
```

## 📚 Дополнительные ресурсы

- [Cellframe SDK Documentation](../../docs/)
- [Python Testing Guide](../../tests/README.md)
- [Development Workflow](../../CONTRIBUTING.md)
- [Code Style Guide](../../STYLE_GUIDE.md)