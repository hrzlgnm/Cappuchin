FROM debian:bookworm@sha256:18023f131f52fc3ea21973cabffe0b216c60b417fd2478e94d9d59981ebba6af
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
LABEL org.opencontainers.image.base.digest="sha256:35286826a88dc879b4f438b645ba574a55a14187b483d09213a024dc0c0a64ed"
