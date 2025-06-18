#!/usr/bin/env python3
"""
Optimized setup.py for Python Cellframe
"""

from setuptools import setup, Extension
import os

# Optimization flags
extra_compile_args = [
    '-O3',
    '-std=c11', 
    '-D_REENTRANT',
    '-DDAP_MEMORY_OPTIMIZED',
    '-fvisibility=hidden'
]

# Debug flags (uncomment for debugging)
# extra_compile_args.extend(['-O0', '-g', '-fsanitize=address'])

ext_modules = [
    Extension(
        'CellFrame',
        sources=[
            'CellFrame/python-cellframe.c',
            'CellFrame/python-cellframe_common.c',
        ],
        include_dirs=[
            'include/',
        ],
        extra_compile_args=extra_compile_args,
        language='c'
    ),
]

setup(
    name="CellFrame",
    version="0.14.0",
    description="Optimized CellFrame SDK Python bindings",
    author="Demlabs",
    license="GNU GPLv3",
    ext_modules=ext_modules,
    python_requires=">=3.8",
    zip_safe=False,
)
