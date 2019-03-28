from ubuntu:latest

RUN apt update -qq
RUN apt -yq install gnupg ca-certificates
RUN echo "deb https://mirror.mxe.cc/repos/apt xenial main" | tee /etc/apt/sources.list.d/mxeapt.list
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys D43A795B73B16ABE9643FE1AFD8FFF16DB45C6AB
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys C6BF758A33A3A276
RUN apt update -qq
RUN apt install -yq mxe-x86-64-w64-mingw32.static-qtbase mxe-x86-64-w64-mingw32.static-qtdeclarative
RUN apt install -yq qtbase5-dev qtdeclarative5-dev qt5-default build-essential

