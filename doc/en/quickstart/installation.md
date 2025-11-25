# Installation Guide

This guide describes how to install the Python-Cellframe SDK.

## 1. Install from DEB Packages (Recommended)

The easiest way to install Python-Cellframe is using the pre-built DEB packages.

### Prerequisites
*   Debian 11/12 or Ubuntu 20.04/22.04
*   Python 3.9+

### Installation Steps

1.  Download the latest packages (`dap-sdk`, `cellframe-sdk`, `python-dap`, `python-cellframe`).
2.  Install them in the following order:

```bash
sudo dpkg -i dap-sdk_*.deb
sudo dpkg -i cellframe-sdk_*.deb
sudo dpkg -i python-dap_*.deb
sudo dpkg -i python-cellframe_*.deb
```

3.  Fix dependencies if necessary:

```bash
sudo apt-get install -f
```

## 2. Build from Source

If you need the latest development version, you can build from source.

### Prerequisites
*   CMake 3.10+
*   GCC/Clang
*   Python 3 Development Headers (`python3-dev`)

### Build Steps

```bash
git clone https://gitlab.demlabs.net/cellframe/python-cellframe.git
cd python-cellframe
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

## 3. Verify Installation

Check if the modules can be imported in Python:

```bash
python3 -c "import CellFrame; print(f'CellFrame version: {CellFrame.get_version()}')"
```

