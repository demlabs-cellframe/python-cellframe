# Core Modules

The SDK is divided into several main modules.

## CellFrame Module

This is the main entry point for blockchain logic.

*   **CellFrame.init() / deinit()**: Initialization and cleanup.
*   **CellFrame.set_log_level()**: Logging configuration.

### CellFrame.chain
Functionality related to the blockchain structure.
*   `Wallet`: Management of keys and addresses.
*   `TX`: Transaction creation and parsing.
*   `Ledger`: Access to the ledger (transactions history, balances).
*   `Chain`: Block and atom management.

### CellFrame.network
Networking and node communication.
*   `Net`: Network configuration.
*   `Node`: Remote node interaction.

### CellFrame.services
Services running on the node.
*   `Service`: Base class for creating custom services.

### CellFrame.common
Common utilities and types.
*   `Datum`: Base unit of data in the chain.

## DAP Module (`python-dap`)

Low-level system access.

### dap.core
*   `config`: Configuration file parsing.
*   `log_it`: Logging function.

### dap.crypto
*   `HashFast`: Hash functions.
*   `Sign`: Digital signatures.
*   `CryptoKey`: Key management.

### dap.global_db
*   Global Database (GDB) access (KeyValue storage distributed across the network).

