FROM ubuntu:18.04
RUN mkdir work
WORKDIR work
COPY . .
RUN ./install.sh
