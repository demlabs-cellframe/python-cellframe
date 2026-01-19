# Hello World

A minimal script to verify that Python-Cellframe is installed and working.

## 1. Basic Script

Create a file named `hello_cellframe.py`:

```python
import CellFrame

def main():
    # Initialize global context
    CellFrame.initialize_context()

    print("Hello, CellFrame!")
    print(f"CellFrame version: {CellFrame.get_version()}")

    # Clean up resources
    CellFrame.shutdown_context()

if __name__ == "__main__":
    main()
```

## 2. Run

```bash
python3 hello_cellframe.py
```
