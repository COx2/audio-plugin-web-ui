FROM ubuntu:latest AS base

RUN DEBIAN_FRONTEND=noninteractive apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    git \
    clang-11 \
    cmake \
    ninja-build \
    pkg-config \
    openssh-client \
    libjack-jackd2-dev \
    ladspa-sdk \
    libcurl4-openssl-dev  \
    libfreetype6-dev \
    libx11-dev  \
    libxcomposite-dev  \
    libxcursor-dev  \
    libxcursor-dev  \
    libxext-dev  \
    libxinerama-dev  \
    libxrandr-dev \
    libxrender-dev \
    libwebkit2gtk-4.0-dev \
    libglu1-mesa-dev  \
    mesa-common-dev \
    libasound2-dev

# Make sure clang is the default compiler:
RUN DEBIAN_FRONTEND=noninteractive \
    update-alternatives --install /usr/bin/cc cc /usr/bin/clang-11 100

RUN DEBIAN_FRONTEND=noninteractive \
    update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-11 100
    
# Custom build phase
COPY . /app

WORKDIR /app

RUN ./Scripts/build_linux_ninja.sh
