# Отладка

Как отлаживать Python приложения с Python-Cellframe.

## Логирование

Используйте стандартный модуль `logging`. В SDK настроен логгер `CellFrame`.

```python
import logging

logger = logging.getLogger("CellFrame")
logger.info("Информационное сообщение")
logger.error("Сообщение об ошибке")
```

Если вы используете legacy API, доступен `logIt`:

```python
from CellFrame.legacy.DAP.Core import logIt

logIt.info("Информационное сообщение")
logIt.error("Сообщение об ошибке")
```

## Интерактивная отладка (pdb)

1. Вставьте `import pdb; pdb.set_trace()` в код.
2. Запустите скрипт: `python3 my_script.py`.
3. Выполнение остановится на точке останова.

## Segmentation Faults (Сбои сегментации)

Если приложение падает (segfault):
1. Включите дампы памяти: `ulimit -c unlimited`.
2. Запустите приложение до падения.
3. Проанализируйте дамп с помощью `gdb`:
   ```bash
   gdb python3 core
   bt
   ```
4. Ищите кадры Python в трассировке.
