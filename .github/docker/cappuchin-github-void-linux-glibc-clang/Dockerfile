FROM ghcr.io/void-linux/void-glibc@sha256:f5d88cafc86032345532fdeacaf0c84e6226c032bbfbe14130bbe0d9b11ec76c
RUN xbps-install -Syu xbps \
    && xbps-install -Syu \
    && xbps-install -Sy \
    bash \
    clang \
    clang-tools-extra \
    cmake \
    curl \
    git \
    gzip \
    jq \
    ninja \
    tar \
    wget \
    && xbps-remove -Ooy

LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Void Linux Glibc Clang"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-glibc"
