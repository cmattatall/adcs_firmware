#!/bin/bash

function cleanup()
{
    while(( "$?" == 0 ))
    do
        pushd ../ > /dev/null
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
apt-get install -y libboost-dev
apt-get install -y libboost-all-dev

apt-get install -y libudev-dev
apt-get install -y libfox-1.6-dev

TOOLCHAIN_INSTALL_ROOT="/opt/msp430_toolchain"
CMAKE_INSTALL_ROOT="/usr/local/cmake"


mkdir -p -- $CMAKE_INSTALL_ROOT
pushd $CMAKE_INSTALL_ROOT
wget "https://github.com/Kitware/CMake/releases/download/v3.19.5/cmake-3.19.5-Linux-x86_64.tar.gz" -P $(pwd)
tar -xvzf $(pwd)/cmake-3.19.5-Linux-x86_64.tar.gz -C $(pwd)
ln -s $(pwd)/cmake-3.19.5-Linux-x86_64/bin/cmake /usr/local/bin/cmake
ln -s $(pwd)/cmake-3.19.5-Linux-x86_64/bin/ccmake /usr/local/bin/ccmake
ln -s $(pwd)/cmake-3.19.5-Linux-x86_64/bin/cmake-gui /usr/local/bin/cmake-gui
ln -s $(pwd)/cmake-3.19.5-Linux-x86_64/bin/cpack /usr/local/bin/cpack
ln -s $(pwd)/cmake-3.19.5-Linux-x86_64/bin/ctest /usr/local/bin/ctest
cmake --version && ctest --version && cpack --version
popd


mkdir -p -- $TOOLCHAIN_INSTALL_ROOT
pushd $TOOLCHAIN_INSTALL_ROOT

# HID API
git clone git://github.com/signal11/hidapi.git
pushd ./hidapi
./bootstrap
./configure --prefix=/usr
make && make install
popd # leave hidapi


mkdir -p -- $TOOLCHAIN_INSTALL_ROOT/libmsp430
pushd $TOOLCHAIN_INSTALL_ROOT/libmsp430
wget http://www.ti.com/lit/sw/slac460y/slac460y.zip
unzip slac460y.zip
wget https://dlbeer.co.nz/articles/slac460y/slac460y-linux.patch
patch -p1 < slac460y-linux.patch
make && make install
popd # leave $TOOLCHAIN_INSTALL_ROOT/libmsp430


git clone https://github.com/dlbeer/mspdebug.git
pushd $TOOLCHAIN_INSTALL_ROOT/mspdebug
make && make install
#ln -s $TOOLCHAIN_INSTALL_ROOT/mspdebug/mspdebug /usr/local/bin/mspdebug
udevadm control --reload-rules && udevadm trigger
popd # leave $TOOLCHAIN_INSTALL_ROOT/mspdebug

# DEVICE SUPPORT FILES
wget "https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/latest/exports/msp430-gcc-support-files-1.208.zip"
unzip msp430-gcc-support-files-1.208.zip

# MSP430-ELF-GCC
wget "http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/9_2_0_0/export/msp430-gcc-9.2.0.50_linux64.tar.bz2"  && tar -xjvf msp430-gcc-9.2.0.50_linux64.tar.bz2

pushd $TOOLCHAIN_INSTALL_ROOT/msp430-gcc-9.2.0.50_linux64/bin
find ~+ -type f -executable -exec sh -c 'ln -s {} /usr/local/bin/"$(basename {})" ' \;
popd # leave $TOOLCHAIN_INSTALL_ROOT/msp430-gcc-9.2.0.50_linux64/bin