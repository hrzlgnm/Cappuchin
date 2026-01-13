FROM debian:trixie@sha256:f0b609fc09804b3e800e1357f6d8d6a7467ea8a93da2885206afee62034694e2
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
