FROM ghcr.io/void-linux/void-glibc@sha256:32363eeb92798503405864420cdc70eaa64724f61020f7f2e13dbb6ceb2b6bbb
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash clang clang-tools-extra cmake git gzip jq ninja tar wget \
    && xbps-remove -Ooy
LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Void Linux Glibc Clang"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-glibc"
