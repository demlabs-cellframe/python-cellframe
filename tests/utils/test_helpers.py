"""
Test utilities for Python Cellframe tests
Тестовые утилиты для тестов Python Cellframe
"""

import time
import functools
from typing import Any, Callable

def retry_on_failure(max_retries: int = 3, delay: float = 1.0):
    """Декоратор для повторения тестов при неудаче"""
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            last_exception = None
            
            for attempt in range(max_retries):
                try:
                    return func(*args, **kwargs)
                except Exception as e:
                    last_exception = e
                    if attempt < max_retries - 1:
                        time.sleep(delay)
                    
            raise last_exception
        return wrapper
    return decorator

def skip_if_module_missing(module_name: str):
    """Декоратор для пропуска тестов если модуль недоступен"""
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            try:
                __import__(module_name)
                return func(*args, **kwargs)
            except ImportError:
                import pytest
                pytest.skip(f"Module {module_name} not available")
        return wrapper
    return decorator

class TestHelper:
    """Вспомогательный класс для тестов"""
    
    @staticmethod
    def assert_function_exists(module, function_name: str):
        """Проверяет существование функции в модуле"""
        assert hasattr(module, function_name), f"Function {function_name} not found in module"
        assert callable(getattr(module, function_name)), f"Attribute {function_name} is not callable"
    
    @staticmethod
    def assert_type_or_none(value: Any, expected_type: type):
        """Проверяет что значение имеет ожидаемый тип или None"""
        assert value is None or isinstance(value, expected_type), f"Expected {expected_type} or None, got {type(value)}"
    
    @staticmethod
    def safe_call(func: Callable, *args, **kwargs):
        """Безопасный вызов функции с обработкой исключений"""
        try:
            return func(*args, **kwargs)
        except Exception as e:
            return {"error": str(e), "type": type(e).__name__}

def measure_execution_time(func: Callable) -> Callable:
    """Декоратор для измерения времени выполнения"""
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        execution_time = end_time - start_time
        print(f"Function {func.__name__} executed in {execution_time:.4f} seconds")
        return result
    return wrapper
