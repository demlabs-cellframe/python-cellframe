# Architecture Overview

The Python-Cellframe SDK provides Python bindings for the CellFrame platform, allowing developers to write services in Python while leveraging the performance of the core C implementation.

## Layers

The architecture consists of several layers:

1.  **python-cellframe**: High-level bindings for blockchain specific functionality (Wallets, Transactions, Ledger, Consensus).
2.  **python-dap**: Low-level bindings for the DAP SDK (Networking, Cryptography, Configuration, Global Database).
3.  **cellframe-sdk (C)**: The core implementation of CellFrame blockchain protocols.
4.  **dap-sdk (C)**: The foundational library for networking, crypto, and distributed systems.

## Interaction

Python code interacts with the C core through Python C Extensions.

*   **Calls**: Python calls methods on SDK objects (`Wallet`, `AppCliServer`, etc.), which are forwarded to C functions.
*   **Callbacks**: The C core can call Python functions (listeners, notifiers) when events occur (e.g., network packet received, transaction detected).

## Memory Management

Most objects in Python-Cellframe are wrappers around C structures.
*   **Ownership**: Some objects are owned by the C core and only referenced by Python. Others are created in Python and managed by Python's GC, which cleans up the underlying C memory when the object is destroyed.
*   **Threading**: See `threading_model.md` for details on concurrency.

