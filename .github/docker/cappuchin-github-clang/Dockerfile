FROM debian:bookworm@sha256:264982ff4d18000fa74540837e2c43ca5137a53a83f8f62c7b3803c0f0bdcd56
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
    ninja-build && \
    rm -rf /var/lib/apt/lists/*
LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Clang"
LABEL org.opencontainers.image.base.name="debian:bookworm"
