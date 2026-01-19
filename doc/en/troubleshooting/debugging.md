# Debugging

How to debug Python applications using Python-Cellframe.

## Logging

Use standard Python logging. The SDK configures a logger named `CellFrame`.

```python
import logging

logger = logging.getLogger("CellFrame")
logger.info("Info message")
logger.error("Error message")
```

If you rely on the legacy API, `logIt` is available from the legacy DAP wrapper:

```python
from CellFrame.legacy.DAP.Core import logIt

logIt.info("Info message")
logIt.error("Error message")
```

## Interactive Debugging (pdb)

1. Insert `import pdb; pdb.set_trace()` in your Python code.
2. Run your script: `python3 my_script.py`.
3. Execution will pause at the breakpoint.

## Segmentation Faults

If the application crashes (segfault):
1. Enable core dumps: `ulimit -c unlimited`.
2. Run the application until it crashes.
3. Analyze the core dump with `gdb`:
    ```bash
    gdb python3 core
    bt
    ```
4. Look for Python frames in the backtrace.
