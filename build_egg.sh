#!/bin/sh
python3 setup_dap.py bdist -d build -u root -g root --formats egg 
python3 setup_cellframe.py bdist -d build -u root -g root --formats egg
