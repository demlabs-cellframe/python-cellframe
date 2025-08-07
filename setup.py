"""
Python Cellframe SDK - Modern Pythonic API for Cellframe Network
"""

from setuptools import setup, find_packages
import os
import shutil
import glob

# Читаем README для long_description
def read_readme():
    try:
        with open("README.md", "r", encoding="utf-8") as fh:
            return fh.read()
    except FileNotFoundError:
        return "Python bindings for Cellframe SDK - Modular Blockchain Platform"

# Find native libraries to include
def find_native_libraries():
    libraries = []
    # Look for built libraries in build directory
    build_dirs = ['build', './build', '../build']
    for build_dir in build_dirs:
        if os.path.exists(build_dir):
            # Find the actual library file (not symlinks)
            real_lib = glob.glob(f"{build_dir}/libpython_cellframe.so.*.*.*")
            if real_lib:
                # Copy to python_cellframe.so for proper Python module import
                src = real_lib[0]
                dst = f"{build_dir}/python_cellframe.so"
                if not os.path.exists(dst) or os.path.getmtime(src) > os.path.getmtime(dst):
                    shutil.copy2(src, dst)
                    print(f"Copied {src} -> {dst} for Python import")
                # Also copy to package directory for immediate access
                pkg_dst = "python_cellframe.so"
                if not os.path.exists(pkg_dst) or os.path.getmtime(src) > os.path.getmtime(pkg_dst):
                    shutil.copy2(src, pkg_dst)
                    print(f"Copied {src} -> {pkg_dst} for package")
                libraries.append(pkg_dst)
            break
    
    return libraries

# Версия SDK
VERSION = "1.0.0"

setup(
    name="python-cellframe",
    version=VERSION,
    
    # Metadata
    author="Demlabs",
    author_email="support@demlabs.net",
    description="Modern Pythonic API for Cellframe Network blockchain platform",
    long_description=read_readme(),
    long_description_content_type="text/markdown",
    url="https://github.com/demlabs-cellframe/cellframe-node",
    
    # Packages
    packages=find_packages(),
    include_package_data=True,
    
    # Package data - include native module
    package_data={
        "": ["python_cellframe.so", "*.so", "*.so.*"],  # Include native modules
        "CellFrame": [
            "py.typed",  # Type hints support
            "templates/*.json",
            "schemas/*.json",
        ],
    },
    
    # Requirements
    install_requires=["python-dap>=3.0.0"],
    
    # Python version
    python_requires=">=3.8",
    
    # Classification
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Topic :: Software Development :: Libraries :: Python Modules",
        "Topic :: System :: Distributed Computing",
        "Topic :: Internet :: WWW/HTTP :: Dynamic Content",
        "Topic :: Security :: Cryptography",
        "Topic :: Office/Business :: Financial",
    ],
    
    # Keywords
    keywords="cellframe blockchain cryptocurrency defi dap sdk python",
    
    # Project URLs
    project_urls={
        "Bug Reports": "https://github.com/demlabs-cellframe/cellframe-node/issues",
        "Source": "https://github.com/demlabs-cellframe/cellframe-node",
        "Documentation": "https://cellframe.net/docs",
        "Homepage": "https://cellframe.net",
        "Changelog": "https://github.com/demlabs-cellframe/cellframe-node/blob/master/CHANGELOG.md",
    },
    
    # Entry points
    entry_points={
        "console_scripts": [
            "cellframe=cellframe.cli:main",
            "cellframe-migrate=cellframe.migration:main",
        ],
    },
    

    
    # Extras
    extras_require={
        "dev": [
            "pytest>=7.0.0",
            "pytest-cov>=4.0.0",
            "black>=22.0.0",
            "flake8>=5.0.0",
            "mypy>=1.0.0",
            "pre-commit>=2.0.0",
        ],
        "async": [
            "aiohttp>=3.8.0",
            "asyncio>=3.4.0",
        ],
        "performance": [
            "psutil>=5.9.0",
            "memory-profiler>=0.60.0",
        ],
        "testing": [
            "pytest>=7.0.0",
            "pytest-asyncio>=0.21.0",
            "pytest-mock>=3.10.0",
            "factory-boy>=3.2.0",
        ],
    },
    
    # Zip safe
    zip_safe=False,
    
    # Options
    options={
        "bdist_wheel": {
            "universal": False,
        },
    },
) 