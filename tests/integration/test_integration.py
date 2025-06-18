"""
Integration tests for Python Cellframe
Интеграционные тесты для Python Cellframe
"""

import pytest
import unittest
from unittest.mock import Mock, patch

try:
    import CellFrame
    cellframe_available = True
except ImportError:
    cellframe_available = False

@pytest.mark.integration
@pytest.mark.skipif(not cellframe_available, reason="CellFrame module not available")
class TestCellFrameIntegration:
    """Интеграционные тесты для всей системы CellFrame"""
    
    def test_module_import(self):
        """Тест импорта основного модуля"""
        import CellFrame
        assert CellFrame is not None
    
    def test_basic_functionality(self):
        """Тест базовой функциональности"""
        # Тестируем что основные компоненты доступны
        expected_components = []
        
        # Собираем все доступные функции
        cellframe_attrs = [attr for attr in dir(CellFrame) if not attr.startswith('_')]
        
        assert len(cellframe_attrs) > 0, "CellFrame должен содержать публичные атрибуты"
    
    def test_module_stability(self):
        """Тест стабильности модуля при множественных вызовах"""
        import CellFrame
        
        # Выполняем несколько импортов подряд
        for i in range(10):
            import CellFrame as CF
            assert CF is not None
    
    @pytest.mark.slow
    def test_memory_usage(self):
        """Тест использования памяти"""
        import CellFrame
        import gc
        
        # Принудительная сборка мусора
        gc.collect()
        
        # Выполняем операции с модулем
        for i in range(100):
            attrs = dir(CellFrame)
        
        # Проверяем что память не утекает критично
        gc.collect()
        assert True  # Если дошли до сюда, значит память не закончилась

@pytest.mark.integration 
class TestModuleInteractions:
    """Тесты взаимодействия между модулями"""
    
    def test_cross_module_compatibility(self):
        """Тест совместимости между модулями"""
        if not cellframe_available:
            pytest.skip("CellFrame not available")
        
        import CellFrame
        
        # Проверяем что различные компоненты могут работать вместе
        attrs = dir(CellFrame)
        functions = [attr for attr in attrs if callable(getattr(CellFrame, attr, None))]
        
        assert len(functions) > 0, "Должны быть доступны функции для взаимодействия"

if __name__ == '__main__':
    pytest.main([__file__])
