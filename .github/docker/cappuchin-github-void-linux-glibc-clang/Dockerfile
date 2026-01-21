FROM ghcr.io/void-linux/void-glibc@sha256:4241b9d38aeebe0bff8118a71d4f32413f382a5928d6a74df77b846c10629dac
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
