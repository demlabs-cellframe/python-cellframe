# Проблемы установки

Частые проблемы при установке Python-Cellframe.

## Зависимости

### "ImportError: libcellframe_sdk.so: cannot open shared object file"
*   **Причина**: Библиотека C SDK не найдена в системных путях.
*   **Решение**: Убедитесь, что пакет `cellframe-sdk` установлен.
    *   Проверьте `ldconfig -p | grep cellframe`.
    *   Если собирали из исходников, запустите `sudo ldconfig`.

### "ModuleNotFoundError: No module named 'dap'"
*   **Причина**: `python-dap` не установлен или не найден Python'ом.
*   **Решение**:
    *   Установите `python-dap_*.deb`.
    *   Проверьте версию Python (например, python3 vs python3.9).

## Ошибки прав доступа

### "Permission denied: /opt/cellframe-node/..."
*   **Причина**: Запуск скриптов без прав доступа к директориям ноды.
*   **Решение**: Запускайте от `root` или добавьте пользователя в группу `cellframe`.

