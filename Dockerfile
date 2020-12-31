# DOCKER FILE TO TEST THE TOOLCHAIN INSTALLATION SCRIPT ON A FRESH MACHINE
FROM ubuntu:18.04
RUN mkdir work
WORKDIR work
RUN apt-get update -y
RUN apt-get install -y python3 python3-pip
COPY ./install_msp430_toolchain.py ./
RUN pip3 install wget
RUN python3 ./install_msp430_toolchain.py
RUN cmake --version
COPY . .

# if this passes, the toolchain is able to both compile native and cross compile
RUN python3 ./regressions.py 
