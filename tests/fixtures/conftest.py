"""
Test fixtures for Python Cellframe tests
Тестовые fixtures для тестов Python Cellframe
"""

import pytest
from unittest.mock import Mock, MagicMock

@pytest.fixture
def mock_cellframe():
    """Mock объект для CellFrame модуля"""
    mock = MagicMock()
    return mock

@pytest.fixture
def sample_data():
    """Образцы данных для тестов"""
    return {
        "test_string": "test_value",
        "test_int": 42,
        "test_bool": True,
        "test_list": [1, 2, 3],
        "test_dict": {"key": "value"}
    }

@pytest.fixture
def temp_file(tmp_path):
    """Временный файл для тестов"""
    file_path = tmp_path / "test_file.txt"
    file_path.write_text("test content")
    return file_path

@pytest.fixture(scope="session")
def cellframe_session():
    """Session-wide fixture для CellFrame"""
    try:
        import CellFrame
        yield CellFrame
    except ImportError:
        yield None

@pytest.fixture
def test_config():
    """Конфигурация для тестов"""
    return {
        "timeout": 30,
        "retries": 3,
        "debug": True
    }
