# python-cellframe

## Prerequisites
To install, you must have Python3.7 including pip and a scikit-build.
sudo apt-get install pip3
If there is a problem on Debian buster 
"unknown system group 'Debian-exim' in statoverride file; the system group got removed"
Use the following command to fix it
dpkg-statoverride --remove /etc/exim4/passwd.client
Now repeat sudo apt-get install pip3

## Build and Run tests:
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
sudo apt-get install libev-dev
sudo apt-get install libmagic-dev
sudo apt-get install libjson-c-dev
sudo apt-get install libjsoncpp-dev
sudo apt-get install libsqlite3-dev
sudo apt-get install libtalloc-dev
sudo apt-get install traceroute
sudo apt-get install libverto-dev
sudo apt-get install lldb-7
sudo apt-get install libmemcached-dev
sudo apt-get install libldb-dev
sudo apt-get install libtalloc-dev
sudo apt-get install libtevent-dev
sudo apt-get install libpython3-dev
sudo apt-get install traceroute

```