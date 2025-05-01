FROM ghcr.io/void-linux/void-glibc@sha256:7816cc9f516017282a0ebc93fe23ded9fc5208cc243f9188f0f75ab6d3e3d62f
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash clang clang-tools-extra cmake git gzip jq ninja tar wget \
    && xbps-remove -Ooy
LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Void Linux Glibc Clang"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-glibc"
