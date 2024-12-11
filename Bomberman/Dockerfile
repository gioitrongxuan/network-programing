FROM debian:10

RUN apt-get update
RUN apt-get install -y gcc make 
RUN apt-get install -y libsdl2-dev
RUN apt-get install -y libsdl2-image-dev
RUN apt-get install -y libsdl2-ttf-dev
RUN apt-get install -y emacs

COPY ./ /app
WORKDIR /app
