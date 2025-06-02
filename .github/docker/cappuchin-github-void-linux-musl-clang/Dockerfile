FROM ghcr.io/void-linux/void-musl@sha256:43bf90b60f3164e2723e067773ad8f9487121b367c989357419dc339f952c731
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash clang clang-tools-extra cmake cppcheck git gzip jq ninja tar wget rust-sccache nodejs \
    && xbps-remove -Ooy
LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Void Linux Musl Clang"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-musl"
