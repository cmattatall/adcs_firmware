FROM ubuntu:18.04

COPY ./install_toolchain_ubuntu.sh .
RUN chmod +x ./install_toolchain_ubuntu.sh
RUN ./install_toolchain_ubuntu.sh

#RUN mkdir work
#WORKDIR work
#COPY . .




# RUN apt-get update && \
#     apt-get install -y redis-server && \
#     apt-get clean

# RUN apt-get update -y
# RUN apt-get install -y wget
# RUN apt-get install -y gcc
# RUN apt-get install -y g++
# RUN apt-get install -y git
# RUN apt-get install -y nano
# RUN apt-get install -y libssl-dev
# RUN apt-get install -y tar
# RUN apt-get install -y curl
# RUN apt-get install -y software-properties-common
# RUN apt-get install -y gnupg-agent
# RUN apt-get install -y ca-certificates
# RUN apt-get install -y apt-transport-https
# RUN apt-get install -y automake
# RUN apt-get install -y autoconf
# RUN apt-get install -y libudev-dev
# RUN apt-get install -y udev
# RUN apt-get install -y autotools-dev
# RUN apt-get install -y libtool
# RUN apt-get install -y libusb-1.0.0-dev
# RUN apt-get install -y libusb-dev
# RUN apt-get install -y pkg-config
# RUN apt-get install -y make
# RUN apt-get install -y vim
# RUN apt-get install -y python3
# RUN apt-get install -y libreadline-dev
# RUN apt-get install -y zip
# RUN apt-get install -y squashfuse
# RUN apt-get install -y valgrind
# RUN apt-get install -y clang
# RUN apt-get install -y libboost-all-dev
# RUN apt-get install -y libudev-dev
# RUN apt-get install -y libfox-1.6-dev

# RUN mkdir /opt/cmake
# RUN wget "https://github.com/Kitware/CMake/releases/download/v3.19.5/cmake-3.19.5-Linux-x86_64.tar.gz" -P /opt/cmake
# RUN tar -xvzf /opt/cmake/cmake-3.19.5-Linux-x86_64.tar.gz -C /opt/cmake
# RUN ln -s /opt/cmake/cmake-3.19.5-Linux-x86_64/bin/cmake /usr/local/bin/cmake
# RUN ln -s /opt/cmake/cmake-3.19.5-Linux-x86_64/bin/ccmake /usr/local/bin/ccmake
# RUN ln -s /opt/cmake/cmake-3.19.5-Linux-x86_64/bin/cmake-gui /usr/local/bin/cmake-gui
# RUN ln -s /opt/cmake/cmake-3.19.5-Linux-x86_64/bin/cpack /usr/local/bin/cpack
# RUN ln -s /opt/cmake/cmake-3.19.5-Linux-x86_64/bin/ctest /usr/local/bin/ctest
# RUN cmake --version && ctest --version && cpack --version


# RUN mkdir /opt/mspdebug
# WORKDIR /opt/mspdebug

# # HID API
# RUN git clone git://github.com/signal11/hidapi.git
# WORKDIR ./hidapi
# RUN ./bootstrap
# RUN ./configure --prefix=/usr
# RUN make
# RUN make install
# WORKDIR /

# #LIBMSP430.SO
# RUN mkdir /opt/mspdebug/libmsp430
# WORKDIR /opt/mspdebug/libmsp430
# RUN wget http://www.ti.com/lit/sw/slac460y/slac460y.zip
# RUN unzip slac460y.zip
# RUN wget https://dlbeer.co.nz/articles/slac460y/slac460y-linux.patch
# RUN patch -p1 < slac460y-linux.patch
# RUN make && make install
# WORKDIR /

# # MSPDEBUG
# WORKDIR /opt/mspdebug
# RUN git clone https://github.com/dlbeer/mspdebug.git
# WORKDIR /opt/mspdebug/mspdebug
# RUN make && make install
# WORKDIR /

