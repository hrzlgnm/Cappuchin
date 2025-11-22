FROM ghcr.io/void-linux/void-glibc@sha256:59ee62518de285ae3561e59f13eefbb065cb60be03051b17046c08b4e9b4f8e2
RUN xbps-install -Syu xbps \
    && xbps-install -Syu \
    && xbps-install -Sy \
    bash \
    clang \
    clang-tools-extra \
    cmake \
    cppcheck \
    curl \
    git \
    gzip \
    jq \
    ninja \
    python3-pip \
    rust-sccache \
    tar \
    wget \
    && xbps-remove -Ooy
RUN python3 -m pip install --break-system-packages --root-user-action=ignore --no-cache-dir gersemi==0.23.1 

LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Void Linux Glibc Clang"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-glibc"
