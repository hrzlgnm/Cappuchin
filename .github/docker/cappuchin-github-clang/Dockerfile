FROM debian:trixie@sha256:01a723bf5bfb21b9dda0c9a33e0538106e4d02cce8f557e118dd61259553d598
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
