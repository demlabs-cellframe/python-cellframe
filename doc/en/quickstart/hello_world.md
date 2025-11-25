# Hello World

This tutorial will guide you through creating your first Python script using CellFrame SDK.

## 1. Basic Script

Create a file named `hello_cellframe.py`:

```python
import CellFrame
import dap.core

def main():
    # Initialize the SDK
    # This sets up memory management, logging, and core subsystems
    if not CellFrame.init():
        print("Failed to initialize CellFrame")
        return

    print("Hello, CellFrame!")
    
    # Get Versions
    print(f"CellFrame Version: {CellFrame.get_version()}")
    print(f"DAP SDK Version: {dap.core.get_version()}")

    # Clean up resources
    CellFrame.deinit()

if __name__ == "__main__":
    main()
```

## 2. Running the Script

Run it with python3:

```bash
python3 hello_cellframe.py
```

## 3. Explanation

*   `import CellFrame`: Imports the main CellFrame SDK module.
*   `CellFrame.init()`: Essential step. Initializes the C libraries (memory pools, loggers). Must be called before other functions.
*   `CellFrame.deinit()`: Frees resources. Good practice to call at exit.

