FROM ghcr.io/void-linux/void-glibc@sha256:45532171dc49955e9a66b7c1803af5bc1f211fc76656177767a0bd5ecec55b83
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash cmake gcc git libsanitizer-devel make ninja rust-sccache tar gzip \
    && xbps-remove -Ooy
LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub GCC"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-glibc"
