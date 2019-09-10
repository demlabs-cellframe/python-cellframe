# python-cellframe

## Build and Run tests:
```
git clone https://gitlab.demlabs.net/cellframe/python-cellframe.git
cd python-cellframe
git submodule update --init
git submodule foreach git submodule update --init
mkdir build
cd build
cmake ../
make
cp ../test/main_test.py ./
python3.7 main_test.py
```

