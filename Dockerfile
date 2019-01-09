# docker build . -t gui-statsgen
# docker run -d --name gsg -p 5022:22 gui-statsgen
# ssh -X -p 5022 root@127.0.0.1 /opt/GUI/GUI
FROM ubuntu:bionic

ENV SSH_PASSWORD "rootpass"

RUN apt-get -qq update
RUN apt-get -qq -y install build-essential \
                           qt5-default \
						   libqt5charts5-dev \
						   supervisor \
						   openssh-server \
						   >/dev/null

# Install SSH access
RUN mkdir /var/run/sshd
RUN echo "root:$SSH_PASSWORD" | chpasswd
RUN sed -i 's/^.*PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN echo "X11UseLocalhost no" >> /etc/ssh/sshd_config
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

# Configure supervisor
RUN mkdir -p /var/log/supervisor
RUN echo "[supervisord]" >> /etc/supervisor/conf.d/supervisord.conf
RUN echo "nodaemon=true" >> /etc/supervisor/conf.d/supervisord.conf
RUN echo "[program:sshd]" >> /etc/supervisor/conf.d/supervisord.conf
RUN echo "command=/usr/sbin/sshd -D" >> /etc/supervisor/conf.d/supervisord.conf

COPY GUI /opt/GUI

WORKDIR /opt/GUI
RUN qmake
RUN make -s -j$(nproc)

CMD [ "/usr/bin/supervisord", "-c",  "/etc/supervisor/conf.d/supervisord.conf" ]

EXPOSE 22
