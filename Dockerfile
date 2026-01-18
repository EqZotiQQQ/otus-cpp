FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive 

RUN apt-get update --fix-missing && \
    apt-get install -y --no-install-recommends \
        ca-certificates \
        curl \
        gnupg \
        lsb-release \
        software-properties-common \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        cmake \
        ninja-build \
        build-essential \
        git \
        python3 \
        python3-pip \
        doxygen \
        graphviz \
        clang \
        gdb \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN pip3 install --break-system-packages conan --upgrade

# Create default Conan profile (important!)
RUN conan profile detect --force

RUN cmake --version && ninja --version

COPY conanfile.txt /app/conanfile.txt

WORKDIR /app

RUN conan install . --build=missing


