# 📦 Installation Guide - Установка СЛК

> **Целевая аудитория:** Системные администраторы, DevOps, Team Leads

## 🎯 Обзор

Это полное руководство по установке **Smart Layered Context (СЛК)** системы в различных окружениях. СЛК спроектирована для легкой установки и быстрого развертывания как для индивидуальных разработчиков, так и для больших команд.

### 📋 Системные требования

#### **Минимальные требования:**
- **ОС:** Windows 10+, macOS 10.15+, Linux (Ubuntu 18.04+, CentOS 7+)
- **Python:** 3.8+ (рекомендуется 3.11+)
- **Память:** 2GB RAM (рекомендуется 4GB+)
- **Диск:** 500MB свободного места (рекомендуется 2GB+)
- **Git:** Версия 2.20+

#### **Рекомендуемые требования:**
- **ОС:** Latest stable versions
- **Python:** 3.11+ с virtual environment
- **Память:** 8GB+ RAM
- **Диск:** SSD 10GB+ свободного места
- **CPU:** Multi-core processor для больших проектов

---

## 🚀 Быстрая установка (5 минут)

### Метод 1: Git Clone (рекомендуемый)

```bash
# 1. Клонирование репозитория
git clone https://github.com/your-org/smart-layered-context.git
cd smart-layered-context

# 2. Проверка установки
./slc помощь

# 3. Инициализация
./slc старт
```

### Метод 2: Download Archive

```bash
# 1. Скачивание архива
wget https://github.com/your-org/smart-layered-context/archive/main.zip
unzip main.zip
cd smart-layered-context-main

# 2. Установка исполняемых прав
chmod +x slc
chmod +x .context/tools/scripts/slc_cli.py

# 3. Проверка
./slc помощь
```

---

## 🛠️ Детальная установка

### Шаг 1: Подготовка среды

#### **Python Virtual Environment (рекомендуется):**
```bash
# Создание виртуального окружения
python3 -m venv slc-env
source slc-env/bin/activate  # Linux/macOS
# slc-env\Scripts\activate   # Windows

# Обновление pip
pip install --upgrade pip
```

#### **Установка зависимостей:**
```bash
# Если есть requirements.txt
pip install -r requirements.txt

# Базовые зависимости (если requirements.txt отсутствует)
pip install click>=8.0.0 jsonschema>=4.0.0 pathlib2>=2.3.0
```

### Шаг 2: Конфигурация системы

#### **Создание символических ссылок (Linux/macOS):**
```bash
# Для глобального доступа к slc команде
sudo ln -s $(pwd)/slc /usr/local/bin/slc

# Или добавить в PATH
echo 'export PATH="$PATH:$(pwd)"' >> ~/.bashrc
source ~/.bashrc
```

#### **Создание bat файла (Windows):**
```batch
@echo off
cd /d "C:\path\to\smart-layered-context"
python .context\tools\scripts\slc_cli.py %*
```

### Шаг 3: Первичная валидация

```bash
# Проверка установки
./slc валидация

# Проверка всех компонентов
./slc статус

# Тест основных функций
./slc шаблоны
./slc список
```

---

## 🏢 Корпоративная установка

### Развертывание для команды

#### **1. Централизованная установка на общем сервере:**

```bash
# Установка в /opt/slc
sudo mkdir -p /opt/slc
sudo chown $USER:$USER /opt/slc
cd /opt/slc

git clone https://github.com/your-org/smart-layered-context.git .
chmod +x slc

# Создание global wrapper
sudo tee /usr/local/bin/slc << 'EOF'
#!/bin/bash
cd /opt/slc && ./slc "$@"
EOF

sudo chmod +x /usr/local/bin/slc
```

#### **2. Настройка общих шаблонов:**

```bash
# Создание shared templates directory
sudo mkdir -p /opt/slc/shared-templates

# Настройка прав доступа
sudo chgrp -R developers /opt/slc/shared-templates
sudo chmod -R 775 /opt/slc/shared-templates

# Конфигурация
./slc config set shared_templates_path "/opt/slc/shared-templates"
```

#### **3. Интеграция с CI/CD:**

**GitHub Actions Example:**
```yaml
name: SLC Integration
on: [push, pull_request]

jobs:
  setup-slc:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup SLC
        run: |
          git clone https://github.com/your-org/smart-layered-context.git slc
          cd slc && ./slc валидация
          
      - name: Organize Project
        run: |
          cd slc && ./slc организуй --dry-run
```

**Jenkins Pipeline:**
```groovy
pipeline {
    agent any
    stages {
        stage('SLC Setup') {
            steps {
                sh '''
                    git clone https://github.com/your-org/smart-layered-context.git slc
                    cd slc && ./slc валидация
                '''
            }
        }
        stage('Project Organization') {
            steps {
                sh 'cd slc && ./slc организуй'
            }
        }
    }
}
```

---

## 🐳 Docker установка

### Dockerfile

```dockerfile
FROM python:3.11-slim

# Установка зависимостей
RUN apt-get update && apt-get install -y \
    git \
    && rm -rf /var/lib/apt/lists/*

# Создание рабочей директории
WORKDIR /app

# Клонирование SLC
RUN git clone https://github.com/your-org/smart-layered-context.git .

# Установка permissions
RUN chmod +x slc && \
    chmod +x .context/tools/scripts/slc_cli.py

# Валидация установки
RUN ./slc валидация

# Entry point
ENTRYPOINT ["./slc"]
CMD ["помощь"]
```

### Docker Compose для команды

```yaml
version: '3.8'

services:
  slc:
    build: .
    volumes:
      - ./projects:/app/projects
      - ./shared-templates:/app/shared-templates
    environment:
      - SLC_SHARED_TEMPLATES=/app/shared-templates
    command: ["статус"]

  slc-web:
    build: .
    ports:
      - "8080:8080"
    command: ["web-server", "--port", "8080"]
    depends_on:
      - slc
```

---

## ⚙️ Конфигурация

### Основной конфигурационный файл

**Создание config.json:**
```json
{
  "system": {
    "version": "4.3.0",
    "auto_update": true,
    "telemetry": false
  },
  "paths": {
    "shared_templates": "/opt/slc/shared-templates",
    "backup_directory": "/opt/slc/backups",
    "logs_directory": "/var/log/slc"
  },
  "performance": {
    "cache_enabled": true,
    "cache_size_mb": 100,
    "parallel_processing": true,
    "max_workers": 4
  },
  "security": {
    "validate_templates": true,
    "sandbox_execution": true,
    "allowed_extensions": [".json", ".md", ".py", ".sh"]
  }
}
```

### Переменные окружения

```bash
# Основные настройки
export SLC_HOME="/opt/slc"
export SLC_CONFIG_PATH="/opt/slc/config.json"
export SLC_SHARED_TEMPLATES="/opt/slc/shared-templates"

# Performance настройки
export SLC_CACHE_ENABLED="true"
export SLC_PARALLEL_PROCESSING="true"
export SLC_MAX_WORKERS="4"

# Logging
export SLC_LOG_LEVEL="INFO"
export SLC_LOG_PATH="/var/log/slc/slc.log"

# Security
export SLC_SANDBOX_MODE="true"
export SLC_VALIDATE_TEMPLATES="true"
```

---

## 🔍 Диагностика и устранение неисправностей

### Общие проблемы и решения

#### **Проблема: `./slc: command not found`**
```bash
# Решение 1: Проверка прав доступа
chmod +x slc
chmod +x .context/tools/scripts/slc_cli.py

# Решение 2: Использование полного пути
python3 .context/tools/scripts/slc_cli.py помощь

# Решение 3: Проверка PATH
echo $PATH
export PATH="$PATH:$(pwd)"
```

#### **Проблема: Python зависимости отсутствуют**
```bash
# Решение: Установка зависимостей
pip install click jsonschema pathlib2

# Или создание requirements.txt
cat > requirements.txt << 'EOF'
click>=8.0.0
jsonschema>=4.0.0
pathlib2>=2.3.0
EOF

pip install -r requirements.txt
```

#### **Проблема: Медленная работа**
```bash
# Диагностика производительности
./slc статус --verbose

# Включение кэширования
export SLC_CACHE_ENABLED="true"

# Увеличение workers
export SLC_MAX_WORKERS="8"
```

### Логирование и мониторинг

#### **Настройка логирования:**
```bash
# Создание лог директории
sudo mkdir -p /var/log/slc
sudo chown $USER:$USER /var/log/slc

# Конфигурация logrotate
sudo tee /etc/logrotate.d/slc << 'EOF'
/var/log/slc/*.log {
    daily
    rotate 30
    compress
    missingok
    notifempty
    create 644 slc slc
}
EOF
```

#### **Health Check Script:**
```bash
#!/bin/bash
# slc-health-check.sh

echo "=== SLC Health Check ==="
echo "Timestamp: $(date)"

# Basic functionality test
if ./slc валидация > /dev/null 2>&1; then
    echo "✅ Basic validation: PASS"
else
    echo "❌ Basic validation: FAIL"
    exit 1
fi

# Performance test
start_time=$(date +%s)
./slc шаблоны > /dev/null 2>&1
end_time=$(date +%s)
duration=$((end_time - start_time))

if [ $duration -lt 5 ]; then
    echo "✅ Performance test: PASS (${duration}s)"
else
    echo "⚠️ Performance test: SLOW (${duration}s)"
fi

# Disk space check
available=$(df /opt/slc | awk 'NR==2{print $4}')
if [ $available -gt 1048576 ]; then  # 1GB in KB
    echo "✅ Disk space: SUFFICIENT"
else
    echo "⚠️ Disk space: LOW"
fi

echo "=== Health Check Complete ==="
```

---

## 🔄 Обновление и обслуживание

### Автоматическое обновление

```bash
#!/bin/bash
# update-slc.sh

echo "Updating SLC system..."

# Backup current installation
cp -r /opt/slc /opt/slc.backup.$(date +%Y%m%d)

# Pull latest changes
cd /opt/slc
git pull origin main

# Validate installation
if ./slc валидация; then
    echo "✅ Update successful"
    rm -rf /opt/slc.backup.$(date +%Y%m%d)
else
    echo "❌ Update failed, rolling back"
    rm -rf /opt/slc
    mv /opt/slc.backup.$(date +%Y%m%d) /opt/slc
    exit 1
fi
```

### Backup стратегия

```bash
#!/bin/bash
# backup-slc.sh

BACKUP_DIR="/opt/slc-backups"
DATE=$(date +%Y%m%d_%H%M%S)
BACKUP_NAME="slc_backup_${DATE}"

mkdir -p "$BACKUP_DIR"

# Create backup archive
tar -czf "$BACKUP_DIR/${BACKUP_NAME}.tar.gz" \
    --exclude='.git' \
    --exclude='__pycache__' \
    --exclude='*.pyc' \
    /opt/slc

echo "Backup created: $BACKUP_DIR/${BACKUP_NAME}.tar.gz"

# Clean old backups (keep last 10)
cd "$BACKUP_DIR"
ls -t *.tar.gz | tail -n +11 | xargs rm -f
```

---

## 🎯 Проверка установки

### Финальная валидация

```bash
#!/bin/bash
# Comprehensive installation validation

echo "🔍 SLC Installation Validation"
echo "=============================="

# 1. Basic command test
echo -n "1. Basic command execution: "
if ./slc помощь > /dev/null 2>&1; then
    echo "✅ PASS"
else
    echo "❌ FAIL"
    exit 1
fi

# 2. Core functionality test
echo -n "2. Core functionality: "
if ./slc статус > /dev/null 2>&1; then
    echo "✅ PASS"
else
    echo "❌ FAIL"
    exit 1
fi

# 3. Template system test
echo -n "3. Template system: "
if ./slc шаблоны | grep -q "шаблонов"; then
    echo "✅ PASS"
else
    echo "❌ FAIL"
    exit 1
fi

# 4. AI recommendations test
echo -n "4. AI recommendations: "
if ./slc подумай тест > /dev/null 2>&1; then
    echo "✅ PASS"
else
    echo "⚠️ LIMITED (AI features may require additional setup)"
fi

# 5. File organization test
echo -n "5. File organization: "
if ./slc организуй --dry-run > /dev/null 2>&1; then
    echo "✅ PASS"
else
    echo "❌ FAIL"
    exit 1
fi

echo ""
echo "🎉 Installation validation complete!"
echo "➡️  Next steps:"
echo "   - Run './slc старт' to initialize"
echo "   - Read documentation in .context/docs/"
echo "   - Try './slc подумай [your task]' for AI recommendations"
```

---

## 📚 Дополнительные ресурсы

### Документация
- **[Quick Start Guide](../user/quick-start.md)** - быстрое начало работы
- **[System Overview](../user/system-overview.md)** - архитектура системы
- **[Configuration Reference](configuration-reference.md)** - детальная конфигурация
- **[Performance Guide](performance-guide.md)** - оптимизация производительности

### Поддержка
- **GitHub Issues** - для сообщения о проблемах
- **Documentation** - полная документация в .context/docs/
- **Community** - форум сообщества
- **Professional Support** - корпоративная поддержка

---

**🎯 Готово!** СЛК система успешно установлена и готова к использованию.

**➡️ Следующие шаги:**
1. Прочитайте [Quick Start Guide](../user/quick-start.md)
2. Изучите [Configuration Reference](configuration-reference.md)
3. Настройте [Performance Optimization](performance-guide.md)

---

*📅 Обновлено: 2025-01-16 | 📖 Версия: v4.3.0 | 🔗 [Назад к документации](../README.md)* 