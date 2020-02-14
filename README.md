# python-cellframe

## Build and Run tests:
To install, you must have Python3.7 and a scikit-build.
```
git clone https://gitlab.demlabs.net/cellframe/python-cellframe.git
cd python-cellframe
git submodule update --init 
cd cellframe-sdk
git submodule update --init
cd ..
sudo python3.7 setup.py install
python3.7 test/main_test.py
```

## How to package 
First install packages 
```
sudo apt-get install devscripts dpkg-dev build-essential
```