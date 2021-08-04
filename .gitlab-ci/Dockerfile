# Wee need unstable for now because of check version
FROM debian:unstable

ENV HOME /root

RUN \
  apt-get update && \
  apt-get -y upgrade && \
  apt-get -y install --no-install-recommends \
    autoconf autoconf-archive automake libtool libconfig-dev \
    make pkg-config gcc gperf \
    check cppcheck valgrind lcov \
    zip xz-utils \
    && \
  apt-get clean

CMD [ "bash" ]

# vim: ft=dockerfile
