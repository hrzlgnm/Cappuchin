FROM ghcr.io/void-linux/void-musl@sha256:e38c278887c2f0bc455f8f6cb019dbba8fd9f0e3ab4956132c04656291ec6d5a
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y \
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
    nodejs \
    python3-pip \
    rust-sccache \
    tar \
    wget \
    && xbps-remove -Ooy
RUN python3 -m pip install --break-system-packages --root-user-action=ignore --no-cache-dir gersemi==0.20.0

LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Void Linux Musl Clang"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-musl"
