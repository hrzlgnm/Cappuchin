FROM ghcr.io/void-linux/void-musl@sha256:2237f148f3c0c18b3914f29717c51b591fb9e9eb40c7a97c80894e4b50734c06
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash clang clang-tools-extra cmake cppcheck git gzip jq ninja tar wget rust-sccache nodejs \
    && xbps-remove -Ooy
LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Void Linux Musl Clang"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-musl"
