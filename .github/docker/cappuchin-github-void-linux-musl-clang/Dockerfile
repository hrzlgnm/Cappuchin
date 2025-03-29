FROM ghcr.io/void-linux/void-musl@sha256:8184a64e38dfb5361f196465c3a2f3b31d43e5baa74cb66f49d5593ae81f3ba4
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash clang clang-tools-extra cmake cppcheck git gzip jq ninja tar wget rust-sccache tar \
    && xbps-remove -Ooy
LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Void Linux Musl Clang"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-musl"
LABEL org.opencontainers.image.base.digest="sha256:8184a64e38dfb5361f196465c3a2f3b31d43e5baa74cb66f49d5593ae81f3ba4"

