# Debugging

How to debug Python applications.

## Logging

Use `dap.log_it` or `CellFrame.logIt` (legacy alias) to write to the application log.

```python
from CellFrame import logIt

logIt.debug("Debug message")
logIt.info("Info message")
logIt.error("Error message")
```

Log location depends on your initialization configuration.

## Interactive Debugging (pdb)

You can use standard Python debugging tools.

1.  Insert `import pdb; pdb.set_trace()` in your python code.
2.  Run your script: `python3 my_script.py`.
3.  Execution will pause at the breakpoint.

## Segmentation Faults

If the application crashes (Segfault):
1.  Enable core dumps: `ulimit -c unlimited`.
2.  Run application until crash.
3.  Analyze core dump with `gdb`:
    ```bash
    gdb python3 core
    bt
    ```
4.  Look for the python frame in the backtrace.

