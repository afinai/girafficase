FROM ubuntu as build

RUN echo "Acquire::Check-Valid-Until \"false\";\nAcquire::Check-Date \"false\";" | cat > /etc/apt/apt.conf.d/10no--check-valid-until

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y

RUN apt install build-essential manpages-dev software-properties-common -y
RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y
RUN apt update && apt install gcc-11 g++-11 -y

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110 \
--slave /usr/bin/g++ g++ /usr/bin/g++-11 \
--slave /usr/bin/gcov gcov /usr/bin/gcov-11 \
--slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-11 \
--slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-11

RUN apt install mysql-server mysql-client -y

RUN apt-get install -y \
    openssh-server \
    gdb \
	git \
	pkg-config \
    rsync zip make cmake ninja-build \
    libboost-dev \
	libssl-dev \
	unixodbc-dev unixodbc\
    flatbuffers-compiler libflatbuffers-dev
	
RUN git clone https://github.com/pocoproject/poco.git && cd poco && git checkout  poco-1.11.0-release && make install

ENV CMAKE_C_COMPILER=/usr/bin/gcc
ENV CMAKE_CXX_COMPILER=/usr/bin/g++
ENV CMAKE_FLATBUFFERS_COMPILER=/usr/bin/flatc

WORKDIR /root

RUN mkdir giraffic && mkdir giraffic/src && mkdir giraffic/contrib && mkdir giraffic/config

COPY ./docker/CMakeLists.txt giraffic
COPY ./src giraffic/src
COPY ./contrib giraffic/contrib
COPY ./config giraffic/config

# expected executable path: /root/build/giraffic/src
RUN mkdir build && cd build && cmake ../giraffic && make

FROM ubuntu as run

RUN echo "Acquire::Check-Valid-Until \"false\";\nAcquire::Check-Date \"false\";" | cat > /etc/apt/apt.conf.d/10no--check-valid-until

ENV DEBIAN_FRONTEND=noninhteractive

RUN apt-get update -y

RUN apt install build-essential manpages-dev software-properties-common -y
RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y
RUN apt update && apt install gcc-11 g++-11 -y

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110 \
--slave /usr/bin/g++ g++ /usr/bin/g++-11 \
--slave /usr/bin/gcov gcov /usr/bin/gcov-11 \
--slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-11 \
--slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-11

RUN apt-get update && apt-get install -y libstdc++6

WORKDIR /root

# Copy binary from build container
COPY --from=build /usr/lib/libPoco* /usr/lib
COPY --from=build /root/giraffic/build .

# Run service
CMD ["./bin/giraffic"]
