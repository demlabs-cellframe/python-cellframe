#!/bin/bash

rm -rf build/docs
mkdir -p build/docs

cd build/docs

sudo apt install python3-virtualenv
virtualenv -p python3 .venv
.venv/bin/activate

pip install -U sphinx
pip install -U sphinx-markdown-builder

# pycfhelpers documentation

rm -rf pycfhelpers_docs
mkdir -p pycfhelpers_docs
cd pycfhelpers_docs

{ echo "n"; echo "Python documentation"; echo "Demlabs"; echo "1";  echo "en"; } | sphinx-quickstart

sphinx-apidoc -f -o . ../../../dists/python-modules/pycfhelpers/

sed -i '1s/^/import os\n/' conf.py

sed -i '2s/^/import sys\n/' conf.py

sed -i '3s|^|sys.path.insert(0, os.path.abspath("../../../dists/python-modules"))\n|' conf.py

sed -i 's#extensions = \[\]#extensions = \["sphinx.ext.autodoc", "sphinx.ext.napoleon", "sphinx_markdown_builder"\]#' conf.py

sed -i '/extensions = \["sphinx.ext.autodoc", "sphinx.ext.napoleon", "sphinx_markdown_builder"\]/a autodoc_mock_imports = \["DAP", "CellFrame", "pydantic", "datetime", "sqlalchemy", "json"\]' conf.py

sed -i '/autodoc_mock_imports = \["DAP", "CellFrame", "pydantic", "datetime", "sqlalchemy", "json"\]/a napoleon_include_private_with_doc = True' conf.py

sed -i '/napoleon_include_private_with_doc = True/a napoleon_include_special_with_doc = True' conf.py

sed -i '/napoleon_include_special_with_doc = True/a napoleon_include_init_with_doc = True' conf.py

sphinx-build -b markdown . _build/pycfhelpers/markdown

# pycftools documentation

cd ../

rm -rf pycftools_docs
mkdir -p pycftools_docs
cd pycftools_docs

{ echo "n"; echo "Python documentation"; echo "Demlabs"; echo "1";  echo "en"; } | sphinx-quickstart

sphinx-apidoc -f -o . ../../../dists/python-modules/pycftools/database/
sphinx-apidoc -o . ../../../dists/python-modules/pycftools/schemas/

sed -i '1s/^/import os\n/' conf.py

sed -i '2s/^/import sys\n/' conf.py

sed -i '3s|^|sys.path.insert(0, os.path.abspath("../../../dists/python-modules/pycftools"))\n|' conf.py

sed -i 's#extensions = \[\]#extensions = \["sphinx.ext.autodoc", "sphinx.ext.napoleon", "sphinx_markdown_builder"\]#' conf.py

sed -i '/extensions = \["sphinx.ext.autodoc", "sphinx.ext.napoleon", "sphinx_markdown_builder"\]/a autodoc_mock_imports = \["DAP", "CellFrame", "pydantic", "datetime", "sqlalchemy", "json"\]' conf.py

sed -i '/autodoc_mock_imports = \["DAP", "CellFrame", "pydantic", "datetime", "sqlalchemy", "json"\]/a napoleon_include_private_with_doc = True' conf.py

sed -i '/napoleon_include_private_with_doc = True/a napoleon_include_special_with_doc = True' conf.py

sed -i '/napoleon_include_special_with_doc = True/a napoleon_include_init_with_doc = True' conf.py

sphinx-build -b markdown . _build/pycftools/markdown

# others

cd ../

build_doc_pycfhepers_dir="$PWD/pycfhelpers_docs/_build/pycfhelpers/markdown"

build_doc_pycftools_dir="$PWD/pycftools_docs/_build/pycftools/markdown"

python3 ../../tools/fix_md.py "$build_doc_pycfhepers_dir"

python3 ../../tools/fix_md.py "$build_doc_pycftools_dir"

cd ../..

mkdir -p release/docs/pycfhelpers

mv build/docs/pycfhelpers_docs/_build/pycfhelpers/markdown/*.md release/docs/pycfhelpers/

mkdir -p release/docs/pycftools

mv build/docs/pycftools_docs/_build/pycftools/markdown/*.md release/docs/pycftools/