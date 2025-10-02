FROM ghcr.io/void-linux/void-glibc@sha256:e85211c969df388cae2825722186848824eaa8df611ebf094b6a1c6128cad365
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash cmake gcc git libsanitizer-devel make ninja rust-sccache tar gzip \
    && xbps-remove -Ooy
LABEL org.opencontainers.image.source="https://github.com/hrzlgnm/Cappuchin"
LABEL org.opencontainers.image.title="Cappuchin GitHub GCC"
LABEL org.opencontainers.image.base.name="ghcr.io/void-linux/void-glibc"
