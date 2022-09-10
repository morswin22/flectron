FROM nvidia/opengl:1.0-glvnd-devel-ubuntu20.04

ENV NVIDIA_DRIVER_CAPABILITIES=${NVIDIA_DRIVER_CAPABILITIES},display
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        git \
        ca-certificates \
        build-essential \
        g++ \
        libxinerama-dev \
        libxext-dev \
        libxrandr-dev \
        libxi-dev \
        libxcursor-dev \
        libxxf86vm-dev \
        libvulkan-dev \ 
        cmake \
        doxygen \
        graphviz \
        gdb && \
    rm -rf /var/lib/apt/lists/* && \
    git clone https://github.com/morswin22/flectron.git

WORKDIR /flectron/build/Debug
RUN cmake -D CMAKE_BUILD_TYPE=DEBUG /flectron
RUN make -j"$(nproc)"

WORKDIR /flectron/build/Release
RUN cmake -D CMAKE_BUILD_TYPE=RELEASE /flectron
RUN make -j"$(nproc)"

COPY . /flectron

WORKDIR /flectron/build/Debug
RUN cmake -D CMAKE_BUILD_TYPE=DEBUG /flectron
RUN make -j"$(nproc)"

WORKDIR /flectron/build/Debug/tests
CMD ./flectron-demo

# WORKDIR /flectron/build/Release
# RUN cmake -D CMAKE_BUILD_TYPE=RELEASE /flectron
# RUN make -j"$(nproc)"

# WORKDIR /flectron/build/Release/tests
# CMD ./flectron-demo
