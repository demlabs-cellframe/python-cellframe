#!/bin/bash

# Создаем виртуальное окружение 
sudo apt install python3-virtualenv
virtualenv -p python3 .venv
.venv/bin/activate

# Устанавливаем Sphinx 
pip install -U sphinx
pip install -U sphinx-markdown-builder

# Вызываем скрипт для генерации pycfhelpers документации
rm -rf pycfhelpers_doc
mkdir pycfhelpers_doc

cd ../dists/python-modules/pycfhelpers/scripts

bash pycfhelpers_doc.sh

mv ../../pycfhelpers_doc/_build/markdown/*.md ../../../../docs/pycfhelpers_doc

# Далее для построения документации остальных модулей нужно проделать все в такой же последовательности как выше

cd ../../../../docs

# Чиним pycfhelpers_doc

pycfhelpers_doc_dir="$PWD/pycfhelpers_doc"

python3 fix_md.py "$pycfhelpers_doc_dir"

