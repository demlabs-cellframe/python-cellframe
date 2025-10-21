"""
CellFrame Common Module
======================

Общие утилиты и компоненты для CellFrame Python биндингов.

Exports:
    - logger: Система логирования
    - types: Общие типы и константы
"""

import logging
import sys
from typing import Any

# Создаем logger для CellFrame
logger = logging.getLogger('cellframe')

# Настраиваем базовую конфигурацию если еще не настроена
if not logger.handlers:
    handler = logging.StreamHandler(sys.stdout)
    formatter = logging.Formatter(
        '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    logger.setLevel(logging.INFO)

# Импортируем типы
from .types import *

# Экспортируем все необходимое
__all__ = [
    'logger',
    # Types will be added by import *
]


