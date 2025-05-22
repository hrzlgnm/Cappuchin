FROM debian:bookworm@sha256:bd73076dc2cd9c88f48b5b358328f24f2a4289811bd73787c031e20db9f97123
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
