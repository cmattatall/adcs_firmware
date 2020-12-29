FROM ubuntu:18.04
RUN apt-get update -y --fix-missing
RUN apt-get install -y apt-utils
RUN apt-get install -y alien build-essential python3 python3-pip git clang nano libssl-dev dos2unix autoconf automake pkg-config udev gdb wget
RUN apt-get clean -y
RUN pip3 install wget
RUN mkdir work
WORKDIR work

RUN wget https://github.com/Kitware/CMake/releases/download/v3.19.2/cmake-3.19.2.tar.gz 
RUN tar -xzvf cmake-3.19.2.tar.gz
WORKDIR cmake-3.19.2
RUN ./bootstrap && make -j $(nproc) && make install -j $(nproc)
WORKDIR work
COPY . .

RUN python3 ./install_msp430_toolchain.py
