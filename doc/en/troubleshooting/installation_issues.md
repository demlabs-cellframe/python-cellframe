# Installation Issues

Common issues when installing Python-Cellframe.

## Dependencies

### "ImportError: libcellframe_sdk.so: cannot open shared object file"
*   **Cause**: The C SDK library is not in the system library path.
*   **Solution**: Ensure `cellframe-sdk` DEB package is installed.
    *   Check `ldconfig -p | grep cellframe`.
    *   If built from source, run `sudo ldconfig`.

### "ModuleNotFoundError: No module named 'dap'"
*   **Cause**: `python-dap` is not installed or not found by Python.
*   **Solution**:
    *   Install `python-dap_*.deb`.
    *   Check if you are using the correct python version (e.g. python3 vs python3.9).

## Permission Errors

### "Permission denied: /opt/cellframe-node/..."
*   **Cause**: Running scripts without necessary privileges to access node directories.
*   **Solution**: Run as `root` or ensure your user is in the correct group (usually `cellframe`).

