#!/bin/bash

rm -rf build/docs
mkdir -p build/docs

cd build/docs

sudo apt install python3-virtualenv
virtualenv -p python3 .venv
.venv/bin/activate

pip install -U sphinx
pip install -U sphinx-markdown-builder

{ echo "n"; echo "Python documentation"; echo "Demlabs"; echo "1";  echo "en"; } | sphinx-quickstart

sphinx-apidoc -f -o . ../../dists/python-modules/pycfhelpers/

sed -i '1s/^/import os\n/' conf.py

sed -i '2s/^/import sys\n/' conf.py

sed -i '3s|^|sys.path.insert(0, os.path.abspath("../../dists/python-modules"))\n|' conf.py

sed -i 's#extensions = \[\]#extensions = \["sphinx.ext.autodoc", "sphinx.ext.napoleon", "sphinx_markdown_builder"\]#' conf.py

sed -i '/extensions = \["sphinx.ext.autodoc", "sphinx.ext.napoleon", "sphinx_markdown_builder"\]/a autodoc_mock_imports = \["DAP", "CellFrame"\]' conf.py

sed -i '/autodoc_mock_imports = \["DAP", "CellFrame"\]/a napoleon_include_private_with_doc = True' conf.py

sed -i '/napoleon_include_private_with_doc = True/a napoleon_include_special_with_doc = True' conf.py

sed -i '/napoleon_include_special_with_doc = True/a napoleon_include_init_with_doc = True' conf.py

sphinx-build -b markdown . _build/pycfhelpers/markdown

build_doc_dir="$PWD/_build/pycfhelpers/markdown"

python3 ../../tools/fix_md.py "$build_doc_dir"

cd ../..

mkdir -p release/docs/pycfhelpers

mv build/docs/_build/pycfhelpers/markdown/*.md release/docs/pycfhelpers/