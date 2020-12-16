FROM ubuntu:18.04
RUN apt-get update -y 
RUN apt-get upgrade -y
RUN apt-get install -y apt-utils && apt-get install -y build-essential libssl-dev git wget nano libusb-1.0.0-dev udev dos2unix autoconf automake pkg-config libssl-dev tar 
RUN apt-get clean -y
RUN wget https://cmake.org/files/v3.18/cmake-3.18.1.tar.gz 
RUN tar -xzvf cmake-3.18.1.tar.gz
WORKDIR cmake-3.18.1
RUN ./bootstrap
RUN make -j $(nproc)
RUN make install -j $(nproc)
WORKDIR /