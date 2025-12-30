FROM debian:trixie@sha256:c71b05eac0b20adb4cdcc9f7b052227efd7da381ad10bb92f972e8eae7c6cdc9
RUN \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install --yes --no-install-recommends \
    build-essential \
    ca-certificates \
    clang-19 \
    cmake \
    git \
    libasan8 \
    libc++-19-dev \
    libc++abi-19-dev \
    libc6-dev \
    libclang-rt-dev \
    ninja-build && \
    rm -rf /var/lib/apt/lists/*
LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Clang"
LABEL org.opencontainers.image.base.name="debian:trixie"
