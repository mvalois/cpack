# docker build . -t cppack
# docker run -d --name gsg -p 5022:22 cppack
# ssh -X -p 5022 root@127.0.0.1 /opt/cppack/cppack-gui
FROM ubuntu:bionic

ENV SSH_PASSWORD "rootpass"

RUN apt-get -qq update
RUN apt-get -qq -y install \
	build-essential \
	qt5-default \
	libqt5charts5-dev \
	supervisor \
	openssh-server \
	>/dev/null

# Install SSH access
RUN mkdir /var/run/sshd && \
	echo "root:$SSH_PASSWORD" | chpasswd && \
	sed -i 's/^.*PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
	echo "X11UseLocalhost no" >> /etc/ssh/sshd_config && \
	sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

# Configure supervisor
RUN mkdir -p /var/log/supervisor && \
	echo "[supervisord]" >> /etc/supervisor/conf.d/supervisord.conf && \
	echo "nodaemon=true" >> /etc/supervisor/conf.d/supervisord.conf && \
	echo "[program:sshd]" >> /etc/supervisor/conf.d/supervisord.conf && \
	echo "command=/usr/sbin/sshd -D" >> /etc/supervisor/conf.d/supervisord.conf

COPY . /opt/cppack

WORKDIR /opt/cppack
RUN make -s

CMD [ "/usr/bin/supervisord", "-c",  "/etc/supervisor/conf.d/supervisord.conf" ]

EXPOSE 22
