FROM ubuntu:18.04
RUN apt-get update -y --fix-missing
RUN apt-get install -y apt-utils
RUN apt-get install -y alien build-essential python3 python3-pip git clang nano libssl-dev dos2unix autoconf automake pkg-config
RUN apt-get clean -y
RUN pip3 install wget
RUN mkdir work
WORKDIR work

RUN wget https://github.com/Kitware/CMake/releases/download/v3.19.2/cmake-3.19.2.tar.gz 
RUN tar -xzvf 



COPY . .



RUN cmake --version
RUN python3 --version
RUN clang --version

RUN python3 ./msp430_toolchain.py