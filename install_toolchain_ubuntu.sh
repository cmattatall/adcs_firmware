#!/bin/bash

function cleanup()
{
    while(( "$?" == 0 ))
    do
        popd > /dev/null
    done
}
trap cleanup EXIT


apt-get update -y 
apt-get install -y redis-server
apt-get clean
apt-get install -y wget
apt-get install -y gcc
apt-get install -y g++
apt-get install -y git
apt-get install -y nano
apt-get install -y libssl-dev
apt-get install -y tar
apt-get install -y curl
apt-get install -y software-properties-common
apt-get install -y gnupg-agent
apt-get install -y ca-certificates
apt-get install -y apt-transport-https
apt-get install -y automake
apt-get install -y autoconf
apt-get install -y libudev-dev
apt-get install -y udev
apt-get install -y autotools-dev
apt-get install -y libtool
apt-get install -y libusb-1.0.0-dev
apt-get install -y libusb-dev
apt-get install -y pkg-config
apt-get install -y make
apt-get install -y vim
apt-get install -y python3
apt-get install -y libreadline-dev
apt-get install -y zip
apt-get install -y squashfuse
apt-get install -y valgrind
apt-get install -y clang
apt-get install -y libboost-all-dev
apt-get install -y libudev-dev
apt-get install -y libfox-1.6-dev

mkdir /opt/cmake
pushd /opt/cmake
wget "https://github.com/Kitware/CMake/releases/download/v3.19.5/cmake-3.19.5-Linux-x86_64.tar.gz" -P $(pwd)
tar -xvzf $(pwd)/cmake-3.19.5-Linux-x86_64.tar.gz -C $(pwd)
ln -s $(pwd)/cmake-3.19.5-Linux-x86_64/bin/cmake /usr/local/bin/cmake
ln -s $(pwd)/cmake-3.19.5-Linux-x86_64/bin/ccmake /usr/local/bin/ccmake
ln -s $(pwd)/cmake-3.19.5-Linux-x86_64/bin/cmake-gui /usr/local/bin/cmake-gui
ln -s $(pwd)/cmake-3.19.5-Linux-x86_64/bin/cpack /usr/local/bin/cpack
ln -s $(pwd)/cmake-3.19.5-Linux-x86_64/bin/ctest /usr/local/bin/ctest
cmake --version && ctest --version && cpack --version
popd

mkdir /opt/mspdebug
pushd /opt/mspdebug

# HID API
git clone git://github.com/signal11/hidapi.git
pushd ./hidapi
./bootstrap
./configure --prefix=/usr
make && make install
popd

#LIBMSP430.SO
mkdir ./libmsp430
pushd ./libmsp430
wget http://www.ti.com/lit/sw/slac460y/slac460y.zip
unzip slac460y.zip
wget https://dlbeer.co.nz/articles/slac460y/slac460y-linux.patch
patch -p1 < slac460y-linux.patch
make && make install
popd

# MSPDEBUG
git clone https://github.com/dlbeer/mspdebug.git
pushd mspdebug
make && make install
ln -s $(pwd)/mspdebug /usr/local/bin/mspdebug
popd

mspdebug --version
mspdebug tilib
