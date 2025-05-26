FROM ghcr.io/void-linux/void-musl@sha256:66dce8377795b612014f609b2b491e5da3c879b4b039efe23ee6badf8c80f888
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash clang clang-tools-extra cmake cppcheck git gzip jq ninja tar wget rust-sccache nodejs \
    && xbps-remove -Ooy
LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub Void Linux Musl Clang"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-musl"
