FROM ubuntu:bionic
WORKDIR /root
RUN apt update &&\
    apt upgrade -y &&\
    apt install build-essential git nano mc -y && \
    git clone https://github.com/NicolaToscan/labSO-project.git
