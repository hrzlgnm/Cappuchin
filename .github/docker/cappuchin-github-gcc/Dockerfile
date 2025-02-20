FROM ghcr.io/void-linux/void-glibc@sha256:804626633255f0a58a5433b313b9215576a8e028c76b64dd27007bcf271bb9f1
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash git cmake ninja gcc libsanitizer-devel make \
    && xbps-remove -Ooy


