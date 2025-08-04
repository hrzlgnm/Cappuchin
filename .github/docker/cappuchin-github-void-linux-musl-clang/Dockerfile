FROM ghcr.io/void-linux/void-musl@sha256:6d9937b50930da36adc43c9a8be4f605cd1079630988871a78a84b75065bee17
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
RUN python3 -m pip install --break-system-packages --root-user-action=ignore --no-cache-dir gersemi==0.22.0

LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Void Linux Musl Clang"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-musl"
