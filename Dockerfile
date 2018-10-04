FROM b90727ba45c9

RUN apt-get -qq update
RUN apt-get -qq -y install build-essential >/dev/null

COPY GUI /opt/GUI

WORKDIR /opt/GUI
RUN qmake
RUN make -j$(nproc)

CMD /opt/GUI/GUI
