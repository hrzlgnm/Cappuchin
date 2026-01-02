FROM ghcr.io/void-linux/void-glibc@sha256:a1c486c9480b13304b67c651e8cf444588019de6e94fb07108c14ea3be631d9a
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
