FROM ghcr.io/void-linux/void-musl@sha256:024a793a6389acb6c3fe387f8b1b7ff6a117f9dd540c2c9cc16858285d340e0d
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
RUN python3 -m pip install --break-system-packages --root-user-action=ignore --no-cache-dir gersemi==0.23.1

LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Void Linux Musl Clang"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-musl"
