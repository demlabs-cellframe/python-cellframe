# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import sys
sys.path.insert(0, os.path.abspath('..'))
sys.path.insert(0, os.path.abspath(os.path.join("..", "..", "pycfhelpers")))
# print(os.path.abspath('...'))
project = 'test'
copyright = '2024, tester'
author = 'tester'
release = '1.0.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['sphinx.ext.autodoc',
              'sphinx.ext.napoleon',
              'sphinx_markdown_builder']

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

autodoc_mock_imports = ['DAP', 'CellFrame', 'TxIn']

napoleon_include_private_with_doc = True
napoleon_include_special_with_doc = True
napoleon_include_init_with_doc = True


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'python_docs_theme'

html_static_path = ['_static']
html_show_sphinx = False # не работает
numpydoc_show_module_members = False # не работает
