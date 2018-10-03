FROM ubuntu:bionic

RUN apt-get -qq update
RUN apt-get -qq -y install build-essential qt5-default libqt5charts5-dev >/dev/null

COPY GUI /opt/GUI

WORKDIR /opt/GUI
RUN qmake
RUN make -j$(nproc)

CMD /opt/GUI/GUI
