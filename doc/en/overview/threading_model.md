# Threading Model

Understanding the threading model is crucial for writing stable applications.

## The Event Loop

The SDK utilizes C-based event loops for network I/O and blockchain consensus operations.
Your Python code interacts with these underlying loops.

## GIL (Global Interpreter Lock)

Since Python has a GIL, only one Python thread can execute Python bytecode at a time.
When the SDK calls a Python callback (e.g. event listener):
1.  It acquires the GIL.
2.  It executes the Python function.
3.  It releases the GIL.

## Implications

*   **Blocking Operations**: Do NOT perform long-running blocking operations (like `time.sleep` or heavy calculations) in callbacks. This may block the SDK's event processing!
*   **Async/Await**: Python's `asyncio` is not natively integrated with the SDK's C event loop yet, but you can use threads for heavy tasks.
*   **Thread Safety**: The SDK objects wrap C structures. Accessing the same C object from multiple Python threads *simultaneously* without locking might be unsafe depending on the specific object's internal C implementation.

## Recommendations

1.  Keep callbacks fast.
2.  Offload heavy processing to separate Python threads or processes if necessary.
3.  Use GDB (Global Database) for state persistence across restarts and potential concurrency handling via its API.

