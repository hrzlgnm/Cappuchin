FROM debian:bookworm@sha256:4abf773f2a570e6873259c4e3ba16de6c6268fb571fd46ec80be7c67822823b3

RUN \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install --yes --no-install-recommends \
    build-essential \
    ca-certificates \
    clang-13 \
    cmake \
    git \
    libc++-13-dev \
    libc++abi-13-dev \
    libc6-dev \
    libstdc++-11-dev \
    ninja-build \
    python3 && \
    rm -rf /var/lib/apt/lists/*
